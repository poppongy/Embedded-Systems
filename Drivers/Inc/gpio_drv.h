/*
 * gpio_drv.h
 *
 *  Created on: Feb 16, 2026
 *      Author: Prince
 */

#ifndef INC_GPIO_DRV_H_
#define INC_GPIO_DRV_H_

#include <stdint.h>
#include "stm32f4xx.h"

//#define RCC_BASE        0x40023800UL
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30UL))
#define BASE_ADDR_AHB1					0x40020000UL
/*
 * Defining base addresses of all Peripherals hanging on AHB1 BUS
 */
#define BASE_ADDR_GPIOA                 (BASE_ADDR_AHB1 + 0x0000)
#define BASE_ADDR_GPIOB                 (BASE_ADDR_AHB1 + 0x0400)
#define BASE_ADDR_GPIOC                 (BASE_ADDR_AHB1 + 0x0800)
#define BASE_ADDR_GPIOD                 (BASE_ADDR_AHB1 + 0x0C00)
#define BASE_ADDR_GPIOE                 (BASE_ADDR_AHB1 + 0x1000)
#define BASE_ADDR_GPIOF                 (BASE_ADDR_AHB1 + 0x1400)
#define BASE_ADDR_GPIOG                 (BASE_ADDR_AHB1 + 0x1800)
#define BASE_ADDR_GPIOH                 (BASE_ADDR_AHB1 + 0x1C00)
#define BASE_ADDR_GPIOI                 (BASE_ADDR_AHB1 + 0x2000)

typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2];
} GPIO_RegDef_t;

typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
	GPIO_PORT_G,
	GPIO_PORT_H,
	GPIO_PORT_I
} gpio_port_t;

typedef enum {
    GPIO_MODE_INPUT = 0,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_AF,
    GPIO_MODE_ANALOG
} gpio_mode_t;

#define PIN_0   	0
#define PIN_1  	 	1
#define PIN_2   	2
#define PIN_3   	3
#define PIN_4   	4
#define PIN_5   	5
#define PIN_6   	6
#define PIN_7   	7
#define PIN_8   	8
#define PIN_9   	9
#define PIN_10  	10
#define PIN_11  	11
#define PIN_12  	12
#define PIN_13  	13
#define PIN_14  	14
#define PIN_15  	15

/*
 	 * PULL UP PULL DOWN RESISTOR CONFIG
*/
#define NO_PUPD    		0
#define PULL_UP         1
#define PULL_DOWN    	2

/*
 	 * ALTERNATE FUNCTION CONFIG
*/
#define AF0							0
#define AF1   						1
#define AF2   						2
#define AF3   						3
#define AF4   						4
#define AF5   						5
#define AF6   						6
#define AF7   						7
#define AF8   						8
#define AF9   						9
#define AF10   						10
#define AF11   						11
#define AF12   						12
#define AF13   						13
#define AF14   						14
#define AF15   						15

void gpio_init(gpio_port_t port, uint8_t pin, gpio_mode_t mode, uint8_t pupd, uint8_t alt);
void gpio_toggle(gpio_port_t port, uint8_t pin);

#endif /* INC_GPIO_DRV_H_ */
