/*
  ESP32 I2C Slave Demo
*/
//======Include Libraries========
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiManager.h>
#include "time.h" 
//Aternative: #include <NTPClient.h>

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"

//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"
//================================

//============Define==============
// Defines the Digital Pin of the "On Board LED".
#define On_Board_LED 2

// Insert Firebase project API Key
#define API_KEY "AIzaSyBOHSeBCviQEJHEREcpAYDAXI36tciFJlE"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://esp32-testing-959e3-default-rtdb.asia-southeast1.firebasedatabase.app/" 

#define slaveAddress 9  //you have to assign an 8-bit address to Slave
//================================

/*===Alternative WiFi Setup for static WiFi Cred===
#include "Network.h"

#define WIFI_SSID "YOURWIFISSIDHERE"
#define WIFI_PASSWORD "YOURWIFIPASSWORDHERE"
//===============================*/

//======Firebase setup============
//Define firebase data
FirebaseData fbdo;
// Define firebase authentication.
FirebaseAuth auth;
// Define firebase configuration.
FirebaseConfig config;

// Parent Node (to be updated in every loop)
String parentPath;

FirebaseJson json;
//================================

//========Variables init==========
//Millis variable to send/store data to firebase database
unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 1000; //--> Sends/stores data to firebase database every 1 second

// Boolean variable for sign in status
bool signupOK = false;

//Global Variables
int value = 0;
int dataArray[8];
int lux, uv, temp, humid, co2;
byte l1, l2, u1, u2, c1, c2;

//Init var for time
const char* ntpServer = "ph.pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 28800;//GMT+8:00
char Time[50];
char Date[50];
int timestamp;
//================================

void setup()
{
  Serial.begin(9600);
  Wire.begin(slaveAddress);
  Wire.onReceive(receiveEvent);//you need to declre it in setup() to receive data from Master
  
  //==========WiFi Setup==========
  //The process of connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
  WiFiManager wm;
  //wm.resetSettings();

   bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("Pollux_Dummy","password"); // password protected ap
 
    if(!res) {
        Serial.println("Failed to connect");
        ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected... :)");
    }
  //================================

  /*Alternative for static WiFi setup
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
    }*/
  
  // Assign the api key (required).
  config.api_key = API_KEY;

  // Assign the RTDB URL (required).
  config.database_url = DATABASE_URL;

  // Sign up validation.
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

  //Firebase init
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  fetchLocalTime();

  //timeClient.begin();
  // Update database path
  //databasePath = "/UsersData/" + uid + "/readings";
  //databasePath = "/Pollux1/readings/"
}

void loop()
{
  //unsigned long currentTimeultra = millis();
  //int second = 0;
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    /*
    do{
    second++;
    }while(second != 3600);*/

    Serial.println();
    Serial.println("---------------Store Data");
    digitalWrite(On_Board_LED, HIGH);

    fetchLocalTime();
    writeFirebase();
  }
}

void receiveEvent(int howmany) //howmany = Wire.write()executed by Master
{
  value = howmany;
}

/*// Function that gets current epoch time
unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}*/

void fetchLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  } 
  strftime(Time, sizeof(Time), "%H:%M:%S", &timeinfo);
  //String asString(time);
  strftime(Date, sizeof(Date), "%B %d %Y", &timeinfo);
  //String asString(date);
  //Serial.println(Time);
  //Serial.println(Date);
}

void insertSerialCommData(){
  lux, uv, temp, humid, co2 = 0;

  //Write data
  for(int i=0; i<value; i++)
  {
    dataArray[i] = Wire.read();
    if(i == 0){
      temp = dataArray[i];
    }
    else if(i == 1){
      humid = dataArray[i];
    }
    else if(i == 2){
      l1  = dataArray[i];
    }
    else if(i == 3){
      l2  = dataArray[i];
    }
    else if(i == 4){
      u1  = dataArray[i];
    }
    else if(i == 5){
      u2  = dataArray[i];
    }
    else if(i == 6){
      c1  = dataArray[i];
    }
    else if(i == 7){
      c2  = dataArray[i];
    }
  }

    co2 = ((int)c1) * 256 + c2;
    lux = ((int)l1) * 256 + l2;
    uv = ((int)u1) * 256 + u2;

    //Serial monitor print
    Serial.println("===Array Start===");
    Serial.println(lux);
    Serial.println(uv);
    Serial.println(temp);
    Serial.println(humid);
    Serial.println(co2);
    Serial.println("===Array End===");

}

void writeFirebase(){
  insertSerialCommData();
  //Lux
  if (Firebase.RTDB.setFloat(&fbdo, "Pollux1/Lux", lux)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }

  //UV
  if (Firebase.RTDB.setFloat(&fbdo, "Pollux1/UV", uv)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }

  //Temp
  if (Firebase.RTDB.setFloat(&fbdo, "Pollux1/Temperature", temp)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }

  //Humidity
  if (Firebase.RTDB.setFloat(&fbdo, "Pollux1/Humidity", humid)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }

  //Co2
  if (Firebase.RTDB.setFloat(&fbdo, "Pollux1/Carbon Dioxide", co2)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }

  //Time
  if (Firebase.RTDB.setString(&fbdo, "Pollux1/Timestamp", Time)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }

  //Date
  if (Firebase.RTDB.setString(&fbdo, "Pollux1/Date", Date)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

/*void writeFirebase1(){
    insertSerialCommData()
  //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

    parentPath= databasePath + "/" + String(timestamp);

    json.set(tempPath.c_str(), String(bme.readTemperature()));
    json.set(humPath.c_str(), String(bme.readHumidity()));
    json.set(presPath.c_str(), String(bme.readPressure()/100.0F));
    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
}*/