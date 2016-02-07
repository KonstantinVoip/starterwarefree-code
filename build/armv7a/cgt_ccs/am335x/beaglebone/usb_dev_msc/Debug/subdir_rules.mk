################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
ramdisk.obj: C:/Share/starterwarefree-code/examples/beaglebone/usb_dev_msc/ramdisk.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../examples/beaglebone/usb_dev_msc" --include_path="../../../../../../../usblib/include" -g --gcc --define=am335x --define=am335x_15x15 --define=DMA_MODE --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="ramdisk.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

usb_dev_msc.obj: C:/Share/starterwarefree-code/examples/beaglebone/usb_dev_msc/usb_dev_msc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../examples/beaglebone/usb_dev_msc" --include_path="../../../../../../../usblib/include" -g --gcc --define=am335x --define=am335x_15x15 --define=DMA_MODE --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="usb_dev_msc.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

usb_msc_structs.obj: C:/Share/starterwarefree-code/examples/beaglebone/usb_dev_msc/usb_msc_structs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../examples/beaglebone/usb_dev_msc" --include_path="../../../../../../../usblib/include" -g --gcc --define=am335x --define=am335x_15x15 --define=DMA_MODE --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="usb_msc_structs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

usbdmscglue.obj: C:/Share/starterwarefree-code/examples/beaglebone/usb_dev_msc/usbdmscglue.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/armv7a" --include_path="../../../../../../../include" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../examples/beaglebone/usb_dev_msc" --include_path="../../../../../../../usblib/include" -g --gcc --define=am335x --define=am335x_15x15 --define=DMA_MODE --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="usbdmscglue.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


