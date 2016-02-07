################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
board.obj: C:/Share/starterwarefree-code/platform/evmAM335x/board.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="board.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

cpsw.obj: C:/Share/starterwarefree-code/platform/beaglebone/cpsw.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="cpsw.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

ctlregcontext.obj: C:/Share/starterwarefree-code/platform/beaglebone/ctlregcontext.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="ctlregcontext.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

dmtimer.obj: C:/Share/starterwarefree-code/platform/beaglebone/dmtimer.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="dmtimer.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

edma.obj: C:/Share/starterwarefree-code/platform/beaglebone/edma.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="edma.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

eeprom.obj: C:/Share/starterwarefree-code/platform/beaglebone/eeprom.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="eeprom.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gpio.obj: C:/Share/starterwarefree-code/platform/beaglebone/gpio.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="gpio.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hs_mmcsd.obj: C:/Share/starterwarefree-code/platform/beaglebone/hs_mmcsd.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="hs_mmcsd.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

hsi2c.obj: C:/Share/starterwarefree-code/platform/beaglebone/hsi2c.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="hsi2c.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

mcspi.obj: C:/Share/starterwarefree-code/platform/beaglebone/mcspi.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="mcspi.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sysdelay.obj: C:/Share/starterwarefree-code/platform/beaglebone/sysdelay.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="platform_lib_sysdelay.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

rtc.obj: C:/Share/starterwarefree-code/platform/beaglebone/rtc.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="rtc.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sysperf.obj: C:/Share/starterwarefree-code/platform/beaglebone/sysperf.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="sysperf.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

uart.obj: C:/Share/starterwarefree-code/platform/beaglebone/uart.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="uart.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

uartConsole.obj: C:/Share/starterwarefree-code/platform/beaglebone/uartConsole.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="uartConsole.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

usb.obj: C:/Share/starterwarefree-code/platform/beaglebone/usb.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="usb.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

watchdog.obj: C:/Share/starterwarefree-code/platform/beaglebone/watchdog.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="../../../../../../../include/armv7a/am335x" --include_path="../../../../../../../include/hw" --include_path="../../../../../../../include" -g --gcc --define=DMA_MODE --define=am3358 --display_error_number --diag_warning=225 --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="watchdog.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


