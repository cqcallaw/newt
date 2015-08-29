################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/array_symbol.cpp \
../src/assert.cpp \
../src/defaults.cpp \
../src/error.cpp \
../src/execution_context.cpp \
../src/indent.cpp \
../src/member_declaration.cpp \
../src/newt.cpp \
../src/parameter.cpp \
../src/parameterlist.cpp \
../src/symbol.cpp \
../src/symbol_table.cpp \
../src/type.cpp \
../src/type_table.cpp \
../src/utils.cpp 

OBJS += \
./src/array_symbol.o \
./src/assert.o \
./src/defaults.o \
./src/error.o \
./src/execution_context.o \
./src/indent.o \
./src/member_declaration.o \
./src/newt.o \
./src/parameter.o \
./src/parameterlist.o \
./src/symbol.o \
./src/symbol_table.o \
./src/type.o \
./src/type_table.o \
./src/utils.o 

CPP_DEPS += \
./src/array_symbol.d \
./src/assert.d \
./src/defaults.d \
./src/error.d \
./src/execution_context.d \
./src/indent.d \
./src/member_declaration.d \
./src/newt.d \
./src/parameter.d \
./src/parameterlist.d \
./src/symbol.d \
./src/symbol_table.d \
./src/type.d \
./src/type_table.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I./ -I../src -I../src/expressions -I../src/statements -I../src/variables -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


