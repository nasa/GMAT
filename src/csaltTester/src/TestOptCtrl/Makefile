# Define macros for locations
BASE_ROOT = ../..
TEST_ROOT = $(BASE_ROOT)/test
BOOST_ROOT = $(HOME)/boost_1_62_0
SNOPT_ROOT = $(HOME)/snopt7
BINDIR = $(TEST_ROOT)/bin
EXECUTABLE = $(BINDIR)/TestOptCtrl

# TEST_FLAGS = -DUSE_64_BIT_LONGS -D__CONSOLE_APP__
TEST_FLAGS = -DUSE_64_BIT_LONGS -D__DEBUG__ -D__CONSOLE_APP__

#OPTIMIZATIONS = -O3 -funroll-loops
OPTIMIZATIONS = 

# Define macros for the needed includes
HEADERS =   -I. \
            -I$(BASE_ROOT)/include \
            -I$(BASE_ROOT)/test/HelperClasses \
            -I$(BASE_ROOT)/test/TestOptCtrl/src/pointpath \
            -I$(BASE_ROOT)/src/userfunutils \
            -I$(BASE_ROOT)/src/collutils \
            -I$(BASE_ROOT)/src/executive \
            -I$(BASE_ROOT)/src/util \
            -I$(BASE_ROOT)/GMAT_Utilities/include \
            -I$(BASE_ROOT)/GMAT_Utilities/util \
            -I$(BASE_ROOT)/GMAT_Utilities/util/interpolator \
            -I$(BASE_ROOT)/GMAT_Utilities/console \
            -I$(BOOST_ROOT) \
            -I$(SNOPT_ROOT)/interfaces \
            -I$(SNOPT_ROOT)/interfaces/include \
            -I$(SNOPT_ROOT)/interfaces/src \
            
DRIVER = src/TestOptCtrl.o \
			src/drivers/CsaltTestDriver.o \
			src/drivers/BrachistochroneDriver.o \
			src/TrajectoryDriver.o \
			src/drivers/HyperSensitiveDriver.o \
			src/PhaseDriver.o \
			src/DecisionVectorDriver.o \
			src/drivers/RayleighDriver.o \
			src/drivers/ConwayOrbitExampleDriver.o \
			src/drivers/ConwayOrbitExampleRKDriver.o \
			src/drivers/RayleighControlStateConstraintDriver.o \
			src/drivers/SchwartzDriver.o \
			src/drivers/MoonLanderDriver.o \
			src/drivers/GoddardRocketDriver.o \
			src/drivers/GoddardRocketThreePhaseDriver.o \
			src/drivers/RauAutomaticaDriver.o \
			src/drivers/LinearTangentSteeringDriver.o \
			src/drivers/ObstacleAvoidanceDriver.o \
			src/drivers/BrysonDenhamDriver.o \
			src/drivers/BrysonMaxRangeDriver.o \
			src/drivers/Hull95Driver.o \
			src/drivers/InteriorPointDriver.o
            
OBJECTS = \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/BrachistichronePathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/BrachistichronePointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/OrbitRaisingMultiPhasePathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/OrbitRaisingMultiPhasePointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/HyperSensitivePathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/HyperSensitivePointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/OrbitRaisingPathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/RayleighPathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/RayleighPointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/ConwayOrbitExamplePathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/ConwayOrbitExamplePointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/RayleighControlStateConstraintPathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/RayleighControlStateConstraintPointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/SchwartzPathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/SchwartzPointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/MoonLanderPathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/MoonLanderPointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/GoddardRocketPathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/GoddardRocketPointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/GoddardRocketThreePhasePathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/GoddardRocketThreePhasePointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/RauAutomaticaPathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/RauAutomaticaPointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/LinearTangentSteeringPathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/LinearTangentSteeringPointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/ObstacleAvoidancePathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/ObstacleAvoidancePointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/BrysonDenhamPathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/BrysonDenhamPointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/BrysonMaxRangePathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/BrysonMaxRangePointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/Hull95PathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/Hull95PointObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/InteriorPointPathObject.o \
    $(TEST_ROOT)/TestOptCtrl/src/pointpath/InteriorPointPointObject.o \
    $(BASE_ROOT)/src/util/LowThrustException.o \
    $(BASE_ROOT)/src/util/RadauMathUtil.o \
    $(BASE_ROOT)/src/util/SparseMatrixUtil.o \
	 $(BASE_ROOT)/src/util/SNOPTFunctionWrapper.o \
    $(BASE_ROOT)/src/executive/Phase.o \
    $(BASE_ROOT)/src/executive/RadauPhase.o \
    $(BASE_ROOT)/src/executive/ImplicitRKPhase.o \
    $(BASE_ROOT)/src/executive/Optimizer.o \
    $(BASE_ROOT)/src/executive/Trajectory.o \
    $(BASE_ROOT)/src/collutils/DecisionVector.o \
    $(BASE_ROOT)/src/collutils/DecVecTypeBetts.o \
    $(BASE_ROOT)/src/collutils/ImplicitRungeKutta.o \
    $(BASE_ROOT)/src/collutils/LobattoIIIA_2Order.o \
    $(BASE_ROOT)/src/collutils/LobattoIIIA_4HSOrder.o \
    $(BASE_ROOT)/src/collutils/LobattoIIIA_4Order.o \
    $(BASE_ROOT)/src/collutils/LobattoIIIA_6Order.o \
    $(BASE_ROOT)/src/collutils/LobattoIIIA_8Order.o \
    $(BASE_ROOT)/src/collutils/LobattoIIIASeparated.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtilRadau.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtil_ImplicitRK.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtil.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtil_AlgPath.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtil_Coll.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtil_MultiPoint.o \
    $(BASE_ROOT)/src/collutils/NLPFuncUtil_Path.o \
    $(BASE_ROOT)/src/collutils/NLPFunctionData.o \
    $(BASE_ROOT)/src/collutils/ProblemCharacteristics.o \
    $(BASE_ROOT)/src/userfunutils/ArrayTrajectoryData.o \
    $(BASE_ROOT)/src/userfunutils/BoundData.o \
    $(BASE_ROOT)/src/userfunutils/DummyPathFunction.o \
    $(BASE_ROOT)/src/userfunutils/DummyPathFunction2.o \
    $(BASE_ROOT)/src/userfunutils/JacobianData.o \
    $(BASE_ROOT)/src/userfunutils/OCHTrajectoryData.o \
    $(BASE_ROOT)/src/userfunutils/OCHTrajectorySegment.o \
    $(BASE_ROOT)/src/userfunutils/TrajectoryData.o \
    $(BASE_ROOT)/src/userfunutils/TrajectorySegment.o \
    $(BASE_ROOT)/src/userfunutils/UserFunction.o \
    $(BASE_ROOT)/src/userfunutils/UserFunctionManager.o \
    $(BASE_ROOT)/src/userfunutils/UserPathFunctionManager.o \
    $(BASE_ROOT)/src/userfunutils/UserPointFunctionManager.o \
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
    $(BASE_ROOT)/GMAT_Utilities/util/TimeTypes.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/BrentDekkerZero.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/CubicSplineInterpolator.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/Interpolator.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/InterpolatorException.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/LagrangeInterpolator.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/LinearInterpolator.o \
    $(BASE_ROOT)/GMAT_Utilities/util/interpolator/NotAKnotInterpolator.o
    

###    $(BASE_ROOT)/src/userfunutils/DummyPathFunction.o \

CPPFLAGS = -g $(HEADERS) $(OPTIMIZATIONS) $(TESTFLAGS) -Wall -std=c++11
CPP = g++

# Link specific flags
LINK_FLAGS =  -lm -v -g \
			   -L$(SNOPT_ROOT)/lib/.libs/ \
			   -lsnopt7_cpp \
			   -lsnopt7 \
			   -Wl,-rpath,.

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
	$(CPP) $(CPPFLAGS) $(LINK_FLAGS) $(DRIVER) $(OBJECTS) $(LINK_FLAGS) -o $@
	@chmod -R 755 $(BINDIR)

clean:
	rm -f $(DRIVER) $(OBJECTS) core

bare: clean
	rm -f $(EXECUTABLE) 
