################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
wdtReset.obj: C:/Share/starterwarefree-code/examples/beaglebone/watchdogTimer/wdtReset.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" --include_path="C:/Share/starterwarefree-code/mmcsdlib/include" --include_path="C:/Share/sota_skd/arm_skd_syl/Inc/boot_image" --include_path="C:/Share/sota_skd/arm_skd_syl/Inc/sota_tab" -g --gcc --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="wdtReset.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


