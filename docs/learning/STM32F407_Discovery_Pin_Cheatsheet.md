# STM32F407 Discovery — Complete Pin Reference
Derived from official schematic MB997 Rev B.2 (all 6 pages)
For use with AECN CAN Node Project

---

## Legend
- ✅ FREE — safe to use, no onboard connection
- ❌ AVOID — connected to onboard hardware
- ⚠️ YOURS — currently used in your project
- 〰️ SHARED — connected to ST-Link, usable with caution

---

## PORT A

| Pin  | Status | Connected To | Notes |
|------|--------|-------------|-------|
| PA0  | ❌ AVOID | USER/WAKE-UP button (B1) | Pg 6 — pulls high on press |
| PA1  | ✅ FREE | Not connected | — |
| PA2  | ⚠️ YOURS | USART2 TX (your UART driver) | Also ST-Link shared line |
| PA3  | 〰️ SHARED | USART2 RX | Reserve alongside PA2 |
| PA4  | ❌ AVOID | Audio codec CS43L22 — I2S3_WS (LRCK) | Pg 4 |
| PA5  | ❌ AVOID | MEMS accelerometer LIS302DL — SPI1_SCK | Pg 6 |
| PA6  | ❌ AVOID | MEMS accelerometer LIS302DL — SPI1_MISO | Pg 6 |
| PA7  | ❌ AVOID | MEMS accelerometer LIS302DL — SPI1_MOSI | Pg 6 |
| PA8  | ✅ FREE | Not connected | — |
| PA9  | ❌ AVOID | USB OTG FS — VBUS sense | Pg 5 |
| PA10 | ❌ AVOID | USB OTG FS — ID | Pg 5 |
| PA11 | ❌ AVOID | USB OTG FS — D- (DM) | Pg 5 — do not use |
| PA12 | ❌ AVOID | USB OTG FS — D+ (DP) | Pg 5 — do not use |
| PA13 | ❌ AVOID | SWD — SWDIO (debugger) | Pg 1 — never reassign |
| PA14 | ❌ AVOID | SWD — SWCLK (debugger) | Pg 1 — never reassign |
| PA15 | ✅ FREE | Not connected (JTDI in JTAG mode) | Safe in SWD mode |

---

## PORT B

| Pin  | Status | Connected To | Notes |
|------|--------|-------------|-------|
| PB0  | ✅ FREE | Not connected | — |
| PB1  | ✅ FREE | Not connected | — |
| PB2  | ❌ AVOID | BOOT1 control | Pg 3 — affects boot mode |
| PB3  | ❌ AVOID | SWD — SWO trace output (ST-Link) | Pg 1 |
| PB4  | ✅ FREE | Not connected (JNTRST in JTAG) | Safe in SWD mode |
| PB5  | ✅ FREE | Not connected | — |
| PB6  | ❌ AVOID | Audio codec CS43L22 — I2C1 SCL | Pg 4 |
| PB7  | ✅ FREE | Not connected | — |
| PB8  | ✅ FREE | Not connected | ✓ Good I2C1 SCL alternative |
| PB9  | ❌ AVOID | Audio codec CS43L22 — I2C1 SDA | Pg 4 — do not use for I2C |
| PB10 | ❌ AVOID | MEMS microphone MP45DT02 — I2S2 CLK | Pg 4 |
| PB11 | ✅ FREE | Not connected | ✓ Good I2C1 SDA alternative |
| PB12 | ⚠️ YOURS | Your external EXTI button | User-wired |
| PB13 | ✅ FREE | Not connected | ✓ Good SPI SCK |
| PB14 | ✅ FREE | Not connected | ✓ Good SPI MISO |
| PB15 | ✅ FREE | Not connected | ✓ Good SPI MOSI |

---

## PORT C

| Pin  | Status | Connected To | Notes |
|------|--------|-------------|-------|
| PC0  | ❌ AVOID | USB OTG FS — power switch enable | Pg 5 |
| PC1  | ✅ FREE | Not connected | — |
| PC2  | ✅ FREE | Not connected | — |
| PC3  | ❌ AVOID | Audio + MEMS microphone PDM_OUT | Pg 4 |
| PC4  | ❌ AVOID | Audio codec — PDM (not fitted but wired) | Pg 4 |
| PC5  | ✅ FREE | Not connected | — |
| PC6  | ✅ FREE | Not connected | — |
| PC7  | ❌ AVOID | Audio codec CS43L22 — I2S3_SCK (MCLK) | Pg 4 |
| PC8  | ✅ FREE | Not connected | — |
| PC9  | ✅ FREE | Not connected | — |
| PC10 | ❌ AVOID | Audio codec CS43L22 — I2S3_SCK | Pg 4 |
| PC11 | ✅ FREE | Not connected | — |
| PC12 | ❌ AVOID | Audio codec CS43L22 — I2S3_SD | Pg 4 |
| PC13 | ✅ FREE | Not connected | ⚠️ Max 3mA drive — no direct LED |
| PC14 | ❌ AVOID | OSC32_IN — 32.768kHz crystal input | Pg 3 |
| PC15 | ❌ AVOID | OSC32_OUT — 32.768kHz crystal output | Pg 3 |

---

## PORT D

| Pin  | Status | Connected To | Notes |
|------|--------|-------------|-------|
| PD0  | ✅ FREE | Not connected | ✅ BEST choice — CAN1_RX (AF9) |
| PD1  | ✅ FREE | Not connected | ✅ BEST choice — CAN1_TX (AF9) |
| PD2  | ✅ FREE | Not connected | — |
| PD3  | ✅ FREE | Not connected | — |
| PD4  | ❌ AVOID | Audio codec CS43L22 — RESET line | Pg 4 |
| PD5  | ❌ AVOID | USB OTG FS — overcurrent detect | Pg 5 |
| PD6  | ✅ FREE | Not connected | — |
| PD7  | ✅ FREE | Not connected | — |
| PD8  | ✅ FREE | Not connected | — |
| PD9  | ✅ FREE | Not connected | — |
| PD10 | ✅ FREE | Not connected | — |
| PD11 | ✅ FREE | Not connected | — |
| PD12 | ⚠️ YOURS | Onboard LED4 — Green | Pg 6 — your project uses it |
| PD13 | ❌ AVOID | Onboard LED3 — Orange | Pg 6 |
| PD14 | ⚠️ YOURS | Onboard LED5 — Red | Pg 6 — your project uses it |
| PD15 | ❌ AVOID | Onboard LED6 — Blue | Pg 6 |

---

## PORT E

| Pin  | Status | Connected To | Notes |
|------|--------|-------------|-------|
| PE0  | ❌ AVOID | MEMS accelerometer LIS302DL — INT1 | Pg 6 |
| PE1  | ❌ AVOID | MEMS accelerometer LIS302DL — INT2 | Pg 6 |
| PE2  | ✅ FREE | Not connected | — |
| PE3  | ❌ AVOID | MEMS accelerometer LIS302DL — CS_I2C/SPI | Pg 6 |
| PE4  | ✅ FREE | Not connected | — |
| PE5  | ✅ FREE | Not connected | — |
| PE6  | ✅ FREE | Not connected | — |
| PE7  | ✅ FREE | Not connected | — |
| PE8  | ✅ FREE | Not connected | — |
| PE9  | ✅ FREE | Not connected | — |
| PE10 | ✅ FREE | Not connected | — |
| PE11 | ✅ FREE | Not connected | — |
| PE12 | ✅ FREE | Not connected | — |
| PE13 | ✅ FREE | Not connected | — |
| PE14 | ✅ FREE | Not connected | — |
| PE15 | ✅ FREE | Not connected | — |

---

## Quick Summary Count

| Status | Count |
|--------|-------|
| ✅ FREE | 40 pins |
| ❌ AVOID (onboard hardware) | 26 pins |
| ⚠️ YOURS (project in use) | 4 pins |
| 〰️ SHARED / caution | 1 pin |

---

## Current Project Pin Usage

| Pin | Peripheral | Function |
|-----|-----------|---------|
| PA2 | USART2 | TX — UART debug logging |
| PA3 | USART2 | RX — reserved |
| PB12 | EXTI line 12 | External button interrupt |
| PD12 | GPIO output | LED green toggle |
| PD14 | GPIO output | LED red toggle |
| TIM2 | Timer | 1ms scheduler timebase |

---

## Recommended Future Peripheral Allocation

| Peripheral | Recommended Pins | Alternate Function |
|-----------|-----------------|-------------------|
| CAN1 TX/RX | PD1 / PD0 | AF9 |
| I2C1 SCL/SDA | PB8 / PB11 | AF4 (PB9 is taken — avoid) |
| SPI1/2 SCK/MISO/MOSI | PB13 / PB14 / PB15 | AF5 |
| Additional EXTI | PE4–PE15 | Any free PE pin |
| Additional UART | PA8 + free pin | Check AF mapping |

---

## Critical Rules

1. Never reassign PA13, PA14 — SWD debugger will stop working
2. Never use PA11, PA12 — USB D+/D- permanently wired
3. PB9 is taken by audio I2C — use PB11 for I2C SDA instead
4. PC13 max drive current is 3mA — never connect LED directly
5. PD0 and PD1 are your cleanest CAN1 pins — nothing else touches them
6. Always verify SystemCoreClock before calculating BRR or timer PSC values
7. Check this sheet before picking any new pin — saves hours of debugging

---

*Generated from STM32F4DISCOVERY schematic MB997 Rev B.2 — January 2012*
*Project: AECN CAN Node Firmware — STM32F407 Discovery*