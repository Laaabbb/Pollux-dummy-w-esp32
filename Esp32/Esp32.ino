/*
  ESP32 I2C Slave Demo
*/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

#define WIFI_SSID "Labro"
#define WIFI_PASSWORD "OedipusElectra1993"

//Provide the token generation process info.
#include "addons/TokenHelper.h"

//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Defines the Digital Pin of the "On Board LED".
#define On_Board_LED 2

// Insert Firebase project API Key
#define API_KEY "AIzaSyBOHSeBCviQEJHEREcpAYDAXI36tciFJlE"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp32-testing-959e3-default-rtdb.asia-southeast1.firebasedatabase.app/" 

#define slaveAddress 9  //you have to assign an 8-bit address to Slave

FirebaseData fbdo;

// Define firebase authentication.
FirebaseAuth auth;

// Definee firebase configuration.
FirebaseConfig config;

//======================================== Millis variable to send/store data to firebase database.
unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 10000; //--> Sends/stores data to firebase database every 10 seconds.
//======================================== 

// Boolean variable for sign in status.
bool signupOK = false;

float Lux;

byte dataArray[4];
float lux, uv, temp, humid, co2ppm = 0;

void setup()
{
  Wire.begin(slaveAddress);
  Serial.begin(9600);
  Wire.onReceive(receiveEvent);//you need to declre it in setup() to receive data from Master
  //-----------------------------------

  //---------------------------------------- The process of connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("---------------Connection");
  Serial.print("Connecting to : ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");

    digitalWrite(On_Board_LED, HIGH);
    delay(250);
    digitalWrite(On_Board_LED, LOW);
    delay(250);
  }
  
  // Assign the api key (required).
  config.api_key = API_KEY;

  // Assign the RTDB URL (required).
  config.database_url = DATABASE_URL;

  // Sign up.
  Serial.println();
  Serial.println("---------------Sign up");
  Serial.print("Sign up new user... ");
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Serial.println("---------------");
  
  // Assign the callback function for the long running token generation task.
  config.token_status_callback = tokenStatusCallback; //--> see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

const unsigned long eventultra = 1100;
unsigned long previousTimeultra = 0;

void loop()
{
  unsigned long currentTimeultra = millis();
  if (currentTimeultra - previousTimeultra >= eventultra) {
   if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    Lux = Serial2.parseFloat();

    Serial.println();
    Serial.println("---------------Store Data");
    digitalWrite(On_Board_LED, HIGH);
    
    // Write an Int number on the database path test/random_Float_Val.
    //Lux
    if (Firebase.RTDB.setFloat(&fbdo, "Test/Lux", lux)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    //UV
    if (Firebase.RTDB.setFloat(&fbdo, "Test/UV", uv)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    //Temp
    if (Firebase.RTDB.setFloat(&fbdo, "Test/Temperature", temp)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    //Humidity
    if (Firebase.RTDB.setFloat(&fbdo, "Test/Humidity", humid)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
  }
}

void receiveEvent(int howmany) //howmany = Wire.write()executed by Master
{

  //lux, 
  uv, temp, humid, co2ppm = 0;

  for(int i=0; i<howmany; i++)
  {
    dataArray[i] = Wire.read();
    if(i == 0){
      lux = dataArray[i];
    }
    else if(i == 1){
      uv = dataArray[i];
    }
    else if(i == 2){
      temp = dataArray[i];
    }
    else if(i == 3){
      humid = dataArray[i];
    }
    /*else if(i == 4){
      co2ppm = dataArray[i];
    }
    if(i == 0){
      uv = dataArray[i];
    }
    else if(i == 1){
      temp = dataArray[i];
    }
    else if(i == 2){
      humid = dataArray[i];
    }
    else if(i == 3){
      co2ppm  = dataArray[i];
    }*/

    Serial.println("===Array Start===");
    Serial.println(lux);
    Serial.println(uv);
    Serial.println(temp);
    Serial.println(humid);
    //Serial.println(co2ppm);
    Serial.println("===Array End===");
  }
}