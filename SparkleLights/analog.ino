
/* Voltage divider connected here to measure battery voltage  */
const byte BATT_MEASURE = A2;
const byte BATT_MEASURE_GND = A3; /* Turn on OUTPUT,LOW when reading */
/* Voltage divider connected here for photoresistor  */
const byte PHOTORESIST_MEAS = A0;
const byte PHOTORESIST_GND = A1; /* Turn on OUTPUT,LOW when reading */

/*  Going below this reading will cause the program to go into
    a low-power state. Uses internal 1.1V reference voltage. 
    (v/1.1)(10/150)*1024 = 595 where V = 9.6 or 3.2V/li-po cell.
    Even with all the lights at 100%, this reading only changes by 5.
    (using a 500 mAh 20C discharge li-po) 
    Emperically, 9.6V was measured to be 574. */
const short MINIMUM_BATTERY_VOLTAGE = 574;

/* After changing the analog reference, the first few readings 
       from analogRead() may not be accurate.  */
const byte AFTER_SWITCHING_ANALOG_REF_READ_THIS_NUMBER_OF_TIMES = 40;

/* Light level where it is considered "dark" */
const short DARKNESS = 500;

boolean batteryVoltageLow(){
    return batteryVoltage() < MINIMUM_BATTERY_VOLTAGE;
}
short batteryVoltage(){
    /* Turn low voltage on */
    pinMode(BATT_MEASURE_GND, OUTPUT);
    
    /* Change internal voltage reference */
    analogReference(INTERNAL);
//    delay(10);
    
    /* After changing the analog reference, the first few readings 
       from analogRead() may not be accurate.  */
    for(byte i = 0; i < AFTER_SWITCHING_ANALOG_REF_READ_THIS_NUMBER_OF_TIMES; i++){
        analogRead(BATT_MEASURE);
    }
    short reading = analogRead(BATT_MEASURE);
    
//    if(VERBOSE){
//        Serial << "batt: " << reading << endl;
//    }
    
    /* Turn low voltage off */
    pinMode(BATT_MEASURE_GND, INPUT);
    
    return reading;
}
boolean dark(){
    return lightLevel() < DARKNESS;
}
short lightLevel(){
    /* Turn low voltage on */
    pinMode(PHOTORESIST_GND, OUTPUT);
    
    /* Change internal voltage reference */
    analogReference(DEFAULT);
    
    /* After changing the analog reference, the first few readings 
       from analogRead() may not be accurate.  */
    for(byte i = 0; i < AFTER_SWITCHING_ANALOG_REF_READ_THIS_NUMBER_OF_TIMES; i++){
        analogRead(PHOTORESIST_MEAS);
    }
    short reading = analogRead(PHOTORESIST_MEAS);
    
    /* Turn low voltage off */
    pinMode(PHOTORESIST_GND, INPUT);
    
    return reading;
    
}
