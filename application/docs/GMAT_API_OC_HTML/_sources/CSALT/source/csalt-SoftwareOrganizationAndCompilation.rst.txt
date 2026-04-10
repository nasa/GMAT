.. _Sec_GMATOC_Organization:

Software Organization and Compilation
======================================

CSALT code and documentation is stored in the main public GMAT repository.   Compilation instructions for building CSALT as a stand-alone library, or as a GMAT plugin, are contained in the GMAT build instructions located on gmatcentral.org (search for CMAKE Build System). 

Classes and utilities that a user must modify or use to solve optimal control problems are located in the src/csalt/src/userfunutils directory.  The doxygen reference material for low-level/executive components that a user need not modify or use are documented separately in the collutils, util, and executive folders, among others.

The CSALT software is organized as shown below:

* docs

  * benchmarking
  * user guide and system description (snapshots of this document)
  
* src/csalt/src

  * userfunutils
  * collutils
  * utils
  * executive
  * include
  
* src/csaltTester/src

  * directories for optimal control unit tests
  * TestOptCtrl directory, which holds example problems

An extensive set of optimal control problem examples is included in the subdirectories of src/csaltTester/src/TestOptCtrl. The example problem driver in src/csaltTester/src/TestOptCtrl/src/TestOptCtrl.cpp allows the user to run all or selected example problems via a command-line interface. Test problem source code drivers are located in the TestOptCtrl/src/drivers folder, and path and point function source code is located in the TestOptCtrl/src/pointpath folder.

.. _Sec_GMATOC_Installation:

Installation
^^^^^^^^^^^^^^

Installation of CSALT and GMAT Optimal Control is complicated by the fact that CSALT relies on several software packages that are not currently bundled with the GMAT installation. As a result, a user must manually place several libraries in specific locations in order to execute CSALT and/or GMAT Optimal Control. The libraries are:

* Sparse Nonlinear OPTimizer (SNOPT) version 7.5 (all platforms)
* Fortran compiler (if compiling SNOPT) and/or Fortran redistributable libraries (if using a pre-compiled SNOPT) 

Windows
"""""""""""""""

The SNOPT libraries -- snopt7.dll and snopt7\_cpp.dll -- must be placed in the GMAT bin directory (the same directory in which the GMAT executable is located).


Mac
""""""""

GMAT provides a configuration file for users to set up MATLAB, PYTHON, SNOPT, and GFORTRAN locations for use with GMAT.  This file is named MacConfigure.txt (previously MATLABConfigure.txt) and is located in the bin folder. To set up snopt7 and gfortran -- which are needed if and only if you are using the CSALTPlugin:

* Open the MacConfigure.txt in the bin directory and edit the SNOPT_LIB_PATH field to point to the location of your snopt7 dynamic libraries (snopt7.dylib and snopt7\_cpp.dylib).
* Edit MacConfigure.txt to point GFORTRAN_LIB_PATH to your gfortran dynamic libraries.

If the CSALT scripts do not run with the GmatConsole command line application, you may need to set up your Terminal so that the system can load the snopt7 and gfortran libraries.  For example, if you are using a .bashrc file, you may need to add something like this:

* export SNOPT7 = <path/to/snopt7/libraries/location/>
* export FORTRAN = <path/to/fortran/libraries/location/>
* export DYLD_LIBRARY_PATH=$FORTRAN:$SNOPT7:$DYLD_LIBRARY_PATH

Linux
""""""""

The SNOPT libraries -- snopt7.dll and snopt7\_cpp.dll -- must be placed in the GMAT lib directory (a directory at the same level as the bin directory containing the GMAT executable).

The SNOPT libraries are built using a Fortran compiler.  On Linux, the most likely compiler is the GCC Fortran compiler, gfortran.  The compiled SNOPT libraries need access to the corresponding Fortran shared library (e.g. libgfortran.so.4).  If that library is not installed on your workstation, place the library in the GMAT lib folder alongside the SNOPT libraries.  You may also need to set the load library path (LD_LIBRARY_PATH) if your snopt libraries were not compiled with a run path setting.
