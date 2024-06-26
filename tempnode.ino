#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>

// Define the sea level pressure in hectopascals
#define SEALEVELPRESSURE_HPA (1013.25)

// Create instances of the BME280 sensor and SSD1306 OLED display
Adafruit_BME280 bme; 
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Define the WiFi credentials and Raspberry Pi server details
const char* ssid = "TEMPFinder";
const char* password = "OPELopel1234567890";
const char* piServer = "192.166.4.11";
const int piPort = 8080;

// Define the delay time between readings
unsigned long delayTime = 250;

// Variables to store the sensor readings
float temperatureC; 
float pressureHPA; 
float altitudeM; 
float humidity; 

void setup() {
  // Initialize the serial console
  Serial.begin(9600);
  Serial.println(F("BME280 test"));

  // Initialize the BME280 sensor
  bool status;
  Wire.begin();
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // Initialize the SSD1306 OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Connect to WiFi
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
  // Read and print the sensor values
  printValues();
  delay(delayTime);
  
  // Send the sensor data to the Raspberry Pi server
  sendToPiServer();
}

void printValues() {
  // Read the sensor values
  temperatureC = bme.readTemperature();
  pressureHPA = bme.readPressure() / 100.0F;
  altitudeM = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();

  // Convert temperature to Fahrenheit
  float temperatureF = temperatureC * 9.0 / 5.0 + 32.0;
  
  // Convert pressure to millibars
  float pressureMB = pressureHPA;
  
  // Convert altitude to feet
  float altitudeFT = altitudeM * 3.28084;

  // Print the sensor values to the serial console
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

  // Clear the OLED display
  display.clearDisplay();

  // Display the WiFi symbol if connected
  if (WiFi.status() == WL_CONNECTED) {
    display.drawBitmap(0, 0, WiFi_Symbol, 16, 16, WHITE);
  } else {
    display.setCursor(0, 0);
    display.print("Connecting to WiFi...");
  }

  // Display the sensor values on the OLED display
  display.setCursor(20, 0);
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

  // Display the server symbol if sending data
  if(client.connected()) {
    display.drawBitmap(100, 0, Server_Symbol, 16, 16, WHITE);
  }

  // Update the OLED display
  display.display();
}

void sendToPiServer() {
  // Create a string to hold the sensor data
  String sensorData = String(temperatureC) + "," + String(pressureHPA) + "," + String(altitudeM) + "," + String(humidity);

  // Convert the string to a character array
  char data[sensorData.length() + 1];
  sensorData.toCharArray(data, sensorData.length() + 1);

  // Create a WiFi client instance
  WiFiClient client;
  
  // Connect to the Raspberry Pi server
  if (client.connect(piServer, piPort)) {
    Serial.println("Connected to the Raspberry Pi server");
    client.print(data);
    client.stop();
  } else {
    Serial.println("Failed to connect to the Raspberry Pi server");
  }
}

// Define the WiFi symbol (16x16 pixels)
const unsigned char WiFi_Symbol[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Define the server symbol (16x16 pixels)
const unsigned char Server_Symbol[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
