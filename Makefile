#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := SCDE

EXTRA_COMPONENT_DIRS += $(PROJECT_PATH)/components/Commands
EXTRA_COMPONENT_DIRS += $(PROJECT_PATH)/components/Modules

include $(IDF_PATH)/make/project.mk

