.. _EMTGSpacecraftFileSpec-label:

EMTG Spacecraft File Specification
===============================================

This section describes the contents of an EMTG spacecraft file. The descriptions are given in the tables in this section and are organized by the spacecraft file block, the line number within a block, and the variable number within a line. It is highly recommended that this file specification be used in conjunction with :ref:`Sec_GMATOC_FormatOfEMTGSpacecraftFile` and the example .emtg\_spacecraft files provided in gmat/data/emtg/.

Several symbols and acronyms are used in the tables in this section: :math:`P` is power; :math:`P_0` is base power delivered by a solar array; :math:`r_s` is the distance from the spacecraft to the Sun; Isp is specific impulse; CSI is constant specific impulse; and VSI is variable specific impulse.

Spacecraft Block
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section describes the contents of the Spacecraft Block of an EMTG spacecraft file.

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Spacecraft_Line1_Entry1:
.. list-table:: EMTG spacecraft file specification for Spacecraft Block, line 1, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - name
    * - Variable number
      - 1
    * - Variable description
      - N/A
    * - Data type
      - string
    * - Units
      - N/A
    * - Value restrictions
      - none
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Spacecraft_Line2_Entry1:
.. list-table:: EMTG spacecraft file specification for Spacecraft Block, line 2, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 2
    * - Line name
      - EnableGlobalElectricPropellantTankConstraint
    * - Variable number
      - 1
    * - Variable description
      - N/A
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - 0 or 1
    * - Other
      - boolean integer

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Spacecraft_Line3_Entry1:
.. list-table:: EMTG spacecraft file specification for Spacecraft Block, line 3, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 3
    * - Line name
      - EnableGlobalChemicalPropellantTankConstraint
    * - Variable number
      - 1
    * - Variable description
      - N/A
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - 0 or 1
    * - Other
      - boolean integer

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Spacecraft_Line4_Entry1:
.. list-table:: EMTG spacecraft file specification for Spacecraft Block, line 4, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 4
    * - Line name
      - EnableGlobalDryMassConstraint
    * - Variable number
      - 1
    * - Variable description
      - N/A
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - 0 or 1
    * - Other
      - boolean integer

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Spacecraft_Line5_Entry1:
.. list-table:: EMTG spacecraft file specification for Spacecraft Block, line 5, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 5
    * - Line name
      - GlobalElectricPropellantTankCapacity
    * - Variable number
      - 1
    * - Variable description
      - N/A
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Spacecraft_Line6_Entry1:
.. list-table:: EMTG spacecraft file specification for Spacecraft Block, line 6, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 6
    * - Line name
      - GlobalFuelTankCapacity
    * - Variable number
      - 1
    * - Variable description
      - N/A
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Spacecraft_Line7_Entry1:
.. list-table:: EMTG spacecraft file specification for Spacecraft Block, line 7, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 7
    * - Line name
      - GlobalOxidizerTankCapacity
    * - Variable number
      - 1
    * - Variable description
      - N/A
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Spacecraft_Line8_Entry1:
.. list-table:: EMTG spacecraft file specification for Spacecraft Block, line 8, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 8
    * - Line name
      - GlobalDryMassBounds
    * - Variable number
      - 1
    * - Variable description
      - Lower bound on global dry mass
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`; :math:`\leq` GlobalDryMassBounds[2]
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Spacecraft_Line8_Entry2:
.. list-table:: EMTG spacecraft file specification for Spacecraft Block, line 8, entry 2.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 8
    * - Line name
      - GlobalDryMassBounds
    * - Variable number
      - 2
    * - Variable description
      - Upper bound on global dry mass
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq` GlobalDryMassBounds[1]
    * - Other
      - N/A

|

Stage Block
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section describes the contents of the Stage Block of an EMTG spacecraft file.

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line1_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 1, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - name
    * - Variable number
      - 1
    * - Variable description
      - Name of the stage
    * - Data type
      - string
    * - Units
      - N/A
    * - Value restrictions
      - none
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line2_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 2, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 2
    * - Line name
      - BaseDryMass
    * - Variable number
      - 1
    * - Variable description
      - Dry mass of the propulsion system base
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line3_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 3, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 3
    * - Line name
      - AdapterMass
    * - Variable number
      - 1
    * - Variable description
      - Mass of the spacecraft-to-power-system adapter
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line4_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 4, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 4
    * - Line name
      - EnableElectricPropellantTankConstraint
    * - Variable number
      - 1
    * - Variable description
      - Is the constraint on the electric propellant tank capacity enabled?
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - 0 or 1
    * - Other
      - boolean integer

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line5_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 5, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 5
    * - Line name
      - EnableChemicalPropellantTankConstraint
    * - Variable number
      - 1
    * - Variable description
      - Is the constraint on the chemical propellant tank capacity enabled?
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - 0 or 1
    * - Other
      - boolean integer

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line6_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 6, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 6
    * - Line name
      - EnableDryMassConstraint
    * - Variable number
      - 1
    * - Variable description
      - Is the constraint on dry mass enabled?
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - 0 or 1
    * - Other
      - boolean integer

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line7_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 7, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 7
    * - Line name
      - ElectricPropellantTankCapacity
    * - Variable number
      - 1
    * - Variable description
      - Maximum electric propellant mass
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - Only enforced if EnableElectricPropellantTankConstraint == 1

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line8_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 8, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 8
    * - Line name
      - ChemicalFuelTankCapacity
    * - Variable number
      - 1
    * - Variable description
      - Maximum chemical fuel mass
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - Only enforced if EnableChemicalPropellantTankConstraint == 1

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line9_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 9, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 9
    * - Line name
      - ChemicalOxidizerTankCapacity
    * - Variable number
      - 1
    * - Variable description
      - Maximum oxidizer mass
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - Only enforced if EnableChemicalPropellantTankConstraint == 1

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line10_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 10, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 10
    * - Line name
      - ThrottleLogic
    * - Variable number
      - 1
    * - Variable description
      - How should the number of thrusters be selected?
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - 1 or 2
    * - Other
      - 1: minimum number of thrusters

        2: maximum number of thrusters

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line11_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 11, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 11
    * - Line name
      - ThrottleSharpness
    * - Variable number
      - 1
    * - Variable description
      - Determines how smooth or sharp the transition between throttle levels is
    * - Data type
      - real
    * - Units
      - N/A
    * - Value restrictions
      - :math:`> 0`
    * - Other
      - The Heaviside step function is analytically approximated using :math:`H = \frac{1}{1 - \exp(-2 k x)}`, :math:`k =` ThrottleSharpness.

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line12_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 12, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 12
    * - Line name
      - PowerSystem
    * - Variable number
      - 1
    * - Variable description
      - Name of power system for stage
    * - Data type
      - string
    * - Units
      - N/A
    * - Value restrictions
      - Must be a valid name in the stage's Power Library block
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line13_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 13, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 13
    * - Line name
      - ElectricPropulsionSystem
    * - Variable number
      - 1
    * - Variable description
      - Name of electric propulsion system for stage
    * - Data type
      - string
    * - Units
      - N/A
    * - Value restrictions
      - Must be a valid name in the stage's Propulsion Library block
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Stage_Line14_Entry1:
.. list-table:: EMTG spacecraft file specification for Stage Block, line 14, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 14
    * - Line name
      - ChemicalPropulsionSystem
    * - Variable number
      - 1
    * - Variable description
      - Name of chemical propulsion system for stage
    * - Data type
      - string
    * - Units
      - N/A
    * - Value restrictions
      - Must be a valid name in the stage's Propulsion Library block
    * - Other
      - N/A

|

Power Library Block
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section describes the contents of the Power Library Block of an EMTG spacecraft file.



Multiple power systems may be added to the power library by adding additional lines that contain all elements specified in line 1.

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry1:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 1
    * - Variable description
      - name for this power system being specified by that line
    * - Data type
      - string
    * - Units
      - N/A
    * - Value restrictions
      - none
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry2:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 2.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 2
    * - Variable description
      - power supply type
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - [0, 1]
    * - Other
      - 0: constant; 1: solar

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry3:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 3.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 3
    * - Variable description
      - power supply curve type
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - [0, 1]
    * - Other
      - 0: Sauer: :math:`P_{Generated} = \frac{P_0}{r_s^2} \left( \frac{\gamma_0 + \gamma_1 / r_s + \gamma_2 / r_s^2}{1 + \gamma_3 r_s + \gamma_4 r_s^2} \right)`

         1: polynomial: :math:`P_{Generated} = \frac{P_0}{r_s^2} \sum_{i=0}^6 r_s^i`

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry4:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 4.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 4
    * - Variable description
      - bus power type
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - [0, 1]
    * - Other
      - 0: type A quadratic: P\_Bus = bus\_power[0] + bus\_power[1] / r_s^1 + bus\_power[2] / r_s^2 

        1: type B conditional: If P\_Provided > bus\_power[0], then P\_Bus = bus\_power[0]. Else, P\_Bus = bus\_power[0] + bus\_power[1] * (bus\_power[2] - P\_provided)

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry5:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 5.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 5
    * - Variable description
      - P0
    * - Data type
      - real
    * - Units
      - kW
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - Base power

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry6:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 6.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 6
    * - Variable description
      - mass per kW
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry7:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 7.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 7
    * - Variable description
      - decay rate
    * - Data type
      - real
    * - Units
      - 1/years
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - rate of power decay over time: :math:`P_{generated} = P_{generated} \exp(-\texttt{decay\_rate}  \cdot t)`, with t in years

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry8:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 8.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 8
    * - Variable description
      - gamma[0]
    * - Data type
      - real
    * - Units
      - assumes r\_s is in km and P is in kW
    * - Value restrictions
      - N/A
    * - Other
      - Power delivered by solar array. 

        If power supply curve type == 0: numerator coefficient of r\_s^0.

        If power supply curve type == 1: coefficient of r\_s^{-2}

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry9:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 9.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 9
    * - Variable description
      - gamma[1]
    * - Data type
      - real
    * - Units
      - assumes r\_s is in au and P is in kW
    * - Value restrictions
      - N/A
    * - Other
      - Power delivered by solar array.

        If power supply curve type == 0: numerator coefficient of r\_s^1.

        If power supply curve type == 1: coefficient of r\_s^{-1}

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry10:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 10.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 10
    * - Variable description
      - gamma[2]
    * - Data type
      - real
    * - Units
      - assumes r\_s is in au and P is in kW
    * - Value restrictions
      - N/A
    * - Other
      - Power delivered by solar array.

        If power supply curve type == 0: numerator coefficient of r\_s^2.

        If power supply curve type == 1: coefficient of r\_s^0

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry11:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 11.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 11
    * - Variable description
      - gamma[3]
    * - Data type
      - real
    * - Units
      - assumes r\_s is in au and P is in kW
    * - Value restrictions
      - N/A
    * - Other
      - Power delivered by solar array.

        If power supply curve type == 0: denominator coefficient of r\_s^1.

        If power supply curve type == 1: coefficient of r\_s^1

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry12:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 12.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 12
    * - Variable description
      - gamma[4]
    * - Data type
      - real
    * - Units
      - assumes r\_s is in au and P is in kW
    * - Value restrictions
      - N/A
    * - Other
      - Power delivered by solar array.

        If power supply curve type == 0: denominator coefficient of r\_s^2.

        If power supply curve type == 1: coefficient of r\_s^2

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry13:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 13.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 13
    * - Variable description
      - gamma[5]
    * - Data type
      - real
    * - Units
      - assumes r\_s is in au and P is in kW
    * - Value restrictions
      - N/A
    * - Other
      - Power delivered by solar array.

        If power supply curve type == 1: coefficient of r\_s^3

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry14:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 14.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 14
    * - Variable description
      - gamma[6]
    * - Data type
      - real
    * - Units
      - assumes r\_s is in au and P is in kW
    * - Value restrictions
      - N/A
    * - Other
      - Power delivered by solar array. If power supply curve type == 1: coefficient of r\_s^4

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry15:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 15.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 15
    * - Variable description
      - bus power[0]
    * - Data type
      - real
    * - Units
      - N/A
    * - Value restrictions
      - N/A
    * - Other
      - If bus power type == 0: Power required by spacecraft bus: coefficient of r\_s^0.

        If bus power type == 1, see note on bus power type.

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry16:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 16.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 16
    * - Variable description
      - bus power[1]
    * - Data type
      - real
    * - Units
      - N/A
    * - Value restrictions
      - N/A
    * - Other
      - If bus power type == 0: Power required by spacecraft bus: coefficient of r\_s^1.

        If bus power type == 1, see note on bus power type.

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Power_Line1_Entry17:
.. list-table:: EMTG spacecraft file specification for Power Library Block, line 1, entry 17.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 17
    * - Variable description
      - bus power[2]
    * - Data type
      - real
    * - Units
      - N/A
    * - Value restrictions
      - N/A
    * - Other
      - If bus power type == 0: Power required by spacecraft bus: coefficient of r\_s^2.

        If bus power type == 1, see note on bus power type.

|

Propulsion Library Block
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section describes the contents of the Propulsion Library Block of an EMTG spacecraft file.



Multiple propulsion systems may be added to the propulsion library by adding additional lines that contain all elements specified in line 1.

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry1:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 1.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 1
    * - Variable description
      - name for this power system being specified by that line
    * - Data type
      - string
    * - Units
      - N/A
    * - Value restrictions
      - none
    * - Other
      - N/A

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry2:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 2.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 2
    * - Variable description
      - thruster mode
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - [0,1,3,4,5]
    * - Other
      - 0: constant thrust and Isp

        1: fixed efficiency CSI

        3: polynomial 1D

        4: stepped H thrust 1D

        5: stepped L mdot 1D.

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry3:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 3.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 3
    * - Variable description
      - throttle table file
    * - Data type
      - string
    * - Units
      - N/A
    * - Value restrictions
      - "none" or  path to external *.ThrottleTable file, relative to GMAT executable location
    * - Other
      - If an external throttle table is to be used, the relevant file is specified here. Otherwise, use "none". If an external table is provided, it overrides local thrust coefficients and power coefficients

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry4:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 4.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 4
    * - Variable description
      - mass per string
    * - Data type
      - real
    * - Units
      - kg
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - mass of a single thruster string

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry5:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 5.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 5
    * - Variable description
      - number of strings
    * - Data type
      - integer
    * - Units
      - N/A
    * - Value restrictions
      - :math:`\geq 1`
    * - Other
      - the number of thruster strings in operation

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry6:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 6.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 6
    * - Variable description
      - Minimum power
    * - Data type
      - real
    * - Units
      - kW
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - Minimum power to thruster

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry7:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 7.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 7
    * - Variable description
      - Maximum power
    * - Data type
      - real
    * - Units
      - kW
    * - Value restrictions
      - :math:`\geq` Minimum power (variable 6)
    * - Other
      - Maximum power to thruster

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry8:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 8.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 8
    * - Variable description
      - constrant thrust
    * - Data type
      - real
    * - Units
      - N
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - If a constant thrust is desired, its value is set here

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry9:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 9.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 9
    * - Variable description
      - constant Isp
    * - Data type
      - real
    * - Units
      - s
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - If a constant Isp is desired, its value is set here

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry10:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 10.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 10
    * - Variable description
      - minimum Isp / monoprop Isp
    * - Data type
      - real
    * - Units
      - s
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - Isp for a monoprop system, if used

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry11:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 11.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 11
    * - Variable description
      - fixed efficiency
    * - Data type
      - real
    * - Units
      - N/A
    * - Value restrictions
      - :math:`0 \leq \texttt{real} \leq 1`
    * - Other
      - Fixed efficiency if operating in fixed efficiency mode

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry12:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 12.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 12
    * - Variable description
      - mixture ratio
    * - Data type
      - real
    * - Units
      - N/A
    * - Value restrictions
      - :math:`0 \leq \texttt{real} \leq 1`
    * - Other
      - Mixture ratio for chemical thruster

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry13:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 13.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 13
    * - Variable description
      - thrust scale factor
    * - Data type
      - real
    * - Units
      - N/A
    * - Value restrictions
      - :math:`\geq 0`
    * - Other
      - Constant factor by which to scale thrust

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry14:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 14.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 14
    * - Variable description
      - thrust coefficient[0]
    * - Data type
      - real
    * - Units
      - assumes thrust is in mN
    * - Value restrictions
      - N/A
    * - Other
      - Thrust coefficient of P^0. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry15:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 15.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 15
    * - Variable description
      - thrust coefficient[1]
    * - Data type
      - real
    * - Units
      - assumes thrust is in mN
    * - Value restrictions
      - N/A
    * - Other
      - Thrust coefficient of P^1. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry16:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 16.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 16
    * - Variable description
      - thrust coefficient[2]
    * - Data type
      - real
    * - Units
      - assumes thrust is in mN
    * - Value restrictions
      - N/A
    * - Other
      - Thrust coefficient of P^2. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry17:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 17.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 17
    * - Variable description
      - thrust coefficient[3]
    * - Data type
      - real
    * - Units
      - assumes thrust is in mN
    * - Value restrictions
      - N/A
    * - Other
      - Thrust coefficient of P^3. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry18:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 18.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 18
    * - Variable description
      - thrust coefficient[4]
    * - Data type
      - real
    * - Units
      - assumes thrust is in mN
    * - Value restrictions
      - N/A
    * - Other
      - Thrust coefficient of P^4. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry19:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 19.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 19
    * - Variable description
      - thrust coefficient[5]
    * - Data type
      - real
    * - Units
      - assumes thrust is in mN
    * - Value restrictions
      - N/A
    * - Other
      - Thrust coefficient of P^5. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry20:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 20.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 20
    * - Variable description
      - thrust coefficient[6]
    * - Data type
      - real
    * - Units
      - assumes thrust is in mN
    * - Value restrictions
      - N/A
    * - Other
      - Thrust coefficient of P^6. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry21:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 21.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 21
    * - Variable description
      - mass flow coefficient[0]
    * - Data type
      - real
    * - Units
      - asumes mass flow is in mg/s
    * - Value restrictions
      - N/A
    * - Other
      - Mass flow coefficient of P^0. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry22:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 22.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 22
    * - Variable description
      - mass flow coefficient[1]
    * - Data type
      - real
    * - Units
      - asumes mass flow is in mg/s
    * - Value restrictions
      - N/A
    * - Other
      - Mass flow coefficient of P^1. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry23:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 23.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 23
    * - Variable description
      - mass flow coefficient[2]
    * - Data type
      - real
    * - Units
      - asumes mass flow is in mg/s
    * - Value restrictions
      - N/A
    * - Other
      - Mass flow coefficient of P^2. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry24:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 24.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 24
    * - Variable description
      - mass flow coefficient[3]
    * - Data type
      - real
    * - Units
      - asumes mass flow is in mg/s
    * - Value restrictions
      - N/A
    * - Other
      - Mass flow coefficient of P^3. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry25:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 25.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 25
    * - Variable description
      - mass flow coefficient[4]
    * - Data type
      - real
    * - Units
      - asumes mass flow is in mg/s
    * - Value restrictions
      - N/A
    * - Other
      - Mass flow coefficient of P^4. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry26:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 26.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 26
    * - Variable description
      - mass flow coefficient[5]
    * - Data type
      - real
    * - Units
      - asumes mass flow is in mg/s
    * - Value restrictions
      - N/A
    * - Other
      - Mass flow coefficient of P^5. Only used if throttle table file is "none".

|

.. _GMATOC_EMTGSpacecraftFileSpec_Block_Propulsion_Line1_Entry27:
.. list-table:: EMTG spacecraft file specification for Propulsion Library Block, line 1, entry 27.
    :widths: 50 50
    :header-rows: 0

    * - Line number
      - 1
    * - Line name
      - N/A
    * - Variable number
      - 27
    * - Variable description
      - mass flow coefficient[6]
    * - Data type
      - real
    * - Units
      - asumes mass flow is in mg/s
    * - Value restrictions
      - N/A
    * - Other
      - Mass flow coefficient of P^6. Only used if throttle table file is "none".

