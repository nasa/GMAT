# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT PolyhedronGravity Plugin.
# (Adapted from the CInterface plugin CMake script)
# This file must be installed in the plugins/PolyhedronGravityPlugin/src directory
#
# Author: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT PolyhedronGravity Plugin setup " ${VERSION}) 

SET(TargetName PolyhedronGravity)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory gravitymodel include parameter plugin)

# ====================================================================
# source files
SET(PLUGIN_SRCS
  factory/PolyhedronGravityModelFactory.cpp
  factory/GravityParameterFactory.cpp
  gravitymodel/PolyhedronBody.cpp
  gravitymodel/PolyhedronGravityModel.cpp
  parameter/GravData.cpp
  parameter/GravReal.cpp
  parameter/SurfaceHeight.cpp
  plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "POLYHEDRONGRAVITYMODEL_EXPORTS")
