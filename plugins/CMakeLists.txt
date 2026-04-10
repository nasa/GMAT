# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Project
# This file must be installed in the GMAT plugins directory
# That is, we should have the directory structure:
#   ./CInterfacePlugin
#   ./DataInterfacePlugin
#   etc... other plugins
#
# Original Author: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

# Convenience macro used by plugins to enable common functionality
# Inputs:
#   TargetName:  Name of plugin target
#   PLUGIN_DIRS: List of directories containing source/header files
#   PLUGIN_SRCS: List of .c and .cpp source files
#   INSTALL_DIR: Directory to install plugin (plugins, bin, etc.)
# Note that all paths should be relative to the plugin's CMakeLists.txt
MACRO(_SETUPPLUGIN TargetName PLUGIN_DIRS PLUGIN_SRCS INSTALL_DIR)
  # Add Preproccessor Definitions
  ADD_DEFINITIONS("-D_DYNAMICLINK")

  # ====================================================================
  # Recursively find all include files in specified directories
  FOREACH(I_DIR ${PLUGIN_DIRS})
    FILE(GLOB_RECURSE IDIR_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${I_DIR}/*.hpp ${I_DIR}/*.h)
    SET(PLUGIN_HEADERS ${PLUGIN_HEADERS} ${IDIR_HEADERS})
  ENDFOREACH()

  # ====================================================================
  # add the install targets
  # headers are included here for IDE-based projects (VS, XCode, etc.)
  ADD_LIBRARY(${TargetName} SHARED ${PLUGIN_SRCS} ${PLUGIN_HEADERS})

  # ====================================================================
  # prepend include directories for header files
  TARGET_INCLUDE_DIRECTORIES(${TargetName} BEFORE PUBLIC ${PLUGIN_DIRS})

  # ====================================================================
  # Add source/header files to IDE-based project source groups
  # Macro defined in top-level CMakeLists.txt
  _ADDSOURCEGROUPS("${PLUGIN_DIRS}")

  # ====================================================================
  # Create build products in specified directory
  # Macro defined in top-level CMakeLists.txt
  _SETOUTPUTDIRECTORY(${TargetName} ${INSTALL_DIR})

  # ====================================================================
  # All plugins should start with "lib". This is always true for Mac/Linux, 
  # but needs to be specified for Windows.
  SET_TARGET_PROPERTIES(${TargetName} PROPERTIES PREFIX "lib")

  # ====================================================================
  # Plugin label should include "Plugin" in IDEs (e.g. VisualStudio)
  SET_TARGET_PROPERTIES(${TargetName} PROPERTIES PROJECT_LABEL "Plugin ${TargetName}")

  # ====================================================================
  # All plugins link to GmatUtil and GmatBase
  TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatUtil)
  TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatBase)

  # ====================================================================
  # Specify where to install plugin
  # - On Windows the dll is RUNTIME (lib is ARCHIVE, ignored here)
  # - On Mac/Linux the dylib/so is LIBRARY
  if(WIN32)
    INSTALL(TARGETS ${TargetName} RUNTIME DESTINATION ${INSTALL_DIR})
  else()
    if(APPLE AND NOT ("${ARGN}" STREQUAL "FORCE"))
      set(FULL_INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/${INSTALL_DIR})
    else()
      set(FULL_INSTALL_DIR ${INSTALL_DIR})
    endif()
    INSTALL(TARGETS ${TargetName} LIBRARY DESTINATION ${FULL_INSTALL_DIR})
  endif()
  
  # ====================================================================
  # On Mac, create symlink to plugin in user-level plugins folder
  if(APPLE AND NOT ("${ARGN}" STREQUAL "FORCE"))
    # Make sure user-level plugin folder exists
    INSTALL(DIRECTORY DESTINATION "${INSTALL_DIR}")
    
    # Get relative path from user-level plugin folder to this plugin's file
    SET(TARGET_SYMLINK_FULLPATH "${CMAKE_INSTALL_PREFIX}/${INSTALL_DIR}")
    SET(TARGET_LIB_FULLFILE "${CMAKE_INSTALL_PREFIX}/${FULL_INSTALL_DIR}/$<TARGET_FILE_NAME:${TargetName}>")
    FILE(RELATIVE_PATH TARGET_RELPATH
      "${TARGET_SYMLINK_FULLPATH}"
      "${TARGET_LIB_FULLFILE}"
    )
    
    # Create symlink
    INSTALL(CODE "EXECUTE_PROCESS( \
                  COMMAND ${CMAKE_COMMAND} -E create_symlink \
                  ${TARGET_RELPATH} \
                  ${TARGET_SYMLINK_FULLPATH}/$<TARGET_FILE_PREFIX:${TargetName}>$<TARGET_FILE_BASE_NAME:${TargetName}>$<TARGET_FILE_SUFFIX:${TargetName}> \
                  )"
    )
  endif()

ENDMACRO(_SETUPPLUGIN)

# ====================================================================
# Declare all public plugins and their default values
# FOR NEW PLUGINS: Add a new line here with the plugin name
#                  Insert new OPTION command alphabetically for readability
# OPTION(PLUGIN_<PLUGINNAME> "<PluginName> Plugin" ON|OFF)
OPTION(PLUGIN_CINTERFACE "CInterface Plugin" OFF)
OPTION(PLUGIN_DATAINTERFACE "DataInterface Plugin" ON)
OPTION(PLUGIN_DATACALLBACK "DataCallback Plugin" OFF)
OPTION(PLUGIN_EPHEMPROPAGATOR "EphemPropagator Plugin" ON)
OPTION(PLUGIN_EKF "EKF Plugin" ON)
OPTION(PLUGIN_ESTIMATION "Estimation Plugins" ON)
OPTION(PLUGIN_EVENTLOCATOR "EventLocator Plugin" ON)
OPTION(PLUGIN_EXTERNALFORCEMODEL "ExternalForceModel Plugin" ON)
OPTION(PLUGIN_EXTRAPROPAGATORS "ExtraPropagators Plugin" ON)
OPTION(PLUGIN_FMINCONOPTIMIZER "FminconOptimizer Plugin" OFF)
OPTION(PLUGIN_FORMATION "Formation Plugin" ON)
OPTION(PLUGIN_GEOMETRICMEASUREMENT "GeometricMeasurement Plugin" OFF)
OPTION(PLUGIN_GMATFUNCTION "GmatFunction Plugin" ON)
OPTION(PLUGIN_MATLABINTERFACE "MatlabInterface Plugin" ON)
OPTION(PLUGIN_MSISE00ATMOSPHERE "MSISE00 Atmosphere Plugin" ON)
OPTION(PLUGIN_NEWPARAMETERS "NewParameter Plugin" ON)
OPTION(PLUGIN_OVTOOFI "OVtoOFI Plugin" ON)
OPTION(PLUGIN_POLYHEDRONGRAVITY "PolyhedronGravity Plugin" ON)
OPTION(PLUGIN_PRODUCTIONPROPAGATORS "ProductionPropagators Plugin" ON)
OPTION(PLUGIN_PYTHONINTERFACE "PythonInterface Plugin" ON)
OPTION(PLUGIN_SAVECOMMAND "SaveCommand Plugin" ON)
OPTION(PLUGIN_SCRIPTTOOLS "ScriptTools Plugin" ON)
OPTION(PLUGIN_STATION "Station Plugin" ON)
OPTION(PLUGIN_THRUSTFILE "ThrustFile Plugin" ON)
OPTION(PLUGIN_TLEPROPAGATOR "TLEPropagator Plugin" ON)
OPTION(PLUGIN_YUKONOPTIMIZER "YukonOptimizer Plugin" ON)

# Variable used to collect a list of all enabled plugins
SET(PluginTargets "")

# ====================================================================
# FOR NEW PLUGINS: This code adds your plugin to the build system
# Insert this code in alphabetical order between the plugins below
#if (PLUGIN_<PLUGINNAME>) # Use your plugin's name
#  SET(PLUGINDIR "<PluginDir>/src/base") # Use your plugin's directory name
#  ADD_SUBDIRECTORY(${PLUGINDIR}) 
#  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
#  SET(PluginTargets ${PluginTargets} ${tmp})
#endif()

# ====================================================================
# CInterface Plugin
if (PLUGIN_CINTERFACE)
  SET(PLUGINDIR "CInterfacePlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# DataInterface Plugin
if (PLUGIN_DATAINTERFACE)
  SET(PLUGINDIR "DataInterfacePlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# DataCallback Plugin
if (PLUGIN_DATACALLBACK)
  SET(PLUGINDIR "DataCallbackPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# EphemPropagator Plugin
if (PLUGIN_EPHEMPROPAGATOR)
  SET(PLUGINDIR "EphemPropagatorPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# EKF Plugin
if (PLUGIN_EKF)
  SET(PLUGINDIR "ExtendedKalmanFilterPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Estimation Plugin
if (PLUGIN_ESTIMATION)
  SET(PLUGINDIR "EstimationPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# EventLocator Plugin
if (PLUGIN_EVENTLOCATOR)
  SET(PLUGINDIR "EventLocatorPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# ExternalModel Plugin
if (PLUGIN_EXTERNALFORCEMODEL)
foreach(ExternalForceModel_Python3_Version ${GMAT_PYTHON3_VERSIONS})
  SET(PLUGINDIR "ExternalForceModelPlugin/src/base")
  SET(PLUGINBINDIR "${PLUGINDIR}/py${ExternalForceModel_Python3_Version}")
  ADD_SUBDIRECTORY(${PLUGINDIR} ${PLUGINBINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${PLUGINBINDIR}" DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endforeach()
endif()

# ====================================================================
# ExtraPropagators Plugin
if (PLUGIN_EXTRAPROPAGATORS)
  SET(PLUGINDIR "ExtraPropagatorsPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# FminconOptimizer Plugin
if (PLUGIN_FMINCONOPTIMIZER)
  SET(PLUGINDIR "FminconOptimizerPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Formation Plugin
if (PLUGIN_FORMATION)
  SET(PLUGINDIR "FormationPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# GeometricMeasurement Plugin
if (PLUGIN_GEOMETRICMEASUREMENT)
  SET(PLUGINDIR "GeometricMeasurementPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# GmatFunction Plugin
if (PLUGIN_GMATFUNCTION)
  SET(PLUGINDIR "GmatFunctionPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# MatlabInterface Plugin
if (PLUGIN_MATLABINTERFACE)
  SET(PLUGINDIR "MatlabInterfacePlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Msise00 Plugin
if (PLUGIN_MSISE00ATMOSPHERE)
  SET(PLUGINDIR "Msise00Plugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# NewParameters Plugin
if (PLUGIN_NEWPARAMETERS)
  SET(PLUGINDIR "NewParameterPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Look for the OVtoOFI Plugin at the end!

# ====================================================================
# PolyhedronGravity Plugin
if (PLUGIN_POLYHEDRONGRAVITY)
  SET(PLUGINDIR "PolyhedronGravityPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# ProductionPropagators Plugin
if (PLUGIN_PRODUCTIONPROPAGATORS)
  SET(PLUGINDIR "ProductionPropagatorPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# PythonInterface Plugin
# Loop over multiple Python versions and create a plugin for each one
if (PLUGIN_PYTHONINTERFACE)
foreach(PythonInterface_Python3_Version ${GMAT_PYTHON3_VERSIONS})
  SET(PLUGINDIR "PythonInterfacePlugin/src/base")
  SET(PLUGINBINDIR "${PLUGINDIR}/py${PythonInterface_Python3_Version}")
  ADD_SUBDIRECTORY(${PLUGINDIR} ${PLUGINBINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${PLUGINBINDIR}" DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endforeach()
endif()

# ====================================================================
# SaveCommand Plugin
if (PLUGIN_SAVECOMMAND)
  SET(PLUGINDIR "SaveCommandPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# ScriptTools Plugin
if (PLUGIN_SCRIPTTOOLS)
  SET(PLUGINDIR "ScriptToolsPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Station Plugin
if (PLUGIN_STATION)
  SET(PLUGINDIR "StationPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# ThrustFile Plugin
if (PLUGIN_THRUSTFILE)
  SET(PLUGINDIR "ThrustFilePlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# TLEPropagator Plugin
if (PLUGIN_TLEPROPAGATOR)
  SET(PLUGINDIR "TLEPropagatorPlugin/src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()


# ====================================================================
# YukonOptimizer Plugin
if (PLUGIN_YUKONOPTIMIZER)
  SET(PLUGINDIR "YukonOptimizerPlugin/src/base")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
endif()

# ====================================================================
# Add built-in plugins to a common folder for IDE-based projects
SET_TARGET_PROPERTIES(${PluginTargets} PROPERTIES FOLDER "GMAT Plugins")

# ====================================================================
# SWIG Libraries for Plugins
if (GMAT_INCLUDE_API)  
  if(API_GENERATE_PYTHON)
    SET(_GMATAPI_GENERATE_JAVA FALSE)
    SET(_GMATAPI_GENERATE_PYTHON TRUE)
    
    foreach(GMATAPI_Python3_Version ${GMAT_PYTHON3_VERSIONS})
      # Station Plugin
      if (PLUGIN_STATION)
        SET(PLUGINDIR "StationPlugin/swig")
        SET(PLUGINBINDIR "${PLUGINDIR}/py${GMATAPI_Python3_Version}")
        ADD_SUBDIRECTORY(${PLUGINDIR} ${PLUGINBINDIR})
      endif()

      # Navigation Plugin
      if (PLUGIN_ESTIMATION)
        SET(PLUGINDIR "EstimationPlugin/swig")
        SET(PLUGINBINDIR "${PLUGINDIR}/py${GMATAPI_Python3_Version}")
        ADD_SUBDIRECTORY(${PLUGINDIR} ${PLUGINBINDIR})
      endif()
    endforeach()
  endif()
  
  if(API_GENERATE_JAVA)
    SET(_GMATAPI_GENERATE_JAVA TRUE)
    SET(_GMATAPI_GENERATE_PYTHON FALSE)
    
    # Station Plugin
    if (PLUGIN_STATION)
      ADD_SUBDIRECTORY("StationPlugin/swig" "StationPlugin/swig/java")
    endif()

    # Navigation Plugin
    if (PLUGIN_ESTIMATION)
      ADD_SUBDIRECTORY("EstimationPlugin/swig" "EstimationPlugin/swig/java")
    endif()
  endif()
endif()

# ====================================================================
# Proprietary plugins
#
# Search for proprietary plugin folder in default paths
# User can change this via CMake GUI or command line
FIND_PATH(GMAT_PROPRIETARYPLUGINS_PATH
  NAMES code/CMakeLists.txt
  PATHS
    ${PROJECT_SOURCE_DIR}/../gmatinternal-git
    ${PROJECT_SOURCE_DIR}/plugins/gmatinternal-git
  DOC "Path to GMAT proprietary plugins (containing code/)"
  NO_DEFAULT_PATH
  )

# Load CMakeLists.txt file from proprietary plugins /code folder
if(GMAT_PROPRIETARYPLUGINS_PATH)
  SET(PLUGINDIR "${GMAT_PROPRIETARYPLUGINS_PATH}/code")
  ADD_SUBDIRECTORY(${PLUGINDIR} "${CMAKE_CURRENT_BINARY_DIR}/proprietary")
  GET_DIRECTORY_PROPERTY(ProprietaryTargets DIRECTORY ${PLUGINDIR} DEFINITION PluginTargets)
  SET(PluginTargets ${PluginTargets} ${ProprietaryTargets})
  SET_TARGET_PROPERTIES(${ProprietaryTargets} PROPERTIES FOLDER "GMAT Plugins/Proprietary")
endif()

# ====================================================================
# Plugins from Other Sources
#
# Add Additional Vendor or Internal Plugins to the GMAT_ADDITIONAL_PLUGINS file
# The file should contain one plugin name or name=value pair per line
#   Line format: PluginName[=PluginDirectory]
FIND_FILE(GMAT_ADDITIONAL_PLUGINS "ListOfAdditionalPlugins.txt"
   PATHS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}
   DOC "Path to file that lists third-party plugins, with file contents: PluginName [= PluginDirectory]"
   NO_DEFAULT_PATH)
   
if (GMAT_ADDITIONAL_PLUGINS)
   FILE(STRINGS ${GMAT_ADDITIONAL_PLUGINS} AdditionalPluginsNVP)

   foreach(CurrPluginNVP ${AdditionalPluginsNVP})
      # Extract plugin name and directory (if provided)
      STRING(STRIP ${CurrPluginNVP} CurrPluginNVP) # Strip leading/trailing spaces
      STRING(REGEX MATCH "^[^=]+" CurrPluginName ${CurrPluginNVP}) # Get plugin name
      if(CurrPluginName STREQUAL CurrPluginNVP) # No directory provided
         SET(CurrPluginDir "")
      else()
         STRING(REPLACE "${CurrPluginName}=" "" CurrPluginDir ${CurrPluginNVP}) # Get plugin directory
      endif()
      
      # Strip leading/trailing space from plugin name and directory
      STRING(STRIP ${CurrPluginName} CurrPluginName)
      STRING(STRIP "${CurrPluginDir}" CurrPluginDir)

      # Assemble search paths based on specified plugin directory
      if(NOT CurrPluginDir)
         # No directory provided -> search for plugin next to GMAT directory
         SET(CurrSearchPaths "${PROJECT_SOURCE_DIR}/../${CurrPluginName}")
      elseif(IS_ABSOLUTE ${CurrPluginDir})
         # Absolute directory path provided -> only search there
         SET(CurrSearchPaths ${CurrPluginDir})
      else()
         # Relative directory path provided -> search relative to provided additional plugins file
         GET_FILENAME_COMPONENT(AdditionalPluginsFileDir ${GMAT_ADDITIONAL_PLUGINS} DIRECTORY)
         SET(CurrSearchPaths "${AdditionalPluginsFileDir}/${CurrPluginDir}")
      endif()
                  
      # Search for plugin folder
      FIND_PATH(GMAT_ADDITIONAL_PLUGINS_${CurrPluginName}
         NAMES CMakeLists.txt
         PATHS ${CurrSearchPaths}
         DOC "Path to additional plugin ${CurrPluginName}"
         NO_DEFAULT_PATH
      )
   
      if(GMAT_ADDITIONAL_PLUGINS_${CurrPluginName})
        SET(PLUGINDIR "${GMAT_ADDITIONAL_PLUGINS_${CurrPluginName}}")
        ADD_SUBDIRECTORY(${PLUGINDIR} "${CMAKE_CURRENT_BINARY_DIR}/${CurrPluginName}")
        GET_DIRECTORY_PROPERTY(AdditionalTargets DIRECTORY ${PLUGINDIR} DEFINITION PluginTargets)
        SET_TARGET_PROPERTIES(${AdditionalTargets} PROPERTIES FOLDER "GMAT Plugins/Additional")
        SET(PluginTargets ${PluginTargets} ${AdditionalTargets})
      endif()
   endforeach()

endif()

# ====================================================================
# OVtoOFI Plugin
if (PLUGIN_OPENFRAMESINTERFACE)
  
   if (PLUGIN_OVTOOFI)
     SET(PLUGINDIR "OVtoOFI/src/base")
     ADD_SUBDIRECTORY(${PLUGINDIR})
     GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
     SET(PluginTargets ${PluginTargets} ${tmp})
   endif()
   
else()

   MESSAGE("======================================")
   MESSAGE("Skipping the OVtoOFI Plugin:")
   MESSAGE("OpenFramesInterface is not being built")
   MESSAGE("=======================================")

endif()

# ====================================================================
# Set plugin properties all at once
if(PluginTargets)
  # Specify the library version
  SET_TARGET_PROPERTIES(${PluginTargets} PROPERTIES VERSION ${GMAT_VERSION})

  # Specify link flags for Win32 targets
  if(WIN32)
    SET_TARGET_PROPERTIES(${PluginTargets} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:\"libcmt.lib\"")
  endif()
endif()
