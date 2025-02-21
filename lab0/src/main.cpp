#include <Arduino.h>
#include <Servo.h>

#define LIGHT_SENSOR_PIN 32
#define LED_PIN 15
#define SERVO_PIN 27
#define CALIBRATION_TIME 10000  // 10 seconds

Servo myServo;  // create servo object to control a servo
int minLight = 4095;  // Initialize with max possible value (12 bit)
int maxLight = 0; // Initialize with min possible value
unsigned long startTime;
bool calibrating = true;

void setup() {

    Serial.begin(115200);
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    myServo.attach(SERVO_PIN);  // attaches the servo on port 27 to the servo object
    myServo.write(90);  // Start with the servo at mid position
    startTime = millis();

    Serial.println("Starting Calibration Phase...");

}

void loop() {

    if (calibrating) {

        digitalWrite(LED_PIN, millis() % 500 < 250); // Blink LED every 500ms
        int lightValue = analogRead(LIGHT_SENSOR_PIN);

        if (lightValue > maxLight) maxLight = lightValue;
        if (lightValue < minLight) minLight = lightValue;
        
        if (millis() - startTime > CALIBRATION_TIME) {

            calibrating = false;
            digitalWrite(LED_PIN, LOW); // Stop blinking

            Serial.print("Minimum Light Value: "); 
            Serial.println(minLight);
            Serial.print("Maximum Light Value: "); 
            Serial.println(maxLight);

        }

        delay(1);

    } else {

        delay(500);

        int lightValue = analogRead(LIGHT_SENSOR_PIN);
        int servoPosition = map(lightValue, minLight, maxLight, 0, 179);
        servoPosition = constrain(servoPosition, 0, 179);
        myServo.write(servoPosition);

        Serial.print("Light Value: "); 
        Serial.println(lightValue);
        Serial.print("Servo Position: "); 
        Serial.println(servoPosition);

    }
    
}