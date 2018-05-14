################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/InterruptSM.c \
../src/dbcontroller.c \
../src/fsm.c \
../src/fsm_rfid.c \
../src/mutex.c \
../src/piMusicBox_2.c \
../src/player.c \
../src/tone.c 

OBJS += \
./src/InterruptSM.o \
./src/dbcontroller.o \
./src/fsm.o \
./src/fsm_rfid.o \
./src/mutex.o \
./src/piMusicBox_2.o \
./src/player.o \
./src/tone.o 

C_DEPS += \
./src/InterruptSM.d \
./src/dbcontroller.d \
./src/fsm.d \
./src/fsm_rfid.d \
./src/mutex.d \
./src/piMusicBox_2.d \
./src/player.d \
./src/tone.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


