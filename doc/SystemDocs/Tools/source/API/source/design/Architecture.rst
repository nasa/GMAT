==============================================
GMAT Architectural Components
==============================================

The GMAT API exposes core GMAT classes and processes to API users.  In order to
understand the API, it is useful to understand at a high level how GMAT works, 
and how the API encapsulates the GMAT design for use outside of the program.   

------------------------------------
The GMAT Architecture
------------------------------------

The GMAT system consists of a set of components that set up a framework for 
executing spacecraft mission analysis simulations, and a set of components used 
to define and run the simulation.  The former is referred to as the GMAT engine.
The latter defines the components that users script when they run a simulation.
:numref:`GmatFlow` shows the connections between the components of the GMAT engine.
The main control element of GMAT is a component called the Moderator.  GMAT's 
Moderator provides an interface into the inner working of GMAT, and manages user
interactions with the system.  Simulations are run in a Sandbox, using clones of 
user objects created in factories and stored in the GMAT configuration.  Users
interact with GMAT through interpreters that convert script or GUI descriptions
of simulation components into the objects used for the simulation.  The results 
of a simulation are passed, through a Publisher, to subscribing components, 
which write files or display data for the user.  In a nutshell, that is the 
architecture shown in :numref:`GmatFlow`.

.. _GmatFlow:
.. figure:: ../../../images/API/GMAT_Engine_Flow.png
   :scale: 50
   :align: center

   The GMAT engine, showing interactions between the components.

Another view of the components used in GMAT is shown in the component stack 
diagram in :numref:`GmatStack`.  GMAT is built on a set of utility functions used for 
string, vector and matrix manipulations, core numerical operations, file manipulations,
and general purpose time and state representations.  The GMAT Engine components 
are built on these utilities, as are the classes defining the objects used in 
a GMAT simulation.  User interfaces into the GMAT system are built on top of
these core elements of the system.

.. _GmatStack:
.. figure:: ../../../images/API/GMATStack.png
   :scale: 70
   :align: center

   The GMAT component stack.

All of the user configured simulation components are built on a class, GmatBase, 
that provides the serialization interfaces used to set the properties of the 
components through object fields.  GmatBase provides the interfaces used when 
reading and writing simulation objects, either to script files or to panels on 
the GMAT graphical user interface.  Resources - objects like the spacecraft 
model, coordinate systems, force models, propagators, environmental elements, 
hardware components, and numerical engines used for estimating, targeting, and 
optimizing - are all built on top of the GmatBase class, as is the mission 
timeline scripted as a sequence of GMAT commands.  The mission timeline is 
referred to as the GMAT mission control sequence in the documentation. 

Most users of the GMAT API do not interact directly with the components of the 
GMAT engine.  Those components are described in the GMAT Architectural 
Specification :ref:`[Architecture]<ArchSpec>`.  Typical users of the GMAT API 
fall into two groups: users that use GMAT components in their work, and users
that manipulate scripted components prior to, during, or after the execution of 
a script.  The API design documented below focuses on these users.  
