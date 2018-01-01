################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/types/specifiers/array_type_specifier.cpp \
../src/types/specifiers/complex_type_specifier.cpp \
../src/types/specifiers/function_declaration.cpp \
../src/types/specifiers/function_type_specifier.cpp \
../src/types/specifiers/maybe_type_specifier.cpp \
../src/types/specifiers/nested_type_specifier.cpp \
../src/types/specifiers/primitive_type_specifier.cpp \
../src/types/specifiers/record_type_specifier.cpp \
../src/types/specifiers/sum_type_specifier.cpp \
../src/types/specifiers/variant_function_specifier.cpp 

OBJS += \
./src/types/specifiers/array_type_specifier.o \
./src/types/specifiers/complex_type_specifier.o \
./src/types/specifiers/function_declaration.o \
./src/types/specifiers/function_type_specifier.o \
./src/types/specifiers/maybe_type_specifier.o \
./src/types/specifiers/nested_type_specifier.o \
./src/types/specifiers/primitive_type_specifier.o \
./src/types/specifiers/record_type_specifier.o \
./src/types/specifiers/sum_type_specifier.o \
./src/types/specifiers/variant_function_specifier.o 

CPP_DEPS += \
./src/types/specifiers/array_type_specifier.d \
./src/types/specifiers/complex_type_specifier.d \
./src/types/specifiers/function_declaration.d \
./src/types/specifiers/function_type_specifier.d \
./src/types/specifiers/maybe_type_specifier.d \
./src/types/specifiers/nested_type_specifier.d \
./src/types/specifiers/primitive_type_specifier.d \
./src/types/specifiers/record_type_specifier.d \
./src/types/specifiers/sum_type_specifier.d \
./src/types/specifiers/variant_function_specifier.d 


# Each subdirectory must supply rules for building sources it contributes
src/types/specifiers/%.o: ../src/types/specifiers/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	$(CXX) -std=c++0x -DLANGUAGE_VERSION="`git describe --long --tags`" -I./ -I../src -I../src/expressions -I../src/types -I../src/types/specifiers -I../src/statements -I../src/statements/declarations -I../src/variables -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


