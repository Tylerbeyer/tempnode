#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
Adafruit_SSD1306 display(128, 64, &Wire, -1);

const char* ssid = "TEMPFinder";
const char* password = "OPELopel1234567890";
const char* piServer = "192.166.4.11";
const int piPort = 8080;


unsigned long delayTime = 1000;

void setup() {
  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  bool status;

  // I2C
  Wire.begin();
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
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
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  display.clearDisplay();
  display.setCursor(0, 0);
  if (WiFi.status() == WL_CONNECTED) {
    display.print("Temp: ");
    display.print(bme.readTemperature());
    display.println(" *C");
    display.print("Press: ");
    display.print(bme.readPressure() / 100.0F);
    display.println(" hPa");
    display.print("Alt: ");
    display.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    display.println(" m");
    display.print("Hum: ");
    display.print(bme.readHumidity());
    display.println(" %");
  } else {
    display.print("Not connected to WiFi");
  }
  display.display();

  Serial.println();
}

void sendToPiServer() {
  WiFiClient client;
  if (!client.connect(piServer, piPort)) {
    Serial.println("Connection to Pi server failed");
    return;
  }

  String data = String("temperature=") + bme.readTemperature() + "&pressure=" + (bme.readPressure() / 100.0F) + "&altitude="+ bme.readAltitude(SEALEVELPRESSURE_HPA) + "&humidity=" + bme.readHumidity();
  client.println(data);

  client.stop();
}
