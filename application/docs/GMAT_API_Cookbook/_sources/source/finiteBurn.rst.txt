**Executing Finite Burns**
==========================

**Problem**
-----------
GMAT's thruster resources allow users to perform finite burns with control over their burn duration, burn direction, etc. in order to propagate maneuvers. A simple GMAT script example of the activation and deactivation of a finite burn can be found below:

.. code-block:: matlab

	BeginMissionSequence;
	%  Propagate for 1/10 of a day, without thrusters on.
	Propagate 'Prop 0.1 Days' prop(Sc) {Sc.ElapsedSecs = 8640};

	%  Turn on thrusters....they will remain on through all events until the
	%  "EndFiniteBurn fb(Sc)" command is executed.
	BeginFiniteBurn 'Turn on Thruster' fb(Sc);

	%  Propagate for 1 day, while thrusters are turned on.
	Propagate 'Prop 1 day' prop(Sc) {Sc.ElapsedDays = 1};

	%  Turn off thrusters   
	EndFiniteBurn 'Turn off Thruster' fb(Sc);

	%  Propagate for 1 day
	Propagate 'Prop 1 Day' prop(Sc) {Sc.ElapsedDays = 1};

API users need to be able to perform finite burn maneuvers while easily being able to modify propagation and burn sequences in order to make corrections to achieve desired maneuver goals.

**Solution**
------------
To utilize GMAT's finite burn capabilities through the API, users will need tank, thruster, FiniteBurn, and FiniteThrust objects. In this guide, we will focus on using a ChemicalTank and ChemicalThruster, but electric tanks and thrusters are also available options. The hardware will be attached to the desired Spacecraft, and the thrust force will be added to the ForceModel in use to propagate the Spacecraft with burns active. With everything set up, API users can turn thrusters on and off through the "IsFiring" boolean on a thruster and setting a Spacecraft's and corresponding FiniteBurn's maneuvering flags. An example covering these steps is shown below.

Example API Finite Burn
***********************

**Step 1**: Instantiate Spacecraft

We need to create a spacecraft that will perform the maneuver. Most of the default Spacecraft settings will be used. The dry mass is reduced to allow more acceleration on the spacecraft during the maneuver. We will also set up a simple default Earth point mass ForceModel and a propagator.

.. code-block:: python

    # Setup a spacecraft and basic propagation system
    sat = gmat.Construct("Spacecraft", "Sat")
    sat.SetField("DryMass", 80.0)

    fm = gmat.Construct("ForceModel", "FM")
    epm = gmat.Construct("PointMassForce", "EPM")
    fm.AddForce(epm)

    prop = gmat.Construct("Propagator", "Prop")
    gator = gmat.Construct("PrinceDormand78", "Gator")
    prop.SetReference(gator)
    prop.SetReference(fm)
	    
**Step 2**: Set Finite Burn specific settings

To model fuel use and thrusting on a Spacecraft, we will attach a ChemicalTank and ChemicalThruster to the Spacecraft. Meanwhile for the burn sequence itself, we create a FiniteBurn object and connect it to the ChemicalThruster. Finally, a burn force is created as a FiniteThrust object and connected to both the FiniteBurn and the Spacecraft that is maneuvering. This force will later be added to the ForceModel so that the force can be applied.

Note that the ChemicalThruster is where we set the thrust direction for the burn. For this case, we will simply use the default settings, applying thrust in the positive velocity direction of the Spacecraft with respect to the Earth.

.. code-block:: python

	# Setup the spacecraft hardware
	tank = gmat.Construct("ChemicalTank", "Fuel")
	tank.SetField("FuelMass", 20.0)
	thruster = gmat.Construct("ChemicalThruster", "Thruster")
	thruster.SetField("Tank", "Fuel")
	thruster.SetField("DecrementMass", True)
	
	sat.SetField("Tanks", "Fuel")
	sat.SetField("Thrusters", "Thruster")
	
	# Ensure the system is up to date
	gmat.Initialize()
	
	# Build the burn model used for the burn
	burn = gmat.Construct("FiniteBurn", "TheBurn")
	burn.SetField("Thrusters", "Thruster")
	burn.SetSolarSystem(gmat.GetSolarSystem())
	burn.SetSpacecraftToManeuver(sat)

	# Build the force that applies the burn
	def setThrust(s, b):
	    bf = gmat.FiniteThrust("Thrust")
	    bf.SetRefObjectName(gmat.SPACECRAFT, s.GetName())
	    bf.SetReference(b)
	    gmat.ConfigManager.Instance().AddPhysicalModel(bf);
	    return bf
	
	burnForce = setThrust(sat, burn)

**Step 3**: Wire the objects together

Here we initialize our setup and grab the ChemicalThruster object specifically owned by our Spacecraft. This ensures we are working with the thruster acting on and decreasing the fuel of our Spacecraft.

.. code-block:: python

	gmat.Initialize()
	prop.AddPropObject(sat)
	prop.PrepareInternals()
	
	# Access the thruster cloned onto the spacecraft
	theThruster = sat.GetRefObject(gmat.THRUSTER, "Thruster")
	
	# Check the spacecraft mass
	print(sat.GetName(), " current mass:  ", sat.GetField("TotalMass"), " kg\n")

**Step 4**: Propagate

In order to switch between a non-firing and firing state, we will need to set whether our ChemicalThruster is firing, our Spacecraft is maneuvering, and what Spacecraft our FiniteBurn is affecting. With everything turned on, we then add our FiniteThrust object from Step 2 to the ForceModel. In this example, our burn will be used to raise the SMA.

.. code-block:: python

	# Take a few steps
	elapsed = 0.0
	dt = 60.0
	gator = prop.GetPropagator()
	
	# Step for 10 minutes
	print("\n Propagate 10 minutes without a 	burn\n-------------------------------------------------\n")
	
	print("Firing: ", theThruster.GetField("IsFiring"), "\n")
	state = gator.GetState()
	r = math.sqrt(state[0]**2 + state[1]**2 + state[2]**2)
	vsq = state[3]**2 + state[4]**2 + state[5]**2
	sma = r * mu / (2.0 * mu - vsq * r)
	print(elapsed, sma, state, sat.GetField("TotalMass"))
	for i in range(10):
		gator.Step(dt)
		elapsed = elapsed + dt
		state = gator.GetState()
		r = math.sqrt(state[0]**2 + state[1]**2 + state[2]**2)
		vsq = state[3]**2 + state[4]**2 + state[5]**2
		sma = r * mu / (2.0 * mu - vsq * r)
		print(elapsed, sma, state, sat.GetField("TotalMass"))
		gator.UpdateSpaceObject()
	
	# Step for 30 minutes with a burn
	print("\n Propagate 30 minutes with a burn\n-------------------------------------------------\n")
	
	# -----------------------------
	# Finite Burn Specific Settings
	# -----------------------------
	# Turn on the thruster
	theThruster.SetField("IsFiring", True)
	sat.IsManeuvering(True)
	#sat.SetPropItem("MassFlow")
	burn.SetSpacecraftToManeuver(sat)
	#burn.TakeAction("SetData")
	
	# Add the thrust to the force model
	prop.AddForce(burnForce)
	psm = prop.GetPropStateManager()
	psm.SetProperty("MassFlow")
	# -----------------------------
	
	prop.PrepareInternals()
	gator = prop.GetPropagator()
	
	print("Firing: ", theThruster.GetField("IsFiring"), "\n")
	# Now propagate thruogh the burn
	for i in range(30):
		gator.Step(dt)
		elapsed = elapsed + dt
		state = gator.GetState()
		r = math.sqrt(state[0]**2 + state[1]**2 + state[2]**2)
		vsq = state[3]**2 + state[4]**2 + state[5]**2
		sma = r * mu / (2.0 * mu - vsq * r)
		print(elapsed, sma, state, sat.GetField("TotalMass"))
		gator.UpdateSpaceObject()

**Step 5**: Turn off the Thruster

We now simply undo our work from Step 4, turning off the burn.

.. code-block:: python

	fm = prop.GetODEModel()
	fm.DeleteForce(burnForce)
	theThruster.SetField("IsFiring", False)
	sat.IsManeuvering(False)
	
	prop.PrepareInternals()
	gator = prop.GetPropagator()

**Discussion**
--------------
This example provides all the core elements required to develop more complex maneuvers through finite burns. By using the IsFiring field on thrusters and managing ForceModel forces, finite burns can be controlled through propagation to apply staggered burn sequences. Here a simple loop stepping through 30 time steps was used for the burn, but a user could also set up different goals through Python (for example, having the burn run until a desired SMA was achieved).