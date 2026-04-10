**State Management with the GMAT API**
======================================

**Problem**
-----------
GMAT contains state data for various objects that can be accessed for data collection.  API users need to also have access to state data and understand the initialization of said data as the state data in GMAT can be a bit confusing.

**Solution**
------------
This chapter demonstrates the access and manipulation of state data from a Spacecraft.  The internal state is retrieved from the GetState method on the Spacecraft, and this state is used to convert outputs to requested state types in the Spacecraft's coordinate system. Here we will cover examples of getting Cartesian and Keplerian states. As will be shown, care must be taken for whether or not this state has been properly initialized.  The Spacecraft state can also be manipulated and gathered in different coordinate systems created by the API user.

**Step 1**: Configure the Spacecraft

We'll need an object that provides the state.  We begin by creating a basic spacecraft, along with a reference to the state data inside of the spacecraft:

.. code-block:: python

    sat = gmat.Construct("Spacecraft","MySat")
    iState = sat.GetState()

The state reference here, iState, operates on the member of the Spacecraft object that GMAT uses when running a simulation.  The "internal state" referenced by iState is the Earth-centered mean-of-J2000 equatorial representation of position and velocity of the spacecraft MySat.  The data is contained in a GmatState object, which can be seen by accessing iState from the Python command prompt like this:

.. code-block:: python

    iState

GmatState objects are used to collect together an epoch and a vector of data defining the object's location in space at that epoch.  These data can be accessed directly:

.. code-block:: python

    print("The state epoch is ", iState.GetEpoch(), "in A1ModJulian, the state has ", iState.GetSize(), " elements, and contains the data:\n ", iState.GetState(), "\n")

The data shown here is the default GmatState vector data for a spacecraft.  The epoch is January 1, 2000 at 12:00:00.000 in TAIModJulian time, or 21545.00000039794 in A.1 ModJulian time.  Note that GMAT uses A.1 Mod Julian as its internal epoch system.  The state has 6 elements.  The position and velocity data at this point are filled in with the dummy entries -999.999.

**Step 2**: Working with Cartesian and Keplerian Representations

A spacecraft in GMAT has a second collection of data: the state data for the spacecraft in the coordinate system set on the spacecraft.  These data are the spacecraft's "display state", named that way because they are the data displayed to the user on the GMAT graphical user interface.  Users interact with the display state similarly to the way they interact with the scripting language.  Data for a Keplerian state can be set using the SetField() method, as shown here:

.. code-block:: python

    sat.SetField("StateType", "Keplerian")
    sat.SetField("SMA", 7015)
    sat.SetField("ECC", 0.0011)
    sat.SetField("INC", 98.6)
    sat.SetField("RAAN", 75)
    sat.SetField("AOP", 90)
    sat.SetField("TA", 33.333)

At this point it can appear to the user that the data is set, but it really is not.  The spacecraft object cannot interpret the state data.  The data set using SetField needs more information than a spacecraft object can provide by itself.  Specifically, the uninitialized spacecraft here does not have a connected coordinate system.  Cartesian state data set on the spacecraft does not have connections defining the coordinate origin, nor the structures needed to set the orientation of the axes defining directions.  Additionally, the spacecraft does not have the the gravitational constant needed to interpret Keplerian data at this point.

In this uninitialized state, the spacecraft uses its GmatState buffer to hold the data entries.  We can see that the data is not yet fully populated by posting queries to the spacecraft:

.. code-block:: python

    print("The internal state buffer just holds preinitialization data (Keplerian here):\n  ", iState.GetState())
    print("but access to the Keplerian state shows that it is not correct:\n  ", sat.GetKeplerianState(), "\n")

The Keplerian state data is not correct because the GMAT objects are not yet initialized.  Once we initialize the system, the Keplerian state will be correct, and the internal state will be updated to the EarthMJ2000Eq system.  The interobject connections necessary for these settings are made by calling the API's Initialize() function:

.. code-block:: python

    gmat.Initialize()
    print("The initialized internal state buffer is EarthMJ2000Eq:\n  ", iState.GetState())
    print("and the Keplerian state is correct:\n  ", sat.GetKeplerianState(), "\n")

Changes made to the state variables are now applied to the state as expected:

.. code-block:: python

    sat.SetField("SMA", 8000)
    print("Internal state:\n ", iState.GetState())
    print("Cartesian:\n ", sat.GetCartesianState())
    print("Keplerian:\n ", sat.GetKeplerianState())
    print()
    sat.SetField("INC", 45)
    print("Internal state:\n ", iState.GetState())
    print("Cartesian:\n ", sat.GetCartesianState())
    print("Keplerian:\n ", sat.GetKeplerianState())
    print()
    sat.SetField("TA", 50)
    print("Internal state:\n ", iState.GetState())
    print("Cartesian:\n ", sat.GetCartesianState())
    print("Keplerian:\n ", sat.GetKeplerianState())
    print()

**Step 3**: Changing Coordinate Systems

The previous section shows how to access Cartesian and Keplerian representations of the system.  In this section we will work with a couple of different coordinate systems: an Earth fixed coordinate system named "ECF" and accessed using the Python reference ecf, and a solar ecliptic system named "SolarEcliptic", referenced as sec.  These coordinate systems are built using the code:

.. code-block:: python

    ecf = gmat.Construct("CoordinateSystem", "ECF", "Earth", "BodyFixed")
    sec = gmat.Construct("CoordinateSystem", "SolarEcliptic", "Sun", "MJ2000Ec")

In this section, the spacecraft sat defined previously will be used with the Earth fixed coordinate system, and a copy of that spacecraft will be used with the solar ecliptic system.  GMAT's objects support a method, Copy(), that copies one object into another object of the same type.  Rather than set up a new spacecraft from scratch, we'll use that framework to get started by creating a new spacecraft and then setting the coordinate systems so that the original spacecraft uses the ECF coordinate system and the new spacecraft uses the solar ecliptic system.

.. code-block:: python

    solsat = gmat.Construct("Spacecraft","SolarSat")
    solsat.Copy(sat)
    
    # Now set coordinate systems
    sat.SetField("CoordinateSystem","ECF")
    solsat.SetField("CoordinateSystem","SolarEcliptic")

We've reset the coordinate system names on the spacecraft at this point, but have yet to reset the associated objects because the Initialize() function that connects objects together has not been called since making the reassignment.  The data reflects this state of the system:

.. code-block:: python

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
	print()

*Note that the initialization state reported here is a bug: resetting object references should toggle the initialization flag, but did not.*

Once we initialize the system, replacing the coordinate system references with the correct objects, the data is once again correct:

.. code-block:: python

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

**Discussion**
--------------
Utilizing the state management foundations covered in this chapter, API users can gather Spacecraft state data throughout missions in desired representations.  The other state and coordinate system types supported by GMAT can be found in the GMAT User Guide under the Spacecraft Orbit State and CoordinateSystem resources, respectively.
