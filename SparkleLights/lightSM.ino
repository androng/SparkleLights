/* 
    High-level behavior that switches between modes 
*/


void lightSM(){
    unsigned long sinceLastState = millis() - lastStateChange;
    
    /* Actions */
    switch(currentLightState){
    case state0FirstPowerOn:{
        /* Turn on first and last light--
           the reed switch should be between these two. */
        const byte LOW_DUTY_CYCLE = 16;
        analogWrite(LEDs[0].pin, LOW_DUTY_CYCLE);
        analogWrite(LEDs[NUM_LIGHTS - 1].pin, LOW_DUTY_CYCLE);
    }
        break;
    case state1SleepMode:
        ;
        break;
    case state2Moving:
        turnOnOffLights(arc());
        break;
    case state3StoppedMoving:{
    /* If first two seconds */
    if(sinceLastState < 2048){
        byte lights = arc();
        
        /* Draw fading arc */
        turnOnOffLights(lights,  255 - sinceLastState/8);
    }
    /* Wait for a bit before animation */
    else if (sinceLastState < 3000){
        turnOnOffLights(0);
    }
    else{
    /* Twinkle animation */
        /* Iterate through each light */
        for (byte i = 0; i < NUM_LIGHTS; i++) {
            /* If LED is on, keep fading up/down */
            if (LEDs[i].currentDutyCycle != 0) {
                if (LEDs[i].increasing) {
                    if (LEDs[i].currentDutyCycle + LEDs[i].incrementBy < LEDs[i].maxDutyCycle) {
                        LEDs[i].currentDutyCycle += LEDs[i].incrementBy;
                    }
                    else {
                        LEDs[i].increasing = 0;
                    }
                }
                else {
                    /* Check for underflow */
                    byte b = LEDs[i].currentDutyCycle - LEDs[i].incrementBy;
                    if (b < LEDs[i].currentDutyCycle) {
                        LEDs[i].currentDutyCycle = b;
                    }
                    else {
                        LEDs[i].currentDutyCycle = 0;
                    }
                }
            }
            /* If LED is not on, turn it on with dice roll. */
            else {
                /* Stop turning on LEDs five seconds before max time.  */
                if (random() % 30 == 0 && sinceLastState < (MAX_STATE3STOPPEDMOVING_DURATION - 5000)) {
                    LEDs[i].increasing = 1;
                    LEDs[i].incrementBy = (random() % 2) + 1;
                    LEDs[i].currentDutyCycle = LEDs[i].incrementBy;
                    LEDs[i].maxDutyCycle = 30 + (random() % 30);
                }
            }
            
        }
        
        /* Output to LEDs */
        for(byte b = 0; b < NUM_LIGHTS; b++){
            analogWrite(LEDs[b].pin, LEDs[b].currentDutyCycle);
        }
    }
    }
        break;
    default:{
        Serial << F("Undefined state ") << currentLightState << endl;
    }
    }
    
    /* Transitions */
    switch(currentLightState){
    case state0FirstPowerOn:
        /* Speed > 2mph? */
        if(highSpeed()){
             changeLightState(state2Moving);
        }
        break;
    case state1SleepMode:
        if(highSpeed() && dark()){
            changeLightState(state2Moving);
        }
        break;
    case state2Moving:
        if(speedGreaterThan2mph() == false){
            changeLightState(state3StoppedMoving);
        }
        break;
    case state3StoppedMoving:
        if(speedGreaterThan2mph()){
            changeLightState(state2Moving);
        }
        else{
            if(sinceLastState > MAX_STATE3STOPPEDMOVING_DURATION){
                changeLightState(state1SleepMode);
                /* All lights off */
                turnOnOffLights(0);
            }
        }
        break;
    }
}

void changeLightState(int s){
    currentLightState = s;
    lastStateChange = millis();
}

