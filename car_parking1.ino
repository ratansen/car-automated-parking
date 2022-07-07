#include <SPI.h>
#include <MFRC522.h>
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

#include<LiquidCrystal.h>
#include<Servo.h>
Servo servo;
int sensor1 = A2;
int sensor2 = A3;
int val =0; 
int value=0;
int x = 4;
int rs = 8 , rw = 7 , enable = 6 , d4 = 5 , d5 = 4 , d6 = 3 , d7 = 2 ;
LiquidCrystal lcd(rs , rw , enable , d4 , d5 , d6 , d7);


void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   //rfid
   SPI.begin();      // Initiate  SPI bus
   mfrc522.PCD_Init();   // Initiate MFRC522
   Serial.println("Bring your card nearby...");
   Serial.println();
   
   pinMode( sensor1 , INPUT) ;
   pinMode( sensor2 , INPUT) ;
   servo.attach(A4);
   lcd.begin( 16 , 4);
   lcd.print("Total slots : 4 " );
   lcd.setCursor(0,1);
   lcd.print("Vacant slots : " ); 
   lcd.setCursor(15 ,1 );
   lcd.print(x) ;
   servo.write(0);
  
}


 //bool flag=0;
 bool gate = false;
// bool sense1=true;
// bool sense2=true;


void loop() {
  // put your main code here, to run repeatedly:
       val = analogRead(sensor1); 
       value = analogRead(sensor2);
      // Look for new cards
      
      if ( ! mfrc522.PICC_IsNewCardPresent() && analogRead(sensor2) > 300 ) 
      {
//        Serial.println("came1");
//              Serial.print("UID tag :");
//      String content= "";
//      byte letter;
//      for (byte i = 0; i < mfrc522.uid.size; i++) 
//      {
//         Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//         Serial.print(mfrc522.uid.uidByte[i], HEX);
//         content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
//         content.concat(String(mfrc522.uid.uidByte[i], HEX));
//      }
//      Serial.println();
//      Serial.print("Message : ");
//      content.toUpperCase();
        return;
      }
      // Select one of the cards
      if ( ! mfrc522.PICC_ReadCardSerial() && analogRead(sensor2) > 300) 
      {
        Serial.println("came2");
        return;
      }
      
      //Show UID on serial monitor
      Serial.print("UID tag :");
      String content= "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++) 
      {
         Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
         Serial.print(mfrc522.uid.uidByte[i], HEX);
         content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
         content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      Serial.println();
      Serial.print("Message : ");
      content.toUpperCase();
      if (content.substring(1) == "E3 16 6F 11") //change here the UID of the card/cards that you want to give access
      {
            Serial.println("Authorized access");
            //Serial.println(content.substring(1));
//            Serial.println(sense1);
//            Serial.println(sense2);
            delay(1000);
              val = analogRead(sensor1);  // read the input pin
            Serial.println(val);
            value = analogRead(sensor2);  // read the input pin
            Serial.println(value);
            
            if( x == 0){
              lcd.clear();
              lcd.print("No slots left");
            }
            if( servo.read() == 0 && x > 0 ){
              x = x - 1;
              delay(500);
              servo.write(90);
              lcd.begin( 16 , 2);
              lcd.print("Total slots : 4 " );
              lcd.setCursor(0,1);
              lcd.print("Vacant slots : " ); 
              lcd.setCursor(15 ,1 );
              lcd.print(x) ;
//              sense1=false;
              gate = true ;
            }
             while(gate){
                 if(analogRead(sensor2) < 300 && servo.read() == 90 ){
                      delay(500);
                      servo.write(0);
//                      sense1=true;
                      gate = false;
                 }
             } 
             Serial.println("came3");
             mfrc522.PICC_HaltA();
             for (byte i = 0; i < mfrc522.uid.size; i++) 
            {
               mfrc522.uid.uidByte[i] = 0x0;
            }
             
      }
     
      else if(analogRead(sensor2) < 300 && servo.read() == 0 && x < 4 ){
            x = x + 1;
            delay(500);
            servo.write(90);
            lcd.begin( 16 , 2);
            lcd.print("Total slots : 4 " );
            lcd.setCursor(0,1);
            lcd.print("Vacant slots : " ); 
            lcd.setCursor(15 ,1 );
            lcd.print(x) ;
//            sense2=false;

            while(analogRead(sensor1) > 300) {
              delay(500);
            }
            
            if(analogRead(sensor1) < 300 && servo.read() == 90 ){
                 delay(500);
                 servo.write(0);
//                 sense2=true;
            }
            Serial.println("came4");
      }
          
         
    //    delay(500);
     else{
        Serial.println(" Access denied");
        delay(3000);
        Serial.println("came5");
      }
      delay(1000);
 }
