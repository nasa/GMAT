# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Project
# This file must be installed in the GMAT source directory.
# That is, we should have the directory structure:
#   ./base 
#   ./console
#   ./gui
#
# Original Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
# Modified for CMake standardization by: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# Variable used to collect a list of all src targets
SET(SrcTargets "")

# ====================================================================
# GMAT util library
SET(SRCDIR "gmatutil")
ADD_SUBDIRECTORY(${SRCDIR})
GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${SRCDIR} DEFINITION TargetName)
SET(SrcTargets ${SrcTargets} ${tmp})

# ====================================================================
# GMAT base library
SET(SRCDIR "base")
ADD_SUBDIRECTORY(${SRCDIR})
GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${SRCDIR} DEFINITION TargetName)
SET(SrcTargets ${SrcTargets} ${tmp})

# ====================================================================
# Console binary
SET(SRCDIR "console")
ADD_SUBDIRECTORY(${SRCDIR})
GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${SRCDIR} DEFINITION TargetName)
SET(SrcTargets ${SrcTargets} ${tmp})

# ====================================================================
# CSALT library and tester
if (GMAT_INCLUDE_CSALT)
   SET(SRCDIR "csalt")
   ADD_SUBDIRECTORY(${SRCDIR})
   GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${SRCDIR} DEFINITION TargetName)
   SET(SrcTargets ${SrcTargets} ${tmp})

   if(GMAT_INCLUDE_CSALT_TESTPROGRAM)   
      SET(SRCDIR "csaltTester")
      ADD_SUBDIRECTORY(${SRCDIR})
      GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${SRCDIR} DEFINITION TargetName)
      SET(SrcTargets ${SrcTargets} ${tmp})
   endif()
endif()

# ====================================================================
# GUI binary
if(GMAT_INCLUDE_GUI)
   SET(SRCDIR "gui")
   ADD_SUBDIRECTORY(${SRCDIR})
   GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${SRCDIR} DEFINITION TargetName)
   SET(SrcTargets ${SrcTargets} ${tmp})
endif()

# ====================================================================
# Bulk changes to the src targets can be made here, using the
# SET_TARGET_PROPERTIES(${SrcTargets} ...) function. This is cleaner
# than applying the target properties at each subdirectory.
SET_TARGET_PROPERTIES(${SrcTargets} PROPERTIES VERSION ${GMAT_VERSION})
SET_TARGET_PROPERTIES(${SrcTargets} PROPERTIES FOLDER "GMAT Core")
