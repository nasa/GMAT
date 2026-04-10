************
Introduction
************

The General Mission Analysis Tool, GMAT, is a general purpose spacecraft mission 
design, analysis, and operations tool.  GMAT is in active development, and has
been used for multiple spacecraft missions.  GMAT is the operational tool for 
maneuver design and navigation on several missions in the Goddard Space Flight 
Center's Flight Dynamics Facility.  It is used for mission design at GSFC and at 
numerous other organizations throughout the world.  GMAT is a free and open 
source tool, available at the GMAT wiki :ref:`[GmatWiki]<GmatWiki>`.

Core capabilities of GMAT can be accessed using an Application Programming 
Interface (API).  This document describes the GMAT API, and includes sample 
usage from Python and MATLAB using Java. 

GMAT is coded using an object oriented approach documented in the GMAT 
Architectural Specification :ref:`[Architecture]<ArchSpec>`.  The system has 
been under development since 2002.  Users interact with GMAT through a 
spacecraft domain specific language built into the system, modeled on the MATLAB 
programming language.  The GMAT API opens the system's object model to users 
that want to interact directly with the core system components, outside of the 
scripted interfaces used when running the application.

..
  The materials presented in this document are divided into three sections:  

The materials presented in this document are divided into two sections:  

* The first section documents the design of the API.  In it, you will find an 
  extremely high level overview of the GMAT code and a matching API overview, 
  a discussion of the philosophy governing the API that includes use cases, and a 
  description of the additions to the GMAT code base added for the API that enable 
  the examples and features requested by the user community.

* The second section is a user's guide for the GMAT API.  It contains instructions 
  for installing the API code, a "Getting Started" tutorial for initial use of the 
  API, and additional hints, tips, and use case descriptions designed to help you
  start using the GMAT API.

.. 
  * The third section is a basic reference guide for the GMAT API.  It includes 
    descriptions of the additions to GMAT that make the API work.  Note that this 
    section is not intended to be a reference guide for GMAT, per se; that reference 
    is found in the GMAT documentation.

This documentation concludes with appendices that provide additional API 
guidelines, review notes, and other information for developers and API users.
