################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
neonVFPBenchmarkApp.obj: C:/Share/starterwarefree-code/examples/beaglebone/neonVFPBenchmark/neonVFPBenchmarkApp.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=vfplib --abi=eabi -me -O2 --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/armv7a" -g --gcc --display_error_number --diag_warning=225 --diag_wrap=off --neon --preproc_with_compile --preproc_dependency="neonVFPBenchmarkApp.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


