#include <Wire.h>
#include <dht11.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd( 8, 7, 6, 5, 4, 3);
int DHT11pin = 9;
dht11 DHT11;

char week[7][4] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
char month[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

const byte DS1307_I2C_ADDRESS = 0x68; // DS1307 (I2C) 地址
const byte NubberOfFields = 7;        // DS1307 (I2C) 資料範圍

int y;                                // 年
byte m, d, w, h, mi, s;               // 月/日/週/時/分/秒


// BCD 轉 DEC
byte bcdTodec(byte val) {
  return ((val / 16 * 10) + (val % 16));
}

void getTime() {
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(DS1307_I2C_ADDRESS, NubberOfFields);

  s = bcdTodec(Wire.read() & 0x7f);
  mi = bcdTodec(Wire.read());
  h = bcdTodec(Wire.read() & 0x7f);
  w = bcdTodec(Wire.read());
  d = bcdTodec(Wire.read());
  m = bcdTodec(Wire.read());
  y = bcdTodec(Wire.read()) + 2000;
}

// 顯示時間
void digitalClockDisplay() {
  byte me = m - 1;
  if (me >= 0 && me <= 11) {
    for (byte f = 0; f < 3; f++) {
      lcd.print(month[me][f]);
    }
  }
  lcd.print(" ");
  lcd.print(d);
  lcd.print(" ");
  lcd.print(y);
  lcd.setCursor(12, 0);
  byte we = w - 1;
  if (we >= 0 && we <= 6) {
    for (byte e = 0; e < 3; e++) {
      lcd.print(week[we][e]);
    }
  }
  lcd.setCursor(4, 1);
  lcd.print(h);
  lcd.print(":");
  lcd.print(mi);
  lcd.print(":");
  lcd.print(s);
}


void setup() {
  lcd.begin(16, 2);

  Wire.begin();
  int i = 13;
  lcd.clear(); // 設定游標位置在第一行行首
  lcd.setCursor(2, 0);
  lcd.print("Now loading");
  delay(500);
  while (i >= 13 && i <= 15) {
    lcd.setCursor(i, 0);
    lcd.print(".");
    delay(500);
    i++;
  }
  delay(400);
  lcd.clear();
  lcd.print("READ SENSOR");
  int chk = DHT11.read(DHT11pin);
  lcd.setCursor(11, 1);
  switch (chk)
  {
    case 0:
      lcd.print("   OK");
      break;
    default:
      lcd.print("ERROR");
      break;
  }
  delay(1000);
}

void loop() {
  int chk = DHT11.read(9);
  boolean DHT11Botton = digitalRead(10);

  lcd.clear();
  getTime(); // 取得時間
  digitalClockDisplay(); // 顯示時間
  
  if (DHT11Botton == HIGH) {

    for (int positionCounter = 0; positionCounter <= 16; positionCounter++) {
      // scroll one position left:
      lcd.scrollDisplayLeft();
      // wait a bit:
      delay(200);
    }
    lcd.clear();
    lcd.print("Temperature ");
    lcd.print((int)DHT11.temperature);
    lcd.print((char)0xDF);
    lcd.print("C");
    lcd.setCursor(0, 1);
    lcd.print("Humidity    ");
    lcd.print((int)DHT11.humidity);
    lcd.print(" %");
    delay(2000);

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