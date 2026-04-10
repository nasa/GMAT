# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT PythonInterface Plugin.
# (Adapted from the MatlabInterface plugin CMake script)
# This file must be installed in the plugins/PythonInterfacePlugin/src/base directory
#
# Author: Darrel Conway
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT PythonInterface Plugin setup (for Python ${PythonInterface_Python3_Version})" ${VERSION})

# ====================================================================
# Major error checks

# Find specified Python3 version, using the specified Python3 root directory
_SETPYTHONROOTDIR(${PythonInterface_Python3_Version})
FIND_PACKAGE(Python3 ${PythonInterface_Python3_Version} EXACT COMPONENTS Development)
if(NOT Python3_Development_FOUND)
  MESSAGE(NOTICE "Skipping PythonInterface: Python ${PythonInterface_Python3_Version} not found")
  _REPORTPYTHONURL(${PythonInterface_Python3_Version})
  RETURN()
endif()

SET(TargetPySuffix "_py${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
SET(TargetName "PythonInterface${TargetPySuffix}")

# ====================================================================
# list of directories containing source/header files
SET(PLUGIN_DIRS command factory function include interface plugin )

# ====================================================================
# source files
SET(PLUGIN_SRCS
    command/CallPythonFunction.cpp
    factory/PythonCommandFactory.cpp
    function/PythonModule.cpp
    interface/PythonInterface.cpp
    plugin/GmatPluginFunctions.cpp
)

# ====================================================================
# Setup common plugin definitions, targets, etc.
_SETUPPLUGIN(${TargetName} "${PLUGIN_DIRS}" "${PLUGIN_SRCS}" plugins)

# Rename DLL export preproccessor definition
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "PYTHON_EXPORTS")

# ====================================================================
# Additional link libraries
# TODO: Currently we link against the Python library (e.g.. libpythonXY.dll), which becomes
#       a runtime link dependency for the PythonInterface plugin. This means the
#       end-user must have the same version of Python installed, e.g. 3.7 or 3.8 or ...
#       Instead we could link against the Python module (Python3::Module), which
#       does not actually link to a Python library during compile. Therefore there
#       is no runtime link dependency, and instead GMAT has to load Python itself.
#       This could be done via a startup file variable (e.g. PYTHONLOCATION), or by
#       looking at the PYTHONHOME environment variable. Investigate options.
#
# NOTE: Doing the above could be troublesome because of the use of Python objects
#       that are omitted from the Python Stable ABI. As a result compiling against
#       one version of Python then running with a different version could crash the
#       Python Interface.
#       Therefore this approach is only safe if the following setting compiles:
#TARGET_COMPILE_DEFINITIONS(${TargetName} PRIVATE Py_LIMITED_API)

# TODO: Link against Python3::Module once Py_LIMITED_API compiles without errors
#       and GMAT provides another method of loading Python at runtime.
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE Python3::Python)
