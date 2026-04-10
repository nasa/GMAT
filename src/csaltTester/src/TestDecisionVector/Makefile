# Project:        Collocation
# Created by:     Wendy Shoan
# Date:           2015.06.30


# Define macros for locations
BASE_ROOT = ../..
TEST_ROOT = ..
BINDIR = $(TEST_ROOT)/bin
EXECUTABLE = $(BINDIR)/TestDecisionVector

TEST_FLAGS = -DUSE_64_BIT_LONGS -D__CONSOLE_APP__
## TEST_FLAGS = -DUSE_64_BIT_LONGS -D__DEBUG__ -D__CONSOLE_APP__

OPTIMIZATIONS = -O3 -funroll-loops

# Define macros for the needed includes
HEADERS =   -I. \
            -I$(BASE_ROOT)/src/collutils \
            -I$(BASE_ROOT)/src/util \
            -I$(BASE_ROOT)/GMAT_Utilities/include \
            -I$(BASE_ROOT)/GMAT_Utilities/util \
            -I$(BASE_ROOT)/GMAT_Utilities/console
            
DRIVER = TestDecisionVector.o
            
OBJECTS = \
    $(BASE_ROOT)/src/collutils/DecisionVector.o \
    $(BASE_ROOT)/src/collutils/DecVecTypeBetts.o \
    $(BASE_ROOT)/src/util/LowThrustException.o \
    $(BASE_ROOT)/GMAT_Utilities/console/ConsoleMessageReceiver.o \
    $(BASE_ROOT)/GMAT_Utilities/util/BaseException.o \
    $(BASE_ROOT)/GMAT_Utilities/util/FileManager.o \
    $(BASE_ROOT)/GMAT_Utilities/util/FileUtil.o \
    $(BASE_ROOT)/GMAT_Utilities/util/GmatGlobal.o \
    $(BASE_ROOT)/GMAT_Utilities/util/Linear.o \
    $(BASE_ROOT)/GMAT_Utilities/util/MessageInterface.o \
    $(BASE_ROOT)/GMAT_Utilities/util/MessageReceiver.o \
    $(BASE_ROOT)/GMAT_Utilities/util/RandomNumber.o \
    $(BASE_ROOT)/GMAT_Utilities/util/RealUtilities.o \
    $(BASE_ROOT)/GMAT_Utilities/util/Rmatrix.o \
    $(BASE_ROOT)/GMAT_Utilities/util/Rmatrix33.o \
    $(BASE_ROOT)/GMAT_Utilities/util/Rvector.o \
    $(BASE_ROOT)/GMAT_Utilities/util/Rvector3.o \
    $(BASE_ROOT)/GMAT_Utilities/util/StringTokenizer.o \
    $(BASE_ROOT)/GMAT_Utilities/util/StringUtil.o \
    $(BASE_ROOT)/GMAT_Utilities/util/TimeTypes.o
##     ./TestDecisionVector.o \
##    $(BASE_ROOT)/GMAT_Utilities/util/ArrayTemplate.o \
##    $(BASE_ROOT)/GMAT_Utilities/util/TableTemplate.o \

CPPFLAGS = $(HEADERS) $(OPTIMIZATIONS) $(TESTFLAGS) -Wall
CPP = g++

# Link specific flags
LINK_FLAGS =  /usr/lib/libstdc++.6.dylib \
               -headerpad_max_install_names \
               -lm -v 

# Object modules for library
$(DRIVER): %.o: %.cpp
	$(CPP) $(CPPFLAGS) $(HEADERS) -c -o $@ $<

$(OBJECTS): %.o: %.cpp %.hpp
	$(CPP) $(CPPFLAGS) $(HEADERS) -c -o $@ $<

#
# Default suffix rule
.SUFFIXES: .o .cpp


# Target definitions
#
all: $(EXECUTABLE)

$(EXECUTABLE): $(DRIVER) $(OBJECTS)
	@if [ ! -x $(BINDIR) ]; then mkdir $(BINDIR); fi 
	$(CPP) $(CPPFLAGS) $(LINK_FLAGS) $(DRIVER) $(OBJECTS) -o $@
	@chmod -R 755 $(BINDIR)

clean:
	rm -f $(DRIVER) $(OBJECTS) core

bare: clean
	rm -f $(EXECUTABLE) 
