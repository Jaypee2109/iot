#include <credentials.h>
#include <Wire.h>
#include <WiFi.h>
#include <HttpClient.h>
#include <DHT20.h>

const char server[] = "18.188.56.179";
const char path[] = "/sensor";

const int networkTimeout = 30000; // 30 seconds
const int networkDelay = 1000;    // 1 second
const int interval = 5000;        // 5 seconds

// Instantiate DHT20
DHT20 dht20;
unsigned long lastPostTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize WiFi
  Serial.println();
  Serial.printf("Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Wire.begin();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }

  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());

  // Start the DHT20
  dht20.begin(); 
  
  Serial.println("DHT20 sensor initialized");

}

void loop() {

  // Only send data every interval
  if (millis() - lastPostTime >= interval) {
    lastPostTime = millis();

    // Read from the DHT20 sensor
    uint8_t status = dht20.read();

    if (status != 0) {
      Serial.print("DHT20 read failed, status code = ");
      Serial.println(status);

      return; // skip sending if we couldn't read sensor properly

    }

    // Get temperature (C) and humidity (%)
    float temperature = dht20.getTemperature();
    float humidity    = dht20.getHumidity();

    // Check for NaN values
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("DHT20 returned NaN values!");

      return;

    }

    Serial.printf("Temperature: %.2f C  |  Humidity: %.2f %%\n", 
                  temperature, humidity);

    // Build a GET request path with query parameters
    String url = String(path) +
                  "?temperature=" + String(temperature) +
                  "&humidity="    + String(humidity);

    // Perform HTTP GET request
    WiFiClient c;
    HttpClient http(c);

    Serial.println("Sending HTTP GET request to AWS EC2...");
    int err = http.get(server, 5000, url.c_str());

    if (err == 0) {
      Serial.println("HTTP request started successfully");

      // Read the status code
      err = http.responseStatusCode();

      if (err >= 0) {
        Serial.print("Response Status: ");
        Serial.println(err);

        // Skip the headers
        err = http.skipResponseHeaders();

        if (err >= 0) {
          Serial.println("HTTP Response Body:");

          unsigned long timeoutStart = millis();

          while ((http.connected() || http.available()) &&
                 ((millis() - timeoutStart) < networkTimeout)) 
          {
            if (http.available()) {
              char c = http.read();
              Serial.print(c);
              timeoutStart = millis();

            } else {
              delay(networkDelay);

            }

          }

          Serial.println("\n== HTTP Response Received ==");

        } else {
          Serial.print("Failed to skip response headers: ");
          Serial.println(err);

        }

      } else {
        Serial.print("Getting response failed: ");
        Serial.println(err);

      }

    } else {
      Serial.print("Connect failed: ");
      Serial.println(err);

    }

    // Close the HTTP connection
    http.stop();

    // Print next update info
    Serial.print("Next update in (ms): ");
    Serial.println(interval);
    Serial.println();

  }

}
