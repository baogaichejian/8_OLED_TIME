//抖音爆改车间主任
//OLED矩阵时钟
//关注"爆改车间"微信公众号获取更多代码
//在下方代码中输入Wi-Fi信息后烧录程序
//s1键更换字体风格。
//共享代码，转发保留作者，新手勤搜索，高手多指教。

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Arduino.h>
#include <U8g2lib.h>    //OLED驱动库
#include <DS3231.h>     //DS3231时钟模块驱动库
#include <Ticker.h>     //定时器库

#include "num.h"

Ticker flipper;        //创建定时器

#define SW1 D5         //定义按键管脚
#define SW2 D6
#define SW3 D7

DS3231 Clock;         //创建时钟实例
bool Century=false;
bool h12;
bool PM;

#ifndef STASSID
#define STASSID "********"      //输入Wi-Fi名
#define STAPSK  "********"      //输入Wi-Fi密码
#endif
const char* ssid     = STASSID;
const char* password = STAPSK;

//-----------
#include <TZ.h>
#include <ESPPerfectTime.h>      //NTP网络时间服务器获取库
const char *ntpServer = "ntp.sjtu.edu.cn";     //上海交通大学NTP服务器

void printTime(struct tm *tm, suseconds_t usec) {
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d.%06ld\n",
                tm->tm_year + 1900,
                tm->tm_mon + 1,
                tm->tm_mday,
                tm->tm_hour,
                tm->tm_min,
                tm->tm_sec,
                usec);
}

uint8_t hh,mm,ss;
uint8_t h_now,m_now,s_now;
uint8_t modenum = 0;
void gettime(){        //定时器定时器获取网络时间
    struct tm *tm = pftime::localtime(nullptr);

    int w_mon ;
    if(tm->tm_mon == 1)w_mon=13;
    else if (tm->tm_mon == 2)w_mon=14;
    else w_mon=tm->tm_mon;
    Serial.printf("%02d:%02d:%02d %02d\n",
                tm->tm_hour,
                tm->tm_min,
                tm->tm_sec,
                ((tm->tm_year%100)+((tm->tm_year%100)/4)+(((tm->tm_year+1900)/100)/4)-2*((tm->tm_year+1900)/100)+(26*(w_mon+1+1)/10)+tm->tm_mday-1)%7
                );
    hh = tm->tm_hour;
    mm = tm->tm_min;
    ss = tm->tm_sec;
    if(WiFi.status() == WL_CONNECTED and tm->tm_year != 70){
      if(Clock.getYear() !=tm->tm_year)Clock.setYear(tm->tm_year+1900);
      if(Clock.getMonth(Century) !=tm->tm_mon)Clock.setMonth(tm->tm_mon);
      if(Clock.getDate() !=tm->tm_year)Clock.setDate(tm->tm_mday);
      //Clock.setDoW(DoW);
      if(Clock.getHour(h12, PM) != tm->tm_hour)Clock.setHour(tm->tm_hour);
      if(Clock.getMinute() != tm->tm_min)Clock.setMinute(tm->tm_min);
      if(Clock.getSecond() != tm->tm_sec)Clock.setSecond(tm->tm_sec);
    }
    
}
//-----------
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R1, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);  
//创建OLED实例

 
void TCA9548A(uint8_t bus)     //TCA9548通道选择子函数
{
  Wire.beginTransmission(0x70);  // TCA9548A address is 0x70
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
}

#define num_x 32
#define num_y 128



void displaytime0(int h,int m,int s){         //时间显示子函数
  TCA9548A(0);
  u8g2.drawXBMP(0,0, num_x, num_y, num0[h/10]);
  u8g2.sendBuffer();
  TCA9548A(1);
  u8g2.drawXBMP(0,0, num_x, num_y, num0[h%10]);
  u8g2.sendBuffer();

  if( s&1 == 1){
    TCA9548A(2);
    u8g2.drawXBMP(0,0, num_x, num_y, num0[10]);
    u8g2.sendBuffer();
    TCA9548A(5);
    u8g2.drawXBMP(0,0, num_x, num_y, num0[10]);
    u8g2.sendBuffer();
  }else{
    TCA9548A(2);
    u8g2.clearDisplay();

    TCA9548A(5);
    u8g2.clearDisplay();
  }
  TCA9548A(3);
  u8g2.drawXBMP(0,0, num_x, num_y, num0[m/10]);
  u8g2.sendBuffer();
  TCA9548A(4);
  u8g2.drawXBMP(0,0, num_x, num_y, num0[m%10]);
  u8g2.sendBuffer();
  TCA9548A(6);
  u8g2.drawXBMP(0,0, num_x, num_y, num0[s/10]);
  u8g2.sendBuffer();
  TCA9548A(7);
  u8g2.drawXBMP(0,0, num_x, num_y, num0[s%10]);
  u8g2.sendBuffer();
}

void displaytime1(int h,int m,int s){
  TCA9548A(0);
  u8g2.drawXBMP(0,0, num_x, num_y, num1[h/10]);
  u8g2.sendBuffer();
  TCA9548A(1);
  u8g2.drawXBMP(0,0, num_x, num_y, num1[h%10]);
  u8g2.sendBuffer();

  if( s&1 == 1){
    TCA9548A(2);
    u8g2.drawXBMP(0,0, num_x, num_y, num1[10]);
    u8g2.sendBuffer();
    TCA9548A(5);
    u8g2.drawXBMP(0,0, num_x, num_y, num1[10]);
    u8g2.sendBuffer();
  }else{
    TCA9548A(2);
    u8g2.clearDisplay();

    TCA9548A(5);
    u8g2.clearDisplay();
  }
  TCA9548A(3);
  u8g2.drawXBMP(0,0, num_x, num_y, num1[m/10]);
  u8g2.sendBuffer();
  TCA9548A(4);
  u8g2.drawXBMP(0,0, num_x, num_y, num1[m%10]);
  u8g2.sendBuffer();
  TCA9548A(6);
  u8g2.drawXBMP(0,0, num_x, num_y, num1[s/10]);
  u8g2.sendBuffer();
  TCA9548A(7);
  u8g2.drawXBMP(0,0, num_x, num_y, num1[s%10]);
  u8g2.sendBuffer();
}

void displaytime2(int h,int m,int s){
  TCA9548A(0);
  u8g2.drawXBMP(0,0, num_x, num_y, num2[h/10]);
  u8g2.sendBuffer();
  TCA9548A(1);
  u8g2.drawXBMP(0,0, num_x, num_y, num2[h%10]);
  u8g2.sendBuffer();

  if( s&1 == 1){
    TCA9548A(2);
    u8g2.drawXBMP(0,0, num_x, num_y, num2[10]);
    u8g2.sendBuffer();
    TCA9548A(5);
    u8g2.drawXBMP(0,0, num_x, num_y, num2[10]);
    u8g2.sendBuffer();
  }else{
    TCA9548A(2);
    u8g2.clearDisplay();

    TCA9548A(5);
    u8g2.clearDisplay();
  }
  TCA9548A(3);
  u8g2.drawXBMP(0,0, num_x, num_y, num2[m/10]);
  u8g2.sendBuffer();
  TCA9548A(4);
  u8g2.drawXBMP(0,0, num_x, num_y, num2[m%10]);
  u8g2.sendBuffer();
  TCA9548A(6);
  u8g2.drawXBMP(0,0, num_x, num_y, num2[s/10]);
  u8g2.sendBuffer();
  TCA9548A(7);
  u8g2.drawXBMP(0,0, num_x, num_y, num2[s%10]);
  u8g2.sendBuffer();
}

void displaytime3(int h,int m,int s){
  TCA9548A(0);
  u8g2.drawXBMP(0,0, num_x, num_y, num3[h/10]);
  u8g2.sendBuffer();
  TCA9548A(1);
  u8g2.drawXBMP(0,0, num_x, num_y, num3[h%10]);
  u8g2.sendBuffer();

  if( s&1 == 1){
    TCA9548A(2);
    u8g2.drawXBMP(0,0, num_x, num_y, num3[10]);
    u8g2.sendBuffer();
    TCA9548A(5);
    u8g2.drawXBMP(0,0, num_x, num_y, num3[10]);
    u8g2.sendBuffer();
  }else{
    TCA9548A(2);
    u8g2.clearDisplay();

    TCA9548A(5);
    u8g2.clearDisplay();
  }
  TCA9548A(3);
  u8g2.drawXBMP(0,0, num_x, num_y, num3[m/10]);
  u8g2.sendBuffer();
  TCA9548A(4);
  u8g2.drawXBMP(0,0, num_x, num_y, num3[m%10]);
  u8g2.sendBuffer();
  TCA9548A(6);
  u8g2.drawXBMP(0,0, num_x, num_y, num3[s/10]);
  u8g2.sendBuffer();
  TCA9548A(7);
  u8g2.drawXBMP(0,0, num_x, num_y, num3[s%10]);
  u8g2.sendBuffer();
}


void displaytime4(int h,int m,int s){
  TCA9548A(0);
  u8g2.drawXBMP(0,0, num_x, num_y, num4[h/10]);
  u8g2.sendBuffer();
  TCA9548A(1);
  u8g2.drawXBMP(0,0, num_x, num_y, num4[h%10]);
  u8g2.sendBuffer();

  if( s&1 == 1){
    TCA9548A(2);
    u8g2.drawXBMP(0,0, num_x, num_y, num4[10]);
    u8g2.sendBuffer();
    TCA9548A(5);
    u8g2.drawXBMP(0,0, num_x, num_y, num4[10]);
    u8g2.sendBuffer();
  }else{
    TCA9548A(2);
    u8g2.clearDisplay();

    TCA9548A(5);
    u8g2.clearDisplay();
  }
  TCA9548A(3);
  u8g2.drawXBMP(0,0, num_x, num_y, num4[m/10]);
  u8g2.sendBuffer();
  TCA9548A(4);
  u8g2.drawXBMP(0,0, num_x, num_y, num4[m%10]);
  u8g2.sendBuffer();
  TCA9548A(6);
  u8g2.drawXBMP(0,0, num_x, num_y, num4[s/10]);
  u8g2.sendBuffer();
  TCA9548A(7);
  u8g2.drawXBMP(0,0, num_x, num_y, num4[s%10]);
  u8g2.sendBuffer();
}
void setup() {
  Serial.begin(115200);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  Wire.begin();
  for(int i = 0 ;i<8;i++){
  TCA9548A(i);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.clearBuffer();
  } 

  for(int s=0;s<6;s++){
    for(int i = 0 ;i<8;i++){
    TCA9548A(i);
    u8g2.setFont(u8g2_font_nokiafc22_tf);
    switch(s){
      case 0:
        u8g2.setCursor(0, 9);
        u8g2.print("setup");
        break;
      case 1:
        u8g2.setCursor(0, 18);
        u8g2.print("OLED");
        break;
      case 2:
        u8g2.setCursor(0, 27);
        u8g2.print("OK!");
        break;
      case 3:
        u8g2.setCursor(0, 36);
        u8g2.print("Wi-Fi");
        break;
      case 4:
        u8g2.setCursor(0, 45);
        u8g2.print("connecting");
        break;
      case 5:
        u8g2.setCursor(0, 54);
        u8g2.print(".");
        break;
    }

    u8g2.sendBuffer(); 
    delay(50);  
    }
  }
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int t = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    for(int i = 0 ;i<8;i++){
      TCA9548A(i);
      u8g2.setCursor(t+2, 54);
      u8g2.print(".");
      u8g2.sendBuffer();
      delay(10);
    }
    t++; 
    if(t==20)break;
  }
  if(t<20){
    for(int i = 0 ;i<8;i++){
      TCA9548A(i);
      u8g2.setCursor(0, 63);
      u8g2.print("succeeded");
      u8g2.sendBuffer();
      delay(10);
    }
  }else{
    for(int i = 0 ;i<8;i++){
      TCA9548A(i);
      u8g2.setCursor(0, 63);
      u8g2.print("failed");
      u8g2.sendBuffer();
      delay(10);
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  pftime::configTzTime(TZ_Asia_Shanghai, ntpServer);
  delay(2000);
  flipper.attach(10, gettime);
  
}

void loop() {
  if(digitalRead(SW1)==LOW){
    delay(5);
    if(digitalRead(SW1)==LOW){
      modenum++;
      if(modenum==5)modenum=0;
      while(digitalRead(SW1)==LOW);
    }
  }
  
  //displaytime(hh,mm,ss);
  if(h_now != Clock.getHour(h12, PM) || m_now != Clock.getMinute() || s_now != Clock.getSecond()){
    switch(modenum){
      case 0:
        displaytime0(Clock.getHour(h12, PM),Clock.getMinute(),Clock.getSecond());
        break;
      case 1:
        displaytime1(Clock.getHour(h12, PM),Clock.getMinute(),Clock.getSecond());
        break;
      case 2:
        displaytime2(Clock.getHour(h12, PM),Clock.getMinute(),Clock.getSecond());
        break;
      case 3:
        displaytime3(Clock.getHour(h12, PM),Clock.getMinute(),Clock.getSecond());
        break;
      case 4:
        displaytime4(Clock.getHour(h12, PM),Clock.getMinute(),Clock.getSecond());
        break;
    }
    h_now = Clock.getHour(h12, PM);
    m_now = Clock.getMinute();
    s_now = Clock.getSecond();
  }
  
  delay(100);

}
