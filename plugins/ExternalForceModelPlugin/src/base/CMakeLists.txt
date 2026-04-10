# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT External Force Model Plugin.
# (Adapted from the PythonInterfacePlugin plugin CMake script)
# This file must be installed in the plugins/ExternalForceModelPlugin/src/base directory
#
# Author: Alex Campbell
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT External Force Model Plugin setup (for Python ${ExternalForceModel_Python3_Version})" ${VERSION})

# ====================================================================
# Major error checks

# Find specified Python3 version, using the specified Python3 root directory
_SETPYTHONROOTDIR(${ExternalForceModel_Python3_Version})
FIND_PACKAGE(Python3 ${ExternalForceModel_Python3_Version} EXACT COMPONENTS Development)
if(NOT Python3_Development_FOUND)
  MESSAGE(NOTICE "Skipping ExternalForceModel: Python ${ExternalForceModel_Python3_Version} not found")
  _REPORTPYTHONURL(${ExternalForceModel_Python3_Version})
  RETURN()
endif()

SET(TargetPySuffix "_py${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
SET(TargetName "ExternalForceModel${TargetPySuffix}")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin forcemodel)

# ====================================================================
# source files
SET(PLUGIN_SRCS    
  factory/ExternalModelFactory.cpp
  plugin/GmatPluginFunctions.cpp
  forcemodel/ExternalModel.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "EXTERNALMODEL_EXPORTS")

# ====================================================================
# Additional link libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE Python3::Python)
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE PythonInterface${TargetPySuffix})
