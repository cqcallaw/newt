################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/statements/assignment_statement.cpp \
../src/statements/complex_instantiation_statement.cpp \
../src/statements/exit_statement.cpp \
../src/statements/for_statement.cpp \
../src/statements/if_statement.cpp \
../src/statements/invoke_statement.cpp \
../src/statements/print_statement.cpp \
../src/statements/return_statement.cpp \
../src/statements/statement_block.cpp 

OBJS += \
./src/statements/assignment_statement.o \
./src/statements/complex_instantiation_statement.o \
./src/statements/exit_statement.o \
./src/statements/for_statement.o \
./src/statements/if_statement.o \
./src/statements/invoke_statement.o \
./src/statements/print_statement.o \
./src/statements/return_statement.o \
./src/statements/statement_block.o 

CPP_DEPS += \
./src/statements/assignment_statement.d \
./src/statements/complex_instantiation_statement.d \
./src/statements/exit_statement.d \
./src/statements/for_statement.d \
./src/statements/if_statement.d \
./src/statements/invoke_statement.d \
./src/statements/print_statement.d \
./src/statements/return_statement.d \
./src/statements/statement_block.d 


# Each subdirectory must supply rules for building sources it contributes
src/statements/%.o: ../src/statements/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/Debug/ -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/expressions -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/types -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/types/specifiers -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/statements -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/statements/declarations -I/home/caleb/Shared/Education/Chico/CSCI499H/newt/src/variables -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


