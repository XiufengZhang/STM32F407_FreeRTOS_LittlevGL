KERNEL_DIR = $(RTOS_DIR)/FreeRTOS-Kernel
include $(RTOS_DIR)/rtos_kernel.mk

C_INCLUDES += -I$(RTOS_DIR)