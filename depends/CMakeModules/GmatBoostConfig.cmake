# $Id$
#
# GMAT: General Mission Analysis Tool.
#
# CMake script that manages the Boost dependency for GMAT
#
# Original Author: Ravi Mathur
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

###############################################################################
# Only download Boost if needed

IF(NOT GMAT_USE_BOOST)
  RETURN()
ENDIF()

###############################################################################
# Download Boost (currently only using the headers)

INCLUDE(FetchContent)

# Set default Boost version, but allow developer to override
SET(Boost_VERSION 1.71.0)
SET(Boost_VERSION_OVERRIDE "" CACHE STRING "(Optional) Override GMAT's default Boost version (currently ${Boost_VERSION}). Leave empty to use the default.")
IF(Boost_VERSION_OVERRIDE)
  SET(Boost_VERSION ${Boost_VERSION_OVERRIDE})
ENDIF()

# Convert Boost version to a file suffix
# Assumes the Boost project continues its established naming convention, i.e. 1.71.0 -> 1_71_0
STRING(REPLACE "." "_" Boost_SUFFIX ${Boost_VERSION})

# Use verbose download if desired
IF(FETCHCONTENT_QUIET)
  SET(QUIET "QUIET")
ENDIF()

# Download Boost
MESSAGE(STATUS "Downloading Boost ${Boost_VERSION} ...")
SET(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Boost_${Boost_SUFFIX})
FetchContent_Populate(boost
  # Use .tar.bz2 because .7z doesn't seem to extract properly (as of Boost 1.71.0)
  URL https://archives.boost.io/release/${Boost_VERSION}/source/boost_${Boost_SUFFIX}.tar.bz2
  SOURCE_DIR ${BOOST_ROOT}
  BINARY_DIR ${GMAT_DEP_BUILD_DIR}/boost
  SUBBUILD_DIR ${GMAT_DEP_BUILD_DIR}/boost-subbuild
  ${QUIET}
)

###############################################################################
# Load Boost

# GMAT currently only uses Boost headers
SET(Boost_USE_DEBUG_LIBS OFF)   # Don't search for Boost debug libs
SET(Boost_USE_RELEASE_LIBS OFF) # Don't search for Boost release libs
SET(Boost_NO_BOOST_CMAKE ON)    # Don't search for Boost package config file
SET(Boost_NO_SYSTEM_PATHS ON)   # Don't search for Boost in system-wide locations
FIND_PACKAGE(Boost ${Boost_VERSION} EXACT REQUIRED) # Will look in BOOST_ROOT
