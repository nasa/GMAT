# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Estimation SWIG interface
# This file must be installed in the plugins/EstimationPlugin/swig directory
#
# Author: Darrel Conway
# 
MESSAGE("==============================")
if(_GMATAPI_GENERATE_PYTHON)
  MESSAGE("ESTIMATION SWIG setup (for Python ${GMATAPI_Python3_Version})" ${VERSION})
endif()
if(_GMATAPI_GENERATE_JAVA)
  MESSAGE("ESTIMATION SWIG setup (for Java ${API_JAVA_TARGET_VERSION})" ${VERSION})
endif()

INCLUDE(${PROJECT_SOURCE_DIR}/build/cmake_modules/GmatSwigConfig.cmake)

if (_GMATAPI_GENERATE_PYTHON)
  if(NOT Python3_Development_FOUND)
    RETURN()
  endif()
  
  SET(GMAT_API_TARGETNAME "gmat${API_PYTHON_SUFFIX}${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
  _SETUPSWIG("navigation" "Python" "navigation_py.i" "GmatEstimation;${GMAT_API_TARGETNAME}")
  ADD_DEPENDENCIES(${TargetNameFull} ${GMAT_API_TARGETNAME} Station)
  TARGET_LINK_LIBRARIES(${TargetNameFull} GmatEstimation)
endif()

if (_GMATAPI_GENERATE_JAVA)
  # Get location of gmat.jar
  SET(OUTPUT_DIR $<$<CONFIG:Release>:${GMAT_BUILDOUTPUT_DIRECTORY}/bin>$<$<CONFIG:Debug>:${GMAT_BUILDOUTPUT_DEBUGDIR}>)
  SET(SWIG_EXTRA_JAR_FILES "${OUTPUT_DIR}/gmat.jar")

  _SETUPSWIG("navigation" "Java" "navigation.i" "GmatEstimation;gmat${API_JAVA_SUFFIX}")
  ADD_DEPENDENCIES(navigation${API_JAVA_SUFFIX} gmat${API_JAVA_SUFFIX} GmatEstimation)
  TARGET_LINK_LIBRARIES(navigation${API_JAVA_SUFFIX} GmatEstimation)
endif()
