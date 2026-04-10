The GMAT Script Debugger
************************
The General Mission Analysis Tool, GMAT, is driven from a script language tailored to the features built into the system.  

GMAT scripts can be quite simple, defining a few components and then using them to perform basic calculations.  New users generally start with this type of setup, and then add features to their scripts as their needs evolve.  Over time, a user's scripts become quite complex in order to model a spacecraft mission accurately.

Along the path from simple to complex scripting, many users find that they run into roadblocks preventing progress because they cannot identify how GMAT is responding to their scripts.  The GMAT debugger, described in this document, addresses this need by allowing users to interact with their scripts while they are running.

Development Plan
================
The GMAT Debugger is being built in stages, providing early functionality to assess feature needs and enhancements for the user community.  The :ref:`requirements` for the system will evolve based on this spiral development methodology.  The lists below are broken into groups that reflect these near term and long term development cycles. 

Build 1: Concept Development
----------------------------
The focus of the initial development cycle is a set of GMAT components that show that a debugger, integrated into the GMAT GUI, is feasible.  Towards that end, the following steps will be performed:

#. Create a "Breakpoint" command that can be inserted into the GMAT mission control sequence that pauses execution.  The user resumes execution by hitting the "Play" button on the tool bar.  This command at this point mimics what happens when a user hits the toolbar "Pause" button (and the code is likely a copy of that code, with changes as needed).

#. Using the Breakpoint command, enable two options using a pop-up dialog or other mechanism. At this point the presentation is less important than functionality.

   * Continue - Resume script execution (like hitting "Play" - we can probably use a copy of that code, massaged for this part.)

   * Examine  - Lets the user can type the name of an object then shows its current settings.  At this iteration the inspector is very basic.  It calls "GetRuntimeObject" to access the object that is requested, and then calls "GetGeneratingString" to display the object's settings.

#. Set up a few demonstration scripts and test things to identify potential issues.  The following need to be included in this:

   * A basic script with a single breakpoint

   * A script with more than one breakpoint, and commands in between

   * A script with a breakpoint inside of a branch command (If, While, etc) showing that we don't have an issue in that context.

   * A script that calls a GMAT function, with a breakpoint inside of the GMAT function.  We need to know if this works.  It night not, and that is okay at this stage, but we need to know if functions present special challenges.

This completes the proof of concept demonstration.  We'll use it to flesh out the actual "first viable product" requirements.

Build 2: Internal Demonstration System
--------------------------------------
At the start of this iteration the requirements for a first viable product will be finalized. The requirements can be found in :ref:`requirements`, broken into the first viable product feature set and a table listing additional features planned for later releases.

The proof of concept system above shows a basic work flow, but it requires that users insert a special pause point (the Breakpoint command) into their script.  A better approach is to mark the breakpoint on the command at which the user want to pause, and to enable running of a script with breakpoints set in either debug mode (stop on breakpoints) or run mode (ignore breakpoints).  

The proof of concept work also focused on functionality, possibly at the expense of usability.  This iteration of the system corrects that defect by making object inspection easier to achieve by using GUI resources available to the developer to make the debugger more generally intuitive to use.  An example of one possible implementation along these lines is the interface to the object inspection functions.  The proof of concept work does not specify how that piece is implemented, enabling some experimentation with the debugger user interface that will be defined in the system design at the start of this iteration of the system.

Upon completion of this phase of the debugger coding, the GMAT debugger will be interface complete, nearly or fully feature complete, and usable, while possibly still a bit buggy.

Build 3: Minimum Viable Product
-------------------------------
The third development cycle for the GMAT debugger runs the completed build 2 system through its paces, evaluating the quality of the system, identifying anomalies, correcting issues that need to be addressed, and completing documentation and testing to produce the initial release of the GMAT debugger.


.. _requirements:

Requirements
============
The GMAT Script Debugger meets the following requirements:

GMAT Debugger 1.0 Requirements
------------------------------
#. System Integration

   a. The GMAT GUI has a debugger for GMAT scripts
   b. The debugger enables examination of GMAT objects during a run

#. Debugger Activation

   a. The GMAT debugger can be turned on and off by the user

#. Breakpoints

   a. Users can set breakpoints on any GMAT command in the mission control sequence.
   b. Breakpoints are only enabled in debug mode.  When the debugger is turned off, any breakpoints that have been set are ignored.
   c. GMAT pauses execution when an enabled breakpoint is encountered, before its command is run.
   d. When GMAT pauses at a breakpoint, the user can take one of the following actions:

      #. The user can resume execution
      #. The user can inspect GMAT objects used in the run

#. Object Inspection

   a. When GMAT is paused at a breakpoint, the user can inspect any object defined in the user's script.
   b. The list of available objects is provided to the user in an easily navigable form.


Future Debugger Features
------------------------
The text above describes the minimum viable GMAT debugger.  This section collects additional features that, while not required for the minimum viable product, need to be considered in the design of the debugger.

#. Stepping

   a. Users can step forward in the mission control sequence from a pause caused by an encountered breakpoint or from a previous forward step.

#. Object Inspection
   
   a. Users can inspect objects inside of inspected objects.
