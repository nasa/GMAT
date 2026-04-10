.. _SpacecraftPropagation:

**Propagation with the GMAT API**
=================================

**Problem**
-----------
GMAT's propagator is used to model spacecraft motion within a specified force model. The propagation can be done through either numerical integration or by an ephemeris. Duration of propagation can either be set directly or through a final goal reached by the spacecraft (i.e. propagating for 3 days, propagating to apoapsis, etc.). API users need to be able to perform a propagation through the API and be able to modify propagator settings.

**Solution**
------------
For this example, we will focus on using a numerical integrator. Key objects in performing propagation are the Spacecraft, ForceModel, Integrator and Propagator. The Propagator is created with desired step settings and then a desired Integrator is attached to it. The types of integrators and analytic propagators supported by GMAT can be found in GMAT's User Guide under the "Propagator" resource reference. The integrator is then used to perform propagation using its Step method. An example of propagator use through the API is shown below.

Example API Propagation
***********************

**Step 1**: Configure the Spacecraft

We'll need a spacecraft to propagate.  The following lines provide a basic low-Earth orbiting spacecraft configuration:

.. _SpacecraftConfig:
.. code-block:: python
    :caption: Spacecraft setup used in some examples

    sat = gmat.Construct("Spacecraft", "LeoSat")
    sat.SetField("DateFormat", "UTCGregorian")
    sat.SetField("Epoch", "12 Mar 2020 15:00:00.000")
    sat.SetField("CoordinateSystem", "EarthMJ2000Eq")
    sat.SetField("DisplayStateType", "Keplerian")
    sat.SetField("SMA", 7005)
    sat.SetField("ECC", 0.008)
    sat.SetField("INC", 28.5)
    sat.SetField("RAAN", 75)
    sat.SetField("AOP", 90)
    sat.SetField("TA", 45)
    sat.SetField("DryMass", 50)
    sat.SetField("Cd", 2.2)
    sat.SetField("Cr", 1.8)
    sat.SetField("DragArea", 1.5)
    sat.SetField("SRPArea", 1.2)

**Step 2**: Configure the Force Model

Next we'll set up a force model.  For this example, we'll use an Earth 8x8 potential model, with Sun and Moon point masses and Jacchia-Roberts drag.  In GMAT, forces are collected in the ODEModel class.  That class is scripted as a "ForceModel" in the script language.  The API accepts either class name.  The force model is built and its (empty) contents displayed using the following API commands:

.. code-block:: python

    # Create the ODEModel container
    fm = gmat.Construct("ForceModel", "TheForces")
    fm.Help()

In this example, the spacecraft is in Earth orbit.  The largest force for the model is the Earth gravity field.  We'll set it to an 8x8 field and add it to the force model using the code

.. code-block:: python

    # An 8x8 JGM-3 Gravity Model
    earthgrav = gmat.Construct("GravityField")
    earthgrav.SetField("BodyName","Earth")
    earthgrav.SetField("Degree",8)
    earthgrav.SetField("Order",8)
    earthgrav.SetField("PotentialFile","JGM2.cof")

    # Add the force into the ODEModel container
    fm.AddForce(earthgrav)

Next we'll build and add the Sun, Moon, and Drag forces, and then show the completed force model.

.. code-block:: python

    # The Point Masses
    moongrav = gmat.Construct("PointMassForce")
    moongrav.SetField("BodyName","Luna")
    sungrav = gmat.Construct("PointMassForce")
    sungrav.SetField("BodyName","Sun")
    
    # Drag using Jacchia-Roberts
    jrdrag = gmat.Construct("DragForce")
    jrdrag.SetField("AtmosphereModel","JacchiaRoberts")
    
    # Build and set the atmosphere for the model
    atmos = gmat.Construct("JacchiaRoberts")
    jrdrag.SetReference(atmos)
    
    # Add all of the forces into the ODEModel container
    fm.AddForce(moongrav)
    fm.AddForce(sungrav)
    fm.AddForce(jrdrag)
    
    # Show the settings on the force model
    fm.Help()

In GMAT, the force model scripting shows the settings for each force.  In the API, you can examine the settings for the individual forces using the Help method:

.. code-block:: python

    earthgrav.Help()

or, with a little work, you can see the GMAT scripting for the complete force model:

.. code-block:: python

    print(fm.GetGeneratingString(0))

**Step 3**: Configure the Integrator

Finally, in order to propagate, we need an integrator.  For this example, we'll use a Prince-Dormand 7(8) Runge-Kutta integrator.  The propagator is set using the code

.. code-block:: python

    # Build the propagation container that connect the integrator, force model, and spacecraft together
    pdprop = gmat.Construct("Propagator","PDProp")
    
    # Create and assign a numerical integrator for use in the propagation
    gator = gmat.Construct("PrinceDormand78", "Gator")
    pdprop.SetReference(gator)
    
    # Set some of the fields for the integration
    pdprop.SetField("InitialStepSize", 60.0)
    pdprop.SetField("Accuracy", 1.0e-12)
    pdprop.SetField("MinStep", 0.0)

**Step 4**: Connect the Objects Together

Next the propagator needs its assigned force.  This assignment is made by passing the force model to the propagator:

.. code-block:: python

    # Assign the force model to the propagator
    pdprop.SetReference(fm)

The propagator also needs to know the object that is propagated.  For this example, that is the spacecraft constructed above:

.. code-block:: python

    pdprop.AddPropObject(sat)

**Step 5**: Initialize the System and Propagate a Step

Finally, the system can be initialized and fired to see a single propagation step.  The top level system initialization is performed using the API's Initialize function.  The propagator uses a propagator specific method, PrepareInternals, to setup the propagation specific interconnections and data structures:

.. code-block:: python

    # Perform top level initialization
    gmat.Initialize()
    # Perform the integration subsystem initialization
    pdprop.PrepareInternals()
    
    # Refresh the integrator reference
    gator = pdprop.GetPropagator()

We can then propagate, and start accumulating the data for display later:

.. code-block:: python

    # Take a 60 second step, showing the state before and after, and start buffering
    # Buffers for the data
    time = []
    pos = []
    vel = []
    
    gatorstate = gator.GetState()
    t = 0.0
    r = []
    v = []
    for j in range(3):
        r.append(gatorstate[j])
        v.append(gatorstate[j+3])
    time.append(t)
    pos.append(r)
    vel.append(v)
    
    print("Starting state:\n", t, r, v)
    
    # Take a step and buffer it
    gator.Step(60.0)
    gatorstate = gator.GetState()
    t = t + 60.0
    r = []
    v = []
    for j in range(3):
        r.append(gatorstate[j])
        v.append(gatorstate[j+3])
    time.append(t)
    pos.append(r)
    vel.append(v)
    
    print("Propped state:\n", t, r, v)

Finally, we can run for a few orbits and show the results:

.. code-block:: python

    for i in range(360):
        # Take a step and buffer it
        gator.Step(60.0)
        gatorstate = gator.GetState()
        t = t + 60.0
        r = []
        v = []
        for j in range(3):
            r.append(gatorstate[j])
            v.append(gatorstate[j+3])
        time.append(t)
        pos.append(r)
        vel.append(v)
    
    import matplotlib.pyplot as plt
    plt.rcParams['figure.figsize'] = (15, 5)
    positions = plt.plot(time, pos)

The velocities can also be plotted:

.. code-block:: python

    velocities = plt.plot(time, vel)

**Discussion**
--------------
This example has showcased one way to generate the position and velocity of a spacecraft over a specified time span. This script is now easily modifiable should a user need to adjust step sizes, adjust the spacecraft initial state, provide a more extensive force model, etc. to quickly rerun GMAT through the API and collect new sets of data.
