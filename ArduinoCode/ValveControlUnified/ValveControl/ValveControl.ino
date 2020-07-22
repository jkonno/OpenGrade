  /*
  * AOG Section Control
  */

//-------------------------------------------------------------------------
//to change the deadband go a the end a change
//the value in the cutvale digital.write
//the original code is BlinkTheRelay from BrianTee AGopenGPS
//-------------------------------------------------------------------------

//Output in ON/OFF MODE: BLADE_UP,BLADE_DOWN,CUTVALUE
//Output in PROPO MODE: BLADE_DIR,PWM_VALUE,CUTVALUE

     
  //loop time variables in milliseconds  
  const byte LOOP_TIME = 100; //10hz
  unsigned long lastTime = LOOP_TIME;
  unsigned long currentTime = LOOP_TIME;

  //Comm checks
  byte watchdogTimer = 0;      //make sure we are talking to AOG
  byte serialResetTimer = 0;   //if serial buffer is getting full, empty it

  //PWM or relay mode
  bool proportionalValve = false;
  //Deadband setting
  int deadband = 5;
  
   //Communication with AgOpenGPS
  bool isDataFound = false, isSettingFound = false;
  int header = 0, tempHeader = 0, temp;

  //The variables used for storage
  byte relayHi=0, relayLo = 0, cutValve = 100;

  //pwm variables
  byte pwmDrive = 0, pwmDisplay = 0, pwmGainUp = 5, pwmMinUp = 50, pwmGainDw = 5, pwmMinDw = 50, pwmMax = 255;
  float pwmValue = 0;


  #define DIR_ENABLE 4 //PD4
  #define PWM_OUT 3 //PD3

  #define R1 4
  #define R2 5
 
  void setup()
  {
    if (proportionalValve){
      TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 to 256 for PWM frequency of   122.55 Hz
      TCCR1B = TCCR1B & B11111000 | B00000100;    // set timer 1 to 256 for PWM frequency of   122.55 Hz 
      
      
      //set the baud rate
      Serial.begin(38400);  
    
      //set pins to output
      pinMode(DIR_ENABLE, OUTPUT);
      pinMode(LED_BUILTIN, OUTPUT);
      
    } else {
      //set the baud rate
      Serial.begin(38400);  
  
      //set pins to output
      pinMode(R1, OUTPUT);
      pinMode(R2, OUTPUT);
      pinMode(LED_BUILTIN, OUTPUT);
    }
 
  }

  void loop()
  {
    //Loop triggers every 100 msec and sends back gyro heading, and roll, steer angle etc
  
    currentTime = millis();
    unsigned int time = currentTime;
  
    if (currentTime - lastTime >= LOOP_TIME)
    {
      lastTime = currentTime;
  
      //If connection lost to AgOpenGPS, the watchdog will count up 
      if (watchdogTimer++ > 250) watchdogTimer = 12;
  
      //clean out serial buffer to prevent buffer overflow
      if (serialResetTimer++ > 20)
      {
        while (Serial.available() > 0) char t = Serial.read();
        serialResetTimer = 0;
      }
  
      //safety - turn off if confused
      if (watchdogTimer > 10) relayLo = 0;
      
      //section relays
      SetRelays();
      
      Serial.println(cutValve);
         
      Serial.flush();   // flush out buffer
     
    } //end of timed loop
  
    //****************************************************************************************
    //This runs continuously, outside of the timed loop, keeps checking UART for new data
          // PGN - 32762 - 127.250 0x7FFA
          //public int mdHeaderHi, mdHeaderLo = 1, cutValve = 2
    if (Serial.available() > 0 && !isDataFound)
    {
      int temp = Serial.read();
      header = tempHeader << 8 | temp;                //high,low bytes to make int
      tempHeader = temp;                              //save for next time
      if (header == 32762) isDataFound = true;        //Do we have a match?
    }
  
    //Data Header has been found, so the next 6 bytes are the data
    if (Serial.available() > 0 && isDataFound)
    {
      isDataFound = false;
      cutValve = Serial.read();
      
      //reset watchdog
      watchdogTimer = 0;
  
      //Reset serial Watchdog  
      serialResetTimer = 0;
    }
    
  }

  void SetRelays(void)
  {
    if (proportionalValve){
      if (cutValve > 102) {
        digitalWrite(DIR_ENABLE, HIGH);
        Serial.print("1,");
      }
      else{
        digitalWrite(DIR_ENABLE, LOW); 
        Serial.print("0,");
      }
  
      pwmValue = 0;
  
      if (cutValve > (100 + deadband))
      {
        pwmValue = ((cutValve-100-deadband)*pwmGainDw + pwmMinDw);
      }
      if  (cutValve < (100 - deadband))
      {
        pwmValue = -((cutValve-100-deadband)*pwmGainUp - pwmMinUp);
      }
      if (pwmValue > pwmMax)
      {
        pwmValue = pwmMax;  
      }
      pwmDrive = pwmValue;
      analogWrite(PWM_OUT, pwmDrive);
      if (pwmValue > 0) digitalWrite(LED_BUILTIN, HIGH);
      else digitalWrite(LED_BUILTIN, LOW);
      Serial.print(String((int)pwmValue)+",");
    } else {
      //Blade position vs deadband
      if (cutValve < (100-deadband)){
        digitalWrite(R1, HIGH);
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.print("1,0,");
      } else if (cutValve > (100+deadband)) {
        digitalWrite(R2, HIGH);
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.print("0,1,");
      } else {
        digitalWrite(R1, LOW);
        digitalWrite(R2, LOW);
        digitalWrite(LED_BUILTIN, LOW);
        Serial.print("0,0,");
      }
    }
  }