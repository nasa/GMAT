.. _TopLevelInterface:

***************************
API Access to GMAT Commands
***************************
:numref:`RunScript` in the :ref:`Introduction` shows how GMAT responds when a user executes a script.  The GMAT API provides a set of functions, described in this chapter, that reproduce the key features of this set of actions for API users.  

.. note:: In the text that follows, the word "command" refers to a GmatCommand member of the Mission Control Sequence when referencing the GMAT application, and to a GmatCommand object when referencing the GMAT API.  A set of linked together commands is called a "sequence."  The term "Mission Control Sequence," abbreviated as "MCS," is reserved for the linked list of commands that are executed when the GMAT application runs a script.

Running in GMAT, the objects created in a user's script are copied into the GMAT Sandbox through a cloning process.  The scripted objects are preserved as scripted and not changed during the run.  Instead, clones of those objects are manipulated during the run.  This cloning is the action represented in the "Clone Resources" bubble in the diagram.  The Mission Control Sequence (MCS) linked list is not cloned.  The MCS linked list created when the script is parsed is directly assigned to the Sandbox used for the run.  The Sandbox is then initialized, resources first followed by the MCS commands.  This initialization process sets up the references between objects used in the run and sets up internal object data.  Resources are initialized first so that the internal object data manipulated by the control sequence is available when the sequence is initialized.  The Mission Control Sequence is then initialized by walking the initialization process through the linked list, initializing the commands in the same order as is followed when the sequence executes.  Once everything used in the run is initialized, the Mission Control Sequence is executed by walking through the linked list of commands, calling the Execute() method on each command until the end of the list is reached.  To summarize, a GMAT run performs these steps when a user runs a loaded script:

#.	Clone resources into a Sandbox
#.	Assign the first node of the Mission Control Sequence list to the Sandbox
#.	Initialize the Resource objects in the Sandbox.  (This initialization follows a specific order of object initialization, the details of which are not important here.)
#.	Initialize the Mission Control Sequence node by node
#.	Execute the Mission Control Sequence

There are several differences between running commands in the managed Sandbox environment of GMAT and running commands through the GMAT API.  

*	In the GMAT API, runs typically are performed on the configured objects directly.  
*	Steps 1 and 2 above are not needed.  
*	The Initialize() command familiar to API users performs step 3, initializing Resources.
*	Additional steps are preformed to execute commands

The functions described below are designed to perform steps 4 and 5, addressing the "additional steps" referenced in the final bullet.

.. _APICommandFunctions:

API Functions for the MCS
=========================
:ref:`MCSFunctions` lists the core functions used to run commands in the GMAT API.  Information about each function is provided following the table.

.. _MCSFunctions:
.. table:: Control Sequence Functions

	================= ============================ ============ ============================================
	Function          Example                      Return Value Description
	================= ============================ ============ ============================================
	Command        	  Command(type, desc)          GmatCommand  Creates a GmatCommand and adds it to the
	                                                            end of the current sequence of GMAT Commands
	Execute           Execute()                    Int      	Runs the sequence of GMATCommands
	Delete Command    DeleteCommand(cmd)           GmatCommand  Deletes the command, removing it from the sequence
	================= ============================ ============ ============================================

**Command** creates a GmatCommand object using the type of command that is built followed by the script syntax used in the command. The code then chains through the current sequence of commands and places the object at the end of the list. For branch commands, the function inserts the command into the branch until the branch terminator is added, and then moves to the next element in the main list.

**Execute** runs the current sequence of commands to completion.

**Delete Command** Removes a command from the command sequence. If deleting branch command, it will remove and delete all children from the branch

Example: Executing a Command
============================
:numref:`BasicPropagate` sketches out Python scripting for a call to the GMAT Propagate command that moves a spacecraft from an initial state to perigee, scripted in GMAT like this:  

.. code-block:: python

	Propagate Prop(Sat) {Sat.Earth.Periapsis}

The full listing for this script is given in :numref:`PropagateToPerigee`.

.. _BasicPropagate:
.. code-block:: python
	:caption: Setting up and running a Propagate command
	:linenos:

	from load_gmat import gmat

	# Core resources used; Configuration omitted
	sat  = gmat.Construct("Spacecraft", "Sat")
	fm   = gmat.Construct("ForceModel", "FM")
	prop = gmat.Construct("Propagator", "Prop")

	propagate = gmat.Command("Propagate", "Prop(Sat) {Sat.Periapsis}")

	gmat.Initialize()

	gmat.Execute()


Example: Executing a Sequence
=============================
A basic GMAT script block for targeting is shown in :numref:`ATargeterControlSeq`.  This scripting targets a burn that moves a spacecraft from an initial orbit to Geosynchronous distance.

.. _ATargeterControlSeq:
.. code-block:: matlab
	:caption: Setting up and running a Targeter
	:linenos:

	% Create resources (configuration omitted)
	Create Spacecraft Sat
	Create ForceModel FM
	Create Propagator Prop
	Create ImpulsiveBurn Burn1
	Create DifferentialCorrectorDC

	BeginMissionSequence

	Target DC

	Vary DC(Burn1.Element1 = 0.5, {Perturbation = 0.0001, MaxStep = 0.2})
	Maneuver Burn1(Sat)
	Propagate Prop(Sat) {Sat.Periapsis}
	Achieve DC(Sat.RMAG = 42165.0, {Tolerance = -0.1})
	
	EndTarget

The corresponding API configuration is shown in :numref:`APITargeting`.  

.. _APITargeting:
.. code-block:: python
	:caption: API Calls that Run a Targeter
	:linenos:


	from load_gmat import gmat

	# Core resources used; Configuration omitted
	sat  = gmat.Construct("Spacecraft", "Sat")
	fm   = gmat.Construct("ForceModel", "FM")
	prop = gmat.Construct("Propagator", "Prop")
	burn = gmat.Construct("ImpulsiveBurn", "Burn1")
	dc = gmat.Construct("DifferentialCorrector", "DC")

	tcs = gmat.Command("Target", "DC")
	gmat.Command("Vary", "DC(Burn1.Element1 = 0.5, {Perturbation = 0.0001, MaxStep = 0.2})")
	gmat.Command("Maneuver", "Burn1(Sat)")
	gmat.Command("Propagate", "Prop(Sat) {Sat.Periapsis}")
	gmat.Command("Achieve", "DC(Sat.RMAG = 42165.0, {Tolerance = -0.1})")
	gmat.Command("EndTarget")

	# Setup
	gmat.Initialize()

	# and run
	gmat.Execute()

