################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/cr_startup_lpc175x_6x.c \
../src/crp.c \
../src/grzegorzw_test.c \
../src/lpc17xx_adc.c \
../src/lpc17xx_clkpwr.c \
../src/lpc17xx_gpio.c \
../src/lpc17xx_libcfg_default.c \
../src/lpc17xx_nvic.c \
../src/lpc17xx_pinsel.c \
../src/lpc17xx_spi.c \
../src/lpc17xx_uart.c 

OBJS += \
./src/cr_startup_lpc175x_6x.o \
./src/crp.o \
./src/grzegorzw_test.o \
./src/lpc17xx_adc.o \
./src/lpc17xx_clkpwr.o \
./src/lpc17xx_gpio.o \
./src/lpc17xx_libcfg_default.o \
./src/lpc17xx_nvic.o \
./src/lpc17xx_pinsel.o \
./src/lpc17xx_spi.o \
./src/lpc17xx_uart.o 

C_DEPS += \
./src/cr_startup_lpc175x_6x.d \
./src/crp.d \
./src/grzegorzw_test.d \
./src/lpc17xx_adc.d \
./src/lpc17xx_clkpwr.d \
./src/lpc17xx_gpio.d \
./src/lpc17xx_libcfg_default.d \
./src/lpc17xx_nvic.d \
./src/lpc17xx_pinsel.d \
./src/lpc17xx_spi.d \
./src/lpc17xx_uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSIS_CORE_LPC17xx -D__LPC17XX__ -I"C:\nxp\workspace\CMSIS_CORE_LPC17xx\inc" -Og -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


