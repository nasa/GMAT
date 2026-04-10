.. _PythonExamples:

=========================================
API Examples
=========================================

Four Python sample use cases were coded using the prototype SWIG API to act as 
a guide to addressing the changes that are needed for the production system.  
These cases ranged from a trivial time system conversion use case to a full 
propagation use case.  These cases, shown in :ref:`SwigOldAndNew`, were then 
reworked into the API syntax documented here.  The following section previews 
the changes coming to the GMAT API by presenting each of these cases as planned 
for the API.  The examples presented here are in Python.  Java examples are 
presented in :ref:`JavaExamples`.

Case 1: Time System Conversion
----------------------------------------

GMAT supports five time systems: A.1 Atomic Time (A1), International Atomic Time 
(TAI), Coordinated Universal Time (UTC), Barycentric Dynamical Time (TDB), and 
Terrestrial Time (TT).  Times in GMAT are stored internally in a modified Julian 
format, referenced to January 5, 1941 at noon.  Conversions between these time 
systems are performed using a time system converter, coded in the TimeSystemConverter 
singleton class.  The time system converter also provides routines to convert 
between modified Julian representations and Gregorian representations.

The simplest usage of the time system converter using GMAT's API consists
of two lines of code; lines 5 and 9 shown here:

.. code-block:: python
   :caption: Python code for time conversions using the GMAT API
   :linenos:

   import gmatpy as gmat
   gmat.Initialize()

   # Get the converter   
   timeConverter = gmat.TimeSystemConverter.Instance()   

   # Convert an epoch
   UTCEpoch = 21738.22145
   TAIEpoch = timeConverter.Convert(UTCEpoch, gmat.TimeSystemConverter.UTC, gmat.TimeSystemConverter.TAI)

This code shows two features of the API.  The first line shows how a Python 
user loads the GMAT system and initializes it for use.  As part  of the 
initialization process, several components are created in the GMAT module 
that are single instance objects, following a singleton design pattern.  These 
singletons are accessed from the API by calling .Instance() on their names. 
Line 5 is an example of this usage.  The time system converter 
singleton is accessed using the object name TimeSystemConverter.  In the 
python code, the object "timeConverter" is connected to the singleton, and 
then used to convert a UTC epoch to the TAI time system on line 9.

At this point, the singleton is ready for the user to ineract with it 
directly.  Using the API, the conversion is immediately available: ::

   >>> timeConverter.ConvertMjdToGregorian(UTCEpoch)   
   '12 Jul 2000 17:18:53.280'
   >>> TAIEpoch = timeConverter.Convert(UTCEpoch,2,1)
   >>> timeConverter.ConvertMjdToGregorian(TAIEpoch)
   '12 Jul 2000 17:19:25.280'

The interactive call shows the correct time system difference arising from the 
number of leap seconds needed to convert from UTC time to TAI time.  Users can
access the leap second count at a specified epoch directly as well: ::

   >>> timeConverter.NumberOfLeapSecondsFrom(TAIEpoch)
   32.0

Case 2: Coordinate System Conversion
----------------------------------------

The time system converter is a stand alone component in GMAT.  It does not 
require external components to perform conversions.  Coordinate systems are more 
complex.  They require connections to other objects in order to compute data.  
:numref:`CoordinateSystemSettings` shows the settings, required and optional, to 
define a GMAT coordinate system.

.. _CoordinateSystemSettings:
.. table:: Coordinate System Settings
   :widths: 15 18 14 53

   +--------------+---------------------------+-----------+-------------------------------------------------+
   | Field        | Type                      | Required? | Description                                     |
   +==============+===========================+===========+=================================================+
   | AxisType     | AxisSystem object         | Yes       | Defines the orientation of the coordinate axes  |
   +--------------+---------------------------+-----------+-------------------------------------------------+
   | Origin       | SpacePoint object         | Yes       | Defines the coordinate system origin            |
   +--------------+---------------------------+-----------+-------------------------------------------------+
   | Primary      | SpacePoint object         | No        | Reference body used in coordinate systems that  |
   |              |                           |           | need a primary body                             |
   +--------------+---------------------------+-----------+-------------------------------------------------+
   | Secondary    | SpacePoint object         | No        | Reference body used in coordinate systems that  |
   |              |                           |           | need a secondary body                           |
   +--------------+---------------------------+-----------+-------------------------------------------------+
   | J2000 Body   | Internal reference object | Yes       | Reference origin in GMAT (always set to Earth)  |
   +--------------+---------------------------+-----------+-------------------------------------------------+
   | Solar System | Solar System object       | Yes       | The solar system used in the run                |
   +--------------+---------------------------+-----------+-------------------------------------------------+

Coordinate systems are defined in GMAT as a collection of objects, using a core
composite component that collect together the axis system defining the 
directions for the coordinate system axes, the bodies used to set the coordinate 
system origin and the axis references, and core GMAT settings used to tie the
coordinate system into the rest of the executing GMAT code.  Many of the user 
objects in GMAT have a structure like this: a core object that uses other 
objects to form a composite component consistent with the rest of the running 
GMAT system.

GMAT performs conversions between coordinate systems using a coordinate system 
converter, coded in the CoordinateConverter class.  The CoordinateConverter
class maintains state information about the most recent conversion performed.
This state data would cause issues using a state converter in a single instance 
context, because the state data from one conversion could be accessed in code 
requesting the state data from a second conversion.  For that reason, the 
CoordinateConverter class does not provide a singleton instance, and a separate
object must be created for each use.

A basic use case for the coordinate system converter takes a state in 
Earth-centered Mean-of-J2000 Equatorial coordinates and converts the state into
Earth-centered Earth-fixed coordinates.  The Python code demonstrating this 
conversion using the GMAT API is

.. code-block:: python
   :caption: Python code for coordinate system conversions
   :linenos:

   import gmatpy as gmat

   # Setup the GMAT data structures for the conversion
   mjd   = gmat.A1Mjd(22326.977184)
   rvIn  = gmat.Rvector6(6988.427, 1073.884, 2247.333, 0.019982, 7.226988, -1.554962)
   rvOut = gmat.Rvector6(0.0, 0.0, 0.0, 0.0, 0.0, 0.0)
   UTCEpoch = 19053.293

   # Create the converter
   csConverter = gmat.CoordinateConverter()

   # Create the input and output coordinate systems
   eci  = gmat.Construct("CoordinateSystem", "ECI", "Earth", "MJ2000Eq")
   ecef = gmat.Construct("CoordinateSystem", "ECEF", "Earth", "BodyFixed")
   gmat.Initialize()
   csConverter.Convert(UTCEpoch, rvIn, eci, rvOut, ecef)

The key features shown in this example for the API are:

* Default objects (like the solar system and Earth objects) are set 
  automatically.
* GMAT objects are built using the Construct() command.


Cases 3 and 4: Force Modeling and Propagation
----------------------------------------------

Force models in GMAT -- or, more properly, dynamics models -- are built by 
creating an object from the force container class, ODEModel, and adding the 
constituent forces to that container.  The ODEModel object is responsible for
accumulating the dynamics into a derivative vector.  The dynamics are computed 
when the GetDerivatives() method is called on the object.  The resulting 
computation is stored in a class member, accessible using the 
GetDerivativeArray() method.

The ODEModel class is one component of a more complicated propagation subsystem 
in GMAT.  That subsystem is designed with spacecraft propagation in mind.  Force
modeling requires an associated spacecraft object.  During initialization, an 
instance of the GMAT helper class, PropagationStateManager (PSM), is used to 
collect data and assemble the state vector used to evaluate the dynamics.  The 
PSM determines the size of the state vector by checking to see if the 6 element 
Cartesian state, mass flow from spacecraft tanks, and the state transition 
matrix or state Jacobian (A-matrix) are needed during the propagation.  Once the 
size of the propagation state vector is determined, the complete vector is 
assembled and initialized, and only then can the dynamics be evaluated.

The steps required for this initialization are largely implemented behind the 
scenes in the GMAT API.  Users that want to manage this setup by hand are 
referred to the sample code in :ref:`SwigOldAndNew`.  The third example in that
chapter shows force model configuration.  API users can access GMAT's dynamics 
models by configuring the forces piece by piece and assigning them to an
ODEModel container.  Here is an example of this process for an Earth point mass 
force model:

.. code-block:: python
   :caption: Python code for force modeling using the API
   :linenos:

   import gmatpy as gmat

   sat = gmat.Construct("Spacecraft", "Sat")
   sat.SetField("Cr", 1.4)
   sat.SetField("SRPArea", 6.5)
   sat.SetField("DryMass", 225)

   psm = gmat.PropagationStateManager()
   psm.SetObject(sat)
   psm.BuildState()

   dynamics = gmat.Construct("ODEModel", "EPM_SRP")
   epm = gmat.PointMassForce("EarthPointMass")
   srp = gmat.SolarRadiationPressure("SRP")
   dynamics.AddForce(epm)
   dynamics.AddForce(srp)
   dynamics.SetPropStateManager(psm)
   dynamics.SetState(psm.GetState())

   pstate = [7000.0, 0.0, 1000.0, 0.0, 8.0, -0.25]

   gmat.Initialize()

   dynamics.BuildModelFromMap()
   dynamics.UpdateInitialData()

   dynamics.GetDerivatives(pstate, 0.0)
   pderiv = dynamics.GetDerivativeArray()


The setup for dynamics modeling above extends with little additional 
configuration to numerical integration.  The integration piece of the 
configuration adds the lines

.. code-block:: python
   :caption: Propagator setup and use in the API
   :linenos:

   prop = gmat.Construct("PrinceDormand78", "MyIntegrator")
   prop.SetPhysicalModel(dynamics)

   gmat.Initialize()

   for i in range(10):
      prop.Step(60.0)

To summarize: The goal of the GMAT API is to make API based configuration as 
simple as possible, while maintaining full access to the capabilities of GMAT.  
Towards that end, the design of the API can be illustrated through a 
representative example implementation of a propagation problem.  A reference 
propagation in the GMAT API looks like this:

.. code-block:: python
   :caption: The Propagation example in Python
   :linenos:

   import gmatpy as gmat

   # Set up the spacecraft
   sat = gmat.Construct("Spacecraft", "Sat")
   sat.SetField("Cr", 1.4)
   sat.SetField("SRPArea", 6.5)
   sat.SetField("DryMass", 225)

   # Make a force container and set its spacecraft
   dynamics = gmat.Construct("ODEModel", "EPM_SRP")
   dynamics.SetObject(sat)

   # Set the forces
   epm = gmat.PointMassForce("EarthPointMass")
   srp = gmat.SolarRadiationPressure("SRP")
   dynamics.AddForce(epm)
   dynamics.AddForce(srp)

   # Propagator configuration
   prop = gmat.Construct("PrinceDormand78", "MyIntegrator")
   prop.SetPhysicalModel(dynamics)

   gmat.Initialize()

   # Number of steps to take
   count = 60

   for i in range(count):
       prop.Step(60.0)


Cases 5: Working with a GMAT Script
----------------------------------------------
The script examples above show how an API user interacts with GMAT components
directly.  This final example shows how a user can work with an existing GMAT
script that needs to change settings on one of the scripted objects.  For this
example, the GMAT sample mission that demonstrates finite burns is used.  The
full script is the Ex_FiniteBurn.script file in the GMAT samples folder.  Part
of that script includes the definition of a chemical thruster, shown through the
thrust vector portion here:

.. code-block:: python
   :caption: The Thruster in GMAT's FiniteBurn Sample Mission
   :linenos:

   Create ChemicalThruster engine1;
   GMAT engine1.CoordinateSystem = Local;
   GMAT engine1.Origin = Earth;
   GMAT engine1.Axes = VNB;
   GMAT engine1.ThrustDirection1 = 1;
   GMAT engine1.ThrustDirection2 = 0;
   GMAT engine1.ThrustDirection3 = 0;
   ...

An API user might want to change the thrust direction before running the script. 
The following API code loads the script, adds an orbit normal component to the 
thrust direction and then runs the script.

.. code-block:: python
   :caption: Changing an Object and then Running a Script
   :linenos:

   import gmatpy as gmat

   gmat.LoadScript("../samples/Ex_FiniteBurn.script")

   Thruster = gmat.GetObject("engine1")
   Thruster.SetField("ThrustDirection2", 1.0)

   gmat.RunScript()
