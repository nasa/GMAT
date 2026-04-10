# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMake template script for GMAT plugins
#
# Modify this script as needed for your own plugin, and place it in your
# plugin's src/base directory.
#
# GMAT maintainers should update this template as appropriate to reflect
# new build system capabilities.
#
# Instructions:
#  - Create your plugin according to standard GMAT plugin conventions
#  - Copy this file to your plugin's src/base directory, name it CMakeLists.txt
#  - Follow the comment instructions below. You can use CMakeLists.txt files
#    from existing GMAT plugins as a guide.
#  - Update the <GMAT>/plugins/CMakeLists.txt file by adding references
#    to this plugin at appropriate places.
#  
# Author: Ravi Mathur (4/18/2015)
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# ====================================================================
MESSAGE("==============================")
MESSAGE("GMAT EKF Plugin setup " ${VERSION}) 

SET(TargetName EKF)

# ====================================================================
# More definitions can be added here if needed

# ====================================================================
# Specify each directory containing source/header files
# Separate each directory with a space
# Make sure to include subdirectories if there are any
SET(PLUGIN_DIRS command EKF estimatedparam factory include noise plugin smoother)

# ====================================================================
# Specify each source file to be compiled for your plugin
# The path to each file should be given relative to your plugin's
# src/base directory (where this CMakeLists.txt file should be)
SET(PLUGIN_SRCS
    command/RunSmoother.cpp
    EKF/ExtendedKalmanFilter.cpp
    EKF/SeqEstimator.cpp
    estimatedparam/EstimatedParameter.cpp
    estimatedparam/EstimatedParameterModel.cpp
    estimatedparam/FirstOrderGaussMarkov.cpp
    factory/EKFCommandFactory.cpp
    factory/ExtendedKalmanFilterFactory.cpp
    factory/EstimatedParameterFactory.cpp
    factory/EstimatedParameterModelFactory.cpp
    factory/ProcessNoiseFactory.cpp
    factory/ProcessNoiseModelFactory.cpp
    factory/SmootherFactory.cpp
    noise/LinearProcessNoise.cpp
    noise/ProcessNoiseBase.cpp
    noise/ProcessNoiseModel.cpp
    noise/SNCProcessNoise.cpp
    plugin/GmatPluginFunctions.cpp
    smoother/Smoother.cpp
    smoother/SmootherBase.cpp
)

# ====================================================================
# This CMake macro (defined in <GMAT>/plugins/CMakeLists.txt) sets up
# definitions and variables common to all plugins.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "KALMAN_EXPORTS")

# ====================================================================
# Additional link libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatEstimation)
