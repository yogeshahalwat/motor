# Motor Controller - Mobile App Control Roadmap (v2)

> **Changes from v1**: No timer integration (timer stays separate). No Blynk platform. Building fully custom: Firebase backend + Flutter Android app. Direct APK distribution (no Play Store).

---

## What We Are Building

A system where the farmer can:
1. **Turn motor ON/OFF from a mobile app** (Android APK installed directly, no Play Store)
2. **See motor status** in real-time (ON/OFF, fault, all-ok, motor running)
3. **Get push notifications** on phone for faults, motor off events
4. Still use the **physical switch** on the metal box as a local backup

### What We Are NOT Building (for now)
- No countdown timer in the app (timer module stays as a separate standalone thing)
- No Blynk dependency (the `blynk` code was reference only)
- No Play Store listing (direct APK install for now)

---

## Architecture Overview

```
+------------------+          +------------------+          +------------------+
|   FARMER'S       |          |   FIREBASE       |          |   MOTOR BOX      |
|   PHONE          |          |   (Google Cloud)  |          |   (Field)        |
|                  |          |                  |          |                  |
|  Flutter App     |  <---->  |  Realtime DB     |  <---->  |  STM32 + GSM     |
|  (Android APK)   |  WiFi/   |  (free tier)     |  4G/2G   |  Module           |
|                  |  4G      |                  |  HTTPS   |                  |
|  - ON/OFF button |          |  - motor_state   |          |  - Relay control |
|  - Status LEDs   |          |  - fault_status  |          |  - Fault detect  |
|  - Notifications |          |  - command        |          |  - Status pins   |
|                  |          |  - timestamp     |          |  - Physical switch|
+------------------+          +------------------+          +------------------+
```

### How it works (simple explanation):
1. **Farmer opens app** on phone, taps "ON" button
2. App writes `{"command": "ON"}` to Firebase cloud database
3. **STM32 + GSM module** in the field continuously checks Firebase every few seconds
4. It reads the command, turns relay ON (motor starts)
5. STM32 writes back `{"motor_state": "ON", "fault": false}` to Firebase
6. **App sees the update** in real-time, shows green "Running" indicator
7. If a fault is detected, STM32 writes `{"fault": true}` to Firebase
8. Firebase triggers a **push notification** to farmer's phone: "FAULT DETECTED"

---

## Technology Stack Explained

### 1. Firebase Realtime Database (Cloud Backend)

**What is it?** A cloud database by Google where data syncs in real-time. When the STM32 writes data, the phone app sees it instantly. When the phone writes a command, STM32 picks it up within seconds. No server to manage, no code to deploy on a server.

**Why Firebase over other options?**

| Cloud Provider | Free Tier | Expiry | Good for IoT? | Complexity | Our Choice? |
|---------------|-----------|--------|---------------|-----------|-------------|
| **Firebase Realtime DB** | 1GB storage, 100 simultaneous connections, 10GB/month download | **Never expires** | Yes -- simple REST API, real-time sync | Low | **YES (recommended)** |
| **Firebase Firestore** | 50K reads/day, 20K writes/day, 1GB storage | **Never expires** | Yes but heavier protocol | Medium | Good alternative |
| **AWS IoT Core** | No free tier (pay from message 1) | N/A | Yes but complex | Very High | No -- too complex and costs from day 1 |
| **Azure IoT Hub** | 8,000 messages/day (free tier) | **Never expires** | Yes | High | No -- complex setup |
| **Supabase** | 500MB DB, 50K auth users, 1GB storage | **Never expires** | Moderate (PostgreSQL based) | Medium | Possible alternative |
| **ThingsBoard Cloud** | 5 devices, 1M data points/month | **Never expires** | Yes -- built for IoT | Medium | Good but less flexible for custom app |

**Firebase Free Tier Details (Spark Plan)**:

| Resource | Free Limit | What Happens If Exceeded |
|----------|-----------|------------------------|
| Simultaneous connections | 100 | New connections refused (but 100 is plenty -- each farmer is 1 connection) |
| Data stored | 1 GB | Cannot write more until you delete old data |
| Data downloaded | 10 GB/month | Reads stop until next month |
| Database writes | Unlimited | No limit on writes |
| Authentication users | 50,000 | Cannot add more users |
| Cloud Functions invocations | 2,000,000/month | Functions stop until next month |
| Cloud Messaging (push notifications) | Unlimited | No limit |

**When do you start paying?** Only if you voluntarily upgrade to Blaze plan (pay-as-you-go) to exceed the above limits. For our use case (1-50 farmers, each sending a few messages per day), **we will never hit these limits.** The free tier is genuinely enough for production use, not just testing.

**Cost if we ever exceed free tier** (Blaze plan, pay-as-you-go):

| Resource | Price |
|----------|-------|
| Data stored | $5 per GB/month |
| Data downloaded | $1 per GB |
| Cloud Functions | $0.40 per million invocations |
| Authentication | Free up to 50K users, then $0.0055/user/month |

**For 50 farmers, estimated Blaze cost: less than $1/month** (most resources stay within free limits).

### 2. Flutter (Mobile App)

**What is it?** A framework by Google to build mobile apps. You write code once (in Dart language) and it compiles to both Android and iOS apps. We only need Android for now, but iOS comes free if needed later.

**Why Flutter?**

| Framework | Language | Learning Curve | Firebase Integration | Android + iOS | Community |
|-----------|----------|---------------|---------------------|--------------|-----------|
| **Flutter** | Dart | Medium (but Dart is similar to Java/JavaScript) | Excellent (official Google packages) | Yes | Very large |
| React Native | JavaScript | Medium | Good (third-party packages) | Yes | Large |
| Kotlin (native Android) | Kotlin | Higher | Good | Android only | Large |
| MIT App Inventor | Visual blocks | Very Low | Poor | Android only | Small |

**Flutter is the best fit because**: Dart is easy to learn, Firebase integration is first-party (made by same company), and if we ever need iOS support, we don't rewrite anything.

### 3. Direct APK Install (Sideloading)

**Can we install directly on farmer's phone without Play Store?** YES.

**How it works:**
1. We build the app in Flutter, which produces a `.apk` file
2. Share the APK via WhatsApp, Bluetooth, USB cable, or download link
3. Farmer opens the file on their Android phone
4. Phone shows a warning: "Install from unknown sources?" -- farmer taps "Allow" (one-time setting)
5. App installs and works exactly like any Play Store app

**Is there any compatibility issue?** No. An APK is an APK -- Android doesn't care where it came from. It runs identically whether from Play Store or sideloaded.

**Advantages of sideloading (for now):**
- No $25 Play Store developer fee
- No Google review process (saves days/weeks)
- No privacy policy / terms of service required
- Instant distribution -- just share the file
- Can update by sharing a new APK

**Disadvantages (why we might want Play Store later):**
- No automatic updates -- farmer must manually install each new version
- Android shows a security warning on install (can scare non-technical users)
- Can't be found by searching on Play Store
- No crash reporting / analytics from Play Console

**Recommendation**: Sideload for POC (0 cost, instant). Move to Play Store when the product is stable and being given to many farmers.

---

## Hardware Requirements

### What We Need From the Custom PCB (STM32G0B1CCT6)

The STM32 board needs these connections for the mobile control feature:

| Function | What It Does | STM32 Peripheral Needed |
|----------|-------------|------------------------|
| **GSM Module TX/RX** | Talk to SIM module (send HTTP requests to Firebase) | USART (hardware UART) |
| **Relay output** | Switch motor ON/OFF | GPIO Output |
| **Physical switch input** | Local ON/OFF button on the metal box | GPIO Input (with pull-up) |
| **Fault detection input** | Read fault signal from motor starter | GPIO Input |
| **All-OK input** | Read all-ok signal | GPIO Input |
| **Motor status input** | Read if motor is currently running | GPIO Input |
| **Motor OFF input** | Read motor off feedback | GPIO Input |
| **Status LED output (optional)** | Show connection status on the box | GPIO Output |

### GSM Module Selection

| Module | Network | Price (INR) | Firebase Compatible? | Recommendation |
|--------|---------|-------------|---------------------|----------------|
| **SIM800L** | 2G GPRS | 500-800 | Yes (HTTP GET/POST) but no HTTPS (no TLS) | **Risky** -- 2G shutdown coming. Also no encryption. |
| **SIM7600E/G** | 4G LTE | 2,500-4,500 | Yes (HTTP + HTTPS with TLS) | **Recommended** -- future-proof, secure |
| **A7672S** | 4G LTE | 2,200-3,500 | Yes (HTTP + HTTPS with TLS) | **Good alternative** -- slightly cheaper than SIM7600 |

**Important**: Firebase requires HTTPS (encrypted connection). SIM800L cannot do HTTPS properly. We would need a workaround (HTTP proxy or Firebase legacy REST endpoint). **SIM7600 or A7672S is strongly recommended** -- they support TLS natively, making Firebase communication simple and secure.

### Questions for Hardware Friend

#### About the Custom PCB:
1. Does the STM32G0B1CCT6 PCB already have a GSM/SIM module on it? Which one?
2. If no GSM module on PCB: is there space/plan to add one? Or will it be a separate daughter board?
3. Which STM32 UART is routed to the GSM module (or available for it)?
4. What other STM32 pins are routed to: relay, physical switch, fault input, status inputs?
5. Is there an SWD debug header (SWDIO, SWCLK, GND, NRST) for flashing?
6. How is the board powered? Input voltage? Onboard regulator?
7. Is there an antenna connector for the GSM module?

#### About Connectivity:
8. Which telecom SIM card? Jio (4G only), Airtel, Vi?
9. Do the farm locations have 4G coverage? (Check with Jio coverage map: jio.com/coverage)
10. Does SIM800L currently work reliably at these locations, or are there connectivity drops?

#### About the Motor Setup:
11. Single-phase or 3-phase motor?
12. Motor power rating (HP/kW)?
13. Does the relay directly switch the motor, or control a contactor/starter?
14. What does each feedback pin actually measure (fault, all-ok, status, motor-off)? Are these from a dedicated motor protection relay?

#### Hardware Friend's Suggestions Welcome:
15. Any concerns about the architecture shown above?
16. Any suggestions for GSM module choice based on field experience?
17. Any power consumption concerns (GSM module + STM32 running 24/7)?

---

## Software Cost Estimation

### One-Time Costs

| Item | Cost | Notes |
|------|------|-------|
| Firebase account | FREE | Google account required (Gmail) |
| Flutter SDK | FREE | Open source |
| Android Studio (for building APK) | FREE | Or use VS Code with Flutter extension |
| VS Code + STM32 extensions | FREE | Already installed |
| STM32CubeMX | FREE | Already installed |
| ST-Link V2 clone (for flashing STM32) | INR 300-500 | One-time purchase. **Need to buy if not already have** |
| Google Play Store developer account | $25 (INR ~2,100) | **Only if we decide to publish on Play Store later. NOT needed for sideloading.** |
| **TOTAL (POC with sideloading)** | **INR 300-500** (just ST-Link) | Everything else is free |

### Recurring Costs

| Item | Monthly Cost | Notes |
|------|-------------|-------|
| Firebase (Spark plan) | **FREE forever** | Up to 100 connections, 1GB storage, 10GB download |
| SIM card data plan | INR 150-300/month | Per device. Minimal data usage (few KB per command) |
| Firebase (Blaze plan, if >50 farmers) | ~$1/month (INR ~85) | Only if we exceed free tier limits |
| **TOTAL per device** | **INR 150-300/month** | Just the SIM data cost |

### Development Time Estimate (Software)

| Task | Estimated Time | Who Does It |
|------|---------------|-------------|
| **STM32 firmware**: GPIO setup, UART to GSM, HTTP/HTTPS to Firebase, relay control, status reading, watchdog | 2-3 weeks | Us (with Claude helping) |
| **Firebase setup**: Create project, configure Realtime DB structure, security rules, push notifications | 1-2 days | Us |
| **Flutter app**: UI design, Firebase integration, login, motor control screen, status display, notifications | 2-3 weeks | Us (with Claude helping) |
| **Testing on bench** (with LED/bulb instead of motor) | 1 week | Us + hardware friend |
| **Field testing** at actual farm | 1 week | Us + hardware friend + farmer |
| **Total estimated time** | **5-8 weeks** | |

### Tools We Will Use (All Free)

| Tool | Purpose | Already Installed? |
|------|---------|-------------------|
| VS Code | Code editor for everything | Yes |
| Claude Code | AI assistant (me) | Yes |
| STM32CubeIDE for VS Code | STM32 pin config + build + flash | Yes (extension installed, need CubeCLT) |
| STM32CubeMX | Visual pin/clock configuration | Yes |
| Flutter SDK | Build the Android app | **Need to install** |
| Android Studio or VS Code Flutter extension | Compile and build APK | **Need to install** |
| Firebase Console | Manage cloud database | Browser-based, nothing to install |
| Git + GitHub (yogeshahalwat/motor) | Version control | Yes, set up |
| ST-Link V2 | Flash firmware to STM32 | **Need to buy (INR 300-500)** |

---

## Hardware Cost Estimation (Per Unit)

### If Custom PCB Already Has GSM Module

| Component | Cost (INR) | Notes |
|-----------|-----------|-------|
| Custom PCB (STM32G0B1CCT6 + components) | ? (ask hardware friend) | Already designed/manufactured |
| GSM module (if on PCB) | Included in PCB cost | |
| Relay module | 100-200 | If not on PCB |
| SIM card | 100-200 | One-time, prepaid |
| Antenna (GSM) | 50-150 | Usually comes with module |
| Enclosure (IP54/IP65 box) | 200-500 | Weather protection for farm use |
| Power supply | 200-500 | 12V/5V depending on design |
| Wiring, connectors | 100-200 | |
| **TOTAL per unit** | **INR 750-1,750 + PCB cost** | |

### If Need to Add GSM Module Separately

| Component | Cost (INR) | Notes |
|-----------|-----------|-------|
| Custom PCB (STM32 + basic components) | ? | |
| SIM7600E module (recommended) | 2,500-4,500 | 4G, HTTPS capable |
| OR A7672S module (budget 4G) | 2,200-3,500 | 4G, slightly cheaper |
| OR SIM800L (not recommended) | 500-800 | 2G only, no HTTPS, sunset risk |
| Everything else same as above | ~750-1,550 | |
| **TOTAL per unit (with SIM7600)** | **INR 3,250-6,050 + PCB cost** | |
| **TOTAL per unit (with A7672S)** | **INR 2,950-5,050 + PCB cost** | |

---

## Firebase Database Structure

This is what the cloud database will look like (for hardware friend to understand the data flow):

```json
{
  "devices": {
    "device_001": {
      "command": {
        "relay": "ON",
        "timestamp": 1696700000,
        "source": "app"
      },
      "status": {
        "motor_running": true,
        "fault": false,
        "all_ok": true,
        "relay_state": "ON",
        "signal_strength": -65,
        "last_seen": 1696700005,
        "uptime_seconds": 86400
      },
      "info": {
        "farmer_name": "Udaibhan",
        "location": "Farm Plot 3",
        "motor_hp": "5",
        "sim_number": "9876543210"
      }
    },
    "device_002": {
      "...": "same structure for next farmer"
    }
  },
  "users": {
    "uid_abc123": {
      "name": "Udaibhan",
      "phone": "9876543210",
      "devices": ["device_001"]
    }
  }
}
```

**Data flow:**
- Phone app WRITES to `devices/device_001/command/relay` = "ON" or "OFF"
- STM32 READS `devices/device_001/command/relay` every 3-5 seconds
- STM32 WRITES to `devices/device_001/status/*` with current readings
- Phone app READS `devices/device_001/status/*` and shows it on screen
- Firebase Cloud Function SENDS push notification when `fault` changes to `true`

---

## Development Phases

### Phase 1: Setup (Week 1)
- [ ] Hardware friend answers questions above
- [ ] Install Flutter SDK + Android tooling
- [ ] Install STM32CubeCLT (compiler/debugger for STM32)
- [ ] Create Firebase project (free, takes 5 minutes)
- [ ] Set up Firebase Realtime Database with the structure above
- [ ] Get STM32 pin mapping from hardware friend

### Phase 2: STM32 Firmware (Weeks 2-3)
- [ ] Configure STM32 pins in CubeMX based on PCB pinout
- [ ] Write UART driver for GSM module communication
- [ ] Implement AT command handler for GSM module (init, network registration, HTTPS)
- [ ] Implement Firebase REST API calls (read command, write status)
- [ ] Implement relay control logic (ON/OFF based on Firebase command)
- [ ] Implement status reading (fault, all-ok, motor status pins)
- [ ] Implement physical switch handling (local override)
- [ ] Implement watchdog (auto-restart if GSM connection lost)
- [ ] Implement fail-safe (if connection lost for X minutes, what happens to relay?)
- [ ] Test with ST-Link debugger on actual PCB

### Phase 3: Flutter App (Weeks 2-3, parallel with firmware)
- [ ] Design simple UI: one screen with ON/OFF button, status indicators, fault alert
- [ ] Implement Firebase Authentication (phone number login -- farmer logs in with OTP)
- [ ] Implement Firebase Realtime Database read/write
- [ ] Implement push notifications (Firebase Cloud Messaging)
- [ ] Build APK
- [ ] Test on 2-3 different Android phones (budget phones that farmers use)

### Phase 4: Integration Testing (Week 4-5)
- [ ] Connect STM32 board (with GSM) to Firebase
- [ ] Connect phone app to same Firebase project
- [ ] Test: tap ON in app --> STM32 receives --> relay turns ON --> status shows in app
- [ ] Test: physical switch ON --> STM32 updates Firebase --> app shows ON
- [ ] Test: simulate fault --> STM32 writes fault --> app shows alert + push notification
- [ ] Test: GSM signal loss and recovery
- [ ] Test: phone has no internet temporarily
- [ ] Test: power cycle the STM32 board (does it reconnect and resume?)

### Phase 5: Field Deployment (Week 5-8)
- [ ] Install at one farm location
- [ ] Monitor for 1-2 weeks
- [ ] Collect farmer feedback
- [ ] Fix issues
- [ ] Deploy to more locations if stable

---

## Security Notes

1. **Firebase Security Rules**: We will configure rules so each farmer can only read/write their own device data. No farmer can see or control another farmer's motor.
2. **Phone authentication**: Firebase supports OTP-based phone login (free for 10K verifications/month). Farmer enters phone number, gets OTP, logs in. No password to remember.
3. **HTTPS**: If using SIM7600/A7672S, all communication between STM32 and Firebase is encrypted (HTTPS/TLS). If using SIM800L, data is unencrypted -- anyone on the network could theoretically intercept motor commands.
4. **Auth token in code**: Unlike Blynk (which hardcodes an auth token), Firebase uses a project API key + device-specific credentials. More secure by design.
5. **Fail-safe**: Define what happens if internet is lost: motor stays in last state? Motor turns OFF after X minutes? This is a safety decision for hardware friend.

---

## Future Features (After POC)

These can be added without changing the core architecture:

| Feature | Difficulty | Cost | Notes |
|---------|-----------|------|-------|
| Timer/scheduler in app | Easy | Free | Add a timer screen in Flutter, schedule commands in Firebase |
| Multi-motor support | Easy | Free | Add more relay outputs, more entries in Firebase |
| Power/current monitoring | Medium | INR 300 (sensor) | Add SCT-013 current sensor, report to Firebase |
| Water tank level | Medium | INR 200-500 (sensor) | Ultrasonic sensor, display in app |
| Data logging / run history | Easy | Free | Firebase already stores timestamped data |
| Hindi / regional language | Easy | Free | Flutter supports localization |
| OTA firmware updates | Hard | Free | Update STM32 firmware remotely via GSM (complex but possible) |
| Play Store listing | Easy | $25 one-time | When ready for wider distribution |
| iOS app | Easy | Free (dev) / $99/year (Apple account) | Flutter already compiles for iOS |
| SMS fallback for non-smartphone users | Medium | SIM cost | Send SMS commands when farmer has no smartphone |
| Multiple users per device | Easy | Free | Firebase auth supports multiple users linked to one device |
| Geofencing / location | Easy | Free | Auto-show nearby motor in app using phone GPS |

---

## Summary

| Aspect | Decision |
|--------|---------|
| **Cloud platform** | Firebase Realtime Database (free forever for our scale) |
| **Mobile app** | Flutter (Dart) -- build APK, sideload to farmer's phone |
| **MCU** | STM32G0B1CCT6 (custom PCB) |
| **GSM module** | SIM7600E or A7672S recommended (4G, HTTPS capable) |
| **Distribution** | Direct APK install (no Play Store for now) |
| **Total software cost** | INR 0-500 (just ST-Link purchase) |
| **Total recurring cost per device** | INR 150-300/month (SIM data only) |
| **Development time** | 5-8 weeks |
| **Can add features later?** | Yes -- architecture supports timer, multi-motor, logging, scheduling |

---

*Document created: 2026-10-08 (v2)*
*Previous version: ROADMAP.md (v1)*
*To be reviewed by both software and hardware team members*
