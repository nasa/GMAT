# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT EventLocator Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/EventLocatorPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT EventLocator Plugin setup " ${VERSION}) 

SET(TargetName EventLocator)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS event factory include locator plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    event/ContactEvent.cpp
    event/ContactResult.cpp
    event/EclipseEvent.cpp
    event/EclipseTotalEvent.cpp
	event/IntrusionEvent.cpp
	event/IntrusionResult.cpp
    factory/EventLocatorFactory.cpp
    locator/ContactLocator.cpp
    locator/EclipseLocator.cpp
	locator/IntrusionLocator.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "LOCATOR_EXPORTS")
