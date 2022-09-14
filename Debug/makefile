################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./capstone_application.obj" \
"./tm4c123gh6pm_startup_ccs.obj" \
"./ex_/test_1.obj" \
"./source/Can_init.obj" \
"./source/Delay.obj" \
"./source/FT81X_LCD.obj" \
"./source/Flash_memory.obj" \
"./source/FlashtoRAMtest.obj" \
"./source/Gpio_led.obj" \
"./source/Hibernation.obj" \
"./source/PMOS_LCD_Control.obj" \
"./source/SaveDistance.obj" \
"./source/Spi_lcd.obj" \
"./source/Sysctl_clk.obj" \
"./source/Uart.obj" \
"./source/Uart7.obj" \
"./source/WakePinData.obj" \
"./source/asm.obj" \
"./source/eve2_display_lib.obj" \
"./source/eve2_lib.obj" \
"./source/my_eve2_lib.obj" \
"./source/timer.obj" \
"../tm4c123gh6pm.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include ex_/subdir_vars.mk
-include source/subdir_vars.mk
-include subdir_rules.mk
-include ex_/subdir_rules.mk
-include source/subdir_rules.mk
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
220128CAN.out \

EXE_OUTPUTS__QUOTED += \
"220128CAN.out" \

BIN_OUTPUTS += \
220128CAN.hex \

BIN_OUTPUTS__QUOTED += \
"220128CAN.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "220128CAN.out"

# Tool invocations
220128CAN.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi -z -m"220128CAN.map" --heap_size=0 --stack_size=1024 -i"C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/lib" -i"C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="220128CAN_linkInfo.xml" --rom_model -o "220128CAN.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

220128CAN.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: Arm Hex Utility'
	"C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/bin/armhex" --diag_wrap=off -o "220128CAN.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "capstone_application.obj" "tm4c123gh6pm_startup_ccs.obj" "ex_\test_1.obj" "source\Can_init.obj" "source\Delay.obj" "source\FT81X_LCD.obj" "source\Flash_memory.obj" "source\FlashtoRAMtest.obj" "source\Gpio_led.obj" "source\Hibernation.obj" "source\PMOS_LCD_Control.obj" "source\SaveDistance.obj" "source\Spi_lcd.obj" "source\Sysctl_clk.obj" "source\Uart.obj" "source\Uart7.obj" "source\WakePinData.obj" "source\asm.obj" "source\eve2_display_lib.obj" "source\eve2_lib.obj" "source\my_eve2_lib.obj" "source\timer.obj" 
	-$(RM) "capstone_application.d" "tm4c123gh6pm_startup_ccs.d" "ex_\test_1.d" "source\Can_init.d" "source\Delay.d" "source\FT81X_LCD.d" "source\Flash_memory.d" "source\FlashtoRAMtest.d" "source\Gpio_led.d" "source\Hibernation.d" "source\PMOS_LCD_Control.d" "source\SaveDistance.d" "source\Spi_lcd.d" "source\Sysctl_clk.d" "source\Uart.d" "source\Uart7.d" "source\WakePinData.d" "source\asm.d" "source\eve2_display_lib.d" "source\eve2_lib.d" "source\my_eve2_lib.d" "source\timer.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

