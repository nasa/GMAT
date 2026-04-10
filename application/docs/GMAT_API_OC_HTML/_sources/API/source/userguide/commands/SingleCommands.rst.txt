.. _Commands:

*********************
Configuring a Command
*********************
GmatCommand objects are the building blocks of the Mission Control Sequence.  The mission sequence is coded as a doubly linked list of GmatCommands, with, for some commands, branching enabled that into a subsequence of commands that perform a specific task for the main command.  This design means that there are two elements of the GMAT command structure needed by API users: the ability to configure commands that they need to run, and the ability to chain together commands to build a full mission timeline.

This chapter focuses on the procedure that makes a single GMAT command accessible to an API user.  The Propagate command is used to illustrate the procedure.  Information about sequences of commands can be found in :ref:`CommandSequences`.

Command Categories
==================
GMAT commands can be grouped into five different types of commands exposed to the command API:

* Basic sequence actions
	* GMAT (used for Assignments)
	* Maneuver
	* Propagate (Base class PropagationEnabledCommand)
	* Report
	* Toggle
	* Write
	* BeginFiniteBurn
	* EndFiniteBurn
* Commands with subsequences, with base classes BranchCommand and ConditionalBranch
	* For / EndFor
	* If / Else / EndIf
	* While / EndWhile
* Solver commands, with base classes SolverSequenceCommand, SolverBranchCommand, or RunSolver
	Target / EndTarget
	Optimize / EndOptimize
* Solver action commands
	* Vary
	* Achieve
	* Minimize
	* NonlinearConstraint
* Function calling commands
	* BeginFunction / EndFunction
	* CallFunction
	* CallBuiltinGmatFunction

The basic sequence action commands provide many of the core building blocks of a mission sequence.  These commands can be used as stand alone calls by API users to perform specific actions: Propagate to a stopping condition, maneuver a spacecraft, toggle writing for published data, and so forth.

The remaining commands all use a command sequence in one way or another. 

In addition to these command types, there are commands that are not exposed to the API at this time, for a variety of reasons stated below:

* Sequence Management commands
	* BeginMissionSequence
	* NoOp
* Sequence utility commands
	* BeginScript / EndScript
* Commands that interact with the GMAT GUI
	* ClearPlot
	* PenDown
	* PenUp
	* MarkPoint
	* PlotCommand
	* SaveMission
	* Stop
	* UpdateDynamicData
	* FindEvents

  The command API does not expose the sequence management commands because they are not needed for API users. The BeginScript and EndScript commands are used to group together GMAT commands so that they can be hidden on the Mission tab of the GMAT GUI.  The final set of commands are designed to manipulate the GMAT GUI in a variety of ways.  Since API users are not running the GUI, they are not needed.

Commands that are built into GMAT plugin code are not included in this list.  Of particular for many API users are the RunSimulator and RunEstimator commands.  Those commands are configured as single commands that are executed, and are covered by the design described in this chapter.  Simulation and Estimation are complex processes, so users that need API access to those two commands in particular, should pay careful attention to the resource creation process for the components needed for orbit determination configurations. 

Setting up a Command
====================
While Resources built with the API use the Construct function.  Commands are built using command function.  :numref:`ConstructSyntax` illustrates the difference by summarizing the syntax followed by an example.  

.. _ConstructSyntax:
.. code-block:: python
	:caption: Syntax for the Construct command

	# Resource creation:  gmat.Construct(<Resource Type>, <Resource Name>)
	sat = gmat.Construct("Spacecraft", "Sat")
	sat.SetField("X",  6600.0)
	sat.SetField("Y",     0.0)
	sat.SetField("Z",   300.0)
	sat.SetField("VX",    0.0)
	sat.SetField("VY",    8.0)
	sat.SetField("VZ",    0.0)

	# Command creation:  gmat.Command(<Command Type>, <Command text>)
	evolve = gmat.Command("Propagate", "Prop(Sat) {Sat.RMAG = 7200}")

In GMAT, resources are objects that have separate script entries for each element that is being built.  Those settings are configured in the API using SetField() method calls like those shown in the listing.  Commands are scripted using a single line of text with all of the command configuration data on the line of script.  The API follows this approach, building resources through construction and field setting and commands in a single construction step.  The GMAT scripting that corresponds to :numref:`ConstructSyntax` is shown in :numref:`ScriptSyntax`.

.. _ScriptSyntax:
.. code-block:: python
	:caption: GMAT scripting corresponding to the API calls in :numref:`ConstructSyntax`.

	Create Spacecraft Sat
	Sat.X  = 6600.0;
	Sat.Y  =    0.0;
	Sat.Z  =  300.0;
	Sat.VX =    0.0;
	Sat.VY =    8.0;
	Sat.VZ =    0.0;

	Propagate Prop(Sat) {Sat.RMAG = 7200}

APIInterpreter is a new interpreter introduced in the GMAT R2024a code base. It provides, for commands constructed through the API, the same functionality as is provided by the ScriptInterrpter


Command Initialization and Execution
====================================
The Initialize() method in the GMAT API initializes the objects created and named by the user, and stored in GMAT's configuration.  Commands are also stored in the configuration, so this method does  initialize Mission Control Sequence commands. Command sequences are initialized by the same Initialize() function call. 

After initialization, the command or command sequence is run by calling the Execute() method.  That method fires current command sequence, running to completion of the passed in sequence and returning a int indicating the result: 

* 1 if run was successful
* -1 if sandbox number is invalid
* -2 if exception thrown during sandbox initialization
* -3 if unknown error occurred during sandbox initialization
* -4 if execution interrupted by user
* -5 if exception thrown during the sandbox execution
* -6 if unknown error occurred during sandbox execution
* -7 if unkown error at the API level
