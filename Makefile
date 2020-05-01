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
TARGET = Optical_GUI_STM32F407ZG
VERSION = _V1.0.0
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
app/main.c \
app/stm32f4xx_it.c \
system/system_stm32f4xx.c \
drivers/src/clk.c \
drivers/src/gpio.c \
drivers/src/fttouch.c \
drivers/src/gttouch.c \
drivers/src/tsctouch.c \
drivers/src/iwdg.c \
drivers/src/spi.c \
drivers/src/spiflash.c \
drivers/src/ssdlcd.c \
drivers/src/timer.c \
drivers/src/uart.c \
lfs/lfs.c \
lfs/lfs_util.c \
lvgl/lv_core/lv_group.c \
lvgl/lv_core/lv_indev.c \
lvgl/lv_core/lv_lang.c \
lvgl/lv_core/lv_obj.c \
lvgl/lv_core/lv_refr.c \
lvgl/lv_core/lv_style.c \
lvgl/lv_core/lv_vdb.c \
lvgl/lv_draw/lv_draw.c \
lvgl/lv_draw/lv_draw_arc.c \
lvgl/lv_draw/lv_draw_img.c \
lvgl/lv_draw/lv_draw_label.c \
lvgl/lv_draw/lv_draw_line.c \
lvgl/lv_draw/lv_draw_rbasic.c \
lvgl/lv_draw/lv_draw_rect.c \
lvgl/lv_draw/lv_draw_triangle.c \
lvgl/lv_draw/lv_draw_vbasic.c \
lvgl/lv_fonts/lv_font_builtin.c \
lvgl/lv_fonts/lv_font_dejavu_10.c \
lvgl/lv_fonts/lv_font_dejavu_10_cyrillic.c \
lvgl/lv_fonts/lv_font_dejavu_10_latin_sup.c \
lvgl/lv_fonts/lv_font_dejavu_20.c \
lvgl/lv_fonts/lv_font_dejavu_20_cyrillic.c \
lvgl/lv_fonts/lv_font_dejavu_20_latin_sup.c \
lvgl/lv_fonts/lv_font_dejavu_30.c \
lvgl/lv_fonts/lv_font_dejavu_30_cyrillic.c \
lvgl/lv_fonts/lv_font_dejavu_30_latin_sup.c \
lvgl/lv_fonts/lv_font_dejavu_40.c \
lvgl/lv_fonts/lv_font_dejavu_40_cyrillic.c \
lvgl/lv_fonts/lv_font_dejavu_40_latin_sup.c \
lvgl/lv_fonts/lv_font_monospace_8.c \
lvgl/lv_fonts/lv_font_symbol_10.c \
lvgl/lv_fonts/lv_font_symbol_20.c \
lvgl/lv_fonts/lv_font_symbol_30.c \
lvgl/lv_fonts/lv_font_symbol_40.c \
lvgl/lv_hal/lv_hal_disp.c \
lvgl/lv_hal/lv_hal_indev.c \
lvgl/lv_hal/lv_hal_tick.c \
lvgl/lv_misc/lv_anim.c \
lvgl/lv_misc/lv_area.c \
lvgl/lv_misc/lv_circ.c \
lvgl/lv_misc/lv_color.c \
lvgl/lv_misc/lv_font.c \
lvgl/lv_misc/lv_fs.c \
lvgl/lv_misc/lv_gc.c \
lvgl/lv_misc/lv_ll.c \
lvgl/lv_misc/lv_log.c \
lvgl/lv_misc/lv_math.c \
lvgl/lv_misc/lv_mem.c \
lvgl/lv_misc/lv_task.c \
lvgl/lv_misc/lv_templ.c \
lvgl/lv_misc/lv_txt.c \
lvgl/lv_misc/lv_ufs.c \
lvgl/lv_objx/lv_arc.c \
lvgl/lv_objx/lv_bar.c \
lvgl/lv_objx/lv_btn.c \
lvgl/lv_objx/lv_btnm.c \
lvgl/lv_objx/lv_calendar.c \
lvgl/lv_objx/lv_canvas.c \
lvgl/lv_objx/lv_cb.c \
lvgl/lv_objx/lv_chart.c \
lvgl/lv_objx/lv_cont.c \
lvgl/lv_objx/lv_ddlist.c \
lvgl/lv_objx/lv_gauge.c \
lvgl/lv_objx/lv_img.c \
lvgl/lv_objx/lv_imgbtn.c \
lvgl/lv_objx/lv_kb.c \
lvgl/lv_objx/lv_label.c \
lvgl/lv_objx/lv_led.c \
lvgl/lv_objx/lv_line.c \
lvgl/lv_objx/lv_list.c \
lvgl/lv_objx/lv_lmeter.c \
lvgl/lv_objx/lv_mbox.c \
lvgl/lv_objx/lv_objx_templ.c \
lvgl/lv_objx/lv_page.c \
lvgl/lv_objx/lv_preload.c \
lvgl/lv_objx/lv_roller.c \
lvgl/lv_objx/lv_slider.c \
lvgl/lv_objx/lv_spinbox.c \
lvgl/lv_objx/lv_sw.c \
lvgl/lv_objx/lv_ta.c \
lvgl/lv_objx/lv_table.c \
lvgl/lv_objx/lv_tabview.c \
lvgl/lv_objx/lv_tileview.c \
lvgl/lv_objx/lv_win.c \
lvgl/lv_porting/lv_port_disp.c \
lvgl/lv_porting/lv_port_indev.c \
lvgl/lv_themes/lv_theme.c \
lvgl/lv_themes/lv_theme_alien.c \
lvgl/lv_themes/lv_theme_default.c \
lvgl/lv_themes/lv_theme_material.c \
lvgl/lv_themes/lv_theme_mono.c \
lvgl/lv_themes/lv_theme_nemo.c \
lvgl/lv_themes/lv_theme_night.c \
lvgl/lv_themes/lv_theme_templ.c \
lvgl/lv_themes/lv_theme_zen.c \
libraries/STM32F4xx_StdPeriph_Driver/src/misc.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cec.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_aes.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_des.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_tdes.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dcmi.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dfsdm.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dsi.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash_ramfunc.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmpi2c.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_md5.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_sha1.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_lptim.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_qspi.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sai.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spdifrx.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c \
libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c

# ASM sources
ASM_SOURCES =  \
system/gcc_startup/startup_stm32f40xx.s


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
-D__VFP_FP__ \
-D__FPU_PRESENT=1 \
-DGCC_ARM_CM4F\
-DLV_CONF_INCLUDE_SIMPLE

# AS includes
AS_INCLUDES = \
-Iapp \
-Isystem \
-Isystem/include

# C includes
C_INCLUDES =  \
-I. \
-Iapp \
-Idrivers/inc \
-Ilfs \
-Ilvgl \
-Ilvgl/lv_core \
-Ilvgl/lv_draw \
-Ilvgl/lv_fonts \
-Ilvgl/lv_hal \
-Ilvgl/lv_misc \
-Ilvgl/lv_objx \
-Ilvgl/lv_porting \
-Ilvgl/lv_themes \
-Ilibraries/STM32F4xx_StdPeriph_Driver/inc \
-Isystem \
-Isystem/include


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
LDSCRIPT = system/gcc_link/STM32F407ZGTx_FLASH_ExtSRAM.ld

# libraries
LIBS = -lc -lm -lnosys
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
