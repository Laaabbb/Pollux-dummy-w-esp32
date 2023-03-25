/*
  I2C Master Demo
*/
#include <dht.h>
#include<Wire.h>

#define slaveAddress 9
#define DHT11_PIN 7
#define anInput     A0        //analog feed from MQ135
#define co2Zero     55        //calibrated CO2 0 level

dht DHT;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  //-----------------------------------
  pinMode(anInput,INPUT);       //MQ135 analog feed set for input
  //--------------------------------------------------------------------
}

void loop()
{
  delay(1);
  int co2now[10];        //int array for co2 readings
  int co2raw = 0;        //int for raw value of co2
  int co2ppm = 0;        //int for calculated ppm
  int zzz = 0;           //int for averaging

  int chk = DHT.read11(DHT11_PIN);

  int temp = DHT.temperature;
  int humd = DHT.humidity;

  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);

     for (int x = 0;x<10;x++)  //samplpe co2 10x over 2 seconds
  {                   
    co2now[x]=analogRead(A0);
    delay(200);
  }

  for (int x = 0;x<10;x++)  //add samples together
  {                     
    zzz=zzz + co2now[x];  
  }
  
  co2raw = zzz/10;               //divide samples by 10
  co2ppm = co2raw - co2Zero;     //get calculated ppm

  Serial.print("AirQuality = ");
  Serial.print(co2ppm);  // prints the value read
  Serial.println(" PPM");

  byte dataArray[3] = {temp, humd, co2ppm};

  Wire.beginTransmission(slaveAddress); //address is queued for checking if the slave is present
  for (int i=0; i<3; i++)
  {
    Serial.println("===Array===");
    Wire.write(dataArray[i]);  //data bytes are queued in local buffer
    Serial.println(dataArray[i]);
  }
  Wire.endTransmission(); //all the above queued bytes are sent to slave on ACK handshaking
}