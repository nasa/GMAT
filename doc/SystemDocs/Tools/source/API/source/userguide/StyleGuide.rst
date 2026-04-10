*********************************************
Conventions in the API Examples and Use Cases
*********************************************

The usage section of this documentation shows API users how to perform several
common tasks using the GMAT API.  

General Conventions
===================

* Numbers that would normally display as 16 digits are truncated to fit on the 
  page when necessary.
* Extraneous white space has been removed from some output.
* Interactive and scripted code segments are shown offset in special blocks, 
  like this:

  .. code-block:: python

     >>> import gmatpy as gmat

  Interactive Python blocks, like the one shown above, include Python's triple 
  bracket marker.  Blocks from Python script files do not have this marker:  

  .. code-block:: python

     import gmatpy as gmat

  Interactive MATLAB elements are displayed similarly, with MATLAB's Command 
  Window line marker:

  .. code-block:: matlab

     >> load_gmat

Note that the typesetting for MATLAB is also different from the Python settings.

User Workspace and GMAT Typography
==================================
This documentation provides examples in the two application environments 
supported by the core GMAT API development team: Python and MATLAB (via Java).
The API is built using SWIG, and can be built for other platforms.  This 
documentation does not address other platforms.

Work performed using the API involves interactions with objects created in GMAT 
that are accessed in the user's application environment.  This results in 
references in the user's environment of objects in the GMAT environment.  The
following conventions are observed in this document to help clarify this
component dichotomy:

* Objects created in GMAT use camel-cased names.  User references to those 
  objects are presented in lowercase.  For instance, the Python script line

  .. code-block:: python
  
     mysat = gmat.Construct("Spacecraft", "MySat")

  creates a GMAT Spacecraft object named "MySat," stored in GMAT, that a user 
  accesses through their mysat environment variable.