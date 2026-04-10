************
Introduction
************
GMAT users need the ability to calculate the effects of environmental and spacecraft induced forces on the components of a modeled spacecraft.  More specifically, GMAT users need the system to model the mass distribution and moments of inertia of a spacecraft, and the effects of environmental forces and maneuvers on the spacecraft hardware.  The needed modeling provides information about the torques affecting the vehicle, a key input for modeling the angular momentum accumulation during the simulation of a mission.

The GMAT spacecraft model is built as a central spacecraft object with hardware additions that are used to represent tanks, thrusters, sensors, and communications equipment.  The hardware components contain data structures for the location and orientation of each hardware element, along with physical properties needed to model each component relative to a spacecraft body based origin and axis system [#f1]_ .  A recent addition to the GMAT spacecraft model is the ability to represent the spacecraft surfaces as a collection of flat plates with specified reflection coefficients for the purposes of solar radiation pressure modeling. These elements are used in the design to determine the moment arms for the torques, and, for the plate model, the specular properties of each plate for the force modeling.

GMAT provides orbit modeling capabilities for control and planning for orbital maneuvers, but does not model the dynamics of a vehicle's attitude.  The forces modeled in GMAT contain most of the information needed to compute torques induced by those forces. The GMAT force model code is updated, in this design specification, to use the hardware and plate model properties for the evaluation of torques applied to the vehicle for specific forces needed by the GOES-R team.  Other forces that would benefit from this addition are identified as future requirements.

.. [#f1] The location and orientation data in the GMAT spacecraft model has only been used lightly, and needs to be tested rigorously (at this writing - remove note when done).

Requirements
============

Torque Modeling Requirements
----------------------------
Torque modeling in GMAT is built around the core definition

 .. math::

    \boldsymbol{\tau} = \mathbf{r} \times \mathbf{F}

The model in GMAT is built around the vector **r** from the body coordinate system origin to the location of an applied force, **F**.  The model in GMAT is built component by component from the collection of hardware elements specified in the user's script, summing the elements to build the accumulated torque from the core spacecraft through all n attached hardware elements:

 .. math::
    :label: rcrossF

    \boldsymbol{\tau}_T = \sum_{i=0}^{n}\mathbf{r}_i \times \mathbf{F}_i

The vector **r** for extended components is the vector to the "center of force" used for its applied torque.  Element 0 in this expression provides for modeling of the spacecraft as a whole, separate from modeling the torque as a collection of attached components. For a plate based spacecraft model, the "0" element defaults to the case where the center of mass is coincident with the origin used for torque modeling (that is, :math:`r_0 = 0.0`), and therefore provides no contribution to the total torque.

GMAT uses additions to the code implemented specifically for torque modeling.  The requirements below identify the necessary additions to the system, first as needed to support GOES, followed by the requirements that may be addressed later to make the implementation more general purpose.

GOES Specific Requirements
""""""""""""""""""""""""""
**Hardware Models**

#. GMAT Spacecraft objects model the following properties:

   A. The center of mass of the spacecraft.

      #. The center of mass is specified in a body fixed coordinate system
      #. The center of mass accounts for the following hardware components added to the spacecraft

         a. Fuel tanks

      #. The remaining mass sources are accumulated into a single spacecraft center of mass representation

   #. The moment of inertial of the spacecraft

      #. The moment of inertial tensor has its principle axes aligned with the spacecraft's body-fixed coordinate system
      #. The total moment of inertia accumulates the moments from the core spacecraft component and attached hardware.

         a. The spacecraft specifies a core moment of inertia tensor.
         #. Moments of inertial are accumulated from hardware elements that set individual moments.

            * Attached hardware specifies moments of inertia in a system aligned with the axes of the spacecraft's body-fixed coordinate system. 
            * The hardware moment of inertia is referenced to an origin specified in spacecraft body coordinates.

         #. The total moment of inertia is calculated using the parallel axis theorem and the moments of the hardware elements specifying individual tensors.
         #. Specification is a moment of inertia tensor is an option on the spacecraft hardware.  (If a hardware element does not specify an inertia tensor, its moment is included in the core inertia tensor.)

   #. The spacecraft subsystem includes moment arms for torque modeling from the following sources:

      #. The following hardware element moment arms:

         #. Thrusters

      #. The following geometry based moment arms:

         #. The plate model used for solar radiation pressure modeling

**Torque Modeling**

#. GMAT models torques applied to a spacecraft.

   A. Torque modeling is activated using GMAT scripting.
   #. The torque is modeled with respect to the spacecraft body coordinate system.

#. Torques are modeled for the following forces:

   A. Solar Radiation Pressure (SRP)

      #. SRP using the n-plate model

   #. Gravitational fields
      
      #. Gravity gradient torques are modeled for the central body of the force model
      #. TBD: Inclusion of other gravity bodies in the force model

   #. Thrust vectors during maneuvers

#. GMAT reports torque data for use by users.

   A. Torques are reported for the following elements

      #. The total computed torque acting on the spacecraft.
      #. The torque from individual torque-enabled force model components
      #. The aggregate torque from all active spacecraft thrusters during a maneuver
      #. The torque from each spacecraft thruster active during a maneuver

   #. Torques are reported in the following reference frames:

      #. The body coordinate system.  This is the default setting.
      #. Any of GMAT's inertial coordinate systems

   #. Torques are computed with respect to the spacecraft's TBD.  (BCS origin?  CoM?  CoG?)

Future Requirements
"""""""""""""""""""

**Hardware Model Updates**

TBD, but including the following:

#. The spacecraft subsystem includes moment arms for torque modeling from the following sources:

   #. The following geometry based moment arms:

      #. The SPAD model used for drag and SRP modeling
      #. The plate model used for drag modeling


**Torque Modeling**

TBD, but including the following:

#. Torques will be modeled for the following additional forces:

   #. Atmospheric drag
   #. Solar Radiation Pressure (SRP)

      #. SRP using the SPAD model
      #. SRP using a box-wing model

#. GMAT reports torque data for use by users.

   A. Torques are reported for the following elements

      #. The torque from spacecraft hardware elements


Angular Momentum Modeling Requirements
--------------------------------------

TBD