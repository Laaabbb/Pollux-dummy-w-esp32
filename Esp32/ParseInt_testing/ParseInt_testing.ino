#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define RXp2 16
#define TXp2 17

int test = 0;

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  display.clearDisplay();
}
void loop() {
    Serial.println("Message Received: ");
    while(Serial2.available()>0){
      test = Serial2.parseInt();
      Serial.println(test);
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,28);
      display.println(test);
      display.display();
      display.clearDisplay();
      delay(1000);
    }
    //Serial.println(Serial2.readString());
}