.. _Examples:


Propagation Command
====================

.. _PropagateToPerigee:
.. code-block:: python
	:caption: Setting up and running a Propagate command
	:linenos:

	from load_gmat import gmat

	# Core resources used
	sat  = gmat.Construct("Spacecraft", "Sat")
	sat.SetField("DateFormat", "UTCGregorian")
	sat.SetField("Epoch", "02 Jan 2023 12:00:00.000")
	sat.SetField("X", 0)
	sat.SetField("Y", -3100)
	sat.SetField("Z", -11591)
	sat.SetField("VX", 5.155)
	sat.SetField("VY", 0.0)
	sat.SetField("VZ", 0.0)

	fm   = gmat.Construct("ForceModel", "Dynamics")
	epm  = gmat.Construct("PointMassForce", "EPM")
	fm.AddForce(epm)

	prop = gmat.Construct("Propagator", "Prop")
	prop.SetField("FM", "Dynamics")

	propagate = gmat.Command("Propagate", "Prop(Sat) {Sat.Periapsis}")

	gmat.Initialize()

	gmat.Execute()


Targeting Command
==================

.. _BurnTargeting:
.. code-block:: python
	:caption: Setting up and running a Propagate command
	:linenos:

	from load_gmat import gmat

	# Core resources used
	sat  = gmat.Construct("Spacecraft", "Sat")
	sat.SetField("DateFormat", "UTCGregorian")
	sat.SetField("Epoch", "10 Feb 2023 12:00:00.000")
	sat.SetField("DispayStateType", "Keplerian")
	sat.SetField("SMA", 6900)
	sat.SetField("ECC", 0.005)
	sat.SetField("INC", 28.5)
	sat.SetField("RAAN", 45)
	sat.SetField("AOP", 90.0)
	sat.SetField("TA", 180.0)

	fm   = gmat.Construct("ForceModel", "Dynamics")
	epm  = gmat.Construct("PointMassForce", "EPM")
	fm.AddForce(epm)

	prop = gmat.Construct("Propagator", "Prop")
	prop.SetField("FM", "Dynamics")

	burn = gmat.Construct("ImpulsiveBurn", "Burn")
	dc = gmat.Construct("DifferentialCorrector", "DC")

	# First propagate so we start at perigee 
	gmat.Command("Propagate", "Prop(Sat) {Sat.Periapsis}")	

	# Target commands
	gmat.Command("Target", "DC")
	# Note that commands are added to the branch in the Target command
	# until the EndTarget command is added	
	gmat.Command("Vary", "DC(Burn.Element1 = 0.5, {Perturbation = 0.0001})")
	gmat.Command("Maneuver", "Burn(Sat)")
	gmat.Command("Propagate", "Prop(Sat) {Sat.Apoapsis}")
	gmat.Command("Achieve", "DC(Sat.Earth.RMAG = 42165.0, {Tolerance = 0.1})")
	gmat.Command("EndTarget")

	gmat.Initialize()

	# Execute() runs the entire sequence to completion
	gmat.Execute()
