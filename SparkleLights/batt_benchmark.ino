/* 
    Measures how long the battery lasts with 180° arc 
    Start program with full battery. Will output every five minutes until 
    certain analog reading taken. 
*/

void batt_benchmark(){
    delay(1000);
    turnOnOffLights(0x07); /* Turn on three lights */
    
    unsigned long lastOutputTime = 0;  
  
    while(1){
        if(millis() - lastOutputTime > 30000){
            Serial << millis() / 60000 << "," << batteryVoltage() << endl;
            lastOutputTime = millis();
        }
      
        if(batteryVoltageLow()){
           Serial << "batt low" << endl;
           /* Halt program */
           while(1){
               turnOnOffLights(0x00);
               Serial.flush();
               sleepNow();
           }
       } 
    }
}
