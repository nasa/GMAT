# CSALT: Collocation Stand Alone Library and Toolkit
#
# Copyright (c) 2002-2026 United States Government as represented by the
# Administrator of the National Aeronautics and Space Administration.
# All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# You may not use this file except in compliance with the License.
# You may obtain a copy of the License at:
# http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
# express or implied.   See the License for the specific language
# governing permissions and limitations under the License.
#
# Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Purchase
# Order NNG16LD52P.
#
# -----------------------------------------------------------------------------

MESSAGE("==============================")
MESSAGE("CSALT test program " ${VERSION}) 

SET(TargetName CSALTTester)

# Add preprocessor definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")

Set(CSALT_TEST_DIRS
   src/HelperClasses
   src/TestOptCtrl/src
   src/TestOptCtrl/src/drivers
   src/TestOptCtrl/src/pointpath
   src/TestOptCtrl/src/guess
   src/TestAPI/src
   src/TestAPI/src/drivers
   src/TestAPI/src/guess
   src/TestAPI/src/pointpath
   ${CSALT_DIRS}
   ${CSALT_C_WRAPPER_DIRS}
   ${UTIL_DIRS}
)


SET(CSALT_TEST_SRCS
    src/TestOptCtrl/src/TestOptCtrl.cpp
    src/TestOptCtrl/src/TrajectoryDriver.cpp
    src/TestOptCtrl/src/PhaseDriver.cpp
    src/TestOptCtrl/src/DecisionVectorDriver.cpp
    src/TestOptCtrl/src/ConsoleMessageReceiver.cpp
    src/TestOptCtrl/src/drivers/CsaltTestDriver.cpp
    src/TestOptCtrl/src/drivers/BangBangDriver.cpp
    src/TestOptCtrl/src/drivers/BrachistochroneDriver.cpp
    src/TestOptCtrl/src/drivers/BrysonDenhamDriver.cpp
    src/TestOptCtrl/src/drivers/BrysonMaxRangeDriver.cpp
    src/TestOptCtrl/src/drivers/ConwayOrbitExampleDriver.cpp
    src/TestOptCtrl/src/drivers/ConwayOrbitExampleRKDriver.cpp
    src/TestOptCtrl/src/drivers/GoddardRocketThreePhaseDriver.cpp
    src/TestOptCtrl/src/drivers/GoddardRocketDriver.cpp
    src/TestOptCtrl/src/drivers/Hull95Driver.cpp
    src/TestOptCtrl/src/drivers/HyperSensitiveDriver.cpp
    src/TestOptCtrl/src/drivers/CatalyticGasOilCrackerDriver.cpp
    src/TestOptCtrl/src/drivers/InteriorPointDriver.cpp
    src/TestOptCtrl/src/drivers/LinearTangentSteeringDriver.cpp
    src/TestOptCtrl/src/drivers/LinearTangentSteeringStaticVarDriver.cpp
    src/TestOptCtrl/src/drivers/MoonLanderDriver.cpp
    src/TestOptCtrl/src/drivers/ObstacleAvoidanceDriver.cpp
    src/TestOptCtrl/src/drivers/RauAutomaticaDriver.cpp
    src/TestOptCtrl/src/drivers/RayleighControlStateConstraintDriver.cpp
    src/TestOptCtrl/src/drivers/RayleighDriver.cpp
    src/TestOptCtrl/src/drivers/SchwartzDriver.cpp
    src/TestOptCtrl/src/drivers/HohmannTransferDriver.cpp
	src/TestOptCtrl/src/drivers/TutorialDriver.cpp
    src/TestOptCtrl/src/pointpath/GoddardRocketThreePhasePathObject.cpp
    src/TestOptCtrl/src/pointpath/GoddardRocketThreePhasePointObject.cpp
    src/TestOptCtrl/src/pointpath/SchwartzPathObject.cpp
    src/TestOptCtrl/src/pointpath/SchwartzPointObject.cpp
    src/TestOptCtrl/src/pointpath/BangBangPointObject.cpp
    src/TestOptCtrl/src/pointpath/BangBangPathObject.cpp
    src/TestOptCtrl/src/pointpath/BrachistichronePointObject.cpp
    src/TestOptCtrl/src/pointpath/BrachistichronePathObject.cpp
    src/TestOptCtrl/src/pointpath/ConwayOrbitExamplePointObject.cpp
    src/TestOptCtrl/src/pointpath/ConwayOrbitExamplePathObject.cpp
    src/TestOptCtrl/src/pointpath/MoonLanderPointObject.cpp
    src/TestOptCtrl/src/pointpath/MoonLanderPathObject.cpp
    src/TestOptCtrl/src/pointpath/HyperSensitivePathObject.cpp
    src/TestOptCtrl/src/pointpath/CatalyticGasOilCrackerPathObject.cpp
    src/TestOptCtrl/src/pointpath/HyperSensitivePointObject.cpp
    src/TestOptCtrl/src/pointpath/CatalyticGasOilCrackerPointObject.cpp
    src/TestOptCtrl/src/pointpath/OrbitRaisingMultiPhasePointObject.cpp
    src/TestOptCtrl/src/pointpath/OrbitRaisingMultiPhasePathObject.cpp
    src/TestOptCtrl/src/pointpath/OrbitRaisingPointObject.cpp
    src/TestOptCtrl/src/pointpath/OrbitRaisingPathObject.cpp
    src/TestOptCtrl/src/pointpath/RauAutomaticaPointObject.cpp
    src/TestOptCtrl/src/pointpath/RauAutomaticaPathObject.cpp
    src/TestOptCtrl/src/pointpath/RayleighPointObject.cpp
    src/TestOptCtrl/src/pointpath/RayleighPathObject.cpp
    src/TestOptCtrl/src/pointpath/RayleighControlStateConstraintPointObject.cpp
    src/TestOptCtrl/src/pointpath/RayleighControlStateConstraintPathObject.cpp
    src/TestOptCtrl/src/pointpath/GoddardRocketPointObject.cpp
    src/TestOptCtrl/src/pointpath/GoddardRocketPathObject.cpp
    src/TestOptCtrl/src/pointpath/BrysonDenhamPointObject.cpp
    src/TestOptCtrl/src/pointpath/BrysonDenhamPathObject.cpp
    src/TestOptCtrl/src/pointpath/BrysonMaxRangePointObject.cpp
    src/TestOptCtrl/src/pointpath/BrysonMaxRangePathObject.cpp
    src/TestOptCtrl/src/pointpath/InteriorPointPointObject.cpp
    src/TestOptCtrl/src/pointpath/InteriorPointPathObject.cpp
    src/TestOptCtrl/src/pointpath/Hull95PointObject.cpp
    src/TestOptCtrl/src/pointpath/Hull95PathObject.cpp
    src/TestOptCtrl/src/pointpath/LinearTangentSteeringPointObject.cpp
    src/TestOptCtrl/src/pointpath/LinearTangentSteeringPathObject.cpp
    src/TestOptCtrl/src/pointpath/LinearTangentSteeringPointStaticVarObject.cpp
    src/TestOptCtrl/src/pointpath/LinearTangentSteeringPathStaticVarObject.cpp
    src/TestOptCtrl/src/pointpath/ObstacleAvoidancePointObject.cpp
    src/TestOptCtrl/src/pointpath/ObstacleAvoidancePathObject.cpp
    src/TestOptCtrl/src/pointpath/HohmannTransferPointObject.cpp
    src/TestOptCtrl/src/pointpath/HohmannTransferPathObject.cpp
	src/TestOptCtrl/src/pointpath/TutorialPointObject.cpp
    src/TestOptCtrl/src/pointpath/TutorialPathObject.cpp
)

# ====================================================================
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE CSALT_TEST_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# add the install targets

# Create CSALT test executable
ADD_EXECUTABLE(${TargetName} ${CSALT_TEST_SRCS} ${CSALT_TEST_HEADERS})

# Add necessary include directories
TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${CSALT_DIRS})
TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${CSALT_TEST_DIRS})
TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${SNOPT_INCLUDE_DIR})
TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${Boost_INCLUDE_DIR})

if(PLUGIN_PROPRIETARY_CSALT_C_WRAPPER)
   TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${CSALT_C_WRAPPER_DIRS})
endif()

# Link needed libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${SNOPT_CPPLIBRARY})
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${SNOPT_LIBRARY})
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE CSALT)
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatUtil)

if(PLUGIN_PROPRIETARY_CSALT_C_WRAPPER)
   TARGET_LINK_LIBRARIES(${TargetName} PRIVATE CsaltCWrapper)
endif()

if(UNIX)
  if(APPLE)
    TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${COREFOUNDATION_LIBRARY} ${CORESERVICES_LIBRARY})
  else()
    TARGET_LINK_LIBRARIES(${TargetName} PRIVATE Threads::Threads)
  endif()
endif()

# Windows-specific link flags
if(WIN32)
  SET_TARGET_PROPERTIES(${TargetName} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:\"libcmt.lib\" /INCREMENTAL:NO")
endif()

# Create build products in bin directory
_SETOUTPUTDIRECTORY(${TargetName} bin)

# Override debug output directory
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  LIBRARY_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  )

# Specify where to install (make install or VS "INSTALL" project)
if(WIN32)
  INSTALL(TARGETS ${TargetName} RUNTIME DESTINATION bin)
else()
  if(APPLE)
    SET(INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/Frameworks)
  else()
    SET(INSTALL_DIR bin)
  endif()
  INSTALL(TARGETS ${TargetName} DESTINATION bin)
endif()

# Set RPATH to find shared libraries in default locations on Mac/Linux
if(UNIX)
  if(APPLE)
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/;@loader_path/../plugins/;@loader_path/../plugins/proprietary/"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/;\$ORIGIN/../plugins/;\$ORIGIN/../plugins/proprietary/"
      )
  endif()
endif()
