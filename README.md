# TNG-Packet: Amateur Radio Packet System
An open-source digital communication system for amateur radio enthusiasts, built for robust packet exchange over VHF.

## Overview
TNG-Packet is a lightweight, noise-resistant packet radio system designed for amateur radio operators, using the callsign **6L5TNG**. It replaces traditional APRS (1200bps AFSK, 1200/2200Hz) with a custom **TNG-Shift modulation (1000Hz pulses)** for better noise immunity. The project leverages affordable mobile stations (**Baofeng UV-5R**) and base stations (**Icom IC-7100**) on the 145MHz FM band. Unlike APRS, TNG-Packet avoids CI-V commands, relying on audio input for future signal quality analysis (RSSI, S/N, decode success rate). The current demo is transmit-only, using an Arduino Uno with a simple terminal interface for speed control and packet transmission.

---
## Key Features
- **TNG-Shift Modulation:** 1000Hz pulses with variable durations (e.g., 150bps: 0=40ms, 1=60ms) for robust data transfer.
- **Adaptive Speeds:** 150bps, 300bps, 1200bps, 2400bps, selected via terminal commands (`/1`, `/2`, `/3`, `/4`).
- **Distinct Tones:** Speed-specific start/end tones (e.g., 150bps: 2100Hz-1700Hz-2100Hz) for easy receiver identification.
- **VOX Operation:** Uses mobile station VOX to eliminate PTT hardware.
- **Packet Structure:** Compact packets (up to 52 bytes) with callsign, ID, type, speed flag, text payload, and CRC-16.
- **Future Plans:** Signal compensation (RSSI/SN-based speed adjustment), ESP32 integration, and receive functionality.

This project is ideal for hams experimenting with low-cost, custom packet radio systems. The current demo is transmit-only, with plans to add receiving and signal analysis using ESP32.

---
## Hardware Setup

### Components
**Mobile Station (Baofeng UV-5R):**
- Frequency: 145.000MHz (or local packet frequency, e.g., 145.010MHz).
- Mode: FM, 12.5kHz bandwidth.
- VOX: Enabled, sensitivity 3-5.
- Power: 1W (low power for testing).

**Base Station (Icom IC-7100):**
- Frequency: 145.000MHz.
- Mode: FM, used for receiving test packets (no CI-V).

**Arduino Uno:**
- Microcontroller: 16MHz, 32KB flash, 2KB SRAM.
- Pin 8 (PWM): Audio output to mobile station.
- Serial: 9600 baud for terminal interface.

**Interface Circuit:**
```
Arduino Pin 8 ---- 10kΩ ---- 0.1uF ---- Baofeng 3.5mm Ring (Mic +)
Arduino GND ----------------------------- Baofeng 3.5mm Sleeve (Mic -)
```

**Cost Estimate:** ~$3 USD (3.5mm TRRS cable: $2, resistor: $0.10, capacitor: $0.20).

---
## TNG-Packet Protocol

### Packet Structure
- **Start Sequence:** Speed-specific tones (see below).
- **Header (10 bytes):**
  - Callsign (4 bytes): ASCII "6L5TNG".
  - Packet ID (2 bytes): 0-65535, fixed at 0001 in demo.
  - Type (1 byte): 0x01 (text, slang codes like "73" planned).
  - Speed Flag (1 byte): 0x01=150bps, 0x02=300bps, 0x03=1200bps, 0x04=2400bps.
  - Reserved (2 bytes): For future use.
- **Payload:** Up to 40 bytes, ASCII text (e.g., "73", "QSL").
- **Checksum:** CRC-16 (2 bytes).
- **End Sequence:** Speed-specific tones.
- **Total Size:** Up to 52 bytes + sequences (150bps ≈ 3s, 2400bps ≈ 200ms).

### TNG-Shift Modulation
**Encoding:** 1000Hz pulses with duration-based bits:
- 150bps: 0=40ms, 1=60ms, gap=20ms.
- 300bps: 0=20ms, 1=30ms, gap=10ms.
- 1200bps: 0=10ms, 1=15ms, gap=5ms.
- 2400bps: 0=5ms, 1=10ms, gap=3ms.

**Tones:**
- **150bps:** Start = 2100Hz-1700Hz-2100Hz (60ms each, 180ms), End = 1700Hz-2100Hz (60ms, 120ms).
- **300bps:** Start = 2200Hz-1800Hz-2200Hz (50ms, 150ms), End = 1800Hz-2200Hz (50ms, 100ms).
- **1200bps:** Start = 2300Hz-1900Hz (40ms, 80ms), End = 1900Hz-2300Hz (40ms, 80ms).
- **2400bps:** Start = 2400Hz (60ms), End = 2400Hz (40ms).

---
## Speed Recommendations
- **150bps:** Best for weak signals (RSSI < 50) or long range (10-50km), ~3s/packet.
- **300bps:** Reliable for short range (1-5km), ~1.5s/packet.
- **1200bps:** Medium range (5-20km), needs RSSI > 150, ~400ms/packet.
- **2400bps:** High-speed, strong signals (RSSI > 200), risk of audio distortion, ~200ms/packet.

---
## Installation

### Hardware:
1. Connect Arduino Uno to mobile station as described.
2. Configure mobile station: 145.000MHz, FM, VOX ON (sensitivity 3-5), 1W.
3. Set base station to 145.000MHz, FM, for receiving.

### Software:
1. Install Arduino IDE.
2. Copy the demo code below to a new sketch (`TNG_Packet_Demo.ino`).
3. Upload to Arduino Uno.

---
## Demo Code
```cpp
// TNG-Packet Demo: TNG-Shift Modulation (150/300/1200/2400bps) for Baofeng UV-5R
// Arduino Uno, Pin 8 (PWM) for audio output, VOX mode
// Serial monitor (9600 baud): Terminal commands (/1, /2, /3, /4 for speed, /s for send)
// Updated: Added 150bps with distinct start/end tones (2100Hz-1700Hz)
...
```
*(Full code continues as in project description)*

---
## Usage

### Setup Hardware:
- Connect Arduino Uno to mobile station as shown in the circuit diagram.
- Configure mobile station: 145.000MHz, FM, VOX ON (sensitivity 3-5), 1W.
- Set base station to 145.000MHz, FM, for receiving.

### Run the Demo:
- Open Arduino IDE, upload the code to the Uno.
- Open Serial Monitor (9600 baud).
- Use commands:
  - `/1`: Set speed to 150bps.
  - `/2`: Set speed to 300bps.
  - `/3`: Set speed to 1200bps.
  - `/4`: Set speed to 2400bps.
  - `/s <text>`: Send text (e.g., `/s 73` or `/s QSL`, max 40 characters).

### Expected Output:
- **Serial Monitor:** Speed set to: 150bps or Sent: 73.
- **Mobile station transmits:**
  - 150bps: "beep-boop-beep" (2100Hz-1700Hz-2100Hz) → data → "boop-beep" (1700Hz-2100Hz).
  - 300bps: "beep-boop-beep" (2200Hz-1800Hz-2200Hz) → data → "boop-beep" (1800Hz-2200Hz).
  - 1200bps: "beep-boop" (2300Hz-1900Hz) → data → "boop-beep" (1900Hz-2300Hz).
  - 2400bps: "beep" (2400Hz) → data → "beep" (2400Hz).
- **Base station (IC-7100)** receives tones and data, identifiable by tone sequence.

### Troubleshooting:
- No transmission: Check VOX sensitivity (3-5), verify audio connections.
- Unclear tones: Test base station audio output, consider LM358 op-amp for signal boost.
- 2400bps distortion: Switch to 150bps or 300bps, check mobile station audio quality.

---
## Signal Compensation (Future)
- Adjust speed based on signal quality (RSSI, S/N, decode success rate).
- Weak signal (RSSI < 50, S/N < 10dB): 150bps.
- Medium signal (RSSI 50-150, S/N 10-20dB): 300bps.
- Strong signal (RSSI 150-200, S/N 20-30dB): 1200bps.
- Very strong signal (RSSI > 200, S/N > 30dB): 2400bps.

**Implementation Plan:** Use ESP32 ADC for audio input (mobile station 2.5mm speaker), LM358 for RSSI/SN, auto-switch speeds after 3 failed decodes.

---
## Limitations
- **Transmit-Only:** Current demo excludes receiving due to Arduino Uno memory constraints (2KB SRAM).
- **VOX Sensitivity:** 150bps (180ms tones) is most reliable; 2400bps (60ms tones) may need higher sensitivity.
- **Mobile Station Audio:** 2400bps may distort; prefer 150bps/300bps for reliability.
- **Regulatory:** Ensure compliance with 145MHz amateur radio licensing.

---
## Future Work
- Receiving: Add TNG-Shift decoding via mobile station 2.5mm speaker → ESP32 ADC.
- Signal Compensation: Implement RSSI/SN-based speed adjustment with LM358.
- ESP32 Upgrade: Add web interface, higher memory, and faster ADC.
- Slang Codes: Support "73" (0x01), "QSL" (0x02), "CQCQ", "GL", etc.
- Testing: Plan base station receive tests (distance, antenna types).

---
## Contributing
We welcome contributions! To get started:
1. Fork the repository.
2. Create a branch (`git checkout -b feature-name`).
3. Commit changes (`git commit -m "Add feature"`).
4. Push to the branch (`git push origin feature-name`).
5. Open a pull request.

**Note:** Please test on 145MHz with a valid amateur radio license. Share ideas for tones, slang codes, or hardware tweaks!

---
## License
MIT License - Free to use, modify, and distribute for non-commercial amateur radio projects.

---
## Contact
For questions or collaboration, reach out via GitHub Issues or your local ham radio club.  
**73 from 6L5TNG!**

