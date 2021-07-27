/*------------------------------
  By.Zheng
  Facebook: https://www.facebook.com/shiwenooo
  Line: zheng-shiwen
  --------------------------------
  這部分一開始有用點心
  不過到最後懶得去寫Function
  所以某些部份就直接一直複製貼上了xDDD
  有問題或是有更好的寫法都歡迎一起交流喔^^
  --------------------------------*/

//函數引用
//溝通靠電流
int itsmat = 4;//pin4輸出配對成功電流，輸出給NODEMCU
int errormat = 3;//pin3輸出配對失敗電流
//pin2蜂鳴器
int beeper = 6;
//卡片序號儲存
char* cardID[] = {
  "16:21:28:12",
  "16:21:28:52",
  "16:21:28:22"
};
#define Users (sizeof(cardID)/sizeof(char *)) //array size 
//馬達
#include <Servo.h>
Servo myservo; // 建立Servo物件，控制伺服馬達
//開關
boolean onoff = true;
//繼電器
const byte RealyPin = 2;
//繼電器
//----------------------------------------
//RFID
#include "SPI.h"
#include "MFRC522.h"
#define SS_PIN 10//Nss
#define RST_PIN 9//Rst
#define SP_PIN 8//nul
MFRC522 rfid(SS_PIN, RST_PIN);//設置RFID
MFRC522::MIFARE_Key key;//儲存
//RFID
//----------------------------------------
//LCD顯示模組
#include <LiquidCrystal_I2C.h>
// 設定 LCD I2C 位址
// addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
//LCD顯示模組
//----------------------------------------
//函數引用結束
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);//通訊埠9600
  SPI.begin();//開始
  rfid.PCD_Init();//初始化rfid
  //----------------------------------------
  // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
  lcd.begin(16, 2);
  // 閃爍
  lcd.backlight(); // 開啟背光
  delay(250);
  lcd.noBacklight(); // 關閉背光
  delay(250);
  lcd.backlight();// 開啟背光
  // 輸出初始化文字
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Hello Home V1.0");
  delay(1000);
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("Hello,_____!");
  //----------------------------------------
  //設定繼電器
  pinMode(RealyPin, OUTPUT);
  digitalWrite(RealyPin, HIGH);
  //----------------------------------------
  //溝通
  pinMode(itsmat, OUTPUT);//
  digitalWrite(itsmat, LOW);//輸出成功
  ///////////////////////////
  pinMode(errormat, OUTPUT);//
  digitalWrite(errormat, LOW);//輸出成功配對失敗
  //馬達
  myservo.attach(7); // 連接數位腳位7，伺服馬達的訊號線
  myservo.write(0); //轉0度
  //----------------------------------------
  //讀取存取卡片使用者
  Serial.print("目前人數：");
  Serial.println(Users);
  Serial.println("目前使用者id：");
  for (int i = 0; i < Users; i++) {
    Serial.println(cardID[i]);
  }
  Serial.println("-------------------");
  //蜂鳴器
  pinMode(beeper, OUTPUT);
  digitalWrite(beeper, LOW);
  //-------------------

}

void loop() {
  // put your main code here, to run repeatedly:

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  // Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
      piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("你感應的卡並不屬於MIFARE的形式"));
    return;
  }

  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? ":" : "");
  }
  strID.toUpperCase();


  Serial.print("卡片金鑰: ");
  Serial.println(strID);
  //卡片配對
  for (int i = 0; i < Users; i++) {
    if (strID == cardID[i]) {
      Serial.println("配對成功");
      Serial.println(onoff);
      Serial.println("-------------------");
      digitalWrite(beeper, HIGH);
      delay(100);
      digitalWrite(beeper, LOW);

      match();
      break;//配對成功後跳出ＬＯＯＰ
    } else {
      Serial.print("配對失敗次數");
      Serial.println(i + 1);
      if (i == Users - 1) {
        //告訴wifi配對失敗
        Serial.println("-------------------");
        digitalWrite(errormat, HIGH);//輸出成功配對失敗
        delay(50);
        digitalWrite(errormat, LOW);//輸出成功配對失敗
        //BEEP三次
        digitalWrite(beeper, HIGH);
        delay(100);
        digitalWrite(beeper, LOW);
        delay(100);
        digitalWrite(beeper, HIGH);
        delay(100);
        digitalWrite(beeper, LOW);
        delay(100);
        digitalWrite(beeper, HIGH);
        delay(100);
        digitalWrite(beeper, LOW);
        //=========================
        lcd.clear();
        lcd.setCursor(0, 0); // 設定游標位置在第一行行首
        lcd.print("This card id was");
        lcd.setCursor(0, 1); // 設定游標位置在第二行行首
        lcd.print("not found");
        delay(1500);
        lcd.clear();
        lcd.setCursor(0, 0); // 設定游標位置在第一行行首
        lcd.print("not found");
        lcd.setCursor(0, 1); // 設定游標位置在第二行行首
        lcd.print("on this Sql");
        delay(1500);
        lcd.clear();
        lcd.setCursor(0, 0); // 設定游標位置在第一行行首
        lcd.print("Please change a");
        lcd.setCursor(0, 1); // 設定游標位置在第二行行首
        lcd.print("another one!");
        delay(1500);
        lcd.clear();
        lcd.setCursor(0, 0); // 設定游標位置在第一行行首
        lcd.print("Waiting to read");
        lcd.setCursor(0, 1); // 設定游標位置在第二行行首
        lcd.print("RFID_CARD");
        break;
      }
    }

  }
  //---------------------

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

//執行卡片配對成功
void match() {
  if (onoff) {
    lcd.clear();
    lcd.setCursor(0, 0); // 設定游標位置在第一行行首
    lcd.print("Success!");
    lcd.setCursor(0, 1); // 設定游標位置在第二行行首
    lcd.print("Waiting for Servo");
    digitalWrite(RealyPin, LOW);  //繼電器通電,讓馬達轉動

    //通訊管道
    digitalWrite(itsmat, HIGH);//1
    delay(50);
    digitalWrite(itsmat, LOW);//0
    //============================
    myservo.write(90); //轉90度
    delay(1000);//給予馬達轉動時間
    digitalWrite(RealyPin, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0); // 設定游標位置在第一行行首
    lcd.print("Hi!");
    lcd.setCursor(0, 1); // 設定游標位置在第二行行首
    lcd.print("good bye");
    onoff = false;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0); // 設定游標位置在第一行行首
    lcd.print("Hi!");
    lcd.setCursor(0, 1); // 設定游標位置在第二行行首
    lcd.print("Waiting for Servo");
    digitalWrite(RealyPin, LOW); //繼電器通電

    //通訊管道
    digitalWrite(itsmat, HIGH);//1
    delay(50);
    digitalWrite(itsmat, LOW);//0
    //============================
    myservo.write(0); //轉0度
    delay(1000);//給予馬達轉動時間
    digitalWrite(RealyPin, HIGH);

    lcd.clear();
    lcd.setCursor(0, 0); // 設定游標位置在第一行行首
    lcd.print("Hi!");
    lcd.setCursor(0, 1); // 設定游標位置在第二行行首
    lcd.print("Welcome");
    onoff = true;
  }
}
