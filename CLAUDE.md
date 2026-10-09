# CLAUDE.md — Motor Controller Project

## Project Overview
IoT motor controller for Indian farmers. Allows remote ON/OFF control of agricultural motors (water pumps, irrigation) via a custom Android mobile app. The motor box sits in the field with an STM32 MCU + GSM module that communicates with Firebase cloud, and the farmer controls it from a Flutter Android app (direct APK install, no Play Store).

## Architecture
- **MCU**: STM32G0B1CCT6 on a custom PCB (LQFP48 package)
- **Connectivity**: A7670 GSM/LTE module (4G, on-board with UFL antenna connector)
- **Cloud**: Firebase Realtime Database (free Spark plan)
- **Mobile App**: Flutter (Dart), Android APK sideloaded to farmer phones
- **Communication**: STM32 → HTTPS REST API → Firebase ← Flutter app

## Repository Structure
```
motor/
├── CLAUDE.md                  # This file — project context for Claude
├── README.md                  # Project description
├── ROADMAP.md                 # v1 roadmap (superseded by v2)
├── ROADMAP_v2.md              # Current roadmap — architecture, decisions, questions for hardware
├── SIM_GUIDE.md               # SIM card research and recommendations
├── reference/                 # Reference code (not production — for learning only)
│   ├── blynk.ino              # Blynk + SIM800L GSM motor control (Arduino)
│   └── timer.ino              # Timer-based motor control with TM1637 display (Arduino)
├── firmware/                  # STM32 firmware (to be created)
│   └── (STM32CubeMX project will go here)
├── app/                       # Flutter mobile app (to be created)
│   └── (Flutter project will go here)
└── docs/                      # Additional documentation
```

## Key Decisions Made
- **No Blynk**: Blynk code is reference only. Building fully custom solution.
- **No timer integration**: Timer module stays separate. This project is mobile ON/OFF control only (for now).
- **Firebase over AWS/Azure**: Free tier never expires, simplest for our scale.
- **Flutter over React Native/Kotlin**: Google ecosystem (matches Firebase), cross-platform.
- **Direct APK sideloading**: No Play Store for POC. Share .apk via WhatsApp/USB.
- **SIM7600/A7672S recommended**: 4G, HTTPS capable. SIM800L (2G) has sunset risk and no TLS.
- **A7670 confirmed on PCB**: Hardware friend's board uses A7670 (SIMCom 4G Cat-1 module, same family as A7672S).

## Hardware (Custom PCB)
- **MCU**: STM32G0B1CCT6 — ARM Cortex-M0+, 64MHz, 256KB Flash, 144KB RAM
- **Package**: LQFP48 (44 GPIO pins)
- **GSM Module**: A7670 (SIMCom Cat-1 4G LTE) — on-board, UFL antenna connector, auto-powers on (no PWRKEY pin to STM32, only TX/RX)
- **GSM UART**: USART1 — PA9 (TX), PA10 (RX)
- **SIM**: M2M (Machine-to-Machine) IoT SIM card
- **Pin mapping**:
  - PA9 — USART1 TX (to A7670)
  - PA10 — USART1 RX (from A7670)
  - PB8 — Motor ON signal (relay/contactor control output)
  - PB1 — Fault signal (input from motor protection relay)
  - PB2 — Motor OK signal (input, motor running status)
  - Physical switch — hardwired bypass (controls contactor directly, not connected to STM32)
- **Motor**: 3-phase, 10HP — relay controls a starter/contactor (not direct motor switching)
- **Power**: Onboard buck converter/regulator
- **Debug**: SWD header present (SWDIO, SWCLK, GND, NRST) — flash via ST-Link V2

## Development Environment
- **IDE**: VS Code with STM32CubeIDE extension (v3.11.0)
- **Pin/Clock Config**: STM32CubeMX (v6.18.1) installed at `C:\Users\L118810\AppData\Local\Programs\STM32CubeMX`
- **Compiler**: STM32CubeCLT — installed at `C:\ST\STM32CubeCLT_1.22.0` (ARM GCC 14.3.1, CMake 4.3.1, Ninja 1.13.2)
- **Flutter SDK**: Installed, pinned to 3.29.3 (`C:\flutter`)
- **Git**: Configured with personal GitHub account (yogeshahalwat), SSH over port 443

## Git Setup
- **Remote**: `git@github-personal:yogeshahalwat/motor.git` (SSH via `github-personal` alias)
- **SSH config**: Uses `~/.ssh/id_ed25519_personal` key, connects via `ssh.github.com:443` (port 22 blocked by corporate firewall)
- **Local identity**: `user.name = "Yogesh Ahalwat"`, `user.email = "yogeshahalwat@gmail.com"`
- **This is a personal project** — completely separate from company project at `C:\pat\LRL_PAT_CLI`

## Coding Conventions (to be followed when writing firmware)
- STM32 HAL library for peripheral access
- Use STM32CubeMX generated code structure — write user code only inside `/* USER CODE BEGIN */` and `/* USER CODE END */` blocks
- C language for firmware (not C++)
- Dart language for Flutter app
- Keep Firebase security rules strict — each farmer can only access their own device data

## Security Notes
- Never commit secrets (API keys, Firebase config, auth tokens) to git
- The reference `blynk` file contains a hardcoded Blynk auth token — this is NOT our code, just reference
- Firebase project credentials go in environment config / gitignored files
- Use HTTPS (TLS) for all STM32-to-Firebase communication

## Current Status
- [x] Git + GitHub setup (personal account, SSH working)
- [x] VS Code + STM32 extension installed
- [x] STM32CubeMX installed and working
- [x] STM32CubeCLT (compiler/debugger) — installed at `C:\ST\STM32CubeCLT_1.22.0`
- [x] Flutter SDK — installed, pinned to 3.29.3 (`C:\flutter`)
- [x] Firebase project — created (`motor-controller-be320`, Realtime DB + Phone Auth)
- [x] Hardware friend's answers to PCB questions — A7670 on UART1, SWD header, buck regulator, 3-phase 10HP motor via starter
- [ ] STM32 firmware development
- [x] Flutter app development — login + motor ON/OFF screen built, tested on emulator
- [ ] Integration testing
- [ ] Field deployment

## Useful Commands
```bash
# SSH test to personal GitHub
ssh -T git@github-personal

# Git push to personal repo
git push origin main

# Launch STM32CubeMX from VS Code
# Click butterfly icon in sidebar → "Launch STM32CubeMX"
```

## Team
- **Software**: Yogesh (this repo owner) — firmware + app development
- **Hardware**: Friend (name TBD) — custom PCB design, motor wiring, field installation
- **Target users**: Indian farmers controlling agricultural water pump motors
