/*
  I2C Slave Demo
*/
 
#include<Wire.h>
#define slaveAddress 9  //you have to assign an 8-bit address to Slave
byte dataArray[3];

int temp, humd, co2ppm = 0;

void setup()
{
  Wire.begin(slaveAddress);
  Serial.begin(9600);
  //-----------------------------------
  Wire.onReceive(receiveEvent);//you need to declre it in setup() to receive data from Master
}

void loop()
{
  delay(1);
}

void receiveEvent(int howmany) //howmany = Wire.write()executed by Master
{
  int temp = 0;
  int humd = 0;
  int co2ppm = 0;

  for(int i=0; i<howmany; i++)
  {
    dataArray[i] = Wire.read();
    if(i == 0){
      temp = dataArray[i];
    }
    else if(i == 1){
      humd = dataArray[i];
    }
    else if(i == 2){
      co2ppm = dataArray[i];
    }
    Serial.println("===Array Start===");
    Serial.println(temp);
    Serial.println(humd);
    Serial.println(co2ppm);
    Serial.println("===Array End===");
  }
}