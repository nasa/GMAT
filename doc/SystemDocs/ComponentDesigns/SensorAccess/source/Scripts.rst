***************************************
A Collection of Sensor Access Use Cases
***************************************

This section presents the scripting requires to write sensor contact data in GMAT.  The intent is to show current functionality, and to illustrate the changes needed as the feature set expands to meet the requirements identified by the user community.

The scripting shown here is the minimal set of script lines necessary to configure contact reporting in GMAT.  These script listings are not intended to be complete GMAT scripts. 

The scripting strategy is as follows:

* Only change the existing scripting as needed.
* As complexity emerges, point out the reasoning for additions.
* As the locator paradigm changes, point out the paradigm shift (e.g., multiple locators to one locator; post processing vs runtime processing).
* Perform minimal changes to default settings.  The intent of this chapter is to note the changes needed feature by feature, not to fully configure the system for a run.

For the purposes of providing meaningful examples, some of the reports here are based on the Artemis program `timeline <https://en.wikipedia.org/wiki/List_of_Artemis_missions>`_\ .  All Artemis references in these examples are intended to be Moon orbiting spacecraft.  GeoSat is an Earth orbiter (not necessarily at geosynchronous distance).


Station AOS/:LOS Examples
=========================
The focus for the first part of this chapter is contact reporting using GMAT's current SPACE-based post processing for spacecraft observed by ground stations.

Case 1: AOS/LOS at a ground station
-----------------------------------

**Case 1a**:  AOL/LOS at a ground station

**Use case parameters**

* One Spacecraft target
* One GroundStation observer
* No occultations
* No sensor interference
* The ground station is the sensor
    * Conic Sensor
    * 7 degree elevation (default setting in GMAT)

Current GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft GEOSat;

    Create GroundStation myStation;

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.Target = GEOSat;
    GMAT ContactLocator1.Filename = 'ContactLocator1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {myStation};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* None

**Notes**

* One output report

    * AOS and LOS only

* Sample script: Ex_R2015a_StationContactLocator.script 


**Case 1b**:  AOL/LOS and pass data at a ground station

**Use case parameters**

* One Spacecraft target
* One GroundStation observer
* No occultations
* No sensor interference
* The ground station is the sensor
 
    * Conic Sensor
    * 7 degree elevation (default setting in GMAT)

* Include pass data:

    * azimuth/elevation/range at fixed intervals
    * Starts at AOS, increments until LOS is reached

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft GEOSat;

    Create GroundStation myStation;

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.Target = GEOSat;
    GMAT ContactLocator1.Filename = 'ContactLocator1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {myStation};
    GMAT ContactLocator1.LightTimeDirection = Transmit;
    GMAT ContactLocator1.PassDataInterval = 60;
    GMAT ContactLocator1.IncludeRange = true;

**Required GMAT Changes**

* Addition of pass data settings PassDataInterval and IncludeRange
* Code updates to retrieve the data from SPICE
* Report updates for the new data

**Notes**

* One output report

    * AOS and LOS
    * Intermediate data values during each pass

* For now, these examples will ignore the "pass data" options in the following examples.  The changes noted here will apply whenever the pass data settings are needed.


Case 2:  AOL/LOS at multiple ground stations
--------------------------------------------

**Use case parameters**

* One Spacecraft target
* Three GroundStation observers
* No occultations
* No sensor interference
* Conic Sensor, 7 degree elevation

Current GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft GEOSat;

    Create GroundStation station1 station2 station3;

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.Target = GEOSat;
    GMAT ContactLocator1.Filename = 'ContactLocator2.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {station1, station2, station3};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* None

**Notes**

* One output report

    * AOS and LOS only


Case 3:  AOL/LOS, Lunar Orbit, multiple ground stations
-------------------------------------------------------

**Use case parameters**

* One Spacecraft target, orbiting the Moon
* Three GroundStation observers
* Lunar occultations
* No sensor interference
* Conic Sensor, 7 degree elevation

Current GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft Artemis1;

    Create GroundStation station1 station2 station3;

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.OccultingBodies = {Luna};
    GMAT ContactLocator1.Target = Artemis1;
    GMAT ContactLocator1.Filename = 'LunarContact1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {station1, station2, station3};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* None

**Notes**

* One output report

    * AOS and LOS only

* Accounts for LOS as spacecraft goes behind the Moon


Case 4:  AOL/LOS, 2 Lunar Orbits, multiple ground stations
----------------------------------------------------------

**Use case parameters**

* Two Spacecraft targets, orbiting the Moon
* Three GroundStation observers
* Lunar occultations
* No sensor interference
* Conic Sensor, 7 degree elevation

Current GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft Gateway Artemis4;

    Create GroundStation station1 station2 station3;

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.OccultingBodies = {Luna};
    GMAT ContactLocator1.Target = Gateway;
    GMAT ContactLocator1.Filename = 'GatewayContact1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {station1, station2, station3};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

    Create ContactLocator ContactLocator2;
    GMAT ContactLocator2.OccultingBodies = {Luna};
    GMAT ContactLocator2.Target = Artemis4;
    GMAT ContactLocator2.Filename = 'Artemis4Contact1.txt';
    GMAT ContactLocator2.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator2.InitialEpoch = '21545';
    GMAT ContactLocator2.StepSize = 300;
    GMAT ContactLocator2.FinalEpoch = '21545.138';
    GMAT ContactLocator2.UseLightTimeDelay = true;
    GMAT ContactLocator2.UseStellarAberration = true;
    GMAT ContactLocator2.WriteReport = true;
    GMAT ContactLocator2.RunMode = Automatic;
    GMAT ContactLocator2.UseEntireInterval = true;
    GMAT ContactLocator2.Observers = {station1, station2, station3};
    GMAT ContactLocator2.LightTimeDirection = Transmit;

**Required GMAT Changes**

* None

**Notes**

* Requires 2 output reports

    * AOS and LOS only

* Accounts for LOS as spacecraft go behind the Moon

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft Gateway Artemis4;

    Create GroundStation station1 station2 station3;

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.OccultingBodies = {Luna};
    GMAT ContactLocator1.Targets = {Gateway, Artemis4};
    GMAT ContactLocator1.Filename = 'GatewayContact1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {station1, station2, station3};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* Allow setting of multiple spacecraft targets for a single locator
* Update the report format for multiple targets

**Notes**

* Allows multiple targets
* All data in a single report

    * AOS and LOS only

* Accounts for LOS as spacecraft go behind the Moon

Station Masking
===============
From this point forwards, the GMAT R2020a scripting is insufficient to support the feature needs.  The additions grow gradually as new capabilities are added to the system.

Case 5:  AOL/LOS at a masked ground station
-------------------------------------------

**Use case parameters**

* One Spacecraft target
* One GroundStation observer
* No occultations
* No sensor interference
* The ground station has a sensor with a mask
    
    * Custom Sensor, collocated with the station
    * Mask references zenith and northwards to orient the mask

* GMAT Automatically uses a mask if one is found for a station

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft GEOSat;

    Create GroundStation myStation;
    GMAT myStation.AddHardware = {maskedSensor};

    Create Sensor maskedSensor;
    GMAT maskedSensor.FieldOfView = theMask;

    Create CustomFOV theMask;
    GMAT theMask.ConeAngles = [ 75 75 82    75  75  82  75 ];
    GMAT theMask.ClockAngles = [ 0 90 90.1 150 210 270 270.1 ];

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.Target = GEOSat;
    GMAT ContactLocator1.Filename = 'ContactLocator1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {myStation};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* Add sensor objects.  This may just be a rename of the "Imager" added for GOES support.
* Add the needed kernels (at least an instrument kernel - IK) for station masks.
* Add code that converts GMAT FOV masks to SPICE IK format.
* Update the contact locator so the station masking is used in contact reporting.

**Notes**

* One output report
* May want to add the option to use elevation angles as a substitute for cone angles when setting the mask

Case 6:  AOL/LOS, omitting solar intrusion and lunar occultation
----------------------------------------------------------------

**Use case parameters**

* One Lunar orbiting Spacecraft target
* One GroundStation observer
* Lunar occultations
* Solar interference
* No sensor interference
* The ground station has a sensor with a mask
    
    * Custom Sensor, collocated with the station
    * Mask references zenith and northwards to orient the mask

* GMAT Automatically uses a mask if one is found for a station

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft Artemis1;

    Create GroundStation myStation;
    GMAT myStation.AddHardware = {maskedSensor};

    Create Sensor maskedSensor;
    GMAT maskedSensor.FieldOfView = theMask;

    Create CustomFOV theMask;
    GMAT theMask.ConeAngles = [ 75 75 82    75  75  82  75 ];
    GMAT theMask.ClockAngles = [ 0 90 90.1 150 210 270 270.1 ];

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.OccultingBodies = {Luna};
    GMAT ContactLocator1.IntrudingBodies = {Sun};
    GMAT ContactLocator1.IntrusionAngles = [ 3.0 ];
    GMAT ContactLocator1.Target = Artemis1;
    GMAT ContactLocator1.Filename = 'ContactLocator1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {myStation};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* All case 5 changes
* Add the scripting and processing for intruding bodies
* Add intrusion angles defining each intruding body's intrusion zone

Note the difference between an intruding body and an occulting body.  Occulting bodies must fall between the target and the observer to interfere with signal detection, while signal detection does not occur any time an intruding body lines up, within a specified angle, with the target-observer vector.

**Notes**

* One output report
* LOS occurs both outside of the mask and when the target is within the set angle of the intruding body
* The developer may want to add the option to use elevation angles as a substitute for cone angles when setting the mask.


Spacecraft AOS/LOS Examples
===========================

Case 7: AOS/LOS at a spacecraft
-------------------------------

**Use case parameters**

* One Spacecraft target
* One Spacecraft observer
* Earth occultations
* No sensor interference
* The observing spacecraft has a sensor

    * Conic Sensor
    * 7 degree cone angle

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft GEOSat;

    Create Spacecraft TDRS8;
    GMAT TDRS8.AddHardware = {TDRSImager}
    GMAT TDRS8.Attitude = NadirPointing;

    Create Sensor TDRSImager;
    GMAT TDRSImager.FieldOfView = TDRSCone;
    GMAT TDRSImager.DirectionX = 1;     % Point the Sensor to BCS X == Nadir
    GMAT TDRSImager.DirectionY = 0;
    GMAT TDRSImager.DirectionZ = 0;

    Create ConicalFOV TDRSCone;
    GMAT TDRSCone.FieldOfViewAngle = 7;

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.OccultingBodies = {Earth};
    GMAT ContactLocator1.Target = GEOSat;
    GMAT ContactLocator1.Filename = 'SatelliteLocator1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {TDRS8};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* Sensor features coded for ground station sensors
* Allow spacecraft observers
* Use Sensor Pointing Vector (ground stations use a Zenith-North assumption)
* Write spacecraft ephemeris kernels (SPKs) for the observer spacecraft
* Write spacecraft attitude kernels (CKs) for the observer spacecraft
* Write sensor frame kernels (FKs) for the spacecraft sensor
* Make sure that occultation calculations work for space-to-space signal paths - e.g., Does the station based occultation calculation have code to ignore the station's central body?  Turn that off if the observer is in space rather than body fixed.
* Update the report as needed

**Notes**

* One output report

    * AOS and LOS only


Case 8: AOS/LOS at a spacecraft with 2 sensors
----------------------------------------------

**Use case parameters**

* One Spacecraft target
* One Spacecraft observer
* Earth occultations
* No sensor interference
* The observing spacecraft has 2 sensors:

    * Sensor 1: a conic Sensor with a 10 degree cone angle
    * Sensor 2: a conic sensor with a 0.1 degree cone angle
    * Sensors are 90 degree offset in pointing from one another

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft GEOSat;

    Create Spacecraft SFEO1;            % Space Force Earth Orbiter 1
    GMAT SFEO1.AddHardware = {SFEOWide, SFEONarrow}
    GMAT SFEO1.Attitude = NadirPointing;

    Create Sensor SFEOWide;
    GMAT SFEOWide.FieldOfView = SFEOWideCone;
    GMAT SFEOWide.DirectionX = 1;     % Point the Sensor to BCS X
    GMAT SFEOWide.DirectionY = 0;
    GMAT SFEOWide.DirectionZ = 0;

    Create Sensor SFEONarrow;
    GMAT SFEONarrow.FieldOfView = SFEONarrowCone;
    GMAT SFEONarrow.DirectionX = 0;     % Point the Sensor to BCS Y
    GMAT SFEONarrow.DirectionY = 1;
    GMAT SFEONarrow.DirectionZ = 0;

    Create ConicalFOV SFEOWideCone;
    GMAT SFEOWideCone.FieldOfViewAngle = 10;

    Create ConicalFOV SFEONarrowCone;
    GMAT SFEONarrowCone.FieldOfViewAngle = 0.1;

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.OccultingBodies = {Earth};
    GMAT ContactLocator1.Target = GEOSat;
    GMAT ContactLocator1.Filename = 'SatelliteLocator1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    %% Option: Do this to use all attached sensors?
    % GMAT ContactLocator1.Observers = {SFEO1};
    GMAT ContactLocator1.Observers = {SFEO1.SFEOWide, SFEO1.SFEONarrow};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* Case 7 changes
* Enable per-sensor scripting in the contact locator
* Update the report to write data sensor by sensor

**Ed. note:** With this use case I think we're starting to see where there is utility in run-time processing rather than post-processing.  I might want my spacecraft attitude to react to changes detected in script - for instance, SFEOWide sees a change so it wants to rotate the spacecraft to point SFEONarrow to watch the neighborhood of the change.

**Notes**

* One output report

    * AOS and LOS only


Case 9: AOS/LOS Multiple Target Spacecraft
------------------------------------------

**Use case parameters**

* Two Spacecraft target
* Two Spacecraft observers
* Earth and Lunar occultations
* No sensor interference
* The observing spacecraft have one sensor each
    * Masked (Square, 8 deg corners, but I'll use a full mask definition)
    * For single sensor collocated with the spacecraft, we treat the spacecraft as the sensor

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft GEOSat;

    Create Spacecraft SFO1 SFO2;        % Space Force Observers 1 and 2
    GMAT SFO1.AddHardware = {SFOImager}
    GMAT SFO2.AddHardware = {SFOImager}

    Create Sensor SFOImager;
    GMAT SFOImager.FieldOfView = sqMask;
    GMAT SFOImager.FieldOfView = TDRSCone;
    GMAT SFOImager.DirectionX = 1;
    GMAT SFOImager.DirectionY = 0;
    GMAT SFOImager.DirectionZ = 0;
    GMAT SFOImager.SecondDirectionX = 0;
    GMAT SFOImager.SecondDirectionY = 1;
    GMAT SFOImager.SecondDirectionZ = 0;

    Create CustomFOV sqMask;
    GMAT sqMask.ConeAngles =  [ 45 135 225 315 ];
    GMAT sqMask.ClockAngles = [  8   8   8   8 ];

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.OccultingBodies = {Earth, Luna};
    GMAT ContactLocator1.Target = {Gateway, Artemis4};
    GMAT ContactLocator1.Filename = 'SatelliteLocator1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {SFO1, SFO2};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* Allow spacecraft observers
* Write spacecraft ephemeris kernels (SPKs) for the observer spacecraft
* Write spacecraft attitude kernels (CKs) for the observer spacecraft
* Update the report as needed

**Notes**

* One output report

    * AOS and LOS only


Case 10: AOS/LOS, Multiple Target Spacecraft, Target Station
------------------------------------------------------------

**Use case parameters**

* Two Spacecraft targets
* One Body-fixed target (on the Moon)
* Two Spacecraft observers
* Earth and Lunar occultations
* No sensor interference
* The observing spacecraft have one sensor each
    * Masked (Square, 8 deg corners, but I'll use a full mask definition)
    * For single sensor collocated with the spacecraft, we treat the spacecraft as the sensor

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft GEOSat;

    Create GroundStation MoonBaseAlpha; % Set up a target on the Moon
    GMAT MoonBaseAlpha.CentralBody = Luna;

    Create Spacecraft SFO1 SFO2;
    GMAT SFO1.AddHardware = {SFOImager}
    GMAT SFO2.AddHardware = {SFOImager}

    Create Sensor SFOImager;
    GMAT SFOImager.FieldOfView = sqMask;
    GMAT SFOImager.FieldOfView = TDRSCone;
    GMAT SFOImager.DirectionX = 1;
    GMAT SFOImager.DirectionY = 0;
    GMAT SFOImager.DirectionZ = 0;
    GMAT SFOImager.SecondDirectionX = 0;
    GMAT SFOImager.SecondDirectionY = 1;
    GMAT SFOImager.SecondDirectionZ = 0;

    Create CustomFOV sqMask;
    GMAT sqMask.ConeAngles =  [ 45 135 225 315 ];
    GMAT sqMask.ClockAngles = [  8   8   8   8 ];

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.OccultingBodies = {Earth, Luna};
    GMAT ContactLocator1.Target = {Gateway, Artemis4, MoonBaseAlpha};
    GMAT ContactLocator1.Filename = 'SatelliteLocator1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {SFO1, SFO2};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* All use case 9 additions
* Enable ground station targets

**Notes**

* One output report

    * AOS and LOS only



Case 11: AOS/LOS, Multiple Target Spacecraft, Target Station, Account for Solar Intrusion
-----------------------------------------------------------------------------------------

**Use case parameters**

* Two Spacecraft targets
* One Body-fixed target (on the Moon)
* Two Spacecraft observers
* Earth and Lunar occultations
* Solar intrusion
* No sensor interference
* The observing spacecraft have one sensor each
    * Masked (Square, 8 deg corners, but I'll use a full mask definition)
    * For single sensor collocated with the spacecraft, we treat the spacecraft as the sensor

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft GEOSat;

    Create GroundStation MoonBaseAlpha; % Set up a target on the Moon
    GMAT MoonBaseAlpha.CentralBody = Luna;

    Create Spacecraft SFO1 SFO2;
    GMAT SFO1.AddHardware = {SFOImager}
    GMAT SFO2.AddHardware = {SFOImager}

    Create Sensor SFOImager;
    GMAT SFOImager.FieldOfView = sqMask;
    GMAT SFOImager.FieldOfView = TDRSCone;
    GMAT SFOImager.DirectionX = 1;
    GMAT SFOImager.DirectionY = 0;
    GMAT SFOImager.DirectionZ = 0;
    GMAT SFOImager.SecondDirectionX = 0;
    GMAT SFOImager.SecondDirectionY = 1;
    GMAT SFOImager.SecondDirectionZ = 0;

    Create CustomFOV sqMask;
    GMAT sqMask.ConeAngles =  [ 45 135 225 315 ];
    GMAT sqMask.ClockAngles = [  8   8   8   8 ];

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.OccultingBodies = {Earth, Luna};
    GMAT ContactLocator1.IntrudingBodies = {Sun};
    GMAT ContactLocator1.IntrusionAngles = [3];
    GMAT ContactLocator1.Target = {Gateway, Artemis4, MoonBaseAlpha};
    GMAT ContactLocator1.Filename = 'SatelliteLocator1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {SFO1, SFO2};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* All use case 10 additions
* Enable intrusion processing in space based assets

**Notes**

* One output report

    * AOS and LOS only



Case 12: Occultation and Intrusion Per Sensor
---------------------------------------------

**Use case parameters**

* Two Spacecraft targets
* One Body-fixed target (on the Moon)
* Two Spacecraft observers
* Earth and Lunar occultations
* No sensor interference
* The observing spacecraft have two sensors each
    * Sensor 1: Masked (Square, 8 deg corners, rectangular), offset 0.5 m in X from the spacecraft center of mass
    * Sensor 2: Cone, 0.1 deg cone angle, offset 0.5 m in Y from the spacecraft center of mass
* Sensor 1 has no intrusion constraints
* Sensor 2 cannot measure when the sun in within 2 degrees of the boresight
* Sensor 1 is not used from spacecraft 2

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create Spacecraft GEOSat;

    Create GroundStation MoonBaseAlpha; % Set up a target on the Moon
    GMAT MoonBaseAlpha.CentralBody = Luna;

    Create Spacecraft SFO1 SFO2;
    GMAT SFO1.AddHardware = {SFOImager, SFOZoom}
    GMAT SFO2.AddHardware = {SFOImager, SFOZoom}

    Create Sensor SFOImager;
    GMAT SFOImager.FieldOfView = sqMask;
    GMAT SFOImager.FieldOfView = TDRSCone;
    GMAT SFOImager.DirectionX = 1;
    GMAT SFOImager.DirectionY = 0;
    GMAT SFOImager.DirectionZ = 0;
    GMAT SFOImager.SecondDirectionX = 0;
    GMAT SFOImager.SecondDirectionY = 1;
    GMAT SFOImager.SecondDirectionZ = 0;
    GMAT SFOImager.HWOriginInBCSX = 0.0005;
    GMAT SFOImager.HWOriginInBCSY = 0;
    GMAT SFOImager.HWOriginInBCSZ = 0;

    Create Sensor SFOZoom;
    GMAT SFOZoom.FieldOfView = sqMask;
    GMAT SFOZoom.FieldOfView = TDRSCone;
    GMAT SFOZoom.DirectionX = 1;
    GMAT SFOZoom.DirectionY = 0;
    GMAT SFOZoom.DirectionZ = 0;
    GMAT SFOZoom.SecondDirectionX = 0;
    GMAT SFOZoom.SecondDirectionY = 1;
    GMAT SFOZoom.SecondDirectionZ = 0;
    GMAT SFOZoom.HWOriginInBCSX = 0;
    GMAT SFOZoom.HWOriginInBCSY = 0.0005;
    GMAT SFOZoom.HWOriginInBCSZ = 0;

    Create CustomFOV sqMask;
    GMAT sqMask.ConeAngles =  [ 45 135 225 315 ];
    GMAT sqMask.ClockAngles = [  8   8   8   8 ];

    Create ConicalFOV zoom;
    GMAT zoom.FieldOfViewAngle = 0.1;

    Create VisibilitySet vs
    GMAT vs.Constraint = {suncon}
    GMAT vs.Sensor     = {SFO1.SFOZoom, SFO2.SFOZoom}
    GMAT vs.Targets    = {Gateway, Artemis4, MoonBaseAlpha}

    Create SensorConstraint suncon
    GMAT suncon.IntrudingBodies = {Sun}
    GMAT suncon.IntrusionAngle  = [2]
    GMAT suncon.OccultingBodies = {Earth, Moon}
    GMAT suncon.LightTime       = true

    Create ContactLocator ContactLocator1;
    GMAT ContactLocator1.OccultingBodies = {Earth, Luna};
    GMAT ContactLocator1.Target = {Gateway, Artemis4, MoonBaseAlpha};
    GMAT ContactLocator1.Filename = 'SatelliteLocator1.txt';
    GMAT ContactLocator1.InputEpochFormat = 'TAIModJulian';
    GMAT ContactLocator1.InitialEpoch = '21545';
    GMAT ContactLocator1.StepSize = 300;
    GMAT ContactLocator1.FinalEpoch = '21545.138';
    GMAT ContactLocator1.UseLightTimeDelay = true;
    GMAT ContactLocator1.UseStellarAberration = true;
    GMAT ContactLocator1.WriteReport = true;
    GMAT ContactLocator1.RunMode = Automatic;
    GMAT ContactLocator1.UseEntireInterval = true;
    GMAT ContactLocator1.Observers = {SFO1.SFOImager, SFO1.SFOZoom, SFO2.SFOZoom};
    GMAT ContactLocator1.LightTimeDirection = Transmit;

**Required GMAT Changes**

* All use case 11 additions
* Enable intrusion processing sensor by sensor using SensorConstraint objects
* Add VisibilitySet objects to apply constraints sensor by sensor
    * All settings are shown here, but these are overrides to the locator.  The scripting could be

        .. code-block:: matlab

            Create VisibilitySet vs
            GMAT vs.Constraint = {suncon}
            GMAT vs.Sensor     = {SFO1.SFOZoom, SFO2.SFOZoom}
            
            Create SensorConstraint suncon
            GMAT suncon.IntrudingBodies = {Sun}
            GMAT suncon.IntrusionAngle  = [2]


            
**Notes**

* One output report
    * AOS and LOS only
* VisibilitySets connect detailed settings to sensors.
* SensorConstraints override the settings on the locator sensor by sensor

**Ed. note:** This functionality requires fairly extensive bookkeeping, and has the feel of "round 2" coding, only implemented when it becomes needed.


Post Processing vs Real Time Processing
=======================================
The following use cases are intended provide example of real-time processing.  Everything presented above can be implemented using either SPICE or internal (real-time) processing.  These examples cannot be generated using post processing, though there may be other script based approaches to addressing these needs.

These examples focus on access calculations for spacecraft observers, but omit the complexities of masks and other details presented above.

Case 13: Maneuver While In Contact
----------------------------------

**Use case parameters**

* One ground station target
* One Spacecraft observer
* Earth occultations
* No sensor interference
* The observing spacecraft has a sensor used to send telemetry to the ground

Updated GMAT Scripting
**********************
.. code-block:: matlab

    Create GroundStation WhiteSands;

    Create Spacecraft WeatherLEO;
    GMAT WeatherLEO.AddHardware = {Telemeter}
    GMAT WeatherLEO.Attitude = NadirPointing;

    Create Sensor Telemeter;
    GMAT Telemeter.FieldOfView = TDRSCone;
    GMAT Telemeter.DirectionX = 1;     % Point the Sensor to BCS X == Nadir
    GMAT Telemeter.DirectionY = 0;
    GMAT Telemeter.DirectionZ = 0;

    % Nadir pointing basically to the whole Earth
    Create ConicalFOV TDRSCone;
    GMAT TDRSCone.FieldOfViewAngle = 90;

    Create ContactLocator Locator1;
    GMAT Locator1.OccultingBodies = {Earth};
    GMAT Locator1.Target = WhiteSands;
    GMAT Locator1.Filename = 'SatelliteLocator1.txt';
    GMAT Locator1.InputEpochFormat = 'TAIModJulian';
    GMAT Locator1.InitialEpoch = '21545';
    GMAT Locator1.StepSize = 300;
    GMAT Locator1.FinalEpoch = '21545.138';
    GMAT Locator1.UseLightTimeDelay = true;
    GMAT Locator1.UseStellarAberration = true;
    GMAT Locator1.WriteReport = true;
    GMAT Locator1.RunMode = Automatic;
    GMAT Locator1.UseEntireInterval = true;
    GMAT Locator1.Observers = {WeatherLEO};
    GMAT Locator1.LightTimeDirection = Transmit;

    BeginMissionSequence

    While WeatherLEO.BrouwerLongSMA > 6850.0;
        % Half day steps while the orbit decays
        Propagate Prop(WeatherLEO) {WeatherLEO.ElapsedDays = 0.5} 
    EndWhile

    % Raise SMA while in contact using a finite burn

    % Prop to AOS
    While WeatherLEO.BrouwerLongSMA < 6860.0;
        Propagate Prop(WeatherLEO) {WeatherLEO.Locator1.AOS}
        BeginFiniteBurn fb(WeatherLEO);
            Propagate Prop(WeatherLEO) {WeatherLEO.Locator1.LOS}
        EndFiniteBurn fb(WeatherLEO);
    EndWhile
        
    While WeatherLEO.BrouwerLongSMA > 6850.0;
        % Half day steps while the orbit decays
        Propagate Prop(WeatherLEO) {WeatherLEO.ElapsedDays = 0.5} 
    EndWhile



**Required GMAT Changes**

* Triggering the burn while in contact using a Locator object forces a paradigm shift from SPICE based post processing to internal processing
* Note that this example can be scripted using other clever means
* Further assessment is likely needed to determine the full feature set for the calculated parameters.  Here is a partial list:
    * Locator.Contact (1 when in contact of locator observers, 0 when out of contact)
    * Locator.AOS (stopping condition, triggering when Contact changes from 0 to 1)
    * Locator.LOS (stopping condition, triggering when Contact changes from 1 to 0)
* TBD: Is a Locator needed for these calculated parameters?  e.g. "Sat.Sensor1.Contact" may be what we want...
    * It may be that we only create Locators when we want reports over time spans
    * For in timeline use, we would sensors (with something like visibility sets and sensor constraints?) when we need event management in the run.


Additional use cases
--------------------

* Change attitude upon LOS in a spacecraft sensor
* Change attitude to center a target in a sensor FOV
* Maneuver in response to maneuvering of a target to maintain range
