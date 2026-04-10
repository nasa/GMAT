.. _CommandSequences:

************************
Using a Command Sequence (Work still in progress)
************************
The GMAT Mission control sequence is a doubly linked list of GMAT command objects.  The API provides capabilities to implement the same capabilities using native language calls in Python or MATLAB (and Java).  This chapter explains how to build a command sequence using the API.

Basic Approach
==============
THe GmatCommand class has methods designed to navigate the list of commands in the mission sequence, and to add and remove members of that list based on user actions, shown in :numref:`GmatCommandMethods`.  The API lets users interact through those methods, or through API specific functions that are designed to simplify working with the sequence.

.. _GmatCommandMethods:
.. table:: GmatCommand methods for Sequence Manipulation

	======================= ============== ================================================================
	Method                  Returns        Description
	======================= ============== ================================================================
	GetNext()               GmatCommand*   Returns the next command in the sequence, or a null pointer
	GetPrevious()           GmatCommand*   Returns the previous command in the sequence, or a null pointer
	Append(cmd)             bool           Adds the GmatCommand cmd to the end of the command's sequence
	Insert(cmd, prev)       bool           Inserts cmd into the sequence immediately after the prev command
	Remove(cmd)             GmatCommand*   Removes cmd from the sequence
	GetChildCommand(which)  GmatCommand*   Retrieves a child branch from the command
	Initialize()            bool           Prepares the command for use
	Execute()               bool           Runs the command
	======================= ============== ================================================================

While API users can use these method directly, the commands shown in :ref:`TopLevelInterface` simplify working with the mission sequence, as will be shown shortly.  :numref:`MethodFunctionMap` shows the API functions that correspond to these GmatCommand methods.

.. _MethodFunctionMap:
.. table:: API functions and GmatCommand methods

	======================= ======================= ====================================================
	API Function            GmatCommand Method      Comments
	======================= ======================= ====================================================
	*Not Needed*            GetNext()               API users already have access to the command objects
	*Not Needed*            GetPrevious()           API users already have access to the command objects
	AddCommand              Append(cmd)
	InsertCommand           Insert(cmd, prev)
	*Not Needed*            Remove(cmd)             API users already have access to the command objects
	*Not Needed*            GetChildCommand(which)  Children are managed through the AddCommand method
	InitializeCommand       Initialize()
	ExecuteCommand          Execute()
	Step                    *Not Needed*            GMAT users run full scripts rather than stepping    
	Reset                   *Not Needed*            GMAT users control the sequence differently   
	======================= ======================= ====================================================

.. todo:: Determine if there is a need for child access through a function for API users

Additional information about the API functions can be found in :ref:`APICommandFunctions`.

Two Examples
============
The API functions described above and in :ref:`Commands` are used to build command sequences from the API target Python and Java code bases.  Two examples of this process are presented below: a propagation sequence to perigee and then apogee with reporting following each propagation, and a targeting example used to raise a spacecraft from an initial circular orbit to geosynchronous altitude.

Stepping Propagation
--------------------
In GMAT, a user might want to perform propagation for a spacecraft from an initial state to perigee, and then to apogee, reporting state data along the way, using the scripting in :numref:`PeriApoReportScript`.

.. _PeriApoReportScript:
.. code-block:: python
	:caption: Back to back propagation scripting, with reporting

	BeginMissionSequence

	Propagate Prop(Sat) {Sat.Periapsis};
	Write Sat.X Sat.Y Sat.Z
	Propagate Prop(Sat) {Sat.Apoapsis};
	Write Sat.X Sat.Y Sat.Z


This sequence can be performed in the API using the Python code shown in :numref:`PropPrintProp`.

.. _PropPrintProp:
.. code-block:: python
	:caption: Back to back propagation steps, with reporting

	# The propagation commands
	propagate1 = gmat.Construct("Propagate", "Prop(Sat) {Sat.Periapsis}")
	propagate2 = gmat.Construct("Propagate", "Prop(Sat) {Sat.Apoapsis}")

	gmat.AddCommand(propagate1, propagate2)

	gmat.Initialize()

	if gmat.InitializeCommand(propagate1):
		current = propagate1
		count = 1

		while current is not None:

			# Step() runs the current command to completion and returns the next one
			current = gmat.Step(current)
			print("Position at step ", count, ": [", 
			      sat.GetNumber("X"), sat.GetNumber("Y"), 
			      sat.GetNumber("Z"), "]")

		print("Run complete")



Targeting
---------
The first maneuver for a Hohmann transfer moves a spacecraft from an initial circular orbit with an initial semimajor axis to a transfer trajectory that has apoapse distance equal to the target semimajor axis.  The transfer is completed through a send maneuver that circularizes the orbit.  The size and timing of each maneuver is set by a mission analyst.  Those settings are often generated using a targeting system, simplifying the work performed by hand by the analyst.  GMAT provides a differential corrector component designed for this type of problem.  The Python code shown in :numref:`TargetGEO` shows how to use the command API functions with GMAT's differential corrector to tune the first maneuver, moving a spacecraft from an initial low Earth orbit to geosynchronous altitude.  The code shown there shows the mission sequence portion of the configuration.  :numref:`BurnTargeting` is a complete listing for this example.

.. _TargetGEO:
.. code-block:: python
	:caption: Targeting using the API command interfaces

	# The propagations
	propagate1 = gmat.Construct("Propagate", "Prop(Sat) {Sat.Periapsis}")
	propagate2 = gmat.Construct("Propagate", "Prop(Sat) {Sat.Apoapsis}")

	# Target commands
	target    = gmat.Construct("Target", "DC")
	vary      = gmat.Construct("Vary", "DC(Burn.Element1 = 0.5, {Perturbation = 0.0001})")
	mnvr      = gmat.Construct("Maneuver", "Burn(Sat)")
	achieve   = gmat.Construct("Achieve", "DC(Sat.Earth.RMAG = 42165.0, {Tolerance = 0.1})")
	endtarget = gmat.Construct("EndTarget", "")

	# Build the targeting sequence
	#
	# Note that commands are added to the branch in the Target command 
	# until the EndTarget command is added
	gmat.AddCommand(target, vary)
	gmat.AddCommand(target, mnvr)
	gmat.AddCommand(target, propagate2)
	gmat.AddCommand(target, achieve)
	gmat.AddCommand(target, endtarget)

	# Build the top level sequence so we start at perigee
	gmat.AddCommand(propagate1, target)

	gmat.Initialize()

	if gmat.InitializeCommand(propagate1):
		# Execute() runs the entire sequence to completion
		gmat.Execute(propagate1)
