.. include:: <isonum.txt>

.. _APIFunctions:

Functions Used in the GMAT API
==============================

The GMAT API provides several functions that simplify GMAT use from Java and 
Python.  GMAT's user classes include member functions, called methods, that 
provide interfaces into GMAT objects that help simplify calls into the GMAT 
objects.  These features of the API are described in this chapter.  Script users
may also want to refer to the :ref:`ScriptUsage` chapter for information 
about functions tailored to running GMAT scripts through the API.

General API Functions
---------------------
API specific code in can be broken into three blocks: General API functions used 
to interact with the system, functions specific to driving GMAT using script 
files, and methods that are implemented on the GMAT classes to simplify object
interactions using the API.  

.. _GeneralPurposeFunctions:
.. table:: General Purpose Functions Controlling the API

   ================ ============================ ============ ============================================
   Function         Example                      Return Value Description
   ================ ============================ ============ ============================================
   Help             gmat.Help("MySat")           string       Returns help for the input item
   Setup            gmat.Setup("StartFile.txt")  void         Initializes the GMAT system with a custom
                                                              startup file
   Initialize       gmat.Initialize()            none         Sets up interconnections between objects and
                                                              reports on missing pieces
   ShowObjects      gmat.ShowObjects()           string       Lists the configured objects
   ShowClasses      gmat.ShowClasses("Burn")     string       Lists the classes available of a given type
   Construct        gmat.Construct(              object       Creates an instance of a class with a given 
                       "Spacecraft","Sat")                    name and adds it to the GMAT configuration
   Copy             gmat.Copy(sat, "Sat2")                    Creates a new object using the settings on 
                                                              an existing object
   GetObject        gmat.GetObject("Sat")        object       Retrieves an object from the configuration
   ================ ============================ ============ ============================================


GMAT Object Methods
-------------------
GMAT's user classes have been updated with member functions (or methods) that 
facilitate use for the objects by API users.  These methods are shown in 
:numref:`ObjectMethods`. 

.. _ObjectMethods:
.. table:: Methods added to GMAT objects for API users

   ================ ======================= ============ =================================================
   Function         Example                 Return Value Description
   ================ ======================= ============ =================================================
   Help             Sat.Help()              string       Retrieves help for an object
   SetField         Sat.SetField("X",0.0)   bool         Sets a field on an object
   GetField         Sat.GetField("X")       string       Retrieves the setting for a field as a string
   GetNumber        Sat.GetNumber("X")      double       Retrieves the setting for numerical field
   GetVector        Sat.GetVector("X")      vector       Retrieves the setting for Rvector field
   GetMatrix        Sat.GetMatrix("X")      matrix       Retrieves the setting for Rmatrix field
   ================ ======================= ============ =================================================
