# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT ExtraPropagators Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/ExtraPropagatorsPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT ExtraPropagators Plugin setup " ${VERSION}) 

SET(TargetName ExtraPropagators)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin propagator)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/ExtraPropagatorFactory.cpp
    plugin/GmatPluginFunctions.cpp
    propagator/BulirschStoer.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "PROPAGATOR_EXPORTS")
