################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
dmtimerCounter.obj: C:/Share/starterwarefree-code/examples/beaglebone/dmtimer/dmtimerCounter.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.2/include" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include/armv7a/am335x" -g --gcc --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="dmtimerCounter.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


