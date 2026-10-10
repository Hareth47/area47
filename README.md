# ESP8266 Network Threat Detection and Attack Simulation

An educational network-security project using an **ESP8266 Wi-Fi board**, a **16×2 LCD with an I²C interface**, external **Python traffic-simulation scripts**, and an **HTML dashboard** to demonstrate monitoring of flood-style network activity.

> **Safety notice:** Run simulations only on devices and networks you own or are explicitly authorized to test. Keep tests inside an isolated lab environment. Do not send flood traffic to public IP addresses or third-party networks.

## Project Overview

The project demonstrates how selected network traffic patterns can be simulated and surfaced to an operator through a local display and a browser-based interface.

- **ESP8266:** monitored Wi-Fi device running the device-side firmware.
- **LCD:** displays a short status or the traffic/attack type reported by the implementation.
- **Python scripts:** external traffic-simulation tools.
- **HTML dashboard:** presents attack counts and current activity associated with the ESP8266.

The team identified UDP flood and TCP flood as key simulation scenarios. The exact detection rules, data path, and communication method should be confirmed from the project source code.

## Main Features

- ESP8266-based Wi-Fi monitoring prototype.
- Local status display using a 16×2 LCD and I²C interface.
- Python-based traffic simulation for controlled lab scenarios.
- HTML dashboard for viewing attack counts and current activity.
- Educational demonstration of network monitoring and flood-style traffic patterns.

## Hardware Components

| Component | Purpose |
|---|---|
| ESP8266 / NodeMCU-class Wi-Fi development board | Wi-Fi connectivity and device-side firmware |
| 16×2 character LCD | Displays local status and reported event type |
| I²C LCD backpack | Provides I²C communication with the LCD |
| Jumper wires | Hardware signal connections |
| USB cable / suitable power source | Power and programming connection |
| Computer | Runs Python scripts and opens the HTML dashboard |

**Important:** Confirm the exact ESP8266 board revision, LCD voltage, I²C address, and SDA/SCL pin mapping from the physical hardware and firmware before wiring or publishing a definitive circuit diagram.

## Software and Technologies

- **ESP8266 firmware:** device-side monitoring and LCD updates. Confirm the exact language, framework, and libraries from the firmware source.
- **Python:** external scripts used to simulate controlled traffic scenarios.
- **HTML:** browser-based dashboard for viewing event counts and current activity.
- **Wi-Fi / TCP / UDP:** network technologies relevant to the project's lab demonstrations.

## Attack Scenarios

### UDP Flood

A controlled lab scenario involving a high volume of UDP datagrams. Potential monitoring indicators include packet rate, byte rate, destination-port distribution, and deviation from baseline traffic.

### TCP Flood

A controlled lab scenario involving unusually frequent TCP packets or connection attempts. The precise subtype should be documented only if it is implemented by the project.

> A simulation running successfully does not prove that every packet reached the monitored device. A displayed counter or event label alone is not proof of a verified attack or service disruption.

## General Workflow

1. Connect the ESP8266 to the authorized test Wi-Fi network.
2. Start the device firmware and verify the LCD status.
3. Open the HTML dashboard.
4. Run one of the project's Python simulation scripts against the designated lab target, using bounded test settings.
5. Observe the reported event type on the LCD and activity summary on the dashboard.
6. Save logs and, where available, compare displayed results with an independent packet capture.
7. Stop the simulation and verify that the test environment returns to normal.

The exact commands and configuration depend on the actual source files in the repository and should be documented after those files are added.

## Suggested Repository Structure

The following is a suggested layout. Adjust names to match the files actually included in this repository.

```text
.
├── README.md
├── firmware/
│   └── esp8266_firmware.ino
├── python/
│   ├── udp_flood_simulation.py
│   └── tcp_flood_simulation.py
├── dashboard/
│   ├── index.html
│   ├── style.css
│   └── script.js
├── docs/
│   ├── project-report.pdf
│   └── images/
└── tests/
    └── test-notes.md
```

These are example filenames, not a claim that these exact files already exist. Replace them with the real repository contents.

## Testing and Validation

For meaningful evaluation, record:

- Test date and authorized lab topology.
- Scenario type and bounded configuration.
- ESP8266 status and firmware logs.
- LCD and dashboard observations with timestamps.
- Independent packet-capture results, if available.
- Detection latency and false-positive/false-negative counts, if measured.
- Device or service behavior before, during, and after each test.

Do not report detection accuracy or successful service disruption unless supported by reproducible measurements.

## Limitations

- The exact firmware, Python scripts, HTML files, and communication details must be documented from the implementation.
- The final circuit diagram and pin mapping should be verified against the actual board.
- Dashboard counts must be clearly defined as simulation runs, alerts, or independently verified events.
- No detection accuracy or independent attack-impact measurement should be claimed without test evidence.
- The project should be described as a monitoring and simulation prototype unless prevention or mitigation is implemented and tested.

## Team Members

- Hareth Jalal Al_Absari
- Badr Sultan Al_Maqrami
- Sakhr Fadl Al_Shalili
- Bashar Ali Al_Saeedi

## Future Improvements

- Document the verified circuit schematic and pin mapping.
- Add timestamped logs and compare displayed counts with packet captures.
- Define and evaluate detection thresholds against normal baseline traffic.
- Distinguish simulated scenarios from independently verified alerts.
- Add authentication and input validation if the dashboard supports remote control.
- Include screenshots of the actual dashboard and reproducible lab test results.

## References

- Espressif Systems: https://www.espressif.com/
- Python Documentation: https://docs.python.org/3/
- RFC 768 — User Datagram Protocol: https://www.rfc-editor.org/rfc/rfc768
- RFC 9293 — Transmission Control Protocol: https://www.rfc-editor.org/rfc/rfc9293

## Disclaimer

This repository is intended for educational and authorized defensive testing only. The authors do not encourage unauthorized disruption of networks or services. Use the project only within the scope of explicit permission.
