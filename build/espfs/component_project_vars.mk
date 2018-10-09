# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/espfs/include $(IDF_PATH)/components/espfs/lib/heatshrink
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/espfs -lespfs
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += espfs
component-espfs-build: 
