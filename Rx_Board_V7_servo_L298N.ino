/*
 2.4G Receiver nRF24L01 module Arduino
 Control steering servo+ESC
 Written by: Pason Tanpaiboon
 Febuary 2016
 Version.1.8
 22/2/2016 change esc writing
 20/4/2016 change potentiometer to steering wheel and trigger of pistal transmitter
 19-06-2017 change from esc to be L298N module
 
 This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
 or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <Servo.h> 
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };//2 pipes communication
uint8_t received_data[3];
uint8_t num_received_data =sizeof(received_data);

const int servo1 = 3;       //  servo
const int dumpsw  = 4;//dump swich pin 4
Servo myservo;  // create servo object to control a servo
int servoval;           // variable to read the value from the analog pin
int dumpswval=0;
int pos = 0 ;  
int escval;  
/*
const int esc1 = 6 ;// esc 
Servo myesc;  // create servo object to control a servo
*/
//////////////////////////////////////////////////////////////////
/*
 const int enableR = 3;
 const int MotorR1 = 4;
 const int MotorR2 = 5;
 int enableRval ;//no use
 int MotorR1val ;//no use
 int MotorR2val ;//no use
 int MotorRspeed ;//no use
 */

//Driving Motor
const int enableL = 6;
const int MotorL1 = 7;
const int MotorL2 = 8;
int enableLval ;//driving motor
int MotorL1val ;//driving motor
int MotorL2val ;//driving motor
int MotorLspeed ;//driving motor
//////////////////////////////////////////////////////////////////

void setup(void)
{
  /*
  pinMode (enableR, OUTPUT);
   pinMode (MotorR1, OUTPUT);
   pinMode (MotorR2, OUTPUT);  
   digitalWrite(enableR , HIGH);
   */
  pinMode (enableL, OUTPUT);
  pinMode (MotorL1, OUTPUT);
  pinMode (MotorL2, OUTPUT);  
  digitalWrite(enableL , HIGH);

  Serial.begin(115200);

  delay(300); //wait until the esc starts in case of Arduino got power first
  myservo.attach(servo1);  // attaches the servo on pin 3 to the servo object 
  myservo.write(90);
  pinMode (dumpsw, OUTPUT);

  radio.begin();
  radio.setRetries(15,15);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  radio.printDetails();
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();

}//void setup

void loop(void)
{

  ///////////////////////////////Radio reading///////////////////////////////////////////////////////     
  if ( radio.available() )
  {
    bool done = false;
    while (!done)
    {
      done = radio.read(&received_data, num_received_data  );
      delay(30);
    }// while (!done)

    Serial.print(received_data[0]);//Steering value
    Serial.print ("---"); 
    Serial.print(received_data[1]);//esc value
    Serial.print ("---"); 
    Serial.print(received_data[2]);//
    Serial.print (">>>"); 
    
    radio.stopListening();

    radio.startListening();

    /**************************************************************steering servo*********************************************************************/
    servoval = received_data[0];
    servoval = servoval*10;//scale back

    servoval = map(servoval, 150 , 910, 180, 0);

    if (servoval >= 88 && servoval <= 97 )
    {
      servoval = 94; 
      myservo.write(servoval);//nutral position 
      delay(30);
    }

    myservo.write(servoval); // sets the servo position according to the scaled value
    delay(30);// waits for the servo to get there

    /********************************************************************esc servo***************************************************************/

    escval = received_data[1];

    escval = escval*10 ; //mutiplier 10 and 2.9 matched china car esc 20A

    escval = map(escval, 160 , 900 ,0, 180); //triggger

    /*************************************************************Stopping***************************************************************/
    //   if (escval >= 520 && escval <= 550 ) //Nuetral steering wheel + Nuetral trigger
    if (escval >= 85 && escval <= 98 ) //Nuetral steering wheel + Nuetral trigger
    {
      escval = 90; 
      MotorLspeed = 0;//off 
      MotorL1val = 0;
      MotorL2val = 0;
      Engine();
      Serial.print("Stopping");
    }
    /*************************************************************Straight Forward***************************************************************/
    if ( escval < 85 )//Nuetral steering wheel + positive trigger
    {
      MotorLspeed =  map(escval,90,180,255,0);
      MotorL1val = 1;
      MotorL2val = 0;
      Engine();
      Serial.print("Forward");
    }      
    /*************************************************************Straight Backward***************************************************************/
    if ( escval > 98   )//Nuetral steering wheel + negative trigger    
    {
      MotorLspeed = map(escval,90,180,0,255);
      MotorL1val = 0;
      MotorL2val = 1;   
      Engine();
      Serial.print("Backward");
    }

    Serial.print (">>>"); 
    Serial.print(servoval);
    Serial.print ("---"); 
    Serial.print(escval);
    Serial.print ("---"); 
    Serial.print(MotorLspeed);
    Serial.print ("---"); 
    Serial.println(dumpswval);
    /***********************************************Dump switch****************************************************************************/
    dumpswval = received_data[2];
    if ( dumpswval == 1 ){ // >HIGH dump on
      digitalWrite(dumpsw,HIGH);
      delay(15);   
    }
    else  {
      digitalWrite(dumpsw,LOW);
      delay(15);   
    }
  }// if ( radio.available() )
}//void loop(void)

void Engine() {
  /*  
   //Right Motor 
   analogWrite(enableR , MotorRspeed);// 
   digitalWrite (MotorR1,MotorR1val);
   digitalWrite (MotorR2,MotorR2val);  
   delay (15);
   */
  //Left Motor 
  analogWrite(enableL , MotorLspeed);// 
  digitalWrite (MotorL1,MotorL1val);
  digitalWrite (MotorL2,MotorL2val);  
  delay (15);
}

