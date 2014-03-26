/*  
    Revolights clone 
    with six parallel LEDs
    
 */

#include <TimedAction.h>
#include <Streaming.h>
#include <avr/sleep.h>
#include <avr/power.h>

const byte VERBOSE = 1;

/* 1 = back wheel, 0 = front wheel */
const byte BACK_WHEEL = 1;

/* The light arc animation speeds is a fraction of this number.
   This is calculated from a 26" wheel */
const unsigned short PERIOD_AT_1MPH = 4640;
const unsigned short PERIOD_AT_2MPH = PERIOD_AT_1MPH/2;
/* 5mph is just above walking speed */
const unsigned short PERIOD_AT_HIGH_SPEED = PERIOD_AT_1MPH/7;
/*  Maximum speed 50mph = 93ms on a 26" wheel. 
    Switch actuations that come more frequently will be ignored.  */
const unsigned short MINIMUM_PERIOD_BETWEEN_SWITCHING = 93;

/* Maximum animation length time */
const unsigned long MAX_STATE3STOPPEDMOVING_DURATION = 32000;

/*  The reed switch is angle zero and the lights are ordered in the 
    direction of a forward moving wheel. */
const byte NUM_LIGHTS = 6;
const short DEGREES_PER_LED = 360/NUM_LIGHTS;

/*  Lights are ordered in the direction of a forward moving wheel.
    Mount front magnet to fork. Mount back magnet to lower, 
    horizontal part. 
    The angles are inclusive. So if you have five lights and end angle
    is 180, the third light will always turn on because 180 takes 2.5 
    lights. */
const short FRONT_ARC_START_ANGLE = 111; /* in degrees */
const short FRONT_ARC_END_ANGLE = 150; 
const short BACK_ARC_START_ANGLE = 130;
const short BACK_ARC_END_ANGLE = 169;

volatile boolean reedSwitchLevelFell = 0;
unsigned long lastSwitchActivation = 0;
/*  The difference between the last and second-to-last switch.
    In other words, speed. */
unsigned long lastPeriod = 2320; 
unsigned long secondLastPeriod = 2320; 

TimedAction taskMaintainArc = TimedAction(30, maintainArc);
TimedAction taskLightSM = TimedAction(30, lightSM);
/* The 4 is so that lightSM and speedGreaterThan5mph() can run for enough
    periods before the millis clock stops. */
TimedAction taskCheckForSleep = TimedAction(PERIOD_AT_2MPH * 4 + 50, checkForSleep);

typedef struct {
    byte pin;
    byte increasing;
    byte maxDutyCycle;
    byte currentDutyCycle;
    byte incrementBy;
} LED;

LED LEDs[NUM_LIGHTS] = {
    {11, 1, 255, 0, 2},
    {10, 1, 255, 0, 2},
    {9, 1, 255, 0, 2},
    {6, 1, 255, 0, 2},
    {5, 1, 255, 0, 2},
    {3, 0, 255, 0, 2} 
};

unsigned long lastStateChange = 0;

enum lightState {
    state0FirstPowerOn,
    state1SleepMode,
    state2Moving,
    state3StoppedMoving
};

int currentLightState = state0FirstPowerOn;

void setup(){
    Serial.begin(115200);
    attachInterrupt(0, reedSwitchFalling, FALLING);
    
    for(byte b = 0; b < NUM_LIGHTS; b++){
        pinMode(LEDs[b].pin, OUTPUT);
    }
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

//    pinMode(13, OUTPUT);
//    batt_benchmark(); /* Will halt program */
}

void loop(){
    /* ISR for reed switch */
    if(reedSwitchLevelFell){
        unsigned long currentTime = millis();
        /* "Low pass filter" */
        if(currentTime - lastSwitchActivation > MINIMUM_PERIOD_BETWEEN_SWITCHING){
            /* Update the times */
            secondLastPeriod = lastPeriod;
            lastPeriod = currentTime - lastSwitchActivation;
            lastSwitchActivation = currentTime;
        }
        reedSwitchLevelFell = 0;
    }
    
//    taskMaintainArc.check();

//    if(VERBOSE){
//        Serial << "batteryVoltage " << batteryVoltage() << endl;
//        Serial << "lightLevel " << lightLevel() << endl;
//        Serial << "speedGreaterThan: " << speedGreaterThan5mph() << endl; 
//        
//        Serial << currentLightState << " " << millis() << endl;
//    }

//    if(batteryVoltageLow() && currentLightState != state0FirstPowerOn){
    if(batteryVoltageLow()){
        if(VERBOSE){
            Serial << "batt low" << endl;
        }      
        /* Flash lights and go back to sleep */
        changeLightState(state1SleepMode);
        
        turnOnOffLights(0xFF, 32); /* The number 32 is arbitrary */
        delay(4); /* Delay so that you can see the lights */
        turnOnOffLights(0);
        
        Serial.flush();
        sleepNow();
    }
    else{  
        taskLightSM.check();
    }
    
    taskCheckForSleep.check();
    
}
void checkForSleep(){
    if(currentLightState == state1SleepMode){
        Serial.flush();
        
        sleepNow();
    }
}
void sleepNow(){
    /* Power off EVERYTHING */
    ADCSRA &= ~(1 << 7); /* Disable ADC before shut down*/
    PRR = 0xFF;
    
    sleep_enable();
    sei();
    sleep_cpu();
    
    sleep_disable();
    
    /* Power on EVERYTHING */
    PRR = 0;
    ADCSRA |= (1 << 7); 
    
}
/* ISR for reed switch */
void reedSwitchFalling(){
    reedSwitchLevelFell = 1;
}
void maintainArc(){
//    turnOnOffLights(1 << (millis() / 1000 % 6));
    turnOnOffLights(arc());
}
boolean speedGreaterThan2mph(){
    return (millis() - lastSwitchActivation) < PERIOD_AT_2MPH 
    && lastPeriod < PERIOD_AT_2MPH;
}
/**/
boolean highSpeed(){
    return (millis() - lastSwitchActivation) < PERIOD_AT_HIGH_SPEED 
    && lastPeriod < PERIOD_AT_HIGH_SPEED;
}
