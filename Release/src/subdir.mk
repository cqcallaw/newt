################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/array.cpp \
../src/assert.cpp \
../src/compound_type.cpp \
../src/compound_type_instance.cpp \
../src/defaults.cpp \
../src/dimension.cpp \
../src/error.cpp \
../src/execution_context.cpp \
../src/function.cpp \
../src/indent.cpp \
../src/index.cpp \
../src/member_declaration.cpp \
../src/member_definition.cpp \
../src/member_instantiation.cpp \
../src/member_instantiation_list.cpp \
../src/newt.cpp \
../src/parameter.cpp \
../src/symbol.cpp \
../src/symbol_context.cpp \
../src/symbol_table.cpp \
../src/type.cpp \
../src/type_table.cpp \
../src/utils.cpp 

OBJS += \
./src/array.o \
./src/assert.o \
./src/compound_type.o \
./src/compound_type_instance.o \
./src/defaults.o \
./src/dimension.o \
./src/error.o \
./src/execution_context.o \
./src/function.o \
./src/indent.o \
./src/index.o \
./src/member_declaration.o \
./src/member_definition.o \
./src/member_instantiation.o \
./src/member_instantiation_list.o \
./src/newt.o \
./src/parameter.o \
./src/symbol.o \
./src/symbol_context.o \
./src/symbol_table.o \
./src/type.o \
./src/type_table.o \
./src/utils.o 

CPP_DEPS += \
./src/array.d \
./src/assert.d \
./src/compound_type.d \
./src/compound_type_instance.d \
./src/defaults.d \
./src/dimension.d \
./src/error.d \
./src/execution_context.d \
./src/function.d \
./src/indent.d \
./src/index.d \
./src/member_declaration.d \
./src/member_definition.d \
./src/member_instantiation.d \
./src/member_instantiation_list.d \
./src/newt.d \
./src/parameter.d \
./src/symbol.d \
./src/symbol_context.d \
./src/symbol_table.d \
./src/type.d \
./src/type_table.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I./ -I../src -I../src/expressions -I../src/specifiers -I../src/statements -I../src/variables -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


