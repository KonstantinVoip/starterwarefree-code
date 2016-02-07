################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
crc16.obj: C:/Share/starterwarefree-code/third_party/xmodem/crc16.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../mmcsdlib/include" --include_path="../../../../../../../third_party/fatfs/src" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include/armv7a/am335x" -g --gcc --define=am335x --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="crc16.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

fat_mmcsd.obj: C:/Share/starterwarefree-code/third_party/fatfs/port/fat_mmcsd.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../mmcsdlib/include" --include_path="../../../../../../../third_party/fatfs/src" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include/armv7a/am335x" -g --gcc --define=am335x --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="fat_mmcsd.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

ff.obj: C:/Share/starterwarefree-code/third_party/fatfs/src/ff.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../mmcsdlib/include" --include_path="../../../../../../../third_party/fatfs/src" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include/armv7a/am335x" -g --gcc --define=am335x --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="ff.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hs_mmcsd_fs.obj: C:/Share/starterwarefree-code/examples/beaglebone/hs_mmcsd/hs_mmcsd_fs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../mmcsdlib/include" --include_path="../../../../../../../third_party/fatfs/src" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include/armv7a/am335x" -g --gcc --define=am335x --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="hs_mmcsd_fs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hs_mmcsd_rw.obj: C:/Share/starterwarefree-code/examples/beaglebone/hs_mmcsd/hs_mmcsd_rw.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../mmcsdlib/include" --include_path="../../../../../../../third_party/fatfs/src" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include/armv7a/am335x" -g --gcc --define=am335x --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="hs_mmcsd_rw.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

xmodem.obj: C:/Share/starterwarefree-code/third_party/xmodem/xmodem.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../mmcsdlib/include" --include_path="../../../../../../../third_party/fatfs/src" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include/armv7a/am335x" -g --gcc --define=am335x --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="xmodem.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


