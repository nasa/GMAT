.. include:: <isonum.txt>

Object Usage with the GMAT API
==============================

The goal of this section is to help you start using the GMAT API quickly, while
introducing features of the interface in a natural progression of steps needed 
to solve a simple orbital state conversion problem.  

Overview
========
The GMAT API is built using the Simplified Wrapper and Interface Generator, 
SWIG.  SWIG connects code written in C and C++ with a variety of high level 
languages.  The production GMAT API provides Python and Java connections for
GMAT functionality.  C++ programmers can also use the API specific code 
interfaces through direct calls into the GMAT libraries.  The following sections
describe the provided interfaces.  Following that, three levels of interface 
usage are described:

#. Usage based primarily on GMAT scripting, with API generated component changes.
#. High level usage of GMAT components to meet specific user needs.
#. Class and object level access to GMAT components for expert users.


GMAT API Interfaces
^^^^^^^^^^^^^^^^^^^
The production GMAT API is built with Python and Java wrappers.  The Java 
wrappers are used to also provide the MATLAB interface into the API.


The Python Interface
""""""""""""""""""""
The Python wrappers are identified by the suffix "_py."  Python imports are 
provided as .py files, which connect to associated shared library files.  The 
Python API is packaged in the gmatpy folder contained in the GMAT bin folder. 
The API is loaded by importing that folder into the Python environment.


The Java and MATLAB Interface
"""""""""""""""""""""""""""""
The Java interface is provided in Java Archive (jar) files and associated 
libraries.  These files include the GMAT base code API interfaces and the 
interfaces to the components used in the navigation subsystem, tabulated below.

.. table:: Java archives for the GMAT API

   +----------------+------------------+--------------------------------+
   | Java Wrapper   | Python Wrapper   | Contents                       |
   +================+==================+================================+
   | gmat.jar       | gmat_py.py       | GMAT Base and utility code     |
   +----------------+------------------+--------------------------------+
   | station.jar    | station_py.py    | Groundstation components       |
   +----------------+------------------+--------------------------------+
   | navigation.jar | navigation_py.py | Orbit determination components |
   +----------------+------------------+--------------------------------+

MATLAB users load the GMAT API by calling the load_gmat.m MATLAB script in the 
GMAT bin folder.

Interface Complexity
^^^^^^^^^^^^^^^^^^^^
The GMAT API is designed for three different styles of usage: Users working with 
configurations based on GMAT scripting, users that work with GMAT objects 
through API calls, and users that work at a low level with GMAT objects 
directly.

GMAT Script Drivers
"""""""""""""""""""
One use for the GMAT API is to act as a front end for GMAT mission runs.  In 
this context, the user starts a runtime environment for the controlling 
language (e.g. a Python session or MATLAB), loads and initializes GMAT using the 
API, and then loads a GMAT script into the running environment.  At this point 
the user might manipulate setting on the GMAT objects used in the script to
tailor the run.  Once the configuration is ready, the API is used to run the 
script.

More details can be found in the :ref:`ScriptUsage` chapter.

High Level Access
"""""""""""""""""
A second use of the GMAT API is as a tool for using intermediate level GMAT 
objects to model portions of an analysis problem, and to feed the modeled 
results back to the driving system for further analysis.  When used this way, 
the GMAT API provides proven and tested components used to meet the user's needs
for building blocks for a problem running outside of GMAT.  Examples of this 
type of usage include

* Converting state data from one coordinate system into another
* Accessing the GMAT force models for accelerations and Jacobians of a given
  state.
* Accessing the Navigation measurement models for retrieve calculated
  measurement values.

The walk-through provided in the :ref:`UseCase1` chapter covers the techniques 
needed for high level object access and usage.

Component Level Access
""""""""""""""""""""""
Some API users need access to the details of GMAT's components in order to 
control them at a fine-grained level during use, to extend them with new 
computed data during use, or to monitor and report their state during use.  
These users may want to configure the objects by hand, and may want to 
manipulate the objects differently from the ways anticipated by the GMAT 
developers.  The GMAT API allows for this level of access to GMAT's components.  
The :ref:`Doxygen<Doxygen>` generated object level documentation provides a 
guide to this type of usage.
