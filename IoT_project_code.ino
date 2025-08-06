#include <DHT.h>
#include <WiFi.h>
#include <WebServer.h>

// Define pins
#define DHT_PIN 2        // DHT22 pin
#define MQ135_PIN 34     // MQ135 analog pin
#define RELAY_PIN 19     // Relay pin (NO state)
#define RED_LED_PIN 22   // Red LED pin
#define GREEN_LED_PIN 23 // Green LED pin
bool relayState = HIGH;
// WiFi credentials
const char* ssid = "Avengers";
const char* password = "";

// Initialize server
WebServer server(50);

// DHT setup
#define DHTTYPE DHT22
DHT dht(DHT_PIN, DHTTYPE);

// Thresholds
float maxTemperature = 30.0; // Max temperature in °C
float maxHumidity = 70.0;    // Max humidity in %
int maxAirQuality = 25;     // Max air quality in ppm

// Variables
float temperature, humidity;
int airQuality;
bool alarmActive = false;

void setup() {
  Serial.begin(9600);
  dht.begin();

  // Initialize pins
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // Set initial states
  digitalWrite(RELAY_PIN, HIGH);      // Fan OFF (default state)
  digitalWrite(RED_LED_PIN, LOW);    // Red light OFF
  digitalWrite(GREEN_LED_PIN, HIGH); // Green light ON initially

  connectToWiFi();
  setupWebServer();
}

void loop() {
  monitorSensors();
  controlSystem();
  server.handleClient();
}

// Connect to WiFi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Setup web server
void setupWebServer() {
  server.on("/", []() {
    
    bool isCritical = (temperature > maxTemperature || humidity > maxHumidity || airQuality > maxAirQuality);

    // Build the HTML response
    String html = "<html><head><title>Monitoring System</title>";
    html += "<style>";
    html += "body { background-color: black; color: white; text-align: center; font-family: Arial, sans-serif; }";
    html += "table { margin: 20px auto; border-collapse: collapse; }";
    html += "th, td { border: 1px solid white; padding: 10px; }";
    html += "button { padding: 10px 20px; font-size: 16px; cursor: pointer; }";
    html += ".indicator { width: 20px; height: 20px; border-radius: 50%; display: inline-block; margin-left: 10px; }";
    html += ".green { background-color: green; }";
    html += ".red { background-color: red; }";
    html += "</style></head><body>";

    html += "<h1>Environment Monitoring</h1>";

    // Display data in a table
    html += "<table>";
    html += "<tr><th>Parameter</th><th>Value</th><th>Threshold</th></tr>";
    html += "<tr><td>Temperature</td><td>" + String(temperature) + " °C</td><td>" + String(maxTemperature) + " °C</td></tr>";
    html += "<tr><td>Humidity</td><td>" + String(humidity) + " %</td><td>" + String(maxHumidity) + " %</td></tr>";
    html += "<tr><td>Air Quality</td><td>" + String(airQuality) + " ppm</td><td>" + String(maxAirQuality) + " ppm</td></tr>";
    html += "</table>";

    // Add the status indicators
    html += "<p><b>Status:</b>";
    html += "<span class='indicator " + String(isCritical ? "red" : "green") + "'></span>";
    html += "</p>";

    // Add a button to control the relay
    html += "<form action='/toggle' method='POST'>";
    html += "<button type='submit'>" + String(relayState == HIGH ? "Turn ON" : "Turn OFF") + " Relay</button>";
    html += "</form>";

    html += "</body></html>";

    server.send(200, "text/html", html);
  });

  // Route to toggle the relay
  server.on("/toggle", HTTP_POST, []() {
    relayState = !relayState; // Toggle relay state
    digitalWrite(RELAY_PIN, relayState);
    server.sendHeader("Location", "/"); // Redirect back to the main page
    server.send(303);
  });

  server.begin();
  Serial.println("Web server started.");
}



// Read DHT22 sensor
void readDHT22() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}

// Read MQ135 sensor
void readMQ135() {
  int sensorValue = analogRead(MQ135_PIN);
  airQuality = map(sensorValue, 0, 4095, 0, 500); // Adjusted for ESP32 ADC resolution (12-bit)

  Serial.print("Air Quality: ");
  Serial.print(airQuality);
  Serial.println(" ppm");
}

// Monitor sensors
void monitorSensors() {
  readDHT22();
  readMQ135();
    delay(5000);
}

// Control the relay and LEDs based on thresholds
void controlSystem() {
  if (temperature > maxTemperature || humidity > maxHumidity || airQuality > maxAirQuality) {
    alarmActive = true;
    relayState = LOW;
    digitalWrite(RELAY_PIN, LOW);   // Turn ON fan
    digitalWrite(RED_LED_PIN, HIGH); // Red light ON
    digitalWrite(GREEN_LED_PIN, LOW); // Green light OFF
    Serial.println("ALARM: Threshold exceeded!");
  } else {
    alarmActive = false;
    relayState = HIGH;
    digitalWrite(RELAY_PIN, HIGH);    // Turn OFF fan
    digitalWrite(RED_LED_PIN, LOW);  // Red light OFF
    digitalWrite(GREEN_LED_PIN, HIGH); // Green light ON
    Serial.println("System is normal.");
  }
}
