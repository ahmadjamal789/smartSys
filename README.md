# smartSys
<img width="704" height="448" alt="Screenshot from 2026-10-04 07-23-38" src="https://github.com/user-attachments/assets/3b217287-bf31-4be3-90ec-58da61db16bc" />
# Smart Home Monitoring System

An Arduino Uno based monitor that tracks temperature, humidity, motion and gas/smoke, and reports everything on a 20x4 I2C LCD with LED and buzzer indicators. Built with PlatformIO.

## Features

- Temperature and humidity readout (DHT11)
- Motion detection (PIR sensor)
- Gas/smoke detection (MQ sensor, analog output)
- 20x4 I2C LCD status display
- Green, blue and red LED status indicators
- Buzzer that sounds **only** for gas/smoke

## Hardware

| Component | Quantity |
|---|---|
| Arduino Uno | 1 |
| DHT11 temperature/humidity sensor | 1 |
| PIR motion sensor (e.g. HC-SR501) | 1 |
| MQ gas/smoke sensor (analog output) | 1 |
| 20x4 LCD with I2C backpack | 1 |
| Green, blue and red LEDs | 1 each |
| 220 ohm resistors (one per LED) | 3 |
| Buzzer | 1 |
| Breadboard and jumper wires | as needed |

## Pin Map

| Device | Arduino Pin |
|---|---|
| DHT11 data | D2 |
| PIR output | D9 |
| Green LED | D8 (via 220 ohm resistor) |
| Blue LED | D5 (via 220 ohm resistor) |
| Red LED | D7 (via 220 ohm resistor) |
| Buzzer | D6 |
| MQ sensor (AO) | A0 |
| LCD I2C SDA | A4 |
| LCD I2C SCL | A5 |

Power the sensors and LCD from 5V and make sure all grounds are shared. A bare 4-pin DHT11 needs a 10k ohm pull-up resistor between DATA and VCC (3-pin modules already include one).

## Behaviour

**Startup:** the LCD shows "SMART HOME / MONITOR SYSTEM", then a short warm-up message.

**LCD layout (20x4):**

| Row | Content |
|---|---|
| 1 | Temperature and humidity, e.g. `Temp:27C Humid:60%` |
| 2 | `MOTION DETECTED` while the PIR is triggered, otherwise blank |
| 3 | `GAS DETECTED!` while gas/smoke is above the threshold, otherwise blank |
| 4 | `All Clear` when neither motion nor gas is detected |

**LEDs and buzzer:**

| Condition | Green | Blue | Red | Buzzer |
|---|---|---|---|---|
| No motion | ON | OFF | - | - |
| Motion detected | OFF | ON | - | - |
| Gas/smoke detected | - | - | ON | ON |

## Software Setup

1. Install [PlatformIO](https://platformio.org/) (VS Code extension or CLI).
2. Clone or copy this project and open it in PlatformIO.
3. Make sure `platformio.ini` targets the Uno and includes the libraries below.
4. Build and upload.

Example `platformio.ini`:

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
monitor_speed = 9600
lib_deps =
    marcoschwartz/LiquidCrystal_I2C
    adafruit/DHT sensor library
    adafruit/Adafruit Unified Sensor
```

## Configuration

These values are at the top of `main.cpp`:

| Setting | Default | Notes |
|---|---|---|
| `LCD_ADDRESS` | `0x27` | Try `0x3F` if the screen stays blank |
| `GAS_THRESHOLD` | `100` | Range 0-1023. Raise it if you get false alarms |
| `BUZZER_FREQ` | `2000` | Buzzer tone in Hz |
| `DHT_INTERVAL` | `1500` ms | DHT11 needs at least 1 second between reads |
| `DISPLAY_INTERVAL` | `250` ms | LCD refresh rate |

## Troubleshooting

**PIR always reports motion**
- The PIR needs 30-60 seconds to settle after power-up. Increase the warm-up delay in `setup()`.
- On the HC-SR501, turn the time-delay pot fully counter-clockwise and set sensitivity to the middle.
- Check the wire on D9 and the shared ground. A floating pin gives random readings.
- Keep the sensor away from fans, heat sources and direct sunlight.

**Temperature reads an impossible value (for example -5 C)**
- The DHT11 only measures 0-50 C, so values outside that range mean corrupted data.
- Check the pinout, add a 10k ohm pull-up if using a bare sensor, and confirm the sensor is really a DHT11 (a DHT22 needs `DHT_TYPE DHT22`).
- Unstable power from the buzzer and LEDs can also corrupt readings.

**LCD stays blank**
- Try address `0x3F`, and adjust the contrast potentiometer on the I2C backpack.

**Gas alarm triggers constantly**
- MQ sensors need a long warm-up. Let them heat up for a few minutes and raise `GAS_THRESHOLD`.

## Project Structure

```
.
├── platformio.ini
├── src/
│   └── main.cpp
└── README.md
```

<img width="1040" height="780" alt="image" src="https://github.com/user-attachments/assets/7e806af6-d343-4eb2-829c-bb26099fe0dc" />

