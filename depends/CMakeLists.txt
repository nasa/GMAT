# $Id$
#
# GMAT: General Mission Analysis Tool.
#
# CMake script that manages dependency download for GMAT
#
# Original Author: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# Set directory used for all dependency intermediate build products
SET(GMAT_DEP_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")

###############################################################################
# Include subdirectories that manage individual dependencies
###############################################################################

INCLUDE(CMakeModules/GmatBoostConfig.cmake)
