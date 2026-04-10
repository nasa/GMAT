The GMAT Help system provides help text for several categories of topics.  This 
file provides that help.  Topics are identified by restructured text reference
labels.  The help itself is provided verbatim following each label.

.. _TopLevel:

---------------------------------------
GMAT Application Programmer's Interface
---------------------------------------

The GMAT Application Programmer's Interface (API) is a set of tools that can be 
used to exercise GMAT capabilities from other platforms.  GMAT releases include 
API builds for Java, Python, and MATLAB*.  

This help system includes high level help on some topics of general interest.  
Topic specific help is accessed by identifing the topic in the call to the help 
command.  For example, help for the API commands is presented for a Python user 
when the user requests it like this:

   >>> import gmatpy as gmat
   >>> gmat.Help("Commands")

The following topics are available for more information:

  Commands          API specific commands
  Groups            Provides a list of categories of objects you can create
  Objects           Lists the objects you have built using the Construct command
  ScriptUsage       Help using the API to drive GMAT scripts 





+-+-------------------------------------------------------------------+
| | * Users working from other platforms can use the Simplified       |
| | Wrapper and Interface Generator (SWIG) tool used for these builds |
| | to create the GMAT API for their platform.  The Python and Java   |
| | SWIG files are included in the GMAT source code releases,         |
| | starting with the 2019 GMAT release.                              |
+-+-------------------------------------------------------------------+

.. _ScriptUsage:

-------------------
GMAT API Script Use
-------------------

The GMAT API can be used to drive GMAT scripts using the following commands:

  LoadScript(file)  Loads a script file into the system
  RunScript         Runs a loaded script
  SaveScript(file)  Saves a script to the selected file
  GetRuntimeObject  Retrieves an object in its start following a run
  GetRunSummary     Retrieves the states of objects at the end of each command 
                    executed
  
Sample Use Case:
****************

Load the GEO transfer sample mission, run it, and display the targeted transfer 
orbit insertion maneuver.

Sample Usage (Python):
   
   >>> import gmatpy as gmat
   >>> gmat.LoadScript("../samples/Ex_GEOTransfer.script")
   True
   >>> gmat.RunScript()
   True
   >>> toi = gmat.GetRuntimeObject("TOI")
   >>> print(toi.GetGeneratingString(0))
   
   %----------------------------------------
   %---------- Burns
   %----------------------------------------
   
   Create ImpulsiveBurn TOI;
   GMAT TOI.CoordinateSystem = Local;
   GMAT TOI.Origin = Earth;
   GMAT TOI.Axes = VNB;
   GMAT TOI.Element1 = 2.818672162431357;
   GMAT TOI.Element2 = 0;
   GMAT TOI.Element3 = 0;
   GMAT TOI.DecrementMass = false;
   GMAT TOI.Isp = 300;
   GMAT TOI.GravitationalAccel = 9.81;

Sample Usage (MATLAB):

   >> [myMod, gmatStartupPath, result] = load_gmat('../samples/Ex_GEOTransfer.script');
   Initialize Moderator Status: 1
   Interpret Script Status: 1
   >> gmat.gmat.RunScript()
   
   ans =
   
     logical
   
      1
   
   >> toi = gmat.gmat.GetRuntimeObject('TOI');
   >> toi.GetGeneratingString(0)
   
   ans =
   
   
   %----------------------------------------
   %---------- Burns
   %----------------------------------------
   
   Create ImpulsiveBurn TOI;
   GMAT TOI.CoordinateSystem = Local;
   GMAT TOI.Origin = Earth;
   GMAT TOI.Axes = VNB;
   GMAT TOI.Element1 = 2.818672162431357;
   GMAT TOI.Element2 = 0;
   GMAT TOI.Element3 = 0;
   GMAT TOI.DecrementMass = false;
   GMAT TOI.Isp = 300;
   GMAT TOI.GravitationalAccel = 9.81;


.. _Commands:

-----------------
GMAT API Commands
-----------------

The GMAT API provides a set of commands that can be used to drive GMAT from
Python or Java for API users that rely on the underlying GMAT system for 
component management.  These commands can be broken into two groups: API
system commands that drive the "GMAT Engine," and commands used to access GMAT 
classes and objects. 

*******************
System API Commands
*******************
  Help                  Provides hierarchical API help information
  Setup                 Prepares the API or created objects for use
  Construct(type,name)  Makes GMAT objects that are used with the API
  Copy(obj,name)        Constructs a copy of a GMAT object
  Initialize            Prepares objects for use.  Initialize is called before 
                        using objects created using the Construct function to 
                        establish object-to-object connections.  

                        Initialize is called for Script based user automatically 
                        when a script is run.  
  Update                Function to assist updating certain resources by taking
                        multiple function calls to modify internal properties
                        of the resource and combining them into one function

*******************
Classes and Objects
*******************
  ShowClasses           Displays a list of creatable classes
  ShowClasses(type)     Displays a list of creatable classes of a given type
  GetObject(name)       Gets a constructed object
  ShowObjects           Displays a list of constructed objects
  ShowObjects(type)     Displays a list of constructed objects of a given type



.. _Groups:

------------------------
GMAT API Class Groupings
------------------------

Help is avaialble for the following GMAT object types:

  *Note: A Work in progress!*
  Forces            Force and force model objects
  Propagators       Numerical integrators and analytic propagators
  Coordinates       Coordinate Systems


.. _Objects:

-------------------
Constructed Objects
-------------------

.. <Online>
   The following objects have been constructed:

   <OBJECTLIST UnknownObject>

GMAT objects built using the Construct command are resources that are stored in 
the GMAT "configuration."  The current list of available objects can be accessed 
using the ShowObjects function:

   ShowObjects()

A list of objects of a specified type can be shown by specifying the class of
the object, like this:

   ShowObjects("Spacecraft")

.. _Coordinates:

------------------
Coordinate Systems
------------------

**Configuration and use of the GMAT coordinate systems is planned for 
implementation in the second build of the API.  The following text is a
preview of the planned implementation.**


GMAT coordinate systems are defined as a core container object and associated 
references that specify the system's origin and orientation in a variety of
different ways.  

The GMAT User's Guide provides information about how a user builds these 
systems, and the GMAT Mathematical Specification provides details about how 
they are implemented.  

This help topic shows how to define a GMAT coordinate system in the API.  Note 
that, before use, all coordinate systems must be initialized using either the 
Initialize command* or through a script run so that object-to-object connections 
are established.

.. <Online>
   The following coordinate systems have been constructed in this API run:

   <OBJECTLIST CoordinateSystem>

From the API, an Earth centered Mean of J2000 coordiante system can be built 
using the syntax

   >>> import gmatpy as gmat
   >>> eci = gmat.Construct("CoordinateSystem", "ECI")
   >>> eci.SetField("Origin","Earth")
   True
   >>> eci.SetField("Axes","MJ2000Eq")
   True
   >>> gmat.Initialize()

The GMAT scripting for this object is nearly identical:

   Create CoordinateSystem ECI;
   GMAT ECI.Origin = Earth;
   GMAT ECI.Axes = MJ2000Eq;

Other coordiante systems are built similarly.  An object referenced coordinate 
system scripted as 

   Create CoordinateSystem Sat1Sat2;
   GMAT Sat1Sat2.Origin = Sat1;
   GMAT Sat1Sat2.Axes = ObjectReferenced;
   GMAT Sat1Sat2.XAxis = R;
   GMAT Sat1Sat2.ZAxis = N;
   GMAT Sat1Sat2.Primary = Sat1;
   GMAT Sat1Sat2.Secondary = Sat2;

in GMAT is built in the API using the commands

   >>> import gmatpy as gmat
   >>> sat1sat2 = gmat.Construct("CoordinateSystem", "Sat1Sat2")
   >>> sat1sat2.SetField("Origin","Sat1")
   >>> sat1sat2.SetField("Axes","ObjectReferenced")
   >>> sat1sat2.SetField("XAxis","R")
   >>> sat1sat2.SetField("ZAxis","N")
   >>> sat1sat2.SetField("Primary","Sat1")
   >>> sat1sat2.SetField("Secondary","Sat2")
   >>>gmat.Initialize()
   >>>
   
Once configured, conversions between coordinate systems can be coded usign the 
syntax described in the API User's guide.

.. _Forces:

------
Forces
------

Forces are accumulated in ODEModel objects.  You need to create an ODEModel 
object first, like this:

   myForceModel = gmat.Construct("ODEModel", "Forces")

and then create and add each force separately to your ODEModel, like this:

   force = gmat.Construct("PointMassForce", "Earth")
   myForceModel.AddForce(force)

.. <Online>
   The following forces can be built: 

   <CLASSLIST PhysicalModel>

GMAT forces are all based on a core component, PhysicalModel.  The current 
list of available forces can be accessed using the ShowClasses function:

   ShowClasses("PhysicalModel")

   
.. _Propagators:

-----------
Propagators
-----------

GMAT's propagators provide a set of tools used to move a Cartesian state 
through time.  In general, GMAT's propagators are used together with a 
Spacecraft object to generate the spacecraft's trajectory.  

GMAT provides both numerical integrators and ephemeris based propagators that 
are used for this purpose.  Assembly of the full propagation system for a 
numerical integrator requires the configuration of a dynamics model, the 
integrator, connection to a Spacecraft object that supplies force model 
parameters (like the areas and coefficients for drag and solar radiation 
pressure modeling), and the environmental connections supplying the solar system 
settings for the model.  The full configuration for propagation is documented in 
the propagation section (accessed using Help("Propagation")).

.. <Online>
   The following propagators can be built: 

   <CLASSLIST Propagator>

As an example, the syntax 

   myPropagator = gmat.Construct("PrinceDormand78", "myPropagator")

is used to create a Pronce-Dormand 7(8) Runge-Kutta propagator for use.  The 
dynamics model is assocated with this propagator using the syntax

   myForceModel = gmat.Construct("ODEModel", "Forces")
   myPropagator.AddField("FM", "Forces")

The complete list of GMAT propagators that can be used is listed with the 
command

   ShowClasses("Propagator")


.. _Propagation:

----------------------
Spacecraft Propagation
----------------------

**Configuration and use of the full GMAT propagation system is planned for 
implementation in the second build of the API.  The following text is a
partial preview of the planned implementation.**

Spacecraft propagation requires configuration of at least two objects - for
ephemeris based propagation - and up to three types of objects, with subobjects, 
for numerical integration.  This text describes the procedure followed to set up 
and execute numerical integration outside of the context of a GMAT script.


Objects Required for Numerical Integration
******************************************

Numerical integration of a spacecraft requires three objects: the spacecraft, a 
numerical integrator, and a dynamics model.  The dynamics model requires 
addition of the components needed for propagation - that is, the forces used for
force modeling, and Jacobians used when modelign either the A-matrix or the
State Transition Matrix.  For some types of propagation, the Spacecraft needs 
configuration of the attached hardware elements - tanks if fuel mass is included
in the modeling, and thrusters if finite burn based accelerations are included.
All of these objects need to be interconnected to drive the propagation system.
The interconnections are managed in the API using a command defined for the
process, Initialize().

Part 1: Setup for Propagation
*****************************

As a preview of the implementation process, the following sample code configures 
the components needed for propagation.  Note that this section is 
pre-implementation, and subject to change.

   myPropagator = gmat.Construct("PrinceDormand78", "myPropagator")

   myForceModel = gmat.Construct("ODEModel", "Forces")
   epm = gmat.Construct("PointMassForce", "Earth")
   lpm = gmat.Construct("PointMassForce", "Luna")
   spm = gmat.Construct("PointMassForce", "Sun")
   myForceModel.AddForce(epm)
   myForceModel.AddForce(lpm)
   myForceModel.AddForce(spm)

   myPropagator.SetField("FM", "Forces")
   
   sat = gmat.Construct("Spacecraft", "Sat")
   myForceModel.SetField("Spacecraft", sat)
   
Part 2: Spacecraft Propagation
******************************

Once the objects used for propagation have been defined, the object state data 
is et, the Initialize command called, and propagation executed using the Step
method on the integrator:  

   sat.SetField("Epoch", "28636.000")
   sat.SetField("X", 42165.0)
   sat.SetField("Y", 0.0)
   sat.SetField("Z", 15.0)
   sat.SetField("VX", 0.0)
   sat.SetField("VY", 3.076)
   sat.SetField("VZ", 0.0)
   
   gmat.Initialize()
   myPropagator.Step(60.0)
   
Note that fixed step integration specifies the time step to use (in seconds).
Variable step propagation simply leaves that entry blank:

   myPropagator.Step()
