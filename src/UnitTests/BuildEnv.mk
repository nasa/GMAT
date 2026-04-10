# $Id: BuildEnv.mk 10149 2012-01-11 20:32:30Z lindajun $
# Build environment file for Windows

# location of GMAT base and console for exception handling
#GMAT_BASE = ../../../src/base
#GMAT_CONSOLE = ../../../src/console
GMAT_BASE = c:/Projects/GmatDevelopment/src/base
GMAT_CONSOLE = c:/Projects/GmatDevelopment/src/console
GMAT_PLUGINS = c:/Projects/GmatDevelopment/plugins

# Flags used to control the build
CONSOLE_APP = 1
USE_SPICE = 0
USE_F2C_VERSION = 1

# MATLAB data
# location of MATLAB headers and libraries
# Removed MATLAB dependency

# SPICE data
# location of CSPICE headers and libraries
ifeq ($(USE_SPICE), 1)
SPICE_DIR = C:/projects/cspice
SPICE_INCLUDE = -I$(SPICE_DIR)/include
SPICE_CPP_FLAGS = -D__USE_SPICE__ $(SPICE_INCLUDE)
SPICE_LIB_DIR = $(SPICE_DIR)/lib
SPICE_LIBRARIES = $(SPICE_LIB_DIR)/cspice.a
else
SPICE_INCLUDE =
SPICE_CPP_FLAGS =
SPICE_LIB_DIR =
SPICE_LIBRARIES =
endif


#BASE_CPP_FLAGS = -DDEBUG_MEMORY $(SPICE_CPP_FLAGS)
BASE_CPP_FLAGS = $(SPICE_CPP_FLAGS)
BASE_LINK_FLAGS = $(SPICE_LIBRARIES)

# Compiler options
CPP = g++
C = gcc
FORTRAN = gfortran

ifeq ($(USE_F2C_VERSION), 1)
F2C_INCLUDE = -IC:/f2c/include
FORTRAN_LIB = C:/f2c/lib/libf2c.a
else
F2C_INCLUDE = 
FORTRAN_LIB = 
endif

ifeq ($(PROFILE_BUILD), 1)
PROFILE_FLAGS = -pg
else
PROFILE_FLAGS = 
endif


OPTIMIZATIONS =  -DwxUSE_UNIX=0 -D_X86_=1 -DWIN32 -DWINVER=0x0400 -D__WIN95__ \
                 -D__GNUWIN32__ -D__WIN32__ -mthreads -DSTRICT  -D__WXMSW__ \
                 -D__WINDOWS__ -fno-pcc-struct-return -O2\
                 -finline-functions -funroll-loops -fno-rtti -DNO_GCC_PRAGMA \
                 -malign-double -fexceptions \
                 -fexpensive-optimizations -march=pentium4
                 
#  -march=pentium4 -mfpmath=sse -fomit-frame-pointer -DNDEBUG

# Do not edit below this line -- here we build up longer compile/link strings
LINUX_MAC = 0

WX_CPP_FLAGS = $(WX_DEFINES) $(STC_CPP_FLAGS) $(EMAIL_CPP_FLAGS)
WX_LINK_FLAGS = $(STC_LIBRARIES) $(EMAIL_LIBRARIES)

GUI_CPP_FLAGS = `/usr/local/bin/wx-config --cppflags` $(WX_CPP_FLAGS) $(IL_CPP_FLAGS) 
GUI_LINK_FLAGS = `/usr/local/bin/wx-config --libs --gl-libs --static=no`


# Set options for debugging and profiling
ifeq ($(DEBUG_BUILD), 1)
DEBUG_FLAGS = -g
else
DEBUG_FLAGS = 
endif

# Build the complete list of flags for the compilers
CPP_BASE = $(OPTIMIZATIONS) $(CONSOLE_FLAGS) $(BASE_CPP_FLAGS) -Wall -ffriend-injection \
           $(PROFILE_FLAGS) $(DEBUG_FLAGS)

CPPFLAGS = $(CPP_BASE) $(PROFILE_FLAGS) $(DEBUG_FLAGS)
CFLAGS = -Wall

F77_FLAGS = $(CPPFLAGS)

LINK_FLAGS = $(BASE_LINK_FLAGS) $(FORTRAN_LIB) $(DEBUG_FLAGS)

CONSOLE_LINK_FLAGS = -L../base/lib $(FORTRAN_LIB) $(DEBUG_FLAGS) $(PROFILE_FLAGS)

