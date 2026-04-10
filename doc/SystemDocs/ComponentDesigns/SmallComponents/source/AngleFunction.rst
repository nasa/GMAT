******************
The Angle Function
******************

Overview
========
GMAT users need the ability to compute the angle between two objects as seen from the location of a third object.  The observing object is the vertex of the angle.  The internal GMAT function Angle is used to compute that value.  The Angle function calculates the instantaneous geometric angle between objects, ignoring subtleties like light transit times, atmospheric distortions, and spacetime curvature and ignoring intervening bodies.

Users can use the Angle function to evaluate a number of orbital and ground based values.  One example of the alignment of objects as seen from a ground station - that is, the collinearity of two objects seen from a ground based point - to evaluate when a sensor on the ground is blinded (collinear with the Sun or Moon), or when radio interference may be problematic (collinearity of two objects in orbit as seen from the ground, or from a third object).

Scripting
=========
The Angle function takes three input arguments: the vertex for the angle, and two objects that lie on the vectors pointing away from the vertex.  The scripting shown in :numref:`BasicAngleScript` is an example showing the calculation of the angle between a vector pointing from a ground station (named Station) to a spacecraft (Sat) and a vector pointing from teh same ground station to the Sun.

.. _BasicAngleScript:
.. code-block:: matlab
   :caption: The Angle Function
   :linenos:

   Create Spacecraft Sat;
   Create GroundStation Station;

   Create Variable SatStationSunAngle;

   SatStationSunAngle = Angle(Station, Sat, Sun);

The Angle function works with any GMAT object that has a physical location.


Mathematical Specification
==========================
The Angle function uses two unit vectors to compute its output: a unit vector from the vertex to the first object, :math:`\hat{\textbf{r}}_1` and a vector from the vertex to the second object :math:`\hat{\textbf{r}}_2`.  The vector for a leg of the angle calculation is built using the location of the vertex, :math:`\textbf{R}_v` and the nth endpoint :math:`\textbf{R}_n`:

.. math::
   :label: A Leg of the Angle

   \textbf{r}_n = \textbf{R}_n - \textbf{R}_v

The resulting vector is normalized to make a unit vector :math:`\hat{\textbf{r}}_n`:

.. math::
   :label: The nth Unit Vector

   \hat{\textbf{r}}_n = {{\textbf{r}_n}\over{|\textbf{r}_n|}}

The resulting angle is 

.. math::
   :label: The Angle Calculation

   \theta = \arccos(\hat{\textbf{r}}_1 \cdot \hat{\textbf{r}}_2)

The output reported to the user takes the returned value and converts it into degrees.


A Complete Script
=================
:numref:`CollocationScript` shows one use case for the Angle function.  When this script is run, collocations between a spacecraft Sat1 and the Sun, Moon, and a second spacecraft (Sat2) are reported to GMAT's Message window.  Each collocation is given a different angle for the collocation determination.

.. _CollocationScript:
.. code-block:: matlab
   :caption: The Angle Function
   :linenos:

	Create Spacecraft Sat1;
	Sat1.DateFormat = UTCGregorian;
	Sat1.Epoch = '26 May 2022 14:30:00.000';
	Sat1.CoordinateSystem = EarthMJ2000Eq;
	Sat1.DisplayStateType = Keplerian;
	Sat1.SMA = 7200;
	Sat1.ECC = 0.025;
	Sat1.INC = 98;
	Sat1.RAAN = 300;
	Sat1.AOP = 0;
	Sat1.TA = 0;

	Create Spacecraft Sat2;
	Sat2.DateFormat = UTCGregorian;
	Sat2.Epoch = '26 May 2022 14:30:00.000';
	Sat2.CoordinateSystem = EarthMJ2000Eq;
	Sat2.DisplayStateType = Keplerian;
	Sat2.SMA = 7000;
	Sat2.ECC = 0.02;
	Sat2.INC = 12.85;
	Sat2.RAAN = 75;
	Sat2.AOP = 0;
	Sat2.TA = 90;

	Create GroundStation Tucson;
	Tucson.CentralBody = Earth;
	Tucson.StateType = Spherical;
	Tucson.HorizonReference = Sphere;
	Tucson.Location1 = 32.2226;
	Tucson.Location2 = 249.0253;
	Tucson.Location3 = 0.7282;

	Create ForceModel Prop_ForceModel;
	Prop_ForceModel.CentralBody = Earth;
	Prop_ForceModel.PrimaryBodies = {Earth};
	Prop_ForceModel.PointMasses = {Sun, Luna};

	Create Propagator Prop;
	Prop.FM = Prop_ForceModel;
	Prop.Type = PrinceDormand78;
	Prop.InitialStepSize = 60;
	Prop.MinStep = 0.0;
	Prop.MaxStep = 600;

	Create OpenFramesInterface OpenFrames1;
	OpenFrames1.Add = {Sat1, Sat2, Tucson};

	Create XYPlot AnglePlot;
	AnglePlot.XVariable = Sat1.ElapsedDays;
	AnglePlot.YVariables = {SunSat MoonSat SatSat};

	Create Variable SunSat MoonSat SatSat
	Create String   Data

	BeginMissionSequence;

	SunSat = Angle(Tucson, Sat1, Sun);
	MoonSat = Angle(Tucson, Sat1, Luna); 
	SatSat = Angle(Tucson, Sat1, Sat2);

	While Sat1.ElapsedDays < 5.0

	   Propagate Prop(Sat1, Sat2);
	   SunSat = Angle(Tucson, Sat1, Sun);
	   MoonSat = Angle(Tucson, Sat1, Luna); 
	   SatSat = Angle(Tucson, Sat1, Sat2);
		
		If SunSat < 5
		   Data = Sprintf('Sat1-Sun Collocation   %s:  %lf degrees', Sat1.UTCGregorian, SunSat);
		   Write Data
		EndIf

		If MoonSat < 2
		   Data = Sprintf('Sat1-Moon Collocation  %s:  %lf degrees', Sat1.UTCGregorian, MoonSat);
		   Write Data
		EndIf

		If SatSat < 1
		   Data = Sprintf('Sat1-Sat2 Collocation  %s:  %lf degrees', Sat1.UTCGregorian, SatSat);
		   Write Data
		EndIf

	EndWhile
