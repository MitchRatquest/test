int LOAD = 12;
int DATA = 11;
int CLK = 10;

byte temp1[5];
byte temp2[5];
byte temp3[5];
byte temp4[5];

byte serialinput[100]; //temparray for serial buffer input
byte displayarray[100];

int gotserial = 0;
int currentarraylength = 0;


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
  Serial.setTimeout(200);
  pinMode(12, OUTPUT);         //load
  pinMode(11, OUTPUT);         //SDATA
  pinMode(10, OUTPUT);         //SDCLK
  digitalWrite(12, HIGH);      //inhibit chip
  
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

void loop()
{

  sendDisplay(0xF3);         //power levels as detailed page 10 (F0 -F6, F8 = shutdown)

  int serialsize = 0;        //temp var for sniffing the serial events
  
  while(Serial.available() > 0)
  {
    
    serialinput[serialsize] = Serial.read(); //serial prints out bytes anyway
    serialsize++;                            //increment array explicit integer
    gotserial = 1;                           //serial received flag 
    
  }

  if(gotserial)
  {
    for(int i = 0; i < serialsize; i++)
    {
      displayarray[i] = serialinput[i];
    }
    currentarraylength = serialsize;  //make sure we get the size of the array into a fresh variable or it'll reset
    gotserial = 0;                    //reset flag
   
  }
  
  bettermarquee(displayarray,200,currentarraylength);  //send the goods to the screen thrower
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
 

void bettermarquee(byte letterarray[], int delaytime, int arraysize)
{
  for(int i = 0; i < arraysize; i++)  //sets the max for the array cycle
  {
      for(int a = 0; a < 5; a++)
      {
        if(letterarray[i] ==32)temp4[a] = alphabet[letterarray[i]-6][a];   //CHECK FOR SPACES IN INPUT MESSAGE
        if(letterarray[i] < 64)temp4[a] = alphabet[letterarray[i]-6][a];   //CHECK FOR NOT LETTERS IN INPUT MESSAGE
        else temp4[a] = alphabet[letterarray[i]-65][a];                    //subtract 65 as they're ASCII inputs and need 0-26 array address (A=65 ASCII, A=0 array)
      }

      sendany(0, temp1);                              //push to screen
      sendany(1, temp2);
      sendany(2, temp3);
      sendany(3, temp4);
      delay(delaytime);

      for(int copyar =0; copyar < 5; copyar++) ///CANT DIRECTLY COPY ARRAYS I GUESS
      {
        temp1[copyar] = temp2[copyar];         //shift all to left 1
      }
      for(int copyar =0; copyar < 5; copyar++) ///CANT DIRECTLY COPY ARRAYS I GUESS
      {
        temp2[copyar] = temp3[copyar];         //shift all to left 1
      }
      for(int copyar =0; copyar < 5; copyar++) ///CANT DIRECTLY COPY ARRAYS I GUESS
      {
        temp3[copyar] = temp4[copyar];         //shift all to left 1
      }
  }
}  
 
 
