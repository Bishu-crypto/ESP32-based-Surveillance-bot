# Wiring Notes

## Flashing (MB Shield)
- Plug USB into MB shield
- Hold FLASH button → tap RST → release FLASH
- Upload in Arduino IDE during "Connecting......" dots
- After upload: press RST to run

## GPIO Reference — AI Thinker ESP32-CAM
| GPIO | Function          | Notes                        |
|------|-------------------|------------------------------|
| 33   | Red LED           | Active LOW                   |
| 4    | Flash LED         | Active HIGH, very bright     |
| 0    | Camera / Boot     | Must float during normal run |
| 26   | Camera SIOD       | Reserved                     |
| 27   | Camera SIOC       | Reserved                     |
| 25   | Camera VSYNC      | Reserved                     |
| 23   | Camera HREF       | Reserved                     |
| 22   | Camera PCLK       | Reserved                     |
| 21   | Camera Y5         | Reserved                     |
| 19   | Camera Y4         | Reserved                     |
| 18   | Camera Y3         | Reserved                     |
| 5    | Camera Y2         | Reserved                     |
| 35   | Camera Y9         | Input only                   |
| 34   | Camera Y8         | Input only                   |
| 39   | Camera Y7         | Input only                   |
| 36   | Camera Y6         | Input only                   |
| 32   | Camera PWDN       | Reserved                     |

## Safe GPIOs for Motors (Exp 06+)
12, 13, 14, 15, 2, 16

## L298N Wiring (Exp 06+)
| ESP32-CAM | L298N | Function          |
|-----------|-------|-------------------|
| GPIO 12   | IN1   | Left motor dir A  |
| GPIO 13   | IN2   | Left motor dir B  |
| GPIO 14   | IN3   | Right motor dir A |
| GPIO 15   | IN4   | Right motor dir B |
| GPIO 2    | ENA   | Left speed PWM    |
| GPIO 16   | ENB   | Right speed PWM   |
| GND       | GND   | Common ground     |

## Power Rails
- ESP32-CAM → 5V separate supply
- Motors     → 7.4V LiPo
- All GNDs must be connected together