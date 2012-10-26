################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Board.cpp \
../src/BoardTest.cpp \
../src/Dialog.cpp \
../src/OthelloBoard.cpp \
../src/OthelloDlg.cpp \
../src/OthelloMove.cpp \
../src/OthelloView.cpp 

OBJS += \
./src/Board.o \
./src/BoardTest.o \
./src/Dialog.o \
./src/OthelloBoard.o \
./src/OthelloDlg.o \
./src/OthelloMove.o \
./src/OthelloView.o 

CPP_DEPS += \
./src/Board.d \
./src/BoardTest.d \
./src/Dialog.d \
./src/OthelloBoard.d \
./src/OthelloDlg.d \
./src/OthelloMove.d \
./src/OthelloView.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


