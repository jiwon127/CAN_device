################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./tm4c123gh6pm_startup_ccs.obj" \
"./checked/capstone_application.obj" \
"./checked/test_1.obj" \
"./header/Can_init.obj" \
"./header/Delay.obj" \
"./header/FT81X_LCD.obj" \
"./header/Flash_memory.obj" \
"./header/Gpio_led.obj" \
"./header/Hibernation.obj" \
"./header/PMOS_LCD_Control.obj" \
"./header/SaveDistance.obj" \
"./header/Spi_lcd.obj" \
"./header/Sysctl_clk.obj" \
"./header/Uart.obj" \
"./header/Uart7.obj" \
"./header/WakePinData.obj" \
"./header/asm.obj" \
"./header/eve2_display_lib.obj" \
"./header/eve2_lib.obj" \
"./header/my_eve2_lib.obj" \
"./header/timer.obj" \
"../tm4c123gh6pm.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include checked/subdir_vars.mk
-include header/subdir_vars.mk
-include subdir_rules.mk
-include checked/subdir_rules.mk
-include header/subdir_rules.mk
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
	"C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --opt_for_speed=1 --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi -z -m"220128CAN.map" --heap_size=0 --stack_size=1024 -i"C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/lib" -i"C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/include" --reread_libs --diag_wrap=off --display_error_number --warn_sections --xml_link_info="220128CAN_linkInfo.xml" --rom_model -o "220128CAN.out" $(ORDERED_OBJS)
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
	-$(RM) "tm4c123gh6pm_startup_ccs.obj" "checked\capstone_application.obj" "checked\test_1.obj" "header\Can_init.obj" "header\Delay.obj" "header\FT81X_LCD.obj" "header\Flash_memory.obj" "header\Gpio_led.obj" "header\Hibernation.obj" "header\PMOS_LCD_Control.obj" "header\SaveDistance.obj" "header\Spi_lcd.obj" "header\Sysctl_clk.obj" "header\Uart.obj" "header\Uart7.obj" "header\WakePinData.obj" "header\asm.obj" "header\eve2_display_lib.obj" "header\eve2_lib.obj" "header\my_eve2_lib.obj" "header\timer.obj" 
	-$(RM) "tm4c123gh6pm_startup_ccs.d" "checked\capstone_application.d" "checked\test_1.d" "header\Can_init.d" "header\Delay.d" "header\FT81X_LCD.d" "header\Flash_memory.d" "header\Gpio_led.d" "header\Hibernation.d" "header\PMOS_LCD_Control.d" "header\SaveDistance.d" "header\Spi_lcd.d" "header\Sysctl_clk.d" "header\Uart.d" "header\Uart7.d" "header\WakePinData.d" "header\asm.d" "header\eve2_display_lib.d" "header\eve2_lib.d" "header\my_eve2_lib.d" "header\timer.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

