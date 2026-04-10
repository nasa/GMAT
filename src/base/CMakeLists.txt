# $Id$
# 
# GMAT: General Mission Analysis Tool.
# 
# CMAKE script file for GMAT base library
# This file must be installed in the src/base directory
#
# Author: Joris T. Olympio
# Modified For Production By: Josh Fisher
#  
# DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING!
#

MESSAGE("==============================")
MESSAGE("GMAT base setup " ${VERSION}) 

SET(TargetName GmatBase)

# Add preprocessor definitions
ADD_DEFINITIONS("-D_DYNAMICLINK")
ADD_DEFINITIONS("-DEXP_STL")

SET(SPICE_SRCS
  "attitude/SpiceAttitude.cpp"
  "spice/SpiceAttitudeKernelReader.cpp"
  "spice/SpiceAttitudeKernelWriter.cpp"
  "spice/SpiceFrameKernelWriter.cpp"
  "spice/SpiceInterface.cpp"
  "spice/SpiceOrbitKernelReader.cpp"
  "spice/SpiceKernelReader.cpp"
  "spice/SpiceKernelWriter.cpp"
  "spice/SpiceOrbitKernelWriter.cpp"
  "spice/SpiceSCClockKernelWriter.cpp")

# ====================================================================
# F2C
MESSAGE("-- Use F2C version of MSISE.")
SET(MSISE_SRCS "solarsys/msise90_sub.c")

# ====================================================================
# list of directories containing source/header files
SET(BASE_DIRS
  api
  asset
  attitude
  burn
  command
  configs
  coordsystem
  event
  executive
  factory
  factory/guicomponents
  forcemodel
  forcemodel/harmonic
  foundation
  function
  hardware
  include
  interface
  interpreter
  math
  parameter
  plugin
  propagator
  solarsys
  solver
  spice
  spacecraft
  stopcond
  subscriber
  )

# ====================================================================
# source files
SET(BASE_SRCS
    api/APIFunctions.cpp
    api/APIException.cpp
    api/APIMessageReceiver.cpp
    api/HelpSystem.cpp
    asset/AssetException.cpp
    asset/BodyFixedPoint.cpp
    asset/GroundstationInterface.cpp
    attitude/Attitude.cpp
    attitude/AttitudeException.cpp
    attitude/CCSDSAttitude.cpp
    attitude/CommandableNadirPointing.cpp
    attitude/CSFixed.cpp
    attitude/Kinematic.cpp
    attitude/NadirPointing.cpp
    attitude/PrecessingSpinner.cpp
    attitude/Spinner.cpp
    attitude/ThreeAxisKinematic.cpp
    burn/BurnException.cpp
    burn/Burn.cpp
    burn/FiniteBurn.cpp
    burn/ImpulsiveBurn.cpp
    burn/InertialManeuverFrame.cpp
    burn/ManeuverFrameManager.cpp
    burn/ManeuverFrame.cpp
    burn/VnbManeuverFrame.cpp
    command/Achieve.cpp
    command/Assignment.cpp
    command/BeginFiniteBurn.cpp
    command/BeginFunction.cpp
    command/BeginMissionSequence.cpp
    command/BeginScript.cpp
    command/BranchCommand.cpp
    command/CallFunction.cpp
    command/CallBuiltinGmatFunction.cpp
    command/CommandException.cpp
    command/FindEvents.cpp
    command/GmatCommand.cpp
    command/CommandUtil.cpp
    command/ClearPlot.cpp
    command/ConditionalBranch.cpp
    command/Create.cpp
    command/Else.cpp
    command/EndFiniteBurn.cpp
    command/EndFor.cpp
    command/EndFunction.cpp
    command/EndIf.cpp
    command/EndOptimize.cpp
    command/EndScript.cpp
    command/EndTarget.cpp
    command/EndWhile.cpp
    command/For.cpp
    command/If.cpp
    command/ManageObject.cpp
    command/Maneuver.cpp
    command/MarkPoint.cpp
    command/Minimize.cpp
    command/NonlinearConstraint.cpp
    command/NoOp.cpp
    command/Optimize.cpp
    command/PenDown.cpp
    command/PenUp.cpp
    command/Propagate.cpp
    command/PropagationEnabledCommand.cpp
    command/PlotCommand.cpp
    command/SolverSequenceCommand.cpp
    command/RHSEquation.cpp
    command/Report.cpp
    command/RunSolver.cpp
    command/SaveMission.cpp
    command/SolverBranchCommand.cpp
    command/Stop.cpp
    command/Target.cpp
    command/Toggle.cpp
    command/UpdateDynamicData.cpp
    command/Vary.cpp
    command/While.cpp
    command/Write.cpp
    configs/ConfigManagerException.cpp
    configs/ConfigManager.cpp
    configs/ItemManager.cpp
    configs/PluginItemManager.cpp
    coordsystem/AxisSystem.cpp
    coordsystem/BodyFixedAxes.cpp
    coordsystem/BodyInertialAxes.cpp
    coordsystem/CoordinateBase.cpp
    coordsystem/CoordinateConverter.cpp
    coordsystem/CoordinateSystemException.cpp
    coordsystem/CoordinateSystem.cpp
    coordsystem/CoordinateTransformation.cpp
    coordsystem/CoordinateTranslation.cpp
    coordsystem/DynamicAxes.cpp
    coordsystem/EquatorAxes.cpp
    coordsystem/GeocentricSolarEclipticAxes.cpp
    coordsystem/GeocentricSolarMagneticAxes.cpp
    coordsystem/InertialAxes.cpp
    coordsystem/ItrfCoefficientsFile.cpp
    coordsystem/LocalAlignedConstrainedAxes.cpp
    coordsystem/MeanOfDateAxes.cpp
    coordsystem/MJ2000EcAxes.cpp
    coordsystem/MJ2000EqAxes.cpp
    coordsystem/MODEcAxes.cpp
    coordsystem/MODEqAxes.cpp
    coordsystem/MOEEcAxes.cpp
    coordsystem/MOEEqAxes.cpp
    coordsystem/ObjectReferencedAxes.cpp
	coordsystem/SpiceAxes.cpp
    coordsystem/TODEcAxes.cpp
    coordsystem/TODEqAxes.cpp
    coordsystem/TEMEAxes.cpp
    coordsystem/TEMEAxes.cpp
    coordsystem/TOEEcAxes.cpp
    coordsystem/TOEEqAxes.cpp
    coordsystem/TopocentricAxes.cpp
    coordsystem/TransformUtil.cpp
    coordsystem/TrueOfDateAxes.cpp
    coordsystem/ICRFAxes.cpp
    coordsystem/ICRFFile.cpp
    coordsystem/ITRFAxes.cpp
    coordsystem/IAUFile.cpp
    coordsystem/BodySpinSunAxes.cpp
    event/EventException.cpp
    event/EventLocator.cpp
    event/LocatedEvent.cpp
    executive/ListenerManager.cpp
    executive/ListenerManagerInterface.cpp
    executive/Moderator.cpp
    executive/PlotInterface.cpp
    executive/PlotReceiver.cpp
    executive/PublisherException.cpp
    executive/Publisher.cpp
    executive/SandboxException.cpp
    executive/Sandbox.cpp
    factory/AtmosphereFactory.cpp
    factory/AttitudeFactory.cpp
    factory/AxisSystemFactory.cpp
    factory/BurnFactory.cpp
    factory/CalculatedPointFactory.cpp
    factory/CelestialBodyFactory.cpp
    factory/CommandFactory.cpp
    factory/CoordinateSystemFactory.cpp
    factory/FactoryException.cpp
    factory/FactoryManager.cpp
    factory/Factory.cpp
    factory/FunctionFactory.cpp
    factory/HardwareFactory.cpp
    factory/FieldOfViewFactory.cpp
    factory/InterfaceFactory.cpp
    factory/MathFactory.cpp
    factory/ODEModelFactory.cpp
    factory/ParameterFactory.cpp
    factory/PhysicalModelFactory.cpp
    factory/PlanetographicRegionFactory.cpp
    factory/PlateFactory.cpp
    factory/PropagatorFactory.cpp
    factory/PropSetupFactory.cpp
    factory/SolarSystemFactory.cpp
    factory/SolverFactory.cpp
    factory/SpacecraftFactory.cpp
    factory/StopConditionFactory.cpp
    factory/SubscriberFactory.cpp
    factory/guicomponents/GuiFactory.cpp
    factory/guicomponents/GmatWidget.cpp
    factory/guicomponents/PluginWidget.cpp
    forcemodel/DragForce.cpp
    forcemodel/FiniteThrust.cpp
    forcemodel/ODEModelException.cpp
    forcemodel/ODEModel.cpp
    forcemodel/GravityField.cpp
    forcemodel/GravityBase.cpp
    forcemodel/HarmonicField.cpp
    forcemodel/NumericJacobianOriginal.cpp
    forcemodel/PhysicalModel.cpp
    forcemodel/PointMassForce.cpp
    forcemodel/SolarRadiationPressure.cpp
    #forcemodel/EventModel.cpp
    forcemodel/RelativisticCorrection.cpp
    forcemodel/harmonic/Harmonic.cpp
    forcemodel/harmonic/HarmonicGravity.cpp
    foundation/Covariance.cpp
    foundation/ElementWrapper.cpp
    foundation/EquationInitializer.cpp
    foundation/FileUpdaterSVN.cpp
    foundation/GmatBaseException.cpp
    foundation/GmatBase.cpp
    foundation/GmatState.cpp
    foundation/GmatType.cpp
    foundation/IChangeListener.cpp
    foundation/ObjectInitializer.cpp
    foundation/SpacePoint.cpp
    foundation/StateManager.cpp
    foundation/TriggerManager.cpp
    function/BuiltinGmatFunction.cpp
    function/Function.cpp
    function/FunctionException.cpp
    function/FunctionManager.cpp
    function/GetEphemStates.cpp
    function/GetLastState.cpp
    function/ObjectManagedFunction.cpp
    function/Pause.cpp
    function/QuaternionProduct.cpp
    function/QuaternionRotation.cpp
    function/QuaternionToDCM.cpp
    function/SystemTime.cpp
    function/SetSeed.cpp
    function/ConvertTime.cpp
    function/Sign.cpp
    function/Str2num.cpp
    function/Num2str.cpp
    function/RotationMatrix.cpp
    function/Angle.cpp
    function/UserDefinedFunction.cpp
    hardware/ChemicalTank.cpp
    hardware/ConicalFOV.cpp  
    hardware/CustomFOV.cpp  
    hardware/ChemicalThruster.cpp
    hardware/ElectricTank.cpp
    hardware/ElectricThruster.cpp
    hardware/FieldOfView.cpp
    hardware/FieldOfViewException.cpp
    hardware/FuelTank.cpp
    hardware/HardwareException.cpp
    hardware/Hardware.cpp
    hardware/Imager.cpp
    hardware/NuclearPowerSystem.cpp
    hardware/PowerSystem.cpp
    hardware/RectangularFOV.cpp  
    hardware/SolarPowerSystem.cpp
    hardware/Thruster.cpp
    interface/InterfaceException.cpp
    interface/Interface.cpp
    interface/GmatInterface.cpp
    interpreter/InterpreterException.cpp
    interpreter/Interpreter.cpp
    interpreter/MathParser.cpp
    interpreter/MathTree.cpp
    interpreter/ScriptInterpreter.cpp
    interpreter/ScriptReadWriter.cpp
    interpreter/Validator.cpp
	interpreter/ApiInterpreter.cpp
    math/Abs.cpp
    math/Acos.cpp
    math/Acosh.cpp
    math/Add.cpp
    math/Asin.cpp
    math/Asinh.cpp
    math/Atan.cpp
    math/Atan2.cpp
    math/BuiltinFunctionNode.cpp
    math/Ceil.cpp
    math/Cos.cpp
    math/Cosh.cpp
    math/Cross3.cpp
    math/DegToRad.cpp
    math/Determinant.cpp
    math/Diag.cpp
    math/Divide.cpp
    math/Exp.cpp
    math/Fix.cpp
    math/Floor.cpp
    math/FunctionRunner.cpp
    math/Log.cpp
    math/Log10.cpp
    math/Inverse.cpp
    math/MathElement.cpp
    math/MathException.cpp
    math/MathFunction.cpp
    math/MathNode.cpp
    math/Min.cpp
    math/Mod.cpp
    math/Multiply.cpp
    math/Negate.cpp
    math/Norm.cpp
    math/NumericFunctionNode.cpp
    math/Power.cpp
    math/RadToDeg.cpp
    math/Rand.cpp
    math/Randn.cpp
    math/Sin.cpp
    math/Sinh.cpp
    math/Sprintf.cpp
    math/Sqrt.cpp
    math/StringFunctionNode.cpp
    math/Strcat.cpp
    math/Strcmp.cpp
    math/Strfind.cpp
    math/Strlen.cpp
    math/Strrep.cpp
    math/Substr.cpp
    math/Subtract.cpp
    math/Tan.cpp
    math/Tanh.cpp
    math/Transpose.cpp
    math/EquationWrapper.cpp
    parameter/AlternateEquinoctialParameters.cpp
    parameter/AngularParameters.cpp
    parameter/Array.cpp
    parameter/ArrayWrapper.cpp
    parameter/ArrayElementWrapper.cpp
    parameter/AttitudeData.cpp
    parameter/AttitudeParameters.cpp
    parameter/AttitudeReal.cpp
    parameter/AttitudeRmat33.cpp
    parameter/AttitudeRvector.cpp
    parameter/BallisticMassParameters.cpp
    parameter/BallisticMassReal.cpp
    parameter/BooleanWrapper.cpp
    parameter/BplaneData.cpp
    parameter/BplaneParameters.cpp
    parameter/BplaneReal.cpp
    parameter/BrouwerMeanLongParameters.cpp
    parameter/BrouwerMeanShortParameters.cpp
    parameter/BurnData.cpp
    parameter/BurnParameters.cpp
    parameter/BurnReal.cpp
    parameter/CartesianParameters.cpp
    parameter/DelaunayParameters.cpp
    parameter/EnvData.cpp
    parameter/EnvReal.cpp
    parameter/EquinoctialParameters.cpp
    parameter/ExpressionParser.cpp
    parameter/HardwareParameters.cpp
    parameter/HardwareReal.cpp
    parameter/HardwareRvector.cpp
    parameter/HardwareString.cpp
    parameter/IncomingAsymptoteParameters.cpp
    parameter/KeplerianParameters.cpp
    parameter/ModEquinoctialParameters.cpp
    parameter/OrbitTime.cpp
    parameter/NumberWrapper.cpp
    parameter/ObjectWrapper.cpp
    parameter/ObjectPropertyWrapper.cpp
    parameter/OnOffWrapper.cpp
    parameter/OrbitalParameters.cpp
    parameter/OrbitCovarianceParameters.cpp
    parameter/OrbitData.cpp
    parameter/OrbitReal.cpp
    parameter/OrbitRmat33.cpp
    parameter/OrbitRmat66.cpp
    parameter/OrbitRvec6.cpp
    parameter/OrbitStmParameters.cpp
    parameter/OutgoingAsymptoteParameters.cpp
    parameter/ParameterDatabase.cpp
    parameter/ParameterInfo.cpp
    parameter/Parameter.cpp
    parameter/ParameterWrapper.cpp
    parameter/PlanetodeticParameters.cpp
    parameter/PlanetData.cpp
    parameter/PlanetParameters.cpp
    parameter/PlanetReal.cpp
    parameter/RealVar.cpp
    parameter/RefData.cpp
    parameter/Rmat33Var.cpp
    parameter/Rmat66Var.cpp
    parameter/Rvec3Var.cpp
    parameter/Rvec6Var.cpp
    parameter/RvectorVar.cpp
    parameter/SpacecraftData.cpp
    parameter/SphericalParameters.cpp
    parameter/StringWrapper.cpp
    parameter/StringObjectWrapper.cpp
    parameter/StringVar.cpp
    parameter/TimeData.cpp
    parameter/TimeParameters.cpp
    parameter/TimeReal.cpp
    parameter/TimeString.cpp
    parameter/Variable.cpp
    parameter/VariableWrapper.cpp
    parameter/AttitudeString.cpp
    plugin/DynamicLibrary.cpp
    plugin/GmatEventHandler.cpp
    plugin/GuiInterface.cpp
    propagator/AdamsBashforthMoulton.cpp
    propagator/DormandElMikkawyPrince68.cpp
    propagator/Integrator.cpp
    propagator/PredictorCorrector.cpp
    propagator/PrinceDormand45.cpp
    propagator/PrinceDormand78.cpp
    propagator/PropagationStateManager.cpp
    propagator/Propagator.cpp
    propagator/PropSetup.cpp
    propagator/RungeKutta89.cpp
    propagator/RungeKuttaFehlberg56.cpp
    propagator/RungeKuttaNystrom.cpp
    propagator/RungeKutta.cpp
    solarsys/Asteroid.cpp
    solarsys/AtmosphereModel.cpp
    solarsys/Barycenter.cpp
    solarsys/CalculatedPoint.cpp
    solarsys/CelestialBody.cpp
    solarsys/Comet.cpp
    solarsys/DeFile.cpp
    solarsys/EphemSmoother.cpp
    solarsys/ExponentialAtmosphere.cpp
    solarsys/JacchiaRobertsAtmosphere.cpp
    solarsys/LibrationPoint.cpp
    solarsys/MediaCorrectionInterface.cpp
    solarsys/Moon.cpp
    solarsys/Msise90Atmosphere.cpp
    solarsys/PlanetaryEphemException.cpp
    solarsys/PlanetaryEphem.cpp
    solarsys/PlanetographicRegion.cpp
    solarsys/Planet.cpp
    solarsys/ShadowState.cpp
    solarsys/SlpFile.cpp
    solarsys/SolarFluxReader.cpp
    solarsys/SolarSystemException.cpp
    solarsys/SolarSystem.cpp
    solarsys/Star.cpp
    solver/Solver.cpp
    solver/DifferentialCorrector.cpp
    solver/Optimizer.cpp
    solver/InternalOptimizer.cpp
    solver/ISolverListener.cpp
    solver/ExternalOptimizer.cpp
    solver/DerivativeModel.cpp
    solver/Gradient.cpp
    solver/Jacobian.cpp
    solver/LineSearch.cpp
    spacecraft/FormationInterface.cpp
    spacecraft/Plate.cpp
    spacecraft/Spacecraft.cpp
    spacecraft/SpaceObject.cpp
    spacecraft/TextTrajectoryFile.cpp
    stopcond/StopCondition.cpp
    subscriber/AttitudeWriterCK.cpp
    subscriber/EphemerisFile.cpp
    subscriber/EphemerisWriter.cpp
    subscriber/EphemWriterCCSDS.cpp
    subscriber/EphemWriterCode500.cpp
    subscriber/EphemWriterSPK.cpp
    subscriber/EphemWriterSTK.cpp
    subscriber/EphemWriterWithInterpolator.cpp
    subscriber/EphemManager.cpp
    subscriber/MessageWindow.cpp
    subscriber/TextEphemFile.cpp
    subscriber/OrbitView.cpp
    subscriber/OwnedPlot.cpp
    subscriber/ReportFile.cpp
    subscriber/Subscriber.cpp
    subscriber/XyPlot.cpp
    subscriber/OrbitPlot.cpp
    subscriber/GroundTrackPlot.cpp
    subscriber/GroundTrack.cpp
    subscriber/DynamicDataDisplay.cpp
    subscriber/DynamicDataInterface.cpp
    solarsys/SimpleExponentialAtmosphere.cpp
    solarsys/SpecialCelestialPoint.cpp
    ${MSISE_SRCS}
    ${SPICE_SRCS}
)

# ====================================================================
# Recursively find all include files, which will be added to IDE-based
# projects (VS, XCode, etc.)
FILE(GLOB_RECURSE BASE_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp)

# ====================================================================
# add the install targets

# Create GmatBase as a shared library
ADD_LIBRARY(${TargetName} SHARED ${BASE_SRCS} ${BASE_HEADERS})
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES DEFINE_SYMBOL "GMAT_EXPORTS")

# Add necessary include directories
TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${BASE_DIRS})
TARGET_INCLUDE_DIRECTORIES(${TargetName} PUBLIC ${CSPICE_INCLUDE_DIR})

# Link needed libraries
TARGET_LINK_LIBRARIES(${TargetName} PUBLIC optimized ${CSPICE_LIB} debug ${CSPICE_LIB_DEBUG})
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE XercesC::XercesC)
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${CMAKE_DL_LIBS})
TARGET_LINK_LIBRARIES(${TargetName} PRIVATE GmatUtil)

if(UNIX)
  if(APPLE)
    TARGET_LINK_LIBRARIES(${TargetName} PRIVATE ${COREFOUNDATION_LIBRARY} ${CORESERVICES_LIBRARY})
  else()
    TARGET_LINK_LIBRARIES(${TargetName} PRIVATE Threads::Threads)
  endif()
else()
  # XERCES_STATIC_LIBRARY required on Windows
  TARGET_COMPILE_DEFINITIONS(${TargetName} PUBLIC -DXERCES_STATIC_LIBRARY)
endif()

# Library name should start with "lib"
# This is always true for Mac/Linux, but needs to be specified for Windows
SET_TARGET_PROPERTIES(${TargetName} PROPERTIES PREFIX "lib")

# ====================================================================
# Add source/header files to IDE-based project source groups
# Macro defined in top-level CMakeLists.txt
_ADDSOURCEGROUPS("${BASE_DIRS}")

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
