################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/specifiers/array_type_specifier.cpp \
../src/specifiers/compound_type_specifier.cpp \
../src/specifiers/function_type_specifier.cpp \
../src/specifiers/primitive_type_specifier.cpp \
../src/specifiers/type_specifier.cpp 

OBJS += \
./src/specifiers/array_type_specifier.o \
./src/specifiers/compound_type_specifier.o \
./src/specifiers/function_type_specifier.o \
./src/specifiers/primitive_type_specifier.o \
./src/specifiers/type_specifier.o 

CPP_DEPS += \
./src/specifiers/array_type_specifier.d \
./src/specifiers/compound_type_specifier.d \
./src/specifiers/function_type_specifier.d \
./src/specifiers/primitive_type_specifier.d \
./src/specifiers/type_specifier.d 


# Each subdirectory must supply rules for building sources it contributes
src/specifiers/%.o: ../src/specifiers/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I./ -I../src -I../src/expressions -I../src/specifiers -I../src/statements -I../src/variables -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


