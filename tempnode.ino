#include <Wire.h> // Library for I2C communication
#include <Adafruit_Sensor.h> // Base class for sensor drivers
#include <Adafruit_BME280.h> // Driver for BME280 sensor
#include <Adafruit_GFX.h> // Generic graphics library for Adafruit displays
#include <Adafruit_SSD1306.h> // Driver for SSD1306 OLED display
#include <ESP8266WiFi.h> // Library for WiFi connectivity on ESP8266

#define SEALEVELPRESSURE_HPA (1013.25) // Sea level pressure in hectopascals

Adafruit_BME280 bme; // Create an instance of the BME280 sensor driver
Adafruit_SSD1306 display(128, 64, &Wire, -1); // Create an instance of the SSD1306 OLED display driver

const char* ssid = "TEMPFinder"; // WiFi network SSID
const char* password = "OPELopel1234567890"; // WiFi network password
const char* piServer = "192.166.4.11"; // IP address of the Raspberry Pi server
const int piPort = 8080; // Port number for the Raspberry Pi server

unsigned long delayTime = 1000; // Delay between measurements in milliseconds

void setup() {
  Serial.begin(9600); // Initialize serial communication at 9600 baud
  Serial.println(F("BME280 test")); // Print a message to the serial monitor

  bool status;

  // Initialize I2C communication
  Wire.begin();
  status = bme.begin(0x76); // Initialize the BME280 sensor
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!"); // Error message if sensor is not found
    while (1); // Loop indefinitely if sensor is not found
  }

  // Initialize the OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay(); // Clear the display
  display.setTextSize(1); // Set the text size to 1
  display.setTextColor(WHITE); // Set the text color to white

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    display.setCursor(0, 0);
    display.print("Connecting to WiFi..."); // Display a message on the OLED display
    display.display();
    delay(500); // Wait for 500ms before retrying
  }

  Serial.println("-- Default Test --");
  Serial.println();
}

void loop() {
  printValues(); // Print the sensor readings to the serial monitor and OLED display
  delay(delayTime); // Wait for the specified delay time
  sendToPiServer(); // Send the sensor readings to the Raspberry Pi server
}

void printValues() {
  // Read sensor values
  float temperatureC = bme.readTemperature(); // Read temperature in Celsius
  float pressureHPA = bme.readPressure() / 100.0F; // Read pressure in hectopascals
  float altitudeM = bme.readAltitude(SEALEVELPRESSURE_HPA); // Read altitude in meters
  float humidity = bme.readHumidity(); // Read humidity in percent

  // Convert temperature to Fahrenheit
  float temperatureF = temperatureC * 9.0 / 5.0 + 32.0;

  // Convert pressure to millibars
  float pressureMB = pressureHPA;

  // Convert altitude to feet
  float altitudeFT = altitudeM * 3.28084;

  // Print sensor readings to the serial monitor
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

  // Display sensor readings on the OLED display
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
  // Create a string with the sensor readings
  String sensorData = String(temperatureC) + "," + String(pressureHPA) + "," + String(altitudeM) + "," + String(humidity);

  // Convert the string to a character array
  char data[sensorData.length() + 1];
  sensorData.toCharArray(data, sensorData.length() + 1);

  // Connect to the Raspberry Pi server
  WiFiClient client;
  if (client.connect("192.168.1.100", portNumber)) { // Replace "192.168.1.100" with the IP address of your Raspberry Pi
    Serial.println("Connected to the Raspberry Pi server");
    client.print(data); // Send the sensor readings to the Raspberry Pi server
    client.stop(); // Close the connection to the Raspberry Pi server
  } else {
    Serial.println("Failed to connect to the Raspberry Pi server");
  }
}
