=========================================
The API Design
=========================================

:numref:`GmatStack` shows an overview of the GMAT component stack.  The stack 
for the GMAT API, shown in :numref:`APIStack`, has a similar appearance.  Users 
interact with the GMAT API through an interface layer built using the Simplified 
Wrapper and Interface Generator, SWIG.  SWIG generates interfaces and shared 
libraries for Python and Java, and can generate similar interface code for other 
languages when needed.  Classes in GMAT's code base are exposed through this 
interface using language specific wrappers.  Users interact with the GMAT 
classes through these wrappers.

.. _APIStack:
.. figure:: ../../../images/API/GMATAPIStack.png
   :scale: 70
   :align: center

   The GMAT API stack.

Using the SWIG interface code, users can work directly with GMAT classes on a
class by class/object by object level.  Users that work this way need a pretty 
complete understanding of object linkages and interactions in GMAT.  Using that 
expertise, they either imitate many of the steps that are performed by the GMAT 
engine when GMAT is run or make calls to the components of the engine to perform
the required actions.  

Most users would rather work at a less detailed level than this object by 
object interaction.  There are two groups of users in this category: those that are
familiar with GMAT and want to use the API to run GMAT scripts, making API calls 
to adapt their scripts along the way, and those that want to use capabilities
provided by GMAT inside of models that they are running in a tool like MATLAB or 
Python, or in a compiled application written in a language like Java.  The API 
provides a set of helper functions that encapsulate the GMAT engine behind calls 
that simplify the management tasks of the GMAT engine for these users.  These 
API helpers are exposed through the SWIG interface layer for use by these API 
users.

A driving feature of the GMAT API is the incorporation of usability features for
the API user community.  During the prototyping exercise for the API, the 
development team found that the SWIG system provides a simple mechanism for 
exposing GMAT components in the Python, Java, and MATLAB environments.  However,
users working in those systems still found it difficult to use the prototype API
because of a lack of on line documentation and apparent inconsistencies in the
methods in GMAT.  The production API addresses the first of these issues through
the incorporation of class and object level help functions for classes that are
identified as "API ready."  Interface inconsistencies are addressed through the 
addition of methods to the source code that simplify the class and object 
interfaces, leaving in place where necessary the interfaces that appear to API 
users to be inconsistent because of internal code needs in the GMAT system.
