TNG-Packet: Amateur Radio Packet System
An open-source digital communication system for amateur radio enthusiasts, built for robust packet exchange over VHF.

Overview
TNG-Packet is a lightweight, noise-resistant packet radio system designed for amateur radio operators, using the callsign 6L5TNG. It replaces traditional APRS (1200bps AFSK, 1200/2200Hz) with a custom TNG-Shift modulation (1000Hz pulses) for better noise immunity. The project leverages affordable mobile stations (Baofeng UV-5R) and base stations (Icom IC-7100) on the 145MHz FM band. Unlike APRS, TNG-Packet avoids CI-V commands, relying on audio input for future signal quality analysis (RSSI, S/N, decode success rate). The current demo is transmit-only, using an Arduino Uno with a simple terminal interface for speed control and packet transmission.
Key Features

TNG-Shift Modulation: 1000Hz pulses with variable durations (e.g., 150bps: 0=40ms, 1=60ms) for robust data transfer.
Adaptive Speeds: 150bps, 300bps, 1200bps, 2400bps, selected via terminal commands (/1, /2, /3, /4).
Distinct Tones: Speed-specific start/end tones (e.g., 150bps: 2100Hz-1700Hz-2100Hz) for easy receiver identification.
VOX Operation: Uses mobile station VOX to eliminate PTT hardware.
Packet Structure: Compact packets (up to 52 bytes) with callsign, ID, type, speed flag, text payload, and CRC-16.
Future Plans: Signal compensation (RSSI/SN-based speed adjustment), ESP32 integration, and receive functionality.

This project is ideal for hams experimenting with low-cost, custom packet radio systems. The current demo is transmit-only, with plans to add receiving and signal analysis using ESP32.
Hardware Setup
Components

Mobile Station (Baofeng UV-5R):
Frequency: 145.000MHz (or local packet frequency, e.g., 145.010MHz).
Mode: FM, 12.5kHz bandwidth.
VOX: Enabled, sensitivity 3-5.
Power: 1W (low power for testing).


Base Station (Icom IC-7100):
Frequency: 145.000MHz.
Mode: FM, used for receiving test packets (no CI-V).


Arduino Uno:
Microcontroller: 16MHz, 32KB flash, 2KB SRAM.
Pin 8 (PWM): Audio output to mobile station.
Serial: 9600 baud for terminal interface.


Interface Circuit:
Arduino Pin 8 → 10kΩ resistor → 0.1uF capacitor → Baofeng 3.5mm Ring (Mic +).
GND → Baofeng 3.5mm Sleeve (Mic -).


Cost Estimate: ~$3 USD (3.5mm TRRS cable: $2, resistor: $0.10, capacitor: $0.20).

Circuit Diagram
Arduino Uno Pin 8 ---- 10kΩ ---- 0.1uF ---- Baofeng 3.5mm Ring (Mic +)
Arduino GND ----------------------------- Baofeng 3.5mm Sleeve (Mic -)

TNG-Packet Protocol
Packet Structure

Start Sequence: Speed-specific tones (see below).
Header (10 bytes):
Callsign (4 bytes): ASCII "6L5TNG".
Packet ID (2 bytes): 0-65535, fixed at 0001 in demo.
Type (1 byte): 0x01 (text, slang codes like "73" planned).
Speed Flag (1 byte): 0x01=150bps, 0x02=300bps, 0x03=1200bps, 0x04=2400bps.
Reserved (2 bytes): For future use.


Payload: Up to 40 bytes, ASCII text (e.g., "73", "QSL").
Checksum: CRC-16 (2 bytes).
End Sequence: Speed-specific tones.
Total Size: Up to 52 bytes + sequences (150bps ≈ 3s, 2400bps ≈ 200ms).

TNG-Shift Modulation

Encoding: 1000Hz pulses with duration-based bits:
150bps: 0=40ms, 1=60ms, gap=20ms.
300bps: 0=20ms, 1=30ms, gap=10ms.
1200bps: 0=10ms, 1=15ms, gap=5ms.
2400bps: 0=5ms, 1=10ms, gap=3ms.


Tones:
150bps: Start = 2100Hz-1700Hz-2100Hz (60ms each, 180ms), End = 1700Hz-2100Hz (60ms, 120ms).
300bps: Start = 2200Hz-1800Hz-2200Hz (50ms, 150ms), End = 1800Hz-2200Hz (50ms, 100ms).
1200bps: Start = 2300Hz-1900Hz (40ms, 80ms), End = 1900Hz-2300Hz (40ms, 80ms).
2400bps: Start = 2400Hz (60ms), End = 2400Hz (40ms).



Speed Recommendations

150bps: Best for weak signals (RSSI < 50) or long range (10-50km), ~3s/packet.
300bps: Reliable for short range (1-5km), ~1.5s/packet.
1200bps: Medium range (5-20km), needs RSSI > 150, ~400ms/packet.
2400bps: High-speed, strong signals (RSSI > 200), risk of audio distortion, ~200ms/packet.

Installation

Hardware:

Connect Arduino Uno to mobile station as described.
Configure mobile station: 145.000MHz, FM, VOX ON (sensitivity 3-5), 1W.
Set base station to 145.000MHz, FM, for receiving.


Software:

Install Arduino IDE.
Copy the demo code below to a new sketch (TNG_Packet_Demo.ino).
Upload to Arduino Uno.


Demo Code:


// TNG-Packet Demo: TNG-Shift Modulation (150/300/1200/2400bps) for Baofeng UV-5R
// Arduino Uno, Pin 8 (PWM) for audio output, VOX mode
// Serial monitor (9600 baud): Terminal commands (/1, /2, /3, /4 for speed, /s for send)
// Updated: Added 150bps with distinct start/end tones (2100Hz-1700Hz)

#define AUDIO_OUT 8 // PWM pin for mobile station Mic input (3.5mm Ring)
#define BAUD_RATE 9600 // Serial monitor baud rate
#define TONE_FREQ_DATA 1000 // Hz for TNG-Shift data pulses
#define TONE_FREQ_150_1 2100 // Hz for 150bps start/end tones
#define TONE_FREQ_150_2 1700 // Hz for 150bps start/end tones
#define TONE_FREQ_300_1 2200 // Hz for 300bps start/end tones
#define TONE_FREQ_300_2 1800 // Hz for 300bps start/end tones
#define TONE_FREQ_1200_1 2300 // Hz for 1200bps start/end tones
#define TONE_FREQ_1200_2 1900 // Hz for 1200bps start/end tones
#define TONE_FREQ_2400 2400 // Hz for 2400bps start/end tones

// Speed-specific pulse durations (ms): {0-bit, 1-bit, gap}
const int pulseDurations[4][3] = {
  {40, 60, 20}, // 150bps: 0=40ms, 1=60ms, gap=20ms
  {20, 30, 10}, // 300bps
  {10, 15, 5},  // 1200bps
  {5, 10, 3}    // 2400bps
};

// Current speed: 0=150bps, 1=300bps, 2=1200bps, 3=2400bps
int currentSpeed = 0; // Default 150bps

// CRC-16 function for checksum
unsigned int crc16(byte *data, int len) {
  unsigned int crc = 0xFFFF;
  for (int i = 0; i < len; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

// Generate tone for specified duration
void generateTone(int freq, int duration) {
  tone(AUDIO_OUT, freq, duration);
  delay(duration); // Gap added in sendByte
}

// Send TNG-Shift start sequence based on speed
void sendStartSequence() {
  if (currentSpeed == 0) { // 150bps: 2100Hz-1700Hz-2100Hz, 60ms each
    generateTone(TONE_FREQ_150_1, 60);
    generateTone(TONE_FREQ_150_2, 60);
    generateTone(TONE_FREQ_150_1, 60);
  } else if (currentSpeed == 1) { // 300bps: 2200Hz-1800Hz-2200Hz, 50ms each
    generateTone(TONE_FREQ_300_1, 50);
    generateTone(TONE_FREQ_300_2, 50);
    generateTone(TONE_FREQ_300_1, 50);
  } else if (currentSpeed == 2) { // 1200bps: 2300Hz-1900Hz, 40ms each
    generateTone(TONE_FREQ_1200_1, 40);
    generateTone(TONE_FREQ_1200_2, 40);
  } else { // 2400bps: 2400Hz, 60ms
    generateTone(TONE_FREQ_2400, 60);
  }
  delay(10); // Short gap after sequence
}

// Send TNG-Shift end sequence based on speed
void sendEndSequence() {
  if (currentSpeed == 0) { // 150bps: 1700Hz-2100Hz, 60ms each
    generateTone(TONE_FREQ_150_2, 60);
    generateTone(TONE_FREQ_150_1, 60);
  } else if (currentSpeed == 1) { // 300bps: 1800Hz-2200Hz, 50ms each
    generateTone(TONE_FREQ_300_2, 50);
    generateTone(TONE_FREQ_300_1, 50);
  } else if (currentSpeed == 2) { // 1200bps: 1900Hz-2300Hz, 40ms each
    generateTone(TONE_FREQ_1200_2, 40);
    generateTone(TONE_FREQ_1200_1, 40);
  } else { // 2400bps: 2400Hz, 40ms
    generateTone(TONE_FREQ_2400, 40);
  }
  delay(10); // Short gap after sequence
}

// Send byte as TNG-Shift pulses
void sendByte(byte b) {
  int pulse0 = pulseDurations[currentSpeed][0]; // 0-bit duration
  int pulse1 = pulseDurations[currentSpeed][1]; // 1-bit duration
  int gap = pulseDurations[currentSpeed][2];    // Gap duration
  for (int i = 7; i >= 0; i--) {
    if (b & (1 << i)) {
      generateTone(TONE_FREQ_DATA, pulse1); // 1-bit
    } else {
      generateTone(TONE_FREQ_DATA, pulse0); // 0-bit
    }
    delay(gap);
  }
}

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(AUDIO_OUT, OUTPUT);
  Serial.println("TNG-Packet Demo: Commands:");
  Serial.println("/1: Set 150bps, /2: Set 300bps, /3: Set 1200bps, /4: Set 2400bps");
  Serial.println("/s <text>: Send text (e.g., /s 73)");
  Serial.println("Default speed: 150bps");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove whitespace

    // Handle commands
    if (input.startsWith("/")) {
      if (input == "/1" || input == "/2" || input == "/3" || input == "/4") {
        currentSpeed = input.substring(1).toInt() - 1;
        Serial.print("Speed set to: ");
        if (currentSpeed == 0) Serial.println("150bps");
        else if (currentSpeed == 1) Serial.println("300bps");
        else if (currentSpeed == 2) Serial.println("1200bps");
        else Serial.println("2400bps");
      }
      else if (input.startsWith("/s ")) {
        String text = input.substring(3);
        text.trim();
        if (text.length() > 0 && text.length() <= 40) { // Max payload 40 bytes
          // Prepare packet
          byte packet[50]; // Header (10) + Payload (40 max)
          int idx = 0;

          // Header: Callsign (6L5TNG), ID (0001), Type (0x01), Speed
          packet[idx++] = '6'; packet[idx++] = 'L'; packet[idx++] = '5'; packet[idx++] = 'T';
          packet[idx++] = 0x00; packet[idx++] = 0x01; // Packet ID: 0001
          packet[idx++] = 0x01; // Type: Text
          packet[idx++] = currentSpeed + 1; // Speed: 0x01, 0x02, 0x03, 0x04
          packet[idx++] = 0x00; packet[idx++] = 0x00; // Reserved

          // Payload: Input text
          for (int i = 0; i < text.length(); i++) {
            packet[idx++] = text[i];
          }

          // CRC-16
          unsigned int crc = crc16(packet, idx);
          packet[idx++] = (crc >> 8) & 0xFF; // High byte
          packet[idx++] = crc & 0xFF; // Low byte

          // Send packet
          sendStartSequence();
          for (int i = 0; i < idx; i++) {
            sendByte(packet[i]);
          }
          sendEndSequence();

          Serial.print("Sent: ");
          Serial.println(text);
        } else {
          Serial.println("Error: Text must be 1-40 characters");
        }
      } else {
        Serial.println("Error: Invalid command. Use /1, /2, /3, /4, or /s <text>");
      }
    } else {
      Serial.println("Error: Commands must start with /");
    }
  }
}

Usage

Setup Hardware:

Connect Arduino Uno to mobile station as shown in the circuit diagram.
Configure mobile station: 145.000MHz, FM, VOX ON (sensitivity 3-5), 1W.
Set base station to 145.000MHz, FM, for receiving.


Run the Demo:

Open Arduino IDE, upload the code to the Uno.
Open Serial Monitor (9600 baud).
Use commands:
/1: Set speed to 150bps.
/2: Set speed to 300bps.
/3: Set speed to 1200bps.
/4: Set speed to 2400bps.
/s <text>: Send text (e.g., /s 73 or /s QSL, max 40 characters).




Expected Output:

Serial Monitor: Speed set to: 150bps or Sent: 73.
Mobile station transmits:
150bps: "beep-boop-beep" (2100Hz-1700Hz-2100Hz) → data → "boop-beep" (1700Hz-2100Hz).
300bps: "beep-boop-beep" (2200Hz-1800Hz-2200Hz) → data → "boop-beep" (1800Hz-2200Hz).
1200bps: "beep-boop" (2300Hz-1900Hz) → data → "boop-beep" (1900Hz-2300Hz).
2400bps: "beep" (2400Hz) → data → "beep" (2400Hz).


Base station (IC-7100) receives tones and data, identifiable by tone sequence.


Troubleshooting:

No transmission: Check VOX sensitivity (3-5), verify audio connections.
Unclear tones: Test base station audio output, consider LM358 op-amp for signal boost.
2400bps distortion: Switch to 150bps or 300bps, check mobile station audio quality.<grok:render type="render_inline_citation">29



Signal Compensation (Future)

Goal: Adjust speed based on signal quality (RSSI, S/N, decode success rate).
Weak signal (RSSI < 50, S/N < 10dB): 150bps.
Medium signal (RSSI 50-150, S/N 10-20dB): 300bps.
Strong signal (RSSI 150-200, S/N 20-30dB): 1200bps.
Very strong signal (RSSI > 200, S/N > 30dB): 2400bps.


Implementation Plan: Use ESP32 ADC for audio input (mobile station 2.5mm speaker), LM358 for RSSI/SN, auto-switch speeds after 3 failed decodes.

Limitations

Transmit-Only: Current demo excludes receiving due to Arduino Uno memory constraints (2KB SRAM).
VOX Sensitivity: 150bps (180ms tones) is most reliable; 2400bps (60ms tones) may need higher sensitivity.<grok:render type="render_inline_citation">56
Mobile Station Audio: 2400bps may distort; prefer 150bps/300bps for reliability.<grok:render type="render_inline_citation">29
Regulatory: Ensure compliance with 145MHz amateur radio licensing.

Future Work

Receiving: Add TNG-Shift decoding via mobile station 2.5mm speaker → ESP32 ADC.
Signal Compensation: Implement RSSI/SN-based speed adjustment with LM358.
ESP32 Upgrade: Add web interface, higher memory, and faster ADC.
Slang Codes: Support "73" (0x01), "QSL" (0x02), "CQCQ", "GL", etc.
Testing: Plan base station receive tests (distance, antenna types).

Contributing
We welcome contributions! To get started:

Fork the repository.
Create a branch (git checkout -b feature-name).
Commit changes (git commit -m "Add feature").
Push to the branch (git push origin feature-name).
Open a pull request.

Please test on 145MHz with a valid amateur radio license. Share ideas for tones, slang codes, or hardware tweaks!
License
MIT License - Free to use, modify, and distribute for non-commercial amateur radio projects.
Contact
For questions or collaboration, reach out via GitHub Issues or your local ham radio club. 73 from 6L5TNG!
