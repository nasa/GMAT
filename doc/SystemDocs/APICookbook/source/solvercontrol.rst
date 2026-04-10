**Control of Scripted Solvers**
===============================

This chapter uses targeting and optimization scripts to demonstrate control of the targeting and optimization processes using the GMAT API.  It uses the sample scripts for a Hohmann transfer and for a minimum fuel lunar orbit insertion to show the steps used for control of the related processes. This chapter build on the command access capabilities described in :ref:`CommandAccessChapter`.

Some of the code presented in this chapter uses the functions defined in :ref:`CommandFunctionPython`, imported through the CommandFunctions package.


**Problem**
-----------
GMAT's optimizers and targeters provide a set of tools that can be used to tune the parameters used when solving analysis problems.  These solver capabilities are scripted in a "Solver Control Sequence."  The Solver control sequence is an ordered list of GMAT commands that work together to solve a scripted analysis problem.  An example can be found in the Ex\_HohmannTransfer sample script.  The solver control sequence for from that script is shown in :numref:`ATargetingSequence`.

.. _ATargetingSequence:
.. code-block:: matlab
   :caption: The Hohmann Transfer targeting sequence

	Target 'Raise and Circularize' DC;
	   Vary 'Vary TOI.V' DC(TOI.Element1 = 0.5, {Perturbation = 0.0001, ...
	       Lower = 0, Upper = 3.14159, MaxStep = 0.2});
	   Maneuver 'Apply TOI' TOI(DefaultSC);
	   Propagate 'Prop to Apogee' DefaultProp(DefaultSC) {DefaultSC.Apoapsis};
	   Achieve 'Achieve RMAG' DC(DefaultSC.Earth.RMAG = 42165, {Tolerance = 0.1});
	   Vary 'Vary GOI.V' DC(GOI.Element1 = 0.5, {Perturbation = 0.0001, ...
	       Lower = 0, Upper = 3.14159, MaxStep = 0.2});
	   Maneuver 'Apply GOI' GOI(DefaultSC);
	   Achieve 'Achieve ECC' DC(DefaultSC.ECC = 0, {Tolerance = 0.1});
	EndTarget;

Sometimes the scripted control sequence needs to be adjusted, either because the goals of the sequence have changed or because the variables are not conditioned properly, leading to an inability to converge on a solution to the analysis problem.  API users need to be able to tune the solver sequence through API calls to the underlying objects.  

**Solution**
------------
The solver control sequences for targeting and for optimization share setting of variables through the Vary command.  Targeting specifies the target goals using the Achieve command.  Optimization uses the Minimize command to set the objective function that is minimized, and the NonlinearConstraint command to constrain the optimization solution.  The API provides access to each of these commands so that their settings can be viewed and changed.  The sections below show how to work with each component.

Vary Command Access
*******************
Scripting for the Vary command is shown in :numref:`AVaryCommand`.

.. _AVaryCommand:
.. code-block:: matlab
   :caption: A single Vary command

   Vary 'Vary TOI.V' DC(TOI.Element1 = 0.5, {Perturbation = 0.0001, ...
          Lower = 0, Upper = 3.14159, MaxStep = 0.2, ...
          AdditiveScaleFactor = 0.0, MultiplicativeScaleFactor = 1.0});

The right side of the equality setting for each field in the command can be set in the API.  The API also provides access to the most recent evaluated value for the variable.  :numref:`VaryFields` shows the field names and access attributes available through the API.

.. _VaryFields:
.. table:: Fields on the Vary Command

   ============================= ======== ================================================================
   Field                         Access   Description and notes 
   ============================= ======== ================================================================
   SolverName                    R        The name of the targeter or optimizer controlling this variable
   InitialValue                  R/W      Variable value used to start the targeting or optimization
   Perturbation                  R/W      Perturbation applied to compute the Jacobian numerically.  
                                          0.0 not allowed
   Lower                         R/W      The minimum allowed value for the variable, for solvers that 
                                          allow control over the minimum.
   Upper                         R/W      The maximum allowed value for the variable, for solvers that 
                                          allow control over the maximum.
   MaxStep                       R/W      The largest change allowed in the magnitude of the variable
   AdditiveScaleFactor           R/W      A factor added to the variable value 
   MultiplicativeScaleFactor     R/W      A factor used to scale the variable
   CurrentValue                  R        The most recent value of the variable during a run.
   ============================= ======== ================================================================

API users access these settings after loading a script.  They can be accessed prior to a run, or after the script has been run in preparation for a subsequent run.  :numref`VaryExample` shows how to set some of the field settings from the API.  

.. _VaryExample:
.. code-block:: python
   :caption: Sample code for modifying a Vary command

   from load_gmat import gmat
   import CommandFunctions as cf

   # Load the Hohmann transfer script into GMAT.
   retval = gmat.LoadScript("../scripts/Ex_HohmannTransfer.script")

   mcs = cf.GetMissionSequence()

   # Locate the Target command
   solverCmd = cf.FindSolverCommand(mcs)

   # Find the 1st Vary command
   vary1 = cf.FindChild(solverCmd, "Vary")

   # Change some settings
   val = 2.4

    # Set from a Python variable
   vary1.SetField("InitialValue", val)
   
   # Set from a numeric value
   vary1.SetField("Perturbation", 0.0005)

Control of the Vary command parameters is a stepping stone to control of GMAT's targeting and optimization processes, discussed below.

Use Case 1: Targeting
*********************
Targeting in GMAT is driven by commands between an opening "Target" command and a closing "EndTarget" command.  Example script is shown in :numref:`TargetEndTargetExample`.

.. _TargetEndTargetExample:
.. code-block:: matlab
   :caption: The start and end of a targeter control sequence

   Target DC
      % Control sequence, with at least one Vary and one Achieve command, goes here
   EndTarget

That target control sequence contains the variables, manipulated by a Vary command (described above), a timeline of actions to execute, and the targeting goals, captured in Achieve commands.  The Target command provides a field used to find the name of the targeter used, and a simple Boolean field that checks to see if the targeter control sequence has succeeded in finding a valid solution.

.. _TargetFields:
.. table:: Fields on the Target Command

   ======================= ====== ================================================================
   Field                   Access Description and notes                        
   ======================= ====== ================================================================
   SolverName              R      The name of the targeter used in the run
   Targeter                R      The name of the targeter used in the run
   TargeterConverged       R      Boolean flag indicating if targeter convergence has been 
                                  achieved
   ======================= ====== ================================================================

The Achieve command (scripting: :numref:`AchieveScripting`) controls the goals of the targeting run.  

.. _AchieveScripting:
.. code-block:: matlab
   :caption: Example of an Achieve command

   Achieve DC(sat.SMA = 42165.0, {Tolerance = 0.1});

The user sets desired values for the targeter goals and tolerances on those values.  The targeter retrieves those settings and used them, along

.. _AchieveFields:
.. table:: Fields on the Achieve Command

   ======================= ====== ================================================================
   Field                   Access Description and notes                        
   ======================= ====== ================================================================
   SolverName              R      The name of the targeter controlling this goal
   Goal                    R      String identifying the targeter goal
   GoalValue               R/W    The desired value of the goal
   Tolerance               R/W    The precision needed in the achieved value relative to the 
                                  desired value in the converged solution
   AchievedValue           R      The value of the goal following a run.  Note that this is a 
                                  numeric field only accessed using GetNumber()
   ======================= ====== ================================================================
 
The Achieve fields provide full control over teh goals of the targeting run.  :numref:`AchieveExample` illustrated their use.

.. _AchieveExample:
.. code-block:: python
   :caption: Sample code setting the tolerance on an Achieve command and accessing the Target command fields

   achieve2 = cf.FindChild(solverCmd, "Achieve", 2)
   achieve2.SetField("Tolerance", 1e-7)

   print("Variable 1:  Initial Value:", vary1.
   gmat.RunScript()

   print("The targeter", solverCmd.GetField("Targeter"), 
      "completed its run with convergence status", 
      solverCmd.GetField("TargeterConverged"))

   print("The goal", achieve2.GetField("Goal"), "=", 
      achieve2.GetField("GoalValue"), "has a tolerance of",
      achieve2.GetField("Tolerance"), "and a final value",
      achieve2.GetNumber("AchievedValue"))

The full sample Python code used when writing this section can be found in :ref:`TargeterPythonDriver`.

Use case 2: Optimization
************************
Optimization in GMAT is driven by commands between an opening "Optimize" command and a closing "EndOptimize" command.  Example script for these commands is shown in :numref:`OptimizeEndOptimizeExample`.

.. _OptimizeEndOptimizeExample:
.. code-block:: matlab
   :caption: The start and end of an optimizer control sequence

   Optimize opt
      % Control sequence, with at least one Vary and one Minimize or NonlinearConstraint command, goes here
   EndOptimize

That optimization control sequence contains the variables, manipulated by a Vary command (described above), a timeline of actions to execute, and the optimization objective function, captured in a Minimize command, along with any constraints to be applied to the optimization process, specified using NonlinearConstraint commands.  The Optimize command provides a field used to find the name of the optimizer used, and a simple Boolean field that checks to see if the optimization control sequence has succeeded in finding a valid solution.  These fields are described in :numref:`OptimizeFields`.

.. _OptimizeFields:
.. table:: Fields on the Optimize Command

   ======================= ====== ================================================================
   Field                   Access Description and notes                        
   ======================= ====== ================================================================
   SolverName              R      The name of the optimizer used in the run
   OptimizerName           R      The name of the optimizer used in the run
   OptimizerConverged      R      Boolean flag indicating if optimizer convergence has been 
                                  achieved
   ======================= ====== ================================================================

Linear and nonlinear equality and inequality constraints are specified in NonlinearConstraint commands.  The fields of those commands, shown in :numref:`ConstraintFields`, provide similar control for optimization to the Achieve controls for targeting.  

.. _ConstraintScripting:
.. code-block:: matlab
   :caption: Example of an optimization constraint

   NonlinearConstraint opt(MMSRef.Luna.SMA = 2300);

The constraints, scripted as in :numref:`ConstraintScripting`, are specified in the format <arg1> operator <arg2>.  The API provides methods for evaluating both the left side argument, arg1, and the right side argument, arg2 using the fields shown in :numref:`ConstraintFields`.  The value for arg2 can also be specified using the RHSValue field.  Tolerances can be set on nonlinear constraints using the Tolerance setting.  Users should be cautious with this setting: some optimizers support it at the individual constraint level, while other do not.
 
.. _ConstraintFields:
.. table:: Fields on the NonlinearConstraint Command

   ======================= ====== ================================================================
   Field                   Access Description and notes                        
   ======================= ====== ================================================================
   SolverName              R      The name of the optimizer controlling this goal
   ConstraintArg1          R      The text description of the left hand side of the constraint
   LHSValue                R      The most recent (or desired) value of argument 1.  Note that the
                                  argument to the left of the operator is not a number, and cannot 
                                  be set.
   ConstraintArg2          R      The text description of the right hand side of the constraint
   RHSValue                R/W    The most recent (or desired) value of argument 2
   Operator                R/W    The operator used to compare the left and right arguments.  
                                  Valid operators are >=, =, and <=.
   Tolerance               R/W    Tolerance setting for equality constrains, for optimizers that 
                                  support that setting at the constraint level.
   ======================= ====== ================================================================

The final component used in optimization problems if the objective function, defining the function that needs to be minimized during optimization.  In GMAT scripting, that function is set using the Minimize command exemplified in :numref:`ObjectiveFunctionScripting`.

.. _ObjectiveFunctionScripting:
.. code-block:: matlab
   :caption: Scripting for the objective function definition for optimization

   Minimize opt(myCostFunction);

The API provides access to reading the objective function settings using the fields shown in :numref:`MinimizeFields`.

.. _MinimizeFields:
.. table:: Fields on the Minimize Command

   ======================= ====== ================================================================
   Field                   Access Description and notes                        
   ======================= ====== ================================================================
   SolverName              R      The name of the optimizer controlling this goal
   OptimizerName           R      The name of the optimizer controlling this goal
   ObjectiveName           R      The scripted cost function
   Cost                    R      The most recent value of the objective function 
   ======================= ====== ================================================================
 
An example showing the use of these settings is shown in :numref:`OptimizerControlExample`.

.. _OptimizerControlExample:
.. code-block:: python
   :caption: Sample code (simplified) showing control of constraints and access to the objective function

   # See full example (below) for some definitions

   nlc =  cf.FindChildByName(solverCmd, "SMA = 2300")
   nlc.SetField("RHSValue", 2100.0)

   # Run with the scripted settings
   gmat.RunScript()

   minCommand = cf.FindChild(solverCmd, "Minimize")

   print("\n\nObjective function:  ", minCommand.GetField("ObjectiveName"), 
         "=" , minCommand.GetNumber("Cost"), "\n")

   print("Constraint results:", nlc.GetField("ConstraintArg1"), 
         nlc.GetField("Operator"), nlc.GetField("ConstraintArg2"), 
         " ==> ", nlc.GetNumber("LHSValue"), nlc.GetField("Operator"), 
         nlc.GetNumber("RHSValue"))


The full sample Python code used when writing this section can be found in :ref:`OptimizerPythonDriver`.

**Discussion**
--------------
This chapter provides usage examples for controlling GMAT scripts that target and optimize trajectories using the Python API.

**Complete Targeter Script**
----------------------------

.. _TargeterPythonDriver:
.. literalinclude:: code/EditVariable_Direct.py
   :language: python
   :caption: Complete example for targeter control

**Complete Optimizer Script**
-----------------------------

.. _OptimizerPythonDriver:
.. literalinclude:: code/EditOptimizer_Direct.py
   :language: python
   :caption: Complete example for optimizer control
