# ArduinoShower
An arduino powered shower timer -This project can be installed in showers to help manage water usage and encourage shorter showers.

### Explanation of the Code

The code provided is designed to monitor the water flow through a shower using a flow sensor connected to an Arduino. The primary goal is to manage water usage by triggering visual alerts (flashing an LED) after specific durations of continuous water flow. Here's a breakdown of what the code does:

1. **Flow Sensor Setup:**
   - The flow sensor is connected to an interrupt pin on the Arduino. It generates pulses as water flows through it, which the Arduino counts to detect water usage.

2. **Timing Logic:**
   - The code measures the elapsed time since the shower was turned on.
   - It tracks different time thresholds:
     - **After 5 minutes:** The LED flashes slowly 3 times to signal that the user has been showering for 5 minutes.
     - **After 8 minutes:** The LED flashes at a faster rate (200ms intervals) for 3 seconds to indicate that the user has been showering for 8 minutes.
     - **After 10 minutes:** The LED starts flashing rapidly and continuously (200ms intervals) as a more urgent signal.

3. **Persistence Logic:**
   - If the shower is turned off after the 10-minute mark and then restarted within 60 seconds, the rapid flashing continues immediately, discouraging users from bypassing the system by briefly stopping the shower.
   - If the shower is not restarted within 60 seconds, the system resets after 2 minutes of inactivity, allowing a fresh cycle for a new user.

4. **System Reset:**
   - After 2 minutes of no flow, the system fully resets, clearing any prior state. This ensures that a new shower session starts with no leftover alerts from a previous session.
  
---

### Steps to Turn the Code into a Functioning Product

1. **Hardware Components:**
   - **Flow Sensor:** Choose a suitable flow sensor compatible with the Arduino. Ensure it can accurately measure the flow rate of the shower.
   - **Arduino Board:** Select an appropriate Arduino model (e.g., Arduino Uno) to run the code.
   - **LED Indicator:** Use a bright LED or a similar visual indicator connected to the Arduino. Optionally, consider using a buzzer for an audible alert.
   - **Power Supply:** Ensure the Arduino and any connected peripherals (like the LED) have a stable power source. If the system is to be used in a bathroom, waterproofing and safety must be considered.

2. **Circuit Design:**
   - **Wiring:** Connect the flow sensor to the Arduino, ensuring it is attached to an interrupt-capable pin (e.g., pin 2). Connect the LED to an appropriate digital pin (e.g., pin 13), with a resistor to limit current.
   - **Enclosure:** Design or select a waterproof enclosure for the Arduino and associated circuitry to protect the electronics from water exposure.

3. **Testing and Calibration:**
   - **Calibration:** Calibrate the flow sensor to ensure accurate timing based on real shower water flow rates.
   - **Testing:** Thoroughly test the system in a controlled environment to verify that the LED flashing occurs at the correct intervals and that the persistence logic works as expected.

4. **User Interface:**
   - **Feedback Mechanism:** Consider adding a user-friendly interface, such as a small display or additional LEDs, to show how much time is left before each alert. This could improve user compliance with water-saving goals.

5. **Safety Considerations:**
   - **Electrical Safety:** Implement safeguards to ensure the system is safe to use in a wet environment, possibly integrating with a low-voltage or battery-powered system.
   - **Waterproofing:** Ensure that all components exposed to moisture are properly sealed.

6. **Installation:**
   - **Integration with Shower:** Install the flow sensor in the water supply line of the shower. Ensure that the installation does not restrict water flow or cause leaks.
   - **Mounting:** Mount the Arduino and LED in a visible but safe location within the bathroom, ensuring they are easy to see but not at risk of water exposure.

---

### Testing

Let's walk through the scenario where you shower for 6 minutes and 45 seconds and then turn the shower off.

#### Step-by-Step Breakdown

1. **Shower On:**
   - You turn on the shower. The flow sensor detects water flow, and the Arduino begins counting the time from when the flow started (`lastFlowTime` is set).

2. **Shower Duration:**
   - The Arduino continuously monitors the elapsed time.
   - The code has checks for different time thresholds (5 minutes, 8 minutes, and 10 minutes).

3. **6 Minutes and 45 Seconds:**
   - At the 5-minute mark, the LED flashes slowly 3 times (flashing at 500ms intervals). This indicates that you have been showering for 5 minutes.
   - After that, no other flashing occurs because you haven't reached the 8-minute mark (which triggers a 3-second urgent flash) or the 10-minute mark (which triggers continuous rapid flashing).

4. **Shower Off:**
   - You turn the shower off after 6 minutes and 45 seconds.
   - The flow sensor stops detecting water flow, and the variable `isFlowing` is set to `false`.
   - The `flowStopTime` is recorded with the current time, marking when the water flow stopped.

5. **LED State:**
   - The LED is turned off (`digitalWrite(ledPin, LOW);`), and it remains off since the flow has stopped.

6. **Timer and State After the Shower:**
   - The timer (`lastFlowTime`) is effectively paused because the water flow has stopped, but the system hasn't reset yet.
   - The system enters a waiting period. If the shower is restarted within 2 minutes, it would continue from where it left off.
   - Since the shower was turned off before the 10-minute mark, the persistence logic does not trigger, and no flashing persists.
   - If the shower remains off for 2 minutes, the system fully resets, and `lastFlowTime`, `flowPulseCount`, and other state variables are cleared.

#### Summary of What You Would See:

- **During the Shower:** After 5 minutes, the LED flashes slowly 3 times, then remains off.
- **After the Shower:** Once you turn off the shower at 6 minutes and 45 seconds, the LED turns off, and the system is in a "paused" state. If you don't restart the shower within 2 minutes, the system will reset and be ready for a new session.

#### Final State:

- **LED:** Off.
- **System Timer (`lastFlowTime`):** Paused.
- **System State:** Waiting for either a restart within 2 minutes or a full reset after 2 minutes.

---


### Summary

This project involves building a water flow monitoring system for a shower, using an Arduino to alert users when they've been showering for an extended period. 
