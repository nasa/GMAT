# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Formation Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/FormationPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT Formation Plugin setup " ${VERSION}) 

SET(TargetName Formation)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory formation include plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/FormationFactory.cpp
    formation/Formation.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "FORMATION_EXPORTS")
