# Copilot Instructions

## Hardware

- **Board**: Waveshare RP2040-Zero (RP2040)
- **Platform**: PlatformIO with `platform = raspberrypi`, `board = pico`, `framework = arduino`
- **Exposed GPIO pins**: 0–15, 26–29
  - Only reference or use these GPIO pins. Pins 16–25 and 30+ are not exposed on this board.

### Onboard LED

- **Type**: Single built-in LED
- **Pin**: `LED_BUILTIN`
- **Usage**: `digitalWrite(LED_BUILTIN, HIGH/LOW)`
