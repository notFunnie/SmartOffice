#include <LiquidCrystal_I2C.h>
#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <dhtnew.h>

#define DHTPIN D12     // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

//Initialize DHT11 and LCD objects
DHTNEW mySensor(DHTPIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

//DB and WiFi Credentials
#define FIREBASE_HOST "remotelightswitch-97376.firebaseio.com"
#define FIREBASE_AUTH "Z2av7uZ53rrnzYLhc5BjENtpO3EMgv3E58mPotgp"
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

//Initialize variables
int stat = D13;
int led = D11;
int ledStatus = 0;
String msg = "";
String oldMsg = msg;
long lastTime = 0;
long startTime = millis();
float setPoint = 75;
float tempMax = 126;
float tempMin = 28;
float humMax = 90;
float humMin = 10;
float t = 70;
float h = 50;
float hif = 72;
//-------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  lcd.begin();

  Serial.println("DHTxx test!");

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(); Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(stat, OUTPUT);

  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("KidOYO Messages:");
  lcd.setCursor(0, 1);
}

//---------------------------------------------------------------------------------------
void loop() {
  //Get LED status
  ledStatus = Firebase.getInt("status");
  Serial.print("LED Status: "); Serial.println(ledStatus);
  digitalWrite(led, ledStatus);

  //Get LCD message
  msg = Firebase.getString("message");
  if (msg != oldMsg) {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(msg);
    oldMsg = msg;
  }

  //Read temperature. If 2 seconds elapsed, update reading
  startTime = millis() % 10000;
  if (abs(startTime - lastTime) > 2000) {
    Serial.println("ENTERED THE IF STATEMENT!!!!!");
    delay(100);
    mySensor.read();
    h = mySensor.humidity; //Temp in C
    t = mySensor.temperature; //RH
    t = (t/.5556)+32; //Convert to F
    hIndex();

    // Check if any reads failed or out of range and exit early (to try again).
    if (isnan(h) || isnan(t) || t < tempMin || t > tempMax || h < humMin || h > humMax) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      Firebase.setFloat("temp", t);
      Firebase.setFloat("humidity", h);
      Firebase.setFloat("hif", hif);
      if (Firebase.failed()) {
        Serial.print("setting /something failed:");
        Serial.println(Firebase.error());
        return;
      }
      else {
        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print(" *C ");
      }
    }
    lastTime = startTime;
  }

  //Thermostat code. Turn on "AC" when temperature above setpoint.
  if (Firebase.getFloat("temp") > setPoint) {
    digitalWrite(stat, HIGH);
  }
  else {
    digitalWrite(stat, LOW);
  }
  Serial.println(); //Gotta keep 'em separated
}

void hIndex() {
  hif = -42.379 + 2.04901523*t + 10.14333127*h - .22475541*t*h - .00683783*t*t- .05481717*h*h+ .00122874*t*t*h + .00085282*t*h*h - .00000199*t*t*h*h;
}
