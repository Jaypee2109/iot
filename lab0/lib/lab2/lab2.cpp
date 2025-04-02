#include <Arduino.h>
#include <Adafruit_CAP1188.h>

#define RED_LED_PIN 25
#define YELLOW_LED_PIN 26
#define GREEN_LED_PIN 27
#define BUZZER_PIN 15

#define LEDC_CHANNEL   0

// Global variables for buzzer timing
unsigned long previousBuzzerMillis = 0;
bool buzzerState = false; // false = off, true = on
unsigned long buzzerInterval = 0;

Adafruit_CAP1188 cap = Adafruit_CAP1188();


boolean sensorTouched(){
  
  uint8_t touched = 0;

  // Read touch sensor status
  touched = cap.touched();  

  // Check if any sensor is touched
  if (touched != 0) {

    Serial.println("Touched!");

    return true;

  } else {

    Serial.println("Not touched.");

    return false;

  }
 
}

void togglePinState(int pinNumber) {
  
  // Read the current state of the pin
  int currentState = digitalRead(pinNumber);

  // Toggle pin state
  if (currentState == HIGH) {

    digitalWrite(pinNumber, LOW);

  } else {

    digitalWrite(pinNumber, HIGH);

  }

}


void buzzer(uint32_t freq, uint8_t volume) {
  
  // Set the frequency
  ledcWriteTone(LEDC_CHANNEL, freq);

  // Set the volume
  ledcWrite(LEDC_CHANNEL, volume);

}


void updateBuzzer() {

  unsigned long currentMillis = millis();

  // Decide the buzzer pattern based on the traffic light state.
  bool isRedMode = (digitalRead(RED_LED_PIN) == HIGH);
  bool isGreenMode = (digitalRead(GREEN_LED_PIN) == HIGH);

  if (isRedMode) {

    // Red mode: 250ms on, 250ms off
    buzzerInterval = 250;

  } else {
    
    // Green mode: 500ms on, 1500ms off
    buzzerInterval = buzzerState ? 500 : 1500;

  }

  if (currentMillis - previousBuzzerMillis >= buzzerInterval) {

    previousBuzzerMillis = currentMillis;

    // Toggle buzzer state
    buzzerState = !buzzerState;  

    if (buzzerState) {

      // Turn buzzer on
      buzzer(500, 100);

    } else {
      
      // Turn buzzer off
      ledcWriteTone(LEDC_CHANNEL, 0);

    }

  }

}


void nonBlockingDelay(unsigned long delayTime) {

  unsigned long startTime = millis();

  while (millis() - startTime < delayTime) {

    // Update state of buzzer
    updateBuzzer();

  }

}


// Red phase pattern
void crossing(){

  // Stay in green for 5 more seconds
  nonBlockingDelay(5000);

  // Stay in yellow for 2 seconds
  togglePinState(GREEN_LED_PIN);
  togglePinState(YELLOW_LED_PIN);
  nonBlockingDelay(2000);

  // Stay in red for 10 seconds
  togglePinState(YELLOW_LED_PIN);
  togglePinState(RED_LED_PIN);
  nonBlockingDelay(10000);

  // Stay in red/yellow for 2 seconds
  togglePinState(YELLOW_LED_PIN);
  nonBlockingDelay(2000);

  // Change to green
  togglePinState(RED_LED_PIN);
  togglePinState(YELLOW_LED_PIN);
  togglePinState(GREEN_LED_PIN);

}


void setup() {

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // Attach the buzzer pin to a LEDC channel
  ledcAttachPin(BUZZER_PIN, LEDC_CHANNEL);

  Serial.begin(115200);
  Serial.println("CAP1188 test!");

  // Initialize the sensor
  if (!cap.begin()) {

    Serial.println("CAP1188 not found");
    while (1);

  }

  Serial.println("CAP1188 found!");

  // Initialize with TL red

  // Stay in red for 10 seconds
  togglePinState(RED_LED_PIN);
  nonBlockingDelay(10000);  

  // Stay in red/yellow for 2 seconds
  togglePinState(YELLOW_LED_PIN);
  nonBlockingDelay(2000); 
  
  // Change to green
  togglePinState(RED_LED_PIN);
  togglePinState(YELLOW_LED_PIN);
  togglePinState(GREEN_LED_PIN);  
    
}


void loop() {

  // Update state of buzzer
  updateBuzzer();

  // Check if sensor is touched
  if(sensorTouched() == true){

    // Start red phase
    crossing();

    // Call sensorTouched again to reset state
    sensorTouched();

  }

}