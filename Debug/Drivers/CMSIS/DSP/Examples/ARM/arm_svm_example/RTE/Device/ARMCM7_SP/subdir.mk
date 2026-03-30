################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/startup_ARMCM7.c \
../Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/system_ARMCM7.c 

OBJS += \
./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/startup_ARMCM7.o \
./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/system_ARMCM7.o 

C_DEPS += \
./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/startup_ARMCM7.d \
./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/system_ARMCM7.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/%.o Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/%.su Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/%.cyclo: ../Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/%.c Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407xx -DSTM32 -DSTM32F407G_DISC1 -DSTM32F4 -DSTM32F407VGTx -c -I../Inc -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Application" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Core" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/Inc" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/Src" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/CMSIS/Core/Include" -I"C:/Users/Prince/Desktop/EmbeddedSystemsM4CortexMCU/MCUADVANCED/AECN/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS-2f-DSP-2f-Examples-2f-ARM-2f-arm_svm_example-2f-RTE-2f-Device-2f-ARMCM7_SP

clean-Drivers-2f-CMSIS-2f-DSP-2f-Examples-2f-ARM-2f-arm_svm_example-2f-RTE-2f-Device-2f-ARMCM7_SP:
	-$(RM) ./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/startup_ARMCM7.cyclo ./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/startup_ARMCM7.d ./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/startup_ARMCM7.o ./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/startup_ARMCM7.su ./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/system_ARMCM7.cyclo ./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/system_ARMCM7.d ./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/system_ARMCM7.o ./Drivers/CMSIS/DSP/Examples/ARM/arm_svm_example/RTE/Device/ARMCM7_SP/system_ARMCM7.su

.PHONY: clean-Drivers-2f-CMSIS-2f-DSP-2f-Examples-2f-ARM-2f-arm_svm_example-2f-RTE-2f-Device-2f-ARMCM7_SP

