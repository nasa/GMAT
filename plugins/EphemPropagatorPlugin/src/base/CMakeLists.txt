# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT EphemPropagator Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/EphemPropagatorPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT EphemPropagator Plugin setup " ${VERSION}) 

SET(TargetName EphemPropagator)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin propagator)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/EphemPropFactory.cpp
    plugin/GmatPluginFunctions.cpp
    propagator/EphemerisPropagator.cpp
	propagator/CcsdsEphPropagator.cpp
    propagator/Code500Propagator.cpp
    propagator/SPKPropagator.cpp
    propagator/StkEPropagator.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "EPHEM_PROPAGATOR_EXPORTS")
