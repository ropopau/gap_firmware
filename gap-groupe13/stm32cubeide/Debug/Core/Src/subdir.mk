################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/gapcli_cmd_callback.c \
../Core/Src/gapcom_fsm.c \
../Core/Src/gapcom_impl.c \
../Core/Src/log_manager.c \
../Core/Src/main.c \
../Core/Src/mpu60x0.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/gapcli_cmd_callback.o \
./Core/Src/gapcom_fsm.o \
./Core/Src/gapcom_impl.o \
./Core/Src/log_manager.o \
./Core/Src/main.o \
./Core/Src/mpu60x0.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/gapcli_cmd_callback.d \
./Core/Src/gapcom_fsm.d \
./Core/Src/gapcom_impl.d \
./Core/Src/log_manager.d \
./Core/Src/main.d \
./Core/Src/mpu60x0.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu17 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/parking/Documents/stm32_workspaces/project/libgapcom/Inc" -I"/home/parking/Documents/stm32_workspaces/project/libgapcom/Inc/gapcom" -I"/home/parking/Documents/stm32_workspaces/project/libgapcom/Inc/nanopb" -I"/home/parking/Documents/stm32_workspaces/project/libgapcom/Inc/TinyFrame" -O0 -ffunction-sections -fdata-sections -Wall -std=gnu17 -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/gapcli_cmd_callback.cyclo ./Core/Src/gapcli_cmd_callback.d ./Core/Src/gapcli_cmd_callback.o ./Core/Src/gapcli_cmd_callback.su ./Core/Src/gapcom_fsm.cyclo ./Core/Src/gapcom_fsm.d ./Core/Src/gapcom_fsm.o ./Core/Src/gapcom_fsm.su ./Core/Src/gapcom_impl.cyclo ./Core/Src/gapcom_impl.d ./Core/Src/gapcom_impl.o ./Core/Src/gapcom_impl.su ./Core/Src/log_manager.cyclo ./Core/Src/log_manager.d ./Core/Src/log_manager.o ./Core/Src/log_manager.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mpu60x0.cyclo ./Core/Src/mpu60x0.d ./Core/Src/mpu60x0.o ./Core/Src/mpu60x0.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

