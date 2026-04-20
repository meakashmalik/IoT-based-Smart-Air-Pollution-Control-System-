/************************************************************
 SMART AIR POLLUTION CONTROL (CALIBRATED VERSION)
 ESP8266 + DHT11 + MQ2 + OLED + BLYNK WEB
 FIXES:
 Better MQ2 thresholds ✔
 Stable readings ✔
 Correct OLED output ✔
 Blynk values proper ✔
************************************************************/
#define BLYNK_TEMPLATE_ID "TMPL38khFGPon"
#define BLYNK_TEMPLATE_NAME "Smart Air Pollution Control"
#define BLYNK_AUTH_TOKEN "hdr321Ht6TLmpMc3xhN5euPZo5ecC4RJtfJrJu8"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//---------------- WIFI ----------------
char ssid[] = "Your_Wifi_Name_Here";
char pass[] = "Wifi_Password";
//---------------- DHT -----------------
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
//---------------- MQ2 -----------------
#define MQ2_PIN A0
//---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//---------------- TIMER ---------------
BlynkTimer timer;
//======================================
void bootScreen()
{
 display.clearDisplay();
 display.setTextSize(2);
 display.setTextColor(SSD1306_WHITE);
 display.setCursor(0,0);
 display.println("SMART");
 display.setTextSize(1);
 display.setCursor(0,22);
 display.println("AIR CONTROL");
 display.display();
 delay(2000);
}
//======================================
void sendData()
{
 float h = dht.readHumidity();
 float t = dht.readTemperature();
 // Read MQ2 multiple times for stable average
 int gas = 0;
 for(int i=0; i<10; i++)
 {
 gas += analogRead(MQ2_PIN);
 delay(10);
 }
 gas = gas / 10;
 if (isnan(h) || isnan(t))
 {
 Serial.println("DHT Error");
 return;
 }
 String statusText;
 // Calibrated thresholds for your 850 reading room
 if (gas < 780)
 statusText = "GOOD";
 else if (gas < 860)
 statusText = "NORMAL";
 else if (gas < 930)
 statusText = "BAD";
 else
 statusText = "DANGER";
 //------------- BLYNK ---------------
 Blynk.virtualWrite(V0, t);
 Blynk.virtualWrite(V1, h);
 Blynk.virtualWrite(V2, gas);
 Blynk.virtualWrite(V3, statusText);
 if(gas > 930)
 {
 Blynk.logEvent("alert", "Danger! High Pollution Detected!");
 }
 //------------- OLED ----------------
 display.clearDisplay();
 display.setTextSize(1);
 display.setTextColor(SSD1306_WHITE);
 display.setCursor(0,0);
 display.print("T:");
 display.print(t,0);
 display.print("C ");
 display.print("H:");
 display.print(h,0);
 display.print("%");
 display.setCursor(0,12);
 display.print("Gas:");
 display.print(gas);
 display.setCursor(0,24);
 display.print(statusText);
 display.display();
 //------------- SERIAL --------------
 Serial.print("Temp:");
 Serial.print(t);
 Serial.print(" Hum:");
 Serial.print(h);
 Serial.print(" Gas:");
 Serial.print(gas);
 Serial.print(" Status:");
 Serial.println(statusText);
}
//======================================
void setup()
{
 Serial.begin(115200);
 dht.begin();
 Wire.begin(D2, D1); // SDA, SCL
 if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
 {
 Serial.println("OLED Failed");
 while(1);
 }
 display.clearDisplay();
 display.display();
 bootScreen();
 Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
 timer.setInterval(2000L, sendData);
}
//======================================
void loop()
{
 Blynk.run();
 timer.run();
}