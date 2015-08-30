################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/statements/array_declaration_statement.cpp \
../src/statements/assignment_statement.cpp \
../src/statements/declaration_statement.cpp \
../src/statements/exit_statement.cpp \
../src/statements/for_statement.cpp \
../src/statements/if_statement.cpp \
../src/statements/print_statement.cpp \
../src/statements/statement.cpp \
../src/statements/statement_block.cpp \
../src/statements/struct_declaration_statement.cpp \
../src/statements/struct_instantiation_statement.cpp 

OBJS += \
./src/statements/array_declaration_statement.o \
./src/statements/assignment_statement.o \
./src/statements/declaration_statement.o \
./src/statements/exit_statement.o \
./src/statements/for_statement.o \
./src/statements/if_statement.o \
./src/statements/print_statement.o \
./src/statements/statement.o \
./src/statements/statement_block.o \
./src/statements/struct_declaration_statement.o \
./src/statements/struct_instantiation_statement.o 

CPP_DEPS += \
./src/statements/array_declaration_statement.d \
./src/statements/assignment_statement.d \
./src/statements/declaration_statement.d \
./src/statements/exit_statement.d \
./src/statements/for_statement.d \
./src/statements/if_statement.d \
./src/statements/print_statement.d \
./src/statements/statement.d \
./src/statements/statement_block.d \
./src/statements/struct_declaration_statement.d \
./src/statements/struct_instantiation_statement.d 


# Each subdirectory must supply rules for building sources it contributes
src/statements/%.o: ../src/statements/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I./ -I../src -I../src/expressions -I../src/statements -I../src/symbols -I../src/variables -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


