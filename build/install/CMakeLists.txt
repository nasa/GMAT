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

MESSAGE("==============================")
MESSAGE("Setting up wxWidgets install instructions for GMAT " ${VERSION})

# ====================================================================
# Install wxWidgets libraries
# This should be investigated further, since it's possible to build
# release/debug GMAT using either release/debug wxWidgets. Therefore there
# should be a nice cross-platform way of checking for the version of
# wxWidgets that is actually being used.
if(wxWidgets_FOUND)
  if(WIN32)
    # For Windows, dlls must be next to executables in bin
    INSTALL(DIRECTORY ${wxWidgets_LIB_DIR}/
      DESTINATION bin
      USE_SOURCE_PERMISSIONS
      CONFIGURATIONS Release MinSizeRel RelWithDebInfo
      FILES_MATCHING PATTERN "*u_*.dll" # Only release ("u" suffix)
      PATTERN "msw*" EXCLUDE
      )
    INSTALL(DIRECTORY ${wxWidgets_LIB_DIR}/
      DESTINATION bin
      USE_SOURCE_PERMISSIONS
      CONFIGURATIONS Debug
      FILES_MATCHING PATTERN "*ud_*.dll" # Only debug ("ud" suffix)
      PATTERN "msw*" EXCLUDE
      )
  else()
    # For Mac/Linux, wxlibs are in lib/ or Mac app bundle
    if(APPLE)
      SET(WX_INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/Frameworks)
    else()
      SET(WX_INSTALL_DIR lib)
    endif()

    # Set wxlib type based on debug/release
    # This is needed because it's possible (on Mac/Linux) to build debug
    # GMAT using release wxWidgets
    if(wxWidgets_USE_DEBUG)
      SET(WX_DEBUG_PATTERN "libwx_*ud[_-]*")
    else()
      SET(WX_DEBUG_PATTERN "libwx_*u[_-]*")
    endif()

    INSTALL(DIRECTORY ${wxWidgets_LIBRARY_DIRS}/
      DESTINATION ${WX_INSTALL_DIR}
      USE_SOURCE_PERMISSIONS
      CONFIGURATIONS Release MinSizeRel RelWithDebInfo
      FILES_MATCHING PATTERN "libwx_*u[_-]*"
      PATTERN "wx" EXCLUDE
      )
    INSTALL(DIRECTORY ${wxWidgets_LIBRARY_DIRS}/
      DESTINATION ${WX_INSTALL_DIR}
      USE_SOURCE_PERMISSIONS
      CONFIGURATIONS Debug
      FILES_MATCHING PATTERN "${WX_DEBUG_PATTERN}"
      PATTERN "wx" EXCLUDE
      )

    # On Mac, change dylib install names to use RPATH
    # This is required because wxWidgets uses absolute paths for its library
    # install names, which makes them non-portable.
    if(APPLE)
      # Create command for mac_change_install_names script
      SET(CHNM "./mac_change_install_names.sh")
      SET(ARG1 "\"${CMAKE_INSTALL_PREFIX}/${WX_INSTALL_DIR}\"")
      SET(ARG2 "\"libwx*.dylib\"")
      SET(ARG3 "\"${wxWidgets_LIBRARY_DIRS}\"")
      SET(ARG4 "\"@rpath\"")

      # Collect list of all GMAT components that use wxWidgets
      SET(MAC_BINDIR ${GMAT_MAC_APPBUNDLE_PATH}/Frameworks)
      SET(MAC_GUIDIR ${GMAT_MAC_APPBUNDLE_PATH}/MacOS)
      SET(MAC_PLUGINDIR ${GMAT_MAC_APPBUNDLE_PATH}/plugins)

      # GUI uses wxWidgets
      SET(ARGN "${ARGN} \"${CMAKE_INSTALL_PREFIX}/${MAC_GUIDIR}/GMAT*\"")

      # Most plugins do not use wxWidgets. Nevertheless, we can specify
      # every plugin and the mac_change_install_names script will ignore ones
      # that do not use wxWidgets. This enables future or 3rd-party plugins 
      # that use wxWidgets to have their rpaths automatically corrected.
      SET(ARGN "${ARGN} \"${CMAKE_INSTALL_PREFIX}/${MAC_PLUGINDIR}/lib*.dylib\"")

      # Append full pathname of new components here using above pattern

      # Finally create install command to change rpaths and install names
      SET(CHNM_COMMAND "${CHNM} ${ARG1} ${ARG2} ${ARG3} ${ARG4} ${ARGN}")
      INSTALL(CODE "MESSAGE(STATUS \"Changing wxWidgets library install names\")")
      INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${CHNM_COMMAND} WORKING_DIRECTORY \"${CMAKE_CURRENT_SOURCE_DIR}\")")
    endif()
  endif()
endif()
