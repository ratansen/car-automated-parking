#include <ESP8266WiFi.h>

//Firebase Arduino library to upload data to realtime database
#include <FirebaseArduino.h>

//NTP Client library to fetch current time from NTP client
#include <NTPClient.h>
#include <WiFiUdp.h>

//Wifi manager library to change SSID and Password during runtime
#include <WiFiManager.h>

//Firebase related parameters
#define FIREBASE_HOST "nodemcu-test-28d25-default-rtdb.firebaseio.com" //database url
#define FIREBASE_AUTH "wNxFVddy0N0wOlW6KA6r3Oiah6H2F4emohrrCAmd" // project secret key

//Time related parameters
const long utcOffsetInSeconds = 19800;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

//Strings used to create path name
String stringOne, documentName;

// pins for indicator LEDs and button
#define TRIGGER_PIN 0
int connection_pin = 2;
int ap_mode = 4;
int error_pin = 5;

int timeout = 600;
bool isAPOn = false;

void setup() {
  //Setting the pins
  pinMode(connection_pin , OUTPUT);
  pinMode(ap_mode , OUTPUT);
  pinMode(error_pin , OUTPUT);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //Setup station mode
  WiFi.mode(WIFI_STA);

  //Connecting
  while (WiFi.status() != WL_CONNECTED) {
    //Check if user presses button, then turn on AP mode
    if (digitalRead(TRIGGER_PIN)  == 0) {
      changePass();
    }
    
    Serial.print(".");
    digitalWrite(connection_pin , HIGH);
    delay(100);
    digitalWrite(connection_pin , LOW);
    delay(100);
  }

  //On connection established
  digitalWrite(connection_pin , HIGH);
  Serial.println("Connected");

  //Setup firebase object
  Firebase.begin(FIREBASE_HOST , FIREBASE_AUTH);

  // Create string for path
  stringOne = String("ID1/");
  documentName = String();

  // Starting the time client
  timeClient.begin();

}

void loop() {
  //Check if user presses button, then turn on AP mode
  if (digitalRead(TRIGGER_PIN)  == 0) {
    changePass();
  }

  // listen for serial data regularly and upload the data to firebase when available
  if (Serial.available()) {
    //Read incoming string
    String str = Serial.readString();
    //Serial.println(str);

    //Updating time client in every loop
    timeClient.update();

    //create the document path
    documentName = stringOne + timeClient.getEpochTime();

    //Upload the recieved string
    Firebase.setString(documentName, str);

    Serial.println(Firebase.success());

    // handle error
    if (Firebase.failed()) {
      digitalWrite(error_pin , HIGH);
      Serial.print("setting /message failed:");
      Serial.println(Firebase.error());
      return;
    }
    Serial.println("Uploaded");
    digitalWrite(error_pin , LOW);
  }
}

void changePass() {
  //Indicate AP mode is on
  digitalWrite(ap_mode , HIGH);

  //WiFiManager Object initialized
  WiFiManager wm;

  // set configportal timeout
  wm.setConfigPortalTimeout(timeout);

  if (!wm.startConfigPortal("On_Demand_AP")) {
    Serial.println("failed to connect and hit timeout");
    digitalWrite(error_pin , HIGH);
    delay(3000);
    digitalWrite(error_pin , LOW);
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
  }

  //On Connected to new WiFi network
  Serial.println("connected...yeey :)");
  isAPOn = false;
  digitalWrite(ap_mode , LOW);
}

void setAPmode() {
  digitalWrite(ap_mode , HIGH);
  isAPOn = true;
}
