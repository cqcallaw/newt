################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/array.cpp \
../src/assert.cpp \
../src/defaults.cpp \
../src/driver.cpp \
../src/error.cpp \
../src/execution_context.cpp \
../src/execution_context_list.cpp \
../src/function.cpp \
../src/indent.cpp \
../src/member_instantiation.cpp \
../src/newt.cpp \
../src/record.cpp \
../src/sum.cpp \
../src/symbol.cpp \
../src/symbol_context.cpp \
../src/symbol_table.cpp \
../src/type.cpp \
../src/type_table.cpp \
../src/unit.cpp \
../src/utils.cpp 

OBJS += \
./src/array.o \
./src/assert.o \
./src/defaults.o \
./src/driver.o \
./src/error.o \
./src/execution_context.o \
./src/execution_context_list.o \
./src/function.o \
./src/indent.o \
./src/member_instantiation.o \
./src/newt.o \
./src/record.o \
./src/sum.o \
./src/symbol.o \
./src/symbol_context.o \
./src/symbol_table.o \
./src/type.o \
./src/type_table.o \
./src/unit.o \
./src/utils.o 

CPP_DEPS += \
./src/array.d \
./src/assert.d \
./src/defaults.d \
./src/driver.d \
./src/error.d \
./src/execution_context.d \
./src/execution_context_list.d \
./src/function.d \
./src/indent.d \
./src/member_instantiation.d \
./src/newt.d \
./src/record.d \
./src/sum.d \
./src/symbol.d \
./src/symbol_context.d \
./src/symbol_table.d \
./src/type.d \
./src/type_table.d \
./src/unit.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x -I./ -I../src -I../src/expressions -I../src/types -I../src/types/specifiers -I../src/statements -I../src/statements/declarations -I../src/variables -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


