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
MESSAGE("CSALT library setup " ${VERSION}) 

SET(TargetName CSALT)

# Add preprocessor definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DCSALT_EXPORTS")
ADD_DEFINITIONS("-DEXP_STL")

SET(CSALT_DIRS
   src
   src/include
   src/collutils
   src/executive
   src/userfunutils
   src/util
   ${UTIL_DIRS}
)

SET(CSALT_SRCS
    src/collutils/NLPFuncUtil.cpp
    src/collutils/DecisionVector.cpp
    src/collutils/NLPFuncUtil_Coll.cpp
    src/collutils/NLPFunctionData.cpp
    src/collutils/LobattoIIIA_4Order.cpp
    src/collutils/LobattoIIIASeparated.cpp
    src/collutils/NLPFuncUtil_MultiPoint.cpp
    src/collutils/LobattoIIIA_8Order.cpp
    src/collutils/NLPFuncUtil_Path.cpp
    src/collutils/NLPFuncUtil_AlgPath.cpp
    src/collutils/NLPFuncUtilRadau.cpp
    src/collutils/LobattoIIIA_6Order.cpp
    src/collutils/ProblemCharacteristics.cpp
    src/collutils/LobattoIIIA_4HSOrder.cpp
    src/collutils/NLPFuncUtil_ImplicitRK.cpp
    src/collutils/DecVecTypeBetts.cpp
    src/collutils/ImplicitRungeKutta.cpp
    src/collutils/LobattoIIIA_2Order.cpp
    src/util/RadauMathUtil.cpp
    src/util/LobattoIIIaMathUtil.cpp
    src/util/SNOPTFunctionWrapper.cpp
    src/util/LowThrustException.cpp
    src/util/BaryLagrangeInterpolator.cpp
    src/util/SparseMatrixUtil.cpp
    src/util/ScaleUtility.cpp
    src/util/ScalingUtility.cpp
    src/util/GuessGenerator.cpp
    src/util/ArrayTrajectoryData.cpp

    src/util/OCHTrajectoryData.cpp
    src/util/TrajectorySegment.cpp
    src/util/DummyPathFunction.cpp
    src/util/TrajectoryData.cpp
    src/util/OCHTrajectorySegment.cpp
    src/util/DummyPathFunction2.cpp
    src/util/ModEqDynamics.cpp

    src/executive/ExecutionInterface.cpp
    src/executive/ImplicitRKPhase.cpp
    src/executive/RadauPhase.cpp
    src/executive/OrbitPhase.cpp
    src/executive/SnoptOptimizer.cpp
    src/executive/Phase.cpp
    src/executive/Trajectory.cpp
    src/executive/OrbitTrajectory.cpp
    src/userfunutils/BoundData.cpp
    src/userfunutils/OptimalControlFunction.cpp
    src/userfunutils/PathFuncProperties.cpp
    src/userfunutils/FunctionOutputData.cpp
    src/userfunutils/UserFunctionManager.cpp
    src/userfunutils/UserPointFunctionManager.cpp
    src/userfunutils/PathFunctionContainer.cpp
    src/userfunutils/UserFunctionProperties.cpp
    src/userfunutils/FunctionInputData.cpp
    src/userfunutils/UserPathFunctionManager.cpp
    src/userfunutils/PointFunctionContainer.cpp
    src/userfunutils/JacobianData.cpp
    src/userfunutils/UserPointFunction.cpp
    src/userfunutils/OrbitPointFunction.cpp
    src/userfunutils/FunctionContainer.cpp

    src/userfunutils/UserPathFunction.cpp
    src/userfunutils/OrbitPathFunction.cpp
    src/userfunutils/UserFunction.cpp
)

# ====================================================================
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE CSALT_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# add the install targets

# Create CSALT as a shared library
ADD_LIBRARY(${TargetName} SHARED ${CSALT_SRCS} ${CSALT_HEADERS})

# Add necessary include directories
TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${CSALT_DIRS})
TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${SNOPT_INCLUDE_DIR})
TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${Boost_INCLUDE_DIR})

# Link needed libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${SNOPT_CPPLIBRARY})
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${SNOPT_LIBRARY})
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatUtil)

if(UNIX)
  if(APPLE)
    TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${COREFOUNDATION_LIBRARY} ${CORESERVICES_LIBRARY})
  else()
    TARGET_LINK_LIBRARIES(${TargetName} PRIVATE Threads::Threads)
  endif()
endif()

# Library name should start with "lib"
# This is always true for Mac/Linux, but needs to be specified for Windows
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES PREFIX "lib")

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
  INSTALL(TARGETS ${TargetName} LIBRARY DESTINATION ${INSTALL_DIR})
endif()

# Install SNOPT libraries
  if(GMAT_INCLUDE_CSALT_INSTALL_SNOPT)

  if(WIN32)
    SET(SNOPT_INSTALL_DIR bin)
    SET(SNOPT_PATTERN "*.dll")
  else()
    if(APPLE)
      SET(SNOPT_INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/Frameworks)
      SET(SNOPT_PATTERN "*snopt7*.dylib")
    else()
      SET(SNOPT_INSTALL_DIR lib)
      SET(SNOPT_PATTERN "*snopt7*.so*")
    endif()
  endif()

  INSTALL(DIRECTORY ${SNOPT_LIB_DIR}/
          DESTINATION ${SNOPT_INSTALL_DIR}
          USE_SOURCE_PERMISSIONS
          FILES_MATCHING PATTERN "${SNOPT_PATTERN}"
          PATTERN ".*" EXCLUDE
  )
endif()

# Set RPATH to find shared libraries in default locations on Mac/Linux
if(UNIX)
  if(APPLE)
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/;\$ORIGIN/../lib/"
      )
  endif()
endif()
