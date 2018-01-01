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
	$(CXX) -std=c++0x -DLANGUAGE_VERSION="\"`git describe --long --tags`\"" -I./ -I../src -I../src/expressions -I../src/types -I../src/types/specifiers -I../src/statements -I../src/statements/declarations -I../src/variables -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


