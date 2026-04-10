# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT GUI
# This file must be installed in the src/gui directory
#
# Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
# Modified For CMake Overhaul By: Ravi Mathur
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
# 

MESSAGE("==============================")
MESSAGE("GMAT GUI setup " ${VERSION})

# ====================================================================
# Major error checks
if(NOT wxWidgets_FOUND)
  MESSAGE(WARNING "Skipping GUI: wxWidgets missing")
  RETURN()
endif()

# Find OpenGL
FIND_PACKAGE(OpenGL)
if(NOT OPENGL_FOUND)
  MESSAGE(WARMING "Skipping GUI: OpenGL missing")
  RETURN()
endif()

# ====================================================================
SET(TargetName GmatGUI)

# ====================================================================
# SPICE specifics
SET(SPICE_SRCS "spacecraft/SpicePanel.cpp")

# Add preprocessor definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-D__USE_STC_EDITOR__")

# ====================================================================
# list of directories containing source/header files
SET(GUI_DIRS
  app
  asset
  burn
  command
  controllogic
  coordsystem
  debugger
  event
  forcemodel
  foundation
  function
  hardware
  include
  include/bitmaps
  include/il
  mission
  output
  plugin
  propagator
  rendering
  solarsys
  solver
  spacecraft
  subscriber
  view
  )

# ====================================================================
# source files
SET(GUI_SRCS 
    app/AboutDialog.cpp
    app/FileUpdateDialog.cpp
    app/GmatApp.cpp
    app/GmatAppData.cpp
    app/CompareFilesDialog.cpp
    app/CompareTextDialog.cpp
    app/GmatConnection.cpp
    app/GmatMainFrame.cpp
    app/GmatMenuBar.cpp
    app/GmatToolBar.cpp
    app/GuiInterpreter.cpp
    app/GuiListenerManager.cpp
    app/GuiMessageReceiver.cpp
    app/GuiPublisher.cpp
    app/GmatNotebook.cpp
    app/GmatServer.cpp
    app/GmatTreeItemData.cpp
    app/GuiPlotReceiver.cpp
    app/InteractiveMatlabDialog.cpp
    app/MissionTreeItemData.cpp
    app/RunScriptFolderDialog.cpp
    app/ResourceTree.cpp
    app/ScriptPanel.cpp
    app/SetPathDialog.cpp
    app/TextEphemFileDialog.cpp
    app/WelcomePanel.cpp
    asset/GroundStationPanel.cpp
    burn/FiniteBurnSetupPanel.cpp
    burn/ImpulsiveBurnSetupPanel.cpp
    command/AchievePanel.cpp
    command/AssignmentPanel.cpp
    command/BeginFiniteBurnPanel.cpp
    command/CallFunctionPanel.cpp
    command/EndFiniteBurnPanel.cpp
    command/FindEventsPanel.cpp
    command/GmatCommandPanel.cpp
    command/ManageObjectPanel.cpp
    command/ManeuverPanel.cpp
    command/MinimizePanel.cpp
    command/NonlinearConstraintPanel.cpp
    command/OptimizePanel.cpp
    command/PropagatePanel.cpp
    command/ReportPanel.cpp
    command/ScriptEventPanel.cpp
    command/TargetPanel.cpp
    command/TogglePanel.cpp
    command/VaryPanel.cpp
    controllogic/ForPanel.cpp
    controllogic/ConditionPanel.cpp
    coordsystem/CoordPanel.cpp
    coordsystem/CoordSysCreateDialog.cpp
    coordsystem/CoordSystemConfigPanel.cpp
    debugger/Breakpoint.cpp
    debugger/DebuggerCommandFactory.cpp
    debugger/InspectorPanel.cpp
    event/EventLocatorPanel.cpp
    forcemodel/DragInputsDialog.cpp
    foundation/ArraySetupDialog.cpp
    foundation/ArraySetupPanel.cpp
    foundation/GmatColorPanel.cpp
    foundation/GmatDialog.cpp
    foundation/GmatMdiChildFrame.cpp
    foundation/GmatPanel.cpp
    foundation/GmatBaseSetupPanel.cpp
    foundation/GmatSavePanel.cpp
    foundation/GmatStaticBoxSizer.cpp
    foundation/GuiItemManager.cpp
    foundation/MultiPathSetupPanel.cpp
    foundation/ParameterCreateDialog.cpp
    foundation/ParameterSelectDialog.cpp
    foundation/ParameterSetupPanel.cpp
    foundation/ShowScriptDialog.cpp
    foundation/ShowSummaryDialog.cpp
    foundation/SinglePathSetupPanel.cpp
    foundation/UserInputValidator.cpp
    function/FunctionSetupPanel.cpp
    function/MatlabFunctionSetupPanel.cpp
    hardware/BurnThrusterPanel.cpp
    hardware/PowerSystemConfigPanel.cpp
    hardware/TankAndMixDialog.cpp
    hardware/ThrusterCoefficientDialog.cpp
    hardware/ThrusterConfigPanel.cpp
    mission/DecoratedTree.cpp
    mission/MissionTree.cpp
    mission/MissionTreeToolBar.cpp
    mission/TreeViewOptionDialog.cpp
    mission/UndockedMissionPanel.cpp
    output/CompareReportPanel.cpp
    output/EventFilePanel.cpp
    output/OutputTree.cpp
    output/ReportFilePanel.cpp
    plugin/WxGuiInterface.cpp
    propagator/PropagationConfigPanel.cpp
    propagator/PropagatorSelectDialog.cpp
    rendering/Camera.cpp
    rendering/GLStars.cpp
    rendering/GmatOpenGLSupport.cpp
    rendering/Light.cpp
    rendering/ModelManager.cpp
    rendering/ModelObject.cpp
    rendering/Rendering.cpp
    rendering/Structure.cpp
    rendering/StructureReader.cpp
    rendering/StructureReader3ds.cpp
    rendering/SurfaceBase.cpp
    solarsys/BarycenterPanel.cpp
    solarsys/CelesBodySelectDialog.cpp
    solarsys/CelestialBodyOrbitPanel.cpp
    solarsys/CelestialBodyOrientationPanel.cpp
    solarsys/CelestialBodyPanel.cpp
    solarsys/CelestialBodyPropertiesPanel.cpp
    solarsys/CelestialBodyVisualizationPanel.cpp
    solarsys/LibrationPointPanel.cpp
    solarsys/UniversePanel.cpp
    solver/DCSetupPanel.cpp
    solver/SQPSetupPanel.cpp
    solver/SolverCreatePanel.cpp
    solver/SolverGoalsPanel.cpp
    solver/SolverSetupPanel.cpp
    solver/SolverVariablesPanel.cpp
    spacecraft/AttitudePanel.cpp
    spacecraft/BallisticsMassPanel.cpp
    spacecraft/FormationSetupPanel.cpp
    spacecraft/OrbitDesignerDialog.cpp
    spacecraft/OrbitPanel.cpp
    spacecraft/OrbitSummaryDialog.cpp
    spacecraft/PowerSystemPanel.cpp
    spacecraft/SpacecraftPanel.cpp
    spacecraft/SpaceObjectSelectDialog.cpp
    spacecraft/TankPanel.cpp
    spacecraft/ThrusterPanel.cpp
    spacecraft/VisualModelCanvas.cpp
    spacecraft/VisualModelPanel.cpp
    subscriber/DynamicDataDisplaySetupPanel.cpp
    subscriber/DynamicDataSettingsDialog.cpp
    subscriber/EphemerisFilePanel.cpp
    subscriber/GroundTrackArea.cpp
    subscriber/GroundTrackCanvas.cpp
    subscriber/GroundTrackCurve.cpp
    subscriber/GroundTrackPlotPanel.cpp
    subscriber/GroundTrackWindow.cpp
    subscriber/MdiChild3DViewFrame.cpp
    subscriber/MdiChildDynamicDataFrame.cpp
    subscriber/MdiChildGroundTrackFrame.cpp
    subscriber/MdiChildViewFrame.cpp
    subscriber/MdiChildTsFrame.cpp
    subscriber/MdiGlPlotData.cpp
    subscriber/MdiTableViewFrame.cpp
    subscriber/MdiTsPlotData.cpp
    subscriber/OrbitViewCanvas.cpp
    subscriber/OrbitViewPanel.cpp
    subscriber/ReportFileSetupPanel.cpp
    subscriber/SubscriberSetupPanel.cpp
    subscriber/ViewCanvas.cpp
    subscriber/TsPlotCanvas.cpp
    subscriber/TsPlotXYCanvas.cpp
    subscriber/TsPlotCurve.cpp
    subscriber/TsPlotOptionsDialog.cpp
    subscriber/XyPlotSetupPanel.cpp
    view/EditorPanel.cpp
    view/EditorPreferences.cpp
    view/EditorPrintout.cpp
    view/FindReplaceDialog.cpp
    view/ScriptEditor.cpp
    view/ViewTextDialog.cpp
    view/ViewTextFrame.cpp
    ${SPICE_SRCS}
)

# ====================================================================
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE GUI_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# add the icon information
SET(GMAT_ICON_DIR "resource")
SET(GMAT_ICON_O "GmatIcon.o")
if(WIN32)
  SET(GMAT_ICON_RC "GmatIcon.rc")
elseif(APPLE)
  SET(GMAT_ICON_RC "GMATMac.icns")
endif()
SET(GMAT_ICON_RC "${CMAKE_CURRENT_SOURCE_DIR}/${GMAT_ICON_DIR}/${GMAT_ICON_RC}")
SET(GMAT_ICON_O "${CMAKE_CURRENT_SOURCE_DIR}/${GMAT_ICON_DIR}/${GMAT_ICON_O}")
SET(GUI_SRCS ${GUI_SRCS} ${GMAT_ICON_RC})

# ====================================================================
# add the install targets

# Import wxWidgets build information (headers/libs)
# RPATHs are always set for the GmatGui installed executable, but in order
# for wxWidgets libs to be found by the GmatGui build executable this
# INCLUDE step must occur before the GmatGui target is created.
INCLUDE(${wxWidgets_USE_FILE})

# Create GmatGui as an executable
ADD_EXECUTABLE(${TargetName} ${GUI_SRCS} ${GUI_HEADERS})

# Add includes
TARGET_INCLUDE_DIRECTORIES(${TargetName} PRIVATE ${GUI_DIRS})
TARGET_INCLUDE_DIRECTORIES(${TargetName} PRIVATE ${OPENGL_INCLUDE_DIR})

# Add libraries
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatUtil)
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatBase)
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${wxWidgets_LIBRARIES})
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${OPENGL_LIBRARIES})

if(UNIX AND NOT APPLE)    # for threaded OpenGL on Linux
   TARGET_LINK_LIBRARIES(${TargetName} PRIVATE X11)
endif()

# The GUI name should be just "GMAT"
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES OUTPUT_NAME "GMAT")

# The debug GUI should have the same postfix as top-levl CMakeLists.txt
# This is required because CMAKE_DEBUG_POSTFIX only applies to libraries
# and not executables as of CMake 3.0.2
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# ====================================================================
# Add source/header files to IDE-based project source groups
# Macro defined in top-level CMakeLists.txt
_ADDSOURCEGROUPS("${GUI_DIRS}")

if(WIN32)
  SET_TARGET_PROPERTIES(${TargetName} PROPERTIES LINK_FLAGS "/SUBSYSTEM:windows")
endif()

# Create build products in bin directory
_SETOUTPUTDIRECTORY(${TargetName} bin)

# Override debug output directory
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${GMAT_BUILDOUTPUT_DEBUGDIR}
  )

# Specify where to install (make install or VS "INSTALL" project)
if(APPLE)
  SET(INSTALL_DIR ${GMAT_MAC_APPBUNDLE_PATH}/MacOS)
else()
  SET(INSTALL_DIR bin)
endif()
INSTALL(TARGETS ${TargetName} DESTINATION ${INSTALL_DIR})

# Install Mac bundle files
if(APPLE)
  INSTALL(CODE "MESSAGE(STATUS \"Creating GMAT.app bundle\")")

  # Install Mac icon file
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/resource/GMATMac.icns
    DESTINATION ${GMAT_MAC_APPBUNDLE_PATH}/Resources
    RENAME gmat.icns
    )

  # Install Mac plist file
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Info_GMAT.plist
    DESTINATION ${GMAT_MAC_APPBUNDLE_PATH}
    RENAME Info.plist
    )
  SET(CMD sed -e "s/EXECUTABLE/RunGMAT/" "-i \"\" " Info.plist)
  INSTALL(CODE "EXECUTE_PROCESS(COMMAND ${CMD} WORKING_DIRECTORY \"${CMAKE_INSTALL_PREFIX}/${GMAT_MAC_APPBUNDLE_PATH}\")")

  # Install Mac wrapper script to run GMAT
  INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/RunGMAT
    DESTINATION ${GMAT_MAC_APPBUNDLE_PATH}/MacOS
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ
    )
endif()

# Set RPATH to find shared libraries in default locations on Mac/Linux
if(UNIX)
  if(APPLE)
    SET(MAC_BASEPATH "../Frameworks/") # GmatBase and wxLibs in Frameworks
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "@loader_path/${MAC_BASEPATH}"
      )
  else()
    SET_TARGET_PROPERTIES(${TargetName} PROPERTIES INSTALL_RPATH
      "\$ORIGIN/;\$ORIGIN/../lib/"
      )
  endif()
endif()
