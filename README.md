#Embedded Software Engineering Portfolio

STM32 Bare-Metal Two-Node CAN Network

Status: Phase 1 Complete (CAN Communication)Next Milestone: I²C Sensor Integration and SPI Flash Storage

Overview

This project demonstrates a bare-metal embedded firmwarearchitecture implemented on two STM32F407 Discovery boards using CMSIS register-level programming.

The firmware implements reusable peripheral drivers, interrupt-drivencommunication, cooperative scheduling, and application-layer CANmessaging without relying on STM32 HAL or an RTOS.

Features

    CMSIS register-level peripheral configuration   
    Modular driver architecture 
    GPIO driver
    Timer driver
    EXTI driver
    UART driver
    CAN driver
    Cooperative software scheduler
    Interrupt-driven CAN reception
    Two-node CAN communication
    Heartbeat supervision 
    Heartbeat timeout detection 
    Heartbeat recovery detection
    Button-event messaging over CAN
    UART diagnostics
    Logic analyzer verification

Hardware

STM32F407 Discovery (Node A)

STM32F407 Discovery (Node B)

SN65HVD230 CAN Transceivers

Logic Analyzer

Breadboard

Push Button

LEDs

Software Architecture

Application
│
├── State Machine
│
├── Cooperative Scheduler
│
├── CAN Message Processing
│
├── UART Diagnostics
│
└── Driver Layer
      ├── GPIO
      ├── Timer
      ├── EXTI
      ├── UART
      └── CAN

CAN Network

Two STM32 nodes communicate using a custom CAN application protocol.

Heartbeat

Producer
   │
Heartbeat (0x100)
   │
   ▼
Receiver

The receiver supervises:

Startup detection

Timeout detection

Recovery detection

Button Event

Producer
Button Press
     │
CAN Frame (0x123)
     │
     ▼
Receiver

The receiver:

Reads the CAN frame

Decodes the payload

Toggles an LED

Logs the received frame over UART

Verification

The project has been validated using:

UART logging

Logic analyzer captures

Dual-node testing

Verified functionality:

Heartbeat transmission

Heartbeat reception

Heartbeat timeout

Heartbeat recovery

Button event transmission

Button event reception

CAN transmit timeout handling

Cooperative scheduler operation

Concepts Demonstrated

Bare-metal embedded programming

CMSIS register-level development

Interrupt-driven firmware

Cooperative scheduling

Driver abstraction

CAN protocol implementation

UART-based debugging

Embedded state machines

Timeout handling

Modular firmware design

Future Development

The next milestones include:

I²C driver

BME280 environmental sensor integration

SPI driver

W25Q64 flash memory integration

CAN request/response messaging

Data logging to external flash

Repository Status

Phase                   Status

GPIO                    ✅ CompleteTimers                  ✅ CompleteEXTI                    ✅ CompleteUART                    ✅ CompleteCAN Driver              ✅ CompleteTwo-Node CAN Network    ✅ CompleteHeartbeat Supervision   ✅ CompleteI²C Driver              🔄 NextSPI Driver              🔄 Planned

License

This project is licensed under the MIT License. See the LICENSE filefor details.
