# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT Util library
# This file must be installed in the src/base directory
#
# Author: Darrel J. Conway
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT util setup " ${VERSION}) 

SET(TargetName GmatUtil)

# Add preprocessor definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DEXP_STL")

# ====================================================================
# list of directories containing source/header files
SET(UTIL_DIRS
  include
  util
  util/datawriter
  util/interpolator
  util/matrixoperations
  )

# ====================================================================
# source files
SET(UTIL_SRCS
    util/A1Date.cpp
    util/A1Mjd.cpp
    util/AngleUtil.cpp
    util/AttitudeConversionUtility.cpp
    util/AttitudeUtil.cpp
    util/BaseException.cpp
    util/BodyFixedStateConverter.cpp
    util/CalculationUtilities.cpp
    util/CCSDSAEMEulerAngleSegment.cpp
    util/CCSDSAEMQuaternionSegment.cpp
    util/CCSDSAEMReader.cpp
    util/CCSDSAEMSegment.cpp
    util/CCSDSEMReader.cpp
    util/CCSDSEMSegment.cpp
    util/CCSDSEMWriter.cpp
	util/CCSDSEphemerisFile.cpp
    util/CCSDSOEMReader.cpp
    util/CCSDSOEMSegment.cpp
    util/CCSDSOEMWriter.cpp
    util/Code500EphemerisFile.cpp
    util/ColorDatabase.cpp
    util/CubicSpline.cpp
    util/Date.cpp
    util/DateUtil.cpp
    util/ElapsedTime.cpp
    util/Ephemeris.cpp
    util/EopFile.cpp
    util/FileManager.cpp
    util/FileUtil.cpp
    util/GmatGlobal.cpp
    util/GmatTime.cpp
    util/GravityFileUtil.cpp
    util/GregorianDate.cpp
    util/IFileUpdater.cpp
    util/LeapSecsFileReader.cpp
    util/Linear.cpp
    util/MemoryTracker.cpp
    util/MessageInterface.cpp
    util/MessageReceiver.cpp
    util/NumericJacobian.cpp
    util/NPlateHistoryFileReader.cpp
    util/RandomNumber.cpp
    util/RealUtilities.cpp
    util/RgbColor.cpp
    util/Rmatrix33.cpp
    util/Rmatrix66.cpp
    util/Rmatrix.cpp
    util/Rvector3.cpp
    util/Rvector6.cpp
    util/Rvector.cpp
    util/SPADFileReader.cpp
    util/StateConversionUtil.cpp
    util/STKEphemerisFile.cpp
    util/StringTokenizer.cpp
    util/StringUtil.cpp
    util/TextParser.cpp
    util/TimeSystemConverter.cpp
    util/TimeTypes.cpp
    util/UtcDate.cpp
    util/datawriter/DataBucket.cpp
    util/datawriter/DataWriter.cpp
    util/datawriter/DataWriterInterface.cpp
    util/datawriter/DataWriterMaker.cpp
    util/datawriter/WriterData.cpp
    util/interpolator/InterpolatorException.cpp
    util/interpolator/Interpolator.cpp
    util/interpolator/BrentDekkerZero.cpp
    util/interpolator/CubicSplineInterpolator.cpp
    util/interpolator/HermiteInterpolator.cpp
    util/interpolator/LinearInterpolator.cpp
    util/interpolator/NotAKnotInterpolator.cpp
    util/interpolator/LagrangeInterpolator.cpp
    util/matrixoperations/CholeskyFactorization.cpp
    util/matrixoperations/LUFactorization.cpp
    util/matrixoperations/MatrixFactorization.cpp
    util/matrixoperations/QRFactorization.cpp
    util/matrixoperations/SchurFactorization.cpp
    util/Frozen.cpp
    util/OrbitDesignerTime.cpp
    util/RepeatSunSync.cpp
    util/RepeatGroundTrack.cpp
    util/SunSync.cpp
)

# ====================================================================
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE UTIL_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# add the install targets

# Create GmatUtil as a shared library
ADD_LIBRARY(${TargetName} SHARED ${UTIL_SRCS} ${UTIL_HEADERS})
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "GMATUTIL_EXPORTS")

# Add necessary include directories
TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${UTIL_DIRS})
IF(GMAT_USE_BOOST_VARIANT)
  TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${Boost_INCLUDE_DIR})
ENDIF()

# Library name should start with "lib"
# This is always true for Mac/Linux, but needs to be specified for Windows
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES PREFIX "lib")

# ====================================================================
# Add source/header files to IDE-based project source groups
# Macro defined in top-level CMakeLists.txt
_ADDSOURCEGROUPS("${UTIL_DIRS}")

# Windows-specific link flags
if(WIN32)
  SET_TARGET_PROPERTIES(${TargetName} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:\"libcmt.lib\" /INCREMENTAL:NO")
endif()

# Create build products in bin directory
_SETOUTPUTDIRECTORY(${TargetName} bin)

# Override debug output directory
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  LIBRARY_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  )

# Specify where to install (make install or VS "INSTALL" project)
if(WIN32)
  INSTALL(TARGETS ${TargetName} RUNTIME DESTINATION bin)
else()
  if(APPLE)
    SET(INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/Frameworks)
  else()
    SET(INSTALL_DIR bin)
  endif()
  INSTALL(TARGETS ${TargetName} LIBRARY DESTINATION ${INSTALL_DIR})
endif()

# Set RPATH to find shared libraries in default locations on Mac/Linux
if(UNIX)
  if(APPLE)
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/;@loader_path/../plugins/;@loader_path/../plugins/proprietary/"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/;\$ORIGIN/../plugins/;\$ORIGIN/../plugins/proprietary/"
      )
  endif()
endif()
