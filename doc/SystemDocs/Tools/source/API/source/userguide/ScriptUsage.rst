.. _ScriptUsage:

************
Script Usage
************

The GMAT API can be used as a front end for driving the GMAT application in a 
"headless" mode.  You might want to do this to run GMAT remotely, to script 
product generation, or to perform a large scale run like a Monte-Carlo run or
a scan through a set of parameters.  This section introduces the API features 
that make these processes possible.

API Functions for Script Users
==============================
The GMAT API includes five functions, shown in :numref:`ScriptDrivers` 
specifically designed for script based usage.  

.. _ScriptDrivers:
.. table:: Functions Used to Run Scripts in the API
   :widths: 18 20 15 47

   ================ ======================== ============ ============================================
   Function         Example                  Return Value Description
   ================ ======================== ============ ============================================
   LoadScript       LoadScript("script")     bool         Loads a script into the GMAT system
   RunScript        RunScript()              bool         Runs a loaded script
   SaveScript       SaveScript("script")     bool         Saves the configured objects to a file
   GetRuntimeObject GetRuntimeObject ("Sat") GmatBase*    Retrieves an object from a GMAT run
   GetRunSummary    GetRunSummary()          String       Retrieves a listing of the spacecraft data
                                                          from a script run for each command in the 
                                                          script
   ================ ======================== ============ ============================================

Background: The GMAT Run Script Process
---------------------------------------
In GMAT, when a user runs a script three steps are taken:

#. The script is read and GMAT objects are created that match the objects 
   described by the script.
   
   * GMAT Resources are stored in the GMAT Configuration.
   * GMAT Commands are connected together to create the Mission Control Sequence.
#. The objects from the script are copied ("cloned") into a memory location, the 
   Sandbox, used for the run.  

   * This creates a new set of objects used for the run.
   * After cloning, these run time objects are connected together as needed and 
     initialized.  
#. The Mission Control Sequence is executed sequentially, starting from the first
   command in the sequence.  Command execution manipulates the run time objects 
   to simulate the scripted mission.

API users drive this process using the functions in :numref:`ScriptDrivers`.  

Driving a Script From the API
-----------------------------
API users take the following steps to execute a script:

#. Start the API environment (e.g. Python or MATLAB) from the GMAT bin directory
   or start the application and change the current directory to the GMAT bin 
   directory.
#. Load GMAT into the environment.  The MATLAB implementation includes a 
   scripted function for this process.
   
   * **Python**: import gmatpy
   * **MATLAB**: load_gmat()
#. Read a script into the loaded GMAT engine.
   
   * **Python**: gmat.LoadScript(*script_path_and_name*)
   * **MATLAB**: GMATAPI.LoadScript(*script_path_and_name*)
#. Run the script.  This step performs the run time object cloning and 
   initialization and then executes the Mission Control Sequence.
   
   * **Python**: gmat.RunScript()
   * **MATLAB**: GMATAPI.RunScript()


Examples
========
The sections above describe in general terms how to run GMAT scripts from the
API.  The following sample usage shows these features in Python and MATLAB.

Example: Running a Sample Mission
---------------------------------
The first example shows how to run a sample mission from the API and retrieve 
data generated from the run.  The example runs the sample mission 
Ex_GEOTransfer.script in the GMAT samples folder, then accesses the targeted 
maneuvers from the script and computes the total delta-V needed for the run.

Python
^^^^^^
Use of the API in Python is performed through direct calls to the functions 
described above.

.. code-block:: shell
   :caption: Sample Run: Calculating the Delta-V for the GEO Transfer, Run in Python
   :linenos:

   $ python3
   Python 3.6.7 (default, Oct 22 2018, 11:32:17) 
   [GCC 8.2.0] on linux
   Type "help", "copyright", "credits" or "license" for more information.
   >>> import gmatpy as gmat
   >>> gmat.LoadScript("../samples/Ex_GEOTransfer.script")
   True
   >>> gmat.RunScript()
   True
   >>> TOI = gmat.GetRuntimeObject("TOI")
   >>> MCC = gmat.GetRuntimeObject("MCC")
   >>> MOI = gmat.GetRuntimeObject("MOI")
   >>> toidv = float(TOI.GetField("Element1"))
   >>> mccdv = (float(MCC.GetField("Element1"))**2+float(MCC.GetField("Element2"))**2)**0.5
   >>> moidv = float(MOI.GetField("Element1"))
   >>> deltaV = abs(toidv)+mccdv+abs(moidv)
   >>> print("Total Delta-V Cost: ", deltaV, " km/s")
   Total Delta-V Cost:  4.394839062410714  km/s
   >>> exit()


MATLAB
^^^^^^
Loading the GMAT API in MATLAB is a moderately complicated procedure, so the API 
developers have wrapped the load process in a MATLAB function, load_gmat.m.  The
function takes two optional arguments: the name of a script, and the startup 
file used to initialize GMAT.  The example shown below leaves both inputs blank
so that it closely matches the Python example, above.  

.. code-block:: matlab
   :caption: Sample Run: Calculating the Delta-V for the GEO Transfer, Run in MATLAB
   :linenos:

   >> load_gmat()
   Initialize Moderator Status: 1
   No script provided to load.

   ans =

   Instance of GMAT Moderator is initialized. No script ready to run.

   >> GMATAPI.LoadScript("../samples/Ex_GEOTransfer.script")

   ans =

     logical

      1

   >> GMATAPI.RunScript()

   ans =

     logical

      1

   >> TOI = GMATAPI.GetRuntimeObject("TOI");
   >> MCC = GMATAPI.GetRuntimeObject("MCC");
   >> MOI = GMATAPI.GetRuntimeObject("MOI");
   >> toidv = str2num(TOI.GetField("Element1"));
   >> mccdv = sqrt(str2num(MCC.GetField("Element1"))^2+str2num(MCC.GetField("Element2"))^2);
   >> moidv = str2num(MOI.GetField("Element1"));
   >> DeltaV = abs(toidv)+mccdv+abs(moidv)

   DeltaV =

      4.394839062410714

   >> exit

..
   Example: Performing a Parameter Scan
   ------------------------------------
   The first example, shown above, demonstrates interactive use of the GMAT API.
   This example scripts the API calls in order to run the same script multiple 
   times, scanning through parameter settings and looking for a trajectory from 
   a low-Earth orbit initial trajectory to a flyby of the Moon.

   Python
   ^^^^^^


   MATLAB
   ^^^^^^