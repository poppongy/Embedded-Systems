# AECN — Automotive Embedded Control Node
Baremetal STM32F407 CAN Bus Firmware | CMSIS Register-Level | Two-Node Network

## Overview
AECN is a portfolio-grade embedded firmware project demonstrating a complete two-node CAN bus
network built entirely at the register level on the STM32F407 Discovery board using CMSIS — no
HAL, no middleware, no abstractions beyond what was written from scratch.

The project implements four communication drivers, a cooperative software scheduler, and an
automotive-grade node supervision system — all running concurrently without an RTOS.

Both nodes are built from the same codebase.** A single compile-time flag selects the node
role at build time, mirroring how production automotive firmware handles multi-ECU builds.

---

## Demonstration

> Demo video link — coming soon

**What the demo shows:**
- Both nodes running simultaneously on real hardware
- 100ms heartbeat transmission verified on logic analyzer
- Button press triggering a CAN command frame to Node B
- Node B detecting Node A fault when unplugged
- Node B logging recovery when Node A reconnects
- Cooperative scheduler running uninterrupted throughout

---

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        Node A — Producer                        │
│                      STM32F407 Discovery                        │
│                                                                 │
│  ┌──────────┐   ┌──────────┐   ┌──────────┐   ┌────────────┐  │
│  │  TIM2    │   │  EXTI    │   │  USART2  │   │   bxCAN    │  │
│  │Scheduler │   │  Button  │   │   UART   │   │  TX/RX     │  │
│  │  1ms IRQ │   │  PB12    │   │  PA2     │   │  PD0/PD1   │  │
│  └────┬─────┘   └────┬─────┘   └──────────┘   └─────┬──────┘  │
│       │              │                               │         │
│  ┌────▼──────────────▼───────────────────────────────▼──────┐  │
│  │              Cooperative State Machine                    │  │
│  │    Heartbeat TX (100ms) | Button Event | LED Toggle       │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────┬───────────────────────────┘
                                      │
                          ┌───────────▼────────────┐
                          │   SN65HVD230            │
                          │   CAN Transceiver       │
                          └───────────┬─────────────┘
                                      │
                              CAN_H ──┤── 120Ω
                              CAN_L ──┤── 120Ω
                                      │
                          ┌───────────▼─────────────┐
                          │   SN65HVD230             │
                          │   CAN Transceiver        │
                          └───────────┬──────────────┘
                                      │
┌─────────────────────────────────────▼───────────────────────────┐
│                        Node B — Receiver                        │
│                      STM32F407 Discovery                        │
│                                                                 │
│  ┌──────────┐   ┌──────────┐   ┌──────────┐   ┌────────────┐  │
│  │  TIM2    │   │  bxCAN   │   │  USART2  │   │  Node      │  │
│  │Scheduler │   │  RX IRQ  │   │   UART   │   │ Supervisor │  │
│  │  1ms IRQ │   │  FIFO0   │   │  PA2     │   │ Watchdog   │  │
│  └──────────┘   └──────────┘   └──────────┘   └────────────┘  │
│                                                                 │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │              Cooperative State Machine                    │  │
│  │   CAN RX Processing | Heartbeat Supervision | LED Toggle  │  │
│  └───────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

---

## Features

### Drivers (CMSIS register-level, no HAL)
| Driver | Peripheral | Key Details |
|--------|-----------|-------------|
| Timer | TIM2 | 1ms interrupt-driven tick, PSC=1600-1, ARR=10-1, 16MHz HSI |
| External Interrupt | EXTI | PB12 falling edge, SYSCFG routing, deferred ISR processing |
| UART | USART2 | 115200 baud, PA2 TX, blocking TX driver, snprintf logging layer |
| CAN | bxCAN | 500kbps, PD0/PD1 AF9, acceptance filter bank 0, TX mailbox, RX FIFO0 IRQ |

### Cooperative Scheduler
- TIM2-based 1ms software timebase
- `soft_timer_t` abstraction with drift-free `timer_expired_periodic()`
- Multiple concurrent tasks with zero RTOS overhead
- Verified 100ms heartbeat with zero drift on logic analyzer

### CAN Network
- Real two-node network over SN65HVD230 differential transceivers
- Real ACKs — not loopback simulation
- Heartbeat frame: ID=0x100, DLC=1, DATA=0xAA at 100ms intervals
- Button event frame: ID=0x123, DLC=3, DATA=0x11 0x22 0x33

### Node Supervision (Node B)
- Timestamp-based watchdog — no timer drift dependency
- 500ms timeout threshold — tolerates 5 missed heartbeats before fault
- Three-state machine: `NODE_A_UNKNOWN` → `NODE_A_ALIVE` → `NODE_A_OFFLINE`
- Fault logged once on transition — no UART flooding
- Recovery detected and logged automatically on heartbeat resumption

---

## Hardware

### Bill of Materials
| Component | Quantity | Purpose |
|-----------|----------|---------|
| STM32F407 Discovery (MB997) | 2 | Node A and Node B |
| SN65HVD230 CAN Transceiver Module | 2 | Differential bus driver |
| 120Ω resistor | 2 | CAN bus termination |
| Jumper wires | — | Wiring |

### Pin Assignments
| Signal | Pin | Alternate Function |
|--------|-----|--------------------|
| CAN1_RX | PD0 | AF9 |
| CAN1_TX | PD1 | AF9 |
| USART2_TX | PA2 | AF7 |
| EXTI Button | PB12 | — |
| LED Green | PD12 | — |
| LED Orange | PD13 | — |
| LED Red | PD14 | — |

### Bus Wiring
```
Node A                    Node B
STM32 PD1 → TXD ─────────────── TXD ← STM32 PD1
STM32 PD0 ← RXD          RXD → STM32 PD0
3.3V      → VCC           VCC ← 3.3V
GND       → GND ─── GND ← GND

          CAN_H ─── 120Ω ─┐
          CAN_L ───────────┤
          CAN_H ───────────┤
          CAN_L ─── 120Ω ─┘
```

---

## Project Structure

```
AECN/
├── Application/
│   ├── app.c                  # main loop entry point
│   ├── state_machine.c        # cooperative scheduler + node logic
│   └── state_machine.h
├── Drivers/
│   ├── Inc/
│   │   ├── gpio_drv.h
│   │   ├── timer_drv.h
│   │   ├── exti_drv.h
│   │   ├── uart_drv.h
│   │   └── can_drv.h
│   └── Src/
│       ├── gpio_drv.c         # baremetal GPIO driver
│       ├── timer_drv.c        # TIM2 scheduler + soft_timer_t API
│       ├── exti_drv.c         # EXTI interrupt driver
│       ├── uart_drv.c         # USART2 TX driver
│       └── can_drv.c          # bxCAN full driver
├── Core/
│   └── main.c
├── docs/
│   └── learning/
│       ├── timer_notes.md
│       ├── exti_notes.md
│       ├── uart_notes.md
│       └── can_notes.md
		└── README.md
```

---

## Building and Flashing

### Prerequisites
- STM32CubeIDE 2.0.0
- Two STM32F407 Discovery boards
- Two SN65HVD230 transceiver modules
- ST-Link USB driver

### Flash Node A (Producer)
1. Open project in STM32CubeIDE
2. In `state_machine.h` set:
```c
#define NODE_ROLE NODE_PRODUCER
```
3. Connect Node A via USB
4. Build and flash (Run → Debug or Ctrl+F11)

### Flash Node B (Receiver)
1. In `state_machine.h` set:
```c
#define NODE_ROLE NODE_RECEIVER
```
2. Connect Node B via USB
3. Build and flash

### Verify Operation
- Connect logic analyzer to PA2 (UART TX) on either node at 115200 baud
- Node A should show `HB TX OK` every 100ms
- Node B should show `Heartbeat detected` on first contact
- Unplug Node A — Node B logs `Heartbeat is offline` within 500ms
- Replug Node A — Node B logs `Heartbeat recovered`

---

## Key Engineering Decisions

**Why baremetal over HAL?**
HAL abstractions hide the hardware. Writing directly to CMSIS registers builds genuine understanding of peripheral behavior — critical for debugging, optimizing, and adapting firmware to new hardware.

**Why a cooperative scheduler over RTOS?**
A cooperative scheduler is simpler, deterministic, and sufficient for this task set. The deliberate choice to hit its limits before introducing FreeRTOS ensures understanding of both approaches rather than defaulting to RTOS for everything.

**Why timestamp-based watchdog over soft timer?**
A `soft_timer_t` watchdog runs independently of CAN RX events and can trigger false faults between valid heartbeats. A timestamp comparison against `timer_get_ticks()` only evaluates elapsed time when polled — no race condition, no false positives.

**Why disable FMPIE0 in the ISR?**
Without disabling the interrupt, the CAN controller re-triggers the ISR continuously while FMP0 remains set — before the main loop has a chance to read and release the mailbox. Disabling on entry and re-enabling after processing is the correct deferred processing pattern.

---

## What I Learned

- STM32 bxCAN initialization requires exiting sleep mode before requesting init mode — INAK will not clear without a stable bus in normal mode
- The 11 consecutive recessive bits requirement means INAK polling hangs without a real transceiver in normal mode — loopback mode satisfies this internally
- Clearing `EXTI->PR` with `=` instead of `|=` prevents accidentally clearing pending flags for other lines sharing the same vector
- `timer_expired_periodic()` uses `start += period` to prevent drift accumulation — critical for periodic CAN scheduling
- ISR principle: do the minimum, set a flag, defer all work to main loop context

---

## Logic Analyzer Evidence

| Test | Result |
|------|--------|
| Heartbeat interval (Node A) | 100ms ± 0ms — zero drift |
| Button press + heartbeat coexistence | No scheduler disruption |
| Node B first contact | `Heartbeat detected` logged correctly |
| Fault detection | `Heartbeat is offline` within 500ms of Node A disconnect |
| Recovery detection | `Heartbeat recovered` logged on Node A reconnect |
| Timer event during CAN activity | Scheduler unaffected |

---

## Author

**Prince** — Recent Software Engineering graduate
Clarksville, Tennessee
GitHub: [poppongy](https://github.com/poppongy)

*Targeting entry-level embedded firmware roles in automotive, aviation, and medical device industries.*

---

## References

- STM32F407 Reference Manual (RM0090)
- STM32F407 Discovery Schematic (MB997 Rev B.2)
- CMSIS Core Documentation
- ISO 11898-1 CAN Bus Standard