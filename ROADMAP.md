# Motor Controller - Mobile App Control Roadmap

## What We Have Today

### 1. Timer-Based Motor Controller (`timer` file)
- **Hardware**: Arduino Nano/Uno + TM1637 7-segment display + rotary encoder + relay
- **What it does**: Farmer turns the rotary knob to set a countdown timer (e.g., 2 hours 30 minutes). After 3 seconds of no input, the relay turns ON (motor starts). When countdown hits 00:00, relay turns OFF (motor stops). Display shows "OFF".
- **Smart features**: Saves timer to EEPROM on power loss (AC detect pin), resumes countdown on reboot. Battery backup keeps the board alive briefly during outage to save state. 3-phase motor feedback pin adds safety debounce when motor is running.
- **Pins used**: TM1637 (8, 9), Relay (12), Encoder (2, 3), AC Detect (4), 3-Phase feedback (10), Battery backup (6)

### 2. Blynk GSM Motor Controller (`blynk` file)
- **Hardware**: Arduino Nano/Uno + SIM800L GSM module + relay + physical switch
- **What it does**: Connects to Blynk IoT cloud via SIM800L (2G GPRS). Farmer can control relay ON/OFF from Blynk mobile app (virtual pin V1). Physical switch on pin 10 also toggles the relay locally.
- **Monitoring**: Reads fault detection (pin 7), "all OK" status (pin 8), motor on/off status (pin 9), motor off feedback (pin 12). Sends LED indicators to Blynk app (V2, V3, V4). Logs events: `all_ok`, `fault_detect`, `motor_off`, `online_`.
- **Watchdog**: Hardware Timer1 at 1Hz counts seconds. If Blynk stays disconnected for 150 seconds (2.5 min), MCU resets itself. Prevents stuck offline state.
- **Connection**: SIM800L on SoftwareSerial (pins 2, 3), connects to `blynk.cloud:8080`, APN = "internet"

### Current Limitation
These are **two separate systems** on two separate Arduino boards. The timer controller has no internet. The Blynk controller has no timer/display. Neither has been combined into one.

---

## What We Want To Build

A **single unified system** where the farmer can:
1. Control motor ON/OFF from a **mobile app** (Android APK)
2. Set a **countdown timer** from the app (not just the physical rotary knob)
3. See **motor status** in real-time (ON/OFF, fault, all-ok)
4. Still use the **physical rotary encoder + display** as a local backup (works even without internet)
5. Get **notifications** on phone for faults, motor off, timer complete

---

## Approach Options (Choose One)

### Option A: Stay on Blynk (Recommended for POC)

**What is Blynk?** A cloud platform that provides a ready-made mobile app. You don't build an app from scratch -- you drag-and-drop buttons/sliders/gauges in Blynk's app builder, and they automatically talk to your hardware.

| Aspect | Details |
|--------|---------|
| **Cost** | Free tier: 5 devices, 1 user, 100K messages/month. Plenty for POC |
| **Mobile app** | Already built by Blynk. Android + iOS. Just add widgets |
| **Server** | Blynk Cloud (managed by them). No AWS/VPS/hosting needed |
| **Complexity** | LOW -- your `blynk` file already connects to it. Just add timer widgets |
| **Time to POC** | 1-2 weeks |
| **Scalability** | Up to 5 devices free. $29/month for 10 devices. Good enough for 1-10 farmers |
| **Custom APK** | No. Farmer uses the standard Blynk app from Play Store |

**Why recommended**: You already have working Blynk code. The Blynk app already has timer widgets, button widgets, LED indicators, and notifications built in. Adding timer control from the app is literally adding a few virtual pin handlers in your firmware code. Minimum new learning needed.

**Limitation**: Farmer must install the Blynk app (not your own branded app). UI is customizable but within Blynk's widget system.

---

### Option B: Firebase + Custom Flutter App

**What is this?** You build your own Android app (using Flutter or React Native) that talks to Google Firebase (a cloud database). Your hardware sends data to Firebase, the app reads from Firebase. Full control over the UI.

| Aspect | Details |
|--------|---------|
| **Cost** | Firebase Spark (free): 1GB storage, 50K reads/day, 20K writes/day. $0/month |
| **Mobile app** | You build it yourself (Flutter/React Native). Full custom UI |
| **Server** | Firebase (Google Cloud). No AWS/VPS needed |
| **Complexity** | MEDIUM-HIGH -- need to learn Flutter, Firebase, REST API on the MCU |
| **Time to POC** | 4-8 weeks |
| **Scalability** | Excellent. Firebase scales automatically. Pay-as-you-go beyond free tier |
| **Custom APK** | Yes. Your own branded app on Play Store |

**Why consider**: If you want a fully branded app with your own name/logo, or if you plan to sell this as a product to many farmers. Also, Firebase's free tier is extremely generous and never expires.

**Limitation**: You must build and maintain the app yourself. The hardware-to-Firebase communication requires HTTP or MQTT, which is heavier than Blynk's lightweight protocol on 2G.

---

### Option C: Self-Hosted MQTT + Custom App

**What is this?** You rent a small cloud server ($5/month VPS), run Mosquitto MQTT broker on it, build your own app, and your hardware publishes/subscribes to MQTT topics.

| Aspect | Details |
|--------|---------|
| **Cost** | ~$5/month (DigitalOcean/Vultr VPS) |
| **Mobile app** | You build it yourself |
| **Server** | You manage it yourself |
| **Complexity** | HIGH -- server admin, MQTT setup, app development, security |
| **Time to POC** | 6-12 weeks |
| **Scalability** | Best. No per-device cost. But you maintain everything |
| **Custom APK** | Yes |

**Why consider**: Only if scaling to 50+ devices and you want zero per-device cloud cost. Overkill for a POC.

---

## Our Recommendation: Option A (Blynk) for POC, Migrate to Option B Later

**Phase 1 (now)**: Build the combined system on Blynk. Get it working, test with real farmers.
**Phase 2 (later, if scaling)**: If you need a custom branded app or the business grows beyond 5-10 units, migrate to Firebase + Flutter. The hardware firmware barely changes -- only the cloud communication layer swaps out.

---

## Hardware Plan

### What We Need to Buy

#### MCU (Microcontroller) -- DECISION NEEDED

**Current**: Arduino Nano (ATmega328P) -- 2KB RAM, 32KB Flash, no WiFi/BLE, no built-in connectivity.

**Problem**: Running SIM800L + Blynk + timer + display + encoder on an Arduino Nano with 2KB RAM is tight. Adding more features (OTA updates, security) will be difficult.

| Option | Price (INR) | Pros | Cons |
|--------|-------------|------|------|
| **Keep Arduino Nano + SIM800L** (current) | ~1,700 total | Already working, cheapest | 2KB RAM limit, no OTA, 2G only |
| **STM32G0B1CCT6** (your custom PCB) | Depends on PCB | 144KB RAM, 256KB Flash, powerful timers, industrial grade | Need to port all code from Arduino to STM32 HAL. More complex. No built-in connectivity -- still needs SIM module |
| **ESP32 + SIM800L (TTGO T-Call)** | ~2,000 | 520KB RAM, WiFi+BLE+GSM in one board, OTA support, Arduino compatible (easy port) | Slightly higher power consumption |
| **ESP32 + SIM7600 (LILYGO T-SIM7600)** | ~4,000 | 4G future-proof, GPS, ESP32 power | Most expensive |

#### Question for Hardware Friend:
> The custom PCB uses STM32G0B1CCT6. Is this PCB already designed/manufactured with SIM800L or another GSM module on it? Or is the STM32 PCB only for the timer/display/relay part, and connectivity is still TBD?
>
> If the STM32 PCB doesn't have a GSM module, we need to decide: add a SIM module to the STM32 board, or use a separate connectivity board (ESP32-based) that talks to the STM32 over UART/SPI.

#### GSM Module -- CRITICAL DECISION

| Module | Network | Price (INR) | Future-Proof | Notes |
|--------|---------|-------------|-------------|-------|
| **SIM800L** (current) | 2G GPRS | 500-800 | **NO** -- Jio has NO 2G. Airtel/Vi may shut down 2G by 2027 | Works today on Airtel/Vi. Cheapest option |
| **SIM7600E** | 4G LTE + 2G fallback | 2,500-4,500 | Yes | Recommended if building new hardware |
| **A7672S** | 4G + 2G, dual SIM | 2,200-3,500 | Yes | Good balance of cost and features |

#### Question for Hardware Friend:
> Which SIM module is on the custom PCB? SIM800L or SIM7600 or something else?
> If SIM800L: are we okay with the 2G sunset risk, or should we redesign for SIM7600/A7672S?
> Which telecom carrier's SIM will we use? Jio (4G only, no 2G), Airtel, Vi?

#### Other Components

| Component | Already Have? | Notes |
|-----------|--------------|-------|
| TM1637 Display | Yes (from timer build) | Keep for local display |
| Rotary Encoder | Yes (from timer build) | Keep for local time setting |
| Relay Module | Yes (both builds) | Single channel, 5V |
| ST-Link V2 debugger | **NEED TO BUY** if using STM32 | ~INR 300-500 for clone. Required to flash STM32 |
| SIM card (data plan) | Need active SIM | Airtel/Vi for 2G (SIM800L) or Jio for 4G (SIM7600) |
| Power supply | **Question** | What powers the box? 12V? 5V? Solar? Grid AC with adapter? |
| Antenna for GSM | Usually included with SIM module | Ensure it's rated for the frequency band |

---

## Software Plan

### Phase 1: Combine Timer + Blynk into One Firmware

**Goal**: Single firmware that does everything both current sketches do, plus timer control from the app.

#### Blynk App Widgets (what the farmer sees on phone):

| Widget | Virtual Pin | Function |
|--------|-------------|----------|
| Button | V1 | Motor ON/OFF toggle |
| LED | V2 | "All OK" indicator (green) |
| LED | V3 | "Fault" indicator (red) |
| LED | V4 | Motor running indicator |
| Slider or Numeric Input | V10 | Set timer hours (0-12) |
| Slider or Numeric Input | V11 | Set timer minutes (0-50, step 10) |
| Button | V12 | Start countdown from app |
| Value Display | V13 | Show remaining time (HH:MM) |
| Notification | -- | Push notification when timer ends, fault detected, motor off |

#### Firmware Architecture:

```
+--------------------------------------------------+
|                MAIN CONTROLLER                     |
|                                                    |
|  +------------+  +------------+  +--------------+  |
|  | Timer       |  | Blynk/GSM  |  | Display      |  |
|  | Engine      |  | Connection |  | Manager      |  |
|  |             |  |            |  |              |  |
|  | - countdown |  | - V1 relay |  | - TM1637     |  |
|  | - hours/min |  | - V2-V4    |  | - show time  |  |
|  | - EEPROM    |  |   status   |  | - show OFF   |  |
|  | - AC detect |  | - V10-V13  |  |              |  |
|  +------------+  |   timer    |  +--------------+  |
|                   | - events   |                    |
|  +------------+  | - watchdog |  +--------------+  |
|  | Encoder     |  +------------+  | Relay        |  |
|  | Handler     |                   | Controller   |  |
|  | - ISR       |                   | - safety     |  |
|  | - debounce  |                   | - feedback   |  |
|  +------------+                   +--------------+  |
+--------------------------------------------------+
```

#### Key Logic:
- Timer can be set from **either** the rotary encoder (local) **or** the app (V10/V11/V12)
- Whichever source sets the timer last wins (local encoder overrides app, app overrides encoder)
- Remaining time is synced back to the app every 60 seconds (V13)
- Motor ON/OFF works from both physical switch and app button (V1)
- All safety features (fault detect, AC detect, EEPROM save, watchdog) remain unchanged
- If Blynk disconnects, local operation continues normally (encoder + display still work)

### Phase 2: Port to STM32 (if using custom PCB)

If the final hardware is the STM32G0B1CCT6 custom PCB:
1. Port Arduino code to STM32 HAL (or use STM32duino for easier transition)
2. Configure pins in STM32CubeMX (we already have this set up in VS Code)
3. Replace `SoftwareSerial` with STM32 hardware UART for GSM module
4. Replace `EEPROM.h` with STM32 Flash or external EEPROM
5. Replace AVR timer interrupts with STM32 timer configuration
6. Replace `attachInterrupt` with STM32 EXTI configuration

**Estimated effort**: 2-3 weeks if experienced with STM32, 4-6 weeks if learning.

### Phase 3: Future Features (After POC Works)

| Feature | Difficulty | Description |
|---------|-----------|-------------|
| **Scheduling** | Medium | Set recurring schedules ("run motor daily 6AM-8AM") via app |
| **Multi-motor** | Medium | Control 2-3 motors from one board (add relay channels) |
| **Power monitoring** | Medium | Add SCT-013 current sensor to detect dry-run / overload |
| **Water level** | Low-Medium | Add ultrasonic sensor in tank to show water level in app |
| **OTA updates** | Easy (ESP32) / Hard (STM32) | Update firmware remotely without visiting the farm |
| **Voice control** | Low | Integrate with Google Assistant via Blynk |
| **Multi-language** | App-side | Hindi/regional language support in the app |
| **Data logging** | Low | Log motor run hours, power cycles, faults over time |
| **SMS fallback** | Medium | If farmer has no smartphone, send SMS commands instead |

---

## Cost Summary

### Option A: Blynk POC (Minimum Viable Product)

| Item | Cost (INR) | One-time / Recurring |
|------|-----------|---------------------|
| Hardware (if keeping Arduino + SIM800L) | 0 (already have) | One-time |
| Hardware (if new ESP32 + SIM800L board) | ~2,000 | One-time |
| Hardware (if using STM32 custom PCB + SIM module) | Depends on PCB | One-time |
| ST-Link debugger (if STM32) | ~400 | One-time |
| SIM card + data plan | ~200/month | Recurring |
| Blynk Cloud (free tier, up to 5 devices) | $0 | Recurring |
| Blynk Cloud (Starter, if >5 devices) | $29/month (~INR 2,400/month) | Recurring |
| **Total POC (existing hardware)** | **~INR 200/month** (just SIM data) | |

### Option B: Firebase + Custom App (Later)

| Item | Cost (INR) | One-time / Recurring |
|------|-----------|---------------------|
| All hardware above | Same | |
| Firebase (free tier) | $0 | Recurring |
| Flutter app development | Your time / developer cost | One-time |
| Google Play Store listing | $25 (~INR 2,000) one-time | One-time |
| **Total** | **~INR 200/month** (just SIM data) + development time | |

---

## Security Concerns

1. **AUTH TOKEN EXPOSED**: The Blynk auth token (`d6FyTAf8rXVJcxvk9Ilc0IcbAo5KNdgJ`) is hardcoded in the `blynk` file. **If this GitHub repo is public, anyone can control the motor.** Either make the repo private or move the token to a separate config file not tracked by git.
2. **2G is unencrypted**: SIM800L sends data over plain HTTP. The auth token travels in cleartext over the cellular network. Not critical for a motor relay but worth knowing.
3. **Fail-safe default**: The `blynk` code sets `relayState = 1` on boot (relay ON). Consider whether relay OFF is safer on boot (prevent unintended motor start).
4. **SIM card security**: Consider enabling SIM PIN lock (`modem.simUnlock("1234")` is commented out).

---

## Questions for Hardware Friend

Please discuss and answer these -- they determine many of the software decisions:

### About the Custom PCB (STM32G0B1CCT6):
1. What is the PCB designed for? Is it a replacement for the Arduino Nano in the timer circuit, the Blynk circuit, or both combined?
2. Does the PCB have a GSM/SIM module on it? If yes, which one (SIM800L, SIM7600, A7672S, other)?
3. What is the pin mapping on the PCB? Which STM32 pins connect to: relay, encoder, display, GSM module TX/RX, fault/status inputs, AC detect, battery backup?
4. How is the board powered? What voltage input? Is there an onboard voltage regulator?
5. Is there an SWD debug header on the PCB for flashing via ST-Link?

### About Connectivity:
6. Which telecom SIM card will be used? Jio (4G only), Airtel, Vi?
7. Are the farm locations where this will be deployed covered by 2G (for SIM800L) or do we need 4G (SIM7600)?
8. Is there any WiFi available near the motor box, or is cellular the only option?

### About the Motor:
9. What type of motor? Single-phase or 3-phase?
10. What is the motor power rating (HP/kW)?
11. Is the relay directly switching the motor, or does it control a contactor/starter?
12. What does the 3-phase feedback pin (currently pin 10) actually measure? Is it a phase-failure relay output?

### About the Product:
13. Is this for personal use (1-2 motors) or do we plan to sell/deploy to multiple farmers?
14. If multiple farmers: do they each get their own Blynk account, or do we manage all devices from one account?
15. Budget ceiling per unit for hardware?

---

## Market Reference

### How Others Are Solving This

**KisanRaja** (India, kisanraja.com): GSM-based motor controller for farmers. Uses IVRS (voice call) instead of app -- farmer calls a toll-free number, speaks commands in local language. Includes dry-run protection, voltage monitoring, theft alerts. Estimated INR 5,000-15,000 per unit.

**Fasal** (India, fasal.co): Premium IoT agriculture platform. ESP32-based controllers for pumps, valves, fertigation. Mobile app with multi-language support (Hindi, Marathi, Tamil, etc.). Solar-powered wireless valves. Enterprise pricing.

**Generic GSM Motor Starters on IndiaMART/Amazon**: Widely available at INR 2,500-8,000. Most use SIM800/SIM900 + call/SMS control. Some have basic Android apps. Our project is similar but more configurable.

### Open Source References on GitHub
- **popmonac/Home-Automation**: ESP32 + SIM800 + Blynk, 4-channel relay. Very similar architecture.
- **Narala-28/Smart_Motor_WaterPump_Automation**: ESP32 + Blynk, current monitoring, dry-run protection.
- **RambabuDhanavath/smart-farm-pump-controller**: ESP32 + Spring Boot + MQTT + Android app (custom stack example).
- **TinyGSM library** (github.com/vshymanskyy/TinyGSM): Already used in our `blynk` code. Supports SIM800, SIM7600, A7672S. Migration between modems requires only changing the `#define` line.

---

## Next Steps (In Order)

1. **Hardware friend answers the questions above** (especially about the custom PCB)
2. **Decide MCU platform**: Stay on Arduino Nano, move to ESP32, or use the STM32 custom PCB
3. **Decide GSM module**: Keep SIM800L (2G) or upgrade to SIM7600 (4G)
4. **Combine timer + blynk firmware** into single codebase on chosen platform
5. **Configure Blynk app** with timer widgets (V10-V13)
6. **Test on bench** with a lightbulb or small motor before deploying to field
7. **Field test** at actual farm location
8. **Iterate** based on farmer feedback

---

*Document created: 2026-10-08*
*To be reviewed by both software and hardware team members*
