# SPI Display Project

This is a SPI display demo for TI CC3551E wireless microcontroller with support for ST7789 (240×320) and ST7735 (128×128) displays. This example project is based on SIMPLELINK-WIFI-SDK (need to install seperately).

## Pre-requisites

1. [CCStudio IDE](https://www.ti.com/tool/download/CCSTUDIO) (v20.5.1 or newer) with TI Clang compiler (v4.0.4 LTS or newer)
2. [SIMPLELINK-SDK-WIFI-SDK](https://www.ti.com/tool/download/SIMPLELINK-WIFI-SDK) (v10.10 or newer)
3. [Wi-Fi Toolbox](https://www.ti.com/tool/download/SIMPLELINK-WIFI-TOOLBOX) (v4.1.16 or newer - tied to SIMPLELINK-SDK-WIFI-SDK installation)
4. [Sysconfig](https://www.ti.com/tool/download/SYSCONFIG) (v1.26.3 or newer - tied to SIMPLELINK-SDK-WIFI-SDK installation)
5. [LP-EM-CC35X1 LaunchPad development kit](https://www.ti.com/tool/LP-EM-CC35X1)
6. ST7789 or ST7735 based SPI display

**Note** : Project is tested with [waveshare 2inch LCD Display Module](https://www.waveshare.com/2inch-lcd-module.htm) (ST7789 controller) and [TI BOOSTXL-EDUMKII BoosterPack](https://www.ti.com/tool/BOOSTXL-EDUMKII) (ST7735 controller). Other display / controller variants may require adjustments to work correctly.

## Quick Start

### Hardware Setup

Connect your SPI display to the CC35X1 LaunchPad using these GPIO pins:

| Display Pin | CC35X1 GPIO | Signal |
|-------------|-------------|--------|
| CLK (SCK) | GPIO27 | SPI0 Clock |
| MOSI (DIN) | GPIO29 | SPI0 Data |
| CS | GPIO13 | Chip Select |
| DC (RS) | GPIO34 | Data/Command |
| RST | GPIO30 | Reset (active low) |
| GND | GND | Ground |
| VCC | 3.3V | Power |

**Note** : If using the BOOSTXL-EDUMKII BoosterPack kit with ST7735 based TFT display, simply stack it on the LP-EM-CC35X1.

### Building & Flashing

1. **Open CCS Theia** and import the `SPI_Display` project
2. **Select Display** - Edit `display_config.h`:
   ```c
   #define USE_ST7789          /* or USE_ST7735 */
   ```
3. **Build** - Right-click project → Build Project
4. **Flash** - Run → Start Debugging

### Expected Output

**Display:** Solid colors cycling (Red → Yellow → Green → Cyan → Blue → Magenta)

**Console:**
```
========================================
SPI Display Color Animation Demo
========================================
MCU: CC3551E
Display: ST7789 (240x320 pixels)
Status: Initializing...
========================================

Status: Runtime: 4.2 sec | Frames: 100 | FPS: 24.00
Status: Runtime: 8.3 sec | Frames: 200 | FPS: 24.00
```

## Project Overview

This project demonstrates SPI display control on embedded systems with CC35xxE wireless microcontrollers. 

## Selecting Display Controller

Both display drivers are always present. Select which to use by editing `display_config.h`:

### Use ST7789 (240×320) - Default
```c
#define USE_ST7789
/* #define USE_ST7735 */
```

### Switch to ST7735 (128×128)
```c
/* #define USE_ST7789 */
#define USE_ST7735
```

Save the file and rebuild!

### Troubleshooting

| Problem | Solution |
|---------|----------|
| No display configured | Check `display_config.h` - must define `USE_ST7789` or `USE_ST7735` |
| Missing headers | Verify `display_config.h` exists; rebuild with Clean |
| Build fails | Run Project → Clean, then rebuild |

## Flashing to Hardware

1. **Connect Display Hardware** - Verify all GPIO connections are secure
2. **Flash via CCS Theia** - Click Debug panel or Run → Debug As → TI SimpleLink Project
3. **Verify Success** - Display should cycle colors, UART shows startup message

### Flash Troubleshooting

| Problem | Solution |
|---------|----------|
| Cannot connect to target | Verify USB cable; try reconnecting LaunchPad |
| Display shows nothing | Check GPIO connections; verify display power |
| No UART output | Set terminal to 115200 baud (8N1) |

## Architecture

### Hardware Layer

**Display Drivers:**
- `st7789_lcd.c/h` - ST7789 controller (240×320)
- `st7735_lcd.c/h` - ST7735 controller (128×128)

**Key Functions:**
- `LCD_init()` - Initialize display controller
- `LCD_fillScreen(color)` - Fill entire screen
- `LCD_fillRect(x, y, w, h, color)` - Fill rectangle
- `LCD_drawImage()` - Draw image from memory
- `LCD_drawContentFrame()` - Draw partial frame

### Application Layer

**Main Components:**
- `main_freertos.c` - FreeRTOS bootstrap
- `spi_display.c` - Application logic and animation loop
- `spi_display.syscfg` - TI Drivers configuration
- `display_config.h` - Display selection

**Threading:**
- `mainThread()` - Initializes drivers, creates display thread
- `displayThread()` - Main animation loop (24 FPS)

## Animation System

### Color Cycling

The current animation fills the screen with colors from a palette:

```c
void draw_color_bar_pattern(void)
{
    int color_idx = (frame_count / 30) % PALETTE_SIZE;
    uint16_t color = color_palette[color_idx];
    LCD_fillScreen(color);
}
```

**Palette:** RED → YELLOW → GREEN → CYAN → BLUE → MAGENTA

### Frame Rate Control

- **Target:** 24 FPS (42ms per frame)
- **Method:** System timer calculates elapsed time; sleeps only if early
- **Timing:** Uses `clock_gettime(CLOCK_MONOTONIC)` for accuracy

### Logging

- **Trigger:** Every 100 frames (~4.2 seconds at 24 FPS)
- **Output:** Runtime, frame count, average FPS
- **Timing:** Based on actual system timer, not frame counting

## Code Customization

### Change Frame Rate

In `spi_display.c` line 124:
```c
uint32_t target_frame_ms = 40;  /* Adjust this value */
```

Options:
- `40` = 24 FPS (default)
- `33` = 30 FPS
- `50` = 20 FPS

### Change Color Palette

In `spi_display.c` lines 79-86:
```c
static const uint16_t color_palette[] = {
    0xF800,  /* RED */
    0xF840,  /* YELLOW */
    0x07E0,  /* GREEN */
    0x07FF,  /* CYAN */
    0x001F,  /* BLUE */
    0xF81F   /* MAGENTA */
};
```

Common RGB565 colors:
- `0x0000` = BLACK, `0xFFFF` = WHITE
- `0xF800` = RED, `0x07E0` = GREEN, `0x001F` = BLUE
- `0xF840` = YELLOW, `0x07FF` = CYAN, `0xF81F` = MAGENTA

### Change Color Cycle Speed

In `spi_display.c` line 92:
```c
int color_idx = (frame_count / 30) % PALETTE_SIZE;
```

- `30` = 1.25 sec per color (default)
- `60` = 2.5 sec per color
- `15` = 0.6 sec per color

### Change Logging Frequency

In `spi_display.c` line 145:
```c
if (frame_count - last_logged_frame >= 100) {
```

- `100` = ~4.2 sec (default)
- `50` = ~2.1 sec
- `200` = ~8.3 sec

### Create Custom Animation

Replace `draw_color_bar_pattern()`:

```c
void draw_color_bar_pattern(void)
{
    /* Your animation code here */
    /* Available: LCD_fillScreen(), LCD_fillRect(), LCD_drawImage() */
    /* Current frame: frame_count */
    /* Color palette: color_palette[] */
}
```

**Important:** Keep rendering fast - simple operations only.

## Performance

### ST7789 (240×320)

| Metric | Value |
|--------|-------|
| Frame Data Size | 153,600 bytes |
| SPI Speed | 40 MHz |
| SPI Transfer Time | ~30.7 ms |
| Actual FPS | ~24 FPS |
| Memory Usage | ~12 bytes (state only) |
| CPU per Frame | <1 ms |

### ST7735 (128×128)

| Metric | Value |
|--------|-------|
| Frame Data Size | 32,768 bytes |
| SPI Speed | 15 MHz |
| SPI Transfer Time | ~17.5 ms |
| Actual FPS | ~24 FPS |
| Memory Usage | ~12 bytes (state only) |
| CPU per Frame | <1 ms |

## Implementation Details

### SPI Communication

- **Interface:** SPI0
- **Data Width:** 8-bit
- **Mode:** POL0_PHA0 (standard SPI)
- **CS:** Software controlled (GPIO13, active low)
- **Transfer:** Blocking `SPI_transfer()` via TI Drivers
- **Color Format:** RGB565 (2 bytes per pixel)

### GPIO Control

- **CS (GPIO13):** Low during transfer
- **DC/RS (GPIO34):** High for data, Low for commands
- **RST (GPIO30):** Active-low reset

### Display Initialization

1. Assert reset (low), wait 10ms
2. Deassert reset (high), wait 120ms
3. Send configuration commands
4. Display ready

Full sequence in `LCD_init()` function.

## File Structure

```
SPI_Display/
├── spi_display.c           Main application logic
├── spi_display.syscfg      TI Drivers configuration
├── main_freertos.c         FreeRTOS bootstrap
├── display_config.h        Display selection (ST7789/ST7735)
├── st7789_lcd.c/h          ST7789 driver (240×320)
├── st7735_lcd.c/h          ST7735 driver (128×128)
├── README.md               This file
└── Debug/                  Build output
```

## License

This project is provided as-is for educational and commercial use.
