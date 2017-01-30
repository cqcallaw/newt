################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/types/alias_definition.cpp \
../src/types/array_type.cpp \
../src/types/complex_type.cpp \
../src/types/function_type.cpp \
../src/types/maybe_type.cpp \
../src/types/primitive_type.cpp \
../src/types/record_type.cpp \
../src/types/sum_type.cpp \
../src/types/unit_type.cpp \
../src/types/variant_function_type.cpp 

OBJS += \
./src/types/alias_definition.o \
./src/types/array_type.o \
./src/types/complex_type.o \
./src/types/function_type.o \
./src/types/maybe_type.o \
./src/types/primitive_type.o \
./src/types/record_type.o \
./src/types/sum_type.o \
./src/types/unit_type.o \
./src/types/variant_function_type.o 

CPP_DEPS += \
./src/types/alias_definition.d \
./src/types/array_type.d \
./src/types/complex_type.d \
./src/types/function_type.d \
./src/types/maybe_type.d \
./src/types/primitive_type.d \
./src/types/record_type.d \
./src/types/sum_type.d \
./src/types/unit_type.d \
./src/types/variant_function_type.d 


# Each subdirectory must supply rules for building sources it contributes
src/types/%.o: ../src/types/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x -I./ -I../src -I../src/expressions -I../src/types -I../src/types/specifiers -I../src/statements -I../src/statements/declarations -I../src/variables -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


