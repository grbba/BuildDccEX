/*
    © 2020, Harald Barth.

    This file is part of CommandStation-EX

    This is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    It is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CommandStation.  If not, see <https://www.gnu.org/licenses/>.

*/

////////////////////////////////////////////////////////////////////////////////
//
// WIFI_ON: All prereqs for running with WIFI are met
// Note: WIFI_CHANNEL may not exist in early config.h files so is added here if needed.

// ! ARDUINO_ARCH_SAMD has been removed form here so by default NO Wifi/Ethernet for SAMD

// For the CLI patch ENABLE_WIFI and ENABLE_ETHERNET is always true no apply the decision matrix for setting up or not

#if defined(ARDUINO_AVR_UNO)  // ! all false it doesn't fit
    #define WIFI_ON false
    #define ETHERNET_ON false
#endif

#if defined(ARDUINO_AVR_UNO_WIFI_REV2) // Nothing for the R2 yet we don't know anything about this #not supported
    #define WIFI_ON false
    #define ETHERNET_ON false
#endif

#if defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560)  // enable both interfaces 
    #define WIFI_ON true
    #define ETHERNET_ON true
#endif

#if defined(ARDUINO_AVR_NANO_EVERY)  || defined(ARDUINO_AVR_NANO)  // ! all false it doesn't fit yet
    #define WIFI_ON false
    #define ETHERNET_ON false
#endif

#if defined(WIFI_ON)
    #ifndef WIFI_CHANNEL
        #define WIFI_CHANNEL 1
    #endif
#endif

// I allow both for all architectures but only one can be configured. This has to be controlled in the session
// handler as well as the 'enforecmlent' otherwise is just by obfuscation of the command send - which could be 
// reproduced by a user over the serial to get both in // 
// add a feature disbling this limitation at the users own risk ? 

// > grbba :: get me both interfaces on a MEGA but nowhere else 
#if WIFI_ON && ETHERNET_ON && !ARDUINO_AVR_MEGA2560
  #error Command Station does not support WIFI and ETHERNET at the same time.
#endif
// grbba >

////////////////////////////////////////////////////////////////////////////////
//
// This defines the speed at which the Arduino will communicate with the ESP8266 module.
// Currently only devices which can communicate at 115200 are supported.
//
#define WIFI_SERIAL_LINK_SPEED 115200
