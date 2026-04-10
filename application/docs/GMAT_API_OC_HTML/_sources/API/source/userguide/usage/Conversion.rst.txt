Time and Coordinate Conversions
===============================
One simple application of the GMAT API is the construction of routines that
convert from one systm to another.  Two examples of that application are 
presented here: time system conversion and coordinate system conversion. 

Time System Conversion
----------------------


Coordinate System Conversion
----------------------------


.. code-block:: python

   >>> import gmatpy as gmat
   >>> moonec = gmat.Construct("CoordinateSystem", "MoonEc", "Luna", "MJ2000Ec")
   >>> eartheq = gmat.Construct("CoordinateSystem", "EarthEq", "Earth", "MJ2000Eq")
   >>> instate = gmat.Rvector6()
   >>> outstate = gmat.Rvector6()
   >>> instate[0] = 4000.0
   >>> instate[1] = 1000.0
   >>> instate[2] = 0.0
   >>> instate[3] = 0.1
   >>> instate[4] = 0.4
   >>> instate[5] = 3.1
   >>> print(instate)
   4000          1000          0             0.1           0.4           3.1
   >>> cconverter = gmat.CoordinateConverter()
   >>> gmat.Initialize()
   >>> cconverter.Convert(28718, instate, moonec, outstate, eartheq)
   True
   >>> print(outstate)
   278537.716 272130.548 84187.769 -0.64373770 -0.28543905 3.30709590

