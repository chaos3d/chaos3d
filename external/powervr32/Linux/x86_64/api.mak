
ifneq (,$(filter OGLES,$(APIS)))
LIBDIR ?= "$(SDKDIR)/Builds/Linux/x86_64/Lib"
endif

ifneq (,$(filter OGLES2,$(APIS)))
LIBDIR ?= "$(SDKDIR)/Builds/Linux/x86_64/Lib"
endif

ifneq (,$(filter OGLES3,$(APIS)))
LIBDIR ?= "$(SDKDIR)/Builds/Linux/x86_64/Lib"
endif

ifneq (,$(filter OCL,$(APIS)))
LIBDIR ?= "$(SDKDIR)/Builds/Linux/x86_64/Lib"
endif

ifneq (,$(filter PVRSHELL,$(DEPENDS)))

ifneq (,$(filter OGL,$(APIS)))

ifeq "$(EGLBUILD)" "1"
else
SHELLAPI ?= GLX
X11BUILD = 1
endif

endif

SHELLAPI ?= KEGL

endif