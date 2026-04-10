# Define macros for the needed includes
BASE_ROOT = ../..
EXE_PATH = .



HEADERS =   -I. \
            -I$(BASE_ROOT)/GMAT_Utilities/include \
            -I$(BASE_ROOT)/GMAT_Utilities/util \
            -I$(BASE_ROOT)/GMAT_Utilities/util/interpolator
            
OBJECTS = \
    $(BASE_ROOT)/GMAT_Utilities/util/BaseException.o \
    $(BASE_ROOT)/GMAT_Utilities/util/MessageInterface.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/Interpolator.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/LinearInterpolator.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/NotAKnotInterpolator.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/CubicSplineInterpolator.o \
    $(EXE_PATH)/TestInterpMain.o


CPPFLAGS = $(HEADERS) -Wall -std=c++11 -g
CPP = g++

# Link specific flags
LINK_FLAGS =  -lm

# Target definitions
#
all: TestInterpMain

clean:
	rm *.o

# Object modules for library
TestInterpMain: $(OBJECTS)
	$(CPP) $(LINK_FLAGS) $(OBJECTS) -o TestInterpolator

$(OBJECTS): %.o: %.cpp %.hpp
	$(CPP) $(CPPFLAGS) $(HEADERS) -c -o $@ $<
