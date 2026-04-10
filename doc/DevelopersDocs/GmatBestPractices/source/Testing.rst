.. _Testing:

*****************
Developer Testing
*****************
The GMAT test system consists of two pieces: a GUI test system, driven by SmartBear's TestComplete product, and a custom script based test system run using MATLAB.  Developers focus on the latter system.

GUI Test System Considerations
==============================
The GUI test system is only run on Windows.  When a developer makes a change the affects the GMAT GUI, they notify the GUI test analyst that there are GUI changes so that the analyst knows to watch for changes in the automated tests.

Script Test System Considerations
=================================
The script test system is described at `Script Testing <https://gmat.atlassian.net/wiki/spaces/GW/pages/380273291/Script+Testing>`_ on the GMAT Wiki.  Developers use this system to test new code features, perform regression tests in GMAT subsystems, and to sanity check code changes before making them available to other developers.  

Configuration of the script test system
---------------------------------------
The script test system is stored in a subversion repository behind the GSFC firewall, currently located at 

   https://gs580s-svn.gsfc.nasa.gov/svn/GMAT

Access to this repository is outside of the scope of this document - contact the GMAT project lead at GSFC for this access.  Once access is provided, the repository should be checked out for local use:

   svn checkout https://gs580s-svn.gsfc.nasa.gov/svn/GMAT

The repository is large, so users may want to limit the checkout to the test/script subfolder of the repository.  

Once the checkout is complete, setup can proceed.  The test system is run from the test/script/bin folder of the repository, and is run on a Release build of GMAT, installed external to the test system folders.  You'll need that GMAT location to proceed.

The test system uses data files and other input settings for GMAT that may differ from the files in the GMAT code repository.  The files used for GMAT testing are copied to the install folder using the MATLAB script "preparegmat.m."  System setup requires the developer to open MATLAB, and run this script, passing in the path to their GMAT installation.  For example, if GMAT was installed in the "/home/gmatbuilds/DailyBuild" folder, the MATLAB command, run from the test system's bin folder, is

   >> preparegmat('/home/gmatbuilds/DailyBuild')

This command copies files into place, updates the GMAT startup file for test runs, and updates the MATLAB path so taht the test system components can be found.

Each test run is configured using a "rundef" structure that defines the tests to be run, the location of the GMAT executable, and the location used for output files for the run.  A simple example of a rundef file is the files used for sanity checking the system, described next.

Smoke Tests
-----------
The "sanity check" tests are performed by running a subset of the test cases, called the GMAT "smoke tests."  The repository includes a driver file for the smoke tests on Windows named "RunSmokeTests.m," shown in :numref:`SmokeTestRundef`.  The key setting to check in this file is the GMAT location, defined by the RunDef.GmatExe setting in the file.  Point this location to your GMAT install folder.  The Cases, Categories, and Folders settings identify the tests to be run.  In the listing, we are running the smoke tests, identified by the category setting "Smoke."

.. _SmokeTestRundef:
.. code-block:: matlab
   :caption: The smoke test driver (Windows)
   :linenos:

   clear RunDef;
   addpath(fullfile(fileparts(mfilename('fullpath')), 'contrib'));

   %% Name and location of your application  (executable) file.
   RunDef.GmatExe      = 'C:\GMAT\gmat\GMAT-R2020a-Windows-x64\bin\GMAT.exe';

   %% Delete old SmokeRun folder
   outputDir = GetFullPath(fullfile('..', 'output', 'SmokeRun'), 'fat');
   if exist(outputDir, 'dir')
       rmdir(outputDir, 's');
   end

   %% Test config for smoke tests
   RunDef.Build        = 'SmokeRun';
   RunDef.Modes        = {'script'};
   RunDef.Comparisons  = {'truth'};
   RunDef.Reporters    = {'ScreenReporter'};
   RunDef.Cases        = {};
   RunDef.Categories   = {'Smoke'};
   RunDef.Folders      = {};
   RunDef.Requirements = {};
   RunDef.FilterMode   = 'or';
   gmattest(RunDef)

Once the run configuration is defined, the tests can be run.  The basic process is

#. Open MATLAB in the test system's test/script/bin folder (or change the current MATLAB directory to that location).
#. Configure MATLAB and GMAT as needed:
   * Run preparegmat if this is a fresh build, so that the test system data is placed into the GMAT folders.
   * Run "setup" if this is a new MATLAB session running a previously prepared GMAT installation.
#. Run the smoke test script.

:numref:`ExampleSmokeRun` shows, in abbreviated form, a sample smoke test run on Linux.


.. _ExampleSmokeRun:
.. code-block:: matlab
   :caption: The smoke test driver (Windows)
   :linenos:

   >> preparegmat('~/gsfcgmat/TestTarget/GMAT-R2020a-Linux-x64')         
   Modifying startup file ~/gsfcgmat/TestTarget/GMAT-R2020a-Linux-x64/bin/gmat_startup_file.txt... done
   Copying test files from /home/djc/jazz/test/script/gmatdata to ~/gsfcgmat/TestTarget/GMAT-R2020a-Linux-x64/data... done
   Copying test files from /home/djc/jazz/test/script/input/Functions/MATLAB/ForUserFunctions to ~/gsfcgmat/TestTarget/GMAT-R2020a-Linux-x64/userfunctions/matlab... done
   Copying test files from /home/djc/jazz/test/script/input/Functions/Python/ForUserFunctions to ~/gsfcgmat/TestTarget/GMAT-R2020a-Linux-x64/userfunctions/python... done
   >> RunSmokeTestsLinux

   RunDef = 

     struct with fields:

       GmatExe: '~/gsfcgmat/TestTarget/GMAT-R2020a-Linux-x64/bin/GmatConsole'
   ...
   -----------------------------------------------------------------
   Summary
   -----------------------------------------------------------------
   System configuration: 
   Tested build: SmokeRun
   Regression build: SmokeRun

   Total number of scripts: 84 (+0 deferred)
   Percent Passed: 98.81%
   New issues: 0
      Output changes: 0
      New truth failures: 0
      New crashes: 0
      New script errors: 0
      New comparator errors: 0
   Fixed issues: 0
   Total issues: 1

   Test system warnings: 1
   New deferred issues: 0
   Fixed deferred issues: 0
   Total deferred issues: 0

   Run Time: 00 hours, 04 minutes, 25 seconds

   -----------------------------------------------------------------
   ...
   -----------------------------------------------------------------
   All Known Issues
   -----------------------------------------------------------------
   Crashes/hangs/etc.:
      None

   Script errors:
      Vary_optimizer_fmincon__InitialValue_Variable:
         ERROR MESSAGE: Output file ~/gsfcgmat/TestTarget/GMAT-R2020a-Linux-x64/output/Vary_optimizer_fmincon__InitialValue_Variable.report does not exist

   Comparator errors:
      None

   Truth failures:
      None

   -----------------------------------------------------------------
   Test System Warnings
   -----------------------------------------------------------------
   Passing tests tagged with bugs:
      (To fix, remove bug ID from Bugs key in .tc file)
      EphemStressTests_Default_Prop_STK_60Sec_MixedStopTypes_LogicTypes_DiscontinuityTypes_SolverTypes_ForwardDirect [GMT-6090]
   Running TestHelper...
   >> 

The key items to check are the passing percentage and the list of any test case failures from the run.  The passing percentage on Windows should be 100%.  On Mac or Linux, one test case in the smoke tests is expected to fail, because the fmincon optimizer plugin is not available on those platforms.
