/*
 2.4G Transmitter nRF24L01 module Arduino
 Control steering servo+ESC
 Written by: Pason Tanpaiboon
 Febuary 2016
 Version.1.8
 22/2/2016 change esc writing
 26/2/2016 2-pipes sketch
 20/4/2016 change potentiometer to steering wheel and trigger of pistal transmitter
 19/6/2017 change from esc to be L298N module
 
 This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ 
 or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

uint8_t data[3] ; 
const uint8_t buffer_size = sizeof(data);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };//two pipes communication 

const int analogInPinX = A3;// 
const int analogInPinY= A4;
const int potpin= A0;// 
const int dumpsw = 4;//dump switch pin 4

int Str_value = 0; 
int Esc_value = 0;       

int outputValue = 0; 
int potval=0;
int dumpswval = 0;

void setup(void)
{
  Serial.begin(115200);
  pinMode(analogInPinX,INPUT );
  pinMode(analogInPinY,INPUT );
  pinMode(potpin,INPUT );

  radio.begin();
  radio.setRetries(15,15);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  radio.printDetails();
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.stopListening();
}

void loop(void)
{

  Str_value = analogRead(analogInPinX); 
  Str_value = Str_value/10;
  data[0] = Str_value;
  //data[0] = map(Str_value, 150, 1020 , 0 , 180); //steering wheel
  //////////////////////////////////////////////////////////////////////// 
  Esc_value = analogRead(analogInPinY); 

  //Esc_value = map(Esc_value, 20, 95 , 2000 , 1000); 

  data[1] = Esc_value/10;//scaling escval
  /////////////////////////////////////////////////////////////////////////
  dumpswval = digitalRead(dumpsw);
  delay(15); 

  if (dumpswval == HIGH){

    data[2] = 1;//dump switch on,HIGH,not exceed 255
  }
  else {
    data[2] = 0;//dump switch off,LOW
  }
  /////////////////////////////////////////////////////////////////////////

  radio.stopListening();

  bool ok = radio.write(  data ,sizeof(data) );
  delay(30);
  radio.startListening();
  delay(20);
  if (ok){
    Serial.print("data[0]=");
    Serial.print(data[0]);
    Serial.print(" data[1]=");
    Serial.print(data[1]);
    Serial.print(" data[2]=");
    Serial.println(data[2]);
    // Serial.print(" potpin=");
    // Serial.println(analogRead(analogInPinY));
    // Serial.print(" data[3]=");
    // Serial.println(data[3]);
  }
  else
    Serial.println("Failed");

}//void loop()























