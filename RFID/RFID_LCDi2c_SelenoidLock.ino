#include <RFID.h>
#include <Wire.h>  // i2C Conection Library
#include <LiquidCrystal_I2C.h>  //Memanggil i2C LCD Library
LiquidCrystal_I2C lcd(0x3F, 16, 2);
/*
Sambungan Module RFID RC-522 ke Arduino
* MOSI: Pin 11
* MISO: Pin 12
* SCK : Pin 13
* SS/SDA: Pin 10
* RST: Pin 9
* RQ: Tidak digunakan
*/
#include <SPI.h>
#include <RFID.h>
#define SS_PIN 10
#define RST_PIN 9

RFID rfid(SS_PIN,RST_PIN);

const int buzzer = 7;   //Passive buzzer ke Pin D7 dan GND
const int doorLock = 8; //Driver Selenoid Door Lock/Relay input ke pin D8
int serNum[5];          //Variable buffer Scan Card

int cards[][5] = {      //ID Kartu yang diperbolehkan masuk
  {141,125,2,197,55}
  
};

bool access = false;

int alarm = 0;
uint8_t alarmStat = 0;
uint8_t maxError = 5;
//Coding berikut dibuat oleh Dani untuk www.belajarduino.com
//Semoga bermanfaat bagi yang sedang belajar RFID system
//Support by RAJACELL - BEKASI
//Semua module yang dibutuhkan dapat dibeli di www.tokopedia.com/rajacell
void setup(){
    Serial.begin(9600);
    lcd.begin();
    SPI.begin();
    rfid.init();

    pinMode(doorLock, OUTPUT);
    digitalWrite(doorLock, HIGH);

  tone (buzzer,1200);
  lcd.setCursor (0,0);
  lcd.print(F("  RFID System   "));
  lcd.setCursor (0,1);
  lcd.print(F(" Control Access "));
  delay (2000);
  lcd.clear();
  noTone (buzzer);
}

void loop(){
  if (alarm >= maxError){
    alarmStat = 1;    }
  
  if (alarmStat == 0){
  lcd.setCursor (0,0);
  lcd.print(F(" -System Ready- "));
  lcd.setCursor (0,1);
  lcd.print(F(" Scan Your Card "));

    if(rfid.isCard()){
    
        if(rfid.readCardSerial()){
            Serial.print(rfid.serNum[0]);
            Serial.print(" ");
            Serial.print(rfid.serNum[1]);
            Serial.print(" ");
            Serial.print(rfid.serNum[2]);
            Serial.print(" ");
            Serial.print(rfid.serNum[3]);
            Serial.print(" ");
            Serial.print(rfid.serNum[4]);
            Serial.println("");
            
            for(int x = 0; x < sizeof(cards); x++){
              for(int i = 0; i < sizeof(rfid.serNum); i++ ){
                  if(rfid.serNum[i] != cards[x][i]) {
                      access = false;
                      break;
                  } else {
                      access = true;
                  }
              }
              if(access) break;
            }   
        }
        
       if(access){
          Serial.println("Welcome!");
          lcd.setCursor (0,0);
          lcd.print(F(" Akses diterima "));
          lcd.setCursor (0,1);
          lcd.print("ID:");
          lcd.print(rfid.serNum[0]); lcd.print(rfid.serNum[1]);
          lcd.print(rfid.serNum[2]); lcd.print(rfid.serNum[3]);
          lcd.print(rfid.serNum[4]);
           digitalWrite(doorLock, LOW);
           tone (buzzer,900);
           delay(100);
           tone (buzzer,1200);
           delay(100);
           tone (buzzer,1800);
           delay(200);
           noTone(buzzer);  
           delay(600);
           lcd.setCursor (0,0);
           lcd.print(F(" Silahkan Masuk "));
           lcd.setCursor (0,1);
           lcd.print(F("AutoLock after "));
           for(int i=5; i>0; i--){
            lcd.setCursor (15,1); lcd.print(i);
            delay (1000);
           }
           digitalWrite(doorLock, HIGH);
           lcd.clear();
      } else {
           alarm = alarm+1;
           Serial.println("Not allowed!"); 
           lcd.setCursor (0,0);
           lcd.print(F(" Akses ditolak  "));
           lcd.setCursor (0,1);
           lcd.print("ID:");
           lcd.print(rfid.serNum[0]); lcd.print(rfid.serNum[1]);
           lcd.print(rfid.serNum[2]); lcd.print(rfid.serNum[3]);
           lcd.print(rfid.serNum[4]);
           tone (buzzer,900);
           delay(200);
           noTone(buzzer);
           delay(200);
           tone (buzzer,900);
           delay(200);
           noTone (buzzer);  
           delay(500);
           lcd.clear();      
       }        
    }
    rfid.halt();
}
else {
  lcd.setCursor (0,0);
  lcd.print(F("-System LOCKED- "));
  lcd.setCursor (0,1);
  lcd.print(F(" Please Wait "));
for(int i=60; i>0; i--){
tone (buzzer,1800);
lcd.setCursor (13,1); lcd.print(i);
lcd.print(F("  "));delay (1000);}
noTone (buzzer);
alarmStat = 0;
alarm = 0;
  }
}
