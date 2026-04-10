# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT CInterface Plugin.
# This file must be installed in the plugins/CInterfacePlugin/src directory
#
# Author: Josh Fisher
# Modified by Ravi Mathur: Extensive CMake build system overhaul
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT CInterface Plugin setup " ${VERSION}) 

SET(TargetName CInterface)

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory include plugin)

# ====================================================================
# specify source files to be compiled
SET(PLUGIN_SRCS
    command/PrepareMissionSequence.cpp
    factory/CCommandFactory.cpp
    plugin/CInterfacePluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.

# FORCE the plugin to install directly into bin
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" bin FORCE)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "CINTERFACE_EXPORTS")

# Set RPATH to find shared libraries in default locations on Mac/Linux
if(UNIX)
  if(APPLE)
    SET(MAC_BASEPATH "../${GMAT_MAC_APPBUNDLE_PATH}/Frameworks/")
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/${MAC_BASEPATH}"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/"
      )
  endif()
endif()

# Override debug output directory
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  LIBRARY_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  )

# ====================================================================
# Setup installation of CInterface Matlab scripts (opengmat, closegmat, etc.)
INSTALL( DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../matlab/"
  DESTINATION ${CMAKE_INSTALL_PREFIX}/matlab/libCInterface
  USE_SOURCE_PERMISSIONS
  PATTERN ".*" EXCLUDE  # e.g. .gitignore, .ds_store, etc...
  )

# Create the CInterface Matlab thunk files if Matlab exists
if(Matlab_FOUND)
  # Create the command to be executed by Matlab
  SET(MSTART "\"${Matlab_MAIN_PROGRAM}\"")
  if(WIN32)
    # Need -wait on Windows so matlab call doesn't immediately return
    SET(MSTART "${MSTART} -wait")
  endif()
  SET(MSTART "${MSTART} -nosplash -nodesktop -nodisplay -r")
  SET(MPATH1 "addpath('${CMAKE_INSTALL_PREFIX}/bin');")
  SET(MPATH2 "addpath('${CMAKE_INSTALL_PREFIX}/matlab/libCInterface');")
  SET(MCD "cd('${CMAKE_CURRENT_SOURCE_DIR}/../build/matlab');")
  SET(MDEBUG "debugsuffix='${CMAKE_DEBUG_POSTFIX}';")
  SET(MPREP "prepareInterface_cmake;")
  SET(MEXIT "exit;")
  SET(MATLAB_COMMAND "${MSTART} \"${MPATH1} ${MPATH2} ${MCD} ${MPREP} ${MEXIT}\"")
  SET(MATLAB_COMMAND_DEBUG "${MSTART} \"${MPATH1} ${MPATH2} ${MCD} ${MDEBUG} ${MPREP} ${MEXIT}\"")

  # Set up install commands for debug/release configurations.
  # On single-config generators (e.g. make), simply check the current config.
  # On multi-config generators (e.g. VS, XCode), there is a CMake deficiency
  # that does not allow per-generator INSTALL(CODE ...) commands. The
  # workaround for this is to try creating both release and debug versions
  # of the CInterface Matlab thunk files. The Matlab prepareInterface script
  # will gently exit if it couldn't build either one (won't error out).

  # Matlab command for multi-config or release builds
  if( CMAKE_CONFIGURATION_TYPES OR (CMAKE_BUILD_TYPE STREQUAL Release) OR (CMAKE_BUILD_TYPE STREQUAL MinSizeRel) OR (CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo) )
    # Matlab command for release configurations
    MESSAGE(STATUS "Will install CInterface (release) Matlab thunk files with command:\n${MATLAB_COMMAND}")
    INSTALL(CODE "MESSAGE(STATUS \"Launching Matlab to build CInterface (release) thunk files...patience is a virtue...\")")
    INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${MATLAB_COMMAND} WORKING_DIRECTORY \"${CMAKE_INSTALL_PREFIX}/bin\" TIMEOUT 30)")
  endif()

  # Matlab command for multi-config or debug builds
  if( CMAKE_CONFIGURATION_TYPES OR (CMAKE_BUILD_TYPE STREQUAL Debug) )
    # Matlab command for debug configurations
    MESSAGE(STATUS "Will install CInterface (debug) Matlab thunk files with command:\n${MATLAB_COMMAND_DEBUG}")
    INSTALL(CODE "MESSAGE(STATUS \"Launching Matlab to build CInterface (debug) thunk files...patience is a virtue...\")")
    INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${MATLAB_COMMAND_DEBUG} WORKING_DIRECTORY \"${CMAKE_INSTALL_PREFIX}/bin\" TIMEOUT 30)")
  endif()

# Warn user if Matlab was not found
else()
  MESSAGE(WARNING "Matlab not found, will not build Matlab prototype files for the GMAT CInterface Plugin.")
endif()
