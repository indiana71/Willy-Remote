/******************* WiFi Robot Remote Control Mode ********************/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "pitches.h"
#include "melody.h"

// connections for drive Motors
int PWM_A = D2;
int PWM_B = D1;
int DIR_A = D4;
int DIR_B = D3;

const int buzPin = D5;      // set digital pin D5 as buzzer pin (use active buzzer)
const int ledPin = D8;      // set digital pin D8 as LED pin (use super bright LED)
const int wifiLedPin = D0;  // set digital pin D0 as indication, the LED turn on if NodeMCU connected to WiFi as STA mode

int pauseBetweenNotes = 0;  // Tempo di pausa tra due note

String command;          // String to store app command state.
int SPEED = 1023;        // 330 - 1023.
int speed_Coeff = 3;

ESP8266WebServer server(80);      // Create a webserver object that listens for HTTP request on port 80

unsigned long previousMillis = 0;

// String sta_ssid = "Vodafone-33754181";      // set Wifi networks you want to connect to
// String sta_password = "562jad7td8a5t5z";  // set password for Wifi networks 

String sta_ssid = "Andrea's Galaxy S23 Ultra";      // set Wifi networks you want to connect to
String sta_password = "GalaxyUltra";  // set password for Wifi networks 


void setup(){
  Serial.begin(115200);    // set up Serial library at 115200 bps
  Serial.println();
  Serial.println("*WiFi Robot Remote Control Mode*");
  Serial.println("--------------------------------------");

  pinMode(buzPin, OUTPUT);      // sets the buzzer pin as an Output
  pinMode(ledPin, OUTPUT);      // sets the LED pin as an Output
  pinMode(wifiLedPin, OUTPUT);  // sets the Wifi LED pin as an Output
  digitalWrite(buzPin, LOW);
  digitalWrite(ledPin, LOW);
  digitalWrite(wifiLedPin, HIGH);
   
  // Set all the motor control pins to outputs
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
 
  // Turn off motors - Initial state
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);

  // set NodeMCU Wifi hostname based on chip mac address
  String chip_id = String(ESP.getChipId(), HEX);
  int i = chip_id.length()-4;
  chip_id = chip_id.substring(i);
  chip_id = "wificar-" + chip_id;
  String hostname(chip_id);
 
  Serial.println();
  Serial.println("Hostname: "+hostname);

  // first, set NodeMCU as STA mode to connect with a Wifi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(sta_ssid.c_str(), sta_password.c_str());
  Serial.println("");
  Serial.print("Connecting to: ");
  Serial.println(sta_ssid);
  Serial.print("Password: ");
  Serial.println(sta_password);

  // try to connect with Wifi network about 10 seconds
  unsigned long currentMillis = millis();
  previousMillis = currentMillis;
  while (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis <= 10000) {
    delay(500);
    Serial.print(".");
    currentMillis = millis();
  }

  // if failed to connect with Wifi network set NodeMCU as AP mode
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("*WiFi-STA-Mode*");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(wifiLedPin, LOW);    // Wifi LED on when connected to Wifi as STA mode
    delay(3000);
  } else {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(hostname.c_str());
    IPAddress myIP = WiFi.softAPIP();
    Serial.println("");
    Serial.println("WiFi failed connected to " + sta_ssid);
    Serial.println("");
    Serial.println("*WiFi-AP-Mode*");
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    digitalWrite(wifiLedPin, HIGH);   // Wifi LED off when status as AP mode
    delay(3000);
  }


  server.on ( "/", HTTP_handleRoot );       // call the 'handleRoot' function when a client requests URI "/"
  server.onNotFound ( HTTP_handleRoot );    // when a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  server.begin();                           // actually start the server
 
  ArduinoOTA.begin();                       // enable to receive update/uploade firmware via Wifi OTA
}


void loop() {
    ArduinoOTA.handle();          // listen for update OTA request from clients
    server.handleClient();        // listen for HTTP requests from clients
   
      command = server.arg("State");          // check HTPP request, if has arguments "State" then saved the value
      if (command == "F") Forward();          // check string then call a function or set a value
      else if (command == "B") Backward();
      else if (command == "R") TurnRight();
      else if (command == "L") TurnLeft();
      else if (command == "G") ForwardLeft();
      else if (command == "H") BackwardLeft();
      else if (command == "I") ForwardRight();
      else if (command == "J") BackwardRight();
      else if (command == "S") Stop();
      else if (command == "V") BeepHorn();
      else if (command == "W") TurnLightOn();
      else if (command == "w") TurnLightOff();
      else if (command == "0") SPEED = 330;
      else if (command == "1") SPEED = 400;
      else if (command == "2") SPEED = 470;
      else if (command == "3") SPEED = 540;
      else if (command == "4") SPEED = 610;
      else if (command == "5") SPEED = 680;
      else if (command == "6") SPEED = 750;
      else if (command == "7") SPEED = 820;
      else if (command == "8") SPEED = 890;
      else if (command == "9") SPEED = 960;
      else if (command == "q") SPEED = 1023;
}

// function prototypes for HTTP handlers
void HTTP_handleRoot(void){
  server.send ( 200, "text/html", "" );       // Send HTTP status 200 (Ok) and send some text to the browser/client
 
  if( server.hasArg("State") ){
     Serial.println(server.arg("State"));
  }
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

// function to move forward
void Forward(){
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

// function to move backward
void Backward(){
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

// function to turn right
void TurnRight(){
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

// function to turn left
void TurnLeft(){
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

// function to move forward left
void ForwardLeft(){
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED/speed_Coeff);
}

// function to move backward left
void BackwardLeft(){
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED/speed_Coeff);
}

// function to move forward right
void ForwardRight(){
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_A, SPEED/speed_Coeff);
  analogWrite(PWM_B, SPEED);
}

// function to move backward left
void BackwardRight(){
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, SPEED/speed_Coeff);
  analogWrite(PWM_B, SPEED);
}

// function to stop motors
void Stop(){ 
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}

// function to beep a buzzer
void BeepHorn(){
  digitalWrite(buzPin, HIGH);
  delay(150);
  digitalWrite(buzPin, LOW);
  delay(80);

  //  suonaSuoneria(melody1, noteDurations1, 1000, 1.30, (sizeof(melody1) / sizeof(melody1[0])));       // Clacson corriera
  //  suonaSuoneria(melody3, noteDurations3, 1000, 1.30, (sizeof(melody3) / sizeof(melody3[0])));       // Tetris
  suonaSuoneria(melody4, noteDurations4, 800, 1.30, (sizeof(melody4) / sizeof(melody4[0])));            // Hazard (Dixie)
  //  suonaSuoneria(melody5, noteDurations5, 3200, 1.20, (sizeof(melody5) / sizeof(melody5[0])));       // Seven nation army
  //  suonaSuoneria(melody6, noteDurations6, 2400, 1.30, (sizeof(melody6) / sizeof(melody6[0])));       // Campioni del mondo
  //  suonaSuoneria(melody7, noteDurations7, 2400, 1.20, (sizeof(melody7) / sizeof(melody7[0])));       // Bevo Bevo
/*
  for (int bb = 0; bb < 3; bb++) {                                                                      // Best Brau Theme
    suonaSuoneria(melody82, noteDurations82, 2000, 1.30, (sizeof(melody82) / sizeof(melody82[0])));
    suonaSuoneria(melody82, noteDurations82, 2000, 1.30, (sizeof(melody82) / sizeof(melody82[0])));
    suonaSuoneria(melody81, noteDurations81, 2000, 1.30, (sizeof(melody81) / sizeof(melody81[0])));
  }
*/
  //  suonaSuoneria(melody9, noteDurations9, 1400, 1.30, (sizeof(melody9) / sizeof(melody9[0])));       // Best Brau Theme (Only rit)
  //  suonaSuoneria(melody10, noteDurations10, 2000, 1.00, (sizeof(melody10) / sizeof(melody10[0])));   // Che la sia crucca o terona
  //  suonaSuoneria(melody11, noteDurations11, 1400, 1.30, (sizeof(melody11) / sizeof(melody11[0])));   // Maza la vecia
  //  suonaSuoneria(melody12, noteDurations12, 1000, 1.30, (sizeof(melody12) / sizeof(melody12[0])));   // Gne gne gne gne gne gne gnee
  //  suonaSuoneria(melody13, noteDurations13, 6000, 1.30, (sizeof(melody13) / sizeof(melody13[0])));   // Rumore piu fastioso del mondo 
/*
  for (int k = 0; k < 6; k++) {                                                                          // Koala
    suonaSuoneria(melody14, noteDurations14, 800, 1.30, (sizeof(melody14) / sizeof(melody14[0])));
  }
*/
}

// function to turn on LED
void TurnLightOn(){
  digitalWrite(ledPin, HIGH);
}

// function to turn off LED
void TurnLightOff(){
  digitalWrite(ledPin, LOW);
} 


/* Funzione per riprodurre la nota usando tone()
 * Parametri passati:
 * int* melodia : puntatore alla melodia scritta nel file melody.h
 * int* noteDurata : puntatore all'array della durata di ogni singola nota
 * int durata : paramentro per regolare la velocità della nota
 * float pausa : parametro moltiplicativo per impostare la pausa tra le note
 * int sizeMelodia : dimensione array della melodia da riprodurre
 */

void suonaSuoneria(int* melodia, int* noteDurata, int durata, float pausa, int sizeMelodia) {

  for (int thisNote = 0; thisNote < sizeMelodia; thisNote++) {

    int noteDuration = durata / noteDurata[thisNote];
    tone(buzPin, melodia[thisNote], noteDuration);

    pauseBetweenNotes = noteDuration * pausa;
    delay(pauseBetweenNotes);

    noTone(buzPin);
    
  }
}
