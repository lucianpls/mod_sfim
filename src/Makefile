# Makefile.lcl, which should define
# DEST, the folder where the module gets copied
# look at Makefile.lcl.example

MODULE = mod_sfim
TARGET = .libs/$(MODULE).so
C_SRC = $(MODULE).c

MAKEOPT ?= Makefile.lcl
include $(MAKEOPT)

default:	$(TARGET)

$(TARGET):	$(C_SRC)
	$(APXS) -c $<

install	:	$(TARGET)
	$(SUDO) $(CP) $(TARGET) $(DEST)

clean	:
	$(RM) -rf .libs *.o *.lo *.slo *.la
