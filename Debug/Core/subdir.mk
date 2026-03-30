################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/main.c \
../Core/system_init.c 

OBJS += \
./Core/main.o \
./Core/system_init.o 

C_DEPS += \
./Core/main.d \
./Core/system_init.d 


# Each subdirectory must supply rules for building sources it contributes
Core/%.o Core/%.su Core/%.cyclo: ../Core/%.c Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407xx -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Application" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Core" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/Inc" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/Src" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/CMSIS/Core/Include" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core

clean-Core:
	-$(RM) ./Core/main.cyclo ./Core/main.d ./Core/main.o ./Core/main.su ./Core/system_init.cyclo ./Core/system_init.d ./Core/system_init.o ./Core/system_init.su

.PHONY: clean-Core

