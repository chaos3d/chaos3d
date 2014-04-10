
ifneq (,$(filter OGLES,$(APIS)))
LIBDIR ?= "$(SDKDIR)/Builds/Linux/armv7/Lib"
endif

ifneq (,$(filter OGLES2,$(APIS)))
LIBDIR ?= "$(SDKDIR)/Builds/Linux/armv7/Lib"
endif

ifneq (,$(filter OGLES3,$(APIS)))
LIBDIR ?= "$(SDKDIR)/Builds/Linux/armv7/Lib"
endif

ifneq (,$(filter OCL,$(APIS)))
LIBDIR ?= "$(SDKDIR)/Builds/Linux/armv7/Lib"
endif

ifneq (,$(filter PVRSHELL,$(DEPENDS)))
SHELLAPI ?= KEGL
endif