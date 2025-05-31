/***************************************************
 * Smart Hub ESP32 Firmware
 * Handles sensor readings, POSTs data to FastAPI,
 * and receives fan/light control decisions.
 ***************************************************/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <time.h>
#include "../include/config.h"  // Wi-Fi + API config

// --- Pin Configuration ---
#define FAN_PIN    23
#define LIGHT_PIN  22
#define PIR_PIN    15
#define TEMP_PIN   4

// --- Objects ---
WiFiClientSecure wifi;
HttpClient client(wifi, serverAddress, 443);
OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

// --- Connect to Wi-Fi ---
void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");
}

// --- Format time string ---
String getCurrentTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "1970-01-01T00:00:00";
  char buf[25];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  return String(buf);
}

// --- Read Sensors ---
float readTemperature() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}

bool readMotion() {
  return digitalRead(PIR_PIN) == HIGH;
}

// --- POST Sensor Data ---
void sendSensorData(float temperature, bool motion, String datetime) {
  StaticJsonDocument<256> doc;
  doc["temperature"] = temperature;
  doc["presence"] = motion;
  doc["datetime"] = datetime;

  String json;
  serializeJson(doc, json);

  client.beginRequest();
  client.post("/api/data");
  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", json.length());
  client.beginBody();
  client.print(json);
  client.endRequest();

  Serial.print("POST → ");
  Serial.println(client.responseStatusCode());
  Serial.println(client.responseBody());
}

// --- GET Control Decision ---
void getAndApplyDecision() {
  client.get("/api/decision");

  int status = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("GET → ");
  Serial.println(status);
  Serial.println("Response: " + response);

  if (status != 200) return;

  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, response)) return;

  digitalWrite(FAN_PIN, doc["turn_fan_on"] ? HIGH : LOW);
  digitalWrite(LIGHT_PIN, doc["turn_light_on"] ? HIGH : LOW);
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  connectToWiFi();
  wifi.setInsecure();  // Accept self-signed HTTPS
  configTime(0, 0, "pool.ntp.org");
  sensors.begin();
}

// --- Main Loop ---
void loop() {
  float tempC = readTemperature();
  bool motion = readMotion();
  String now = getCurrentTimeString();

  Serial.println("Temperature: " + String(tempC) + "°C");
  Serial.println("Motion: " + String(motion));
  Serial.println("Time: " + now);

  sendSensorData(tempC, motion, now);
  delay(500);
  getAndApplyDecision();
  delay(10000);  // Run every 10s
}
