Implementation Plan
===================
The bulk of the work required for GMAT to support sensor access feature is documented in the design chapter of this document.  This chapter addresses the implementation approach to build that design.

* Before implementation can begin, the GMAT development team needs to evaluate the options presented in :ref:`CodeOptions` and decide on the implementation approach for sensor access reporting in GMAT.
* Known issues that affect sensor access data generation need to be addressed.  One in particular - the limited support in GMAT for attitude control - is critical for successful implementation of access computations for sensors on spacecraft.

The basic implementation plan recommended here is to proceed by implementing core components, unit testing those elements, and to add complexity over time, building up capability as the process proceeds.  The Sensor cod is complex enough that is should be developed on a separate repository branch.  

The detailed development plan is the purview of the development team.  Were Thinking Systems to develop the code, the order of development would be planned to address ground station based needs first, and then spacecraft based needs, through the following development cycles [1]_\ :

#. **Design Review** Evaluate the design and select the implementation option.

	At this writing, it appears likely that option 1 will be selected for the ground station contact reporting.  Initial development will add sensors and sensor masking to the current SPICE based ContactLocator code, making that post processing reporting available earlier than any Spacecraft based sensor access code. 

	The decision about spacecraft based sensor reporting is not as clear at this writing.  THe remainder of this plan is based on a post-processing approach across the board.

#. **Minimum Viable Ground Station** Focus initially on building the minimum viable product for ground station based contact reporting.

	#. Review the current implementation of the ContactLocator and IntrusionLocator code.
	#. Implement scripting of Sensor objects.  (Note that for the GOES mission, there is an "Imager" object in the code already that provides the basis of a GMAT sensor.  I recommend using this object, scripted as a "Sensor,"" for the core object here.)
	#. Determine the complete set of kernels that must be generated for mask based sensor access reporting for a sensor collocated with a ground station (no position offset).
	#. Add code to the ContactLocator to generate the resulting instrument kernels (IK) using the settings from a GMAT sensor.  (The frame kernels - FK files - are likely to already exist.)
	#. Code any additional kernels required.  (At this writing, I think all that is needed is the IK coding for mask based contact processing using collocated sensor/ground station settings.)
	#. Test the current contact locators with station masks.
	#. Add the code to report the internal pass points to contact reports.
	#. This completes the initial development cycle.  Merge into the Dev branch and continue development on the sensor branch.

#. **Minimum Tested Ground Station Product** Enhance the ground station code to address system consistency and user feedback.

	#. Update the implementation to ensure that GMAT's visualization and computational components present a consistent view of contact events.
	#. TBD: Add code to address sensors offset from the station location.
	#. Compare event computations reported in GMAT with other systems, and correct any bugs uncovered from this testing.
	#. Incorporate the contact reporting updates into the test system.
	#. Address bugs uncovered by users requesting the feature.
	#. This completes the initial development update cycle.  Merge into the Dev branch and continue development on the sensor branch.

#. **Usage Settings** Add the sensor usage configuration objects to GMAT.
	
	The initial coding, above, makes use of the current ContactLocator configuration to identify multiple stations all seeking to view a single spacecraft.  The current code generates occulting events based on the bodies in the force model, and does not account for intrusion of external bodies - primarily the Sun - into the station's conical field of view.  This development cycle enhances the code above to account for those factors through implementation of the SensorConstraint class.  In the absence of a SensorConstraint object, the sensor defaults to the current GMAT settings.

	This cycle may also implement the VisibilitySet class, which enables many-to-mane sensor contacts.  If the VisibilitySet is coded at this time, the initial coding only needs to account for spacecraft targets.  There is no current requirement for ground station to general space point objects at this point in the development.

#. **Spacecraft Prep** If needed, address known issues in GMAT related to attitude so that spacecraft sensor accesses can be added to the system.  (This may be deferrable.  The current code allows for sensor pointing in the spacecraft's BCS, and it allows for attitude control for nadir-pointing spacecraft, for spacecraft locked to the axis system of a coordinate system, and for attitude history file based attitudes.  It does not allow for attitude control of a spacecraft in any coordinate system because the BCS to external coordinate system rotation matrix is either missing or not scriptable.)

#. **Design Decision Checkpoint** The plan presented above all builds on the current SPICE based ContactLocator code, and produces a functional post-processing station contact locator.  Note that it does not allow for run time contact usage.  For example, if maneuvers need to be performed during station contact, the code developed here does not accommodate that need.  GMAT scripting may be able to address it, though not as elegantly.  The plan below assumes that the post-processing approach is selected for the remainder of the development.  It will require updating if hte internal development option is selected.

#. **Minimum Viable Spacecraft** This cycle implements the initial set of spacecraft to outside object access computations.  The focus on the first cycle is spacecraft-to-spacecraft contact calculations.

	#. Review the current station sensor code developed above, and identify and address any issues with using that approach for spacecraft sensors.
	#. Implement spacecraft usage of sensor objects.  This is likely large functional already using the "AddHardware" field on a Spacecraft.
	#. Implement the spacecraft-spacecraft features of the VisibilitySet class.  The focus in this initial code is handling of multiple target spacecraft, so a single sensor may be used initially.
	#. Identify all needed SPICE kernels for sensors with masks on a spacecraft.  (This will likely require IK, FK, and CK files as noted in the design.)
	#. Code and test the new kernel writers.
	#. Code the kernel writers for target spacecraft identified in the scripting.
	#. Test the updated contact locator.
	#. Add code to report spacecraft viewing of target spacecraft.

	#. This completes the initial spacecraft access development cycle.  Merge into the Dev branch and continue development on the sensor branch.

#. **Minimum Tested Spacecraft Product** Enhance the ground station code to address system consistency and user feedback.

	#. Update the implementation to ensure that GMAT's visualization and computational components present a consistent view of contact events.
	#. Compare event computations reported in GMAT with other systems, and correct any bugs uncovered from this testing.
	#. Incorporate the contact reporting updates into the test system.
	#. Address bugs uncovered by users requesting the feature.
	#. This completes the initial spacecraft access development update cycle.  Merge into the Dev branch and continue development on the sensor branch.

#. **Contact Reporting Code Completion** This cycle completes coding for the spacecraft to outside object access computations by adding support for other space point objects and sensor constraints.

	#. Identify the kernels needed for reporting access to celestial bodies, stations, and other space points identified in the requirements.
	#. Code the kernel writers for target objects identified in the scripting.
	#. Add code to handle the sensor constraint settings to the spacecraft access code.
	#. Implement the loops needed to complete the ViibilitySet access computations.
	#. Test the updated contact locator.
	#. Add code to report spacecraft viewing of all target objects.
	#. This completes the spacecraft access development cycle.  Merge into the Dev branch and continue development on the sensor branch.

#. **Initial Complete Product** Enhance the ground station code to address system consistency and user feedback.

	#. Update the implementation to ensure that GMAT's visualization and computational components present a consistent view of contact events.
	#. Compare event computations reported in GMAT with other systems, and correct any bugs uncovered from this testing.
	#. Incorporate the contact reporting updates into the test system.
	#. Address bugs uncovered by users requesting the feature.
	#. This completes the feature development.  Merge into the Dev branch and delete the sensor branch from the repository.

Note that the steps described above are an outline of the option 1 development process.  "No plan survives contact with the enemy."  (Attributed to Helmuth von Moltke)

.. rubric:: Footnotes

.. [1] These may be developed as Agile sprints.  I hesitate to use that language since the cycles here are likely of uneven duration.

Implementation Notes
--------------------

* GMAT's current attitude modeling supports attitudes fixed in alignment with a coordinate system's axis system, nadir pointing attitude systems, spinning spacecraft, and attitude history file representations.  This may not be sufficient for the new capabilities because it does not allow for transformations from the spacecraft's body fixed coordinate system into a general GMAT coordinate frame.  The attitude system must be assessed to ensure that it supports the needs of the users of the new feature.

* Reporting of space point access from a Spacecraft using the SPICE based approach requires the ability to write attitude kernel files.  This may be the opportunity to address attitude output in general, writing the attitude file types that GMAT can read.

* Users working an an HPC environment encounter sporadic crashes in GMAT because of the time based temporary kernel naming scheme implemented in the current EventLocator subsystem.  That code should be changed to use a platform independent globally unique identifier scheme. 