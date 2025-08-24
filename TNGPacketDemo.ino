// TNG-Packet Demo: TNG-Shift Modulation (150/300/1200/2400bps) for Baofeng UV-5R
// Arduino Uno, Pin 8 (PWM) for audio output, VOX mode
// Serial monitor (9600 baud): Terminal commands (/1, /2, /3, /4 for speed, /s for send)
// Updated: Added 150bps with distinct start/end tones (2100Hz-1700Hz)

#define AUDIO_OUT 8 // PWM pin for Baofeng Mic input (3.5mm Ring)
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