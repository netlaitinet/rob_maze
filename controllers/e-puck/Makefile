C_SOURCES = e-puck.c hardware.c runtime.c history.c compass.c orient.c moves.c strafe.c tools.c

CFLAGS += -Wall -Wextra -Werror

null :=
space := $(null) $(null)
WEBOTS_HOME_PATH?=$(subst $(space),\ ,$(strip $(subst \,/,$(WEBOTS_HOME))))
include $(WEBOTS_HOME_PATH)/resources/Makefile.include
