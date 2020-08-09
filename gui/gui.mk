LVGL_DIR = $(GUI_DIR)
LVGL_DIR_NAME = lvgl
CSRCS ?=
include $(LVGL_DIR)/$(LVGL_DIR_NAME)/lvgl.mk

# 加入course
C_SOURCES += $(CSRCS)
# include 在src下.mk文件已经添加到 CFLAGS
# C_INCLUDES += CFLAGS

C_INCLUDES += -I$(LVGL_DIR)
C_INCLUDES += -I$(LVGL_DIR)/$(LVGL_DIR_NAME)
C_INCLUDES += -I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src