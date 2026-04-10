.. GMAT TLE Propagator documentation master file, created by
   sphinx-quickstart on Wed Dec  4 20:04:39 2019.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

GMAT TLE Propagator
===================
GMAT users have requested the ability to model spacecraft using NORAD two-line
element sets. Thinking Systems was asked about this possibility while discussing
orbit modeling for the Planetary Society's Lightsail 2 spacecraft in the fall of 
2019.  This plugin component, free for distribution and use under the Apache 
license, provides that capability.  Attribution for use of the component is 
appreciated, as is feedback on any issues encountered.

The TLEPropagator plugin component from Thinking Systems adds the ability to
propagate a spacecraft directly from a NORAD two-line element set.  The 
implementation provided here integrates the SPICE TLE propagator into GMAT so 
that spacecraft can be modeled from the TLE data files available from Celestrak
or from other sources that format their TLE data similarly.  The general format
for an element set used by this component is

   | LIGHTSAIL 2             
   | 1 44420U 19036AC  19311.70264562  .00005403  00000-0  12176-2 0  9991
   | 2 44420  24.0060  72.9267 0016343 241.6999 118.1833 14.53580129 17852

The first line identifies the spacecraft described by the subsequent two line
element set.  `Celestrak <https://www.celestrak.com/NORAD/documentation/tle-fmt.php>`_
provides a detailed description of the element set itself.

This manual explains how to install and use the component, and provides 
background information about how the component was built and tested.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   Installation
   Scripting
   Testing



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
