/*  
    Low-level code that turns lights on, decides 
    which to turn on. 
*/

/* Uses current time and speed to calculate which lights
   should be on to produce an arc. */
byte arc(){
    const unsigned long sinceLastSwitch = millis() - lastSwitchActivation;
//    const unsigned long sinceLastSwitch = 50 - lastSwitchActivation;
      
    /* Current angle the bike is at with respect to the magnet */
    const short currentWheelAngle = 360 * (sinceLastSwitch)/(lastPeriod);
    
    /* Arc start angle with respect to the reed switch. 
       same calculation as currentWheelAngle with an offset. 
       360 * (sinceLastSwitch)/(lastPeriod) */
    short arcStartAngle;
    short arcEndAngle;
    if(BACK_WHEEL){
        arcStartAngle = BACK_ARC_START_ANGLE - currentWheelAngle;
        arcEndAngle = BACK_ARC_END_ANGLE - currentWheelAngle;
    } 
    else {
        arcStartAngle = FRONT_ARC_START_ANGLE - currentWheelAngle;
        arcEndAngle = FRONT_ARC_END_ANGLE - currentWheelAngle;
//        arcStartAngle = -FRONT_ARC_END_ANGLE - currentWheelAngle;
//        arcEndAngle = -FRONT_ARC_START_ANGLE - currentWheelAngle;
    }
    /* Handle negatives */
    while(arcStartAngle < 0 || arcEndAngle < 0){
        arcStartAngle += 360;
        arcEndAngle += 360;
    }
//    if(VERBOSE == 1){
//        Serial << arcStartAngle << ", " << arcEndAngle << "," << arcEndAngle - arcStartAngle << endl;
//    }
    const char arcStartLight = arcStartAngle/DEGREES_PER_LED;
    const char arcEndLight = arcEndAngle/DEGREES_PER_LED;
//    if(VERBOSE == 1){
//        Serial <<  _DEC(arcStartLight) << ", " << _DEC(arcEndLight) << endl << endl;
//    }
    /* Prepare final output variable */
    byte lights = 0;
    
    for(byte b = arcStartLight; b <= arcEndLight; b++){
        lights |= (1 << (b % NUM_LIGHTS));
    }
    return lights;
}

/* Each bit = one light. */
void turnOnOffLights(byte lights) {turnOnOffLights(lights, 255);}
void turnOnOffLights(byte lights, byte dutyCycle){
    for(byte b = 0; b < NUM_LIGHTS; b++){
//        digitalWrite(LEDs[b].pin, lights & (1 << b));

        if(lights & (1 << b)){
            analogWrite(LEDs[b].pin, dutyCycle);
        }
        else{
            digitalWrite(LEDs[b].pin, LOW);
        }
    }
//    digitalWrite(LEDs[0].pin, lights & (1 << 0));
//    digitalWrite(LEDs[1].pin, lights & (1 << 1));
//    digitalWrite(LEDs[2].pin, lights & (1 << 2));
//    digitalWrite(LEDs[3].pin, lights & (1 << 3));
//    digitalWrite(LEDs[4].pin, lights & (1 << 4));
//    digitalWrite(LEDs[5].pin, lights & (1 << 5));
}
