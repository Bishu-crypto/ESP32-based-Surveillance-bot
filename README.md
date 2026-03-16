# ESP32-CAM Surveillance Bot

![progress](https://img.shields.io/badge/progress-57%25-blue)
![platform](https://img.shields.io/badge/platform-ESP32--CAM-green)
![board](https://img.shields.io/badge/board-AI%20Thinker-orange)
![libs](https://img.shields.io/badge/libs-ESPAsyncWebServer%20%7C%20ArduinoJson-yellow)

> WiFi-controlled surveillance robot with live MJPEG stream, WebSocket
> real-time control, and L298N motor driver — built experiment by experiment.

## Hardware

| Component    | Details                              |
|--------------|--------------------------------------|
| MCU          | AI Thinker ESP32-CAM (OV2640) + MB shield |
| Motor driver | L298N                                |
| Chassis      | 2WD robot car kit                    |
| MCU power    | 5V regulated                         |
| Motor power  | 7.4V LiPo                            |

## Experiments

| #  | Experiment                        | Status   | Libraries                              |
|----|-----------------------------------|----------|----------------------------------------|
| 01 | Blink onboard LED                 | ✅ Done  | built-in                               |
| 02 | WiFi + blocking WebServer         | ✅ Done  | WiFi.h, WebServer.h                    |
| 03 | Live MJPEG camera stream          | ✅ Done  | esp_camera.h, esp_http_server.h        |
| 04 | ESPAsyncWebServer + WebSocket     | ✅ Done  | ESPAsyncWebServer, AsyncTCP, ArduinoJson |
| 05 | Stream + WebSocket combined       | ✅ Done  | ESPAsyncWebServer, esp_camera.h |
| 06 | L298N motor control               | ⬜ Todo  | —                                      |
| 07 | Full surveillance bot             | ⬜ Todo  | —                                      |


## Repo Structure
```
esp32-cam-surveillance-bot/
├── experiments/
│   ├── 01_blink/
│   ├── 02_wifi_webserver/
│   ├── 03_mjpeg_stream/
│   ├── 04_async_websocket/
│   ├── 05_stream_ws/
│   ├── 06_motors/
│   ├── 07_surveillance_bot/
│   └── secrets.h.example
├── docs/
│   └── wiring.md
├── assets/
├── .gitignore
└── README.md
```

## Key Learnings

- `WebServer.h` blocks `loop()` — `ESPAsyncWebServer` uses FreeRTOS callbacks instead
- GPIO0 must float during normal run, pulled LOW only for flash mode
- MB shield auto-handles GPIO0 — use FLASH button during "Connecting..." dots
- Garbled bootloader output at startup is normal (74880 baud vs 115200)
- Separate power rails for MCU and motors — common GND mandatory
- Never commit `secrets.h` — use `secrets.h.example` as template

## Arduino IDE Board Settings
```
Board            : AI Thinker ESP32-CAM
Upload Speed     : 115200
CPU Frequency    : 240MHz
Flash Mode       : QIO
Partition Scheme : Huge APP (3MB No OTA / 1MB SPIFFS)
```

## Libraries Used

| Library           | Author         | Version |
|-------------------|----------------|---------|
| ESPAsyncWebServer | ESP32Async     | latest  |
| AsyncTCP          | ESP32Async     | latest  |
| ArduinoJson       | Benoit Blanchon| 6.x     |

## License

MIT