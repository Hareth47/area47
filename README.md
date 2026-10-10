# Triple Attack Detection System

مشروع إنترنت الأشياء والأمن السيبراني لكشف ثلاثة أنماط من حركة الشبكة:
- UDP Flood
- TCP SYN Flood
- Port Scanning

يراقب برنامج Python الحزم باستخدام Scapy، ثم يرسل التنبيهات إلى لوحة NodeMCU ESP8266 عبر HTTP. تعرض اللوحة الحالة والإحصائيات في شاشة LCD I2C ولوحة تحكم ويب.

## Project structure

```text
Triple-Attack-Detection-System/
├── Triple-Attack-Detection-System.pdf
├── requirements.txt
├── detector/
│   └── detector.py
└── firmware/
    └── TripleAttackDetectionSystem/
        └── TripleAttackDetectionSystem.ino
```

## Hardware

- NodeMCU ESP8266
- LCD 16x2 with I2C adapter (address configured as `0x27`)
- USB cable and jumper wires
- Computer connected to the same local network

## Software requirements

- Arduino IDE with ESP8266 board support
- Arduino libraries: `LiquidCrystal_I2C` and `ArduinoJson`
- Python 3
- Npcap on Windows (required by Scapy for packet capture)

Install Python dependencies:

```bash
python -m pip install -r requirements.txt
```

## Setup

### 1. Configure the ESP8266 firmware

Open `firmware/TripleAttackDetectionSystem/TripleAttackDetectionSystem.ino` in Arduino IDE.

Replace the placeholder values with your own Wi-Fi credentials:

```cpp
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
```

Select the correct ESP8266 board and port, then upload the sketch. Keep the board and monitoring computer on the same local network. Find the ESP8266's assigned IP address in the Serial Monitor.

### 2. Configure the Python detector

Open `detector/detector.py` and set:

- `ESP_IP` to the ESP8266's current IP address.
- `NETWORK_INTERFACE` to the exact network interface name supported by Scapy on your computer.

Run the detector with administrator/root privileges because packet capture usually requires elevated permissions:

```bash
python detector/detector.py
```

Stop it with `Ctrl+C`.

### 3. Open the dashboard

From a browser on the same local network, visit:

```text
http://ESP8266_IP/
```

Replace `ESP8266_IP` with the actual address of the board.

## Important security and safety notes

- Use only on networks and devices you own or have explicit permission to test.
- This is a learning/demo project, not a substitute for a production intrusion detection system.
- Do not commit real Wi-Fi passwords, private IP details you consider sensitive, or other secrets to a public repository.
- The HTTP endpoint is intended for a trusted local lab network; do not expose it directly to the public internet.
- Detection thresholds may need tuning for your network and hardware.

## Documentation

The project report PDF is included in the repository. Before submission, confirm that it follows the instructor's official course template and that its filename matches the course requirement.
