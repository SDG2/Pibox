################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/drivers/RC522/scr/RC522.c 

OBJS += \
./src/drivers/RC522/scr/RC522.o 

C_DEPS += \
./src/drivers/RC522/scr/RC522.d 


# Each subdirectory must supply rules for building sources it contributes
src/drivers/RC522/scr/%.o: ../src/drivers/RC522/scr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


