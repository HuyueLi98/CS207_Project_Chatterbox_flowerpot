#include <WaveHC.h>
#include <WaveUtil.h>
const int lightPin = A3;
const int moisturePin = A2; 
const int tmpPin=A1;
const int redpin = 8;
const int greenpin = 9;
int light;
int soild;
int temperature;
int tks;


SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader file;   // This object represent the WAV file for a pi digit or period
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time

#define error(msg) error_P(PSTR(msg))

void setup() {
  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);


 Serial.begin(9600);// Initialize serial communication  // set up Serial library at 9600 bps
  
  
  PgmPrintln("Pi speaker");
  
  if (!card.init()) {
    error("Card init. failed!");
  }
  if (!vol.init(card)) {
    error("No partition!");
  }
  if (!root.openRoot(vol)) {
    error("Couldn't open dir");
  }

  PgmPrintln("Files found:");
  root.ls();
  light = Ldetect();
  soild = Sdetect();
  temperature=Tdetect();
  conditionOut(light,soild,temperature);
}
void loop() {  
  light = Ldetect();
  soild = Sdetect();
  temperature=Tdetect();
  //conditionOut(light,soild,temperature);
  int con=0;
  tks=0;
  
  
  //lightpart
  if(light==3)
  {
    playcomplete("Lhigh.wav");
    con++;
    tks=1;
  }
  
  if(soild==0)
  {
    playcomplete("Sdry.wav");
    con++;
    tks=1;
  }
  else if(soild==3)
  {
    playcomplete("Swet.wav");
    con++;
    tks=1;
  }
  Serial.println(con);

  if(temperature<12||temperature>30)
  {
    playcomplete("Tbad.wav");
    con++;
    tks=1;
  }
  if(con==0)
  {
    digitalWrite(greenpin,HIGH);//green
    digitalWrite(redpin,LOW);
    if(tks==1)
    {
      playcomplete("tks.wav");
      tks=0;
    }
  }
  else if(con==1)
  {
    digitalWrite(greenpin,HIGH);
    digitalWrite(redpin,HIGH);
    //yellow
  }
  else if(con==2)
  {
    digitalWrite(redpin,HIGH);//red
    digitalWrite(greenpin,LOW);
  }
  else if(con==3)
  {
    for(int i=0;i<10;i++)
    {
      digitalWrite(redpin,HIGH);
      digitalWrite(greenpin,LOW);
      delay(500);
      digitalWrite(redpin,LOW);
      digitalWrite(greenpin,LOW);
      delay(500);
    }
    //blink red

  }

  
   
   
   delay(5000);
}



int Ldetect(){
    int light;
    light = analogRead(lightPin);//read the data from potpin which
    //Serial.println(light);
    light=map(light,0,800,0,3);
    
//    playcomplete("lig.wav");//***this line is not finished
//    if(light==0)
//    {
//      playcomplete("low.wav");
//    }
//    else if (light == 1)
//    {
//      playcomplete("mid.wav");
//    }
//    else if (light ==2 )
//    {
//      playcomplete("high.wav");
//    }
//    else if ( light ==3)
//    {
//      playcomplete("vhigh.wav");
//    }
//    return light;
  
}



int Tdetect(){
  int reading; //raw reading from A1 
  float volts;//Voltage output number 
  float celsius; //celsius output number
   reading = analogRead(tmpPin); //read A1
   volts = reading/1023.0 * 5.0;
   celsius=volts*100.0-50; 
//   playcomplete("tmp.wav");//** imcomplete;
//   numberSp(celsius);
//   playcomplete("cel.wav");//** imcomplete
   
   return celsius;
}




int Sdetect(){
   int tempValue = 0; // variable to temporarly store moisture value 
   for (int a = 0; a < 10; a++) { 
     tempValue += analogRead(moisturePin); 
     delay(100); 
   } 
   tempValue=tempValue/10;
 //  Serial.println(tempValue);
   tempValue=map(tempValue,1023,0,0,3);
  // Serial.println(tempValue);
   
//   playcomplete("soi.wav");//** imcomplete;
//    if(tempValue==0)
//    {
//      playcomplete("low.wav");
//    }
//    else if (tempValue == 1)
//    {
//      playcomplete("mid.wav");
//    }
//    else if (tempValue ==2 )
//    {
//      playcomplete("high.wav");
//    }
//    else if ( tempValue ==3)
//    {
//      playcomplete("vhigh.wav");
//    }
   return tempValue; 
}
 void conditionOut(int light,int tempValue,int temperature){

  //light condition output part
      playcomplete("lig.wav");
    if(light==0)
    {
      playcomplete("low.wav");
    }
    else if (light == 1)
    {
      playcomplete("mid.wav");
    }
    else if (light ==2 )
    {
      playcomplete("high.wav");
    }
    else if ( light ==3)
    {
      playcomplete("vhigh.wav");
    }
    delay(500);
    //soild condition output part
   playcomplete("soi.wav");
    if(tempValue==0)
    {
      playcomplete("low.wav");
    }
    else if (tempValue == 1)
    {
      playcomplete("mid.wav");
    }
    else if (tempValue ==2 )
    {
      playcomplete("high.wav");
    }
    else if ( tempValue ==3)
    {
      playcomplete("vhigh.wav");
    }
    delay(500);
   //
      playcomplete("tmp.wav");//** imcomplete;
   numberSp(temperature);
   playcomplete("cel.wav");//** imcomplete
 }
 












///////////PLAY PART/////////////

void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}

void playcomplete(char *name) {
  playfile(name);
  while (wave.isplaying);
  
  // see if an error occurred while playing
  sdErrorCheck();
}
/*
 * Open and start playing a WAV file
 */
void playfile(char *name) {
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  if (!file.open(root, name)) {
    PgmPrint("Couldn't open file ");
    Serial.print(name); 
    return;
  }
  if (!wave.create(file)) {
    PgmPrintln("Not a valid WAV");
    return;
  }
  // ok time to play!
  wave.play();
}

//this part is for playing numbers;
void numberSp(int i)
{
  if (i<0)
  {
    playcomplete("minus.wav");
    i=-i;
  }
  String owav = "0.wav";
  String wav =".wav";
  int tens = i/10;
  int ones = i - tens*10;
  String ten = tens + owav;
  String one = ones + wav;
  char chat_ten[8];
  char chat_one[8];
  ten.toCharArray(chat_ten, 8);
  one.toCharArray(chat_one, 8);

    int sensorValue = 100; 
  String stringOne = "Sensor value: ";
  String stringThree = stringOne + sensorValue;
  Serial.println(stringThree); 
  
  
  if(tens != 0)
  {
    
    playcomplete(chat_ten);
  }
  if(ones!=0)
  {
   playcomplete(chat_one);
  }
}

void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}
