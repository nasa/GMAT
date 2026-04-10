API Best Practices
==================

General Practices
-----------------
API users work more closely with the core GMAT code than users that run GMAT
through the console for GUI applications.  This feature of the API system adds
responsibility for understanding how the system manages objects to the list of 
items an API user must consider.  The following items capture some of the 
lessons we have learned from using the API.

Understand Object Ownership
***************************
The API provides a function, Construct(), that builds GMAT objects and retains
object ownership in the GMAT module.  Objects created using Construct() remain 
GMAT's responsibility for management.  Sometimes, API users may need to create
objects directly by calling the object's constructor, like this:

.. code-block:: python
   :caption: Creating a propagation state manager in Python

   psm = gmat.PropagationStateManager()

or, in MATLAB:

.. code-block:: matlab
   :caption: Creating a propagation state manager in MATLAB

   psm = gmat.gmat.PropagationStateManager()

The objects created this way are managed on the client side of the interface: in 
either Python or the MATLAB Java systems.  The garbage collectors on the client 
side will delete the underlying objects when it determines that the object is no 
longer needed.  This can cause memory management issues for objects that are 
passed to other GMAT objects.  The API code provides a mechanism to assign 
ownership to the component that needs it, using the setSwigOwnership() method in
Java code:

.. code-block:: matlab
   :caption: Managing ownership for a propagation state manager in MATLAB

   % Create the state manager
   psm = gmat.PropagationStateManager();

   % Hand the manager to a force model, and assign ownership to the GMAT object
   fm.SetPropStateManager(psm);
   psm.setSwigOwnership(false()); 

or the thisown setting in Python:

.. code-block:: python
   :caption: Managing ownership for a propagation state manager in Python

   % Create the state manager
   psm = gmat.PropagationStateManager();

   % Hand the manager to a force model, and assign ownership to the GMAT object
   fm.SetPropStateManager(psm);
   psm.thisown = False

For either of these mechanisms, a false setting indicates that the client does 
not own the object.

Java and MATLAB Best Practices
------------------------------

* Adding the bin folder to your MATLAB path allows you to run the GMAT API from
  any other working directory

* Use the GMATAPI MATLAB class contained in the bin folder when using the API
  helper functions Construct(), Copy(), GetObject(), or GetRuntimeObject().
  These functions in the GMATAPI MATLAB class will automatically perform
  class casting, so the object returned is the more specific type instead of
  just being of type GmatBase. The GMATAPI MATLAB class also contains a
  SetClass() function which will also automatically perform the class casting
  on any GmatBase object provided.

Python Best Practices
---------------------

* The import function loads GMAT by loading all of the libraries in the
  gmatpy folder (gmat, station, etc).  These libraries can be imported 
  separately if you do not need all of the API functions in your application.

* The import can rename the interface calls for user convenience.  In this
  document we often load the engine using

     import gmatpy as gmat

* The GMAT startup file is loaded the first time a GMAT API function is 
  called.  Users that want to use a startup file that is different from 
  the default file, gmat_startup_file.txt, can load their file using the 
  Setup(*path_and_startup_file_name*) function call.

  This approach is used for running the API from folders outside of the GMAT bin
  folder, as described in :ref:`RunningExternally`.
