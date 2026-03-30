/*
 * gpio_drv.c
 *
 *  Created on: Feb 15, 2026
 *      Author: Prince
 */

#include "gpio_drv.h"

static GPIO_RegDef_t* gpio_regs(gpio_port_t port)
{
    switch (port) {
        case GPIO_PORT_A: return (GPIO_RegDef_t*)BASE_ADDR_GPIOA;
        case GPIO_PORT_B: return (GPIO_RegDef_t*)BASE_ADDR_GPIOB;
        case GPIO_PORT_C: return (GPIO_RegDef_t*)BASE_ADDR_GPIOC;
        case GPIO_PORT_D: return (GPIO_RegDef_t*)BASE_ADDR_GPIOD;
        case GPIO_PORT_E: return (GPIO_RegDef_t*)BASE_ADDR_GPIOE;
        case GPIO_PORT_F: return (GPIO_RegDef_t*)BASE_ADDR_GPIOF;
        case GPIO_PORT_G: return (GPIO_RegDef_t*)BASE_ADDR_GPIOG;
        case GPIO_PORT_H: return (GPIO_RegDef_t*)BASE_ADDR_GPIOH;
        case GPIO_PORT_I: return (GPIO_RegDef_t*)BASE_ADDR_GPIOI;
        default:          return (GPIO_RegDef_t*)BASE_ADDR_GPIOD;
    }
}

static void gpio_clock_enable(gpio_port_t port)
{
    // AHB1ENR bits: A=0 B=1 C=2 D=3 E=4 H=7
    switch (port) {
        case GPIO_PORT_A: RCC_AHB1ENR |= (1UL << 0); break;
        case GPIO_PORT_B: RCC_AHB1ENR |= (1UL << 1); break;
        case GPIO_PORT_C: RCC_AHB1ENR |= (1UL << 2); break;
        case GPIO_PORT_D: RCC_AHB1ENR |= (1UL << 3); break;
        case GPIO_PORT_E: RCC_AHB1ENR |= (1UL << 4); break;
        case GPIO_PORT_F: RCC_AHB1ENR |= (1UL << 5); break;
        case GPIO_PORT_G: RCC_AHB1ENR |= (1UL << 6); break;
        case GPIO_PORT_H: RCC_AHB1ENR |= (1UL << 7); break;
        case GPIO_PORT_I: RCC_AHB1ENR |= (1UL << 8); break;
        default: break;
    }
    //(void)RCC_AHB1ENR; // small sync read
}

void gpio_init_output(gpio_port_t port, uint8_t pin)
{
    GPIO_RegDef_t* GPIOx = gpio_regs(port);
    gpio_clock_enable(port);

    // MODER: 01 = general purpose output
    GPIOx->MODER &= ~(3UL << (pin * 2));
    GPIOx->MODER |=  (1UL << (pin * 2));
}

void gpio_toggle(gpio_port_t port, uint8_t pin)
{
    GPIO_RegDef_t* GPIOx = gpio_regs(port);
    GPIOx->ODR ^= (1UL << pin);
}


