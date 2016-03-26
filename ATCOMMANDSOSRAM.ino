int LOAD = 12;
int DATA = 11;
int CLK = 10;

byte temp1[5] = {0,0,0,0,0};
byte temp2[5] = {0,0,0,0,0}; 
byte temp3[5] = {0,0,0,0,0};
byte temp4[5] = {0,0,0,0,0};

byte serialinput[100];   //temparray for serial buffer input
byte displayarray[100];  //filled buffer with correct details


int gotserial = 0;
int currentarraylength = 0;


int address = 0;     //track array read location
int interval = 80;
unsigned long prevMillis = 0;

byte alphabet[27][5] =
{
  {0x04, 0x2A, 0x5F, 0x71, 0x91}, //A
  {0x1E, 0x29, 0x4E, 0x69, 0x9E}, //B
  {0x0F, 0x30, 0x50, 0x70, 0x8F}, //C
  {0x1E, 0x29, 0x49, 0x69, 0x9E}, //D
  {0x1F, 0x30, 0x5E, 0x70, 0x9F}, //E
  {0x1F, 0x30, 0x5E, 0x70, 0x90}, //F
  {0x0F, 0x30, 0x53, 0x71, 0x8F}, //G
  {0x11, 0x31, 0x5F, 0x71, 0x91}, //H
  {0x0E, 0x24, 0x44, 0x64, 0x8E}, //I
  {0x01, 0x21, 0x41, 0x71, 0x8E}, //J
  {0x13, 0x34, 0x58, 0x74, 0x93}, //K
  {0x10, 0x30, 0x50, 0x70, 0x9F}, //L
  {0x11, 0x3B, 0x55, 0x71, 0x91}, //M
  {0x11, 0x39, 0x55, 0x73, 0x91}, //N
  {0x0E, 0x31, 0x51, 0x71, 0x8E}, //O
  {0x1E, 0x31, 0x5E, 0x70, 0x90}, //P
  {0x0C, 0x32, 0x56, 0x72, 0x8D}, //Q
  {0x1E, 0x31, 0x5E, 0x74, 0x92}, //R
  {0x0F, 0x30, 0x4E, 0x61, 0x9E}, //S
  {0x1F, 0x24, 0x44, 0x64, 0x84}, //T
  {0x11, 0x31, 0x51, 0x71, 0x8E}, //U
  {0x11, 0x31, 0x51, 0x6A, 0x84}, //V
  {0x11, 0x31, 0x55, 0x7B, 0x91}, //W
  {0x11, 0x2A, 0x44, 0x6A, 0x91}, //X
  {0x11, 0x2A, 0x44, 0x64, 0x84}, //Y
  {0x1F, 0x22, 0x44, 0x68, 0x9F}, //Z
  {0x00, 0x20, 0x40, 0x60, 0x80} //BLANK
};
  
  

void setup()
{
  Serial.begin(57600);
  Serial.setTimeout(50);       //needs to be lower than screen's delay rate 
  pinMode(12, OUTPUT);         //load
  pinMode(11, OUTPUT);         //SDATA
  pinMode(10, OUTPUT);         //SDCLK
  digitalWrite(12, HIGH);      //inhibit chip
  pinMode(9, OUTPUT);
  for(int i = 0; i < 5; i++)   //clear screen
  {
    byte tmpar[5];
    for(int g = 0; g < 5; g++)
    {
      tmpar[g] = alphabet[26][g];
    }
    sendany(i, tmpar);
  }
}

byte ledshit = 0;
void loop()
{

  sendDisplay(0xF3);         //power levels as detailed page 10 (F0 -F6, F8 = shutdown)
  getserial();
  marqueeSet(interval);
  digitalWrite(9, ledshit);
  
}

void sendDisplay(int screen)
{
  digitalWrite(LOAD, LOW);        //chip listen now
  digitalWrite(CLK, LOW);         //clock starts low
  for(int i = 0; i < 8; i++)
  {
    digitalWrite(DATA, screen >> i & 1);  //bitshift left i and 1 to peek the bit
    digitalWrite(CLK, HIGH);              //latches on rising edge
    digitalWrite(CLK, LOW);               //set it back down to toggle data
  }
  digitalWrite(LOAD, HIGH);               //inhibit any more data input
}


void sendany(int disp, byte letter[5])
{
 int tempdisp = 0xA0;     //upper nibble for display change command 
 tempdisp |= disp;        //disp between 0 and 3
 sendDisplay(tempdisp);
 for(int i = 0; i < 5; i++)  //send 5 bytes for pixels
 {
   sendDisplay(letter[i]);
 }
}

byte arraycopy(byte destinationarray[], byte sourcearray[]) 
{
  for(int copyar =0; copyar < 5; copyar++) 
    {
      destinationarray[copyar] = sourcearray[copyar];         
    }
}
 
void getserial()
{
  int serialsize = 0;        //temp var for sniffing the serial events
  gotserial = 0;
  while(Serial.available() > 0)
  {
    serialsize = Serial.readBytesUntil('<', serialinput, 100);  //MUST HAVE LINE TERMINATOR jesus christ how i've Learnt
    if(serialsize > 0)gotserial=1;                              //set flag if we caught one pappi
  }
  if(gotserial)
  {
    if(serialinput[0] == 'A' && serialinput[1] == 'T')  //analog of AT commands i'm pretty sure I used once
    {

      switch (serialinput[2])
      {
        case 'A':
          ledshit = serialinput[3];
          gotserial = 0;
          break;
        case 'B': 
          interval = serialinput[3] + serialinput[4];
          gotserial = 0;
          break;
      }
      //ledshit = serialinput[2];                         //no further parsing, just accept the 3rd position as interval data
      //gotserial = 0;
    }  //COULD ALSO USE CASE FUNCTION FOR [ATB3] or [ATO6] ETC
  
    
    else
    {
      for(int i = 0; i < serialsize; i++)
      {
        displayarray[i] = serialinput[i];
      }
      currentarraylength = serialsize + 1;  //make sure we get the size of the array into a fresh variable or it'll reset AND ADD ONE
      gotserial = 0;                    //reset flag
      //Serial.print(currentarraylength);  //debugging purposes
    } 
  }
}


void quickmarquee(byte letterarray[], int arraysize)
{
 if(address >= arraysize-1)  //we are prepending a space as seen below
  {
    address = 0; 
  }
  for(int i = 0; i < 5; i++)
  {
    if(address==0)temp4[i] = alphabet[26][i]; //add a space at first slot PREPEND THAT OR SUFFER
    else if(letterarray[address-1]==32) temp4[i] = alphabet[letterarray[address-1]-6][i];  //ASCII 32 is space, alphabet[26], subtract 6
    else temp4[i] = alphabet[letterarray[address-1]-65][i];                         //convert from ASCII to alphabet array format
  }
  sendany(0, temp1);  //push each to screen
  sendany(1, temp2);
  sendany(2, temp3);
  sendany(3, temp4);
  
  arraycopy(temp1, temp2); //shift to the right section
  arraycopy(temp2, temp3);
  arraycopy(temp3, temp4);
 
  address++;  //increment address for next time
}

void marqueeSet(int delaytime)
{
  unsigned long currentMillis = millis();  //get current time babu

  if(currentMillis - prevMillis >= delaytime) //peep them differences and do it or not
  {
    prevMillis = currentMillis;
    quickmarquee(displayarray,currentarraylength);
    
  }
}
