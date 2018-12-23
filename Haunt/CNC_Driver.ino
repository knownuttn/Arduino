
long randomX = 0;
long randomY = 0;
  
void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() 
{
  
  while (Serial.available() == 0) 
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(10);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(15); 
  }
 
  String mystring = getline();
  
  if (mystring == "Grbl 0.8c ['$' for help]" ) 
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(10000);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(10000);  
  }

  while (true) 
  {
    
      randomX = GetNextRandom( randomX,400 );
      randomY = GetNextRandom( randomY,200 );

      // Send GCODE to GRBL
      Serial.print ("g1 x");
      Serial.print (randomX);

      Serial.print (" y");
      Serial.print (randomY);

      Serial.print ('\r');

      // Pause until GRBL responds
      while (Serial.available() == 0) 
      {
        ;
      }
      // get what GRBL sent back
      mystring = getline();
    
  }
}

long GetNextRandom ( long oldNumber, long maxValue )
{
  long newRandomNum = random(maxValue);

  while (abs(newRandomNum - oldNumber) <= 50) 
  {
    newRandomNum = random(maxValue);
  }
  return newRandomNum;
}



String getline()
{
  String rslt = "";
  delay(100);
    
  while (Serial.available() > 0 ) 
  {
    int mychar = Serial.read();

    if (mychar == 10 || mychar == 13) 
    {
      if (Serial.available() > 0)
      {
        if (Serial.peek() == 10 || Serial.peek() == 13)
        {
          mychar = Serial.read();
        }
      }
      return rslt;
    }
      
    rslt = rslt + (char)mychar;
      
  }
    return rslt;
}
 
