# Project:        Collocation
# Created by:     Jeremy Knittel
# Date:           2015.08.25


# Define macros for locations
BASE_ROOT = ../..
TEST_ROOT = ..
BOOST_ROOT = /usr/local/include/boost
SNOPT_ROOT = /Users/jknittel/Utilities/SNOPT/SNOPT75_1_8_2016/
BINDIR = $(TEST_ROOT)/bin
EXECUTABLE = $(BINDIR)/TestOptimizer

TEST_FLAGS = -DUSE_64_BIT_LONGS -D__CONSOLE_APP__
## TEST_FLAGS = -DUSE_64_BIT_LONGS -D__DEBUG__ -D__CONSOLE_APP__

#OPTIMIZATIONS = -O3 -funroll-loops
OPTIMIZATIONS = -O1

# Define macros for the needed includes
HEADERS =   -I. \
            -I$(BASE_ROOT)/test/TestOptimizer \
            -I$(BASE_ROOT)/src/userfunutils \
            -I$(BASE_ROOT)/src/collutils \
            -I$(BASE_ROOT)/src/executive \
            -I$(BASE_ROOT)/src/util \
            -I$(BASE_ROOT)/GMAT_Utilities/include \
            -I$(BASE_ROOT)/GMAT_Utilities/util \
            -I$(BASE_ROOT)/GMAT_Utilities/console \
            -I$(BOOST_ROOT) \
			-I$(SNOPT_ROOT)/interfaces \
			-I$(SNOPT_ROOT)/interfaces/include \
			-I$(SNOPT_ROOT)/interfaces/src \
            
DRIVER = TestOptimizer.o
            
OBJECTS = \
    $(BASE_ROOT)/src/util/LowThrustException.o \
    $(BASE_ROOT)/src/util/RadauMathUtil.o \
    $(BASE_ROOT)/src/util/BarycentricLagrangeInterpolator.o \
    $(BASE_ROOT)/src/util/SparseMatrixUtil.o \
	$(BASE_ROOT)/src/util/SNOPTFunctionWrapper.o \
    $(BASE_ROOT)/src/executive/GuessGenerator.o \
    $(BASE_ROOT)/src/executive/Phase.o \
    $(BASE_ROOT)/src/executive/RadauPhase.o \
    $(BASE_ROOT)/src/executive/Optimizer.o \
    $(BASE_ROOT)/src/collutils/DecisionVector.o \
    $(BASE_ROOT)/src/collutils/DecVecTypeBetts.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtilRadau.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtil.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtil_AlgPath.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtil_Coll.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtil_Path.o \
    $(BASE_ROOT)/src/collutils/NLPFunctionData.o \
    $(BASE_ROOT)/src/collutils/ProblemCharacteristics.o \
    $(BASE_ROOT)/src/userfunutils/BoundData.o \
    $(BASE_ROOT)/src/userfunutils/DummyPathFunction.o \
    $(BASE_ROOT)/src/userfunutils/DummyPathFunction2.o \
    $(BASE_ROOT)/src/userfunutils/UserFunction.o \
    $(BASE_ROOT)/src/userfunutils/UserFunctionManager.o \
    $(BASE_ROOT)/src/userfunutils/UserPathFunctionManager.o \
    $(BASE_ROOT)/src/userfunutils/UserFunctionProperties.o \
    $(BASE_ROOT)/src/userfunutils/UserPathFunction.o \
    $(BASE_ROOT)/src/userfunutils/UserPointFunction.o \
    $(BASE_ROOT)/src/userfunutils/FunctionOutputData.o \
    $(BASE_ROOT)/src/userfunutils/FunctionInputData.o \
    $(BASE_ROOT)/src/userfunutils/FunctionContainer.o \
    $(BASE_ROOT)/src/userfunutils/PathFunctionContainer.o \
    $(BASE_ROOT)/src/userfunutils/PointFunctionContainer.o \
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

###    $(BASE_ROOT)/src/userfunutils/DummyPathFunction.o \

CPPFLAGS = $(HEADERS) $(OPTIMIZATIONS) $(TESTFLAGS) -Wall
CPP = g++

# Link specific flags
LINK_FLAGS =  /usr/lib/libstdc++.6.dylib \
               -headerpad_max_install_names \
               -lm -v \
			   -L$(SNOPT_ROOT)/lib/.libs/ \
			   -lsnopt7_cpp \
			   -lsnopt7

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
