.. _CommandAccessChapter:

Accessing GMAT Commands
=======================

**Problem**
-----------
GMAT's mission control sequence (MCS) controls the order of actions taken when a simulation is run.  The MCS is a linked list of objects all derived from the GmatCommand base class.  API users may need access to these commands in order to determine the state of the system at specific nodes in the MCS.  This page shows how to do this, using the Ex\_HohmannTransfer sample script. 

**Solution**
------------
The solver control sequences for targeting and for optimization share setting of variables through the Vary command.  Targeting specifies the target goals using the Achieve command.  Optimization uses the Minimize command to set the objective function that is minimized, and the NonlinearConstraint command to constrain the optimization solution.

Setup and Some Comments
***********************
The targeting example used in this chapter uses the Hohmann transfer sample script supplied with GMAT. Create a folder one level up from the run folder, name it scripts, and copy "Ex_HohmannTransfer.script" from the GMAT samples folder into that folder.

The purpose of this chapter is exploration, using the GMAT API, of the mission control sequence.  The MCS in the Hohmann Transfer sample script is shown in 
:numref:`HohmannTransferScript`.

.. _HohmannTransferScript:
.. code-block:: matlab
   :caption: The Full Hohmann Transfer MCS
   :linenos:

   BeginMissionSequence;

   Propagate 'Prop to Perigee' DefaultProp(DefaultSC) {DefaultSC.Periapsis};

   % Burn in the velocity direction to reach an alternate Apoapsis point
   Target 'Raise and Circularize' DC {SolveMode = Solve, ExitMode = DiscardAndContinue};
      Vary 'Vary TOI.V' DC(TOI.Element1 = 0.5, {Perturbation = 0.0001, Lower = 0, Upper = 3.14159, MaxStep = 0.2});
      Maneuver 'Apply TOI' TOI(DefaultSC);
      Propagate 'Prop to Apogee' DefaultProp(DefaultSC) {DefaultSC.Apoapsis};
      Achieve 'Achieve RMAG' DC(DefaultSC.Earth.RMAG = 42165, {Tolerance = 0.1});
      Vary 'Vary GOI.V' DC(GOI.Element1 = 0.5, {Perturbation = 0.0001, Lower = 0, Upper = 3.14159, MaxStep = 0.2});
      Maneuver 'Apply GOI' GOI(DefaultSC);
      Achieve 'Achieve ECC' DC(DefaultSC.ECC = 0, {Tolerance = 0.1});
   EndTarget;  % For targeter DC

   Propagate 'Prop 1 Day' DefaultProp(DefaultSC) {DefaultSC.ElapsedSecs = 86400};

The GMAT MCS shown here consists of two pieces.  The core MCS is a set of four commands:

.. code-block:: matlab
   :caption: The Top Level MCS
   :linenos:

   BeginMissionSequence;
   Propagate 'Prop to Perigee' DefaultProp(DefaultSC) {DefaultSC.Periapsis};
   Target 'Raise and Circularize' DC {SolveMode = Solve, ExitMode = DiscardAndContinue};
   Propagate 'Prop 1 Day' DefaultProp(DefaultSC) {DefaultSC.ElapsedSecs = 86400};

Each line in this list is an instance of a class derived from the GmatComand class.  GmatCommand provides two methods used to manage the MCS: a GetNext() method that returns a pointer to the next command in the list, and a GetPrevious() method that returns the command preceding the command.  Each list is terminated - on both ends - by a null pointer, indicates as a "None" object in Python.

The Target command on line 3 is an example of a "BranchCommand" object in the GMAT code.  Branch commands are commands that can pass processing in more than one direction: either to the next node in the sequence, or to a branch of commands scripted to perform some specific set of actions.  In this example, the Target command branches to the Targeter Control Sequence that specifies the actions followed to tune a pair of maneuvers that move a spacecraft from low Earth orbit to geosynchronous orbit.

This chapter shows how to access these commands and examine them.  Manipulation of the commands, with a focus on manipulations for the targets and optimizers, will be presented in a later chapter.

Top Level Command Access
************************
This section shows how to view the main MCS.

**Step 1**: Load the script

GMAT Scripts are loaded and run in the APIusing the LoadScript function:

.. code-block:: matlab

   from load_gmat import gmat

   # Load the Hohmann transfer script into GMAT.
   retval = gmat.LoadScript("../scripts/Ex_HohmannTransfer.script")

   if retval == False:
      print("The script failed to load.")
      exit()

This function returns the status of the load: True if the script was loaded, False if the load failed.

**Step 2**:  Access the first command in the list

Access to a loaded script in GMAT is made through the GMAT Moderator.  From the API, access to the moderator, and then to the first command (or node) in the MCS is straightforward:

.. code-block:: matlab

   # Connect to the GMAT engine through the Moderator singleton
   mod = gmat.Moderator.Instance()

   # Access the first node of the MCS; by convention, this is a NoOp command
   startnode = mod.GetFirstCommand()

The command list in GMAT always starts with a node that performs no work - a NoOp command in the core code.  Commands in GMAT have both a type and optionally a name.  The type field identifies the command's class.  The Name field stores the command string assigned to the command.  One way to access these attributes of the current command is by simply printing the Python object directly.  The can be accessed individually using the GetTypeName() and GetName() methods:

.. code-block:: matlab
   :caption: Attributes of the first MCS node

   # View the attributes of the node
   print(startnode)
   print("Type:  ", startnode.GetTypeName())
   print("Name:  ", startnode.GetName())

This code produces the output

.. code-block:: text

   Object of type NoOp named 
   Type:   NoOp
   Name:   

The NoOp command does not have a name, so those fields are empty in the output.

**Step 3**:  Access the next command in the list

The MCS itself is pointed to by this NoOp command.  Access to the MCS is made like this:

.. _TheNextNode:
.. code-block:: matlab
   :caption: Accessing the first scripted command

   # Now access the first scripted command
   node = startnode.GetNext()

The node object now points to the first command in the MCS, as can be verified like this:

.. code-block:: matlab
   :caption: Attributes of the current MCS node

   # View the attributes of the current node
   print(node)
   print("Type:  ", node.GetTypeName())
   print("Name:  ", node.GetName())

with output

.. code-block:: text

   Object of type BeginMissionSequence named 
   Type:   BeginMissionSequence
   Name:   

The node object points to the first scripted command, "BeginMissionSequence," as expected.

**Step 4**:  Access the previous command in the list

GMAT's MCS is implemented as a doubly linked list, meaning that the commands are linked both to subsequent commands and to commands that come before the current command.  Subsequent commands are accessed using GetNext(), and shown above.  The predecessors to the current command are accessed using GetPrevious().  For example, the node object set in :numref:`TheNextNode` points back to the NoOp command at the start of the list, and can be shown like this:

.. code-block:: matlab

   print(node.GetPrevious())

producing the expected output:

.. code-block:: text

   Object of type NoOp named 

**Step 5**:  Display the MCS

The methods presented above can be used to view the complete top level MCS.  The function WalkTheMCS() shown in :numref:`WalkTheMCS` shows how to move through the control sequence and access each node's descriptive attributes.

.. _WalkTheMCS:
.. code-block:: matlab
   :caption: A Utility Function to Show the MCS

   def WalkTheMCS(node):
      '''
         Simple function to show a GmatCommand linked list, omitting children

         inputs:
            node  The starting node for the command list

         returns:
            None.  The command list is written to the display 
      '''
      count = 1

      while node != None:
         indent = "  "
         print(indent, count, ":  ", '{:20}'.format(node.GetTypeName()), node.GetName())
         node = node.GetNext()
         count = count + 1

Whenthis function is called with the node pointing to the BeginMissionSequence command, the ouput shown in :numref:`TheTopMCS` is displayed.

.. _TheTopMCS:
.. code-block:: text
   :caption: The MCS shown using WalkTheMCS()

      1 :   BeginMissionSequence 
      2 :   Propagate            Prop to Perigee
      3 :   Target               Raise and Circularize
      4 :   Propagate            Prop 1 Day

Compare this output with :numref:`HohmannTransferScript` to see how the methods called from the API align with the GMAT scripting.

Branch Command Access
*********************
The MCS for the Hohmann transfer script includes a targeting loop.  The sequence of commands in that loop are not shown when running the WalkTheMCS() function because the targeter commands are branched off of the main mission control sequence.  That branch of commands, called the solver control sequence, is scripted off of the Target command like this in the sample script:

.. _HohmannTransferSCS:
.. code-block:: matlab
   :caption: The Solver Control Sequence for the Hohmann Transfer
   :linenos:

   Target 'Raise and Circularize' DC {SolveMode = Solve, ExitMode = DiscardAndContinue};
      Vary 'Vary TOI.V' DC(TOI.Element1 = 0.5, {Perturbation = 0.0001, Lower = 0, Upper = 3.14159, MaxStep = 0.2});
      Maneuver 'Apply TOI' TOI(DefaultSC);
      Propagate 'Prop to Apogee' DefaultProp(DefaultSC) {DefaultSC.Apoapsis};
      Achieve 'Achieve RMAG' DC(DefaultSC.Earth.RMAG = 42165, {Tolerance = 0.1});
      Vary 'Vary GOI.V' DC(GOI.Element1 = 0.5, {Perturbation = 0.0001, Lower = 0, Upper = 3.14159, MaxStep = 0.2});
      Maneuver 'Apply GOI' GOI(DefaultSC);
      Achieve 'Achieve ECC' DC(DefaultSC.ECC = 0, {Tolerance = 0.1});
   EndTarget;  % For targeter DC

The Target command here is apart of the top level MCS.  The remaining lines are the solver control sequence command, and access to the solver commands requires code that accesses that solver control sequence branch.

**Step 1**: Locate a Branch Command Object

In GMAT, every command that manages branching is derived from the BranchCommand class.  That feature can be used to find the branch commands in the MCS by checking each command to determine if it is a branch command:

.. code-block:: python

   node = startnode.GetNext()
   while node != None:
      branchstatus = "does not branch"
      if node.IsOfType("BranchCommand"):
         branchstatus = "is a branch command"
      print(node.GetTypeName(), branchstatus)
      node = node.GetNext()

This code block locates the Target command in the Hohmann transfer script and identifies it as a command that has a branch:

.. code-block:: text

   BeginMissionSequence does not branch
   Propagate does not branch
   Target is a branch command
   Propagate does not branch

**Step 2**: Access the child command from the Branch Command

Now that the branch command has been located, the first command in the branch is retrieved using the GetChildCommand() method, like this:

.. code-block:: python

   child = node.GetChildCommand()

**Step 3**: Walk through the command branch

Using the child node, the solver control sequence in the Hohmann transfer script can be examined using the same techniques followed for the top level MCS.  This can be done using an updated, and now recursive, function to walk the MCS:

.. code-block:: python

   def WalkTheMCSWithBranches(node, depth = 1, parent = None):
      '''
         Simple function to show a GmatCommand linked list, including children.

         Note that there is a hack in this implementation.  Ideally, we would 
         check nodes against the parent pointer, but in Python, we have 
         references to SWIG pointers to the GmatCommand object pointers.  It's a 
         windy little maze of rooms that all look the same, and that need to be 
         unwound when time permits.

         inputs:
            node  The starting node for the command list
            depth The current depth of the call (used for indentation)
            parent The node that has node as a child, or None for a top level node

         returns:
            None.  The command list is written, including branches, to the display 
      '''
      count = 1
      indent = "  " * depth

      while node != None:

         print(indent, count, ":  ", '{:20}'.format(node.GetTypeName()), node.GetName())

         # Handle the kiddos
         if node.IsOfType("BranchCommand"):
            
            child = node.GetChildCommand()
            WalkTheMCSWithBranches(child, depth + 1, node)

         node = node.GetNext()
         count = count + 1

         if parent != None:
            # Note: This is a hack for now.  We want to compare command pointers,
            # but they are buried several levels deep and are not easily accessed
            if node.GetTypeName() == parent.GetTypeName() and node.GetName() == parent.GetName():
               node = None

Calling WalkTheMCSWithBranches(node) generates the full mission control sequence with branches when called with node set to the BeginMissionSequence command:

.. code-block:: text

      1 :   BeginMissionSequence 
      2 :   Propagate            Prop to Perigee
      3 :   Target               Raise and Circularize
        1 :   Vary                 Vary TOI.V
        2 :   Maneuver             Apply TOI
        3 :   Propagate            Prop to Apogee
        4 :   Achieve              Achieve RMAG
        5 :   Vary                 Vary GOI.V
        6 :   Maneuver             Apply GOI
        7 :   Achieve              Achieve ECC
        8 :   EndTarget            
      4 :   Propagate            Prop 1 Day

Viewing Run Results
*******************
The instructions provided above show how to access the commands in the MCS.  Each command supports a field that reports the state of the spacecraft in the MCS the last time the command was accessed.  The summary for a specific command is accessed through the "Summary" field name.  Before the script has been run, the summary text data is not populated, and the field returns text to that effect.  For example, if the node points to the last propagate command, then accessing the Summary field produces the message

.. code-block:: text

   >>> print(node.GetField("Summary"))
   ******  Changes made to the mission will not be reflected ******
   ******  in the data displayed until the mission is rerun  ******

           Propagate Command: Unnamed
           Command summary is not supported for Propagate (in Single Step Mode)
           or when the command did not execute due to control logic statements.
           Please see the next valid command.

After the script has run, the summary contains more useful data:

.. code-block:: text

   >>> gmat.RunScript()
   True
   >>> print(node.GetField("Summary"))
   ******  Changes made to the mission will not be reflected ******
   ******  in the data displayed until the mission is rerun  ******

           Propagate Command: Unnamed
           Spacecraft       : DefaultSC
           Coordinate System: EarthMJ2000Eq

           Time System   Gregorian                     Modified Julian  
           ----------------------------------------------------------------------    
           UTC Epoch:    02 Jan 2000 18:32:14.796      21546.2723934740
           TAI Epoch:    02 Jan 2000 18:32:46.796      21546.2727638444
           TT  Epoch:    02 Jan 2000 18:33:18.980      21546.2731363444
           TDB Epoch:    02 Jan 2000 18:33:18.980      21546.2731363440

           Cartesian State                       Keplerian State
           ---------------------------           -------------------------------- 
           X  =   7059.8054404060 km             SMA  =   42165.000291122 km
           Y  =   41006.405025033 km             ECC  =   0.0000007749653
           Z  =   6820.5046046645 km             INC  =   12.852951589765 deg
           VX =  -2.9804993875227 km/sec         RAAN =   306.15458093975 deg
           VY =   0.5912233799383 km/sec         AOP  =   180.88186603791 deg
           VZ =  -0.4695068593848 km/sec         TA   =   312.46888463718 deg
                                                 MA   =   312.46895014326 deg
                                                 EA   =   312.46891739023 deg

           Spherical State                       Other Orbit Data
           ---------------------------           --------------------------------
           RMAG =   42164.978228331 km           Mean Motion        =   7.291900370e-05 deg/sec
           RA   =   80.231521342877 deg          Orbit Energy       =  -4.7266742410520 km^2/s^2
           DEC  =   9.3089268671491 deg          C3                 =  -9.4533484821041 km^2/s^2
           VMAG =   3.0746314210029 km/s         Semilatus Rectum   =   42165.000291097 km   
           AZI  =   98.901788705887 deg          Angular Momentum   =   129641.76692671 km^2/s
           VFPA =   90.000032753035 deg          Beta Angle         =  -17.350375964570 deg  
           RAV  =   168.78023389434 deg          Periapsis Altitude =   35786.831314710 km   
           DECV =  -8.7836287310829 deg          VelPeriapsis       =   3.0746321949380 km/s
                                                 VelApoapsis        =   3.0746274294752 km/s 
                                                 Orbit Period       =   86166.636794673 s    

           Planetodetic Properties 
           ---------------------------
           LST       =   80.231355220914 deg
           MHA       =   19.777879001376 deg
           Latitude  =   9.3164365259040 deg
           Longitude =   60.453476219538 deg
           Altitude  =   35787.400879520 km


           Spacecraft Properties 
           ------------------------------
           Cd                    =   2.200000
           Drag area             =   15.00000 m^2
           Cr                    =   1.800000
           Reflective (SRP) area =   1.000000 m^2
           Dry mass              =   850.00000000000 kg
           Total mass            =   850.00000000000 kg
           SPADDragScaleFactor   =   1.000000
           SPADSRPScaleFactor    =   1.000000

There are a few additional features worth mentioning:

*  When the node used to generate the summary data is a branch command, the summary is reported for that node and for each node in the branch. 
*  Commands also have an option to report the summary, starting from the selected node, through the end of the control sequence.  You can access that report through the "MissionSummary" field of the command.

**Discussion**
--------------
This chapter provides an overview showing how to use the GMAT API to access the commands in a script and to begin viewing the data associated with the commands.  Subsequent chapters show how to change data in selected commands.
