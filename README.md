# Embedded-Systems
Creating Industry ready Projects
⚠️Work in Progress – Firmware implementation coming soon

Real-Time Embedded Control System with Fault-Tolerant Communication
    Bare-metal embedded firmware demonstrating real-time control, interrupt-driven communication, and fault-tolerant state management. Designed for use across automotive, industrial,        aerospace, and medical applications.

Table of Contents
Overview
Architecture
Features
Concepts Covered
Hardware Setup
Build Instructions
Usage
License

Overview
    This project implements a real-time embedded controller on an ARM Cortex-M microcontroller, designed with professional embedded software practices.

Key goals:
    Execute deterministic periodic tasks using timer interrupts.
    Interface with hardware peripherals (GPIO, LEDs, sensors).
    Implement interrupt-driven communication with multiple backends (CAN, UART).
    Detect faults, enter safe states, and support recovery.
    Follow a layered architecture for maintainability and scalability.
    The firmware is bare-metal, but structured to reflect real-world embedded systems found in automotive ECUs, industrial controllers, aerospace subsystems, and medical devices.

ARCHITECTURE
+-----------------------------------+
| Application Layer                 |
|  - System State Machine           |
|  - Control Logic                  |
|  - Fault Manager                  |
+-----------------------------------+
| Service Layer                     |
|  - Scheduler                      |
|  - Communication Interface (API)  |
|  - Diagnostics                    |
+-----------------------------------+
| Driver Layer                      |
|  - Timer Driver                   |
|  - GPIO Driver                    |
|  - Communication Drivers          |
|     (CAN / UART / SPI)            |
+-----------------------------------+
| Hardware (STM32 MCU)              |
+-----------------------------------+
Highlights:
    Layered structure separates high-level logic from low-level drivers.
    Abstracted communication allows for protocol flexibility.
    State machine ensures predictable and safe behavior.

Features
    Real-time task scheduling without RTOS
    Interrupt-driven communication (CAN / UART)
    State-machine-based system control: INIT → RUN → FAULT
    Fault detection and safe-state management
    Hardware abstraction for GPIO, LEDs, and timers
    Modular, layered code structure
    Support for multiple communication backends

Concepts Covered
    See Concepts Covered Section below.
 
Hardware Setup
    Minimum Hardware Requirements:
    ARM Cortex-M microcontroller (STM32F407 recommended)
    Optional: CAN transceiver (MCP2551 / SN65HVD230)
    Optional: USB-to-CAN adapter for PC communication
    LEDs for fault indication (any GPIO-driven LED)
    Breadboard and jumper wire

Build Instructions
    Install STM32CubeIDE
    Clone this repository:
    git clone https://github.com/poppongy/Embedded-Software.git
    Open the project in STM32CubeIDE.
    Configure the MCU and clock in main.c or board_config.h.
    Build the project and flash to the MCU.
    Use debug tools or terminal to monitor communication messages and fault status.

Usage
    MCU will automatically start in INIT state on power-up.
    Periodic tasks (sensor reading, communication) run automatically via timer interrupts.
    Faults (e.g., simulated sensor errors) trigger FAULT state and indicate via LEDs.
    Communication interface handles both sending status messages and receiving commands.
    Safe-state management ensures deterministic and predictable behavior under error conditions.

Concepts Covered
    Embedded Software Architecture: Layered structure, modular design, hardware abstraction.
    Real-Time Systems: Timer-based periodic tasks, deterministic scheduling.
    Bare-Metal Programming: Direct peripheral access, main loop + ISR coordination.
    Interrupt Handling: Short, non-blocking ISRs for timers and communication.
    Embedded Communication: CAN and UART interface, message-based handling.
    Fault Detection & Safety-Oriented Design: Safe-state entry, degraded operation, error flagging.
    State Machines: INIT → RUN → FAULT, deterministic transitions.
    Hardware Interaction: GPIO, LEDs, pin multiplexing, peripheral control.
    Debugging & Validation: Fault injection, monitoring, incremental bring-up.
    Professional Practices: Modular code, Git version control, comprehensive documentation

License
    This project is licensed under the MIT License. See LICENSE for details.
