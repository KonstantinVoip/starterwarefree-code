################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
nand_gpmc.obj: C:/Share/starterwarefree-code/nandlib/nand_gpmc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../include" --include_path="../../../../../include/hw" --include_path="../../../../../nandlib/include" -g --gcc --define=am3358 --diag_warning=225 --preproc_with_compile --preproc_dependency="nand_gpmc.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

nandlib.obj: C:/Share/starterwarefree-code/nandlib/nandlib.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../include" --include_path="../../../../../include/hw" --include_path="../../../../../nandlib/include" -g --gcc --define=am3358 --diag_warning=225 --preproc_with_compile --preproc_dependency="nandlib.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


