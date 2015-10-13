################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/array.cpp \
../src/array_type_specifier.cpp \
../src/assert.cpp \
../src/compound_type.cpp \
../src/compound_type_instance.cpp \
../src/compound_type_specifier.cpp \
../src/defaults.cpp \
../src/dimension.cpp \
../src/error.cpp \
../src/execution_context.cpp \
../src/indent.cpp \
../src/index.cpp \
../src/member_declaration.cpp \
../src/member_definition.cpp \
../src/member_instantiation.cpp \
../src/newt.cpp \
../src/parameter.cpp \
../src/parameterlist.cpp \
../src/primitive_type_specifier.cpp \
../src/symbol.cpp \
../src/symbol_context.cpp \
../src/symbol_table.cpp \
../src/type.cpp \
../src/type_specifier.cpp \
../src/type_table.cpp \
../src/utils.cpp 

OBJS += \
./src/array.o \
./src/array_type_specifier.o \
./src/assert.o \
./src/compound_type.o \
./src/compound_type_instance.o \
./src/compound_type_specifier.o \
./src/defaults.o \
./src/dimension.o \
./src/error.o \
./src/execution_context.o \
./src/indent.o \
./src/index.o \
./src/member_declaration.o \
./src/member_definition.o \
./src/member_instantiation.o \
./src/newt.o \
./src/parameter.o \
./src/parameterlist.o \
./src/primitive_type_specifier.o \
./src/symbol.o \
./src/symbol_context.o \
./src/symbol_table.o \
./src/type.o \
./src/type_specifier.o \
./src/type_table.o \
./src/utils.o 

CPP_DEPS += \
./src/array.d \
./src/array_type_specifier.d \
./src/assert.d \
./src/compound_type.d \
./src/compound_type_instance.d \
./src/compound_type_specifier.d \
./src/defaults.d \
./src/dimension.d \
./src/error.d \
./src/execution_context.d \
./src/indent.d \
./src/index.d \
./src/member_declaration.d \
./src/member_definition.d \
./src/member_instantiation.d \
./src/newt.d \
./src/parameter.d \
./src/parameterlist.d \
./src/primitive_type_specifier.d \
./src/symbol.d \
./src/symbol_context.d \
./src/symbol_table.d \
./src/type.d \
./src/type_specifier.d \
./src/type_table.d \
./src/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I./ -I../src -I../src/expressions -I../src/statements -I../src/variables -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


