# ESP8266 Setup Guide — Step by Step

Follow these steps on the laptop where you have the ESP8266 board.

---

## What You Need

- ESP8266 NodeMCU board
- USB cable (micro-USB, the same type used for older Android phones)
- WiFi network (your phone hotspot or home WiFi will work)
- This laptop connected to the internet (for downloading software)

---

## Step 1: Install Arduino IDE

Arduino IDE is the software we use to write code and upload it to the ESP8266 board.

1. Open your browser
2. Go to **https://www.arduino.cc/en/software**
3. Click **"Windows Win 10 and newer, 64 bits"** (or whatever matches your system)
4. It will ask you to donate — you can click **"Just Download"** to skip
5. Run the downloaded `.exe` file
6. Click **Next → Next → Install → Finish** (all defaults are fine)
7. Open Arduino IDE from the Start menu or desktop shortcut

You should see a window with some code like:
```
void setup() {
  // put your setup code here
}

void loop() {
  // put your main code here
}
```

That means Arduino IDE is installed and ready.

---

## Step 2: Add ESP8266 Board Support

Arduino IDE doesn't know about ESP8266 by default. We need to tell it where to find ESP8266 support.

### 2a: Add the ESP8266 URL

1. In Arduino IDE, click **File** (top menu bar) → **Preferences**
2. A settings window opens
3. Find the text box that says **"Additional boards manager URLs"** (near the bottom)
4. If the box is empty, paste this URL directly:
   ```
   https://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
5. If there's already something in the box, add a comma after it, then paste the URL
6. Click **OK**

### 2b: Install the ESP8266 board package

1. Click **Tools** (top menu bar) → **Board** → **Boards Manager...**
   - OR click the board icon on the left sidebar (looks like a circuit board)
2. In the search box at the top, type: **esp8266**
3. You'll see **"esp8266 by ESP8266 Community"** appear in the list
4. Click the **Install** button next to it
5. Wait for it to download and install (~200 MB, takes 2-5 minutes)
6. When it says **"Installed"** next to the name, close the Boards Manager

---

## Step 3: Install Firebase Library

The Firebase library lets our code talk to Firebase easily (instead of writing HTTP requests manually).

1. Click **Sketch** (top menu bar) → **Include Library** → **Manage Libraries...**
   - OR click the books icon on the left sidebar
2. In the search box, type: **Firebase ESP Client**
3. Look for **"Firebase Arduino Client Library for ESP8266 and ESP32"** by **Mobizt**
   - Make sure it's by **Mobizt** (there are other Firebase libraries — we want this specific one)
4. Click the **Install** button
5. If it asks **"Install all dependencies?"** — click **Install All**
6. Wait for it to finish installing
7. Close the Library Manager

---

## Step 4: Get the Code from GitHub

### Option A: If Git is installed on this laptop

Open a terminal (Command Prompt or PowerShell) and run:

```
cd Desktop
git clone https://github.com/yogeshahalwat/motor.git
```

This downloads the entire project. The ESP8266 code is in:
```
Desktop/motor/esp8266/motor_controller/motor_controller.ino
```

### Option B: If Git is NOT installed

1. Open your browser
2. Go to **https://github.com/yogeshahalwat/motor**
3. Click the green **"Code"** button → **"Download ZIP"**
4. Extract the ZIP file to your Desktop
5. The ESP8266 code is in:
   ```
   Desktop/motor-main/esp8266/motor_controller/motor_controller.ino
   ```

---

## Step 5: Open the Code in Arduino IDE

1. In Arduino IDE, click **File** → **Open**
2. Navigate to where you downloaded/cloned the project
3. Go into: `esp8266` → `motor_controller` folder
4. Select **`motor_controller.ino`** and click **Open**
5. The code should appear in the Arduino IDE editor window

You should see code starting with:
```
/*
 * Motor Controller — ESP8266 R&D Version
 ...
```

---

## Step 6: Create the config.h File (Your Credentials)

The code needs your WiFi password and Firebase key to connect. These go in a separate file called `config.h`.

### How to create config.h in Arduino IDE:

1. Look at the tabs at the top of the code editor — you'll see a tab called **`motor_controller`**
2. Click the **small down arrow (▼)** to the right of the tabs
3. Click **"New Tab"**
4. It will ask for a name — type: **config.h**
5. Press Enter or click OK
6. A new empty tab opens — paste this code into it:

```c
// Your actual credentials — this file stays on this laptop only

#define WIFI_SSID      "Airtel_yoge_3490"
#define WIFI_PASSWORD   "air01305"

#define FIREBASE_API_KEY  "AIzaSyB1TCbj_PVpZkWBBvAVPV_5OC2UwdcP2FQ"

#define FIREBASE_DB_URL   "https://motor-controller-be320-default-rtdb.asia-southeast1.firebasedatabase.app"
```

7. Save the file: **Ctrl + S**

**Important:** If your other laptop uses a different WiFi network, change `WIFI_SSID` and `WIFI_PASSWORD` to that network's name and password.

---

## Step 7: Connect the ESP8266 Board

1. Take your ESP8266 NodeMCU board
2. Plug the **micro-USB cable** into the board
3. Plug the **other end** into your laptop's USB port
4. Your laptop should make the "device connected" sound
5. A small LED on the board might blink — that's normal

### Install USB Driver (if needed)

If your laptop doesn't detect the board:

- Most NodeMCU boards use the **CH340** USB chip
- Download the driver from: **https://sparks.gogo.co.nz/ch340.html**
- Install it, then unplug and replug the board

You'll know the driver is working when a new COM port appears (see next step).

---

## Step 8: Select the Board and Port in Arduino IDE

### 8a: Select the board type

1. Click **Tools** (top menu) → **Board** → **esp8266** (submenu) → **NodeMCU 1.0 (ESP-12E Module)**
   - If you don't see "esp8266" in the Board menu, go back to Step 2 — the board package isn't installed
2. After selecting, the bottom-right of Arduino IDE should show "NodeMCU 1.0 (ESP-12E Module)"

### 8b: Select the COM port

1. Click **Tools** → **Port**
2. You should see something like **COM3** or **COM4** or **COM5** (the exact number varies)
   - If you see multiple ports, unplug the ESP8266 → check which ports are listed → plug it back in → the NEW port that appears is your ESP8266
3. Select that COM port

If you don't see ANY COM port:
- Try a different USB cable (some cables are charge-only, no data)
- Try a different USB port on your laptop
- Install the CH340 driver (see Step 7)

---

## Step 9: Upload the Code to ESP8266

1. Click the **Upload button** — it's the **right-pointing arrow (→)** at the top-left of Arduino IDE
   - OR press **Ctrl + U**
2. Arduino IDE will first **compile** the code (you'll see orange text scrolling at the bottom)
3. Then it will **upload** to the board (you'll see progress percentages)
4. Wait about 30-60 seconds
5. When you see **"Done uploading"** at the bottom — the code is running on the board!

### If upload fails:

- **"Port not found"** — Go back to Step 8b, make sure the right port is selected
- **"Board not responding"** — Hold the **FLASH** button on the board while clicking Upload, release after upload starts
- **"Compilation error"** — Make sure you completed Step 3 (Firebase library) and Step 6 (config.h file)

---

## Step 10: Open Serial Monitor (See What's Happening)

The Serial Monitor shows messages from the ESP8266 — like a live log of what it's doing.

1. Click **Tools** → **Serial Monitor**
   - OR press **Ctrl + Shift + M**
   - OR click the magnifying glass icon at the top-right
2. A new panel opens at the bottom (or a new window)
3. **Important:** In the top-right of the Serial Monitor, change the baud rate to **115200**
   - Look for a dropdown that probably says "9600" — change it to "115200"
4. Press the **RST** (reset) button on the ESP8266 board — this restarts the code

You should see output like this:

```
=== Motor Controller ESP8266 R&D ===
Connecting to WiFi......
WiFi connected! IP: 192.168.1.105
Connecting to Firebase... OK!
Setup complete. Polling Firebase every 5 seconds...

Command: OFF | LED: OFF | Status written to Firebase
Command: OFF | LED: OFF | Status written to Firebase
```

### What each line means:

| Message | What's happening |
|---------|-----------------|
| `Connecting to WiFi......` | Board is trying to connect to your WiFi (dots = still trying) |
| `WiFi connected! IP: 192.168.x.x` | WiFi connected successfully — the IP is the board's address on your network |
| `Connecting to Firebase... OK!` | Board connected to Firebase using anonymous auth |
| `Command: OFF \| LED: OFF` | Board read "OFF" from Firebase → LED is off |
| `Command: ON \| LED: ON` | Board read "ON" from Firebase → LED is on! |

### If you see errors:

| Error message | What's wrong | Fix |
|--------------|-------------|-----|
| `Connecting to WiFi` keeps printing dots forever | Wrong WiFi name or password | Check `WIFI_SSID` and `WIFI_PASSWORD` in config.h |
| `Firebase sign-up failed` | Anonymous auth not enabled | Enable Anonymous auth in Firebase Console (Authentication → Sign-in method) |
| `Read failed: path not exist` | No data in Firebase yet | Open the Flutter app and tap ON — this creates the data |

---

## Step 11: Test with the Flutter App!

This is the exciting part — testing the full system end-to-end.

### Make sure:
- ESP8266 is running (Serial Monitor shows "Polling Firebase every 5 seconds...")
- Your phone has the Flutter app installed
- Your phone has internet (mobile data or WiFi — doesn't need to be the same WiFi as ESP8266)

### Test 1: Turn ON

1. Open the **Motor Controller** app on your phone
2. Tap the big **power button** (it should be grey/OFF)
3. The button turns **green** and says **ON**
4. Wait 5 seconds (the board polls every 5 seconds)
5. Check the **Serial Monitor** — you should see:
   ```
   Command: ON | LED: ON | Status written to Firebase
   ```
6. Look at the ESP8266 board — the **small blue LED should be ON** (glowing)
7. In the app, you should see:
   - **Device Online** — green dot (because the board is writing `last_seen` timestamps)
   - **Motor** indicator — green (because `motor_running` = true)
   - **All OK** indicator — blue (because `all_ok` = true)

### Test 2: Turn OFF

1. Tap the power button again in the app
2. It turns grey and says **OFF**
3. Wait 5 seconds
4. Serial Monitor shows: `Command: OFF | LED: OFF`
5. The blue LED on the board turns **OFF**
6. App shows Motor indicator goes grey

### Test 3: Unplug the board

1. Unplug the ESP8266 USB cable (board turns off)
2. Wait about 5 minutes
3. In the app, the **"Device Online"** should change to **"Device Offline"** (grey dot)
   - This is because `last_seen` stops updating, and the app notices the board hasn't reported in 5 minutes

---

## What Just Happened (The Full Picture)

```
📱 Your Phone (Flutter App)
   │
   │  Tap ON → writes "command: ON" to Firebase
   ▼
☁️ Firebase Cloud Database
   │
   │  ESP8266 reads "command: ON" every 5 seconds
   ▼
🔵 ESP8266 Board (on your desk)
   │
   │  Turns on LED + writes back: motor_running=true, all_ok=true, last_seen=now
   ▼
☁️ Firebase Cloud Database
   │
   │  App is listening in real-time
   ▼
📱 Your Phone — shows "Device Online", "Motor Running", "All OK"
```

All three — app, Firebase, and board — are connected and talking to each other!

---

## Troubleshooting

### LED doesn't turn on/off

- Check Serial Monitor — is the command being read correctly?
- Make sure the device path in Firebase matches: `devices/test_device_001`
- The built-in LED is **active LOW** — our code already handles this, but if the LED behavior seems reversed, let us know

### Serial Monitor shows garbage characters

- Change the baud rate dropdown to **115200** (it might be on 9600 by default)
- Press the **RST** button on the board to restart

### Board keeps restarting / crashing

- This usually means a WiFi or Firebase connection issue
- Check your WiFi name and password are correct in config.h
- Make sure your WiFi router is within range of the board

### Everything works but app doesn't show "Device Online"

- Wait at least one full polling cycle (5 seconds)
- Check Firebase Console → Realtime Database — you should see `last_seen` updating with timestamps
- If `last_seen` is updating but app still says offline, the app checks if `last_seen` is within the last 5 minutes

---

## Quick Reference

| What | Value |
|------|-------|
| Board in Arduino IDE | NodeMCU 1.0 (ESP-12E Module) |
| Serial Monitor baud rate | 115200 |
| Firebase project | motor-controller-be320 |
| Device path | devices/test_device_001 |
| LED pin | GPIO2 (built-in, active LOW) |
| Poll interval | 5 seconds |
| WiFi | Your home/hotspot WiFi |
