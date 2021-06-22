# Create the Makefile.lcl, which should define
# DEST, the folder where the apache module should reside
# look at Makefile.lcl.example

MODULE = mod_sfim
TARGET = .libs/$(MODULE).so
C_SRC = $(MODULE).c

include Makefile.lcl

default:	$(TARGET)

$(TARGET):	$(C_SRC)
	apxs -c $<

install	:	$(TARGET)
	$(SUDO) $(CP) $(TARGET) $(DEST)

clean	:
	$(RM) -rf .libs *.o *.lo *.slo *.la

