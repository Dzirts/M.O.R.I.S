
//-----------------------------------------------------------------
//                              INCLUDES
//-----------------------------------------------------------------
#include <IRremote.h>
#include <dht.h>
//-----------------------------------------------------------------------
//      PIN 
//----------------------------------------------------------------------
const int PIN_photoelctric= 0;
const int PIN_pir1=2; 
const int PIN_pir2=4;
const int PIN_DHT=9; 
const int PIN_RECV=8
//const int PIN_6 
//const int PIN_7 
//const int PIN_8 
//const int PIN_9 
//const int PIN_10 
//const int PIN_11 
//const int PIN_12
//--------------------------------------------------------------------
//                            Real time pic 2 -  sensor array
//--------------------------------------------------------------------
// all the data is saveed in this array 
// 00-tmeprature
// 01-Humidity
// 02-light level
// 03-PIR_1
// 04-PIR_2
// 05
// 06
// 07
// 08
// 09
// 10
// 11
// 12
// 13
// 14
// 15
unsigned int RT_Pic[16]

//-----------------------------------------------------------------
//                               VARS
//-----------------------------------------------------------------
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        
//the time when the sensor outputs a low impulse
long unsigned int lowIn;         
//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime; 
//Creats a DHT object
dht DHT; 

IRsend irsend;
IRrecv irrecv(PIN_RECV);
decode_results results;
char letter = Serial.read();

//---------------------------------------------------------
// FUNCTIONS 
//---------------------------------------------------------

void check_button (){
 if (irrecv.decode(&results)){
           Serial.println(results.value, HEX);
           switch (results.decode_type){
            case NEC: Serial.println("NEC"); break ;
            case SONY: Serial.println("SONY"); break ;
            case RC5: Serial.println("RC5"); break ;
            case RC6: Serial.println("RC6"); break ;
            case DISH: Serial.println("DISH"); break ;
            case SHARP: Serial.println("SHARP"); break ;
            case JVC: Serial.println("JVC"); break ;
            case SANYO: Serial.println("SANYO"); break ;
            case MITSUBISHI: Serial.println("MITSUBISHI"); break ;
            case SAMSUNG: Serial.println("SAMSUNG"); break ;
            case LG: Serial.println("LG"); break ;
            case WHYNTER: Serial.println("WHYNTER"); break ;
            case AIWA_RC_T501: Serial.println("AIWA_RC_T501"); break ;
            case PANASONIC: Serial.println("PANASONIC"); break ;
            case DENON: Serial.println("DENON"); break ;
          default:
            case UNKNOWN: Serial.println("UNKNOWN"); break ;
           }
        irrecv.resume();
         } 
}
void send_ir(void)
{
  char letter = Serial.read();
   irsend.sendNEC('0x'+Serial.read(), 32); //send 0x0 code (8 bits)
   if (letter == 'p')
     {
       irsend.sendNEC(0x4FB4AB5, 32); //send 0x0 code (8 bits)
     }
      else if (letter == '+')
     {
       irsend.sendNEC(0x4FB40BF, 32); //send 0x0 code (8 bits)
     }
       else if (letter == '-')
     {
       irsend.sendNEC(0x4FB906F, 32); //send 0x0 code (8 bits)
     }
       else if (letter == 'm')
     {
       irsend.sendNEC(0x4FBCA35, 32); //send 0x0 code (8 bits)
     }
     irrecv.enableIRIn();
}

void PIR(int PIR_NUM){
//pir
     if(PIR_NUM==1)
      {
        PIN=PIN_PIR1;
      }
      if(PIR_NUM==2)
      {
        PIN=PIN_PIR2;
      }
     if(digitalRead(PIN) == HIGH)
     {
       //digitalWrite(ledPin, HIGH);   the led visualizes the sensors output pin state
       if(lockLow){  
//         makes sure we wait for a transition to LOW before any further output is made
         lockLow = false;            
         Serial.println("---");
         Serial.print(("motion detected at %d"),PIR_NIM);
        Serial.print(millis(),"1000");
         Serial.println( "sec"); 
         delay(100);
         }         
         takeLowTime = true;
       }

     if(digitalRead(PIN) == LOW){       
      // digitalWrite(ledPin, LOW);  the led visualizes the sensors output pin state

       if(takeLowTime){
        lowIn = millis();         // save the time of the transition from high to LOW
        takeLowTime = false;      // make sure this is only done at the start of a LOW phase
        }
//       if the sensor is low for more than the given pause, 
  //     we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn /*pause*/){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("motion ended %d"),PIR_NIM);      //output
           Serial.print((millis() - pause),"1000");
           Serial.println("sec");
           delay(100);
           }
       }
}


void temputre () 
{
     int readData = DHT.read11(PIN_DHT); // Reads the data from the sensor
     RT_pic[0] = DHT.temperature; // Gets the values of the temperature
     RT_pic[1] = DHT.humidity; // Gets the values of the humidity
}
void photoelectric(void)
{
    RT_pic[2] = analogRead(PIN_photoelctric);  
    //dark-<10
    //dim <200
    //light <500
    // bright<800
    //else very bright
}  
//-----------------------------------------------------------
//                        SETUP
//-----------------------------------------------------------



void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  pinMode(PIN_PIR1, INPUT);
  pinMode(PIN_PIR2, INPUT);
  digitalWrite(PIN_PIR1, LOW);
  digitalWrite(PIN_PIR2, LOW); 
//    give the sensor some time to calibrate
  Serial.println( "calibrating sensor" );
    for(int i = 0; i == calibrationTime; i++){
      Serial.print(".");
      delay(100);
      }
    Serial.println( "done");
    Serial.println("SENSOR ACTIVE");
    delay(50);
}



//--------------------------------------------------------------
//                             MAIN 
//--------------------------------------------------------------
void loop()
{
 //collect all info from sensors
            photoelectric() ;
            PIR(1);
            PIR(2);
            temp();
            voice();


 //send IR
            if(irrecv.decode(&results)) 
            {
                checkbutton();
                irrecv.resume(); // resume receiver
                delay(50);
            }
            else if (letter == 'p')
            {
                 irsend.sendNEC(0x4FB4AB5, 32); //send 0x0 code (8 bits)
            }
            else if (letter == '+')
            {
                irsend.sendNEC(0x4FB40BF, 32); //send 0x0 code (8 bits)
             }
             else if (letter == '-')
             {
                 irsend.sendNEC(0x4FB906F, 32); //send 0x0 code (8 bits)
             }
             else if (letter == 'm')
             {
                 irsend.sendNEC(0x4FBCA35, 32); //send 0x0 code (8 bits)
             }
             irrecv.enableIRIn();

// send WIFI to resbery 



//get WIFI from resbery 


             
             
}
       
