################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/statements/declarations/array_declaration_statement.cpp \
../src/statements/declarations/complex_instantiation_statement.cpp \
../src/statements/declarations/declaration_statement.cpp \
../src/statements/declarations/function_declaration_statement.cpp \
../src/statements/declarations/inferred_declaration_statement.cpp \
../src/statements/declarations/maybe_declaration_statement.cpp \
../src/statements/declarations/nested_declaration_statement.cpp \
../src/statements/declarations/primitive_declaration_statement.cpp \
../src/statements/declarations/record_declaration_statement.cpp \
../src/statements/declarations/sum_declaration_statement.cpp \
../src/statements/declarations/type_alias_declaration_statement.cpp \
../src/statements/declarations/unit_declaration_statement.cpp 

OBJS += \
./src/statements/declarations/array_declaration_statement.o \
./src/statements/declarations/complex_instantiation_statement.o \
./src/statements/declarations/declaration_statement.o \
./src/statements/declarations/function_declaration_statement.o \
./src/statements/declarations/inferred_declaration_statement.o \
./src/statements/declarations/maybe_declaration_statement.o \
./src/statements/declarations/nested_declaration_statement.o \
./src/statements/declarations/primitive_declaration_statement.o \
./src/statements/declarations/record_declaration_statement.o \
./src/statements/declarations/sum_declaration_statement.o \
./src/statements/declarations/type_alias_declaration_statement.o \
./src/statements/declarations/unit_declaration_statement.o 

CPP_DEPS += \
./src/statements/declarations/array_declaration_statement.d \
./src/statements/declarations/complex_instantiation_statement.d \
./src/statements/declarations/declaration_statement.d \
./src/statements/declarations/function_declaration_statement.d \
./src/statements/declarations/inferred_declaration_statement.d \
./src/statements/declarations/maybe_declaration_statement.d \
./src/statements/declarations/nested_declaration_statement.d \
./src/statements/declarations/primitive_declaration_statement.d \
./src/statements/declarations/record_declaration_statement.d \
./src/statements/declarations/sum_declaration_statement.d \
./src/statements/declarations/type_alias_declaration_statement.d \
./src/statements/declarations/unit_declaration_statement.d 


# Each subdirectory must supply rules for building sources it contributes
src/statements/declarations/%.o: ../src/statements/declarations/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x -DLANGUAGE_VERSION="`git describe --long --tags`" -I./ -I../src -I../src/expressions -I../src/types -I../src/types/specifiers -I../src/statements -I../src/statements/declarations -I../src/variables -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


