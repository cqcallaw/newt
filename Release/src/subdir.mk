################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/array.cpp \
../src/assert.cpp \
../src/builtins.cpp \
../src/defaults.cpp \
../src/driver.cpp \
../src/error.cpp \
../src/execution_context.cpp \
../src/execution_context_list.cpp \
../src/function.cpp \
../src/function_variant.cpp \
../src/indent.cpp \
../src/match.cpp \
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
../src/utils.cpp \
../src/version.cpp 

OBJS += \
./src/array.o \
./src/assert.o \
./src/builtins.o \
./src/defaults.o \
./src/driver.o \
./src/error.o \
./src/execution_context.o \
./src/execution_context_list.o \
./src/function.o \
./src/function_variant.o \
./src/indent.o \
./src/match.o \
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
./src/utils.o \
./src/version.o 

CPP_DEPS += \
./src/array.d \
./src/assert.d \
./src/builtins.d \
./src/defaults.d \
./src/driver.d \
./src/error.d \
./src/execution_context.d \
./src/execution_context_list.d \
./src/function.d \
./src/function_variant.d \
./src/indent.d \
./src/match.d \
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
./src/utils.d \
./src/version.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x -DLANGUAGE_VERSION="`git describe --long --tags`" -I./ -I../src -I../src/expressions -I../src/types -I../src/types/specifiers -I../src/statements -I../src/statements/declarations -I../src/variables -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


