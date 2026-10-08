# CLAUDE.md — Motor Controller Project

## Project Overview
IoT motor controller for Indian farmers. Allows remote ON/OFF control of agricultural motors (water pumps, irrigation) via a custom Android mobile app. The motor box sits in the field with an STM32 MCU + GSM module that communicates with Firebase cloud, and the farmer controls it from a Flutter Android app (direct APK install, no Play Store).

## Architecture
- **MCU**: STM32G0B1CCT6 on a custom PCB (LQFP48 package)
- **Connectivity**: GSM module (SIM7600E or A7672S recommended for 4G; SIM800L is legacy 2G reference only)
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

## Hardware (Custom PCB)
- **MCU**: STM32G0B1CCT6 — ARM Cortex-M0+, 64MHz, 256KB Flash, 144KB RAM
- **Package**: LQFP48 (44 GPIO pins)
- **GSM Module**: TBD — waiting for hardware friend's answer (SIM800L or SIM7600 or A7672S)
- **Pin mapping**: TBD — waiting for custom PCB schematic from hardware friend
- **Debug**: SWD interface via ST-Link V2

## Development Environment
- **IDE**: VS Code with STM32CubeIDE extension (v3.11.0)
- **Pin/Clock Config**: STM32CubeMX (v6.18.1) installed at `C:\Users\L118810\AppData\Local\Programs\STM32CubeMX`
- **Compiler**: STM32CubeCLT (NOT YET INSTALLED — needed before building firmware)
- **Flutter SDK**: NOT YET INSTALLED — needed for app development
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
- [ ] STM32CubeCLT (compiler/debugger) — NOT YET INSTALLED
- [ ] Flutter SDK — NOT YET INSTALLED
- [ ] Firebase project — NOT YET CREATED
- [ ] Hardware friend's answers to PCB questions (pin mapping, GSM module, power)
- [ ] STM32 firmware development
- [ ] Flutter app development
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
