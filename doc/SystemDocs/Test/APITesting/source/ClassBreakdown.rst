.. _ExposedClasses:

*************
GMAT Features
*************
API users work with GMAT on several different levels of interaction:  

* Users familiar with GMAT scripting can use the API to load their scripts, manipulate objects, and run the script.  Following the run, the user can then retrieve data from the objects used in the script.  
* Users familiar with GMAT objects can work directly with those objects, changing settings, configuring interactions, and driving the objects from code native to the API user's platform. 

Testing for these interactions is designed to match these usages.

.. _ScriptExecution:

Driving GMAT Scripts from the API
=================================
:numref:`ScriptFunctions` lists the functions used to run scripts using the API.  The general process flow for API tests of these functions is as follows:

#. Launch a console for the API platform.
#. Load the GMAT API.
#. Set the GMAT log file using the UseLogFile() function.
#. Do one of the following:
   
   A. Execute the function being tested.
   #. Run a test script and test the results.

      #. Load the test script.
      #. Run the script.
      #. Test the resulting output.

#. Write the test artifacts for use by the automated test system.
#. Close the terminal.

This procedure is tailored to the specific test being executed.

.. _ScriptFunctions:
.. table:: Functions Used with GMAT Scripts
   :class: longtable
   :widths: 18 12 25 45

   ================= ============ ========================= ========================================================
   Function          Fields       Examples                  Description/Items to Check
   ================= ============ ========================= ========================================================
   LoadScript        Script name  LoadScript("mcs.script")  Loads a script into the GMAT engine.

                                                            #. Was the script loaded successfully?
                                                            #. Are the scripted objects accessible?
                                                            #. Are scripts that fail to load reported?
   ----------------- ------------ ------------------------- --------------------------------------------------------
   RunScript         *none*       RunScript()               Runs a loaded script

                                                            #. If no script is loaded, is that reported?
                                                            #. Does the script run?
                                                            #. Are expected output files generated?
   ----------------- ------------ ------------------------- --------------------------------------------------------
   SaveScript        script name  SaveScript("mcs2.script") Saves a script, with edits if made.

                                                            #. Is the script saved?
                                                            #. Are edits in the saved script?
   ----------------- ------------ ------------------------- --------------------------------------------------------
   GetRuntimeObject  Object name  GetRuntimeObject("Sat")   Provides access to objects in the GMAT sandbox.

                                                            #. Before running, the Sandbox should not contain the
                                                               scripted objects.  Is that the case?
                                                            #. Can the API access objects after the run?
                                                            #. Do the objects contain the run data?
   ----------------- ------------ ------------------------- --------------------------------------------------------
   GetRunSummary     *none*       GetRunSummary()           Retrieves summary data from the commands in the mission 
                                                            control sequence. 

                                                            #. Is the summary data available for all executed 
                                                               commands?
   ----------------- ------------ ------------------------- --------------------------------------------------------
   UseLogFile        file name    UseLogFile("MyLog.txt")   Sets up a log file for the API session.

                                                            #. Is the log opened and written?
   ----------------- ------------ ------------------------- --------------------------------------------------------
   EchoLogFile       *none*       EchoLogFile()             Writes log file output to the platform console.

                                                            #. Are log messages echoed to the platform terminal?
   ================= ============ ========================= ========================================================



GMAT Classes and the API
========================
The API exposes 481 class names for use with the API.  Testing for these items can be broken into 17 categories.  The notes below provide some guidelines for testing in each category, along with a list of the exposed components as provided by the API's ShowClasses function.  As stated in the introductory text, full test coverage for every GMAT class is not expected for the API test plan.  Instead, coverage by category is needed.  As tests are developed, the test cases use previously untested classes as the basis of the new tests in order to more completely cover the GMAT code base, but full coverage of all classes and methods is not expected.

GMAT class access covers two types of tests.  The API adds features to the core GmatBase class, described in :ref:`APIMethods`.  One set of tests is designed to exercise these methods for components in the system to ensure that the new functions are robust and functional.  

The second test type exercises API use of GMAT objects themselves.  The API methods are used extensively in these tests to work with GMAT objects, but the purpose of this set of tests is to ensure that the API provides users with access to the public features of GMAT objects in each of the class categories presented below.

The general test procedure for GMAT classes and objects is

#. Launch a console for the API platform.
#. Load the GMAT API.
#. Create an object for testing.
#. Exercise the interfaces under test, and ensure that they work as desired.
#. Write test artifacts for use by the automated test system.
#. Close the console.  

GMAT's script test system is used to run the API tests.  That system expects to be passed the name of the API test to be run, the name of the output file that the test run should generate, the name of a "truth" output file used for comparison with the generated file, and the name of the "comparator" that compares the output file with the truth.  API test writers identify these elements in a test comparison file with the file extension ".tc".  This outlined procedure ensures that the test system works as expected.

.. _APIMethods:

API Object Methods
------------------
The GMAT API adds methods to the GmatBase class designed to facilitate use by API consumers.  These methods are built to make use of the API friendlier to users familiar with coding on the API target platforms.  The object methods added to GmatBase for API use are listed in :numref:`ObjectMethods`.

.. _ObjectMethods:
.. table:: Object methods added for the GMAT API
   :class: longtable
   :widths: 15 15 25 45

   ============== ============== ======================= ========================================================
   Method         Fields         Examples                Items to Check
   ============== ============== ======================= ========================================================
   Help           *none*         sat.Help()              By default, provides a list of object fields with their
                                                         types and current values.  This method can be overridden
                                                         (but I don't think that has been done yet).

                                                         #. Does help text display?
                                                         #. Are all scripted fields shown?
                                                         #. Are the types correct?
                                                         #. Are all set values shown?
   -------------- -------------- ----------------------- --------------------------------------------------------
   GetField       Field name     sat.GetField("DryMass") Retrieves the current setting for a field.

                                                         #. Is the value returned as a string?
                                                         #. Is the value correct?
                                                         #. Are all set values accessible?
                                                         #. Can the user access fields using integer IDs?
                                                         #. Are there protected fields that should be made 
                                                            public so API users can access them?
   -------------- -------------- ----------------------- --------------------------------------------------------
   GetNumber      Field name     sat.GetNumber("X")      Retrieves numerical data from real number fields.

                                                         #. Is the value returned as a real number?
                                                         #. Is the value correct?
                                                         #. Does the function report type issues gracefully?  For
                                                            example, if I call "GetNumber" for a string, is there
                                                            a human readable error message that tells me what I 
                                                            did wrong.
   -------------- -------------- ----------------------- --------------------------------------------------------
   SetField       Field name,    sat.SetField("X", 5000) Sets the values for object fields.
                  value                                  

                                                         #. Is the value set?
                                                         #. Is the new value correct?
                                                         #. Does the function report type issues gracefully?  
                                                            e.g. If the user tries setting a string for a numeric
                                                            field, the API should post an easily understood error
                                                            message.
                                                         #. Are invalid entries (that GMAT catches) reported?
   ============== ============== ======================= ========================================================


All "Resource" objects that users can script should have access to these settings in a way that is similar to scripting.  

.. note:: Mission Control Sequence commands may have fields that are not set this way because scripting for commands is performed on single script lines.

Object Categories to Test
=========================
Testing of all 481 exposed classes for the GMAT API would provide extensive overlap with feature tests already built into the GMAT script based test system.  Instead, the API tests are aimed at covering high usage objects (e.g. Spacecraft and dynamics models) and utilities (e.g. time and coordinate system conversion code) heavily, and lower use components more lightly.  Some components - propagators, for example - all work identically, through identical intermediate classes, so their interfaces can be checked as a group rather than class by class.

:numref:`ClassBreakdown` groups the GMAT classes into categories, and provides notes about how to approach each category when writing the corresponding tests.


.. _ClassBreakdown:
.. table:: GMAT class categories.  See :ref:`CategoryNotes` for full lists 
   :class: longtable
   :widths: 25 25 50

   ================================== ======================================= ========================================
   Category                           Member Count                            Coverage Notes
   ================================== ======================================= ========================================
   Coordinate System Classes          20 axis classes, plus CoordinateSystem  Axes are tested in the script tests, 
                                                                              so full coverage is not needed. Test 
                                                                              high usage axes first.  Test conversions
                                                                              between systems.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Participants                       Spacecraft, Formation, GroundStation    Focus on Spacecraft objects, then ground
                                                                              stations
   ---------------------------------- --------------------------------------- ----------------------------------------
   Attitude Classes                   7 classes                               Attitudes are tested in the script  
                                                                              tests, so full coverage is not needed.
                                                                              Focus on nadir pointing and spinner 
                                                                              attitudes
   ---------------------------------- --------------------------------------- ----------------------------------------
   Hardware Classes                   16 classes                              Ensure that hardware connections are 
                                                                              initialized completely by focusing on
                                                                              low thrust components. Use nav tests to
                                                                              check access to sensor/comms classes
   ---------------------------------- --------------------------------------- ----------------------------------------
   Force Modeling                     7 classes, one alias                    Ensure that users can config and 
                                                                              retrieve derivative data.  Numerics are
                                                                              tested in script tests.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Propagator Classes                 12 propagators and a container class    Integrators generally all work the same.
                                                                              Full coverage is not needed.  Focus on
                                                                              one Runge-Kutta and one Ephemeris 
                                                                              propagator, and PropSetup.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Maneuver Definitions               2 classes                               Focus on configuration and linkages.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Solar System Members               7 classes                               Full coverage not needed.  Focus on 
                                                                              access and use of the solar system
                                                                              container.  This coverage can also be
                                                                              tested along with the solar system 
                                                                              testing.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Miscellany                         1 class (Plate)                         Ensure that plates can be configured and
                                                                              attached to spacecraft.
   ---------------------------------- --------------------------------------- ----------------------------------------
   **Lighter Testing Needs** 
   ---------------------------------- --------------------------------------- ----------------------------------------
   Solver Classes                     9 classes plus 8 nav and 10 OC classes  Focus on Nav and OC usage.  Rely on the
                                                                              optimal control group to generate the
                                                                              latter set.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Internal Functions                 15 classes                              Omit or test lightly.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Mission Control Sequence Commands  53 classes                              GMAT Commands are not the focus of 
                                                                              the current API implementation, but 
                                                                              the commands can be accessed using the 
                                                                              API.  Check that the linked lists can be
                                                                              walked in both directions.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Atmosphere Models                  6 classes                               Omit or test lightly, as part of the
                                                                              dynamics model tests.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Calculation Parameter classes      243 Classes                             Omit or test lightly
   ---------------------------------- --------------------------------------- ----------------------------------------
   **No Testing Needs**
   ---------------------------------- --------------------------------------- ----------------------------------------
   Mathematical Operators             42 classes                              Mathematical operations are generally
                                                                              supplied natively by the API platforms,
                                                                              so no tests are needed here.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Subscribers                        15 classes                              Omit.  Outputs are generally built using
                                                                              native platform services.
   ---------------------------------- --------------------------------------- ----------------------------------------
   Miscellany                         3 classes                               Omit.
   ================================== ======================================= ========================================


GMAT Utility Code
=================
GMAT exposes several classes from its utility libraries for API use.  The most notable of these are the conversion classes and time representations, listed in :numref:`UtilityComponents`.

Conversion Utilities
--------------------


.. _UtilityComponents:
.. table:: GMAT Conversion Utilities

   ==================== =====================================================
   Class                Description
   ==================== =====================================================
   CoordinateConverter  Conversion routines between coordinate systems
   -------------------- -----------------------------------------------------
   TimeSystemConverter  Conversion routines between time systems
   ==================== =====================================================



.. _CategoryNotes:

Notes on Testing By Category
============================



Priority Test Cases
-------------------
Coordinate systems consist of a core CoordinateSystem object used to define the origin and other associated settings, and an axis system defining the orientation of the coordinate axes.  The API User's Guide includes examples showing the configuration needed for coordinate systems.

.. csv-table:: Coordinate Systems and Axes

   CoordinateSystem

   MJ2000Eq, MJ2000Ec, TOEEq, TOEEc, MOEEq
   MOEEc, TODEq, TODEc, MODEq, MODEc
   ObjectReferenced, Equator, BodyFixed, BodyInertial, GSE
   GSM, Topocentric, LocalAlignedConstrained, ICRF, BodySpinSun

Most GMAT runs use one or more "participant" objects.  This category includes the Spacecraft and groundstation objects, along with the Formation object used to collect multiple spacecraft together.

.. csv-table:: Participant Classes

   Spacecraft, Formation, GroundStation

The attitude classes set the orientation of a spacecraft in space.

.. csv-table:: Attitude Classes

   CoordinateSystemFixed, Spinner, PrecessingSpinner, NadirPointing, CCSDS-AEM
   SpiceAttitude, ThreeAxisKinematic, , 

Spacecraft sensors and propulsion systems are modeled using the hardware classes.

.. csv-table:: Hardware Classes

   FuelTank, ChemicalTank, ElectricTank, Thruster, ChemicalThruster
   ElectricThruster, NuclearPowerSystem, SolarPowerSystem, Antenna, Transmitter
   Receiver, Transponder, Oscillator

   **Field of View**
   ConicalFOV, RectangularFOV, CustomFOV

API users have direct access, component by component, to the dynamics models used in force modeling and derivative computations.  These pieces are collected together in the propagation system using the ODEModel container class, responsible to combining forces together.  API users can call into individual pieces of the model, or through an ODEModel object that combines them. 

The API User's Guide includes examples that use these classes to model dynamics.

.. csv-table:: Dynamics Modeling

   ODEModel, Alias: ForceModel

   PointMassForce, GravityField, SolarRadiationPressure, DragForce, RelativisticCorrection
   PolyhedronGravityModel

GMAT provides multiple components that move spacecraft through time in the simulation.  The numerical algorithms are implemented in the Propagator classes.  The propagators and dynamics models are collected together using a "PropSetup" object.  The PropSetup connects ODEModel and Propagators together, builds the vector that is propagated, and manages propagation.  The component integration needed for propagation is simplified in the PrepareInternals() method in the PropSetup class.

Note that the ephemeris propagators - the SPK, Code500, and STK propagators - work by interpolating state data from ephemeris files, and therefore do not use dynamics models.  Testing for those pieces should check that there is no dynamics model in use.

The API User's Guide includes examples that use these classes to model dynamics.

.. csv-table:: Propagator Classes

   RungeKutta89, PrinceDormand78, PrinceDormand45, RungeKutta68, RungeKutta56
   AdamsBashforthMoulton, SPK, Code500, STK, PrinceDormand853
   RungeKutta4, BulirschStoer, 

   PropSetup

GMAT identifies the components used during maneuvers through its impulsive and finite burn classes.

.. csv-table:: Maneuver Definitions

   ImpulsiveBurn, FiniteBurn

The space environment is modeled in GMAT using classes tailored to match the bodies and special points in the solar system.  Objects created from these classes are collected together in a SolarSystem container.  In a GMAT run, there is one solar system object used for all modeling in the run. 

.. csv-table:: Solar System Members

   Star, Planet, Moon, Comet, Asteroid
   LibrationPoint, Barycenter

   SolarSystem

The GMAT plate model used for solar radiation pressure modeling (and, eventually, drag modeling) is built using a table of plates attached to a Spacecraft object.

.. csv-table:: Miscellany

   Plate


Additional Test Cases
---------------------

GMAT uses Solvers to tune and optimize trajectories, estimate states, and perform optimal control work.  The API tests for these pieces vary, depending on usage.  :numref:`SolverClasses` identifies the classes that need test cases by setting the class names in **boldface**.  The remaining classes are generally better tested as part of the script test cases because they work by constructing a timeline of commands that work together to perform their tasks. 

.. _SolverClasses:
.. csv-table:: Solver Classes

   DifferentialCorrector, Yukon, SNOPT, VF13ad, FminconOptimizer (Windows only)

   **Navigation Subset**
   *Simulator and Estimators*
   **Simulator**, **BatchEstimator**, **BatchEstimatorInv**, **ExtendedKalmanFilter**, **Smoother**

   *Associated Classes*
   TrackingFileSet, AcceptFilter, RejectFilter, ErrorModel, ProcessNoise-Model
   StateNoiseCompensation, EstimatedParameter

   **Optimal Control Subset**
   FirstOrderGaussMarkov, DynamicsConfiguration, Trajectory, Phase, CustomLinkage-Constraint
   OptimalControlGuess, BoundaryFunction, PathFunction, OptimalControlFunction, EMTGSpacecraft

GMAT contains several internal functions that provide utilities useful in scripting.  The platforms supported by the API provides these capabilities as native services.  Because of that, testing for these functions using the API is not needed.

.. csv-table:: Internal Functions

   GetLastState, GetEphemStates, SetSeed, Pause, pause
   SystemTime, ConvertTime, Sign, sign, Str2num
   str2num, Num2str, num2str, RotationMatrix, GmatFunction

The commands used to script a mission timeline in GMAT are generally tested using the features described in :ref:`ScriptExecution`.

.. csv-table::  Mission Control Sequence Commands
   :class: longtable

   Achieve, Assignment, BeginFiniteBurn, BeginMissionSequence, BeginScript
   CallFunction, CallBuiltinGmatFunction, ClearPlot, Create, Write
   Else, EndFor, EndIf, EndOptimize, EndTarget
   EndWhile, EndScript, EndFiniteBurn, Equation, For
   If, GMAT, Maneuver, MarkPoint, Minimize
   NonlinearConstraint, NoOp, Optimize, PenUp, PenDown
   Propagate, Report, SaveMission, ScriptEvent, Stop
   FindEvents, Target, Toggle, Vary, While
   UpdateDynamicData, Set, CallGmatFunction, Global, CallPythonFunction
   RunEstimator, RunSimulator, CommandEcho, BeginFileThrust, EndFileThrust
   Save, RunSmoother, Collocate

GMAT's atmosphere models are tested as part of the tests of the dynamics models.

.. csv-table:: Atmosphere Models

   Exponential, MSISE90, JacchiaRoberts, MarsGRAM2005, MSISE86
   NRLMSISE00

The calculated parameter tests are performed as part of the earlier test cases.

.. csv-table:: Calculation Parameter Classes (243 Classes)
   :class: longtable
   :widths: 20 20 20 20 20


   Variable, String, Array, ElapsedDays, ElapsedSecs
   CurrA1MJD, A1ModJulian, A1Gregorian, TAIModJulian, TAIGregorian 
   TTModJulian, TTGregorian, TDBModJulian, TDBGregorian, UTCModJulian
   UTCGregorian, X, Y, Z, VX
   VY, VZ, Cartesian, SMA, ECC
   INC, RAAN, RADN, AOP, TA
   MA, EA, HA, MM, Keplerian
   RadApo, RadPer, ModKeplerian RMAG, RA
   DEC, VMAG, RAV, DECV, AZI
   FPA, SphericalRADEC, SphericalAZFPA, Altitude, EquinoctialH
   EquinoctialK, EquinoctialP, EquinoctialQ, MLONG, Equinoctial
   SemilatusRectum, ModEquinoctialF, ModEquinoctialG, ModEquinoctialH, ModEquinoctialK
   TLONG, ModEquinoctial, AltEquinoctialP, AltEquinoctialQ, Delaunayl
   Delaunayg, Delaunayh, DelaunayL, DelaunayG, DelaunayH
   Delaunay, PlanetodeticRMAG, PlanetodeticLON, PlanetodeticLAT, PlanetodeticVMAG
   PlanetodeticAZI, PlanetodeticHFPA, Planetodetic, IncomingRadPer, IncomingC3Energy
   IncomingRHA, IncomingDHA, IncomingBVAZI, IncomingAsymptote, OutgoingRadPer
   OutgoingC3Energy, OutgoingRHA, OutgoingDHA, OutgoingBVAZI, OutgoingAsymptote
   BrouwerShortSMA, BrouwerShortECC, BrouwerShortINC, BrouwerShortRAAN, BrouwerShortAOP
   BrouwerShortMA, BrouwerMeanShort, BrouwerLongSMA, BrouwerLongECC, BrouwerLongINC
   BrouwerLongRAAN, BrouwerLongAOP, BrouwerLongMA, BrouwerMeanLong, VelApoapsis
   VelPeriapsis, Apoapsis, Periapsis, OrbitPeriod, C3Energy
   Energy, HMAG, HX, HY, HZ
   DLA, RLA, MHA, Longitude, Latitude
   LST, BetaAngle, BdotT, BdotR, BVectorMag
   BVectorAngle, Element1, Element2, Element3, V
   N, B, TotalMassFlowRate, TotalAcceleration1, TotalAcceleration2
   TotalAcceleration3, TotalThrust1, TotalThrust2, TotalThrust3, DCM11
   DCM12, DCM13, DCM21, DCM22, DCM23
   DCM31, DCM32, DCM33, EulerAngle1, EulerAngle2
   EulerAngle3, MRP1, MRP2, MRP3, Q1
   Q2, Q3, Q4, Quaternion, AngularVelocityX
   AngularVelocityY, AngularVelocityZ, EulerAngleRate1, EulerAngleRate2, EulerAngleRate3
   DryMass, Cd, Cr, DragArea, SRPArea
   TotalMass, SPADDrag-ScaleFactor, SPADSRP-ScaleFactor, AtmosDensity-ScaleFactor, CdSigma
   CrSigma, SPADDragScale-FactorSigma, SPADSRPScale-FactorSigma, AtmosDensityScale-FactorSigma, OrbitSTM
   OrbitSTMA, OrbitSTMB, OrbitSTMC, OrbitSTMD, OrbitErrorCovariance
   FuelMass, Pressure, Temperature, RefTemperature, Volume
   FuelDensity, DutyCycle, ThrustScaleFactor, GravitationalAccel, ThrustMagnitude
   Isp, MassFlowRate, C1, C2, C3
   C4, C5, C6, C7, C8
   C9, C10, C11, C12, C13
   C14, C15, C16, K1, K2
   K3, K4, K5, K6, K7
   K8, K9, K10, K11, K12
   K13, K14, K15, K16, ThrustDirection1
   ThrustDirection2, ThrustDirection3, TotalPowerAvailable, RequiredBusPower, ThrustPowerAvailable
   Acceleration, AccelerationX, AccelerationY, AccelerationZ, AtmosDensity
   SolverStatus, SolverState, SurfaceHeight


Low Priority Tests
------------------
The platforms supported by the API provides these capabilities as native services.  Because of that, testing for these functions using the API is not needed.

.. csv-table:: Mathematical Operators

   MathElement, Add, Divide, Multiply, Negate
   Subtract, Abs, Ceil, Exp, Fix
   Floor, Log, Log10, Min, Mod
   Power, Sqrt, Cross, Det, Diag
   Inv, Norm, Rand, Randn, Transpose
   Sin, Cos, Tan, Asin, Acos
   Atan, Atan2, DegToRad, RadToDeg, Deg2Rad
   Rad2Deg, Sprintf, Strcat, Strcmp, Strfind
   Strrep, FunctionRunner

Output for API use cases is generally different from the services supplied by GMAT's subscriber classes.  Because of that, testing for these functions using the API is not needed.


.. csv-table:: Subscribers

   ReportFile, TextEphemFile, MessageWindow, XYPlot, EphemerisFile
   OpenGLPlot, Enhanced3DView, OrbitView, GroundTrackPlot, OwnedPlot
   DynamicDataDisplay, FileInterface, ThrustHistoryFile, ThrustSegment

The event location services are driven as post processing features in GMAT, and not tested as part of the API tests.

.. csv-table:: Miscellany

   StopCondition, EclipseLocator, ContactLocator, ,


