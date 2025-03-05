#include <Arduino.h>
#include <TFT_eSPI.h>
#include <Adafruit_CAP1188.h>

TFT_eSPI tft = TFT_eSPI();
Adafruit_CAP1188 cap = Adafruit_CAP1188();

void setup() {
 
  // Configure display
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(2);

  Serial.begin(115200);
  Serial.println("CAP1188 test!");

  // Initialize the sensor
  if (!cap.begin()) {

    Serial.println("CAP1188 not found");
    while (1);

  }

  Serial.println("CAP1188 found!");
    
}

void loop() {
    
    uint8_t touched = cap.touched();  // Read touch sensor status
    int count = 0;  // Number of touched sensors

    // Loop through all 8 possible touch inputs
    for (int i = 0; i < 8; i++) {

        if (touched & (1 << i)) {

            count++;

        }
 
    }

    // Print the number of touched sensors to Serial Monitor
    Serial.print("Number of touched sensors: ");
    Serial.println(count);
  
    // Print the number of touched sensors to display
    tft.drawNumber(count, 50, 100, 7);

    delay(10);          
}