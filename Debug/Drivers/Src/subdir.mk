################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Src/gpio_drv.c \
../Drivers/Src/timer_drv.c 

OBJS += \
./Drivers/Src/gpio_drv.o \
./Drivers/Src/timer_drv.o 

C_DEPS += \
./Drivers/Src/gpio_drv.d \
./Drivers/Src/timer_drv.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Src/%.o Drivers/Src/%.su Drivers/Src/%.cyclo: ../Drivers/Src/%.c Drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407xx -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Application" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Core" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/Inc" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/Src" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/CMSIS/Core/Include" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Src

clean-Drivers-2f-Src:
	-$(RM) ./Drivers/Src/gpio_drv.cyclo ./Drivers/Src/gpio_drv.d ./Drivers/Src/gpio_drv.o ./Drivers/Src/gpio_drv.su ./Drivers/Src/timer_drv.cyclo ./Drivers/Src/timer_drv.d ./Drivers/Src/timer_drv.o ./Drivers/Src/timer_drv.su

.PHONY: clean-Drivers-2f-Src

