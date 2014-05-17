################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dvrp_algorithm.c \
../src/vrp_parse.c 

CU_SRCS += \
../src/dvrp_hehe.cu 

CU_DEPS += \
./src/dvrp_hehe.d 

OBJS += \
./src/dvrp_algorithm.o \
./src/dvrp_hehe.o \
./src/vrp_parse.o 

C_DEPS += \
./src/dvrp_algorithm.d \
./src/vrp_parse.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/opt/cuda/bin/nvcc -G -g -O0 -gencode arch=compute_20,code=sm_20 -gencode arch=compute_20,code=sm_21  -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/opt/cuda/bin/nvcc -G -g -O0 --compile  -x c -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.cu
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/opt/cuda/bin/nvcc -G -g -O0 -gencode arch=compute_20,code=sm_20 -gencode arch=compute_20,code=sm_21  -odir "src" -M -o "$(@:%.o=%.d)" "$<"
	/opt/cuda/bin/nvcc --compile -G -O0 -g -gencode arch=compute_20,code=compute_20 -gencode arch=compute_20,code=sm_21  -x cu -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


