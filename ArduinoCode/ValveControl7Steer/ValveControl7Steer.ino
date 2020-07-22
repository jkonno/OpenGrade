  /*
  * AOG Section Control
  */

//-------------------------------------------------------------------------
//to change the deadband go a the end a change
//the value in the cutvale digital.write
//the original code is BlinkTheRelay from BrianTee AGopenGPS
//-------------------------------------------------------------------------


     
  //loop time variables in milliseconds  
  const byte LOOP_TIME = 200; //5hz
  unsigned long lastTime = LOOP_TIME;
  unsigned long currentTime = LOOP_TIME;

  //Comm checks
  byte watchdogTimer = 0;      //make sure we are talking to AOG
  byte serialResetTimer = 0;   //if serial buffer is getting full, empty it
  
   //Communication with AgOpenGPS
  bool isDataFound = false, isSettingFound = false;
  int header = 0, tempHeader = 0, temp;

  //The variables used for storage
  byte relayHi=0, relayLo = 0, rdHeaderLo = 250, gpsSpeed = 30, rdSpeedXFour = 40, tree = 30, uTurn = 30, hydLift = 30, cutValve = 100, rdSectionControlByte = 10, rdRateSetPointHi = 20, rdRateSetPointLo = 30, rdYouTurnControlByte = 50; 

  #define R1 4
  #define R2 5
 
  void setup()
  {
    //set the baud rate
    Serial.begin(38400);  
  
    //set pins to output
    pinMode(R1, OUTPUT);
    pinMode(R2, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
  }

  void loop()
  {
    //Loop triggers every 200 msec and sends back gyro heading, and roll, steer angle etc
  
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
      
       Serial.print(rdHeaderLo); //steering switch status
      Serial.print(",");
      Serial.print(rdSectionControlByte); //steering switch status
      Serial.print(",");
      Serial.print(rdRateSetPointHi); //steering switch status
      Serial.print(",");
      Serial.print(rdRateSetPointLo); //steering switch status
      Serial.print(",");
      Serial.print(rdSpeedXFour); //steering switch status
      Serial.print(",");
      Serial.print(rdYouTurnControlByte); //steering switch status
      Serial.print(",");
      Serial.println(cutValve); //steering switch status
         
      Serial.flush();   // flush out buffer
     
    } //end of timed loop
  
    //****************************************************************************************
    //This runs continuously, outside of the timed loop, keeps checking UART for new data
          // PGN - 32762 - 127.250 0x7FFA
          //public int mdHeaderHi, mdHeaderLo = 1, mdSectionControlByteHi = 2, mdSectionControlByteLo = 3,
          //mdSpeedXFour = 4, mdUTurn = 5, mdTree = 6, mdHydLift = 7, md8 = 8, md9 = 9;
    if (Serial.available() > 0 && !isDataFound) //find the header, 127H + 250L = 31750
    {
      int temp = Serial.read();
      header = tempHeader << 8 | temp;                //high,low bytes to make int
      tempHeader = temp;                              //save for next time
      if (header == 32766) isDataFound = true;        //Do we have a match? etait 31750
    }
  
    //Data Header has been found, so the next 6 bytes are the data
    if (Serial.available() > 6 && isDataFound)
    {
      isDataFound = false;
      rdHeaderLo = Serial.read();
      rdSectionControlByte = Serial.read();
      rdRateSetPointHi = Serial.read();          // read relay control from AgOpenGPS
      rdRateSetPointLo = Serial.read();    //actual speed times 4, single byte
      rdSpeedXFour = Serial.read();  
      rdYouTurnControlByte = Serial.read();
      //hydLift = Serial.read();
      
      //just get the rest of bytes
      cutValve = Serial.read();   //high,low bytes   
      //Serial.read();  
  
      //reset watchdog
      watchdogTimer = 0;
  
      //Reset serial Watchdog  
      serialResetTimer = 0;
    }
    
  }

  void SetRelays(void)
  {
   if (cutValve < 95) digitalWrite(LED_BUILTIN, HIGH);
    else digitalWrite(LED_BUILTIN, LOW);
    if (cutValve > 105) digitalWrite(R2, LOW);
    else digitalWrite(R2, HIGH);  
    
  }
