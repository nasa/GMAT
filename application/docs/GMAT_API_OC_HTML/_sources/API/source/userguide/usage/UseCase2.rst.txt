***************************************************
Example: MONTE-GMAT Interoperability
***************************************************
GMAT is able to work with other programs through the API. Examples have been made 
between GMAT and MONTE for OSIRIS-REx and LUCY missions. In these examples, data 
will be shared between MONTE and GMAT using interfaces built with MONTE's 
native Python framework and GMAT's API, accessed through Python.

For access to these example scripts contact the GMAT development team. 

Ephemeris Sharing
=================
Both GMAT and MONTE have ephemeris reading and writing capabilities.  GMAT 
supports four types of spacecraft ephemerides: Goddard specific "Code-500", STK
time-position-velocity (.e), CCSDS OEM, and SPICE SPK formats.  MONTE supports 
SPICE based SPK ephemerides, so that format is used for data interchange between
the systems. Ephemeris sharing between GMAT and MONTE is straightforward: 
use the system providing the ephemeris to generate the file, and then import 
it into the other system. 

Maneuver Sharing
=================
Allow maneuver planning for both finite and impulse burns. 

Covariance Sharing
=======================
Possible to share covariance arrays between MONTE and GMAT. The arrays are in
slightly different formats so some conversion will be necessary between the two 
programs. 

Dynamics Sharing
================
Dynamics sharing is done through the External Force Model Plugin. (Currently an alpha 
feature as of R2022a) 
