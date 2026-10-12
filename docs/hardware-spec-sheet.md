# Hardware Specification Sheet
## Motor Controller Board — Fill this BEFORE firmware development

**Instructions for Sunil:** Please fill every field below. Write "N/A" if not applicable.
This document is the single source of truth for firmware development.

---

### MCU (Microcontroller)
- **Exact chip part number** (read from chip marking on board): _______________
- **Package** (LQFP48, LQFP64, etc.): _______________
- **Clock speed**: _______________

### GSM Module
- **Exact module** (A7670, A7672S, SIM7600, etc.): _______________
- **Module firmware version** (AT+GMR command output): _______________
- **Supported features** (tick all that apply):
  - [ ] AT+HTTP commands (HTTP client)
  - [ ] AT+CMQTT commands (MQTT client)
  - [ ] AT+CSSLCFG (SSL/TLS)
- **Default baud rate**: _______________
- **PWRKEY pin connected to MCU?**: Yes / No — If yes, which pin: _______________
- **Module UART logic level**: 1.8V / 2.8V / 3.3V

### Pin Connections (MCU pin → What it connects to)
| MCU Pin | Connected To | Direction | Notes |
|---------|-------------|-----------|-------|
| PA9 | | TX / RX | |
| PA10 | | TX / RX | |
| PB8 | | Output / Input | |
| PB1 | | Output / Input | |
| PB2 | | Output / Input | |
| ___ | | | |
| ___ | | | |
| ___ | | | |

### UART Wiring
- **MCU TX (PA9) connects to module's**: TX / RX (should be RX)
- **MCU RX (PA10) connects to module's**: TX / RX (should be TX)
- **Level shifter between MCU and module?**: Yes / No

### SIM Card
- **SIM type**: Nano / Micro / Mini
- **Provider**: _______________
- **APN**: _______________
- **SIM has active data plan?**: Yes / No

### Power
- **Board input voltage**: _______________
- **MCU supply voltage**: _______________
- **GSM module supply voltage**: _______________
- **Separate regulator for GSM module?**: Yes / No

### Motor / Relay
- **Relay type on board**: _______________
- **PB8 HIGH = relay ON?**: Yes / No
- **What does relay control?**: Contactor coil / Motor directly / Other: ___
- **Fault signal (PB1): LOW = fault active?**: Yes / No
- **Motor OK signal (PB2): LOW = motor OK?**: Yes / No

### Debug
- **SWD header present?**: Yes / No
- **SWD pins**: SWDIO=___ SWCLK=___ GND=___ NRST=___
- **ST-Link tested and working?**: Yes / No

### Flashing
- **Tool used for flashing**: STM32CubeProgrammer / Other: _______________
- **File format preferred**: .bin / .hex
- **Flash start address**: 0x08000000 / Other: _______________

---

**Signed off by hardware engineer:** _______________
**Date:** _______________
