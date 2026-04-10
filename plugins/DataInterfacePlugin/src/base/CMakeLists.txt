# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT DataInterface Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/DataInterfacePlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT DataInterface Plugin setup " ${VERSION}) 

SET(TargetName DataInterface)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command datainterface datareader factory include plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/Set.cpp
    datainterface/DataInterface.cpp
    datainterface/FileInterface.cpp
    datareader/DataReader.cpp
    datareader/FileReader.cpp
    datareader/TcopsVHFAscii.cpp
    datareader/TcopsVHFData.cpp
    factory/DataInterfaceCommandFactory.cpp
    factory/DataInterfaceFactory.cpp
    factory/ReaderFactory.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "DATAINTERFACE_EXPORTS")
