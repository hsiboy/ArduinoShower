// Define the pins for the flow sensor and the output LED
const int flowSensorPin = 2;  // Pin where the flow sensor is connected (interrupt pin) - uses a hall effect sensor with turbine.
const int ledPin = 13;        // Pin where the LED is connected

// Variables for counting pulses and timing
volatile int flowPulseCount = 0;
unsigned long lastFlowTime = 0;
unsigned long lastBlinkTime = 0;
unsigned long flowStopTime = 0;
bool isFlowing = false;
bool ledState = LOW;
bool flashPersist = false; // Tracks if flashing should persist

// Timing thresholds in milliseconds
const unsigned long fiveMinutes = 5 * 60 * 1000;
const unsigned long eightMinutes = 8 * 60 * 1000;
const unsigned long tenMinutes = 10 * 60 * 1000;
const unsigned long oneMinute = 60 * 1000;
const unsigned long twoMinutes = 2 * 60 * 1000;

// Flash intervals
const unsigned long normalFlashInterval = 500; // 0.5 seconds
const unsigned long urgentFlashInterval = 200; // 0.2 seconds
const unsigned long urgentFlashDuration = 3000; // 3 seconds duration

void setup() {
  pinMode(flowSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // Attach interrupt to the flow sensor pin
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), countFlowPulses, RISING);

  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Check if flow has started
  if (flowPulseCount > 0) {
    if (!isFlowing) {
      isFlowing = true;
      lastFlowTime = millis(); // Start timing the flow

      // Check if restarting within 60 seconds of stopping after 10 minutes
      if (millis() - flowStopTime <= oneMinute && flashPersist) {
        lastFlowTime = tenMinutes + 1; // Force the system to start flashing immediately
      } else {
        flashPersist = false; // Reset persist flag if restarting after more than 60 seconds
      }
    }

    unsigned long elapsedTime = millis() - lastFlowTime;

    if (elapsedTime >= tenMinutes) {
      // Flash rapidly indefinitely
      flashLED(urgentFlashInterval);
      flashPersist = true; // Set the flag to persist flashing on restart
    } else if (elapsedTime >= eightMinutes) {
      // Flash at urgent rate for 3 seconds
      flashLED(urgentFlashInterval, urgentFlashDuration);
    } else if (elapsedTime >= fiveMinutes) {
      // Flash normally 3 times, then wait
      flashLED(normalFlashInterval, 3);
    }

    flowStopTime = 0; // Reset flow stop timer as the flow is active
  } else {
    // Handle flow stop
    if (isFlowing) {
      isFlowing = false;
      flowStopTime = millis(); // Record the time when the flow stopped
    }

    // Check if the system should reset after 2 minutes of no flow
    if (flowStopTime != 0 && millis() - flowStopTime >= twoMinutes) {
      resetSystem(); // Reset the system for a new shower cycle
    }

    digitalWrite(ledPin, LOW); // Ensure LED is off when there's no flow
  }

  // Reset flowPulseCount periodically to avoid overflow
  if (millis() - lastFlowTime > 1000) {
    flowPulseCount = 0;
  }
}

// Interrupt service routine to count the pulses from the flow sensor
void countFlowPulses() {
  flowPulseCount++;
}

// Function to flash the LED
void flashLED(unsigned long interval, unsigned long duration = 0) {
  unsigned long startTime = millis();
  while (duration == 0 || millis() - startTime < duration) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastBlinkTime >= interval) {
      lastBlinkTime = currentMillis;
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    }
    if (duration > 0 && millis() - startTime >= duration) break;
  }
  digitalWrite(ledPin, LOW); // Turn off the LED after flashing
}

// Function to reset the system
void resetSystem() {
  isFlowing = false;
  flashPersist = false;
  lastFlowTime = 0;
  flowStopTime = 0;
  flowPulseCount = 0;
  digitalWrite(ledPin, LOW);
  Serial.println("System reset for new shower cycle.");
}
