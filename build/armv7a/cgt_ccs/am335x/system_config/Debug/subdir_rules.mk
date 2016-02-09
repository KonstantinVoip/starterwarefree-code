################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
cache.obj: C:/Share/starterwarefree-code/system_config/armv7a/cache.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" -g --gcc --define=am3358 --diag_warning=225 --neon --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="cache.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

clock.obj: C:/Share/starterwarefree-code/system_config/armv7a/am335x/clock.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" -g --gcc --define=am3358 --diag_warning=225 --neon --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="clock.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

cp15.obj: C:/Share/starterwarefree-code/system_config/armv7a/cgt/cp15.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" -g --gcc --define=am3358 --diag_warning=225 --neon --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="cp15.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

cpu.obj: C:/Share/starterwarefree-code/system_config/armv7a/cgt/cpu.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" -g --gcc --define=am3358 --diag_warning=225 --neon --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="cpu.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

device.obj: C:/Share/starterwarefree-code/system_config/armv7a/am335x/device.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" -g --gcc --define=am3358 --diag_warning=225 --neon --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="device.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

exceptionhandler.obj: C:/Share/starterwarefree-code/system_config/armv7a/am335x/cgt/exceptionhandler.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" -g --gcc --define=am3358 --diag_warning=225 --neon --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="exceptionhandler.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

init.obj: C:/Share/starterwarefree-code/system_config/armv7a/cgt/init.asm $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" -g --gcc --define=am3358 --diag_warning=225 --neon --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="init.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

interrupt.obj: C:/Share/starterwarefree-code/system_config/armv7a/am335x/interrupt.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" -g --gcc --define=am3358 --diag_warning=225 --neon --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="interrupt.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

mmu.obj: C:/Share/starterwarefree-code/system_config/armv7a/mmu.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" -g --gcc --define=am3358 --diag_warning=225 --neon --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="mmu.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

startup.obj: C:/Share/starterwarefree-code/system_config/armv7a/am335x/startup.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -Ooff --opt_for_speed=3 --include_path="C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="C:/Share/starterwarefree-code/include" --include_path="C:/Share/starterwarefree-code/include/hw" --include_path="C:/Share/starterwarefree-code/include/armv7a" --include_path="C:/Share/starterwarefree-code/include/armv7a/am335x" -g --gcc --define=am3358 --diag_warning=225 --neon --unaligned_access=on --enum_type=packed --preproc_with_compile --preproc_dependency="startup.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


