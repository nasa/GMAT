################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BrachistochroneDriver.cpp \
../src/TestOptCtrl.cpp 

OBJS += \
./src/BrachistochroneDriver.o \
./src/TestOptCtrl.o 

CPP_DEPS += \
./src/BrachistochroneDriver.d \
./src/TestOptCtrl.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/home/crc/lowthrusttsi/CPlusPlus/GMAT_Utilities/include -I/home/crc/lowthrusttsi/CPlusPlus/test -I/home/crc/boost_1_62_0 -I/home/crc/snopt7 -I/home/crc/lowthrusttsi/CPlusPlus/test/HelperClasses -I/home/crc/lowthrusttsi/CPlusPlus/src/userfunutils -I/home/crc/lowthrusttsi/CPlusPlus/src/collutils -I/home/crc/lowthrusttsi/CPlusPlus/src/executive -I/home/crc/lowthrusttsi/CPlusPlus/src/util -I/home/crc/lowthrusttsi/CPlusPlus/GMAT_Utilities/util -I/home/crc/lowthrusttsi/CPlusPlus/GMAT_Utilities/console -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


