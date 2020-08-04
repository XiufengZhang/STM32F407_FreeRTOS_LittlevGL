C_SOURCES += lfs.c
C_SOURCES += lfs_util.c

DEPPATH += --dep-path $(LFS_DIR)/littlefs
# Makefile文件中的特殊变量,作用在在依赖文件上,如果当前目录找不到就到VPATH下去找 
VPATH += :$(LFS_DIR)/littlefs

C_INCLUDES += -I$(LFS_DIR)/littlefs
