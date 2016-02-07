################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
syl_mmcsd_proto.obj: C:/Share/starterwarefree-code/mmcsdlib/syl_mmcsd_proto.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../include/armv7a/am335x" --include_path="../../../../../include/armv7a" --include_path="../../../../../mmcsdlib/include" --include_path="../../../../../include" --include_path="../../../../../include/hw" -g --gcc --define=am3358 --diag_warning=225 --preproc_with_compile --preproc_dependency="syl_mmcsd_proto.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


