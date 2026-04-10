Introduction
============

.. note::

   This document is written in present tense, as if the specified features exist.  This is a design document describing the feature and its implementation plan.  The feature does not exist in its entirety.  Once implemented, this document will be updated and this notice removed.

**Nomenclature TBD** *Sensor?  Imager?  Something else?  Using "Sensor" precludes use of that term for other telemetry providers - e.g., temperature, pressure, and wheel rate measuring hardware components.  We need to settle on a term here.*

Spacecraft and ground station objects configured in GMAT can have sensor objects attached to them.  A sensor is used to report viewing properties between a the sensor and other physical objects in the GMAT simulation.  This document presents the requirements and high level implementation for that feature.

.. topic:: Nomenclature
  
   The following terms are used in this document:

   .. glossary::

      Acquisition of Signal (AOS)
         The start of a pass. 

      Field of View (FoV)
         The maximum (angular) area of a sample that a sensor can detect.

      GMAT
         The General Mission Analysis Tool.

      Loss of Signal (LOS)
         The end of a pass. 

      Mission Control Sequence
         See Timeline

      Pass
         The period in which an object can be observed by a sensor.

      Sensor
         A hardware component that has a location and field of view specification.  In GMAT, sensors can be placed on spacecraft or on ground station objects.

      Simulation
         The objects and mission timeline used in a GMAT run.

      Range
         The distance between a sensor and an external object

      Resolution
         The angular size that can be resolved by the sensor.  This is basically the angular size of a pixel for an imaging sensor, for example.

      Target
         An object that is identified for viewing by a sensor.

      Timeline
         The sequence of commands used to run a simulation.  The terms "Control Sequence, "Mission Control Sequence," and "Timeline" are used interchangeably in this document.


Requirements
============

Sensor Objects
--------------
These requirements define GMAT's Sensor class objects.

#. Users can define sensors

   A. GMAT scripting supports sensor definitions   
   #. Sensors are scripted as hardware objects  
      
      #. The same core object can be attached to multiple participants
      #. Attached objects are cloned to the owning participant
      #. Scripting for sensors is similar to tank and thruster scripting
      
#. Sensors can be attached to the following object types:      
   
   A. Spacecraft  
   #. GroundStation

#. Sensors set their pointing direction along a boresight vector defined in the owning object's reference frame.

   A. The boresight for a spacecraft is defined in the spacecraft body coordinate system (BCS).

      #. The spacecraft's attitude is used to rotate from the BCS frame to the physical orientation of the spacecraft and sensor.  The default direction points in the BCS X-axis direction.

   #. The boresight for a ground station points in the topocentric nadir direction.

#. Sensors define an "up" direction orthogonal to the boresight for asymmetric fields of view.

   A. The "up" direction need not be set with an orthogonal vector definition.  

      #. The up direction is computed by GMAT as the projection of the user set direction into the plane orthogonal to the boresight direction.
      #. The up direction cannot be parallel to the boresight direction.

   #. The up direction for a ground station points north.  

      #. If the station is at a pole, it points along the prime meridian, in the direction of the station central body's 180 degree meridian for the north pole and in the direction of the prime meridian for the south pole.

   #. The up direction for a spacecraft points towards the BCS Z-axis by default.
      
#. Sensors define their field of view as one of the following:
   
   A. Omnidirectional  (TBD: Do we want/need this?)
   #. Restricted to a cone half-angle measured from the boresight.
   #. Restricted to a rectangular region, defined by width and height half-angles.
   #. Restricted to a mask, set by azimuth-elevation pairs.

      #. The elevation angle is measured with respect to the boresight vector.
      #. The azimuth angle is measured with respect to the sensor's "up" direction.
      #. The elevation angle between adjacent azimuth angles is linearly interpolated as a function of azimuth.

#. Sensor access occurs between a sensor and one of the following objects    
   
   A. The center of any SpacePoint  

      #. Celestial Bodies: Stars, Planets, Moons, Asteroids, Comets
      #. TBD: Barycenters, Lagrange Points
   
   #. Simulation participants:   
      
      #. Spacecraft
      #. Groundstations
      #. TBD: Formations
      #. Other sensors
      
#. Sensors have the following range options:    
   
   A. Infinite range – any object in the unobstructed field of view can be seen  
   #. Range limits

      #. Minimum range
      #. Maximum range

   #. Range set by the distance to another object  
      
#. Sensors point along a boresight, set based on the owning object type:      
   
   A. Sensors on spacecraft point relative to the spacecraft’s attitude coordinate system 
   #. Sensors on ground stations point relative to the zenith direction of the topocentric coordinates of the ground station  
      
#. Sensor orientation is specified based on a second orthogonal direction:    
   
   A. The second direction for spacecraft is the secondary direction for the attitude system 
   #. The second direction for ground stations is the northward pointing direction perpendicular to the zenith direction   
      
#. The following extended objects can interfere with a sensor:

   A. Planets
   #. Moons
   #. Sun
   #. Asteroids
   #. Comets

#. Users identify potentially interfering objects as part of the configuration of the simulation.
      
#. Sensors can be turned on and off as a mission sequence is run     
      
#. Sensor pointing and orientation can be changed as the mission sequence is run

   A. Pointing is changed for a spacecraft by changing the attitude of the spacecraft.
   #. The pointing direction is fixed sensors associated with a ground stations. 

#. Multiple Sensors can be operated simultaneously.
      
#. Sensor masking cannot be changed during a mission sequence run


Sensor Usage Requirements
-------------------------
These requirements define how a Sensor functions when used in GMAT.

#. Users can configure access computations and reporting in GMAT.

#. Access computations are performed as the control sequence executes.

.. note::
   Run time computation as required here allows scripted actions based on accesses.  This approach is different than the current SPICE-based post-processing used in GMAT for station contacts and shadow entry and exit.

3. The access data includes the following values

   A. Acquisition of signal (AOS), the epoch of the first contact between the sensor and the object
   #. Loss of signal (LOS), the epoch of at which the object leaves the sensor's field of view
   #. AOS and LOS times are reported based on the following features of the field of view boundary:

      #. Angle constraints for the field of view
      #. Minimum and maximum range constraints on the field of view.
      #. Interference from other objects modeled in the simulation.

   #. Pointing directions from the sensor to the object

      #. Pointing directions are reported as the azimuth and elevation of the viewed object in the sensor's field of view.

   #. Ranges to the observed objects

#. Access accounts for the following corrections

   A. Light travel time

#. Users can select to include or omit the access corrections

#. Users can generate reports for the following data:

   A. Pass start (AOS) and end (LOS) times
   #. Pointing directions to the AOS point on a pass
   #. Pointing directions to the LOS point on a pass
   #. Intermediate pass data:

      #. Epoch and pointing directions at fixed intervals for a pass
      #. Sensor to object ranges 
      #. Duration of a pass, defined at the time from AOS to LOS
      #. The epoch and maximum elevation/minimum cone angle of an object passing though the field of view.


Future Requirements
===================
The requirements identified as future requirements add to the requirements listed above.

Sensor Objects
--------------

#. Sensors set their pointing direction along a boresight vector defined in the owning object's reference frame.  The heading element is listed to help identify where the update occurs.

   A. The boresight for a ground station defaults to the topocentric nadir direction.
   #. Users can override the boresight direction for ground stations.

#. Sensors define their field of view as one of the following:

   #. Restricted to a complex angle defined by an inner and outer angle and a min and max clock angle
   #. Restricted to a predefined file format, readable text in one of the above definitions.

#. Sensors can be obstructed by the following extended objects    

   A. Other spacecraft
   #. Spacecraft appendages

#. Sensor access occurs between a sensor and one of the following objects

   #. Center of defined areas (e.g. lunar crater, lunar landing site, etc)

#. Sensor parameters (e.g. az, el, range or other representation of the vector line of sight from the sensor to the object/area) shall calculate rates for tracking of the object

#. Sensor pointing and orientation can be changed as the mission sequence is run

   A. Sensors are capable of articulation with the sensor boresight articulation based on computed direction rates.
   #. Pointing is fixed sensors associated with a ground stations.

#. The sensor field-of-view can be zoomed.

Sensor Usage Requirements
-------------------------

#. The access data include the following values


   #. Users can generate reports for the following data:
      
      #. Azimuth and elevation and range data along with parameters that represent RF or visual data.

   #. All sensor data shall be available to the GMAT navigation capability for use as measurement data.
   #. All sensor data shall be computed in a real-time mode.

#. Access accounts for the following affects

   A. Aberration

#. Access reporting for optical sensors includes the following reportable data:

   A. Resolution, reported as pixel size, for observed objects.
   #. Additional parameters such as spectrum (color) or a variation in object's visual magnitude
   #. For these data, users supply the input data needed for the computation.

      #. For resolution computations, the pixel count for the sensor's field of view.
      #. For spectral properties, the observed object's spectral characteristics.
