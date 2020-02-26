#include <LiquidCrystal_I2C.h>
#include <dht11.h>
#include <Wire.h>

char week[7][4]={"MON","TUE","WED","THU","FRI","SAT","SUN"};
const byte DS1307_I2C_ADDRESS = 0x68; // DS1307 (I2C) 地址
const byte NubberOfFields = 7;        // DS1307 (I2C) 資料範圍 
int y;                                // 年
byte m, d, w, h, mi, s;               // 月/日/週/時/分/秒 
int DHT11signal = 10;
int measurePin = A0;
int ledPower = 2;
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
float A0measured = 0;
float dustDensity = 0;

LiquidCrystal_I2C lcd(0x3F,2,1,0,4,5,6,7,3,POSITIVE);

dht11 DHT11;

void nowLoading(){
  int i =14;
  lcd.print("Now loading");
  delay(500);
   while(i>=14&&i<=16){
    lcd.setCursor(i,0);
    lcd.print(".");
    delay(500);
    i++;
   }
}

void sensorCheck(){
  lcd.print(" READ SENSOR ");
  int chk = DHT11.read(10);
   switch (chk)
   {
    case 0:
    lcd.print("OK");
    break;
    default: 
    lcd.print(" ERROR");
    break;
   }
}

// BCD 轉 DEC
byte bcdTodec(byte val){
     return ((val / 16 * 10) + (val % 16));
} 

void getTime(){
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
void digitalClockDisplay(){
     lcd.print(y);
     lcd.print("/");
     lcd.print(m);
     lcd.print("/");
     lcd.print(d);
     lcd.setCursor(11,0);
     byte we = w - 1;
     if(we>=0&&we<=6){
      for(byte e = 0;e<3;e++){
        lcd.print(week[we][e]);
       }
     }
     lcd.setCursor(15,0);
     lcd.print(h);
     lcd.print(":");
     lcd.print(mi);
}

void dustMeasurement(){
  digitalWrite(ledPower,LOW);          // power on the LED
  delayMicroseconds(samplingTime);
  A0measured = analogRead(measurePin); // read the dust value
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH);         // turn the LED off
  delayMicroseconds(sleepTime);
    
  dustDensity = A0measured * 0.17 * (5.0 / 1024.0) - 0.1;
}

void setup(){
  pinMode(ledPower,OUTPUT);
  Wire.begin();
  lcd.begin(20,4);
  lcd.clear(); // 設定游標位置在第一行行首
  lcd.setCursor(3,0);

  nowLoading();   

  lcd.clear();

  sensorCheck();

  delay(500);
  
}

 void loop(){
  int chk = DHT11.read(10);
  
  dustMeasurement();
  
  lcd.clear();
  getTime(); // 取得時間
  digitalClockDisplay(); // 顯示時間
 
  lcd.setCursor(2,1);
  lcd.print("Temperature ");
  lcd.print((int)DHT11.temperature);
  lcd.print((char)0xDF);
  lcd.print("C");
  
  lcd.setCursor(3,2);
  lcd.print("Humidity   ");
  lcd.print((int)DHT11.humidity);
  lcd.print("%");
  
  lcd.setCursor(1,3);
  lcd.print("PM2.5  ");
  lcd.print(dustDensity*1000);
  lcd.print("ug/m3 ");
  delay(1000);
  }