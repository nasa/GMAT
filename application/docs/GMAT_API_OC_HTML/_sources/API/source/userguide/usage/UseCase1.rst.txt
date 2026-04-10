.. _UseCase1:

************************************************************
Tutorial: Accessing GMAT Propagation and Navigation Features
************************************************************

The use case demonstration for the first GMAT API Beta build focuses on using 
the API for models that exercise core GMAT functions.  For the purposes of this 
release of the API, the target use case suite shows how to setup and use force 
models, propagators, and measurement models.  The implemented functionality in 
the Beta release includes the ability to drive GMAT scripts as well.  That 
capability is described separately in :ref:`ScriptUsage`.

The goal of the use cases described here is to build a GMAT measurement model 
from core components, using the API functions.  GMAT's measurement models 
require a propagation component in order to solve the light transit time portion
of the modeling.  Propagation requires configuration of both a numerical 
integrator and of a force model, along with a spacecraft that supplies model 
parameters.  This guide will walk Python and MATLAB users through the process of 
configuring these components in order to assemble a range measurement.

The problem demonstrated in the example code here provides modeling for an Earth
orbiting spacecraft, "EarthOrbiter."  The spacecraft is an 80 kg vehicle modeled 
in a polar orbit with a 6600 km semimajor axis.  That ensures that we need to 
configure a fair number of system parameters in order to build the simulation.

Verifying Setup
---------------
The GMAT API is included in releases of GMAT beginning with the R2020a release 
of the system. Once GMAT is installed, the API can be accessed from inside of 
the folder containing the GMAT application.  

*  For Python, change directories to the GMAT bin folder and access the API help 
   system:

   .. code-block:: python

      >>> import gmatpy as gmat
      >>> gmat.Help()

      ---------------------------------------
      GMAT Application Programmer's Interface
      ---------------------------------------
      ...


*  For MATLAB users

   * Start MATLAB
   * Change directories to the installed GMAT bin directory
   * Load the API and access the help system:

   .. code-block:: matlab

      >> load_gmat()
      No script provided to load.

      ans =

      Instance of GMAT Moderator is initialized. No script ready to run.

      >> GMATAPI.Help()

      ans =


      ---------------------------------------
      GMAT Application Programmer's Interface
      ---------------------------------------
      ...

Getting Started with the API
----------------------------
.. note::

   This section introduces several API functions and tools for users new to GMAT's
   API by experimenting interactively with the system in Python.  This section 
   introduces interaction with the GMAT API, but is not necessary for the 
   force model, propagation, and measurement configuration described below. 

The modeling for this use case uses an Earth centered coordinate system with 
axes oriented in the mean of J2000 Equatorial frame.  The coordinate system can 
be built using the Construct function of the API.  The Python code for this 
object creation is:

.. code-block:: python

  import gmatpy as gmat
  eartheq = gmat.Construct("CoordinateSystem", "EarthMJ2000Eq", "Earth", "MJ2000Eq")

Users can view the list of objects that have been built during a run using the 
ShowObjects() function:

.. code-block:: python

   >>> gmat.ShowObjects()
   Current GMAT Objects

      EarthMJ2000Eq

   The SolarSystem contains the following bodies:

      [Sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Pluto, Luna]
   >>>

The code above created a coordinate system object, an axis system object, and 
connected those objects together for later use.  The object cannot be fully 
exercised at this point because it is missing some key connections to the modeled
space environment, consisting of a solar system model, member planets and the 
Sun and Moon, along with other internal objects used to tie a user's components 
together.  

For this example, the solar system is not yet connected to the new coordinate 
system, and the bodies needed for use - the Earth, for example - are also not 
yet connected.  The coordinate system object's state can be checked using its 
"IsInitialized()" method:

.. code-block:: python

   >>> eartheq.IsInitialized()
   False

Objects that are ready for use return True from this call.  The API prepares the
objects for use with the Initialize() function:

.. code-block:: python

   >>> gmat.Initialize()
   >>> eartheq.IsInitialized()
   True

The Initialize() function prepares all of the objects that the user has created
for use, and reports any objects that could not be prepared because of missing 
settings.

Objects can be removed individually from GMAT using the Clear(*ObjectName*) 
function, or all once using the Clear() function without specifying an object:

.. code-block:: python

   >>> gmat.ShowObjects()
   Current GMAT Objects

      EarthMJ2000Eq

   >>> gmat.Clear("EarthMJ2000Eq")
   'The object EarthMJ2000Eq has been removed from GMAT.'
   >>> gmat.ShowObjects()
   Current GMAT Objects

   >>> eartheq = gmat.Construct("CoordinateSystem", "EMJ2k", "Earth", "MJ2000Eq")
   >>> gmat.ShowObjects()
   Current GMAT Objects

      EMJ2k

   >>> gmat.Clear()
   'All configured objects have been removed from GMAT.'
   >>> gmat.ShowObjects()
   Current GMAT Objects


   >>> 

The full set of GMAT API commands are described in :ref:`APIFunctions`.

The functions used above are presented working interactively in the GMAT API.  
In the remainder of this section, the configuration is built in a Python script 
included with the GMAT release in the API folder.  Matching MATLAB .m scripts 
are included in the API folder for users more comfortable working in that 
environment.

Spacecraft Configuration
------------------------
.. note::

   The Spacecraft Configuration and Force Model Setup examples are located in 
   the Ex_R2020a_BasicForceModel.py file.  An example of the full configuration, showing 
   one configuration for the exercises, is in the Ex_R2020a_CompleteForceModel file.

   To use one of these files, copy the file you plan to use into GMAT's bin 
   folder.

Spacecraft configuration requires that we define the initial spacecraft state 
data for the orbiter, and then configure the spacecraft properties needed for 
the force modeling.

Construction and The Initial State
++++++++++++++++++++++++++++++++++

Using this coordinate system, the spacecraft state can be configured.  For this 
example, the spacecraft is in a circular polar orbit at the moon on July 20, 
2020.  The orbital state is set using the scripting

.. code-block:: python

   # Load GMAT into memory
   import gmatpy as gmat

   # Spacecraft configuration preliminaries
   earthorb = gmat.Construct("Spacecraft", "EarthOrbiter")
   earthorb.SetField("DateFormat", "UTCGregorian")
   earthorb.SetField("Epoch", "20 Jul 2020 12:00:00.000")

   earthorb.SetField("CoordinateSystem", "EarthMJ2000Eq")
   earthorb.SetField("DisplayStateType", "Keplerian")

   # Orbital state
   earthorb.SetField("SMA", 6600)
   earthorb.SetField("ECC", 0.05)
   earthorb.SetField("INC", 78)
   earthorb.SetField("RAAN", 45)
   earthorb.SetField("AOP", 90)
   earthorb.SetField("TA", 180)

Additional Spacecraft Parameters
++++++++++++++++++++++++++++++++
The force model used for this example may include full field Earth gravity, point
mass effects from the Sun and Moon ("Luna" for GMAT), Jacchia-Roberts drag, and 
solar radiation pressure.  The latter forces require settings for the 
reflectivity and drag coefficients of the spacecraft, its surface 
areas for those forces, and the spacecraft mass.  These settings are made using
the SetField method, and resemble the corresponding GMAT scripting:

.. code-block:: python

   # Spacecraft ballistic properties for the SRP and Drag models
   earthorb.SetField("SRPArea", 2.5)
   earthorb.SetField("Cr", 1.75)
   earthorb.SetField("DragArea", 1.8)
   earthorb.SetField("Cd", 2.1)
   earthorb.SetField("DryMass", 80)

For reference, the GMAT scripting for this configuration is

.. code-block:: matlab

   Create Spacecraft EarthOrbiter;
   GMAT EarthOrbiter.DateFormat = UTCGregorian;
   GMAT EarthOrbiter.Epoch = '20 Jul 2020 12:00:00.000';
   GMAT EarthOrbiter.CoordinateSystem = MoonEc;
   GMAT EarthOrbiter.DisplayStateType = Keplerian;
   GMAT EarthOrbiter.SMA = 4000;
   GMAT EarthOrbiter.ECC = 0.05;
   GMAT EarthOrbiter.INC = 78;
   GMAT EarthOrbiter.RAAN = 45;
   GMAT EarthOrbiter.AOP = 90;
   GMAT EarthOrbiter.TA = 180;
   GMAT EarthOrbiter.DryMass = 80;
   GMAT EarthOrbiter.Cd = 2.1;
   GMAT EarthOrbiter.Cr = 1.75;
   GMAT EarthOrbiter.DragArea = 1.8;
   GMAT EarthOrbiter.SRPArea = 2.5;


.. note:: **Differences between Python and MATLAB code**

   The code shown above and throughout this section is Python code.  In the 
   MATLAB version of this example, there are several differences worth noting 
   because of the platform differences:

   #. GMAT is loaded using the MATLAB load_gmat.m script.  This script loads
      the GMAT libraries into MATLAB, initializes the GMAT system by reading a
      startup file and loading plugins and configuration data identified in that
      file, and optionally loads a GMAT script into memory.
   #. Access to the GMAT functions is made using a call into a nested "gmat" 
      class built for the underlying Java code.  In general, where Python
      users type

      .. code-block:: python
      
         sat = gmat.Construct("Spacecraft","Sat")

      MATLAB users enter

      .. code-block:: matlab
      
         sat = gmat.gmat.Construct('Spacecraft','Sat');

   #. Type identification requires a call that sets the object type.  In the 
      Python implementation of the API, changes from the base GmatBase type to
      the derived type is automatic.  In MATLAB/Java, the user needs to perform 
      the cast.  Where Python users type


      .. code-block:: python
      
         sat = gmat.Construct("Spacecraft","Sat")

      to work with a Spacecraft object, MATLAB users that need to interact with 
      the object **as a Spacecraft** need to enter

      .. code-block:: matlab
      
         sat = gmat.gmat.Construct('Spacecraft','Sat');
         sat = gmat.Spacecraft.SetClass(sat);

      A MATLAB class, GMATAPI, is provided in the bin folder which contains
      static functions that automatically handle the change from the base
      GmatBase type to the derived type just like in the Python API. MATLAB
      users can now type

      .. code-block:: matlab
      
         sat = GMATAPI.Construct('Spacecraft','Sat');

      This type setting becomes important when objects are passed to other 
      objects using methods that require specific object type, as is the case
      when setting forces on a dynamics model or spacecraft on a propagation 
      state manager (examples below).


Force Model Setup
-----------------
GMAT hides the complexity of force modeling in the internal ODEModel class, 
which is aliased to the label "ForceModel" in GMAT scripting.  The GMAT 
scripting for the force model used here is

.. code-block:: matlab

   Create ForceModel FM;
   GMAT FM.CentralBody = Earth;
   GMAT FM.PrimaryBodies = {Earth};
   GMAT FM.GravityField.Earth.Degree = 8;
   GMAT FM.GravityField.Earth.Order = 8;
   GMAT FM.GravityField.Earth.PotentialFile = 'JGM3.cof';

Basic Force Model Configuration
+++++++++++++++++++++++++++++++
Using the API is similar for force configuration, but not identical.  The GMAT 
scripting hides the creation of individual forces and their collection into the
ODEModel force container.  Settings on the forces in an ODEModel are made by 
passing those settings from the force container to the corresponding force.  API 
users access the forces directly, setting their parameters and force by force 
and passing the configured forces into the ODEModel container.  The model 
scripted above is configured using the scripting

.. code-block:: python

   # Force model settings
   fm = gmat.Construct("ForceModel", "FM")
   fm.SetField("CentralBody", "Earth")

   # An 8x8 JGM-3 Gravity Model
   earthgrav = gmat.Construct("GravityField")
   earthgrav.SetField("BodyName","Earth")
   earthgrav.SetField("PotentialFile","../data/gravity/earth/JGM3.cof")
   earthgrav.SetField("Degree",8)
   earthgrav.SetField("Order",8)

   # Add force to the dynamics model
   fm.AddForce(earthgrav)

Note the difference in the calls to Construct in this example.  The dynamics 
model is created using the line

.. code-block:: python

   fm = gmat.Construct("ForceModel", "FM")

and the gravity model component, with the code

.. code-block:: python

   earthgrav = gmat.Construct("GravityField")

The dynamics model has a name, "FM."  The gravity field does not have a name.  
Objects constructed with names are managed by the GMAT code running inside of 
the API library.  Objects that do not have names are not managed by the library.
The object ownership for those objects is the responsibility of the code that 
creates the object.  For the dynamics model under construction here, the user 
has responsibility for the gravity field in this call:

.. code-block:: python

   # Unnamed: The user is responsible for this object
   earthgrav = gmat.Construct("GravityField")

and then passes that responsibility to the dynamics model with this call:

.. code-block:: python

   # Add force to the dynamics model, passing ownership to the dynamics
   fm.AddForce(earthgrav)


Connecting the Spacecraft
+++++++++++++++++++++++++
Before the force model can be used, it needs to be connected to the spacecraft 
that provides state data and force model parameters.  GMAT does this using a 
component called a Propagation State Manager (PSM).  The PSM component is not 
exposed to script users.  It is built inside of the scripted Propagator object
that connects together integrators and force models.

Users that want to work directly with a force model can do so by creating a
Propagation State Manager object and working directly with it.  The force model
built above can be tested using this approach:

.. code-block:: python
   
   psm = gmat.PropagationStateManager()
   psm.SetObject(earthorb)
   psm.BuildState()

The last line here, "psm.BuildState()", creates an internal state object that
connects spacecraft properties to a vector of data used by the force model.  The 
propagation state manager is connected to the force model, and its state set as 
the force model's state, using the scripting

.. code-block:: python
   
   fm.SetPropStateManager(psm)
   fm.SetState(psm.GetState())

Testing the Model
+++++++++++++++++
The steps above produce a GMAT force model configuration that can be used from
the user's application framework.  All that remains is initialization of the 
objects, post initialization preparation, and then calls that exercise the 
model.  Initialization connects the force model to GMAT's underlying resources,
including the solar system objects and core elements of the system 
infrastructure:  

.. code-block:: python
   
   # Assemble all of the objects together
   gmat.Initialize()

GMAT's propagation subsystem, which includes the force model components, 
requires two additional steps before it can be used.  First the state vector 
needs to be set up for the force and propagation modeling.  This step determines
and sets the size of the state and derivative vectors, and sets up mappings 
between the spacecraft that are modeled and that state vector.  The second 
step passes the parameters needed for modeling into the force model and 
propagator objects that are used.

For direct access to the force modeling, the user needs to execute these steps 
directly:

.. code-block:: python

   # Finish force model setup:
   ##  Map spacecraft state into the model
   fm.BuildModelFromMap()
   ##  Load physical parameters needed for the forces
   fm.UpdateInitialData()

Users can display the Cartesian form of the state vector used in the modeling by 
accessing the state vector from the spacecraft:

.. code-block:: python

   # Now access state and get derivative data
   pstate = earthorb.GetState().GetState()
   print("State Vector: ", pstate)

Finally, the force model can be exercised either in its raw form as used by the
integrators by calling the GetDerivatives() method:

.. code-block:: python

   fm.GetDerivatives(pstate)
   dv = fm.GetDerivativeArray()
   print("Derivative:   ", dv)

or by calling it for a specific spacecraft object through the 
GetDerivativesForSpacecraft() method:

.. code-block:: python

   vec = fm.GetDerivativesForSpacecraft(earthorb)
   print("SCDerivative: ", vec)

When these pieces are assembled together, a run of the Ex_R2020a_BasicForceModel script 
shows the input state and derivative outputs to the user:

*Note: numbers have been truncated for display purposes*

.. code-block:: matlab

   $ python3 Ex_R2020a_BasicForceModel.py 
   State Vector:  [1018.819261, -1018.819261, -6778.562873, 5.226958, 5.226958, -1.374825e-15]

   Derivative:    [5.226958, 5.226958, -1.374825e-15, -0.00121383, 0.00121392, 0.00809840]

   SCDerivative:  5.226958 5.226958 -1.374825e-15 -0.00121383 0.00121392 0.00809840


Exercises
+++++++++

#. Add point mass forces for the Sun and Moon to the force model.  The GMAT 
   class for point mass forces is named "PointMassForce".
#. Use the propagation state manager to turn on the A-Matrix computation for the
   force model by passing the "AMatrix" setting to the propagation state manager 
   using its SetProperty method.
#. Add a Jacchia-Roberts drag model and a solar radiation pressure model to the 
   force model. 


Propagator Setup
----------------
.. note::

   The Propagator Setup example shown here is located in Ex_R2020a_PropagationStep.m 
   file.  It uses a basic force model by importing from the Ex_R2020a_BasicFM file, a 
   stripped down version of the force model used in the previous section.  An 
   example of the full configuration, showing one solution for the exercises, is 
   in the PropagateLoop file.

   To use one of the propagation files, copy the file you plan to use into 
   GMAT's bin folder.  Also copy the Ex_R2020a_BasicFM file.


In GMAT scripting the lines of script for a propagator,

.. code-block:: matlab

   Create Propagator PDProp
   GMAT PDProp.FM = FM;
   GMAT PDProp.Type = PrinceDormand78;
   GMAT PDProp.InitialStepSize = 60;
   GMAT PDProp.Accuracy = 1.0e-12;
   GMAT PDProp.MinStep = 0.0;

create an object from the GMAT class PropSetup.  This object is a container for
an object that performs propagation either numerically through an Integrator 
object or analytically through an object implementing an analytic algorithm.  
The latter objects are used, in GMAT, for ephemeris propagators.  The former are 
used for Runge-Kutta integrators, predictor-correctors, and other numerical 
integration algorithms that require associated dynamics models.  When the 
propagator requires a dynamics model, that model is also managed by a PropSetup
object.  The key feature to know for propagator configuration in the GMAT API is
that a "Propagator" is actually a PropSetup object that contains the propagation 
component and, for numerical integrators, a dynamics model.

Working interactively, an API user can see this relationship in Python: 

.. code-block:: python

   >>> import gmatpy as gmat
   >>> pdprop = gmat.Construct("Propagator","PDProp")
   >>> pdprop
   <gmatpy.gmat_py.PropSetup; proxy of <Swig Object of type 'PropSetup *' at 0x7f26b76a57e0> >

Propagator Component Setup
++++++++++++++++++++++++++
When a Propagator is scripted, a PropSetup is created that the user then 
configures for use.  Using the provided MATLAB example, the code that loads the 
force model and builds the PropSetup is

.. code-block:: matlab

   % Load GMAT into memory
   [myMod, gmatStartupPath, result] = load_gmat();

   Ex_R2020a_BasicFM;

   % Build the propagation container class 
   pdprop = GMATAPI.Construct("Propagator","PDProp");

The PropSetup constructed here is a container for the objects used in 
propagation.  The next step configuring this container is creation and  
assignment of an integrator, performed using the steps

.. code-block:: matlab

   % Create and assign a numerical integrator for use in the propagation
   gator = GMATAPI.Construct("PrinceDormand78");
   pdprop.SetReference(gator);

The dynamics model also needs to be set on the PropSetup:

.. code-block:: matlab

   % Assign the force model imported from Ex_R2020a_BasicFM
   pdprop.SetReference(fm);

Once the local references are set, the integrator settings can be made similarly 
to the dynamics model setting in the previous section:

.. code-block:: matlab

   % Set some of the fields for the integration
   pdprop.SetField("InitialStepSize", 60.0);
   pdprop.SetField("Accuracy", 1.0e-12);
   pdprop.SetField("MinStep", 0.0);

Spacecraft and Final Initialization
+++++++++++++++++++++++++++++++++++
In the preceding section, the propagation state manager was built as a separate
component and configured to connect the spacecraft to the dynamics model.  When
working with a PropSetup component, the propagation state manager is integrated 
into the component. As an alternative to the manual steps to configure the
propagation state manager, the PropSetup provides a function,
PrepareInternals(), that handles this configuration for each propagated object
added through the AddPropObject() function, and completes the initialization of
the component and its integrator:

.. code-block:: matlab

   % Setup the spacecraft that is propagated
   pdprop.AddPropObject(earthorb);
   pdprop.PrepareInternals();

GMAT's PropSetup component works by creating copies of the propagator and 
dynamics models.  Those copies need to be set for the application environment
so that the user can use them after configuration.  The PropSetup provides a 
simple mechanism for accessing its copies.  The code that refreshes the local
variables for them to be used, is

.. code-block:: matlab

   % Refresh the 'gator reference
   gator = pdprop.GetPropagator();

Running the Propagator
++++++++++++++++++++++
The propagator can now be used.  A 60-second propagation is performed, showing
the state data before and after the step, using the code

.. code-block:: matlab

   % Take a 60 second step, showing the state before and after
   gator.GetState()
   gator.Step(60);
   gator.GetState()

These calls produce this output from Ex_R2020a_PropagationStep.m:

.. code-block:: matlab

   >> Ex_R2020a_PropagationStep                 
   Initialize Moderator Status: 1
   No script provided to load.

   ans =

      1.0e+03 *

      1.018819261603825
     -1.018819261603827
     -6.778562873085272
      0.005226958779502
      0.005226958779502
     -0.000000000000000


   ans =

      1.0e+03 *

      1.330028382856595
     -0.703241487939055
     -6.763990149325915
      0.005142965479731
      0.005288543267909
      0.000485610549370

   >> 


Exercises
+++++++++

#. Modify the Ex_R2020a_PropagationStep example to use a force model that includes the
   point mass Sun and Moon forces and solar radiation pressure.
#. Wrap the propagator in a loop so that propagation extends for a full day,
   displaying the epoch and position at each propagation step.

The GMAT API and Plug-in Modules
--------------------------------
GMAT plug-in modules package new functionality into shared libraries that GMAT
loads when it starts up.  The API's copy of GMAT loads these modules when they 
are identified in the GMAT startup file.  Standard GMAT functions work on 
components from plugins, but the API calls have several restrictions.

Wrapped Plugins
+++++++++++++++
The Station and Estimation plugin libraries in GMAT include SWIG wrapper code 
for the contained classes.  This reduces the restrictions on those components.

As an example of the restrictions on wrapped plugin code, consider the Station
plugin, which implements GMAT's GroundStation class.  Users of GMAT's GroundStation 
class can access the full feature set for the class.  The user is 
required in Python to cast constructed components to
the derived class type by hand.  The Python auto-cast feature in the GMAT core 
code is not accessible from the plugin component as seen below:

.. code-block:: python

   >>> import gmatpy as gmat
   >>> station = gmat.Construct("GroundStation","Station")
   >>> station
   <gmatpy.gmat_py.GmatBase; proxy of <Swig Object of type 'GmatBase *' at 0x7fccdc983f60> >
   >>> station=gmat.GroundStation.SetClass(station)
   >>> station
   <gmatpy.station_py.GroundStation; proxy of <Swig Object of type 'GroundStation *' at 0x7fccdc983fc0> >
   >>> 

Note that in this code, the station object returned from the call to the 
Construct() function is set as a GmatBase object.  In order to treat it as a 
GroundStation object, the user needed to call the GroundStation.SetClass() 
method on the object in order for Python to identify the object's subclass 
correctly.

MATLAB API users are not required to explicity cast the class, provided they
use the GMATAPI MATLAB class, as shown below:

.. code-block:: matlab

    >> load_gmat();
    No script provided to load.
    >> station = GMATAPI.Construct("GroundStation","Station")

    station =

    Object of type GroundStation named Station

    >> station.getClass()

    ans =

    class gmat.GroundStation

    >> 

Unwrapped Plugins
+++++++++++++++++
Plugin code that is not wrapped in SWIG can be accessed using the API, but only
in a more restricted manner.  As an example, at this writing the VF13ad 
optimizer is available as a GMAT component for users inside of Goddard Space
Flight Center.  The associated plugin builds a component with class name "VF13ad"
that provides the optimization functionality.  The VF13ad optimizer is derived 
from an Optimizer base class in the GMAT core code.  API users can access that 
component as a GmatBase object, or as an Optimizer object, but not as a VF13ad
object, as can be seen here:

.. code-block:: python

   >>> import gmatpy as gmat
   >>> vf13 = gmat.Construct("VF13ad","VF13")
   >>> vf13
   <gmatpy.gmat_py.GmatBase; proxy of <Swig Object of type 'GmatBase *' at 0x7f615f50c2a0> >
   >>> vf13 = gmat.VF13ad.SetClass(vf13)
   Traceback (most recent call last):
     File "<stdin>", line 1, in <module>
   AttributeError: module 'gmatpy' has no attribute 'VF13ad'
   >>> vf13 = gmat.Optimizer.SetClass(vf13)
   >>> vf13
   <gmatpy.gmat_py.Optimizer; proxy of <Swig Object of type 'Optimizer *' at 0x7f614d9cff90> >
   >>> exit()

The underlying object remains a VF13ad component:

.. code-block:: python

   >>> vf13.Help()

   VF13ad  VF13

      Field                                   Type   Value
      --------------------------------------------------------

      ShowProgress                         Boolean   true
      ReportStyle                             List   Normal
      ReportFile                          Filename   <not set>
      MaximumIterations                    Integer   200
      Tolerance                               Real   1e-05
      UseCentralDifferences                Boolean   false
      FeasibilityTolerance                    Real   0.001

but, from the perspective of an API user, is manipulated as an Optimizer or 
GmatBase object.

Measurement Modeling
--------------------
.. note::

   The Measurement Modeling example shown here is located in Ex_R2020a_RangeMeasurement.m 
   file (or the Ex_R2020a_RangeMeasurement.py file for Python users).  

GMAT's Measurement Models are driven through the TrackingFileSet class.  A 
TrackingFileSet defines a measurement as a tracking configuration consisting of 
a signal path and measurement type.  The signal path is defined by the nodes 
that a measurement signal traverses to create the measurement. For example,
the path may be ground station -> spacecraft -> ground station for a range 
measurement.  

The user configures the hardware for each node, assigning antennae, transmitters,
receivers, and transponders as needed to the stations and spacecraft used in the 
measurement.  Error models are configured and assigned to each measurement, media
corrections are toggled, and ancillary components configured - like propagators
when light time correction is applied - to complete the configuration the user 
needs.  The system is complex because the processes involved have many options. 
This guide, and the sample script, step through the process element by element
to build the model.

Spacecraft, Ground Stations, and Propagators
++++++++++++++++++++++++++++++++++++++++++++

.. code-block:: matlab

   % Construct the PropSetup to control propagation (for light time)
   prop = GMATAPI.Construct("PropSetup", "prop");
   prop.GetODEModel().SetField("ErrorControl", "None");
   prop.GetPropagator().SetNumber("MinStep", 0);

   % Create objects for generating measurement
   simsat = GMATAPI.Construct("Spacecraft", "SimSat");
   gds = GMATAPI.Construct("GroundStation", "GDS");

   % Configure Spacecraft initial conditions
   simsat.SetField("DateFormat", "A1ModJulian");
   simsat.SetField("Epoch", "21550");

   % Configure GroundStation
   gds.SetField("StateType", "Spherical");
   gds.SetField("HorizonReference", "Ellipsoid");
   gds.SetField("Location1", 0);
   gds.SetField("Location2", 90.0);
   gds.SetField("Location3", 0);


Hardware Components
+++++++++++++++++++

.. code-block:: matlab

   % Create communication hardware
   % Hardware for ground station
   ant1 = GMATAPI.Construct("Antenna", "Antenna1");
   tmit = GMATAPI.Construct("Transmitter", "Transmitter1");
   tmit.SetField("Frequency", 2067.5);
   rec = GMATAPI.Construct("Receiver", "Receiver1");

   % Hardware for spacecraft
   ant2 = GMATAPI.Construct("Antenna", "Antenna2");
   tpond = GMATAPI.Construct("Transponder", "Transponder1");
   tpond.SetField("TurnAroundRatio","240/221");

   % Set fields
   % Use Antenna1 for Transmitter1 and Receiver1
   tmit.SetField("PrimaryAntenna","Antenna1");
   rec.SetField("PrimaryAntenna","Antenna1");

   % Use Antenna2 for Transponder1
   tpond.SetField("PrimaryAntenna","Antenna2");

   % Add Antenna2 and Transponder1 to spacecraft
   simsat.SetField("AddHardware","{Antenna2, Transponder1}");

   % Add Antenna1, Transmitter1, and Receiver1 to station
   gds.SetField("AddHardware","{Antenna1, Transmitter1, Receiver1}");


Error Models
++++++++++++

.. code-block:: matlab

   % Define range measurements and error model
   tem = GMATAPI.Construct("ErrorModel", "TheErrorModel");
   % Specify these measurements are range measurements in km
   tem.SetField("Type","Range");
   tem.SetField("NoiseSigma", 0.050); % Standard deviation of noise
   tem.SetField("Bias",0); % Bias in measurement

   % Define doppler range rate measurements and error model
   tem2 = GMATAPI.Construct("ErrorModel", "TheErrorModel2");
   % Specify these measurements are doppler range rate measurements
   tem2.SetField("Type","RangeRate");
   tem2.SetField("NoiseSigma", 5e-5); % Standard deviation of noise
   tem2.SetField("Bias",0); % Bias in measurement

   % Add ErrorModels to the ground station
   gds.SetField("ErrorModels","{TheErrorModel, TheErrorModel2}");


The Measurement
+++++++++++++++

.. code-block:: matlab

   % Create a TrackingFileSet to manage the observations
   tfs = GMATAPI.Construct("TrackingFileSet", "SimData");
   tfs.SetField("FileName","TrkFile_API_GN.gmd"); % Still needed even though it's' not written to
   tfs.SetField("UseLightTime", false);
   tfs.SetField("UseRelativityCorrection", false);
   tfs.SetField("UseETminusTAI", false);

   % Define signal paths and measurement type(s)
   % 2-way measurements are used here along the path GDS -> SimSat -> GDS
   % Add range measurements to TrackingFileSet
   tfs.SetField("AddTrackingConfig", "{{GDS,SimSat,GDS}, Range}");
   % Add doppler range rate measurements to TrackingFileSet
   tfs.SetField("AddTrackingConfig", "{{GDS,SimSat,GDS}, RangeRate}");
   tfs.SetPropagator(prop); % Tell TrackingFileSet the propagator to use


Exercising the Model
++++++++++++++++++++

.. code-block:: matlab

   % Initialize the GMAT objects
   gmat.gmat.Initialize()

   % Calculate the measurement
   tdas = tfs.GetAdapters();
   numMeas = tdas.size();

   tda = tdas.get(0);
   md0 = tda.CalculateMeasurement();
   disp("GMAT Range Measurement Value:")
   disp(md0.getValue().get(0))


   % Make sure this is correct
   satState = simsat.GetState();
   gsPos = gds.GetMJ2000Position(satState.GetEpochGT()).GetDataVector();
   satPos = satState.GetState();
   r = gsPos - satPos(1:3);
   rNorm = norm(r);
   disp("Numerical Range Measurement Value (no lighttime):")
   disp(2*rNorm)

   disp("")

   xid = simsat.GetParameterID("CartesianX");
   tda.CalculateMeasurementDerivatives(simsat,xid);
   for ii = 0:5
       deriv(ii+1) = tda.ApiGetDerivativeValue(0,ii);
   end
   disp("GMAT Range Measurement Derivatives:")
   disp(deriv)

   disp("")

   tda = tdas.get(1);
   md1 = tda.CalculateMeasurement();
   disp("GMAT RangeRate Measurement Value:")
   disp(md1.getValue().get(0))


Exercises
+++++++++

#. Modify the one day propagation script to report the range measurement at each
   step where a valid measurement can be computed.
#. Add a second ground station the Ex_R2020a_RangeMeasurement example and report its 
   measurement data.
