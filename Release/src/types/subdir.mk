################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/types/complex_type.cpp \
../src/types/primitive_type.cpp \
../src/types/record_type.cpp \
../src/types/sum_type.cpp \
../src/types/type_definition.cpp 

OBJS += \
./src/types/complex_type.o \
./src/types/primitive_type.o \
./src/types/record_type.o \
./src/types/sum_type.o \
./src/types/type_definition.o 

CPP_DEPS += \
./src/types/complex_type.d \
./src/types/primitive_type.d \
./src/types/record_type.d \
./src/types/sum_type.d \
./src/types/type_definition.d 


# Each subdirectory must supply rules for building sources it contributes
src/types/%.o: ../src/types/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I./ -I../src -I../src/expressions -I../src/types -I../src/types/specifiers -I../src/statements -I../src/statements/declarations -I../src/variables -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


