//Pin selection
Always select pins that are free and not been used for other purposes

Best practice in register configuration is to always clear first before setting. Without
clearing, you can run into incorrect mapping if the register already contains previous
values. Ex. 
SYSCFG->EXTICR[3] &= ~(0xF << 0);   // clear EXTI12 field
SYSCFG->EXTICR[3] |=  (0x1 << 0);   // select Port B

	
//Falling Edge detection
EXTI->FTSR &= ~EXTI_FTSR_TR12; not necessary just enabling edge 
trigger does not require clearing bits before selection. however,
it is important to disable for instance rising edge if you are 
enabling falling edge. This ensures interrupt triggers only on button press
not on release 
EXTI->FTSR |=EXTI_FTSR_TR12;

 //In the ISR
 NVIC clearing is rarely needed in normal ISR handling.
 
 calling flow should follow this pattern -- Hardware must be configured before interrupt routing is enabled
 main()
 ├── system_init()
 │    └── timer2_init()
 │
 └── app_init()
      └── state_machine_init()
           ├── gpio_init(PD12 output)
           ├── gpio_init(PB12 input)
           └── exti_init()

**Why clear PR before unmasking IMR:**
Exactly right. The PR bit can be set by a spurious signal even before your code runs. If you unmask first, that stale flag fires an immediate unexpected interrupt the moment IMR opens the gate. Clear first, then open the gate — correct reasoning.

**Why `=` not `|=` when clearing PR in ISR:**
Your intent explanation is right but the hardware reason is stronger. The PR register is a **write-1-to-clear** register — writing a `1` clears the bit, not sets it. So `|=` is actually not wrong in this specific case. The real danger with `|=` is that it first does a **read** of PR, then ORs, then writes — in that window another line could have set its PR bit, and your write would accidentally clear that one too. Using `= EXTI_PR_PR12` writes only the exact bit you intend. That's the professional reason.

