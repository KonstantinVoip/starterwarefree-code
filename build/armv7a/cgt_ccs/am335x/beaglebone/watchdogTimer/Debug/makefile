################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/arm_5.1.6

ORDERED_OBJS += \
"./wdtReset.obj" \
"../wdtReset.cmd" \
$(GEN_CMDS__FLAG) \
-l"C:\Share\starterwarefree-code\binary\armv7a\cgt_ccs\mmcsdlib\Debug\libmmcsd.lib" \
-l"C:\Share\starterwarefree-code\binary\armv7a\cgt_ccs\am335x\drivers\Debug\drivers.lib" \
-l"C:\Share\starterwarefree-code\binary\armv7a\cgt_ccs\am335x\system_config\Debug\system.lib" \
-l"C:\Share\starterwarefree-code\binary\armv7a\cgt_ccs\am335x\beaglebone\platform\Debug\platform.lib" \
-l"C:\Share\starterwarefree-code\binary\armv7a\cgt_ccs\utils\Debug\utils.lib" \
-l"rtsv7A8_A_le_n_v3_eabi.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset.out \

EXE_OUTPUTS__QUOTED += \
"..\..\..\..\..\..\..\binary\armv7a\cgt_ccs\am335x\beaglebone\watchdogTimer\Debug\wdtReset.out" \

BIN_OUTPUTS += \
../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset.bin \

BIN_OUTPUTS__QUOTED += \
"..\..\..\..\..\..\..\binary\armv7a\cgt_ccs\am335x\beaglebone\watchdogTimer\Debug\wdtReset.bin" \


# All Target
all: ../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset.out

# Tool invocations
../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7A8 --code_state=32 --float_support=VFPv3 --abi=eabi -me -g --gcc --display_error_number --diag_warning=225 -z -m"wdtReset.map" -i"C:/ti/ccsv6/tools/compiler/arm_5.1.6/lib" -i"C:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --reread_libs --warn_sections --xml_link_info="../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset_linkInfo.xml" --ram_model -o "../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset.bin: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armhex"  -o "../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset.bin" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "wdtReset.pp" 
	-$(RM) "wdtReset.obj" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"C:/ti/ccsv6/utils/tiobj2bin/tiobj2bin.bat"  "../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset.out"  "../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset.bin"  "C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armofd.exe"  "C:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armhex.exe"  "C:/ti/ccsv6/utils/tiobj2bin/mkhex4bin.exe" & "../../../../../../../tools/ti_image/tiimage.exe" "0x80000000" "NONE" "../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset.bin" "../../../../../../../binary/armv7a/cgt_ccs/am335x/beaglebone/watchdogTimer/Debug/wdtReset_ti.bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets

