# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE install script for GMAT Project
# This file must be installed in the GMAT build/install directory
#
# Original Author: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# ====================================================================
# Set overall GMAT directory structure
# executed when running "make install" or building the VS "INSTALL" target

MESSAGE("==============================")
MESSAGE("Setting up install directory structure for GMAT " ${VERSION})

# ====================================================================
# Create the overall GMAT directory structure
MESSAGE(STATUS "Will install GMAT to " ${CMAKE_INSTALL_PREFIX})

# Install common items from selected GMAT build output directory
# Exclude items specific to the developer
# NOTE: If a non-default build output directory is chosen, then
# the developer must ensure all necessary GMAT data/config files
# already exist in there (e.g. startup file, data folder, etc.).
INSTALL( DIRECTORY "${GMAT_BUILDOUTPUT_DIRECTORY}/"
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  USE_SOURCE_PERMISSIONS
  PATTERN ".*" EXCLUDE  # e.g. .gitignore, .ds_store, etc...
  PATTERN "debug" EXCLUDE # don't copy debug directory
  PATTERN "bin" EXCLUDE # don't copy bin directory
  PATTERN "plugins" EXCLUDE # don't copy plugins directory
  PATTERN "lib" EXCLUDE # don't copy lib directory
  PATTERN "data/atmosphere/Msise86_Data" EXCLUDE # don't copy msise86_data directory
  )

# Install gmat_startup_file based on OS and build configuration
IF(UNIX)
  INSTALL( FILES
    ${GMAT_BUILDOUTPUT_DIRECTORY}/bin/gmat_startup_file_mac_linux.txt
    DESTINATION bin
    CONFIGURATIONS Release MinSizeRel RelWithDebInfo
    RENAME gmat_startup_file.txt
    )
ELSE()
  INSTALL( FILES
    ${GMAT_BUILDOUTPUT_DIRECTORY}/bin/gmat_startup_file.txt
    DESTINATION bin
    CONFIGURATIONS Release MinSizeRel RelWithDebInfo
    )
ENDIF()

INSTALL( FILES
  ${GMAT_BUILDOUTPUT_DIRECTORY}/debug/gmat_startup_file_debuginstall.txt
  DESTINATION bin
  CONFIGURATIONS Debug
  RENAME gmat_startup_file.txt
  )

# Install GMAT.ini based on build configuration
INSTALL( FILES
  ${GMAT_BUILDOUTPUT_DIRECTORY}/bin/GMAT.ini
  DESTINATION bin
  CONFIGURATIONS Release MinSizeRel RelWithDebInfo
  )

INSTALL( FILES
  ${GMAT_BUILDOUTPUT_DIRECTORY}/debug/GMAT.ini
  DESTINATION bin
  CONFIGURATIONS Debug
  )

# Install Java API support files
IF(API_GENERATE_JAVA)
  INSTALL( FILES
    ${GMAT_BUILDOUTPUT_DIRECTORY}/bin/load_gmat.m
    DESTINATION bin
    CONFIGURATIONS Release MinSizeRel RelWithDebInfo
    )

  INSTALL( FILES
    ${GMAT_BUILDOUTPUT_DIRECTORY}/debug/load_gmat.m
    DESTINATION bin
    CONFIGURATIONS Debug
    )
ENDIF()

# Install Python API support files
IF(API_GENERATE_PYTHON)
  INSTALL( FILES
    ${GMAT_BUILDOUTPUT_DIRECTORY}/bin/gmatpy/__init__.py
    DESTINATION bin/gmatpy
    CONFIGURATIONS Release MinSizeRel RelWithDebInfo
    )

  INSTALL( FILES
    ${GMAT_BUILDOUTPUT_DIRECTORY}/debug/gmatpy/__init__.py
    DESTINATION bin/gmatpy
    CONFIGURATIONS Debug
    )
ENDIF()

# Install Mac end-user configuration file
IF(APPLE)
  INSTALL( FILES
    ${GMAT_BUILDOUTPUT_DIRECTORY}/bin/MacConfigure.txt
    DESTINATION bin
    )
ENDIF()
