# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = Light_GUI_STM32F407ZG
VERSION = _V1.3.0
# Linux	获取系统时间
COMPILE_TIME = $(shell date +"_%y%m%d%H%M%S")


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
ifeq ($(DEBUG), 1)
OPT = -Og
else
OPT = -O1
# 文件名变量追加赋值不能用+= 要用:=方式 也不能用=的方式，否则变量名中含有空格
TARGET := $(TARGET)$(VERSION)$(COMPILE_TIME)
endif


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
APP/main.c \
APP/stm32f4xx_it.c \
System/system_stm32f4xx.c \
Drivers/src/clk.c \
Drivers/src/gpio.c \
Drivers/src/fttouch.c \
Drivers/src/gttouch.c \
Drivers/src/tsctouch.c \
Drivers/src/iwdg.c \
Drivers/src/spi.c \
Drivers/src/spiflash.c \
Drivers/src/ssdlcd.c \
Drivers/src/timer.c \
Drivers/src/uart.c \
emWin_Config/GUI_X.c \
emWin_Config/GUIConf.c \
emWin_Config/LCDConf_stm3240g_eval.c \
emWin_GUI/GUI_Display.c \
emWin_GUI/Fonts/SongType24.c \
emWin_GUI/Fonts/SongType32.c \
emWin_GUI/Images/Home_Logo_100x77.c \
emWin_GUI/Images/Home_Logo_200x154.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/misc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cec.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_aes.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_des.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_tdes.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dcmi.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dfsdm.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dsi.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash_ramfunc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmpi2c.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_md5.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_sha1.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_lptim.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_qspi.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sai.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spdifrx.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c \
Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c

# ASM sources
ASM_SOURCES =  \
System/gcc_startup/startup_stm32f40xx.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DSTM32F40_41xxx \
-DUSE_STDPERIPH_DRIVER \
-DHSE_VALUE=24000000 \
-DARM_MATH_CM4 \
-DARM_MATH_MATRIX_CHECK \
-D__VFP_FP__

# AS includes
AS_INCLUDES = \
-IAPP \
-ISystem \
-ISystem/Include

# C includes
C_INCLUDES =  \
-I. \
-IAPP \
-IDrivers/inc \
-IemWin_Config \
-IemWin_GUI \
-IemWin_Lib/inc \
-ILibraries/STM32F4xx_StdPeriph_Driver/inc \
-ISystem \
-ISystem/Include


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = System/gcc_link/STM32F407ZGTx_FLASH_ExtSRAM.ld

# libraries
LIBS = -lc -lm -lnosys \
emWin_Lib/Lib/STemWin_CM4_wc32_ot.a
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
