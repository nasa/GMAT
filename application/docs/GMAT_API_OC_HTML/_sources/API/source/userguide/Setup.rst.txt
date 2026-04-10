***********************
Setting up the GMAT API
***********************

The GMAT API is included in the GMAT release code beginning with the GMAT R2020a 
release of the system on SourceForge.  The API subsystem included in these 
packages has been tested using Python and MATLAB calls to the API.

Installation
============
Installation of the API is complete when the GMAT release bundle is installed on
the user's workstation.  

Direct Usage
------------
The API can be used immediately by users that want to run it from the GMAT bin 
directory.  Simply open a session in MATLAB or Python, change directories to the
GMAT bin folder, and load the GMAT system into the executing environment.  The
examples shown later in this document can then be run directly in that 
environment.

.. _RunningExternally:

Running the API Outside of the GMAT Folders
-------------------------------------------
Users that want to run the API from a different location from the GMAT system 
need to perform additional steps to configure the system to address two features 
of the system:  

#. GMAT uses a set of data files for its core functions.
#. Running environments need to be able to find the GMAT API interfaces.

The following paragraphs address the configuration settings for these two items.

.. _apistartupfile:

File Location Access
^^^^^^^^^^^^^^^^^^^^
GMAT uses a text data file, gmat_startup_file.txt, to identify and locate GMAT
plug-in components, planetary ephemerides, gravitational potentials, and a large
variety of other data files required during a run.  The GMAT API operates using 
this data file by default, but that causes problems when running outside of the 
GMAT folder structure.  Rather than change the GMAT startup file, API users can
accomplish the folder structure definition by creating an API specific startup 
file. A python scipt to create this file, BuildApiStartupFile.py is in the api 
folder in the main GMAT folder, as well as more detailed instructions in 
API_README.txt. 

Once the api_startup_file is created, you are ready to configure your Python
or MATLAB environment.

.. _PythonAnyFolder:

External Access from Python
^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. note::

   External access from Python requires configuration of an API startup file, as 
   described in the :ref:`apistartupfile` text, above.

The API is loaded from a Python process running outside of the GMAT folders 
using the load_gmat.py module found in the GMAT api folder.  The simplest way to
proceed is to edit that file in place:

#. Open load_gmat.py in a text editor.

#. Change the "<TopLevelGMATFolder>" entry near the top of the file to the 
   absolute path to your top level GMAT folder.  Make sure that the path is 
   enclosed in quotation marks.  

   Windows users will also need to change backslash characters in this string 
   either to double backslashes or to forward slashes so that the Python 
   interpreter can handle the path correctly. 

#. Save the file.

#. Copy the edited load_gmat.py file into the folder that is used for the API 
   run.

   **Note** one advantage of editing the load_gmat file in the api folder is 
   that this file can be copied into any folder that needs access to the API.  
   In other words, once the file has the absolute path set, it can be copied to 
   any folder that needs to act as the home folder for an API run.

Test the configuration to make certain that the API can be run.  Note that, 
rather than directly importing load_gmat, you will want to preserve the imported 
symbols.  This preservation is done using the syntax "from module import \*", as 
shown in this Linux example:

.. code-block:: shell

  $ cd APIFromHere/
  $ python3
  >>> from load_gmat import *
  >>> sat = gmat.Construct("Spacecraft","Sat")
  >>> gmat.ShowObjects()
  Current GMAT Objects

     EarthMJ2000Eq
     EarthMJ2000Ec
     EarthFixed
     EarthICRF
     SolarSystemBarycenter
     Sat

  The SolarSystem contains the following bodies:

     [Sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Pluto, Luna]

  >>> 

External Access from MATLAB
^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. note::

   External access from MATLAB requires configuration of an API startup file, as 
   described in the :ref:`apistartupfile` text, above.

The API is loaded from a MATLAB console running outside of the GMAT folders 
using the load_gmat.m module found in the GMAT bin folder.  That file configures
MATLAB to use the API.  The simplest way to use it is to add the GMAT bin folder 
to your MATLAB path, either in the running MATLAB environment or in the MATLAB 
configuration on your workstation.  You can test the configuration to make 
certain that the API can be run following the steps below, which run the API 
from the folder APIFromHere outside of the GMAT installation folders:

.. code-block:: shell

  $ cd APIFromHere/
  $ matlab -nodesktop
  
  To get started, type doc.
  For product information, visit www.mathworks.com.
   
  >> addpath('/<TopLevelGMATFolder>/bin/')
  >> load_gmat
  No script provided to load.

  ans =

  Instance of GMAT Moderator is initialized. No script ready to run.

  >> sat = GMATAPI.Construct('Spacecraft','MySat')

  sat =

  Object of type Spacecraft named MySat

  >> GMATAPI.ShowObjects()

  ans =

  Current GMAT Objects

     EarthMJ2000Eq
     EarthMJ2000Ec
     EarthFixed
     EarthICRF
     SolarSystemBarycenter
     MySat

  The SolarSystem contains the following bodies:

     [Sun, Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Pluto, Luna]


  >> 

Loading the API
===============

Packaging
---------

The GMAT API is packaged in a set of libraries encapsulating specific pieces of 
functionality.  

.. _Packaging:
.. table:: The component packages built for GMAT's API
  :widths: 12 20 68

  +---------------+--------------------+--------------------------------------+
  | Package Name  | Contents           | Description                          |
  +===============+====================+======================================+
  | gmat          | GMAT Utilities and | Utility classes used in the rest of  |
  |               | Core Components    | GMAT, that do not depend on other    |
  |               |                    | GMAT components, built from gmatutil |
  |               |                    | code.                                |
  |               |                    |                                      |
  |               |                    | Classes used for most of the GMAT    |
  |               |                    | spacecraft modeling, along with the  |
  |               |                    | GMAT "engine" that drives the main   |
  |               |                    | application, built from gmatbase     |
  |               |                    | code.                                |
  +---------------+--------------------+--------------------------------------+
  | station       | Ground station     | Code used to model ground stations   |
  |               | models             |                                      |
  +---------------+--------------------+--------------------------------------+
  | navigation    | Estimation         | Classes used for the GMAT orbit      |
  |               | Components         | determination subsystem.             |
  +---------------+--------------------+--------------------------------------+

Initialization
--------------
There are some differences in how to initialize the API between Python, Java,
and MATLAB, however they all follow the same basic process. The API is first
loaded into the interfacing language, and then the GMAT executive is initialized
through the Moderator. The process of initializing the API is detailed in the
sections below for each supported language. The initialization example for each
language creates an instance of the Moderator object named ``myMod`` that has
been initialized. The ``result`` variable is a flag which returns ``True`` if
the initialization was successful, or ``False`` if unsuccessful.

Python
^^^^^^

The API is loaded from the GMAT bin folder into Python with an import command

Python initialization example:

.. code-block:: python

  import gmatpy as gmat

At this point, the GMAT Python API is not initialized.  A user can initialize it 
using the Setup() command, or by making any call to an API specific function.

The API can be loaded into from any folder from Python if by following the 
:ref:`PythonAnyFolder` instructions, above.

Java
^^^^

Java requires an extra step compared to Python. Not only does the GMAT API need
to be imported, a shared library must also be loaded

.. code-block:: java

  import gmat.*;

  public class main {

      public static void main(String[] args) {

          Moderator myMod;
          boolean result;

          // Load the GMAT library
          System.loadLibrary("gmat"); // On Windows

          myMod = Moderator.Instance();
          result = myMod.Initialize("gmat_startup_file.txt");
      }
  }

At this point, the GMAT Java API is initialized.

MATLAB
^^^^^^

While the MATLAB interface uses the Java API, there are a few extra steps
required due to how Java packages are used inside MATLAB. The . To simplify this, a
MATLAB script which performs all the initialization, load_gmat.m, is included
with the GMAT installation in the *<install-dir>*/bin directory. The load_gmat
script also optionally takes as an input the filename of a GMAT script to load,
and a filename to a custom startup file.

.. code-block:: matlab

  [myMod, gmatStartupPath, result] = load_gmat("sample.script");

The extra steps in initializing the GMAT API through MATLAB is because every
GMAT library and JAR file needs to be loaded explicitly, and it needs to be
loaded by the Java instance inside MATLAB instead of by MATLAB itself. For more
details, see the comments inside the load_gmat.m file.

