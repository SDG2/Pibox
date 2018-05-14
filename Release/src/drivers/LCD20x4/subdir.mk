################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/drivers/LCD20x4/LCD.c \
../src/drivers/LCD20x4/menu_lcd.c 

OBJS += \
./src/drivers/LCD20x4/LCD.o \
./src/drivers/LCD20x4/menu_lcd.o 

C_DEPS += \
./src/drivers/LCD20x4/LCD.d \
./src/drivers/LCD20x4/menu_lcd.d 


# Each subdirectory must supply rules for building sources it contributes
src/drivers/LCD20x4/%.o: ../src/drivers/LCD20x4/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


