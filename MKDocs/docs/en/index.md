# Ícaro's Sensor Box

## Introduction
The **Ícaro** project aims to launch a stratospheric balloon (up to 20-30 km altitude), carrying several payloads, including:
- A radio amateur team with APRS and a U/V repeater.
- **Our sensor module.**

This initiative is organized and streamed on Twitch by _[Araba_ST](https://twitch.com/Araba_ST)_ and _[Labgluon](https://www.twitch.tv/labgluon)_.

!!! info
    Our goal is to develop more complex systems while tackling the added challenge of being the first community-driven project with Twitch participants. Additionally, we face the uncertainty of managing remote teams who have never worked together before.

## Project Objectives

- **WiFi communication testing** with an ESP32-CAM (or equivalent) to determine maximum range.
  - Improve 2.4 GHz antennas.
  - Implement code for reading and transmitting video via WiFi on the ESP32.
- **Telemetry data recording** during the ascent and transmission via LoRa.
- **Design of modular PCBs** for:
  - Reading GPS/IMU/Temperature/Barometer sensors, etc.
  - Voltage regulation.

## Additional Ideas

- Implement an automatic antenna tracking system to always point at the balloon.
- Integrate solar panels.
- Expose an ESP32 to the elements and determine when it stops functioning.

## Project Structure

```plaintext
Icaro
├── Docs/                    # General documentation
│   ├── NamingAndCoding.md   # File/variable naming conventions
│   └── Design.md            # High-level design
├── IcaroSW/                 # Main software
├── Mechanical/              # 3D and mechanical designs
├── Modules/                 # Hardware modules
│   ├── <Module>/            # One folder per board
│   │   ├── Doc/             # Documentation
│   │   ├── PCB/             # PCB design
│   │   ├── Software/        # Module-specific software
│   │   └── Library/         # Software library for the module
│   └── ...
```
Each designed board will have a dedicated software library, while _IcaroSW_ will contain the main mission software.
