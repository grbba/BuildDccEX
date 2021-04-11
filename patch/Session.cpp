#if __has_include("config.h")
#include "config.h"
#else
#warning config.h not found. Using defaults from config.example.h
#include "config.example.h"
#endif
#include "defines.h"
#include "EthernetInterface.h"
#include "WifiInterface.h"
#include "Session.h"
#include "DCC.h"
#include "DCCEXParser.h"
#include "DIAG.h"
#include "Queue.h"

//FWD decl
void EthernetConfig();
void WiFiConfig();
void MotorShieldConfig();
void RMFTConfig();
void MySetupConfig();
void LCNConfig();

// The unique session object used
Session Session::singleton;

// to be added to the class
Queue<_cliCmd> *cmd = new Queue<_cliCmd>(CMDQUEUE);


void optionHandler( const char *option, const char *value){
    DIAG(F("Executing %s(%s)"), option, value);




}


void cliFilter(const byte *cmd)
{
    DIAG(F("CS recieved + cmd  %s, %d"), cmd, strlen( (char *)cmd));

    // the cmd string is of the following format "+cli:s:0" 
    // no spaces + and : allowed anywhere
    // :option:value:option:value: etc... until the end 
    // if we don't get cli after the + ( i.e. very first token) send on to the ATCommand filter

    char option[MAXOPTIONLEN];
    char value[MAXVALUELEN];

    char * pch;
    
    // tokenize the recieved string to extract the parameters
    pch = strtok ((char *)cmd,"+:[]");
    // DIAG(F("Header: %s\n"),pch);

    // check if first token is "cli"
    if (strcmp(pch, "cli") != 0 ) {
        // its not a cli command so send it to the ATCommand handler
        WifiInterface::ATCommand(cmd);
        return; // and we are done here
    }

    pch = strtok (NULL,"]");
    // DIAG(F("Payload Length: %s\n"),pch);

    byte pState = 0; // for Option 1 for Option value    
    pch = strtok(NULL, ":");  // pch points to the first Option
    while (pch != NULL) {
        
        // DIAG(F("%s: %s\n"), pState ? "Value" : "Option", pch);
        
        if ( pState == 0 ) {
            strcpy(option, pch);
            // save pch into the option var
            pState = 1;
        } else {
            // save the pch into the value var and call the executor
            strcpy(value, pch);
            DIAG(F("Adding + cmd to the queue and set CONFIG"));
            Session::get()->setState(CONFIG);
            // optionHandler(option, value);
            pState = 0;
        } 
        pch = strtok(NULL, ":");

    }

    // the cmd string is of the following format  "+cli :s:0" 
    // :option:value:option:value: etc... until the end 
    // if we don't get cli after the + send on to the ATCommand filter

}

void csSessionFilter(Print *stream, byte &opcode, byte &paramCount, int p[])
{
    (void)stream; // avoid compiler warning if we don't access this parameter
    switch (opcode)
    {
    case 'c':
    {
        if (paramCount <= 0 || p[0] != 2437)
        { // must be something else 2437 magic number for 'li'
            break;
        }
        // now we have got cli command to be handeled
        // push it into the buffer
        // for (int i = 1; i < paramCount; i++)
        // {
        //     DIAG(F("cli p %d %d"), i, p[i]);
        // }
        // push the command recieved into the queue
        // need to copy the parameters as i get a copy so when the call goes out of scope
        // i have the address but not the data
        // MAX_command params is set to 10

        // DIAG(F("push %c %d %x"), opcode, paramCount, p);

        _cliCmd x(opcode, paramCount);

        for (int i = 0; i < paramCount; i++)
        {
            x.p[i] = p[i];
        }
        cmd->push(x);                     // command recieved and stored
        Session::get()->setState(CONFIG); // set state to tell there is something to do

        // set opcode to 0 nothing more to be done for the parser;
        opcode = '\0';
    }
    default:
        break;
    }
}

void Session::setup()
{
    DIAG(F("Dcccli Session patch v0.1.12"))
    // filter for intercepting cli commands ('c' + 'li' in p[0])
    DCCEXParser::setFilter(csSessionFilter);
    // intercept the '+' and check if its for me
    DCCEXParser::setAtCommandCallback(cliFilter);
}

void Session::loop()
{
}

void config2(_cliCmd *c)
{
    switch (c->p[1])
    {
    case 's':  // configure MotorShield
    {
        // DIAG(F("Setting MotorShield to %d"), c->p[2]);
        switch(c->p[2]){
            case _ciSTANDARD_MOTOR_SHIELD: { DCC::begin(STANDARD_MOTOR_SHIELD);  break;}
            case _ciPOLOLU_MOTOR_SHIELD: { DCC::begin(POLOLU_MOTOR_SHIELD); break;}
            case _ciFIREBOX_MK1: { DCC::begin(FIREBOX_MK1); break;}
            case _ciFIREBOX_MK1S: { DCC::begin(FIREBOX_MK1S); break;}
            case _ciFUNDUMOTO_SHIELD: { DCC::begin(FUNDUMOTO_SHIELD); break;}
            case _ciIBT_2_WITH_ARDUINO: { DCC::begin(IBT_2_WITH_ARDUINO); break;}
            case _ciNOT_CONFIGURED: { DIAG(F("Error unknown MotorShield specified")); break;}
        }
        break;
    }
    case _ciEthernet:
    {
        break;
    }
    case _ciWiFi:
    {
        break;
    }
    case _ciLCN:
    {
        break;
    }
    case _ciLCD:
    {
        break;
    }
    case _ciLayout:
    {
        break;
    }
    }
}

// runtime config of all relevant items
// This is called from the main loop if the isConfigured Flag is set to CONFIG
void Session::config()
{
    // look into the cmd buffer to find out what has to be configured
    // plus its parameters - issue for things like text to transfer across the parser
    // e.g. Wifi passwords

    // work through the command queue got from the CLI.
    // MINIMAL Config is motorshield

    // WiFiConfig();
    // EthernetConfig();
    // MotorShieldConfig();
    // hasMotorShield = true;
    // RMFTConfig();
    // MySetupConfig();
    // LCNConfig();

    // config cmds may be waiting from the cli
    // DIAG(F("Configuring something... "));
    for (int i = 0; i < cmd->count(); i++)
    {
        auto f = cmd->pop();
        // DIAG(F("pop %c %d %x"), f.opcode, f.paramCount, f.p);
        if (f.paramCount < 2)
        {
            DIAG(F("Missing parameters in cli command"));
        }
        else
        {
            // DIAG(F("Applying config ... "));
            config2(&f);
        }
    }
    this->csState = CONFIGURED;
    LCD(1, F("Configuration done"));
}

void EthernetConfig()
{
#if ETHERNET_ON
    EthernetInterface::setup();
#endif // ETHERNET_ON
}

void WiFiConfig()
{
#if WIFI_ON
    // This will reset the AtCommandCallback !
    WifiInterface::setup(WIFI_SERIAL_LINK_SPEED, F(WIFI_SSID), F(WIFI_PASSWORD), F(WIFI_HOSTNAME), IP_PORT, WIFI_CHANNEL);
    // Set the AtCommandCallback to the session handler which will forward AT commands as bnneded to WifiInterfaces::AtCommand()
    // which is the callback used in WifiInterface.cpp
    DCCEXParser::setAtCommandCallback(cliFilter);

#endif // WIFI_ON
}

// Start the DCC engine ...
// void MotorShieldConfig()
// {
//     _ciMotorDriver *d = new _ciMotorDriver(MOTOR_SHIELD_TYPE);
//     DCC::begin(d->name, d->main, d->prog);
// }

void RMFTConfig()
{
#if defined(RMFT_ACTIVE)
    RMFT::begin();
#endif
}

void MySetupConfig()
{
#if __has_include("mySetup.h")
#define SETUP(cmd) serialParser.parse(F(cmd))
#include "mySetup.h"
#undef SETUP
#endif
}

void LCNConfig()
{
#if defined(LCN_SERIAL)
    LCN_SERIAL.begin(115200);
    LCN::init(LCN_SERIAL);
#endif
}
