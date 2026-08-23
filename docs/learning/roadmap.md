Milestone 1 — Interview-Ready Build
Week 1 — EXTI Event System

 Configure PB12 as EXTI input via SYSCFG
 Enable NVIC line for EXTI15_10
 Implement ISR with volatile flag signal
 Integrate flag check into state_machine_run()
 Demonstrate event-driven LED behavior
 git commit -m "feat: add EXTI button event system"

Week 2 — UART Debug Interface

 Configure USART2 peripheral and GPIO alternate functions
 Calculate baud rate register value for 115200
 Implement blocking uart_send_char() and uart_print()
 Create logging layer: log_event(), log_tick()
 Print scheduler heartbeat and EXTI events over UART
 git commit -m "feat: add UART debug logging layer"

Week 3 — CAN Peripheral Bring-Up

 Configure CAN1 GPIO alternate functions (PD0/PD1)
 Set bxCAN registers: MCR, BTR for 500kbps
 Configure acceptance filters (filter bank 0)
 Initialize TX mailbox and transmit a test frame
 Verify TX with logic analyzer or second node
 git commit -m "feat: bxCAN init and first TX frame"

Week 4 — CAN RX Interrupt Handling

 Enable FIFO0 message pending interrupt (USB_LP_CAN1_RX0_IRQn)
 Implement CAN1_RX0_IRQHandler to read mailbox
 Parse CAN ID and data bytes from received frame
 Route parsed commands into state machine logic
 Control LED color/state via received CAN command
 git commit -m "feat: CAN RX interrupt + command handler"

Week 5 — Periodic CAN Heartbeat Scheduler

 Define heartbeat frame: ID=0x100, data=node status byte
 Create soft_timer_t for 100ms periodic TX
 Use timer_expired_periodic() in scheduler loop
 Log each heartbeat transmission over UART
 Validate 100ms interval with logic analyzer
 git commit -m "feat: 100ms CAN heartbeat scheduler"

Week 6 — Multi-Node Demo + Portfolio Packaging

 Set up second STM32 node with unique CAN ID
 Implement command/response message exchange
 Capture logic analyzer trace of two-node traffic
 Draw architecture diagram for GitHub README
 Write project README with build instructions
 git commit -m "docs: final architecture and portfolio packaging"

