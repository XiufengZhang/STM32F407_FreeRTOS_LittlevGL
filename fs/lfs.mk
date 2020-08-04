LITTLEFS_DIR = littlefs

C_SOURCES += lfs.c
C_SOURCES += lfs_util.c

DEPPATH += --dep-path $(FS_DIR)/$(LITTLEFS_DIR)
# Makefile文件中的特殊变量,作用在在依赖文件上,如果当前目录找不到就到VPATH下去找 
VPATH += :$(FS_DIR)/$(LITTLEFS_DIR)

C_INCLUDES += -I$(FS_DIR)/$(LITTLEFS_DIR)
