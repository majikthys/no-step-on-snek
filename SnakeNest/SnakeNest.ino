// Snake Nest is built for an Arduino Nano
// Monitor hand detector pin and turn on solenoid-relay, light-relay, and sound-relay for a duration
// Then turn off solenoid, lights, and sound
// #define DEBUG

#include <Bounce2.h>

// Pin Definitions
const int HAND_DETECTOR_PIN = 8;           // Hand detection sensor
const int SOLENOID_PIN = 15;                // Relay controlling solenoid
const int JUMP_SCARE_LIGHT_PIN = 19;        // Relay controlling jump scare lights
const int CHILL_LIGHT_PIN = 17;             // Relay controlling ambient lights
const int JUMP_SCARE_SOUND_PIN = 10;        // Relay controlling jump scare sound
const int CHILL_SOUND_PIN = 12;             // Relay controlling ambient sound

// Create button instance for hand detector
Bounce2::Button handDetector = Bounce2::Button();

// Timing constants (in milliseconds)
const unsigned long SNAKE_ACTIVATION_DURATION = 750;  // How long to keep everything activated in ms

const unsigned long ACTIVATION_DURATION = 2200;  // How long to keep everything activated in ms
const unsigned long DEBOUNCE_INTERVAL = 2;       // Debounce interval in ms

void log(const char* message) {
  #ifdef DEBUG
    Serial.println(message);
  #endif
}

void activateSequence() {
  log("Activating nest");
  
  // Turn on jump scare relays, turn off chill relays
  digitalWrite(SOLENOID_PIN, HIGH);
  digitalWrite(JUMP_SCARE_LIGHT_PIN, HIGH);
  digitalWrite(JUMP_SCARE_SOUND_PIN, HIGH);
  digitalWrite(CHILL_LIGHT_PIN, LOW);
  digitalWrite(CHILL_SOUND_PIN, LOW);
  
  // Wait for duration
  delay(SNAKE_ACTIVATION_DURATION);
  digitalWrite(SOLENOID_PIN, LOW);

  delay(ACTIVATION_DURATION - SNAKE_ACTIVATION_DURATION);
  
  // Turn off jump scare relays, turn on chill relays
  digitalWrite(JUMP_SCARE_LIGHT_PIN, LOW);
  digitalWrite(JUMP_SCARE_SOUND_PIN, LOW);
  digitalWrite(CHILL_LIGHT_PIN, HIGH);
  digitalWrite(CHILL_SOUND_PIN, HIGH);
  
  log("Nest deactivated");
}


void debug_pins() {
  // Go through each interesting pin and toggle it on and off while announting on the serial monitor

  // SOLENOID_PIN
  digitalWrite(SOLENOID_PIN, HIGH);
  log("SOLENOID_PIN HIGH");
  delay(1000);
  digitalWrite(SOLENOID_PIN, LOW);
  log("SOLENOID_PIN LOW");
  delay(1000);

  // JUMP_SCARE_LIGHT_PIN
  digitalWrite(JUMP_SCARE_LIGHT_PIN, HIGH);
  log("JUMP_SCARE_LIGHT_PIN HIGH");
  delay(2000);
  digitalWrite(JUMP_SCARE_LIGHT_PIN, LOW);
  log("JUMP_SCARE_LIGHT_PIN LOW");
  delay(2000);


  // CHILL_LIGHT_PIN
  digitalWrite(CHILL_LIGHT_PIN, HIGH);
  log("CHILL_LIGHT_PIN HIGH");
  delay(2000);
  digitalWrite(CHILL_LIGHT_PIN, LOW);
  log("CHILL_LIGHT_PIN LOW");
  delay(2000);

  // JUMP_SCARE_SOUND_PIN
  digitalWrite(JUMP_SCARE_SOUND_PIN, HIGH);
  log("JUMP_SCARE_SOUND_PIN HIGH");
  delay(2000);
  digitalWrite(JUMP_SCARE_SOUND_PIN, LOW);
  log("JUMP_SCARE_SOUND_PIN LOW");
  delay(2000);

  // CHILL_SOUND_PIN
  digitalWrite(CHILL_SOUND_PIN, HIGH);
  log("CHILL_SOUND_PIN HIGH");
  delay(2000);
  digitalWrite(CHILL_SOUND_PIN, LOW);
  log("CHILL_SOUND_PIN LOW");
  delay(2000);
}

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  log("Snake Nest Initializing...");

  // Configure input pin with debouncing
  handDetector.attach(HAND_DETECTOR_PIN, INPUT_PULLUP);
  handDetector.interval(DEBOUNCE_INTERVAL);
  handDetector.setPressedState(LOW);  // Sensor triggers LOW
  
  // Configure output pins
  pinMode(SOLENOID_PIN, OUTPUT);
  pinMode(JUMP_SCARE_LIGHT_PIN, OUTPUT);
  pinMode(JUMP_SCARE_SOUND_PIN, OUTPUT);
  pinMode(CHILL_LIGHT_PIN, OUTPUT);
  pinMode(CHILL_SOUND_PIN, OUTPUT);
  
  #ifdef DEBUG
    debug_pins();
  #endif

  // Ensure everything starts in chill mode
  digitalWrite(SOLENOID_PIN, LOW);
  digitalWrite(JUMP_SCARE_LIGHT_PIN, LOW);
  digitalWrite(JUMP_SCARE_SOUND_PIN, LOW);
  digitalWrite(CHILL_LIGHT_PIN, HIGH);
  digitalWrite(CHILL_SOUND_PIN, HIGH);
  
  // Initial button state
  handDetector.update();
}

void loop() {
  // Update hand detector state
  handDetector.update();

  // Check for hand detection
  if (handDetector.pressed()) {
    log("Hand detected, starting sequence...");
    activateSequence();
    log("Sequence complete");
  }
}



