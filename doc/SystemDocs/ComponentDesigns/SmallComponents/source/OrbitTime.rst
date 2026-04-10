*******************************
OrbitTime Calculation Parameter
*******************************

Overview
========
Given the position of an Earth orbiting spacecraft, :math:`\textbf{r}`, and the position of the Sun, :math:`\textbf{R}_S` expressed in an Earth-centered coordinate system, the OrbitTime parameter calculates the local time of day of a spacecraft's subsatellite point.  That time is computed by calculating the angle between the projection of the spacecraft's position vector into the coordinate system's XY plane with the projection of the Sun vector into the same plane.  The resulting value is reported as the true (rather than mean) time of day, in hours, for the subsatellite point -- a value between 0 and 24.

The OrbitTime is computed based on a 24 hour day.  In the current implementation, it reports data for Earth-centered coordinate systems.  No other origin is permitted.  The parameter is a property of the spacecraft orbital state.  It can be reported, plotted, and used as a stopping condition.  It cannot be set from script.

Scripting
=========
The OrbitTime parameter is coordinate system dependent because it requires specification of the plane used to define the vector projections.  The parameter scripting takes the usual form:

.. code-block:: matlab
   :caption: OrbitTime
   :linenos:

   MySat.EarthMJ2000Eq.OrbitTime

If the CoordinateSystem is not specified, EarthMJ2000Eq is used.  When used in a plot or report:

.. code-block:: matlab
   :caption: OrbitTime used in plots and reports
   :linenos:

   Create XYPlot XYPlot1;
   XYPlot1.XVariable = DefaultSC.A1ModJulian;
   XYPlot1.YVariables = {DefaultSC.EarthMJ2000Eq.OrbitTime};

   Create ReportFile StopTime
   ...
   BeginMissionSequence
   ...
   Report StopTime DefaultSC.OrbitTime;

and as a stopping condition:

.. code-block:: matlab
   :caption: Propagation to Local Noon using OrbitTime
   :linenos:

	Propagate TheProp(TheSC) {TheSC.OrbitTime = 12.0};

Mathematical Specification
==========================
OrbitTime is calculated based on two vectors: the spacecraft position relative to the center of the Earth, :math:`\textbf{r}`, and the Sun vector, :math:`\textbf{R}_S`, expressed in the specified coordinate system.  The Sun vector used in the calculation is the vector from the center of the Earth to the Sun.  The projections of these vectors into the orbital plane, :math:`\textbf{r}^{\perp}` and :math:`\textbf{R}_S^{\perp}` are built by setting the Z component of each vector to zero.  The in-plane vectors are then normalized.  The dot product between the resulting unit vectors gives an angle

.. math::
   :label: The OrbitTime Angle

   \theta = \arccos(\hat{\textbf{r}}^{\perp} \cdot \hat{\textbf{R}}_S^{\perp})

(That angle is expressed in degrees in what follows.)  The angle is quadrant checked, using the cross product of the projected vectors, to ensure that is falls in the interval [0, 360), and then divided by 15 to convert from degrees to hours.  

.. math::
   :label: The time along the orbit

   T_{ot} = \left({{\theta}\over{15.0}}\right) + 12.0

The OrbitTime is offset 12 hours from the value returned by the arccosine because a value of 0 corresponds to local noon, an orbit time of 12.0.  The result is then adjusted to fall in the range [0, 24.0):

.. math::
   :label: The OrbitTime value

   T_{OT} =
   \begin{cases}
       T_{ot},& \text{if } 0 \leq T_{ot} < 24\\
       T_{ot} - 24.0& \text{if } T_{ot} \geq 24
   \end{cases}

This is the value returned from the function.
