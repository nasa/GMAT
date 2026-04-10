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

MESSAGE("==============================")
MESSAGE("GMAT Console setup " ${VERSION})

SET(TargetName GmatConsole)

# ====================================================================
# source files
SET(CONSOLE_SRCS 
    driver.cpp 
    ConsoleAppException.cpp
    PrintUtility.cpp
    ConsoleMessageReceiver.cpp
)

# ====================================================================
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE CONSOLE_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# compilation

# add the install targets
ADD_EXECUTABLE(${TargetName} ${CONSOLE_SRCS} ${CONSOLE_HEADERS})

# The debug Console should have the same postfix as top-levl CMakeLists.txt
# This is required because CMAKE_DEBUG_POSTFIX only applies to libraries
# and not executables as of CMake 3.0.2
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# ====================================================================
# Link libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatUtil)
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatBase)

# ====================================================================
# Add source/header files to IDE-based project source groups
# Macro defined in top-level CMakeLists.txt
_ADDSOURCEGROUPS("")

# Create build outputs in bin directory
_SETOUTPUTDIRECTORY(${TargetName} bin)

# Override debug output directory
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  )

# Specify where to install (make install or VS "INSTALL" project)
if(APPLE)
  SET(INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/MacOS)
else()
  SET(INSTALL_DIR bin)
endif()
INSTALL(TARGETS ${TargetName} DESTINATION ${INSTALL_DIR})

# On Mac, create symlink in user-level bin folder
if(APPLE)
  # Get relative path from user-level bin folder to GmatConsole
  SET(TARGET_SYMLINK_FULLPATH "${CMAKE_INSTALL_PREFIX}/bin")
  SET(TARGET_LIB_FULLFILE "${CMAKE_INSTALL_PREFIX}/${INSTALL_DIR}/${TargetName}")
  FILE(RELATIVE_PATH TARGET_RELPATH
    "${TARGET_SYMLINK_FULLPATH}"
    "${TARGET_LIB_FULLFILE}"
  )
  
  # Create symlink
  INSTALL(CODE "EXECUTE_PROCESS( \
                COMMAND ${CMAKE_COMMAND} -E create_symlink \
                ${TARGET_RELPATH} \
                ${TARGET_SYMLINK_FULLPATH}/${TargetName} \
                )"
  )
endif()
  
# Set RPATH to find shared libraries in default locations on Mac/Linux
if(UNIX)
  if(APPLE)
    SET(MAC_BASEPATH "../Frameworks/")
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/${MAC_BASEPATH}"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/"
      )
  endif()
endif()
