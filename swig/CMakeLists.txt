# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT SWIG interface
# This file must be installed in the swig directory
#
# Author: Matthew Handy
#
MESSAGE("==============================")
if(_GMATAPI_GENERATE_PYTHON)
  MESSAGE("GMAT SWIG setup (for Python ${GMATAPI_Python3_Version})" ${VERSION})
endif()
if(_GMATAPI_GENERATE_JAVA)
  MESSAGE("GMAT SWIG setup (for Java ${API_JAVA_TARGET_VERSION})" ${VERSION})
endif()

INCLUDE(${PROJECT_SOURCE_DIR}/build/cmake_modules/GmatSwigConfig.cmake)
  
# Enable big objects for SWIG generated code
if(MSVC)
  SET (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /bigobj")
  SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
  SET (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /bigobj")
endif()

SET(SWIG_IGNORED_WARNINGS "516,509" CACHE STRING "SWIG warnings to ignore when building the API")
string(LENGTH SWIG_IGNORED_WARNINGS WARNSIZE)

if (WARNSIZE GREATER 1)
   SET(CMAKE_SWIG_FLAGS "-w${SWIG_IGNORED_WARNINGS}")
endif()

if(GMAT_USE_BOOST_VARIANT)
   SET(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -DGMAT_USE_BOOST_VARIANT)
endif()

if(_GMATAPI_GENERATE_PYTHON)
   if(NOT Python3_Development_FOUND)
      RETURN()
   endif()

   # Add option to toggle if the debug build of the Python API links to the debug or release Python libraries
   OPTION(API_PYTHON_LINK_DEBUG_LIBRARIES "Link the debug build of the Python API to the debug Python libraries, if available" OFF)

   _SETUPSWIG("gmat" "Python" "gmat_py.i" "GmatUtil;GmatBase")
   ADD_DEPENDENCIES(${TargetNameFull} GmatBase)
   
   # Install Python initialization file for this Python version
   INSTALL( FILES
       ${GMAT_BUILDOUTPUT_DIRECTORY}/bin/gmatpy/__init__.specific.py
       DESTINATION "${INSTALL_USER_DIR}"
       CONFIGURATIONS Release MinSizeRel RelWithDebInfo
       RENAME __init__.py
    )

   INSTALL( FILES
       ${GMAT_BUILDOUTPUT_DIRECTORY}/debug/gmatpy/__init__.specific.py
       DESTINATION "${INSTALL_USER_DIR}"
       CONFIGURATIONS Debug
       RENAME __init__.py
    )
endif()

if(_GMATAPI_GENERATE_JAVA)
   SET(SWIG_EXTRA_JAVA_FILES "${CMAKE_CURRENT_SOURCE_DIR}/gmat_matlab_loadlibrary.java")
   _SETUPSWIG("gmat" "Java" "gmat.i" "GmatUtil;GmatBase")
   ADD_DEPENDENCIES(${TargetNameFull} GmatBase)
endif()

INSTALL(FILES GMATAPI.m DESTINATION bin)
