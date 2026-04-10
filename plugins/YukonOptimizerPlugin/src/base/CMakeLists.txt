# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT FminconOptimizer Plugin.
# (Adapted from the CInterface plugin CMake script)
#
# Author: Joshua Raymond
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT Yukon Optimizer Plugin setup " ${VERSION}) 

# ====================================================================
# Define plugin target
SET(TargetName YukonOptimizer)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS factory include plugin solver)

# ====================================================================
# source files
SET(PLUGIN_SRCS
    factory/YukonOptimizerFactory.cpp
    plugin/GmatPluginFunctions.cpp
    solver/GmatProblemInterface.cpp
    solver/YukonUserProblem.cpp
    solver/Yukon.cpp
    solver/Yukonad.cpp
    solver/MinQP.cpp
    solver/NLPFunctionGenerator.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "YUKON_EXPORTS")
