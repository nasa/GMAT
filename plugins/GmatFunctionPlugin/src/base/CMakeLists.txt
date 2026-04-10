# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT GmatFunction Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/GmatFunctionPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT GmatFunction Plugin setup " ${VERSION}) 

SET(TargetName GmatFunction)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory function include plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/CallGmatFunction.cpp
    command/Global.cpp
    factory/GmatFunctionCommandFactory.cpp
    factory/GmatFunctionFactory.cpp
    function/GmatFunction.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "GMATFUNCTION_EXPORTS")
