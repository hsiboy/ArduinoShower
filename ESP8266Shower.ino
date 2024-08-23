#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Ticker.h>

// Constants
const int flowSensorPin = D2;  // Flow sensor pin
const int ledPin = D1;         // LED pin

// Variables for tracking the shower state
volatile int flowPulseCount = 0;
unsigned long lastFlowTime = 0;
unsigned long flowStartTime = 0;
bool isFlowing = false;
bool flashPersist = false;

// Shower statistics
int showerCount = 0;
unsigned long longestShower = 0;
String longestShowerTime = "";

// Timing thresholds in milliseconds
const unsigned long fiveMinutes = 5 * 60 * 1000;
const unsigned long eightMinutes = 8 * 60 * 1000;
const unsigned long tenMinutes = 10 * 60 * 1000;
const unsigned long oneMinute = 60 * 1000;
const unsigned long twoMinutes = 2 * 60 * 1000;

// Flash intervals
const unsigned long normalFlashInterval = 500;
const unsigned long urgentFlashInterval = 200;
const unsigned long urgentFlashDuration = 3000;

// Web server
ESP8266WebServer server(80);

// Webpage stored in PROGMEM
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Shower Timer</title>
  <script>
    function loadData() {
      var xhr = new XMLHttpRequest();
      xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          var data = JSON.parse(this.responseText);
          document.getElementById("state").innerText = data.state;
          document.getElementById("duration").innerText = data.duration;
          document.getElementById("showerCount").innerText = data.showerCount;
          document.getElementById("longestShower").innerText = data.longestShower;
        }
      };
      xhr.open("GET", "/data", true);
      xhr.send();
    }
    setInterval(loadData, 1000); // Update every second
  </script>
</head>
<body onload="loadData()">
  <h1>Shower Timer Status</h1>
  <p>Current State: <span id="state">Loading...</span></p>
  <p>Shower duration: <span id="duration">0</span> seconds</p>
  <p>Total showers recorded: <span id="showerCount">0</span></p>
  <p>Longest shower: <span id="longestShower">0</span> seconds</p>
</body>
</html>
)rawliteral";

// Function prototypes
void setupWiFi();
void handleRoot();
void handleData();
void countFlowPulses();
void flashLED(unsigned long interval, unsigned long duration = 0);
void resetSystem();
void updateStatistics(unsigned long showerDuration);

void setup() {
  pinMode(flowSensorPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);

  setupWiFi();

  // Attach interrupt to the flow sensor pin
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), countFlowPulses, RISING);

  // Start the web server
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  // Main logic to track shower state
  if (flowPulseCount > 0) {
    if (!isFlowing) {
      isFlowing = true;
      flowStartTime = millis();
      lastFlowTime = flowStartTime;

      // If restarting within 60 seconds of stopping after 10 minutes
      if (millis() - lastFlowTime <= oneMinute && flashPersist) {
        lastFlowTime = tenMinutes + 1;
      } else {
        flashPersist = false;
      }
    }

    unsigned long elapsedTime = millis() - lastFlowTime;

    if (elapsedTime >= tenMinutes) {
      flashLED(urgentFlashInterval);
      flashPersist = true;
    } else if (elapsedTime >= eightMinutes) {
      flashLED(urgentFlashInterval, urgentFlashDuration);
    } else if (elapsedTime >= fiveMinutes) {
      flashLED(normalFlashInterval, 3);
    }

    lastFlowTime = millis();
  } else {
    if (isFlowing) {
      isFlowing = false;
      unsigned long showerDuration = millis() - flowStartTime;
      updateStatistics(showerDuration);
      resetSystem();
    }
  }
}

void setupWiFi() {
  // Initialize WiFiManager
  WiFiManager wifiManager;
  wifiManager.setAPCallback([](WiFiManager *myWiFiManager) {
    String ssid = "ShowerTimer_" + String(WiFi.macAddress()[4], HEX) + String(WiFi.macAddress()[5], HEX);
    myWiFiManager->setConfigPortalTimeout(180);  // 3 minutes timeout
    myWiFiManager->startConfigPortal(ssid.c_str());
  });

  if (!wifiManager.autoConnect()) {
    Serial.println("Failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }

  Serial.println("Connected to WiFi");
}

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleData() {
  String json = "{";
  json += "\"state\":\"" + String(isFlowing ? "Shower in progress" : "Standby") + "\",";
  json += "\"duration\":\"" + String(isFlowing ? (millis() - flowStartTime) / 1000 : 0) + "\",";
  json += "\"showerCount\":\"" + String(showerCount) + "\",";
  json += "\"longestShower\":\"" + String(longestShower / 1000) + "\"";
  json += "}";
  server.send(200, "application/json", json);
}

void countFlowPulses() {
  flowPulseCount++;
}

void flashLED(unsigned long interval, unsigned long duration) {
  unsigned long startTime = millis();
  while (duration == 0 || millis() - startTime < duration) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastFlowTime >= interval) {
      lastFlowTime = currentMillis;
      digitalWrite(ledPin, !digitalRead(ledPin));
    }
  }
  digitalWrite(ledPin, LOW);
}

void resetSystem() {
  digitalWrite(ledPin, LOW);
  flowPulseCount = 0;
  isFlowing = false;
  Serial.println("System reset");
}

void updateStatistics(unsigned long showerDuration) {
  showerCount++;
  if (showerDuration > longestShower) {
    longestShower = showerDuration;
    longestShowerTime = String(day()) + "/" + String(month()) + "/" + String(year()) + " " + String(hour()) + ":" + String(minute());
  }
}
