GMAT Mass Property Mathematics
==============================
The center of mass location and moment of inertial tensor for a GMAT Spacecraft are set as parameters, element by element, on the GMAT spacecraft.  These settings provide the dry vehicle fields used to compute the inputs to torque calculations in the system.  Spacecraft that use the dry mass property settings script those properties as shown in :numref:`DryMassPropertySettings`.

.. _DryMassPropertySettings:
.. code-block:: matlab
   :caption: Scripting for a fixed Spacecraft center of mass and moment of inertia tensor

    DefaultSC.DryCMx = 0.10;
    DefaultSC.DryCMy = 0.05;
    DefaultSC.DryCMz = 0.44;
 
    DefaultSC.DryMOIxx = 13.91;
    DefaultSC.DryMOIxy =  0.09;
    DefaultSC.DryMOIxz = -0.13;
    DefaultSC.DryMOIyy = 16.95;
    DefaultSC.DryMOIyz =  0.59;
    DefaultSC.DryMOIzz = 19.89;

Note that only six elements of the moment of inertia tensor are specified.  The tensor is symmetric. 

If the spacecraft needs variable values for these elements, the user can configure GMAT one of two ways: either through lookup tables for the center of mass and moment of inertia, or through computation based on the dry spacecraft settings and the mass properties of attached hardware.  This document describes the lookup table implementation initially added to support the GOES spacecraft.  When the computational approach becomes necessary, the document will be updated to accommodate it.

Lookup Table Specification
--------------------------
Scripting for table based center of mass and moment of inertia data is shown in :numref:`TabulatedMassPropertySettings`.  If a script specifies the CMLookupTable field for a spacecraft, the center of mass for the wet vehicle is interpolated from table data, replacing internal calculation of the center of mass of the spacecraft.  Similarly, when a script sets the MOILookupTable field. moments and products of inertia are interpolated from tabulated data tables rather than from settings on the objects themselves.

.. _TabulatedMassPropertySettings:
.. code-block:: matlab
   :caption: Scripting for a fixed Spacecraft center of mass and moment of inertia tensor

    Create Spacecraft SampleSat;
    SampleSat.DryMass = 1250.0;
    SampleSat.Tanks = {OxidizerTank, HydrazineTank};

    % If lookup tables are specified, use tables rather than internal calculations
    SampleSat.CMLookupTable  = SampleSatCenterOfMass.table;
    SampleSat.MOILookupTable = SampleSatMomentOfInertia.table;


Mass properties for a spacecraft may be specified using a pair of tables assigned to the spacecraft that specify the location of the center of mass and the unique moment of inertia tensor components as a function of total spacecraft mass.  The total spacecraft mass is computed as the sum of the spacecraft dry mass with the mass of propellant in each attached fuel tank.  For n tanks,

.. math::
   :label: MassIndex

	M_T = M_{dry} + \sum_{i=1}^{n} m_i

The center of mass data is tabulated in a text file indexed by this value.  A sample file is shown in :numref:`CoMTable`.  The header data in this file is informational only in the current implementation.  The data shown there is not used in the GMAT code but may be used in a later implementation.  

.. _CoMTable:
.. code-block:: matlab
   :caption: Sample Center of Mass data file

    % Center of Mass Datafile
    Spacecraft:  SampleSat
    Index:  TotalMass
    CoordinateSystem:  BCS
    Units:   Meters
    
    BeginData
    2200    0.0220     0.0500    0.3000
    2100    0.0215     0.0366    0.2888
    2000    0.0211     0.0233    0.2777
    1900    0.0206     0.0100    0.2666
    1800    0.0202    -0.0033    0.2555
    1700    0.0197    -0.0166    0.2444
    1600    0.0193    -0.0300    0.2333
    1500    0.0188    -0.0433    0.2222
    1400    0.0184    -0.0566    0.2111
    1300    0.0180    -0.0700    0.2000
    EndData

The data tabulated between the BeginData and EndData lines are used in GMAT to interpolate the location of the center of mass, using GMAT's Lagrange interpolator.  The first column of the data is spacecraft's total mass, providing the index into the table.  This index column must be monotonic, but can be either increasing or, as shown here, decreasing.  The location of the center of mass is tabulated in Cartesian body fixed coordinates.  Each row specifies the center of mass location corresponding to the total mass value in the first column.  The data are in X-Y-Z order.  The center of mass location is specified in meters.  If the mass index from :eq:`MassIndex` is outside of the range given by the upper and lower bounds of the tabulated data, GMAT throws an exception indicating that the spacecraft center of mass cannot be interpolated.

