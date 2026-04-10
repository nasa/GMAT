# $Id$
#
# GMAT: General Mission Analysis Tool.
#
# CMAKE script file for GMAT Project
#
# Original Author: John McGreevy
#
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#


# ====================================================================

INCLUDE(${SWIG_USE_FILE})

# Major error checks
if(NOT SWIG_FOUND)
  MESSAGE(SEND_ERROR "SWIG missing")
  RETURN()
endif()

if(_GMATAPI_GENERATE_JAVA)
  # Initialize default GMAT dependency path for JDK
  if(WIN32)
    SET(Java_ROOT "${PROJECT_SOURCE_DIR}/depends/java/windows/jdk")
  elseif(APPLE)
    SET(Java_ROOT "${PROJECT_SOURCE_DIR}/depends/java/macosx/jdk/Contents/Home")
  else()
    SET(Java_ROOT "${PROJECT_SOURCE_DIR}/depends/java/linux/jdk")

    SET(ENV{PATH} "${Java_ROOT}/lib/server:${SYS_PATH}") # For some reason, if this is missing, it will find JAVA_JVM_LIBRARY in /usr/lib first
  endif()

  SET(JNI_ROOT "${Java_ROOT}")

  FIND_PACKAGE(JNI 11 REQUIRED)
  FIND_PACKAGE(Java 11 COMPONENTS Development REQUIRED)
  INCLUDE(UseJava)

  MESSAGE("Using JNI " ${JNI_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS})

  SET(API_JAVA_TARGET_VERSION 7 CACHE STRING "Version of Java to target")
  SET_PROPERTY(CACHE API_JAVA_TARGET_VERSION PROPERTY STRINGS 11 10 9 8 7 6)
endif()

# Find specified Python3 version, using the specified Python3 root directory
if(_GMATAPI_GENERATE_PYTHON)
  _SETPYTHONROOTDIR(${GMATAPI_Python3_Version})
  FIND_PACKAGE(Python3 ${GMATAPI_Python3_Version} EXACT COMPONENTS Development)
  if(NOT Python3_Development_FOUND)
    MESSAGE(NOTICE "Skipping Python API: Python ${GMATAPI_Python3_Version} not found")
    _REPORTPYTHONURL(${GMATAPI_Python3_Version})
  endif()
endif()

# Add preprocessor definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DEXP_STL")

SET(CMAKE_SWIG_FLAGS "")
SET(UseSWIG_MODULE_VERSION 2)


# Convenience macro used by GMAT SWIG wrappers to enable common functionality
# Inputs:
#   TargetName: Name of SWIG target
#   SWIG_LANGUAGE: Language to create SWIG wrappers for
#   SWIG_SRC: SWIG .i source file
#   TARGET_DEPS: List of targets this depends on
# This macro also uses the following CMake variables if set:
#   SWIG_EXTRA_JAVA_FILES: Additional Java files to compile into the JAR
#   SWIG_EXTRA_JAR_FILES: Additional JAR files to use in the classpath
# Note that all paths should be relative to the plugin's CMakeLists.txt
MACRO(_SETUPSWIG TargetName SWIG_LANGUAGE SWIG_SRC TARGET_DEPS)
  # Set full target name
  STRING(TOLOWER ${SWIG_LANGUAGE} SWIG_LANGUAGE_LOWER)
  STRING(COMPARE EQUAL ${SWIG_LANGUAGE_LOWER} "java" IS_JAVA)
  STRING(COMPARE EQUAL ${SWIG_LANGUAGE_LOWER} "python" IS_PYTHON)

  if(IS_JAVA)
    STRING(CONCAT TargetNameFull ${TargetName} ${API_JAVA_SUFFIX})
  elseif(IS_PYTHON)
    STRING(CONCAT TargetModuleName ${TargetName} ${API_PYTHON_SUFFIX})
    STRING(CONCAT TargetModuleSuffix ${API_PYTHON_SUFFIX} ${Python3_VERSION_MAJOR} ${Python3_VERSION_MINOR})
    STRING(CONCAT TargetNameFull ${TargetName} ${TargetModuleSuffix})
  else()
    MESSAGE(SEND_ERROR "${SWIG_LANGUAGE} is not a supported language")
    RETURN()
  endif()

  # Set INCLUDE_DIRECTORIES based on TARGET_DEPS
  FOREACH(TARGET_DEP ${TARGET_DEPS})
    GET_PROPERTY(TARGET_DIRS TARGET ${TARGET_DEP} PROPERTY INCLUDE_DIRECTORIES)
    INCLUDE_DIRECTORIES(${TARGET_DIRS})
  ENDFOREACH()

  # Add current SWIG directory to INCLUDE_DIRECTORIES
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  # Set source file properties
  SET_PROPERTY(SOURCE ${SWIG_SRC} PROPERTY CPLUSPLUS ON)
  if(IS_JAVA)
    SET_SOURCE_FILES_PROPERTIES(${SWIG_SRC} PROPERTIES SWIG_FLAGS "-package;gmat;-doxygen")
  else()
    SET_SOURCE_FILES_PROPERTIES(${SWIG_SRC} PROPERTIES SWIG_FLAGS "-doxygen")
  endif()

  # Need to pass the include directories to the SWIG wrapper generator in Visual Studio
  if(MSVC)
    GET_DIRECTORY_PROPERTY(SWIG_INCL INCLUDE_DIRECTORIES)
    LIST(TRANSFORM SWIG_INCL PREPEND "-I")
    EXECUTE_PROCESS(COMMAND ${SWIG_EXECUTABLE} -MM -${SWIG_LANGUAGE_LOWER} ${SWIG_INCL} ${SWIG_SRC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE SWIG_HEADER_DEPS
        )
    STRING(REPLACE " \\\n  " ";" SWIG_HEADER_DEPS ${SWIG_HEADER_DEPS})
    # Remove the first two items in the list as they are reudundant
    LIST(REMOVE_AT SWIG_HEADER_DEPS 0 1)
    SET(SWIG_MODULE_${TargetNameFull}_EXTRA_DEPS ${SWIG_HEADER_DEPS})
  endif()

  # Create the library
  SWIG_ADD_LIBRARY(${TargetNameFull} LANGUAGE ${SWIG_LANGUAGE_LOWER} SOURCES ${SWIG_SRC})
  TARGET_LINK_LIBRARIES(${TargetNameFull} GmatUtil GmatBase)
  if(IS_PYTHON)
    SET_TARGET_PROPERTIES(${TargetNameFull} PROPERTIES DEBUG_POSTFIX "" OUTPUT_NAME ${TargetModuleName})
    TARGET_LINK_LIBRARIES(${TargetNameFull} Python3::Module)
  endif()

  GET_TARGET_PROPERTY(SWIG_FILES_DIR ${TargetNameFull} SWIG_SUPPORT_FILES_DIRECTORY)

  # Set output directory
  if(IS_PYTHON)
    SET(OUTPUT_DIR_RELEASE "bin/gmatpy/${TargetModuleSuffix}")
    SET(OUTPUT_DIR_DEBUG "gmatpy/${TargetModuleSuffix}")
  else()
    SET(OUTPUT_DIR_RELEASE "bin")
    SET(OUTPUT_DIR_DEBUG "")
  endif()

  # Create build products in bin directory
  _SETOUTPUTDIRECTORY(${TargetNameFull} ${OUTPUT_DIR_RELEASE})

  # Override debug output directory
  SET_TARGET_PROPERTIES(${TargetNameFull} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}/${OUTPUT_DIR_DEBUG}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}/${OUTPUT_DIR_DEBUG}
  )

  SET(OUTPUT_DIR $<$<CONFIG:Release>:${GMAT_BUILDOUTPUT_DIRECTORY}/${OUTPUT_DIR_RELEASE}>$<$<CONFIG:Debug>:${GMAT_BUILDOUTPUT_DEBUGDIR}/${OUTPUT_DIR_DEBUG}>)

  if(IS_JAVA)
    # Java only extra commands for compiling Java files and creating JAR
    if(SWIG_EXTRA_JAR_FILES)
      SET(API_JAVA_CLASSPATH_FLAG -classpath ${SWIG_EXTRA_JAR_FILES})
    endif()

    # Compile .java files
    ADD_CUSTOM_COMMAND(TARGET ${TargetNameFull} POST_BUILD
        WORKING_DIRECTORY ${SWIG_FILES_DIR}
        COMMENT "Compiling SWIG Java files"
        COMMAND ${CMAKE_COMMAND} -E remove_directory
            ${SWIG_FILES_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E make_directory
            ${SWIG_FILES_DIR}/bin
        COMMAND ${Java_JAVAC_EXECUTABLE}
            --release ${API_JAVA_TARGET_VERSION}
            ${API_JAVA_CLASSPATH_FLAG}
            ${SWIG_EXTRA_JAVA_FILES}
            ${SWIG_FILES_DIR}/*.java -d bin
    )

    # Convert .java file names in SWIG_EXTRA_JAVA_FILES to .class file names
    FOREACH(JAVA_FILE ${SWIG_EXTRA_JAVA_FILES})
      GET_FILENAME_COMPONENT(CLASS_FILE ${JAVA_FILE} NAME_WLE)
      LIST(APPEND SWIG_EXTRA_CLASS_FILES ${CLASS_FILE})
    ENDFOREACH()
    LIST(TRANSFORM SWIG_EXTRA_CLASS_FILES APPEND ".class")

    # Package the JAR file
    ADD_CUSTOM_COMMAND(TARGET ${TargetNameFull} POST_BUILD
        WORKING_DIRECTORY ${SWIG_FILES_DIR}/bin
        COMMENT "Packaging ${TargetName}.jar file"
        COMMAND ${CMAKE_COMMAND} -E chdir ${SWIG_FILES_DIR}/bin ${Java_JAR_EXECUTABLE}
            cf ${OUTPUT_DIR}/${TargetName}.jar gmat/*.class ${SWIG_EXTRA_CLASS_FILES}
    )
  elseif(IS_PYTHON)
    # Copy the .py file to the application folder
    ADD_CUSTOM_COMMAND(TARGET ${TargetNameFull} POST_BUILD
        # WORKING_DIRECTORY ${SWIG_FILES_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E copy
            ${SWIG_FILES_DIR}/${TargetModuleName}.py
            ${OUTPUT_DIR}
    )
  endif()

  # Add folder name for Visual Studio
  if(IS_JAVA)
    SET_TARGET_PROPERTIES(${TargetNameFull} PROPERTIES FOLDER "GMAT API/Java")
  elseif(IS_PYTHON)
    SET_TARGET_PROPERTIES(${TargetNameFull} PROPERTIES FOLDER "GMAT API/Python")
  endif()


  # Set RPATH to find shared libraries in default locations on Mac/Linux
  if(UNIX)
    if(APPLE)
      if (IS_JAVA)
         SET_TARGET_PROPERTIES(${TargetNameFull} PROPERTIES INSTALL_RPATH
         "@loader_path/;@loader_path/../plugins/;@loader_path/../plugins/proprietary/"
         )
      endif()
      if (IS_PYTHON)
         SET_TARGET_PROPERTIES(${TargetNameFull} PROPERTIES INSTALL_RPATH
         "@loader_path/;@loader_path/../../;@loader_path/../../../plugins/;@loader_path/../../../plugins/proprietary/"
         )
      endif()
    else()
      if (IS_JAVA)
         SET_TARGET_PROPERTIES(${TargetNameFull} PROPERTIES INSTALL_RPATH
         "\$ORIGIN/;\$ORIGIN/../plugins/;\$ORIGIN/../plugins/proprietary/:$ORIGIN/../lib"
         )
      endif()
      if (IS_PYTHON)
         SET_TARGET_PROPERTIES(${TargetNameFull} PROPERTIES INSTALL_RPATH
         "\$ORIGIN/:\$ORIGIN/../../:\$ORIGIN/../../../plugins/:\$ORIGIN/../../../plugins/proprietary/:\$ORIGIN/../../../lib"
         )
      endif()
    endif()
  endif()

  # Install binaries in appropriate locations
  SET(INSTALL_USER_DIR "${CMAKE_INSTALL_PREFIX}/bin")
  if(WIN32)
    SET(INSTALL_DIR bin)
  elseif(APPLE AND GMAT_MAC_APPBUNDLE_PATH)
    SET(INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/Frameworks)
  else()
    SET(INSTALL_DIR bin)
  endif()
  
  if (IS_PYTHON)
    SET(INSTALL_DIR "${INSTALL_DIR}/gmatpy/${TargetModuleSuffix}")
    SET(INSTALL_USER_DIR "${INSTALL_USER_DIR}/gmatpy/${TargetModuleSuffix}")

    INSTALL(TARGETS ${TargetNameFull} LIBRARY DESTINATION ${INSTALL_DIR})
    INSTALL(FILES ${OUTPUT_DIR}/${TargetModuleName}.py DESTINATION ${INSTALL_DIR})
    
    # Create symbolic links to binaries on macOS since they are buried in the app bundle
    if(APPLE AND GMAT_MAC_APPBUNDLE_PATH)
      INSTALL(DIRECTORY DESTINATION "${INSTALL_USER_DIR}")

      # Create symlink to python lib
      INSTALL(CODE "EXECUTE_PROCESS( \
          COMMAND ${CMAKE_COMMAND} -E create_symlink \
          ../../../${INSTALL_DIR}/$<TARGET_FILE_NAME:${TargetNameFull}> \
          ${INSTALL_USER_DIR}/$<TARGET_FILE_NAME:${TargetNameFull}> \
          )"
      )
      
      # Create symlink to python script
      INSTALL(CODE "EXECUTE_PROCESS( \
          COMMAND ${CMAKE_COMMAND} -E create_symlink \
          ../../../${INSTALL_DIR}/${TargetModuleName}.py \
          ${INSTALL_USER_DIR}/${TargetModuleName}.py \
          )"
      )
    endif()
  endif()

  if (IS_JAVA)
    INSTALL(TARGETS ${TargetNameFull} DESTINATION ${INSTALL_DIR})
    INSTALL(FILES ${OUTPUT_DIR}/${TargetName}.jar DESTINATION ${INSTALL_DIR})
    
    # Create symbolic links to binaries on macOS since they are buried in the app bundle
    if(APPLE AND GMAT_MAC_APPBUNDLE_PATH)
      # Create symlink to java lib
      INSTALL(CODE "EXECUTE_PROCESS( \
          COMMAND ${CMAKE_COMMAND} -E create_symlink \
          ../${INSTALL_DIR}/lib${TargetNameFull}.jnilib \
          ${INSTALL_USER_DIR}/lib${TargetNameFull}.dylib \
          )"
      )
      
      # Create symlink to jar
      INSTALL(CODE "EXECUTE_PROCESS( \
          COMMAND ${CMAKE_COMMAND} -E create_symlink \
          ../${INSTALL_DIR}/${TargetName}.jar \
          ${INSTALL_USER_DIR}/${TargetName}.jar \
          )"
      )
    endif()
  endif()

ENDMACRO(_SETUPSWIG)
