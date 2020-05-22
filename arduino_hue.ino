#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define PHOTOCELL_PIN A0 // Brightness detector
#define LED_PIN 2  // LED on-board the device
#define MOTION_PIN 16 // Input for HC-S501 motion detector

const long startupTime = 30000;
int lightValue;
int motionValue;
int timer = 0;
int lastCommand;

HTTPClient http;

const char* ssid = "YOUR_ACCESS_POINT_SSID_HERE";
const char* password = "YOUR_ACCESS_POINT_PASSWORD_HERE";

const char* host = "YOUR_PHILIPS_HUE_API_URL";

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(MOTION_PIN, INPUT);
  digitalWrite(LED_PIN, LOW);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Waiting for motion sensor to initialize");
  long timeSinceStart = millis();
  while (timeSinceStart < startupTime) {
    Serial.print(".");
    delay(1000);
    timeSinceStart = millis();
  }
  Serial.println("Done!");

}

void loop() {
  // Capture sensor value
  motionValue = digitalRead(MOTION_PIN);
  digitalWrite(LED_PIN, motionValue ^ 1);
  
  String onValue = String();
  Serial.println(motionValue);

  if (lastCommand != motionValue && timer <= 0) {
    if (motionValue == 1) {
      onValue = "true";
      timer = 10000;
      lastCommand = 1;
    } else {
      onValue = "false";
      timer = 500;
      lastCommand = 0;
    }
  
    // Create connection to Philips Hue
    http.begin("%s/groups/2/action", host);
    http.addHeader("Content-Type", "application/json");
    http.PUT("{\"on\":" + onValue + "}");
    http.writeToStream(&Serial);
    http.end();
  } else {
    if (motionValue == 1) {
      Serial.println("Motion detected: extending timer!");
      timer = 10000;
    }
  }

  timer -= 500;
  delay(500);
}
