.. _SwigOldAndNew:

Comparison of the SWIG Prototype with the Production API
========================================================

The prototype SWIG generated API developed in 2016 can be used with GMAT R2018.
This appendix shows the differences between that version of the SWIG generated 
API and the production API presented in this document for the three examples 
described in :ref:`DesignExamples`.

Time System Conversion
----------------------


.. code-block:: python
   :caption: Time System Conversion using GMAT R2018a SWIG Configuration
   :linenos:

   import gmat_py as gmat

   # Initializing the Moderator configures the converter
   gmat.Moderator.Instance().Initialize('gmat_startup_file.txt')

   # Get the converter
   timeConverter = gmat.TimeSystemConverter.Instance()

   # Convert an epoch
   UTCEpoch = 21738.22145
   TAIepoch = timeConverter.Convert(UTCepoch, 2, 1)



.. code-block:: python
   :caption: Time System Conversion using the Production API
   :linenos:

   import gmat_py as gmat

   # Get the converter
   timeConverter = gmat.theTimeSystemConverter

   # Convert an epoch
   UTCEpoch = 21738.22145
   TAIepoch = timeConverter.Convert(UTCepoch, UTC, TAI)

Coordinate System Conversion
----------------------------

.. code-block:: python
   :caption: Coordinate System Conversion using GMAT R2018a SWIG Configuration
   :linenos:

   import gmat_py as gmat

   # Initialize to set default objects needed to configure the converter
   mod = gmat.Moderator.Instance()
   mod.Initialize('gmat_startup_file.txt')

   # Setup the GMAT data structures for the conversion
   mjd   = gmat.A1Mjd(22326.977184)
   rvIn  = gmat.Rvector6(6988.426918, 1073.884261, 2247.332981, 0.019982, 7.226988, -1.554962)
   rvOut = gmat.Rvector6(0.0, 0.0, 0.0, 0.0, 0.0, 0.0)

   # Create the converter
   csConverter = gmat.CoordinateConverter()

   # Get the solar system and central body
   ss = mod.GetSolarSystemInUse()
   earth = ss.GetBody("Earth")

   # Create the input and output coordinate systems
   eci  = gmat.CoordinateSystem.CreateLocalCoordinateSystem(
       "ECI", "MJ2000Eq", earth, None, None, earth, ss)
   ecef = gmat.CoordinateSystem.CreateLocalCoordinateSystem(
       "ECEF", "BodyFixed", earth, None, None, earth, ss)

   csConverter.Convert(UTCepoch, rvIn, eci, rvOut, ecef)


.. code-block:: python
   :caption: Coordinate System Conversion using the Production API
   :linenos:

   import gmat_py as gmat

   # Setup the GMAT data structures for the conversion
   mjd   = gmat.A1Mjd(22326.977184)
   rvIn  = gmat.Rvector6(6988.426918, 1073.884261, 2247.332981, 0.019982, 7.226988, -1.554962)
   rvOut = gmat.Rvector6(0.0, 0.0, 0.0, 0.0, 0.0, 0.0)

   # Create the converter
   csConverter = gmat.CoordinateConverter()

   # Create the input and output coordinate systems
   eci  = gmat.Construct("CoordinateSystem", 
       "ECI", "Earth", "MJ2000Eq")
   ecef = gmat.Construct("CoordinateSystem", 
       "ECEF", "Earth", "BodyFixed")

   csConverter.Convert(UTCepoch, rvIn, eci, rvOut, ecef)


Force Modeling
--------------

.. code-block:: python
   :caption: Force modeling using GMAT R2018a SWIG Configuration
   :linenos:

   import gmat_py as gmat
   mod = gmat.Moderator.Instance()
   mod.Initialize('gmat_startup_file.txt')

   # Spacecraft setup
   sc = gmat.Spacecraft("sc")

   # Evaluating only the 6 element Cartesian state
   state = [7000.0, 0.0, 1000.0, 0.0, 8.0, -0.25]
   pstate = gmat.new_doubleArray(6)
   for i in range(len(state)):
       gmat.doubleArray_setitem(pstate, i, state[i])

   # Internal required objects
   ss = mod.GetDefaultSolarSystem()
   earth = ss.GetBody("Earth")
   eci  = gmat.CoordinateSystem.CreateLocalCoordinateSystem(
           "ECI", "MJ2000Eq", earth, None, None, earth, ss)

   # The state manager
   psm = gmat.PropagationStateManager()
   psm.SetObject(sc)
   psm.BuildState()
   psm.MapObjectsToVector()

   # The force model
   dynamics = gmat.ODEModel("EPointMassDynamics")
   dynamics.SetForceOrigin(earth)

   epm = gmat.PointMassForce("EarthPointMass")
   dynamics.AddForce(epm)

   # Manage memory - dynamics now owns the epm, so Python should not delete it
   epm.thisown = 0

   dynamics.SetPropStateManager(psm)
   dynamics.SetSolarSystem(ss)
   dynamics.SetInternalCoordSystem(eci)
   dynamics.BufferState()
   dynamics.BuildModelFromMap()

   dynamics.UpdateInitialData()
   dynamics.Initialize()

   pderiv = dynamics.GetDerivativeArray()
   dynamics.GetDerivatives(pstate, 0.0)


.. code-block:: python
   :caption: Force Modeling using the Production API
   :linenos:

   import gmat_py as gmat

   dynamics = gmat.Construct("ODEModel", "EPointMassDynamics")
   epm = gmat.PointMassForce("EarthPointMass")
   dynamics.AddForce(epm)

   # Evaluating only the 6 element Cartesian state
   pstate = [7000.0, 0.0, 1000.0, 0.0, 8.0, -0.25]

   gmat.Initialize()

   pderiv = dynamics.GetDerivativeArray()
   dynamics.GetDerivatives(pstate, 0.0)


Force Modeling and Propagation
------------------------------


.. code-block:: python
   :caption: Propagation using GMAT R2018a SWIG Configuration
   :linenos:

   import gmat_py as gmat
   mod = gmat.Moderator.Instance()
   mod.Initialize('gmat_startup_file.txt')

   # Setup the state for propagation
   state = [7000.0, 0.0, 1000.0, 0.0, 8.0, -0.25]

   # Load some pieces we need to configure the system
   ss = mod.GetDefaultSolarSystem()
   sc = gmat.Spacecraft("sc")

   psm = gmat.PropagationStateManager()
   psm.SetObject(sc)
   psm.BuildState()
   psm.MapObjectsToVector()

   # Setup a Earth/Sun/Moon force model
   # note: Use Moderator for the forces and Python memory management won't segfault 
   dynamics = gmat.ODEModel("Forces")

   epm = mod.CreatePhysicalModel("PointMassForce", "EarthPointMass")
   spm = mod.CreatePhysicalModel("PointMassForce", "SunPointMass")
   mpm = mod.CreatePhysicalModel("PointMassForce", "MoonPointMass")
   spm.SetStringParameter("BodyName", "Sun")
   mpm.SetStringParameter("BodyName", "Luna")

   dynamics.AddForce(epm)
   dynamics.AddForce(spm)
   dynamics.AddForce(mpm)

   # Manage memory
   epm.thisown = 0
   spm.thisown = 0
   mpm.thisown = 0

   # Reference object setup
   dynamics.SetPropStateManager(psm)
   dynamics.SetSolarSystem(ss)

   # ODE model initialization
   dynamics.BufferState()
   dynamics.BuildModelFromMap()
   dynamics.UpdateInitialData()
   rv = dynamics.Initialize()

   # Propagator configuration
   prop = gmat.PrinceDormand78("Propagator")
   prop.SetSolarSystem(ss)
   prop.SetPropStateManager(psm)
   prop.SetPhysicalModel(dynamics)
   prop.Initialize()

   # Set the propagation state
   pstate = dynamics.GetState()
   for i in range(len(state)):
       gmat.doubleArray_setitem(pstate, i, state[i])

   for i in range(count):
       prop.Step(60.0)



.. code-block:: python
   :caption: Propagation using the Production API
   :linenos:

   import gmat_py as gmat

   # Setup the state for propagation
   state = [7000.0, 0.0, 1000.0, 0.0, 8.0, -0.25]

   # Setup a Earth/Sun/Moon force model
   # note: Use Moderator for the forces and Python memory management won't segfault 
   dynamics = gmat.Construct("ODEModel", "Forces")

   epm = mod.Create("PointMassForce", "EarthPointMass")
   spm = mod.Create("PointMassForce", "SunPointMass")
   mpm = mod.Create("PointMassForce", "MoonPointMass")
   spm.SetField("BodyName", "Sun")
   mpm.SetField("BodyName", "Luna")

   dynamics.AddForce(epm)
   dynamics.AddForce(spm)
   dynamics.AddForce(mpm)

   # Propagator configuration
   prop = gmat.PrinceDormand78("Propagator")
   prop.SetPhysicalModel(dynamics)

   gmat.Initialize()

   # Set the propagation state
   pstate = dynamics.GetState()
   for i in range(len(state)):
       gmat.doubleArray_setitem(pstate, i, state[i])

   for i in range(count):
       prop.Step(60.0)
