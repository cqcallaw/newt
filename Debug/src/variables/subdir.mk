################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/variables/array_variable.cpp \
../src/variables/basic_variable.cpp \
../src/variables/member_variable.cpp \
../src/variables/variable.cpp 

OBJS += \
./src/variables/array_variable.o \
./src/variables/basic_variable.o \
./src/variables/member_variable.o \
./src/variables/variable.o 

CPP_DEPS += \
./src/variables/array_variable.d \
./src/variables/basic_variable.d \
./src/variables/member_variable.d \
./src/variables/variable.d 


# Each subdirectory must supply rules for building sources it contributes
src/variables/%.o: ../src/variables/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/Debug/ -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/expressions -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/types -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/types/specifiers -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/statements -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/statements/declarations -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/variables -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


