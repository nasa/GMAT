# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT GUI
# This file must be installed in the src/console directory
#
# Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

PROJECT(GMAT-CSALT_UnitTester C CXX)
cmake_minimum_required(VERSION 3.7)

MESSAGE("==============================")
MESSAGE("GMAT Test Program setup " ${VERSION})

# Enforce C++11
SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

SET(TargetName TestGmatCsalt)

SET(GMAT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/../../base/")
SET(CSALTINTERFACE_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/../../../../gsfcinternal/code/CSALTPlugin/src/base/")
SET(CSALT_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lowthrust/CPlusPlus/")

SET(TESTER_GMAT_BUILD_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/../../../application/")
SET(TESTER_GMAT_LIB_LOCATION "${TESTER_GMAT_BUILD_LOCATION}bin/")
SET(CSALTINTERFACE_LIB_LOCATION "${TESTER_GMAT_BUILD_LOCATION}plugins/proprietary/")
SET(CSALT_LIB_LOCATION "${CSALT_LOCATION}lib/")


find_library(GMATBASE_LIBRARY GmatBase HINTS ${TESTER_GMAT_LIB_LOCATION})
find_library(CSALTINTERFACE_LIBRARY CsaltInterface HINTS ${CSALTINTERFACE_LIB_LOCATION})
find_library(CSALT_LIBRARY CSALT HINTS ${CSALT_LIB_LOCATION})

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${TESTER_GMAT_LIB_LOCATION}" )

SET(BASE_DIRS
  ${GMAT_LOCATION}asset
  ${GMAT_LOCATION}attitude
  ${GMAT_LOCATION}burn
  ${GMAT_LOCATION}command
  ${GMAT_LOCATION}configs
  ${GMAT_LOCATION}coordsystem
  ${GMAT_LOCATION}event
  ${GMAT_LOCATION}executive
  ${GMAT_LOCATION}factory
  ${GMAT_LOCATION}forcemodel
  ${GMAT_LOCATION}forcemodel/harmonic
  ${GMAT_LOCATION}foundation
  ${GMAT_LOCATION}function
  ${GMAT_LOCATION}hardware
  ${GMAT_LOCATION}include
  ${GMAT_LOCATION}interface
  ${GMAT_LOCATION}interpreter
  ${GMAT_LOCATION}math
  ${GMAT_LOCATION}parameter
  ${GMAT_LOCATION}plugin
  ${GMAT_LOCATION}propagator
  ${GMAT_LOCATION}solarsys
  ${GMAT_LOCATION}solver
  ${GMAT_LOCATION}spacecraft
  ${GMAT_LOCATION}stopcond
  ${GMAT_LOCATION}subscriber
  ${GMAT_LOCATION}util
  ${GMAT_LOCATION}util/datawriter
  ${GMAT_LOCATION}util/interpolator
  ${GMAT_LOCATION}../console
  )
# ====================================================================
# header file locationss
SET(CSALT_HEADERS
   ${CSALTINTERFACE_LOCATION}/control
   ${CSALTINTERFACE_LOCATION}/factory
   ${CSALTINTERFACE_LOCATION}/function
   ${CSALTINTERFACE_LOCATION}/guess
   ${CSALTINTERFACE_LOCATION}/include
   ${CSALTINTERFACE_LOCATION}/plugin
   ${CSALTINTERFACE_LOCATION}/source   
   ${CSALT_LOCATION}include
   ${CSALT_LOCATION}GMAT_Utilities/console
   ${CSALT_LOCATION}GMAT_Utilities/include
   ${CSALT_LOCATION}GMAT_Utilities/util
   ${CSALT_LOCATION}GMAT_Utilities/util/interpolator
   ${CSALT_LOCATION}src/collutils
   ${CSALT_LOCATION}src/executive
   ${CSALT_LOCATION}src/userfunutils
   ${CSALT_LOCATION}src/util
   
)

# ====================================================================
# source files
SET(CONSOLE_SRCS 
    TestDriver.cpp 
      ${GMAT_LOCATION}../console/ConsoleMessageReceiver.cpp
)


# ====================================================================
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE CONSOLE_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# compilation

# add the install targets
ADD_EXECUTABLE(${TargetName} ${CONSOLE_SRCS} ${CONSOLE_HEADERS})
TARGET_INCLUDE_DIRECTORIES(${TargetName} PRIVATE ${BASE_DIRS} ${CSALT_HEADERS})

# The debug Console should have the same postfix as top-levl CMakeLists.txt
# This is required because CMAKE_DEBUG_POSTFIX only applies to libraries
# and not executables as of CMake 3.0.2
#SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# ====================================================================
# Link libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${GMATBASE_LIBRARY} ${CSALTINTERFACE_LIBRARY} ${CSALT_LIBRARY})

# ====================================================================
# Add source/header files to IDE-based project source groups
# Macro defined in top-level CMakeLists.txt
#_ADDSOURCEGROUPS("")

# Create build outputs in bin directory
#_SETOUTPUTDIRECTORY(${TargetName} bin)

# Override debug output directory
#SET_TARGET_PROPERTIES(${TargetName} PROPERTIES 
#  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
#  )

# Specify where to install (make install or VS "INSTALL" project)
#INSTALL( TARGETS ${TargetName}
#  DESTINATION bin
#  )

# Set RPATH to find shared libraries in default locations on Mac/Linux
if(UNIX)
  if(APPLE)
    SET(MAC_BASEPATH "../${GMAT_MAC_APPBUNDLE_PATH}/Frameworks/")
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/${MAC_BASEPATH}"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/"
      )
  endif()
endif()
