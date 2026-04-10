=========================================
GMAT Code Updates
=========================================
The GMAT source code was written to be fast and to meet the needs of analysts
modeling spacecraft guidance, navigation and control.  The code includes 
functions used to set up mission simulations through a set of interfaces that 
require that users know the underlying data structures in the code.  API users 
do not necessarily have that knowledge, nor do they have a graphical user interface 
to guide them through setup for the components accessed using the API.  Several
code elements presented here are designed for integration into the base class for user 
components to meet these needs, adding functionality to the GmatBase class used
for simulation components in GMAT, to facilitate API usage.

SetField() and GetField()
-----------------------------------------
Parameter settings on GMAT objects are made through type specific methods with
names like SetStringParameter(), SetRealParameter(), etc.  API users can use 
those methods.  A second set of methods, SetField(), will be added to the code
to adapt user objects to less strongly typed interfaces.  The SetField() methods
are overloaded to adapt to multiple types of input data.  The method checks the 
type for the field that being set, and sets it accordingly.  

.. _FieldSetters:
.. table:: The Field Setting Methods, with Examples for a Spacecraft Named sat
   :widths: 30 25 45

   +----------------------------+-------------------------------------+-------------------------------------------------+
   | Method Signature           | Example                             | Parameter Description                           |
   +============================+=====================================+=================================================+
   | bool SetField(name, value) | sat.SetField("X", 7100.0)           | name is the field name used in GMAT scripting   |
   |                            |                                     |                                                 |
   |                            |                                     | value is the value that is set                  |
   +----------------------------+-------------------------------------+-------------------------------------------------+
   | bool SetField(id, value)   | *% Spacecraft.X has ID 13*          | id is the id of the field in GMAT's object      |
   |                            |                                     |                                                 |
   |                            | sat.SetField(13, 7100.0)            | value is the value that is set                  |
   +----------------------------+-------------------------------------+-------------------------------------------------+

The SetField() method takes either a field name or an integer ID value to 
identify the field to be set, and a string, integer, real number, or Boolean 
value as the new field setting.  On success, the call to SetField() returns true.  
If the value passed to the SetField() method is incompatible with the data 
structure that is being set, or if the field is not a supported field on the 
object, the method returns a false boolean value. 

Parameter settings on GMAT objects are accessed through type specific methods with
names like GetStringParameter(), GetRealParameter(), etc.  API users can use 
those methods.  A second set of methods, GetField(), have been added to the code
to adapt user objects to less strongly typed interfaces.  The GetField() methods
return a string containing the value of the field.  The API users then convert 
that string into the form needed for their code.

.. _FieldAccessors:
.. table:: The Field Access Methods, with Examples for a Spacecraft Named sat
   :widths: 25 30 45

   +----------------------------+-------------------------------------+-------------------------------------------------+
   | Method Signature           | Example                             | Parameter Description                           |
   +============================+=====================================+=================================================+
   | string GetField(name)      | sat.GetField("X")                   | *name* is the field name used in GMAT scripting |
   +----------------------------+-------------------------------------+-------------------------------------------------+
   | string GetField(id)        | sat.GetField(13)                    | *id* is the ID of the field in GMAT's object    |
   +----------------------------+-------------------------------------+-------------------------------------------------+
   | real GetNumber(name)       | sat.GetNumber("X")                  | *name* is the field name used in GMAT scripting |
   +----------------------------+-------------------------------------+-------------------------------------------------+
   | real GetNumber(id)         | sat.GetNumber(13)                   | *id* is the ID of the field in GMAT's object    |
   +----------------------------+-------------------------------------+-------------------------------------------------+

The GetField() method takes either a field name or an integer ID value to 
identify the field to be retrieved.  On success, the current field value is 
returned in a string.  If the field is not a supported field on the object, the
method returns an empty string. 

The GetNumber() method returns numerical data as a real number rather than as a 
string.  If the field is not a numerical field on the object, the method reports 
an empty exception that identifies the type of the field.

Object help
-----------------------------------------
Objects created in the API support a help function that is used to retrieve 
information about the settings available for the object, along with other 
information provided in the object's class.

API users retrieve help for created objects by calling the Help() method on the
object.  For example, a call to the Help() method for an ImpulsiveBurn object, 
IB, returns the following data *(Note: this is a mock-up of the data returned)*:

.. code-block:: python

   >>> IB = gmat.ImpulsiveBurn("Impulse1")
   >>> print(IB.Help())

   Impulse1 is an ImpulsiveBurn object with 9 Fields:

      CoordinateSystem     Type: Object   Value: Local
      Origin               Type: Object   Value: Earth;
      Axes                 Type: Enum     Value: VNB;
      Element1             Type: Real     Value: 25;
      Element2             Type: Real     Value: 0;
      Element3             Type: Real     Value: 0;
      DecrementMass        Type: Boolean  Value: false;
      Isp                  Type: Real     Value: 300;
      GravitationalAccel   Type: Real     Value: 9.81;

GMAT programmers may provide additional help for specific objects on a case by 
case basis by overriding the Help() method.  The API Help command calls into the 
object's Help() method when an object is passed to it, producing the same 
output.  In other words, the output from IB.Help() is the same as the output 
from gmat.Help(IB).

..
  Code Comment Consistency
  -----------------------------------------
  GMAT Code generates class by class documentation using 
  `Doxygen <http://www.doxygen.nl/>`_.  The generated documentation describes 
  every method and attribute for each of GMAT's classes, and acts as a detailed
  guide to the code when the developers have maintained the comments that generate
  the Doxygen product.  The comments in the GMAT code are incomplete, and provide
  inconsistent levels of detail.  The API implementation includes cleaning up of 
  the GMAT code comments for select components, so that the reference section of 
  this manual can link directly to the Doxygen output and present a consistent
  guide for heavily used components.

  The commentary in the GMAT code includes a nontrivial class description and a 
  list of public methods with all arguments defined.  Classes that are identified 
  as "API compliant" include comments that identify required and optional 
  interobject connections, and to show how those connections are made.  For select 
  classes, example API use code is also provided in the class comments.   

  **Placeholder: A Sample output will be added here once the API implementation 
  has solid representative output for a high usage class.**

  *Note: The Help() method may be able to tie into Doxygen output directly.  This
  is a TBD item in the design.*

=========================================
API Support Functions
=========================================

API users can work directly with GMAT objects, setting interobject 
connections and initializing objects to prepare them for use.  However, much of
the GMAT object model is built on composite objects, consisting of a core 
component that uses other GMAT components to jointly complete a task.  In many 
cases, it is easy to miss making connections that are made automatically in the 
GMAT engine, leading to confusion about the component usage.  The API includes 
helper functions that ease the burden of GMAT internal connections for users of 
typical components. 

:numref:`APIHelpFunctions` provides a list of functions supplied as part of the 
GMAT API that help work with GMAT components without the need for detailed 
understanding of the GMAT code base.  These functions work with the GMAT 
engine, hiding the engine functionality from the user while providing services 
that the user would otherwise need to code by hand with objects built with the 
API.  Each function is described briefly below.

.. _APIHelpFunctions:
.. table:: Helper Functions in the GMAT API
   :widths: 20 27 53

   ===============  ================  ==========================================================
   Function         Options           Description                                               
   ===============  ================  ==========================================================
   Initialize()                       Initializes GMAT objects and establishes object to object 
                                      connections.  This command is reentrant.  Subsequent calls
                                      reconnect objects and continue the process of preparing 
                                      the system for use.
   ---------------  ----------------  ----------------------------------------------------------
   Status()                           Returns a string reporting any known configuration issues.
                                      Users call this function if the Initialize() call reported
                                      an issue. 
   ---------------  ----------------  ----------------------------------------------------------
   Create()         ObjectType,       Creates an object of the input type, with the input name.
                    ObjectName        The created object is then managed inside of the API. 
   ---------------  ----------------  ----------------------------------------------------------
   ShowObjects()                      Lists all of the named objects that exist in the current
                                      run.
   ---------------  ----------------  ----------------------------------------------------------
   ShowClasses()    Category          Lists all of the creatable object types of the selected 
                                      category.  Example: ShowClasses("Propagator") lists all
                                      of the propagators available for creation.
   ---------------  ----------------  ----------------------------------------------------------
   Clear()          ObjectName        Clears the GMAT engine by deleting all of the user created
                                      components managed in the API.

                                      Selecting an object by name in the call to Clear() 
                                      results in deletion of that objects, leaving the other
                                      objects available for use. 
   ---------------  ----------------  ----------------------------------------------------------
   Help()           Topic             Shows the API help.  Top level help is displayed if no
                                      topic is selected.  The top level help includes a list of 
                                      topics for the system.  If the user enters a topic, the 
                                      corresponding help is displayed.
   ===============  ================  ==========================================================

.. _Initialization:

**Initialize**  The Initialize() function is used to initialize GMAT objects.  
Each call to Initialize() generates a pass through the GMAT objects constructed 
using the Create() command, setting the inter-object connections that can be set 
and tracking any issues that prevent component use.  The function returns true 
if the initialization succeeded, and false, along with a list of encountered 
issues, if there were problems.

**Status** The Status() method returns a list of issues found when the 
Initialize() method was called.  If there are no known issues, the call to 
Status reports the number of managed objects in the current run.

**Create**  The Create() function is used to create objects that are managed 
inside of the GMAT engine running under the API, including components exposed 
to the API from plugin libraries.  All named objects created using the Create() 
method are built through the GMAT Moderator and passed to the GMAT 
ConfigurationManager singleton.  These actions are invisible to the API user, 
and provide the mechanism used to manage objects in the API. 

**ShowObjects**  The ShowObjects() function is used to show a list of all 
objects managed in the API.  Note that objects created through direct calls to a 
class's constructor rather than through the Create() command are not managed in 
the API, and will not be part of the returned list.

**ShowClasses**  The ShowClasses() function is used to list all of the available 
classes of objects available for creation is an input category. 

**Clear**  The Clear() function is used to clear the configuration of 
created objects.  When an object name is included as a parameter in the 
function call, all other objects will remain available for use.

**Help**  The Help() function is used to get help from inside of the API.
