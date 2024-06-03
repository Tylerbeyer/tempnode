#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; 
Adafruit_SSD1306 display(128, 64, &Wire, -1);

const char* ssid = "TEMPFinder";
const char* password = "OPELopel1234567890";
const char* piServer = "192.166.4.11";
const int piPort = 8080;

unsigned long delayTime = 1000;

float temperatureC; 
float pressureHPA; 
float altitudeM; 
float humidity; 

void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  bool status;

  Wire.begin();
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  WiFi.begin(ssid, password);
  while (WiFi.status()!= WL_CONNECTED) {
    display.setCursor(0, 0);
    display.print("Connecting to WiFi...");
    display.display();
    delay(500);
  }

  Serial.println("-- Default Test --");
  Serial.println();
}

void loop() {
  printValues();
  delay(delayTime);
  sendToPiServer();
}

void printValues() {
  temperatureC = bme.readTemperature();
  pressureHPA = bme.readPressure() / 100.0F;
  altitudeM = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();

  float temperatureF = temperatureC * 9.0 / 5.0 + 32.0;
  float pressureMB = pressureHPA;
  float altitudeFT = altitudeM * 3.28084;

  Serial.print("Temperature (C): ");
  Serial.print(temperatureC);
  Serial.println(" *C");

  Serial.print("Temperature (F): ");
  Serial.print(temperatureF);
  Serial.println(" *F");

  Serial.print("Pressure (hPa): ");
  Serial.print(pressureHPA);
  Serial.println(" hPa");

  Serial.print("Pressure (mbar): ");
  Serial.print(pressureMB);
  Serial.println(" mbar");

  Serial.print("Altitude (m): ");
  Serial.print(altitudeM);
  Serial.println(" m");

  Serial.print("Altitude (ft): ");
  Serial.print(altitudeFT);
  Serial.println(" ft");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.println();

  display.clearDisplay();
  display.setCursor(0, 0);
  if (WiFi.status() == WL_CONNECTED) {
    display.print("Temp (C): ");
    display.print(temperatureC);
    display.println(" *C");
    display.print("Temp (F): ");
    display.print(temperatureF);
    display.println(" *F");
    display.print("Humidity: ");
    display.print(humidity);
    display.println(" %");
    display.print("Pressure: ");
    display.print(pressureHPA);
    display.println(" hPa");
    display.print("Altitude: ");
    display.print(altitudeM);
    display.println(" m");
    display.print("Altitude: ");
    display.print(altitudeFT);
    display.println(" ft");
  } else {
    display.print("Connecting to WiFi...");
  }
  display.display();
}

void sendToPiServer() {
  String sensorData = String(temperatureC) + "," + String(pressureHPA) + "," + String(altitudeM) + "," + String(humidity);

  char data[sensorData.length() + 1];
  sensorData.toCharArray(data, sensorData.length() + 1);

  WiFiClient client;
  if (client.connect(piServer, piPort)) {
    Serial.println("Connected to the Raspberry Pi server");
    client.print(data);
    client.stop();
  } else {
    Serial.println("Failed to connect to the Raspberry Pi server");
  }
}
