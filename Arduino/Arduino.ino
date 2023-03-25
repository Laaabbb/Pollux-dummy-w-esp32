#include "DHT.h"
#include <BH1750.h>
#include "Adafruit_LTR390.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHT1PIN 7
#define DHT1TYPE DHT11
#define anInput     A0        //analog feed from MQ135
#define co2Zero     55        //calibrated CO2 0 level

DHT dht(DHT1PIN, DHT1TYPE);
BH1750 lightMeter;
Adafruit_LTR390 ltr = Adafruit_LTR390();

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(){
  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin();
  lcd.init();
  lcd.backlight();
  dht.begin();
  
  pinMode(anInput,INPUT);       //MQ135 analog feed set for input

  if ( ! ltr.begin() ) {
    Serial.println("Couldn't find LTR sensor!");
    while (1) delay(10);
  }
  Serial.println("Found LTR sensor!");

  ltr.setMode(LTR390_MODE_UVS);
  if (ltr.getMode() == LTR390_MODE_ALS) {
    Serial.println("In ALS mode");
  } else {
    Serial.println("In UVS mode");
  }

  ltr.setGain(LTR390_GAIN_3);
  Serial.print("Gain : ");
  switch (ltr.getGain()) {
    case LTR390_GAIN_1: Serial.println(1); break;
    case LTR390_GAIN_3: Serial.println(3); break;
    case LTR390_GAIN_6: Serial.println(6); break;
    case LTR390_GAIN_9: Serial.println(9); break;
    case LTR390_GAIN_18: Serial.println(18); break;
  }

  ltr.setResolution(LTR390_RESOLUTION_16BIT);
  Serial.print("Resolution : ");
  switch (ltr.getResolution()) {
    case LTR390_RESOLUTION_13BIT: Serial.println(13); break;
    case LTR390_RESOLUTION_16BIT: Serial.println(16); break;
    case LTR390_RESOLUTION_17BIT: Serial.println(17); break;
    case LTR390_RESOLUTION_18BIT: Serial.println(18); break;
    case LTR390_RESOLUTION_19BIT: Serial.println(19); break;
    case LTR390_RESOLUTION_20BIT: Serial.println(20); break;
  }

  ltr.setThresholds(100, 1000);
  ltr.configInterrupt(true, LTR390_MODE_UVS);
}

const unsigned long eventultra = 1100;
unsigned long previousTimeultra = 0;

void loop(){
  unsigned long currentTimeultra = millis();
  int co2now[10];        //int array for co2 readings
  int co2raw = 0;        //int for raw value of co2
  int co2ppm = 0;        //int for calculated ppm
  int zzz = 0;           //int for averaging

  float  temp = dht.readTemperature();
  float humid = dht.readHumidity();
  float lux = lightMeter.readLightLevel();

  if (currentTimeultra - previousTimeultra >= eventultra) {
  Serial.println("====================START====================");
  Serial.print("Light: ");
  Serial.print(lux);
  Serial.println(" lx");

  Serial.print("Temperature = ");
  Serial.println(temp);
  Serial.print("Humidity = ");
  Serial.println(humid);

  if (ltr.newDataAvailable()) {
    Serial.print("UV data: "); 
    Serial.println(ltr.readUVS());
    lcd.setCursor(0, 0);
    lcd.print("UV : ");
    lcd.print(ltr.readUVS());
  }

   for (int x = 0;x<10;x++)  //samplpe co2 10x over 2 seconds
  {                   
    co2now[x]=analogRead(A0);
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
  Serial.println("====================END====================");


  previousTimeultra = currentTimeultra;
  }

}