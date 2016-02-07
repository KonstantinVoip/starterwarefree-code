################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
uartEcho.obj: C:/Share/starterwarefree-code/examples/beaglebone/uart/uartEcho.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include/armv7a/am335x" -g --gcc --display_error_number --diag_warning=225 --preproc_with_compile --preproc_dependency="uartEcho.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


