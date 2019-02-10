int latchPin = 3;
int clockPin = 4;
int dataPin = 2;

//byte leds = 0;

byte lights[16];

byte ON = 0;
byte OFF = 1;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  Serial.begin(9600);

  AllOff();
}

void loop() {

  Snake();

  Random();

  SingleLightOffChase();
  
  for(int loop = 0; loop < 20;loop++)
  {
    AllOff();
    for(int x = 1; x <=8;x++)
    {
      WriteOut(ON,false);
      WriteOut(OFF,true);
    }
    delay(100);

    AllOff();
    for(int x = 1; x <=8;x++)
    {
      WriteOut(OFF,false);
      WriteOut(ON,true);
    }
    delay(100);
  }

  for(int loop = 0; loop < 10;loop++)
  {
     AllOff();
  
    for(int x = 1; x <=8;x++)
    {
      
      WriteOut(ON,false);
      WriteOut(OFF,true);
      
      delay(100);
    }

     AllOff();
  
    for(int x = 1; x <=8;x++)
    {

      WriteOut(OFF,false);
      WriteOut(ON,true);
      
      delay(100);
    }
     delay(100);
  }
  
  for(int loop = 0; loop < 10; loop++)
  {
    AllOff();
    delay(200);
    AllOn();
    delay(200);
  }

  
  for(int loop = 0; loop < 5; loop++)
  {
    delay(100);
    AllOff();
  
    int y = 3;
    for (int x = 3; x <=11;x++)
    {
      lights[x-1] = 0;
      lights[y-1] = 0;

      Update16();
      delay(100);
      y--;
      if (y == 0) 
      {
        y = 16;  
      }
    }
    delay(100);
    AllOff();
    
    y = 11;
    for (int x = 11; x >=3;x--)
    {
      lights[x-1] = ON;
      lights[y-1] = ON;

      Update16();
      delay(100);
      
      y++;
      if (y == 17) 
      {
        y = 1;  
      }
    }
  }
}

void Snake()
{
  AllOff();
  for(int loop = 0; loop < 16;loop++)
  {

    for (int y = 1;y <= loop; y++) 
    {
      WriteOut(ON,true);
      delay(100);
    }
    
    delay(100);
    for(int x = 1; x <=16 - loop; x++)
    {
      WriteOut(OFF,true);
      delay(100);
    }
  }
}

void Random()
{
  AllOff();
  for(int loop = 0; loop < 100;loop++)
  {
    for (int xx = 0; xx <=15;xx++)
    {
      lights[xx] = random(2); 
    }
    Update16();
    delay(100);
  }
}

void SingleLightOffChase()
{
   AllOff();
  for(int loop = 0; loop < 10;loop++)
  {
    AllOn();
    WriteOut(OFF,false);
    
    delay(100);
    for(int x = 1; x <=16; x++)
    {
      WriteOut(ON,true);
      delay(100);
    }
  }
}


void WriteOut(byte state,bool latch)
{
  
  digitalWrite(dataPin, state);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);

  if (latch)
  {
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
  }
  
}

void AllOff()
{
  for (int x = 0; x <=15;x++)
  {
    lights[x] = OFF;
  }
  Update16();
}

void AllOn()
{
  for (int x = 0; x <=15;x++)
  {
    lights[x] = ON;
  }
  Update16();
}

void Update16Reverse()
{
   digitalWrite(latchPin, LOW);
   for (int xx = 0; xx <=15;xx++)
   {
      WriteOut(lights[xx],false);
   }
   digitalWrite(latchPin, HIGH);
   Serial.println(); 
}

void Update16()
{
   digitalWrite(latchPin, LOW);
   for (int xx = 15; xx >=0;xx--)
   {
      WriteOut(lights[xx],false);
   }
   digitalWrite(latchPin, HIGH);
}

/*
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   //shiftOut(dataPin, clockPin, LSBFIRST, leds);
   Serial.println(leds,BIN); 
   digitalWrite(latchPin, HIGH);
}
*/
