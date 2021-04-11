////////////////////////////////////////////////////////////////////////////////////
//  DCC-EX CommandStation-EX   Please see https://DCC-EX.com 
//
// This file is the main sketch for the Command Station.
// 
// CONFIGURATION: 
// Configuration is normally performed by editing a file called config.h.
// This file is NOT shipped with the code so that if you pull a later version
// of the code, your configuration will not be overwritten.
//
// If you used the automatic installer program, config.h will have been created automatically.
// 
// To obtain a starting copy of config.h please copy the file config.example.h which is 
// shipped with the code and may be updated as new features are added. 
// 
// If config.h is not found, config.example.h will be used with all defaults.
////////////////////////////////////////////////////////////////////////////////////

#if __has_include ( "config.h")
  #include "config.h"
#else
  #warning config.h not found. Using defaults from config.example.h 
  #include "config.example.h"
#endif


/*
 *  © 2020,2021 Chris Harlow, Harald Barth, David Cutting, 
 *  Fred Decker, Gregor Baues, Anthony W - Dayton  All rights reserved.
 * 
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.
 */


#include "DCCEX.h"


// Create a serial command parser for the USB connection, 
// This supports JMRI or manual diagnostics and commands
// to be issued from the USB serial console.
DCCEXParser serialParser;

// a singleton session object containing/managing all the config info is reated and used 
// Controls the runstate of the commandstation
// inital setup is minimal and limited to setup the serial com; only
// way to bootstrap the system and get an inital path into the CS
// in the setup just set the filter function
// This will allow to communicate with the commandline interface for setup : recieving
// layout information etc.


void setup()
{
  // The main sketch has responsibilities during setup()

  // Responsibility 1: Start the usb connection for diagnostics
  // This is normally Serial but uses SerialUSB on a SAMD processor
  Serial.begin(115200);
  CONDITIONAL_LCD_START
  {
      // This block is still executed for DIAGS if LCD not in use
      LCD(0, F("DCC++ EX v%S"), F(VERSION));
      LCD(1, F("Starting"));
  }

  Session::get()->setup();

}

void loop()
{

  if(!Session::get()->isConfigured()) { // if not CONFIGURED go to the config function as there
                                        // maybe some config cmds waiting to be executed
    Session::get()->config();           // run what has been in setup() before here ... 
  } 
  
  // The main sketch has responsibilities during loop()

  // Responsibility 1: Handle DCC background processes
  //                   (loco reminders and power checks)
  DCC::loop();

  // Responsibility 2: handle any incoming commands on USB connection
  serialParser.loop(Serial);

// Responsibility 3: Optionally handle any incoming WiFi traffic
#if WIFI_ON
  WifiInterface::loop();
#endif
#if ETHERNET_ON
  EthernetInterface::loop();
#endif

#if defined(RMFT_ACTIVE) 
  RMFT::loop();
#endif

  #if defined(LCN_SERIAL) 
      LCN::loop();
  #endif

  // LCDDisplay::loop();  // ignored if LCD not in use 
  
  // Report any decrease in memory (will automatically trigger on first call)
  static int ramLowWatermark = __INT_MAX__; // replaced on first loop 

  int freeNow = minimumFreeMemory();
  if (freeNow < ramLowWatermark)
  {
    ramLowWatermark = freeNow;
    // LCD(2,F("Free RAM=%5db"), ramLowWatermark);
  }
}
