################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/startup_ARMCM55.c \
../Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/system_ARMCM55.c 

OBJS += \
./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/startup_ARMCM55.o \
./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/system_ARMCM55.o 

C_DEPS += \
./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/startup_ARMCM55.d \
./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/system_ARMCM55.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/%.o Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/%.su Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/%.cyclo: ../Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/%.c Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407xx -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Application" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Core" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/Inc" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/Src" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/CMSIS/Core/Include" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS-2f-DSP-2f-Examples-2f-ARM-2f-arm_convolution_example-2f-RTE-2f-Device-2f-ARMCM55

clean-Drivers-2f-CMSIS-2f-DSP-2f-Examples-2f-ARM-2f-arm_convolution_example-2f-RTE-2f-Device-2f-ARMCM55:
	-$(RM) ./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/startup_ARMCM55.cyclo ./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/startup_ARMCM55.d ./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/startup_ARMCM55.o ./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/startup_ARMCM55.su ./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/system_ARMCM55.cyclo ./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/system_ARMCM55.d ./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/system_ARMCM55.o ./Drivers/CMSIS/DSP/Examples/ARM/arm_convolution_example/RTE/Device/ARMCM55/system_ARMCM55.su

.PHONY: clean-Drivers-2f-CMSIS-2f-DSP-2f-Examples-2f-ARM-2f-arm_convolution_example-2f-RTE-2f-Device-2f-ARMCM55

