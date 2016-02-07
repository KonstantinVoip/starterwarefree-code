################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
mcspiFlash.obj: C:/Share/starterwarefree-code/examples/beaglebone/mcspi/mcspiFlash.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" --include_path="C:/Share/starterwarefree-code/third_party/neonMathLib/inc" -g --define=__ARM_NEON__ --diag_warning=225 --neon --preproc_with_compile --preproc_dependency="mcspiFlash.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


