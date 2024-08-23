# ES8266Shower

## Shower timer code for the ESP8266 with the additional features.

### Code Overview

1. **Wi-Fi Setup (setupWiFi()):**
   - When the ESP8266 is first powered on, it starts broadcasting an SSID like "ShowerTimer_XX" where "XX" are the last two hex digits of the MAC address.
   - Users can connect to this SSID to access a web form for configuring the ESP8266 to connect to the local Wi-Fi network.
   - If the connection to the local Wi-Fi network fails, the ESP8266 returns to broadcasting the setup SSID.

2. **Flow Monitoring and LED Flashing:**
   - The ESP8266 monitors the water flow using a flow sensor connected to a GPIO pin. It counts pulses from the flow sensor to detect water usage.
   - Based on the time since the flow started, the ESP8266 flashes an LED to indicate different time thresholds (5 minutes, 8 minutes, and 10 minutes).

3. **Web Server (handleRoot()):**
   - Once the ESP8266 connects to the local Wi-Fi, it starts a web server.
   - The server provides a web page that displays the current state (standby, shower in progress), the duration of the current shower, the total number of showers recorded, and the longest shower recorded with a timestamp.

4. **Statistics Tracking (updateStatistics()):**
   - The ESP8266 tracks the number of showers and the duration of the longest shower.
   - It stores the longest shower duration and the timestamp when it occurred.

5. **System Reset (resetSystem()):**
   - After a shower ends, the system resets itself, preparing for the next shower session.

### Steps to Turn This Code into a Functioning Product

1. **Hardware Setup:**
   - **ESP8266 Module:** Choose an appropriate ESP8266 module, such as NodeMCU or Wemos D1 Mini, with sufficient GPIO pins and capabilities.
   - **Flow Sensor:** Connect the flow sensor to the designated GPIO pin (D2 in the example). Ensure it’s calibrated to measure water flow accurately.
   - **LED Indicator:** Connect an LED to the designated GPIO pin (D1 in the example) with an appropriate current-limiting resistor.
   - **Power Supply:** Ensure the ESP8266 is properly powered (typically 3.3V or 5V depending on the board used).

2. **Software Setup:**
   - **Arduino IDE:** Install the necessary libraries (`ESP8266WiFi`, `ESP8266WebServer`, `DNSServer`, `WiFiManager`) through the Arduino Library Manager.
   - **Flashing the Code:** Upload the code to the ESP8266 using the Arduino IDE or another compatible platform.

3. **Wi-Fi Configuration:**
   - Upon first use, connect to the SSID broadcast by the ESP8266 (e.g., "ShowerTimer_XX").
   - Open a web browser and follow the on-screen instructions to connect the ESP8266 to your local Wi-Fi network.

4. **Testing and Calibration:**
   - Test the system by running water through the flow sensor and observing the LED and web page output.
   - Calibrate the flow sensor if necessary to ensure accurate timing
