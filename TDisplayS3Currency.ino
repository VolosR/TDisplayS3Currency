#include <TFT_eSPI.h>
#include "time.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <vector> 
#include <HTTPClient.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson.git

#include "NotoSansBold36.h"
#include "Latin_Hiragana_24.h"
#include "NotoSansMonoSCB20.h"
#include "Final_Frontier_28.h"
#include "NotoSansBold15.h"
#include "curr.h"
TFT_eSPI tft = TFT_eSPI();  
TFT_eSprite sprite= TFT_eSprite(&tft);

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec =3600;             //time zone * 3600 , my time zone is  +1 GTM
const int   daylightOffset_sec = 3600; 

#define small NotoSansBold15
#define large NotoSansBold36
#define latin Latin_Hiragana_24
#define midle NotoSansMonoSCB20
#define final Final_Frontier_28

#define c1 0xCE79  //colour gray
#define c2 0xAD01 //color yellow
#define c3  0x80C3     //red

int px=10;
int py=10;

const char* ssid     = "xxxxx";       ///EDIIIT
const char* password = "xxxxx";
String date2="";
String payload="";
StaticJsonDocument<6000> doc;
String endpoint2 ="https://cdn.jsdelivr.net/gh/fawazahmed0/currency-api@1/latest/currencies/hrk.min.json";

String cur[11]={"eur","hrk","usd","gbp","aud","cad","chf","jpy","cny","nzd","btc"};
int fromCurrency=0;
int toCurrency=2;
double amount=1;

String big[22]={"EUR","HRK","USD","GBP","AUD","CAD","CHF","JPY","CNY","NZD","BTC"};
String curNames[11]={"Euro","Croatian Kuna","US Dollar","Pound Sterling","Australian Dollar","Canadian Dollar","Swiss Franc","Japanese Yen","Chinese Renminbi","New Zealand Dollar","BitCoin"};

float res;
String ip="";

int deb1=0; //button debounce
int deb2=0; //button debounce

char timeHour[3];
char timeMin[3];
char timeSec[3];

long period=1000;
long currentTime=0;

void setup() {

pinMode(0,INPUT_PULLUP);  
pinMode(14,INPUT_PULLUP);  

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  WiFi.begin(ssid, password);


while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    
   }
  ip=WiFi.localIP().toString();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  sprite.createSprite(320,170);
  sprite.setTextColor(TFT_GREEN,TFT_BLACK);
  sprite.loadFont(midle);
  sprite.setSwapBytes(true);
  getData();
  printLocalTime();
  draw();
}

void getData()
{
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
   HTTPClient http;
   String    endpoint="https://cdn.jsdelivr.net/gh/fawazahmed0/currency-api@1/latest/currencies/"+cur[fromCurrency]+".min.json";
   http.begin(endpoint); //Specify the URL
   int httpCode = http.GET();  //Make the request
   if (httpCode > 0) { //Check for the returning code
   payload = http.getString();
   char inp[payload.length()];
   payload.toCharArray(inp,payload.length());
   deserializeJson(doc,inp);

    date2=String(doc["date"].as<const char*>());
    res=doc[cur[fromCurrency]][cur[toCurrency]].as<float>();
   }}
  
}

void draw()
{

 
sprite.pushImage(0,0,320,170,curr);

sprite.loadFont(midle);
sprite.setTextColor(c1,TFT_BLACK);
sprite.drawString(date2,185,19);
sprite.fillSmoothRoundRect(276, 107, 30, 24, 4, c3,TFT_BLACK);
sprite.drawString(String(timeSec),280,110);
sprite.unloadFont();

sprite.loadFont(final);
sprite.drawString(big[fromCurrency]+" - "+big[toCurrency],12,60);
sprite.unloadFont();

sprite.setTextColor(c2,TFT_BLACK);
sprite.loadFont(large);
sprite.drawFloat(res,6,12,88);//
sprite.unloadFont();
sprite.setTextColor(c1,TFT_BLACK);

sprite.loadFont(small);
sprite.drawString("IP:"+ip,170,146);
sprite.drawString(curNames[fromCurrency]+" to ",12,126);
sprite.drawString(curNames[toCurrency],12,144);
sprite.setTextColor(c2,TFT_BLACK);
sprite.drawString("TIME",232,107);
sprite.setTextColor(c1,TFT_BLACK);
 
sprite.unloadFont();
sprite.drawString(String(timeHour)+":"+String(timeMin),172,52,7);

sprite.pushSprite(0,0);
}



void printLocalTime()
  {
  struct tm timeinfo;
  
  if(!getLocalTime(&timeinfo)){
   return;}
  
  strftime(timeHour,3, "%H", &timeinfo);
  strftime(timeMin,3, "%M", &timeinfo);
  strftime(timeSec,3, "%S", &timeinfo);
  }

void loop() {
if(digitalRead(0)==0)
{
  if(deb1==0)
  {
    deb1=1;
    fromCurrency++;
    if(fromCurrency==11) fromCurrency=0;
    getData();
    draw();
  }  
}else {deb1=0;}

if(digitalRead(14)==0)
{
  if(deb2==0)
  {
    deb2=1;
    toCurrency++;
    if(toCurrency==11) toCurrency=0;
    getData();
    draw();
  }  
}else {deb2=0;}

if(millis()>currentTime+period)
{
  printLocalTime();
  draw();
  currentTime=millis();
  
}

}
