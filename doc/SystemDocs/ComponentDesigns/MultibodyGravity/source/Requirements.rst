*******************************************
Multiple Full Field Harmonic Gravity Models
*******************************************

Overview
========
This specification describes simultaneous modeling of non-spherical gravitational fields at more than one body, as modeled in GMAT.  Spherically symmetric fields for multiple bodies are already covered in GMAT's pointMassGravity code.

Requirements
------------
GMAT's requirements for non-spherical body gravity modeling through GMAT R2020a are shown in :numref:`R2020aReqs`.

.. _R2020aReqs:
.. table:: Nonspherical Gravity Modeling Requirements

   +------------------+-------------------------------------------------------+
   | ID               | Requirement                                           |
   +==================+=======================================================+
   | FRR-13.1.1.0     | Non-spherical central body                            |
   +------------------+-------------------------------------------------------+
   | FRR-13.1.1.1     | i. Default body                                       |
   +------------------+-------------------------------------------------------+
   | FRR-13.1.1.2     | ii. User defined body                                 |
   +------------------+-------------------------------------------------------+

The table of requirements is updated for GMAT R2022a, adding multiple bodies to the modeling, as shown in :numref:`R2022aReqs`.

.. _R2022aReqs:
.. table:: Nonspherical Gravity Modeling Requirements

   +------------------+-------------------------------------------------------+
   | ID               | Requirement                                           |
   +==================+=======================================================+
   | FRR-13.1.1.0     | Non-spherical bodies                                  |
   +------------------+-------------------------------------------------------+
   | FRR-13.1.1.1     | The dynamics model central body                       |
   +------------------+-------------------------------------------------------+
   | FRR-13.1.1.2     | i. Any default solar system celestial body            |
   +------------------+-------------------------------------------------------+
   | FRR-13.1.1.3     | ii. Any user defined solar system celestial body      |
   +------------------+-------------------------------------------------------+
   | FRR-13.1.1.4     | Non-spherical bodies that are not the central body    |
   +------------------+-------------------------------------------------------+
   | FRR-13.1.2.7     | Drag is coupled with the force model central body and |
   |                  | only available when the central body applies a full   |
   |                  | field gravity model                                   |
   +------------------+-------------------------------------------------------+

The 13.1.1.4 is the only new requirement introduced in this document.  FRR 13.1.2.7 changes the coupling of the drag model from the only full field gravity force allowed in earlier GMAT versions to an association with the force model central body.  This implies that point mass central bodies now allow drag modeling.  Implementation of that extension is secondary to the initial multibody gravity modeling, and listed as a future requirement below. 

Future Requirements
-------------------
At a future date, the requirements in :numref:`FutureRequirements` may apply.

.. _FutureRequirements:
.. table:: Nonspherical Gravity Modeling, Future Requirements

   +------------------+-------------------------------------------------------+
   | ID               | Requirement                                           |
   +==================+=======================================================+
   | FRR-13.1.1.5     | Default gravity models for default bodies             |
   +------------------+-------------------------------------------------------+
   | FRR-13.1.2.8     | Drag is available for any allowed central body        |
   +------------------+-------------------------------------------------------+


Scripting
=========
Users configure non-spherical gravity modeling at multiple bodies similarly to the configuration used at the central body of a dynamics model.  An example of this scripting is show in :numref:`ScriptExample`.  The PrimaryBodies field now allows specification of more than one body.  THe properties of each configured body - that is, the degree and order of the spherical harmonics model, the size limit on the state transition matrix, the gravity model file, and the inclusion of tide modeling - can now be set for each of the gravitating bodies listed in the PrimaryBodies field. 

.. _ScriptExample:
.. code-block:: matlab
   :caption: Specifying Multiple Gravity Fields
   :linenos:

   Create ForceModel EarthMoonProp_ForceModel;
   EarthMoonProp_ForceModel.CentralBody = Earth;
   EarthMoonProp_ForceModel.PrimaryBodies = {Earth, Luna};
   EarthMoonProp_ForceModel.GravityField.Earth.Degree = 12;
   EarthMoonProp_ForceModel.GravityField.Earth.Order = 12;
   EarthMoonProp_ForceModel.GravityField.Earth.StmLimit = 100;
   EarthMoonProp_ForceModel.GravityField.Earth.PotentialFile = 'JGM2.cof';
   EarthMoonProp_ForceModel.GravityField.Earth.TideModel = 'None';
   EarthMoonProp_ForceModel.GravityField.Luna.Degree = 16;
   EarthMoonProp_ForceModel.GravityField.Luna.Order = 16;
   EarthMoonProp_ForceModel.GravityField.Luna.StmLimit = 100;
   EarthMoonProp_ForceModel.GravityField.Luna.PotentialFile = 'LP165P.cof';
   EarthMoonProp_ForceModel.GravityField.Luna.TideModel = 'None';

The default settings for these fields is show in :numref:`ModelDefaults`.

.. _ModelDefaults:
.. table:: Nonspherical Gravity Model Default Settings

   +------------------+---------------------------+
   | Field            | Default Value             |
   +==================+===========================+
   | Degree           | 4                         |
   +------------------+---------------------------+
   | Order            | 4                         |
   +------------------+---------------------------+
   | StmLimit         | 100                       |
   +------------------+---------------------------+
   | PotentialFile    | | JGM2.cof (if Earth)     |
   |                  | | Unset (others)          |
   +------------------+---------------------------+
   | TideModel        | None                      |
   +------------------+---------------------------+
