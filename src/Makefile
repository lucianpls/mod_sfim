# Create the ../Makefile.lcl, which should define
# MOD_PATH, the folder where the apache module should reside

include Makefile.lcl

default:	mod_sfim.c mod_sfim.h
	apxs -c $<
install :       ./.libs/mod_sfim.so
	sudo    cp ./.libs/mod_sfim.so    $(MOD_PATH)

