################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv8/tools/compiler/ti-cgt-arm_5.2.6

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./i2c_if.obj" \
"./main.obj" \
"./network_common.obj" \
"./pinmux.obj" \
"./startup_ccs.obj" \
"./tmp006drv.obj" \
"./uart_if.obj" \
"../cc3200v1p32.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \
-l"C:/ti/CC3200SDK_1.3.0/cc3200-sdk/simplelink/ccs/NON_OS/simplelink.a" \
-l"C:/ti/CC3200SDK_1.3.0/cc3200-sdk/oslib/ccs/free_rtos/free_rtos.a" \
-l"C:/ti/CC3200SDK_1.3.0/cc3200-sdk/driverlib/ccs/Release/driverlib.a" \
-lwebclient.a \
-ljson.a \
-ldriverlib.a \
-lsimplelink.a \
-lfree_rtos.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
freertos_demo.out \

EXE_OUTPUTS__QUOTED += \
"freertos_demo.out" \

BIN_OUTPUTS += \
freertos_demo.bin \

BIN_OUTPUTS__QUOTED += \
"freertos_demo.bin" \


# All Target
all: freertos_demo.out

# Tool invocations
freertos_demo.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_5.2.6/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib --abi=eabi -me -g --no_intrinsics --gcc --define=__SL__ --define=ccs --define=USE_FREERTOS --define=SL_PLATFORM_MULTI_THREADED --define=cc3200 --diag_warning=225 --diag_wrap=off --display_error_number -z -m"freertos_demo.map" --stack_size=0x100 --heap_size=0x4000 -i"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_5.2.6/lib" -i"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_5.2.6/include" -i"C:/ti/CC3200SDK_1.3.0/cc3200-sdk/netapps/http/client/ccs/HTTPClientMinLib" -i"C:/ti/CC3200SDK_1.3.0/cc3200-sdk/netapps/json/ccs/Release/" -i"C:/ti/CC3200SDK_1.3.0/cc3200-sdk/driverlib/ccs/Release" -i"C:/ti/CC3200SDK_1.3.0/cc3200-sdk/simplelink/ccs/OS" -i"C:/ti/CC3200SDK_1.3.0/cc3200-sdk/oslib/ccs/free_rtos" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="freertos_demo_linkInfo.xml" --rom_model -o "freertos_demo.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

freertos_demo.bin: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_5.2.6/bin/armhex"  -o "freertos_demo.bin" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '
	@$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "i2c_if.obj" "main.obj" "network_common.obj" "pinmux.obj" "startup_ccs.obj" "tmp006drv.obj" "uart_if.obj" 
	-$(RM) "i2c_if.d" "main.d" "network_common.d" "pinmux.d" "startup_ccs.d" "tmp006drv.d" "uart_if.d" 
	-@echo 'Finished clean'
	-@echo ' '

post-build:
	-"C:/ti/ccsv8/utils/tiobj2bin/tiobj2bin" "freertos_demo.out" "freertos_demo.bin" "C:/ti/ccsv8/tools/compiler/ti-cgt-arm_5.2.6/bin/armofd" "C:/ti/ccsv8/tools/compiler/ti-cgt-arm_5.2.6/bin/armhex" "C:/ti/ccsv8/utils/tiobj2bin/mkhex4bin"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

