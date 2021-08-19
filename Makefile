##############################################################################
##                                  Syndi™                                  ##
##                                                                          ##
##                         Copyright © 2021 Aquefir                         ##
##                 Released under Artisan Software Licence.              ##
##############################################################################

ifeq ($(strip $(AQ)),)
$(error "AQ was not found in your environment. You need to install the Slick Makefiles from github.com/aquefir/slick to continue.")
endif

include $(AQ)/lib/slick/base.mk

# name of project. used in output binary naming
PROJECT := syndi

# put a ‘1’ for the desired target types to compile
EXEFILE := 1
SOFILE  :=
AFILE   :=

# space-separated path list for #includes
# <system> includes
INCLUDES := $(TROOT)/include/glib-2.0 $(TROOT)/lib/glib-2.0/include
# "local" includes
INCLUDEL := src

# space-separated library name list
LIBS    := glib-2.0
LIBDIRS :=

# ‘3P’ are in-tree 3rd-party dependencies
# 3PLIBDIR is the base directory
# 3PLIBS is the folder names in the base directory for each library
3PLIBDIR :=
3PLIBS   :=

# frameworks (macOS target only)
FWORKS :=

# sources
SFILES    :=
CFILES    :=
CPPFILES  :=
PUBHFILES :=
PRVHFILES := \
	src/common.h \
	src/ircmsg.h \
	src/nwsock.h

# test suite sources
TES_CFILES    :=
TES_CPPFILES  :=
TES_PUBHFILES :=
TES_PRVHFILES :=

# this defines all our usual targets
include $(AQ)/lib/slick/targets.mk