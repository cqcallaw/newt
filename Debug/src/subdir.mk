CPP_SRCS += \
../src/error.cpp \
../src/defaults.cpp \
../src/assert.cpp \
../src/type.cpp \
../src/indent.cpp \
../src/newt.cpp \
../src/parameter.cpp \
../src/parameterlist.cpp \
../src/symbol.cpp \
../src/symbol_table.cpp \
../src/utils.cpp \
../src/variable.cpp

OBJS += \
./src/error.o \
./src/assert.o \
./src/type.o \
./src/indent.o \
./src/newt.o \
./src/parameter.o \
./src/parameterlist.o \
./src/symbol.o \
./src/symbol_table.o \
./src/utils.o \
./src/variable.o 

CPP_DEPS += \
./src/error.d \
./src/gpl_assert.d \
./src/type.d \
./src/indent.d \
./src/newt.d \
./src/parameter.d \
./src/parameterlist.d \
./src/symbol.d \
./src/symbol_table.d \
./src/utils.d \
./src/variable.d

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"../src" -I"../src/expressions" -I"../src/statements" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


