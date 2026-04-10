# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT NewParameter Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/NewParameterPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT NewParameter Plugin setup " ${VERSION}) 

# For some reason the binary is NewParameters, not NewParameter
SET(TargetName NewParameters)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include parameter plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/NewParameterFactory.cpp
    parameter/AtmosDensity.cpp
    parameter/FMAcceleration.cpp
    parameter/FMDensity.cpp
    parameter/OdeData.cpp
    parameter/OdeReal.cpp
    parameter/OdeRvec3.cpp
    parameter/SolverData.cpp
    parameter/SolverReal.cpp
    parameter/SolverState.cpp
    parameter/SolverStatus.cpp
    parameter/SolverString.cpp
    parameter/Torque.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "NEW_PARAMETER_EXPORTS")
