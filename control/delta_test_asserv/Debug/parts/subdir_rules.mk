################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
parts/control.obj: ../parts/control.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/StupidPrograms/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/StupidPrograms/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="C:/StupidPrograms/ti/StellarisWare" --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="parts/control.pp" --obj_directory="parts" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

parts/pccontrol.obj: ../parts/pccontrol.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/StupidPrograms/ti/ccsv5/tools/compiler/arm_5.0.1/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --include_path="C:/StupidPrograms/ti/ccsv5/tools/compiler/arm_5.0.1/include" --include_path="C:/StupidPrograms/ti/StellarisWare" --diag_warning=225 --display_error_number --preproc_with_compile --preproc_dependency="parts/pccontrol.pp" --obj_directory="parts" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


