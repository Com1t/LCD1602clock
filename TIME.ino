#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>

const char *ssid     = "ASUS-G";
const char *password = "0922235662";

const byte DS1307_I2C_ADDRESS = 0x68; // DS1307 (I2C) 地址
const byte NumberOfFields = 7;        // DS1307 (I2C) 資料範圍

int y; // 年
byte m, d, w, h, mi, s; // 月/日/週/時/分/秒 
// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Wire.begin();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(28800);
  
  setTime();
}

void loop() {
  timeClient.update();
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);
  
  getTime(); // 取得時間
  digitalClockDisplay(); // 顯示時間 
  
  delay(1000);
}

// BCD 轉 DEC
byte bcdTodec(byte val){
 return ((val / 16 * 10) + (val % 16));
}

// DEC 轉 BCD
byte decToBcd(byte val){
 return ((val / 10 * 16) + (val % 10));
} 

// 設定時間
void setTime(){
  timeClient.update();
  unsigned long rawTime = timeClient.getEpochTime() / 86400L;  // in days
  unsigned long days = 0, year = 1970;
  uint8_t month;
  static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
  
  while((days += (LEAP_YEAR(year) ? 366 : 365)) <= rawTime)
    year++;
  rawTime -= days - (LEAP_YEAR(year) ? 366 : 365); // now it is days in this year, starting at 0
  days=0;
  for (month=0; month<12; month++) {
    uint8_t monthLength;
    if (month==1) { // february
      monthLength = LEAP_YEAR(year) ? 29 : 28;
    } else {
      monthLength = monthDays[month];
    }
    if (rawTime < monthLength) break;
    rawTime -= monthLength;
  }
  
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(decToBcd(timeClient.getSeconds()));
  Wire.write(decToBcd(timeClient.getMinutes()));
  Wire.write(decToBcd(timeClient.getHours()));
  uint8_t week = timeClient.getDay();
  if( week == 0 )
    week = 7;
  Wire.write(decToBcd(week));
  Wire.write(decToBcd(++rawTime));
  Wire.write(decToBcd(++month));
  Wire.write(decToBcd(year-2000));
  Serial.println(year);
  Wire.endTransmission();
}

// 取得時間
void getTime(){
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  
  Wire.requestFrom(DS1307_I2C_ADDRESS, NumberOfFields);
  
  s  =  bcdTodec(Wire.read() & 0x7f);
  mi =  bcdTodec(Wire.read());
  h  =  bcdTodec(Wire.read() & 0x7f);
  w  =  bcdTodec(Wire.read());
  d  =  bcdTodec(Wire.read());
  m  =  bcdTodec(Wire.read());
  y  =  bcdTodec(Wire.read()) + 2000;
} 

// 顯示時間
void digitalClockDisplay(){
  Serial.print(y);
  Serial.print("/");
  Serial.print(m);
  Serial.print("/");
  Serial.print(d);
  Serial.print(" ( ");
  Serial.print(w);
  Serial.print(" ) ");
  Serial.print(h);
  Serial.print(":");
  Serial.print(mi);
  Serial.print(":");
  Serial.println(s);
}
