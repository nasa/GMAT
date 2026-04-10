********
Overview
********
GMAT provides access to its internal data structures, classes, and objects from either Python or MATLAB and Java through an Application Programming Interface (API).  The GMAT API is currently a beta level component.  The code is in place, documented, and quite usable, but lacks a thorough set of tests that validate its functionality and that uncover issues that may exist.  This document describes the testing procedures for the API as it transitions from beta status to release status, and as the system continues to evolve.

The GMAT API is generated using the SWIG interface compiler.  SWIG uses the GMAT C++ header files to generate language specific code that encapsulates GMAT code for use by the target language.  When a class is exposed for API use, all of the public features of that class are available programmatically to the API user.  

SWIG provides a mechanism to tailor the interfaces that it builds into an API.  Occasionally features are hidden from the user in the API.  This generally happens if the target language does not support the corresponding C++ feature.  When a core feature of a GMAT class is hidden this way, the API developers generally provide an alternative mechanism to access that feature.

.. topic:: Nomenclature
   
   The following terms are used in this document:

   .. glossary::

      GMAT
         The General Mission Analysis Tool.

      API
         The GMAT Application Programming Interface.  From time to time "GMAT API" is shortened to "API" for convenience.

      Platform
         The user's environment for access to the API: either Java (and MATLAB through Java) or Python.  If a feature applies only to one supported platform, the platform dependence will be specified.

      SWIG
         The system used to create platform specific APIs from the GMAT source code.  The name SWIG originated as a acronym for "Simplified Wrapper and Interface Generator."

      Test System
         The GMAT script-based test system, extended to include tests of the GMAT API.  "Test system" in this document does not include the GMAT GUI test system.

      OC
         Optimal Control.  The acronym OC is used occasionally to reference features specific to optimal control.

      Nav
         Navigation.  The shortened form, "nav," is used occasionally to indicate components that belong to GMAT's estimation subsystem.


Testing the API
===============

Testing Philosophy
------------------
GMAT is a large system, consisting of many classes that interact to simulate a spacecraft mission.  The GMAT system is extensively tested multiple times a week using a script based test system running on Linux and Windows.  The system is tested somewhat less frequently, but regularly, on Mac.  The core numerics in GMAT that users use for spacecraft modeling are exercised regularly and proven to be solid for mission design work and, once reviewed and approved, for operations.  That feature of the system makes testing of the API simpler than if the numerics were not already proven for use.  The GMAT API tests focus on interfaces into the tested GMAT system, and as much as possible (given the automated nature of the test system) on usability. 

API Helper Functions
++++++++++++++++++++
The GMAT system includes several enhancements built specifically for API users working in the targeted frameworks.  These features are exercised extensively in the API tests.  There are two types of features added to GMAT that are explicitly called out in this test plan for testing: new functions that are part of the GMAT API subsystem that were explicitly coded to make use of the API simple, and methods added to GMAT objects designed to make them simple to use for API users.  The API test plan identifies the API specific functions explicitly in :ref:`ApiFunctionTests`, and the additions to GMAT classes in :ref:`APIMethods`. 

GMAT Components and the API
+++++++++++++++++++++++++++
The GMAT API exposes GMAT components for direct manipulation in ways that differ from the approach taken by users of the GMAT scripting language or graphical user interface.  The API test system exercises the core GMAT components with that change in approach in mind by exercising GMAT components directly for data input and output, and for computational access when appropriate.  The goal of the API testing framework is not to validate GMAT numerics.  That aspect of testing is performed by the nightly script based test system.  The API tests focus on tests of access to GMAT objects for completeness and reliability.

From time to time, issues in the underlying GMAT code may be uncovered while writing tests for the API.  Those issues are reported in the GMAT bug tracking systems as issues in corresponding GMAT components or subsystems.  GMAT bugs are not considered to be API bugs, and the API tests are not intended to fully test GMAT.  Bugs are reported for the API when the **interfaces** to GMAT components are not working as needed.  

At this writing, the API sees 481 separate GMAT classes.  Roughly half (243, to be precise) of this set falls into the collection of numerical parameters used to report data from GMAT scripts.  42 are mathematical operations.  15 are built-in "GMAT functions."  20 are axis systems, used in the CoordinateSystem class, making the coordinate system subsystem 21 classes.

Testing in detail every GMAT class accessible using the API is unnecessary and redundant with the existing GMAT script based tests.  Instead, the core interfaces into GMAT objects are tested, working through the GMAT class structure area by area.  More details about testing of API calls to GMAT classes are provided in :ref:`ExposedClasses`.  This document is intended to explain that testing approach more completely.