# Makefile for the TestSolarFluxReader unit test (GitHub issue #4 / GMT-8612).
#
# GMAT's src/UnitTests tree predates the current CMake build, so this test links
# against the CMake-built shared libraries in application/bin rather than the
# obsolete per-component static libs referenced by the older test Makefiles.
#
# Prerequisite: build the base libraries first, from the GMAT root, e.g.
#     cmake -S . -B _bld -DCMAKE_BUILD_TYPE=Release
#     cmake --build _bld --target GmatBase -j
#
# Then, in this directory:
#     make            # build TestSolarFluxReader
#     make run        # build and run against the bundled SpaceWeather file
#     make clean

GMAT_ROOT := ../../..
BINDIR    := $(GMAT_ROOT)/application/bin

CXX      ?= c++
CXXFLAGS ?= -std=c++17 -g -O2

# Include every base and gmatutil source directory, plus the common test
# harness and the CSPICE headers pulled in transitively by the base headers.
INCDIRS := $(wildcard $(GMAT_ROOT)/src/base/*/) \
           $(wildcard $(GMAT_ROOT)/src/base/*/*/) \
           $(GMAT_ROOT)/src/gmatutil/include/ \
           $(wildcard $(GMAT_ROOT)/src/gmatutil/util/) \
           $(wildcard $(GMAT_ROOT)/src/gmatutil/util/*/) \
           ../Common/
INCFLAGS := $(addprefix -I,$(INCDIRS)) \
            -I$(GMAT_ROOT)/depends/cspice/macosx/cspice64/include

SRC     := TestSolarFluxReader.cpp ../Common/TestOutput.cpp
TARGET  := TestSolarFluxReader
LIBS    := $(BINDIR)/libGmatBase.dylib $(BINDIR)/libGmatUtil.dylib
RPATH   := -Wl,-rpath,$(abspath $(BINDIR))

WEATHER := $(GMAT_ROOT)/application/data/atmosphere/earth/SpaceWeather-All-v1.2.txt

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCFLAGS) $(SRC) $(LIBS) $(RPATH) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(abspath $(WEATHER))

clean:
	rm -f $(TARGET) TestSolarFluxReader.out

.PHONY: all run clean
