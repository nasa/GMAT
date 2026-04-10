# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Geometric Measurement Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/GeometricMeasurementPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT Geometric Measurement Plugin setup " ${VERSION}) 

SET(TargetName GeometricMeasurements)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include measurement plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/MeasurementFactory.cpp
    measurement/GeometricAzEl.cpp
    measurement/GeometricRADec.cpp
    measurement/GeometricRange.cpp
    measurement/GeometricRangeRate.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "GEOMETRICMEAS_EXPORTS")

# ====================================================================
# Additional link libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatEstimation)
