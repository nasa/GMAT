
State Management with the GMAT API
==================================

The state data in GMAT can be a bit confusing. This notebook introduces
the state variables as used for a GMAT Spacecraft, and provides some
pointers on the manipulation of the state data.

Prepare the GMAT Environment
----------------------------

Before the API can be used, it needs to be loaded into the Python system
and initialized using a GMAT startup file. This can be done from the
GMAT bin folder by importing the gmatpy module, but using that approach
tends to leave pieces in the bin folder that may annoy other users.
Running from an outside folder takes a few steps, which have been
captured in the run\_gmat.py file imported here:

.. code:: ipython3

    from run_gmat import *

Configure a Spacecraft
----------------------

We'll need an object that provides the state. Here's a basic spacecraft,
along with a reference to the state data inside of the spacecraft:

.. code:: ipython3

    sat = gmat.Construct("Spacecraft","MySat")
    iState = sat.GetState()

The state reference here, iState, operates on the member of the
Spacecraft object that GMAT uses when running a simulation. The
"internal state," referenced by iState here, is the Earth-centered
mean-of-J2000 equatorial representation of position and velocity of the
spacecraft MySat. The data is contained in a GmatState object:

.. code:: ipython3

    iState




.. parsed-literal::

    <gmatpy.gmat_py.GmatState; proxy of <Swig Object of type 'GmatState \*' at 0x7f13ceed97e0> >



GmatState objects are used to collect together an epoch and a vector of
data. These data can be accessed directly:

.. code:: ipython3

    print("The state epoch is ", iState.GetEpoch(), ", the state has ", iState.GetSize(), " elements, and contains the data ", iState.GetState())


.. parsed-literal::

    The state epoch is  21545.000000397937 , the state has  6  elements, and contains the data  [-999.999, -999.999, -999.999, -999.999, -999.999, -999.999]


The data shown here is the default GmatState vector data for a
spacecraft. The epoch is January 1, 2000 at 12:00:00.000 in TAIModJulian
time, or 21545.00000039794 in A.1 ModJulian time. Note that GMAT uses
A.1 Mod Julian as its internal epoch system. The state has 6 elements
The position and velocity data are filled in with the dummy entries
-999.999. Working with Cartesian and Keplerian Representations a
spacecraft in GMAT has a second collection of data: the state data for
the spacecraft in the coordinate system set on the spacecraft. These
data are the spacecraft's "display state," named that way because they
are the data displayed to the user. Users interact with the display
state similarly to the way they interact with the scripting language.
Data for a Keplerian state can be set using the SetField() method, as
shown here:

.. code:: ipython3

    sat.SetField("StateType", "Keplerian")
    sat.SetField("SMA", 7015)
    sat.SetField("ECC", 0.0011)
    sat.SetField("INC", 98.6)
    sat.SetField("RAAN", 75)
    sat.SetField("AOP", 90)
    sat.SetField("TA", 33.333)

At this point it can appear at first glance that the data is set, but it
really is not. The spacecraft object cannot interpret the state data.
The data set using SetField needs more information than a spacecraft
object can provide by itself. Specifically, the spacecraft here does not
have a connected coordinate system. Cartesian state data set on the
spacecraft does not have connections defining the coordinate origin, nor
the structures needed to set the orientation of the axes defining
directions. Additionally, the spacecraft does not have the the
gravitational constant needed to interpret Keplerian data.

In this uninitialized state, the spacecraft uses its GmatState buffer to
hold the data entries. We can see that the data is not yet fully
populated by posting queries to the spacecraft:

.. code:: ipython3

    print("The internal state buffer just holds preinitialization data (Keplerian here):  ", iState.GetState())
    print("but access to the Keplerian state shows that it is not correct:", sat.GetKeplerianState())


.. parsed-literal::

    The internal state buffer just holds preinitialization data (Keplerian here):   [7015.0, 0.0011, 98.6, 75.0, 90.0, 33.333]
    but access to the Keplerian state shows that it is not correct: 0                0                0                0                0                0               


The GMAT objects are not yet initialized, so the Keplerian state data is
not correct. Once we initialize the system, the Keplerian state will be
correct, and the internal state will be updated to the EarthMJ2000Eq
system. The interobject connections necessary for these settings are
made by calling the API Initialize() function:

.. code:: ipython3

    gmat.Initialize()
    print("The initialized internal state buffer is EarthMJ2000Eq:  ", iState.GetState())
    print("and the Keplerian state is correct:  ", sat.GetKeplerianState())


.. parsed-literal::

    The initialized internal state buffer is EarthMJ2000Eq:   [-150.99058171804361, -3946.626071010534, 5789.742898439815, -2.23046049968889, -5.931020059857665, -4.095581409074377]
    and the Keplerian state is correct:   7015.000000000001 0.00110000000000004 98.59999999999999 75               90.00000000000402 33.33299999999598


Changes made to the state variables are now applied to the state as
expected:

.. code:: ipython3

    sat.SetField("SMA", 8000)
    print("Internal state:  ", iState.GetState())
    print("Cartesianian     ", sat.GetCartesianState())
    print("Keplerian:       ", sat.GetKeplerianState())
    print()
    sat.SetField("INC", 45)
    print("Internal state:  ", iState.GetState())
    print("Cartesianian     ", sat.GetCartesianState())
    print("Keplerian:       ", sat.GetKeplerianState())
    print()
    sat.SetField("TA", 50)
    print("Internal state:  ", iState.GetState())
    print("Cartesianian     ", sat.GetCartesianState())
    print("Keplerian:       ", sat.GetKeplerianState())


.. parsed-literal::

    Internal state:   [-172.19168264352888, -4500.785255607168, 6602.700383110265, -2.088638988531676, -5.553902317709759, -3.835168124650226]
    Cartesianian      -172.1916826435289 -4500.785255607168 6602.700383110265 -2.088638988531676 -5.553902317709759 -3.835168124650226
    Keplerian:        8000.000000000007 0.001100000000000707 98.59999999999999 75               90.00000000001161 33.3329999999884
    
    Internal state:   [-5697.7414619496, -3020.2186545041395, 4721.90552145557, 1.1208679033712874, -6.413887097497282, -2.7427113896944304]
    Cartesianian      -5697.7414619496 -3020.21865450414 4721.90552145557 1.120867903371287 -6.413887097497282 -2.74271138969443
    Keplerian:        8000.000000000011 0.001100000000000964 45.00000000000001 75.00000000000001 90.00000000001836 33.33299999998167
    
    Internal state:   [-5094.78342738948, -4974.9069027511405, 3633.5822378210464, 2.5169011956354206, -5.379735538828468, -3.8235178821457656]
    Cartesianian      -5094.78342738948 -4974.90690275114 3633.582237821046 2.516901195635421 -5.379735538828468 -3.823517882145766
    Keplerian:        8000.000000000012 0.001100000000001075 45.00000000000001 75.00000000000001 90.00000000002314 49.99999999999523


Changing Coordinate Systems
---------------------------

The previous section shows how to access Cartesian and Keplerian
representations of the system. In this section we will work with a
couple of different coordinate systems: an Earth fixed coordinate system
named "ECF" and accessed using the Python reference ecf, and a solar
ecliptic system named "SolarEcliptic," referenced as sec. These
coordinate systems are built using the code

.. code:: ipython3

    ecf = gmat.Construct("CoordinateSystem", "ECF", "Earth", "BodyFixed")
    sec = gmat.Construct("CoordinateSystem", "SolarEcliptic", "Sun", "MJ2000Ec")

In this section, the spacecraft sat defined previously will be used with
the Earth fixed coordinate system, and a copy of that spacecraft will be
used with the solar ecliptic system. GMAT's objects support a method,
Copy(), that copies an object into another object of the same type.
Rather than set up a new spacecraft from scratch, we'll use that
framework to get started by creating a new spacecraft and then setting
the coordinate systems so that the original spacecraft uses the ECI
coordinate system and the new spacecraft uses the solar ecliptic system.

.. code:: ipython3

    solsat = gmat.Construct("Spacecraft","SolarSat")
    solsat.Copy(sat)
    
    # Now set coordinate systems
    sat.SetField("CoordinateSystem","ECF")
    solsat.SetField("CoordinateSystem","SolarEcliptic")

We've reset the coordinate system names on the spacecraft at this point,
but have yet to reset the associated objects because the Initialize()
function that connects objects together has not been called since making
the reassignment. The data reflects this state of the system:

.. code:: ipython3

    # Show the data after setting the new coordinate systems, before initialization
    print("The spacecraft ", sat.GetName(), " initialization state is ", sat.IsInitialized())
    print("The internal state buffer:  ", iState.GetState())
    print("The ECF Cartesian State:    ", sat.GetCartesianState())
    print("The ECF Keplerian State:    ", sat.GetKeplerianState())
    print()
    print("The spacecraft ", solsat.GetName(), " initialization state is ", sat.IsInitialized())
    print("The internal state buffer (SolarSat):  ", solsat.GetState().GetState())
    print("The SolarEcliptic Cartesian State:     ", solsat.GetCartesianState())
    print("The SolarEcliptic Keplerian State:     ", solsat.GetKeplerianState())


.. parsed-literal::

    The spacecraft  MySat  initialization state is  True
    The internal state buffer:   [-5094.78342738948, -4974.9069027511405, 3633.5822378210464, 2.5169011956354206, -5.379735538828468, -3.8235178821457656]
    The ECF Cartesian State:     -5094.78342738948 -4974.90690275114 3633.582237821046 2.516901195635421 -5.379735538828468 -3.823517882145766
    The ECF Keplerian State:     8000.000000000012 0.001100000000001075 45.00000000000001 75.00000000000001 90.00000000002314 49.99999999999523
    
    The spacecraft  SolarSat  initialization state is  True
    The internal state buffer (SolarSat):   [-5094.78342738948, -4974.9069027511405, 3633.5822378210464, 2.5169011956354206, -5.379735538828468, -3.8235178821457656]
    The SolarEcliptic Cartesian State:      -5094.78342738948 -4974.90690275114 3633.582237821046 2.516901195635421 -5.379735538828468 -3.823517882145766
    The SolarEcliptic Keplerian State:      8000.000000000012 0.001100000000001075 45.00000000000001 75.00000000000001 90.00000000002314 49.99999999999523


*Note that the initialization state reported here is a bug: resetting
object references should toggle the initialization flag, but did not.*

Once we initialize the system, replacing the coordinate system
references with the correct objects, the data is once again correct:

.. code:: ipython3

    # Connect the GMAT objects together
    gmat.Initialize()
    
    # And show the data in the new coordinate systems
    print("The internal state buffer:  ", iState.GetState())
    print("The ECF Cartesian State:    ", sat.GetCartesianState())
    print("The ECF Keplerian State:    ", sat.GetKeplerianState())
    print()
    print("The internal state buffer (SolarSat):  ", solsat.GetState().GetState())
    print("The SolarEcliptic Cartesian State:     ", solsat.GetCartesianState())
    print("The SolarEcliptic Keplerian State:     ", solsat.GetKeplerianState())


.. parsed-literal::

    The internal state buffer:   [-5094.78342738948, -4974.9069027511405, 3633.5822378210464, 2.5169011956354206, -5.379735538828468, -3.8235178821457656]
    The ECF Cartesian State:     3980.769626359613 -5904.072200723337 3633.84663580491 5.31337371013498 1.221190102125526 -3.82343374194999
    The ECF Keplerian State:     7197.708272712511 0.1106817774544226 47.10837940070086 152.2889386356222 322.0637563061007 179.5887814511714
    
    The internal state buffer (SolarSat):   [-5094.78342738948, -4974.9069027511405, 3633.5822378210464, 2.5169011956354206, -5.379735538828468, -3.8235178821457656]
    The SolarEcliptic Cartesian State:      -26505087.9080278 144694001.6268158 4700.442019894719 -27.27732399951776 -11.92620879192113 -1.367891168160935
    The SolarEcliptic Keplerian State:      144849901.1130946 0.2292154440704447 2.702016602265948 280.4191667873194 286.9680459339144 252.9931176051724

