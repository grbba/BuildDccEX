
#ifndef Session_h
#define Session_h

#include "DCCEXParser.h"
#include "MotorDriver.h"
#include "MotorDrivers.h"
#include "Queue.h"

/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))        // '!!' to make sure this returns 0 or 1

/* x=target variable, y=mask */
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK_ALL(x,y) (((x) & (y)) == (y))   // warning: evaluates y twice
#define BITMASK_CHECK_ANY(x,y) ((x) & (y))

#define CMDQUEUE 20

#define MAXOPTIONLEN 10
#define MAXVALUELEN 30

// to be aligned alligned to DccConfig.hpp in the cli
// can't use without the _ci prefix as the names are taken by #defines
// for the indvidual motorshield types

enum CsMotorShield
{
    _ciSTANDARD_MOTOR_SHIELD,
    _ciPOLOLU_MOTOR_SHIELD,
    _ciFIREBOX_MK1,
    _ciFIREBOX_MK1S,
    _ciFUNDUMOTO_SHIELD,
    _ciIBT_2_WITH_ARDUINO,
    _ciNOT_CONFIGURED
};

// configuration item identifier
// from <? command>
enum ConfigurationItems {
    _ciMotorShield = -1474,
    _ciEthernet = -27717,    
    _ciWiFi = -11621,       
    _ciLCN = 24651,       
    _ciLCD = 24641,      
    _ciLayout = -28112
};

struct MotorShield { 
    const FSH *name;
    MotorDriver *main;
    MotorDriver *prog;
    MotorShield( const FSH *n, MotorDriver *m, MotorDriver *p): name(n), main(m), prog(p) {};
};


struct _cliCmd {
    byte opcode;
    byte paramCount;
    int p[DCCEXParser::MAX_COMMAND_PARAMS];
    _cliCmd() {};
    _cliCmd(byte o, byte c): opcode(o), paramCount(c) {};
};

enum SessionState {
    INIT,           // Nothing configured
    CONFIG,         // something has to be configured 
    CONFIGURED      // Nothing to configure ( INIT -> CONFIGURED is only possible if a MotorShield has been configured )
};

class Session 
{
private:
    
    static Session singleton;
    Session() = default;
    Session(const Session&); // non construction-copyable
    Session& operator=( const Session& ); // non copyable

    SessionState csState = INIT;
    uint64_t csConfig = 0;        // each bit designates if a config item is on/off

    bool hasEthernet = false;
    bool hasWiFi = false;
    bool hasLCD = false;
    bool hasMotorShield = false;

public:

    static Session *get() noexcept {
        return &singleton;
    }

    bool isConfigured()             { return csState == CONFIGURED; };
    void setState(SessionState s)   { csState = s; };

    void setup();       // called at setup in the main ino file
    void config();      // caled in the loop in case state is congi
    void loop();

    ~Session() = default;
};

#endif