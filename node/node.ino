/*------------------------------
  By.Zheng
  Facebook: https://www.facebook.com/shiwenooo
  Line: zheng-shiwen
  --------------------------------
  NodeNcu這裡寫得有點亂xD
  而且有些直接套範本
  所以看起來可能會有點不是很養眼
  有問題或是有更好的寫法都歡迎一起交流喔^^
  ------------------------------*/
/*
  這部分是配對arduino的喔!
  //溝通靠電流
  int itsmat = 4;//pin4輸出配對成功電流 接match
  int errormat = 3;//pin3輸出配對失敗電流 接matchfail
*/
int match = 16;
int matchfail = 5;
bool doorinfor = true;


#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial
ESP8266WiFiMulti WiFiMulti;

void setup() {

  USE_SERIAL.begin(115200);
  // USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }
  WiFiMulti.addAP("欲連線WIFI SSID", "欲連線WIFI PASSWORD");
  pinMode(match, INPUT);
  pinMode(matchfail, INPUT);
}

void loop() {
  int matchval = digitalRead(match);
  int failval = digitalRead(matchfail);
  //////////////////////////////////////
  /*
    這裡是如果配對失敗的話欲執行程式碼
    原本想寫配對失敗就傳東西到LINE (疑似有人正在嘗試破解門禁系統)
    不過本人搞丟了一堆卡..
    所以就也懶得改來改去
    這部分的WEBHOOK就也沒寫了
  */
  if (failval == 1) {
    USE_SERIAL.println(failval);
    USE_SERIAL.println("配對失敗");
    USE_SERIAL.println("------------------------");
  }
  //////////////////////////////////////

  if (matchval == 1) {
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      HTTPClient http;
      USE_SERIAL.print("[HTTP] begin...\n");
      if (doorinfor == true) {
        USE_SERIAL.println("上鎖");
        http.begin("輸入觸發的網址"); //HTTP
        USE_SERIAL.println("------------------------");
        doorinfor = false;
      } else {
        USE_SERIAL.println("開鎖");
        http.begin("輸入觸發的網址"); //HTTP
        doorinfor = true;
        USE_SERIAL.println("------------------------");
      }
      USE_SERIAL.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          USE_SERIAL.println(payload);
        }
      } else {
        USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }

  }

