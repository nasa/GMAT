# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT DataCallback Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/DataCallbackPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT DataCallback Plugin setup " ${VERSION}) 

SET(TargetName DataCallback)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin subscriber)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/DataCallbackFactory.cpp
    plugin/GmatPluginFunctions.cpp
    subscriber/DataCallback.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.

# FORCE the plugin to install directly into bin
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" bin FORCE)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "DATACALLBACK_EXPORTS")

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

# Override debug output directory
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  LIBRARY_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  )
