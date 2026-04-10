# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for Thinking Systems' port of Vallado's SGP4 propagator
#
# Author: Darrel Conway
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!

# GMAT plugins from Thinking Systems
OPTION(PLUGIN_ThinkSys_TLEPropagator "A GMAT TLE propagator based on the SPICE implementation of Vallado's SGP4 code" ON)

# Variable used to collect a list of all enabled plugins
SET(PluginTargets "")

# ====================================================================
# SGP4 Propagator Plugin
if(PLUGIN_ThinkSys_TLEPropagator)
  SET(PLUGINDIR "src")
  ADD_SUBDIRECTORY(${PLUGINDIR})
  GET_DIRECTORY_PROPERTY(tmp DIRECTORY ${PLUGINDIR} DEFINITION TargetName)
  SET(PluginTargets ${PluginTargets} ${tmp})
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
