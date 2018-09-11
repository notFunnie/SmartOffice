#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "remotelightswitch-97376.firebaseio.com"
#define FIREBASE_AUTH "Z2av7uZ53rrnzYLhc5BjENtpO3EMgv3E58mPotgp"
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

int ledStatus = 0;
int led = 4;

void setup() {
  Serial.begin(9600);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led, OUTPUT);

  for(int i = 0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, 1);
    delay(500);
    digitalWrite(LED_BUILTIN, 0);
    delay(100);
  }
}


void loop() {
  ledStatus = Firebase.getInt("status");
  Serial.println(ledStatus);
  digitalWrite(led, ledStatus);

  delay(1000);
}
