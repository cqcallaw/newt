################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/symbols/array_symbol.cpp \
../src/symbols/symbol.cpp 

OBJS += \
./src/symbols/array_symbol.o \
./src/symbols/symbol.o 

CPP_DEPS += \
./src/symbols/array_symbol.d \
./src/symbols/symbol.d 


# Each subdirectory must supply rules for building sources it contributes
src/symbols/%.o: ../src/symbols/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I./ -I../src -I../src/expressions -I../src/statements -I../src/symbols -I../src/variables -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


