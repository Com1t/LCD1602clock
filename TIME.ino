#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal.h>

#define pinDHT22 9
#define I2C_ADDRESS 0x68
#define NubberOfFields 7

DHT dht(pinDHT22, DHT22);
LiquidCrystal lcd( 8, 7, 6, 5, 4, 3);                  // RS ,En ,D4 ,D5 ,D6 ,D7
/*LCD R/W pin to ground
  A variable resistor ends to +5V and ground
  wiper to LCD Vo pin (pin3)*/

const char week[7][4] = { "MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN" };
const char month[12][4] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };
uint16_t y;                                     // 年
uint8_t m, d, w, h, mi, s;                      // 月/日/週/時/分/秒

uint8_t bcdTodec(uint8_t val);
void getTime();
void digitalClockDisplay();
void TempHumiDisplay();

void setup() {
  Wire.begin();

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Now loading");
  delay(500);
  
  for (uint8_t i = 13; i >= 13 && i <= 15; i++) {
    lcd.setCursor(i, 0);
    lcd.print(".");
    delay(500);
  }
  delay(400);

  lcd.clear();
  lcd.print("READ SENSOR");
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    lcd.setCursor(11, 1);
    lcd.print("ERROR");
    delay(1000);
  }
  else
  {
    lcd.setCursor(14, 1);
    lcd.print("OK");
    delay(1000);
  }
}
  void loop() {
    boolean dhtBotton = digitalRead(10);
    getTime();               // 取得時間
    
    digitalClockDisplay(); // 顯示時間

    if (dhtBotton == HIGH) {
      for (int positionCounter = 0; positionCounter <= 16; positionCounter++) {
        // scroll one position left:
        lcd.scrollDisplayLeft();
        // wait a bit:
        delay(200);
      }
    
    TempHumiDisplay();
      
      for (int positionCounter = 0; positionCounter <= 16; positionCounter++) {
        // scroll one position left:
        lcd.scrollDisplayLeft();
        // wait a bit:
        delay(200);
      }
      boolean DHT11Botton = LOW;
    }
    delay(1000);
  }
  
/*BCD 轉 DEC*/
uint8_t bcdTodec(uint8_t val) {
  return ((val / 16 * 10) + (val % 16));
}

/*自DS1307 or DS3231 取得時間*/
void getTime() {
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDRESS, NubberOfFields);

  s = bcdTodec(Wire.read() & 0x7f);
  mi = bcdTodec(Wire.read());
  h = bcdTodec(Wire.read() & 0x7f);
  w = bcdTodec(Wire.read());
  d = bcdTodec(Wire.read());
  m = bcdTodec(Wire.read());
  y = bcdTodec(Wire.read()) + 2000;
}

/*顯示時間*/
void digitalClockDisplay() {
  m -= 1;
  if (m >= 0 && m <= 11) {
    for (uint8_t f = 0; f < 3; f++) {
      lcd.print(month[m][f]);
    }
  }
  lcd.print(" ");
  lcd.print(d);
  lcd.print(" ");
  lcd.print(y);
  lcd.setCursor(12, 0);
  w -= 1;
  if (w >= 0 && w <= 6) {
    for (uint8_t e = 0; e < 3; e++) {
      lcd.print(week[w][e]);
    }
  }
  lcd.setCursor(4, 1);

  if (h < 10) {
    lcd.print("0");
  }
  lcd.print(h);
  
  lcd.print(":");
  
  if (mi < 10) {
    lcd.print("0");
  }
  lcd.print(mi);
  
  lcd.print(":");
  
  if (s < 10) {
    lcd.print("0");
  }
  lcd.print(s);
  
}

/*顯示溫濕度*/
void TempHumiDisplay(){
  float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      lcd.clear();
      if (isnan(h) || isnan(t)) {
        lcd.println("Read DHT22 failed");
        lcd.setCursor(11, 1);
        lcd.print("ERROR");
        delay(1000);
      } 
    else {
        lcd.print("Temperature");
        lcd.setCursor(12, 0);
        lcd.print((int)t);
        lcd.print((char)0xDF);
        lcd.print("C");
        lcd.setCursor(0, 1);
        lcd.print("Humidity");
        lcd.setCursor(12, 1);
        lcd.print((int)h);
        lcd.setCursor(15, 1);
        lcd.print("%");
        delay(2000);
      }
}