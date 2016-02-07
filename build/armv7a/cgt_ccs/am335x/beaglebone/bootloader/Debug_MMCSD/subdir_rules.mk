################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
bl_init.obj: C:/Share/starterwarefree-code/bootloader/src/armv7a/cgt/bl_init.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" --include_path="C:/Share/starterwarefree-code/mmcsdlib/include" --include_path="C:/Share/starterwarefree-code/bootloader/include" --include_path="C:/Share/starterwarefree-code/bootloader/include/armv7a/am335x" -g --gcc --define=am335x --define=beaglebone --define=MMCSD --define=am3358 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="bl_init.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

bl_main.obj: C:/Share/starterwarefree-code/bootloader/src/bl_main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" --include_path="C:/Share/starterwarefree-code/mmcsdlib/include" --include_path="C:/Share/starterwarefree-code/bootloader/include" --include_path="C:/Share/starterwarefree-code/bootloader/include/armv7a/am335x" -g --gcc --define=am335x --define=beaglebone --define=MMCSD --define=am3358 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="bl_main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

bl_platform.obj: C:/Share/starterwarefree-code/bootloader/src/armv7a/am335x/bl_platform.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" --include_path="C:/Share/starterwarefree-code/mmcsdlib/include" --include_path="C:/Share/starterwarefree-code/bootloader/include" --include_path="C:/Share/starterwarefree-code/bootloader/include/armv7a/am335x" -g --gcc --define=am335x --define=beaglebone --define=MMCSD --define=am3358 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="bl_platform.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

device.obj: C:/Share/starterwarefree-code/system_config/armv7a/am335x/device.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" --include_path="C:/Share/starterwarefree-code/mmcsdlib/include" --include_path="C:/Share/starterwarefree-code/bootloader/include" --include_path="C:/Share/starterwarefree-code/bootloader/include/armv7a/am335x" -g --gcc --define=am335x --define=beaglebone --define=MMCSD --define=am3358 --display_error_number --diag_warning=225 --diag_wrap=off --preproc_with_compile --preproc_dependency="device.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


