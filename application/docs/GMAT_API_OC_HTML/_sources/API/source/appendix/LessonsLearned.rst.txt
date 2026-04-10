=========================================
GMAT API Prototypes
=========================================

The GMAT development team has experimented with making the GMAT code available 
for external use two previous times, targeting information gathering leading to 
a production GMAT API.  These experiments provide lessons that are incorporated 
into the design presented in this document.  

-----------------------------------------
A short history of the GMAT API
-----------------------------------------

In 2011, core features of GMAT were exposed for access to internal projects at
Goddard Space Flight Center (GSFC) using a plugin library 
:ref:`[CInterfaceAPI]<CInterfaceAPI>`.  That plugin, libCInterface, allows 
access to objects initialized in the GMAT Sandbox.  Calls to those objects 
provide derivative information used by the Orbit Determination
Toolbox (ODTBX) project at GSFC, and by other system users.  This application 
was the first attempt at an API into GMAT computations for external users.
While this early interface provided a needed piece of functionality, the nature
of the C Interface plugin required code changes and a system rebuild when a new 
component needed to be accessed by users of the plugin.  The C Interface code 
generated during this development cycle is exposed for MATLAB access using the 
MATLAB shared library loading mechanism.  The C Interface plugin is still part 
of the GMAT delivery packages at this writing.  Once the API is sufficiently 
complete, the C Interface plugin will be eligible for replacement by the newer, 
more complete API.

A new experiment building a GMAT API prototype was built as an internal research 
and development project at GSFC in 2016 :ref:`[SWIGExperiment]<PrototypeAPI>`.  
The results of that study identified 
the `Simplified Wrapper and Interface Generator (SWIG) <http://www.swig.org/>`_ 
as a useful mechanism for creating a robust GMAT API capable of exporting most 
of the GMAT code for use by external projects.  The 2016 API prototype has been 
used to test interfaces with ODTBX, and works as expected in that context.  
Users of the new approach to access of GMAT capabilities can create and use a 
much more complete set of components than were available in the C Interface 
plugin.  The component exposure using this interface opens GMAT's code to more 
general use, and allows access from a variety of programming languages including 
Python, Java, and C#.  The SWIG approach allows API generation from both core 
GMAT code and from GMAT plugin components, a feature unavailable with the 
prototype C Interface plugin.  The SWIG API generated from this work is 
available in an API branch of the GMAT repository at GSFC.


.. _LessonsLearned:

-----------------------------------------
User Experiences
-----------------------------------------

Users at GSFC were surveyed at the start of the production API work that 
produced the design documented here.  The users surveyed include users familiar 
with both of the API experiments described above.  Feedback was collected for 
the usability of the earlier API systems, and for projected needs at GSFC.  The
highlights of the user experiences informed the API design process.  Key elements 
that are targeted for the production API can be grouped into three use styles, 
three application frameworks (four if C++ is included), two near term needs, and 
a single overriding usage requirement:

*  Use Styles

   * Users of GMAT scripts that want to change scripted data during a run
   * Analysts that want an easy-to-use toolbox of validated astrodynamics components
   * Users that want to interact at a detailed level with instances of GMAT classes

*  GSFC Desired Application Frameworks

   * Python
   * Java
   * MATLAB (via Java)
   * C++ (See note below)
   
*  Near Term Needs

   * Dynamics Modeling and Propagation

     * Dynamics models must include Jacobian data
     * Propagation should be available for all of GMAT's propagators

   * Measurement Models from the Estimation Plugin

*  Usage

   * Users need to be able to use the API without detailed knowledge of GMAT code

     * The API needs usage documentation
     * Users need online access to available API and object settings

These feedback considerations provide guidance for the GMAT API described here.  

.. note::
  **C++ and the API**  

  GMAT is coded in C++.  The tool used to generate the API, SWIG, provides 
  interface code that exposes the native C++ code to users on other development 
  platforms.  SWIG presumes that C++ coders will simply call into the native 
  code directly.  There is no "C++ API" per se, but the functions added to GMAT 
  to support the API on the target platforms  can be called from a developer's 
  C++ code.  Some functionality, like the help system, designed for interactive 
  platform use, is of limited use for users of compiled C++ code.
