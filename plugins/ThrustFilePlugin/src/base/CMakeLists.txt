# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT ThrustFile Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/ThrustFilePlugin/src/base directory
#
# Author: Darrel Conway
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT ThrustFile Plugin setup " ${VERSION}) 

SET(TargetName ThrustFile)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command datareader factory forcemodel include plugin parameter)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/BeginFileThrust.cpp
    command/EndFileThrust.cpp
    datareader/ThfDataSegment.cpp
    datareader/ThrustHistoryFile.cpp
    datareader/ThrustSegment.cpp
    factory/ThrustFileCommandFactory.cpp
    factory/ThrustFileForceFactory.cpp
    factory/ThrustFileReaderFactory.cpp
	factory/ThrustSegmentParameterFactory.cpp
    forcemodel/FileThrust.cpp
    plugin/GmatPluginFunctions.cpp
	parameter/ThrustSegmentData.cpp
	parameter/ThrustSegmentParameters.cpp
	parameter/ThrustSegmentReal.cpp
	parameter/ThrustSegmentRvector.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preprocessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "THRUSTFILE_EXPORTS")

# ====================================================================
# Additional link libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE DataInterface)
