Review Responses
================

The GMAT API was reviewed by interested parties on February 15, 2019.  Five
RFAs (Request for Action) reports are filed based on this review.  The design 
updates based on these RFAs are summarized here.

+----------+-----------------------------------------+------------------------+
| RFA ID   | Title                                   | Reporter               |
+==========+=========================================+========================+
| APIRFA01 | Unsafe C++ API                          | Joel Parker            |
+----------+-----------------------------------------+------------------------+
| APIRFA02 | Python version compatibility            | Joel Parker            |
+----------+-----------------------------------------+------------------------+
| APIRFA03 | Handling of persistent objects in GMAT  | Jacob Englander        |
|          | memory when using the API               |                        |
+----------+-----------------------------------------+------------------------+
| APIRFA04 | Complexity of "Create" Interface        | Steve Hughes           |
+----------+-----------------------------------------+------------------------+
| APIRFA05 | Rename and simplify the Setup() command | Steve Hughes           |
+----------+-----------------------------------------+------------------------+
| APIRFA06 | API Style Guide                         | Steve Hughes           |
+----------+-----------------------------------------+------------------------+

--------------------------------------------------------------------------------
API RFA 01 - Unsafe C++ API
--------------------------------------------------------------------------------

Requested action
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The target languages for the API were presented as including a C++ API, which is 
available "for free" since GMAT is written in C++ natively. But if this is 
intended for end users, the stable "public" API will be interspersed with the 
existing internal-only API, which may cause unintended consequences.

The team should look into the need for a public-facing C++ API, and if there is 
a straightforward way (e.g. in SWIG) to separate the public "safe" C++ API from 
the internal core version.

Supporting Rationale 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Users expecting a production-quality safe API in C++ may be confused or misled 
by the availability of unsafe, internal-only calls if the two are mixed. This 
could lead users to inadvertently using unsafe calls, or avoiding the C++ API 
altogether.

Response
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The "C++ API" for the design is a bit of a misnomer for two reasons.  First, 
SWIG is a tool that wraps C++ code for **other** languages.  As such, there is 
no API product for C++ users.  Second, the typical use case for the API is one 
that follows the procedure

* Open interactive session on the target platform
* Load GMAT into the active session
* Build objects (either by hand or by loading a platform specific script)
* Exercise the objects (either interactively or through platform specific 
  scripts)

C++ users will benefit from the new Create() and Initialize() functions provided
for API users.  These functions automate the object interconnections for GMAT
components, initially for those targeted by the API use cases, and eventually 
over a much broader range of classes.

For C++ users, there is no interactive environment like that supplied by Python
and MATLAB.  C++ works on a "compile and run" paradigm; as such, it does not 
supply a mechanism for the interactive use that makes API functions like the 
GMAT API's Help() functions useful.  C++ users can call that function, but they 
only see the output after compiling and running the compiled program, making the 
functionality limited at best.  Similarly for the other additions to GMAT for 
the API.  The functionality is generally available, but of limited use when 
working in GMAT's native C++ language.

The design document has been updated (see the note in 
:ref:`User Experiences <LessonsLearned>`), intended to clarify how C++ fits into 
the API.)


--------------------------------------------------------------------------------
API RFA 02 - Python version compatibility
--------------------------------------------------------------------------------

Requested action
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The reliance of the current GMAT API on a specific major.minor version of 
Python is not ideal, as it could lead to extensive configuration issues on the 
user side to match versions when using GMAT, alongside other tools that 
potentially have differing requirements. The team should investigate mitigations 
for this, and confirm that: a) this specific minor-version reliance is 
necessary, b) that the chosen version is the best one, and c) that there are no 
better options that could be selected to enhance ease of configuration. 
Investigating the approach of other Python APIs may help here.

Supporting Rationale 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The current GMAT API relies on a specific Python major.minor version as of the 
time of the API release. GMAT will not likely be the only tool being used on 
end-user machines that requires Python, so that specific reliance may cause 
problematic configuration issues. Further, GMAT will have a different release 
schedule from other tools, requiring reconfiguration on each upgrade to make 
sure things don’t get broken. It may result in less adoption of the GMAT API if 
setup is known to be difficult and fragile.

Response
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The GMAT Python interface relies on specific Python major/minor versioning.  The
SWIG documentation distinguishes between Python 2.x and Python 3.x for some 
features, but does not distinguish minor versions.  However, SWIG is generating 
C++ wrapper code which is then compiled and linked.  The issue comes with the 
link step: the shared library links to a specific Python library, which is then 
a dependency for its use.  More about Python binary compatibility can be found 
at https://docs.python.org/3.7/c-api/stable.html.

The API team did try building the current API using the Py_LIMITED_API option
discussed at the link.  That option is incompatible with the current release of
SWIG.

The development team will track this issue during implementation.  If a solution 
is found, it will be implemented and recommended for inclusion with GMAT's 
Python interface build.  If not, the Python linkage will be made to the same 
library as is used for the GMAT Python interface in order to avoid library 
conflicts.  

--------------------------------------------------------------------------------
API RFA 03 - Handling of persistent objects in GMAT memory when using the API
--------------------------------------------------------------------------------

Requested action
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Potential user actions that might cause memory leaks or other issues as objects 
to go out of scope between GMAT and user code should be explored and tested as 
necessary.

If I understand correctly from the review, when the user instantiates an object 
in their Python (or MATLAB or C++) workspace, GMAT creates that object and 
passes it by reference to the calling program. My question is, what happens if 
the user deletes that object in their calling program or if it just goes out of 
scope.

For instance, if the calling program does:

.. code-block:: python
   :caption: RFA 03 Example 1

   myThing = GMAT.createThing()
   myThing.doStuff()
   del myThing

The above code will result in the Python object myThing being deleted and 
therefore the reference to GMAT’s Thing is deleted. But what happens to the 
Thing that GMAT created dynamically? Is it deleted when Python’s reference to it 
is deleted? Or does it persist?

Similarly, suppose you are doing this in a loop

.. code-block:: python
   :caption: RFA 03 Example 2

   for (some range):
       myThing = GMAT.createThing()
       myThing.doStuff()

In the above code, myThing is an automatic variable that vanishes as soon as it 
goes out of scope. Since its scope is one particular iteration of the loop, 
myThing will be created and destroyed n times. What happens to the GMAT object 
on the other end of the myThing reference? Does it get deleted, too, or do we 
get n of them in memory? If the latter, is there any way to get rid of them or 
do we have to have the calling program close GMAT and re-open it?

Does the answer change if the calling program is in C++ vs Python or MATLAB? I 
know that I could create and destroy objects the "right" way by calling into 
GMATbase if I were an expert in the GMAT codebase, but when you create a clean 
C++ API then this issue could come up.

Supporting Rationale 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
I’m worried about creating a memory leak if the user creates and destroys many 
GMAT objects in the calling program.

Response
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Objects created by SWIG generated code are C++ objects contained inside of a 
target platform wrapper.  By default, these objects are managed using the 
platform's memory management facilities.  For example, if a constructor for a 
GMAT object is accessed directly from Java, the resulting object wrapper is 
managed in Java.  When the wrapper goes out of scope, the Java garbage collector 
can delete it.  When the wrapper is deleted, it calls the destructor of the 
contained object.  That call can be overridden using a SWIG setting that 
releases object management to the underlying C++ code on an object by object 
basis.  

The GMAT API will manage this setting inside of the Create() function.  Objects 
constructed using the Create() function will me managed inside of the GMAT 
configuration manager.  Repeated calls to Create() for the same named object 
will return the reference/pointer to the object created on the first call, as 
long as that object remains in the configuration.  Memory management for 
objects created using constructor calls outside of the Create() function are the 
responsibility of the platform making the call.  

As with all C/C++ programming, we will need to watch the memory management 
closely as work proceeds, and provide as much support as we can to making it 
transparent to the user when possible, and simple to understand when necessary.

--------------------------------------------------------------------------------
API RFA 04 - Complexity of "Create" Interface
--------------------------------------------------------------------------------

Requested action
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The examples for object initialization were relatively simple:

.. code-block:: python
   :caption: RFA 04 Example 1

   Create("Spacecraft", "EO1")
   Create("CoordinateSystem", "ECI", "Earth", "MJ2000Eq")

In general, there are a lot of special cases where certain data is required 
(for example, if the Coordinate system axes above were "ObjectReferenced" 
instead of "MJ2000Eq" additional information is required on the Primary and 
Secondary bodies.   

The design needs to clarify how required fields for object creation are handled, 
especially when the choice of one setting changes on a model dramatically 
changes what other information is required to configure that model such as when 
a Propagator Type is SPK instead of an integrator.  Additionally, 

The design also needs to clarify how required vs. optional information will be 
handled at object creation.  For example, Spacecraft has numerous settings.  For 
complex objects what will be required at construction, and what will be set via 
"Set()" methods.

Supporting Rationale 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The API will be quite difficult to use, and possibly error prone, if the 
complexity of object initialization is not well designed and well documented.  

Response
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Object creation is a straigtforward call to the constructor for the object.  The
Create command is used to make constructor calls and place object management 
under control of the GMAT engine by placing the objects into the configuration
database.  (See the description of memory management for RFA 3, above).

The concern here is more about object initialization, performed by the Setup() 
command in the draft document, now renamed Initialize() (see RFA 5, below).  As 
stated in the :ref:`description of that command<Initialization>`, the object to 
object interconnections are set when Initialize() is called prior to object use.
Missing elements are identified in the return from that call.

As the reviewer notes, there is a lot of complexity in some of the GMAT objects. 
Part of the challenge of coding the changes needed for the API is covering that 
complexity.  The development team will concentrate efforts on making high usage
objects as robust as possible in this regard, with a focus on clear 
configuration messages made to the users for the objects used in the use cases.  

While we will make our best efforts to address the object complexity issues 
during the initial year of development, we expect that we will revisit this 
issue once use case 1 has been implemented, in hopes of refining the strategy 
that addresses it at a larger scope than a case by case approach.

--------------------------------------------------------------------------------
API RFA 05 - Rename and simplify the Setup() command
--------------------------------------------------------------------------------

Requested action
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The Setup()/initialization process is confusing and possible dangerous because 
omitting the call may result in an undesirable state.   The code example in the 
review was:

.. code-block:: python
   :caption: RFA 05 Example 1

   import gmat_py as gmat
   gmat.Setup("MyCustomStartupFile.txt")
   csConverter = gmat.CoordinateConverter()
   eci  = gmat.Construct("CoordinateSystem", "ECI", "Earth", "MJ2000Eq")
   ecef = gmat.Construct("CoordinateSystem", "ECEF", "Earth", "BodyFixed")
   gmat.Setup()
   csConverter.Convert(mjd, rvIn, eci, rvOut, ecef)

During the review, it sounded like it is relatively easy to eliminate the first 
call to setup by modifying the import line to contain startup file 
configuration.

The second call confused a lot of people, and there is concern that forgetting 
to call SetUp() could result in an issue in the configuration that is not 
obvious to the user.   The main confusion is that typically in an API, once an 
object is constructed, it is ready for use.    That is not the case in the 
current design.  It appears the SetUp() call is similar to BeginMissionSequence, 
and is required due to the existing GMAT design.  If that is not correct, 
consider designs that do not require a Setup() call.  If the existing design of 
GMAT requires such a call, consider using a more descriptive set of function 
names and provide a way to create and initialize new objects during execution.   
For example

.. code-block:: python
   :caption: RFA 05 Example 1

   import gmat_py as gmat
   csConverter = gmat.CoordinateConverter()
   eci  = gmat.Construct("CoordinateSystem", "ECI", "Earth", "MJ2000Eq")
   
   gmat.InitializeObjects()

   csConverter.Convert(mjd, rvIn, eci, rvOut, ecef)
   // Based on previous execution, determined we need a new model, so create 
   // during execution and use. Need to initialize that object but leave the 
   // state of the rest of the objects alone. 
   ecef = gmat.Construct("CoordinateSystem", "ECEF", "Earth", "BodyFixed")

   // only initialize ecef, other objects are not re-initialized
   gmat.InitializeObjects(ecef) 

Finally, if an object has not been initialized before use, the API must provide 
that information back to the user (probably as an exception).

Supporting Rationale 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The single biggest concern for users/reviewers was the call to SetUp().   
Existing GMAT design may require a Setup()-like call, but if that is the case, 
care must be taken in the design and documentation should be clear, and user 
errors need to be trapped to avoid un-intended non-obvious failure modes. 

Response
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The originally proposed Setup() command has been renamed Initialize().  Based on 
the work one team member performed after the original design document was 
written, we agree that we can incorporate the core GMAT initialization when we 
load the system into the target platform's environment, so you'll see that the 
examples in the design document no longer include that first call.

The purpose of the Initialize() call is to establish object-to-object 
interconnections, and to validate that the objects are ready for use.  This is 
different from the BeginMissionSequence behavior in GMAT, though.  GMAT has 
a requirement when running a script that all of the scripted components be set
and that their references also be connected.  In GMAT, this occurs after objects
are loaded into the GMAT Sandbox.  That step is performed, in the API, using 
the Initialize() command.

One result of initialization here is the generation of a list of connections 
that were needed but not found.  We may present this as an exception, but may 
present it in a more user friendly format.


--------------------------------------------------------------------------------
API RFA 06 - API Style Guide
--------------------------------------------------------------------------------

Requested action
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Develop and maintain an API style guide as development progresses.

Supporting Rationale 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Low-level models and functions in GMAT do not always have a consistent interface 
style between them.  We should ensure as we expose models via the API, that the 
interface style and “wrappers” are consistent between models to make the API 
easy to use. 

Response
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
The development team will include a style guide in the user documentation.  A 
placeholder for it has been added to the User's Guide table of contents. 
