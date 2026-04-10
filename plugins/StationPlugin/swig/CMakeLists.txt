# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Ground Station SWIG interface
# This file must be installed in the plugins/StationPlugin/swig directory
#
# Author: Darrel Conway
# 
MESSAGE("==============================")
if (_GMATAPI_GENERATE_PYTHON)
  MESSAGE("STATION SWIG setup (for Python ${GMATAPI_Python3_Version})" ${VERSION})
endif()
if (_GMATAPI_GENERATE_JAVA)
  MESSAGE("STATION SWIG setup (for Java ${API_JAVA_TARGET_VERSION})" ${VERSION})
endif()

INCLUDE(${PROJECT_SOURCE_DIR}/build/cmake_modules/GmatSwigConfig.cmake)

if (_GMATAPI_GENERATE_PYTHON)
  if(NOT Python3_Development_FOUND)
    RETURN()
  endif()
  
  SET(GMAT_API_TARGETNAME "gmat${API_PYTHON_SUFFIX}${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
  _SETUPSWIG("station" "Python" "station_py.i" "Station;${GMAT_API_TARGETNAME}")
  ADD_DEPENDENCIES(${TargetNameFull} ${GMAT_API_TARGETNAME} Station)
  TARGET_LINK_LIBRARIES(${TargetNameFull} Station)
endif()

if (_GMATAPI_GENERATE_JAVA)
  # Get location of gmat.jar
  SET(OUTPUT_DIR $<$<CONFIG:Release>:${GMAT_BUILDOUTPUT_DIRECTORY}/bin>$<$<CONFIG:Debug>:${GMAT_BUILDOUTPUT_DEBUGDIR}>)
  SET(SWIG_EXTRA_JAR_FILES "${OUTPUT_DIR}/gmat.jar")

  _SETUPSWIG("station" "Java" "station.i" "Station;gmat${API_JAVA_SUFFIX}")
  ADD_DEPENDENCIES(station${API_JAVA_SUFFIX} gmat${API_JAVA_SUFFIX} Station)
  TARGET_LINK_LIBRARIES(station${API_JAVA_SUFFIX} Station)
endif()
