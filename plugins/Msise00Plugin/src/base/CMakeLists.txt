# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT NRLMsise00 Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/NRLMsise00Plugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT Msise00 Plugin setup " ${VERSION}) 

SET(TargetName Msise00)

# Add Preproccessor Definitions
ADD_DEFINITIONS("-DNRLMSISE00_EXPORTS")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS atmosphere factory include plugin)

# ====================================================================
# f2c includes
INCLUDE_DIRECTORIES(${F2C_DIR})

# ====================================================================
# source files
SET(PLUGIN_SRCS
  atmosphere/NRLMsise00Atmosphere.cpp
  atmosphere/nrlmsise00_sub.c
  factory/NRLMsise00Factory.cpp
  plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)
