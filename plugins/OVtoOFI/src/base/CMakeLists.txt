# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT OVtoOFI Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/OVtoOFI/src directory
#
# Author: Ed Behn
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT OVtoOFI Plugin setup " ${VERSION}) 

SET(TargetName OVtoOFI)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin subscriber)

# ====================================================================
# source files
SET(PLUGIN_SRCS
  factory/OVtoOFIFactory.cpp
  factory/InvisOpenFramesViewFactory.cpp
  subscriber/OVtoOFI.cpp
  plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "OVTOOFI_EXPORTS")

# ====================================================================
# Additional link libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE OpenFramesInterface)
