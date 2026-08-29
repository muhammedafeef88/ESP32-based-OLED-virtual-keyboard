# ESP32 OLED Virtual Keyboard

A two-button virtual keyboard built using an ESP32 and a 128×64 OLED display. The system allows users to navigate through characters, select them using a long press and create text directly on the OLED display.

## Features

- ESP32-based virtual keyboard
- 128×64 SSD1306 OLED display
- Two-button control
- Short press for character navigation
- 2-second long press for character selection
- Alphabet keyboard
- Numbers and special characters
- Space and backspace functions
- Switch between letters and symbols
- Highlighted cursor for character selection
- Real-time display of typed text

## Hardware Used

- ESP32 Development Board
- 128×64 OLED Display (SSD1306)
- 2 × Push Buttons
- Breadboard
- Jumper Wires
- USB Cable

## Pin Connections

| Component | ESP32 GPIO |
|-----------|------------|
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| Right Button | GPIO 14 |
| Up Button | GPIO 27 |

The OLED display communicates with the ESP32 using the I2C protocol.

OLED I2C address: `0x3C`

## How It Works

The OLED display shows a virtual keyboard arranged in a 4 × 8 grid.

The user can navigate through the keyboard using two push buttons.

### Right Button — GPIO 14

- Short press → Move the cursor to the right.
- Hold for 2 seconds → Select the highlighted character.

### Up Button — GPIO 27

- Short press → Move the cursor upward.
- Hold for 2 seconds → Select the highlighted character.

The cursor wraps around the keyboard, allowing continuous navigation.

