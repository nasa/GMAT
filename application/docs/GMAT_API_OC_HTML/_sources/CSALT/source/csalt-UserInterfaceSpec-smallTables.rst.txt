.. _Sec_GMATOC_UISpec:

User Interface Specification
===============================================

This section describes the GMAT Optimal Control user interface. The user interface specification is given in the tables in this section and are organized by Resource name.

Trajectory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_Trajectory_GuessSource:
.. list-table:: User interface specification for Trajectory.GuessSource.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - GuessSource
    * - Data Type
      - OptimalControlGuess Resource
    * - Default Value
      - No default
    * - Allowed Values
      - Guess Resource
    * - Description
      - The Guess source for the trajectory. If no guess is provided on a Phase, then the GuessSource from Trajectory is used. For more information see the OptimalControlGuess Resource documentation.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - Phase.GuessSource
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_MajorOptimalityTolerances:
.. list-table:: User interface specification for Trajectory.MajorOptimalityTolerances.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - MajorOptimalityTolerances
    * - Data Type
      - Real Array
    * - Default Value
      - [1e-4]
    * - Allowed Values
      - All elements must be greater than 0.0
    * - Description
      - An array of optimality tolerances for optimizer termination during mesh refinement. For iteration :math:`i`, the optimization tolerance is MajorOptimalityTolerances(:math:`i`). If :math:`\texttt{length}` (MajorOptimalityTolerances) < mesh refinement iteration number, then the optimality tolerance is :math:`\texttt{last}` (MajorOptimalityTolerances).
    * - Notes
      - Caution: If :math:`\texttt{length}` (MajorOptimalityTolerances) > MaxMeshRefinementIterations, the last elements of MajorOptimalityTolerances will not be used.
    * - Units
      - The units are defined by the user's problem scaling.
    * - Field Couplings
      - MaxRelativeErrorTolerance
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_MajorIterationsLimits:
.. list-table:: User interface specification for Trajectory.MajorIterationsLimits.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - MajorIterationsLimits
    * - Data Type
      - Integer Array
    * - Default Value
      - [1000]
    * - Allowed Values
      - All elements must be greater than or equal to 0.
    * - Description
      - An array of major iterations limits for optimizer termination during mesh refinement iteration. For iteration :math:`i`, the major iterations limit is MajorIterationsLimits(:math:`i`). If :math:`\texttt{length}` (MajorIterationsLimits) < mesh refinement iteration number, then the major iteration limit is :math:`\texttt{last}` (MajorIterationsLimits).
    * - Notes
      - Caution: If :math:`\texttt{length}` (MajorIterationsLimits) > MaxMeshRefinementIterations, the last elements of MajorIterationsLimits will not be used.
    * - Units
      - dimensionless
    * - Field Couplings
      - MaxMeshRefinementIterations
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_TotalIterationsLimits:
.. list-table:: User interface specification for Trajectory.TotalIterationsLimits.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - TotalIterationsLimits
    * - Data Type
      - Integer Array
    * - Default Value
      - [20000]
    * - Allowed Values
      - All elements must be greater than or equal to 0.
    * - Description
      - An array of total iterations limits for optimizer termination during mesh refinement iteration. For iteration :math:`i`, the total iterations limit is TotalIterationsLimits(:math:`i`). If :math:`\texttt{length}` (TotalIterationsLimits) < mesh refinement iteration number, then the total iteration limit is :math:`\texttt{last}` (TotalIterationsLimits). Note, for SNOPT, the total iterations is the sum of the Major (SQP) and minor (QP) iterations.
    * - Notes
      - Caution: If :math:`\texttt{length}` (TotalIterationsLimits) > MaxMeshRefinementIterations, the last elements of TotalIterationsLimits will not be used.
    * - Units
      - dimensionless
    * - Field Couplings
      - MaxMeshRefinementIterations
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_FeasibilityTolerances:
.. list-table:: User interface specification for Trajectory.FeasibilityTolerances.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - FeasibilityTolerances
    * - Data Type
      - Real Array
    * - Default Value
      - [1e-6]
    * - Allowed Values
      - All elements must be greater than 0.0
    * - Description
      - An array of feasibility tolerances for optimizer termination during mesh refinement iteration. For iteration :math:`i`, the feasibility tolerance is FeasibilityTolerances(:math:`i`). If :math:`\texttt{length}` (FeasibilityTolerances) < mesh refinement iteration number, then the feasibility tolerance is :math:`\texttt{last}` (FeasibilityTolerances).
    * - Notes
      - Caution: If :math:`\texttt{length}` (FeasibilityTolerances) > MaxMeshRefinementIterations, the last elements of FeasibilityTolerances will not be used.
    * - Units
      - The units are defined by the user's problem scaling.
    * - Field Couplings
      - This field interacts with Phase.MaxRelativeErrorTol.  When trying to achieve a specified MaxRelativeErrorTol for a phase, if the feasiblity tolerance is set too tightly, then the mesh refinement algorithm may not converge.
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_MaxMeshRefinementIterations:
.. list-table:: User interface specification for Trajectory.MaxMeshRefinementIterations.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - MaxMeshRefinementIterations
    * - Data Type
      - Integer
    * - Default Value
      - 0
    * - Allowed Values
      - :math:`0 < \texttt{Integer} < \texttt{Inf}`
    * - Description
      - Maximum number of mesh refinement iterations
    * - Notes
      - Passed through to CSALT Trajectory
    * - Units
      - N/A
    * - Field Couplings
      - MajorIterationsLimits

        TotalIterationsLimits

        FeasibilityTolerances

        MajorOptimalityTolerances
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_PhaseList:
.. list-table:: User interface specification for Trajectory.PhaseList.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - PhaseList
    * - Data Type
      - List of Phase Resources
    * - Default Value
      - Empty List
    * - Allowed Values
      - A list of user-defined Phase Resources.
    * - Description
      - A list of user-defined Phase Resources. These phases are included in the optimzation.
    * - Notes
      - Passed through to CSALT Trajectory
    * - Units
      - N/A
    * - Field Couplings
      - None.
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_SNOPTOutputFile:
.. list-table:: User interface specification for Trajectory.SNOPTOutputFile.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - SNOPTOutputFile
    * - Data Type
      - FileName
    * - Default Value
      - SNOPTOutputFile.txt
    * - Allowed Values
      - Valid file name
    * - Description
      - File containing SNOPT output data.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_AddSimpleLinkageChain:
.. list-table:: User interface specification for Trajectory.AddSimpleLinkageChain.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - AddSimpleLinkageChain
    * - Data Type
      - List of Phase Resources
    * - Default Value
      - Empty List
    * - Allowed Values
      - A list of user-defined Phase Resources. The length of the list be :math:`\geq 2`.
    * - Description
      - Adds a list of phases to be linked with full time and state continuity. Including multiple lines with "AddSimpleLinkageChain" adds multiple simple linkage chain configurations. For example

        

        :math:`\texttt{myTrajectory.AddSimpleLinkageChain} = \\ \left\{\texttt{Phase1}, \texttt{Phase2}\right\}`

        :math:`\texttt{myTrajectory.AddSimpleLinkageChain} = \\ \left\{\texttt{Phase3}, \texttt{Phase4}, \texttt{Phase5} \right\}`

        

        results in full time/state continuity between the end :math:`\texttt{Phase1}` and the start of :math:`\texttt{Phase2}`, and full time/state continuity between end of :math:`\texttt{Phase3}` and the start of :math:`\texttt{Phase4}`, and full time/state continuity between end of :math:`\texttt{Phase4}` and the start of :math:`\texttt{Phase5}`.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - Trajectory.PhaseList. Phases in AddSimpleLinkageChain must also be in PhaseList.
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_CustomLinkages:
.. list-table:: User interface specification for Trajectory.CustomLinkages.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - CustomLinkages
    * - Data Type
      - List of CustomLinkageCosntraints
    * - Default Value
      - Empty List
    * - Allowed Values
      - Any CustomLinkageConstraint Resources created.
    * - Description
      - Adds custom linkage constraints to the trajectory as opposed to the full state/time constraints from the AddSimpleLinkageChain field. For example, a CustomLinkageConstraint object can be added to only restrict the position state. In that case, this field is used to add that constraint to the trajectory.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_OutputCoordinateSystem:
.. list-table:: User interface specification for Trajectory.OutputCoordinateSystem.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - OutputCoordinateSystem
    * - Data Type
      - Coordinate System
    * - Default Value
      - UsePhaseCoordinateSystems
    * - Allowed Values
      - Any created coordinate system object or the keyword "UsePhaseCoordinateSystems"
    * - Description
      - Sets the coordinate system for data written to the output optimal control history file. If "UsePhaseCoordinateSystems" is used, the data from each phase is printed in their respective coordinate systems used in their force models.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_SolutionFile:
.. list-table:: User interface specification for Trajectory.SolutionFile.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - SolutionFile
    * - Data Type
      - String
    * - Default Value
      - <TrajectoryResourceName>Solution.och
    * - Allowed Values
      - File name that the output data will be sent to in the GMAT output folder
    * - Description
      - Used to set the output Optimal Control History file name sent to the GMAT output folder. A new file is created if the current file name is not found
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_AllowFailedMeshOptimizations:
.. list-table:: User interface specification for Trajectory.AllowFailedMeshOptimizations.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - AllowFailedMeshOptimizations
    * - Data Type
      - Boolean
    * - Default Value
      - false
    * - Allowed Values
      - true, false
    * - Description
      - Sets whether the optimization should be stopped if convergence is not achieved in any mesh refinement iteration (:math:`\texttt{FALSE}`) or if mesh refinement should be continued regardless of the final result of the previous optimization attempt (:math:`\texttt{TRUE}`).
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_StateScaleMode:
.. list-table:: User interface specification for Trajectory.StateScaleMode.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - StateScaleMode
    * - Data Type
      - String
    * - Default Value
      - Canonical
    * - Allowed Values
      - Canonical
    * - Description
      - The scaling mode for the orbit state. If the Sun is the central body, then the distance unit is an astronomical unit and the GM of the Sun is 1. If the Earth is the central body, then the distance unit is 42000 km and the GM of the Earth is 1. Otherwise, the radius of the central body is the distance unit and the GM of the central body is 1.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_MassScaleFactor:
.. list-table:: User interface specification for Trajectory.MassScaleFactor.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - MassScaleFactor
    * - Data Type
      - Real
    * - Default Value
      - 1
    * - Allowed Values
      - :math:`\texttt{REAL\_MIN} < \texttt{Real} < \texttt{REAL\_MAX}`
    * - Description
      - The mass scale factor (non-dimensional mass is mass/MassScaleFactor). A best-practice is to set the mass scale factor to be approximately the initial total mass of the spacecraft to result in an initial non-dimensionalized mass of approximately 1.0. Note :math:`\texttt{REAL\_MIN}` is currently set to 2.2250738585072014e-308.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_AddBoundaryFunction:
.. list-table:: User interface specification for Trajectory.AddBoundaryFunction.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - AddBoundaryFunction
    * - Data Type
      - List of OptimalControlFunction resources
    * - Default Value
      - Empty List
    * - Allowed Values
      - Any created OptimalControlFunction resource
    * - Description
      - A list of user-defined OptimalControlFunction resources to be included in the optmization problem.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_PublishUpdateRate:
.. list-table:: User interface specification for Trajectory.PublishUpdateRate.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - PublishUpdateRate
    * - Data Type
      - Integer
    * - Default Value
      - 1
    * - Allowed Values
      - :math:`\texttt{Integer} > 0`
    * - Description
      - Rate at which trajectory state is output to the publisher. Trajectory data is sent to the publisher (which updates plots and reports) every PublishUpdateRate calls to the cost/constraint functions. The output rate to the Optimal Control History file is independent of PublishUpdateRate.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_OptimizationMode:
.. list-table:: User interface specification for Trajectory.OptimizationMode.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - OptimizationMode
    * - Data Type
      - List of strings
    * - Default Value
      - {Minimize}
    * - Allowed Values
      - Feasible point, Minimize, Maximize
    * - Description
      - The optimization mode (meaning to miminize, maximize, or find a feasible solution). For mesh-refinement iteration :math:`i`, the optimization mode is OptimizationMode(:math:`i`). If length(OptimizationMode) < mesh refinement iteration number, then the optimization mode is last(OptimizationMode).
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_MeshRefinementGuessMode:
.. list-table:: User interface specification for Trajectory.MeshRefinementGuessMode.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - MeshRefinementGuessMode
    * - Data Type
      - String
    * - Default Value
      - LastSolutionMostRecentMesh
    * - Allowed Values
      - LastSolutionMostRecentMesh, BestSolutionMostRecentMesh, BestSolutionAnyMesh
    * - Description
      - The initial-guess source for mesh refinement iterations after the first mesh refinement iteration. LastSolutionMostRecentMesh will use the solution attained by the previous mesh refinement iteration -- whether that iteration converged to a feasible solution or not. BestSolutionMostRecentMesh uses the "best" previous solution from only the one immediately previous mesh refinement iteration as the initial guess for the current mesh refinement iteration. BestSolutionAnyMesh uses the "best" previous solution from any previous mesh refinement iteration as an initial guess for the current mesh refinement iteration. The following conditions are used to define "best": The following conditions are used to define "best": (1) If no feasible solutions have been found, then "best" refers to the previous solution with the smallest infeasibility. (2) If one feasible solution has been found, then that solution is "best," regardless of its optimality. (3) If multiple feasible solutions have been found, then the feasible solution with the smallest value of the merit function (i.e., the most optimal solution) is "best."
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_StateDimension:
.. list-table:: User interface specification for Trajectory.StateDimension.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - StateDimension
    * - Data Type
      - Integer
    * - Default Value
      - 7
    * - Allowed Values
      - :math:`\texttt{Integer} > 0`
    * - Description
      - The number of states defining the problem for the trajectory.
    * - Notes
      - Default value of 7 is selected because of 6 states defining position and velocity of point mass and 1 state defining mass of point mass. All Phases of a Trajectory default to Trajectory.StateDimension. However, if Phase.StateDimension is set, then the Phase value overrides the Trajectory value for that Phase. Not currently exposed to users because the default value is currently the only acceptable value.
    * - Units
      - N/A
    * - Field Couplings
      - Phase.StateDimension
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Trajectory_ControlDimension:
.. list-table:: User interface specification for Trajectory.ControlDimension.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Trajectory
    * - Field Name
      - ControlDimension
    * - Data Type
      - Integer
    * - Default Value
      - 3
    * - Allowed Values
      - :math:`\texttt{Integer} > 0`
    * - Description
      - The number of controls defining the problem for the trajectory.
    * - Notes
      - Default value of 3 is selected for 3D control vector. All Phases of a Trajectory default to Trajectory.ControlDimension. However, if Phase.ControlDimension is set, then the Phase value overrides the Trajectory value for that Phase. Not currently exposed to users because the default value is currently the only acceptable value.
    * - Units
      - N/A
    * - Field Couplings
      - Phase.ControlDimension
    * - Access
      - set
    * - Interfaces
      - script

|

Phase
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_Phase_Type:
.. list-table:: User interface specification for Phase.Type.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - Type
    * - Data Type
      - String
    * - Default Value
      - RadauPseudospectral
    * - Allowed Values
      - RadauPseudospectral, HermiteSimpson, ImplicitRKOrder4, ImplicitRKOrder6, ImplicitRKOrder8
    * - Description
      - The transcription algorithm for the phase.
    * - Notes
      - GMAT instantiates a CSALT phase according to the type chosen here by the user.
    * - Units
      - N/A
    * - Field Couplings
      - PointsPerSubPhase, SubPhaseBoundaries
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_ThrustMode:
.. list-table:: User interface specification for Phase.ThrustMode.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - ThrustMode
    * - Data Type
      - String
    * - Default Value
      - Thrust
    * - Allowed Values
      - Thrust, Coast
    * - Description
      - Flag to model the Phase as a thrust or coast phase. In a coast phase, the control magnitude must be zero at all times. In a thrust phase, the control magnitude is allowed to vary. In other words, unless subject to additional constraints, a thrust phase may contain coasting segments.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - DynamicsConfiguration
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_MaxRelativeErrorTolerance:
.. list-table:: User interface specification for Phase.MaxRelativeErrorTolerance.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - MaxRelativeErrorTolerance
    * - Data Type
      - Real
    * - Default Value
      - 1e-05
    * - Allowed Values
      - :math:`\texttt{Real} > 0.0`
    * - Description
      - The maximum allowable relative error in a mesh interval. The mesh refinement algorithm will continue to iterate until all mesh intervals in the phase have a relative error less than MaxRelativeErrorTolerance. The mesh refinment algorithm is different for different transcription types.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - Type
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_SubPhaseBoundaries:
.. list-table:: User interface specification for Phase.SubPhaseBoundaries.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - SubPhaseBoundaries
    * - Data Type
      - Array of Reals
    * - Default Value
      - [-1 1]
    * - Allowed Values
      - Array of real numbers that is monotonically increasing. Allowed values depend on the Type field; see Description for more details.
    * - Description
      - Defines sub-phases within a phase used as an initial guess for mesh refinement and to support different quadrature orders within a given phase to capture slow and fast dynamics within a single phase. When Type is set to RadauPseudospectral, SubPhaseBoundaries must start with -1 and end with 1, with internal values increasing monotonically. For other transcriptions,  SubPhaseBoundaries must start with 0 and end with 1, with internal values increasing monotonically. The default value assumes the default Type = RadauPseudospectral.
    * - Notes
      - Passed through to CSALT MeshIntervalFractions
    * - Units
      - N/A
    * - Field Couplings
      - Type, PointsPerSubPhase
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_PointsPerSubPhase:
.. list-table:: User interface specification for Phase.PointsPerSubPhase.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - PointsPerSubPhase
    * - Data Type
      - Array of Integers
    * - Default Value
      - [5]
    * - Allowed Values
      - Array of integers that has one less entry than SubPhaseBoundaries.  Allowed values depend on the Type field; see Desciption for more details.
    * - Description
      - Models sub-phases within a phase. For RadauPseudospectral, this allows for different quadrature orders for each sub-phase. The first entry in PointsPerSubPhase is the number of points for the first sub-phase in SubPhaseBoundaries. For example, if SubPhaseBoundaries = [-1 0.5 1], and PointsPerSubPhase = [5 3], then the phase is modeled using two sub-phases. The first sub-phase is from non-dimensional time -1 to 0.5 and is modeled with 5 points, and the second sub-phase is from non-dimensional time 0.5 to 1.0 and is modeled with 3 points. Note: Mesh refinement will change these values; the values provided are guesses for the discretization. In general, set the number of points as low as possible to obtain convergence on the initial mesh and allow the mesh refinement algorithm to determine a finer mesh. On the other hand, for an ImplictRKOrder* method, the order of the quadrature is fixed. In this case, the value of PointsPerSubPhase sets the number of implicit integration steps. Using the earlier example, if Phase.Type = ImplicitRKOrder6, there would be 5 IRK steps in the first subphase, each of order 6, and 3 IRK steps in the second subphase, each of order 6. The default value assumes the default Type = RadauPseudospectral.
    * - Notes
      - Passed through to CSALT MeshIntervalNumPoints
    * - Units
      - N/A
    * - Field Couplings
      - Type, SubPhaseBoundaries
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_GuessSource:
.. list-table:: User interface specification for Phase.GuessSource.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - GuessSource
    * - Data Type
      - Guess Resource
    * - Default Value
      - No default
    * - Allowed Values
      - OptimalControlGuess Resource
    * - Description
      - The Guess source for the phase. If no guess is provided, the Trajectory's Guess resource is used.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - Trajectory.Guess
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_StateLowerBound:
.. list-table:: User interface specification for Phase.StateLowerBound.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - StateLowerBound
    * - Data Type
      - Array of Reals
    * - Default Value
      - No default
    * - Allowed Values
      - Array of real numbers with same length as number of state decision parameters (StateDimension)
    * - Description
      - The lower bound on state variables at all mesh and stage points within a phase. 

        

        Bounds must be consistent with the state type

        and should be specified with respect to the central body of the DynamicsConfiguration with

        :math:`\texttt{J2000Eq}` axes.
    * - Notes
      - Passed through to CSALT
    * - Units
      - Units of the State Vector. Typically [km km km km/s km/s km/s and kg] if using Cartesian state, and modified accordingly for other state types.
    * - Field Couplings
      - StateDimension, Trajectory.StateDimension
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_StateUpperBound:
.. list-table:: User interface specification for Phase.StateUpperBound.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - StateUpperBound
    * - Data Type
      - Array of Reals
    * - Default Value
      - No default
    * - Allowed Values
      - Array of real numbers with same length as number of state decision parameters (StateDimension)
    * - Description
      - The upper bound on state variables at all mesh and stage points within a phase. 

        

        Bounds must be consistent with the state type

        and should be specified with respect to the central body of the DynamicsConfiguration with

        :math:`\texttt{J2000Eq}` axes.
    * - Notes
      - Passed through to CSALT
    * - Units
      - Units of the State Vector. Typically [km km km km/s km/s km/s and kg] if using Cartesian state, and modified accordingly for other state types.
    * - Field Couplings
      - StateDimension, Trajectory.StateDimension
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_ControlLowerBound:
.. list-table:: User interface specification for Phase.ControlLowerBound.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - ControlLowerBound
    * - Data Type
      - Array of Reals
    * - Default Value
      - No default
    * - Allowed Values
      - Array of real numbers with same length as number of control decision parameters (ControlDimension)
    * - Description
      - The non-dimensional lower bound on control variables at all mesh and stage points within a phase. In standard non-dimensionalization, 0, -1 is minimum control, and +1 is maximum control.
    * - Notes
      - Passed through to CSALT
    * - Units
      - non-dimensional
    * - Field Couplings
      - ControlDimension, Trajectory.ControlDimension
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_ControlUpperBound:
.. list-table:: User interface specification for Phase.ControlUpperBound.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - ControlUpperBound
    * - Data Type
      - Array of Reals
    * - Default Value
      - No default
    * - Allowed Values
      - Array of real numbers with same length as number of control decision parameters (ControlDimension)
    * - Description
      - The non-dimensional upper bound on control variables at all mesh and stage points within a phase. In standard non-dimensionalization, 0, -1 is minimum control, and +1 is maximum control.
    * - Notes
      - Passed through to CSALT
    * - Units
      - non-dimensional
    * - Field Couplings
      - ControlDimension, Trajectory.ControlDimension
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_EpochFormat:
.. list-table:: User interface specification for Phase.EpochFormat.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - EpochFormat
    * - Data Type
      - String
    * - Default Value
      - UTCGregorian
    * - Allowed Values
      - Valid GMAT epoch type. E.g., UTCModJulian, TDBGregorian
    * - Description
      - The epoch format used to specify all epoch fields for the phase.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - InitialEpoch, FinalEpoch, EpochLowerBound, EpochUpperBound
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_InitialEpoch:
.. list-table:: User interface specification for Phase.InitialEpoch.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - InitialEpoch
    * - Data Type
      - GMAT Epoch
    * - Default Value
      - No default
    * - Allowed Values
      - Valid GMAT Epoch
    * - Description
      - Initial guess for the initial epoch of the phase.  This value is modified during optimization.
    * - Notes
      - N/A
    * - Units
      - Time.
    * - Field Couplings
      - EpochFormat
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_FinalEpoch:
.. list-table:: User interface specification for Phase.FinalEpoch.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - FinalEpoch
    * - Data Type
      - GMAT Epoch
    * - Default Value
      - No default
    * - Allowed Values
      - Valid GMAT Epoch
    * - Description
      - Initial guess for the final epoch of the phase.  This value is modified during optimization.
    * - Notes
      - N/A
    * - Units
      - Time
    * - Field Couplings
      - EpochFormat
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_EpochLowerBound:
.. list-table:: User interface specification for Phase.EpochLowerBound.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - EpochLowerBound
    * - Data Type
      - GMAT Epoch
    * - Default Value
      - No default
    * - Allowed Values
      - Valid GMAT Epoch
    * - Description
      - The lower bound on time variables at all mesh and stage points within a phase.
    * - Notes
      - N/A
    * - Units
      - Time
    * - Field Couplings
      - EpochFormat
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_EpochUpperBound:
.. list-table:: User interface specification for Phase.EpochUpperBound.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - EpochUpperBound
    * - Data Type
      - GMAT Epoch
    * - Default Value
      - No default
    * - Allowed Values
      - Valid GMAT Epoch
    * - Description
      - The upper bound on time variables at all mesh and stage points within a phase.
    * - Notes
      - N/A
    * - Units
      - Time
    * - Field Couplings
      - EpochFormat
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_DynamicsConfiguration:
.. list-table:: User interface specification for Phase.DynamicsConfiguration.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - DynamicsConfiguration
    * - Data Type
      - DynamicsConfiguration Resource
    * - Default Value
      - No default
    * - Allowed Values
      - Valid DynamicsConfiguration Resource
    * - Description
      - The grouping of spacecraft, force models, and maneuver models to be used for the phase.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - StateLowerBound,StateUpperBound
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_MaxControlMagnitude:
.. list-table:: User interface specification for Phase.MaxControlMagnitude.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - MaxControlMagnitude
    * - Data Type
      - Real
    * - Default Value
      - 1
    * - Allowed Values
      - :math:`0 < \texttt{Real} < \texttt{Inf}`
    * - Description
      - The maximum magnitude of the non-dimensional control vector. Warning: Setting this value to greater than 1.0 corresponds to control authority greater than what the controller can physically provide and can result in non-physical thrust profiles and trajectories. The upper bound should be set to :math:`\leq 1` except for when using homotopy or troubleshooting.
    * - Notes
      - N/A
    * - Units
      - Dimensionless
    * - Field Couplings
      - None
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_MinControlMagnitude:
.. list-table:: User interface specification for Phase.MinControlMagnitude.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - MinControlMagnitude
    * - Data Type
      - Real
    * - Default Value
      - 0
    * - Allowed Values
      - :math:`0 < \texttt{Real} < \texttt{Inf}`
    * - Description
      - The minimum magnitude of the non-dimensional control vector. Warning: Setting this value to greater than 1.0 corresponds to control authority greater than what the controller can physically provide and can result in non-physical thrust profiles and trajectories. The upper bound should be set to :math:`\leq 1` except for when using homotopy or troubleshooting.
    * - Notes
      - N/A
    * - Units
      - Dimensionless
    * - Field Couplings
      - None
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_OverrideColorInGraphics:
.. list-table:: User interface specification for Phase.OverrideColorInGraphics.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - OverrideColorInGraphics
    * - Data Type
      - Boolean
    * - Default Value
      - false
    * - Allowed Values
      - true, false
    * - Description
      - Flag to override the default color defined on the spacecraft and to use the color defined by OrbitColor in graphics during optimization. This field allows phases to appear using different colors in the graphics.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - OrbitColor
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_BuiltInCost:
.. list-table:: User interface specification for Phase.BuiltInCost.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - BuiltInCost
    * - Data Type
      - String
    * - Default Value
      - No default
    * - Allowed Values
      - RMAGFinal, TotalMassFinal, AbsoluteEpochFinal
    * - Description
      - The cost function.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - Trajectory.OptimizationMode
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_BuiltInBoundaryConstraints:
.. list-table:: User interface specification for Phase.BuiltInBoundaryConstraints.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - BuiltInBoundaryConstraints
    * - Data Type
      - StringArray
    * - Default Value
      - No default
    * - Allowed Values
      - N/A
    * - Description
      - Boundary constraints for the phase. These boundary constraints do not support analytic Jacobians.
    * - Notes
      - This interface is intended to support rapid implementation but is being replaced with assignment-based constraints. As of this writing, there are no constraints implemented via this interface.
    * - Units
      - N/A
    * - Field Couplings
      - None
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_OrbitColor:
.. list-table:: User interface specification for Phase.OrbitColor.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - OrbitColor
    * - Data Type
      - ColorType
    * - Default Value
      - Red
    * - Allowed Values
      - Valid predefined color name or RGB triplet value between 0 and 255
    * - Description
      - The color for the phase in graphics if OverrideColorInGraphics is true.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - OverrideColorInGraphics
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_StateDimension:
.. list-table:: User interface specification for Phase.StateDimension.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - StateDimension
    * - Data Type
      - Integer
    * - Default Value
      - 7
    * - Allowed Values
      - :math:`\texttt{Integer} > 0`
    * - Description
      - The number of states defining the problem for the phase.
    * - Notes
      - Default value of 7 is selected because of 6 states defining position and velocity of point mass and 1 state defining mass of point mass. All Phases of a Trajectory default to Trajectory.StateDimension. However, if Phase.StateDimension is set, then the Phase value overrides the Trajectory value for that Phase. Not currently exposed to users because the default value is currently the only acceptable value.
    * - Units
      - N/A
    * - Field Couplings
      - Trajectory.StateDimension
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_Phase_ControlDimension:
.. list-table:: User interface specification for Phase.ControlDimension.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - Phase
    * - Field Name
      - ControlDimension
    * - Data Type
      - Integer
    * - Default Value
      - 3
    * - Allowed Values
      - :math:`\texttt{Integer} > 0`
    * - Description
      - The number of control variables defining the problem for the phase.
    * - Notes
      - Default value of 3 is selected for 3D control vector. All Phases of a Trajectory default to Trajectory.ControlDimension. However, if Phase.ControlDimension is set, then the Phase value overrides the Trajectory value for that Phase. Not currently exposed to users because the default value is currently the only acceptable value.
    * - Units
      - N/A
    * - Field Couplings
      - Trajectory.ControlDimension
    * - Access
      - set
    * - Interfaces
      - script

|

DynamicsConfiguration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_DynamicsConfiguration_ForceModels:
.. list-table:: User interface specification for DynamicsConfiguration.ForceModels.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - DynamicsConfiguration
    * - Field Name
      - ForceModels
    * - Data Type
      - List of Force Model Resources
    * - Default Value
      - No default
    * - Allowed Values
      - List of Valid Force Model Resources
    * - Description
      - Lists the force model for each spacecraft. Currently, only a single spacecraft is supported, so only a single force model should be provided.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - Spacecraft
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_DynamicsConfiguration_Spacecraft:
.. list-table:: User interface specification for DynamicsConfiguration.Spacecraft.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - DynamicsConfiguration
    * - Field Name
      - Spacecraft
    * - Data Type
      - List of Spacecraft Resources
    * - Default Value
      - No default
    * - Allowed Values
      - List of Valid Spacecraft Resources
    * - Description
      - Lists the spacecraft that are modeled.
    * - Notes
      - Currently, only one spacecraft per phase is supported. If there are multiple spacecraft, then the ith spacecraft in the list is the spacecraft to which the ith elements in the DynamicsModels, FiniteBurns, and ImpulsiveBurns lists apply.
    * - Units
      - N/A
    * - Field Couplings
      - ForceModels, FiniteBurns, ImpulsiveBurns
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_DynamicsConfiguration_FiniteBurns:
.. list-table:: User interface specification for DynamicsConfiguration.FiniteBurns.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - DynamicsConfiguration
    * - Field Name
      - FiniteBurns
    * - Data Type
      - List of Finite Burns
    * - Default Value
      - No default
    * - Allowed Values
      - List of Finite Burns (including EMTG models, if used)
    * - Description
      - Lists the finite burn resources for each spacecraft.
    * - Notes
      - Currently, finite burns MUST be EmtgInterface resources
    * - Units
      - N/A
    * - Field Couplings
      - Spacecraft
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_DynamicsConfiguration_ImpulsiveBurns:
.. list-table:: User interface specification for DynamicsConfiguration.ImpulsiveBurns.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - DynamicsConfiguration
    * - Field Name
      - ImpulsiveBurns
    * - Data Type
      - List of Impulsive Burns
    * - Default Value
      - No default
    * - Allowed Values
      - List of Impulsive Burns
    * - Description
      - Lists the impulsive burn resources for each spacecraft.
    * - Notes
      - Not currently implemented.
    * - Units
      - N/A
    * - Field Couplings
      - Spacecraft
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_DynamicsConfiguration_EMTGTankConfig:
.. list-table:: User interface specification for DynamicsConfiguration.EMTGTankConfig.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - DynamicsConfiguration
    * - Field Name
      - EMTGTankConfig
    * - Data Type
      - List of Fuel Tank Hardware
    * - Default Value
      - No default
    * - Allowed Values
      - List of valid Fuel Tank Hardware resources
    * - Description
      - Lists the fuel tanks that are used by the selected spacecraft to represent the depletion of fuel mass along a trajectory on the GMAT side of the GMAT/CSALT interface. The tank(s) selected must be attached to the spacecraft in use and the total mass of the spacecraft must be at least as much as the upper bound on mass in the phase objects using this spacecraft. This field is only required when the DynamicsConfiguration is using an EMTGSpacecraft for a Phase that is not a coast phase.
    * - Notes
      - 
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

OptimalControlGuess
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_OptimalControlGuess_Type:
.. list-table:: User interface specification for OptimalControlGuess.Type.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlGuess
    * - Field Name
      - Type
    * - Data Type
      - String
    * - Default Value
      - No default
    * - Allowed Values
      - GMATArray, CollocationGuessFile
    * - Description
      - Defines the array guess type: GMATArray-based or file-based.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlGuess_TimeSystem:
.. list-table:: User interface specification for OptimalControlGuess.TimeSystem.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlGuess
    * - Field Name
      - TimeSystem
    * - Data Type
      - String
    * - Default Value
      - No default
    * - Allowed Values
      - Any GMAT Modified Julian Time System
    * - Description
      - The time system used in the data array specified by ArrayName. Only used if Type = GMATArray.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - Type
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlGuess_CoordinateSystem:
.. list-table:: User interface specification for OptimalControlGuess.CoordinateSystem.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlGuess
    * - Field Name
      - CoordinateSystem
    * - Data Type
      - CoordinateSystemResource
    * - Default Value
      - EarthMJ2000Eq
    * - Allowed Values
      - CoordinateSystem Resource
    * - Description
      - The coordinate system used in the data array specified by ArrayName. Only used if Type = GMATArray.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - Type
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlGuess_GuessArray:
.. list-table:: User interface specification for OptimalControlGuess.GuessArray.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlGuess
    * - Field Name
      - GuessArray
    * - Data Type
      - Array of Reals
    * - Default Value
      - No default
    * - Allowed Values
      - Each element of the array must be :math:`-\texttt{Inf} < \texttt{Real} < \texttt{Inf}`
    * - Description
      - The array containing the guess data.
    * - Notes
      - The columns of the array are: time, states, controls. The rows are the different values of those variables. The times must increase monotonically as the row number increases. The array must have at least 5 rows.
    * - Units
      - N/A
    * - Field Couplings
      - Type
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlGuess_FileName:
.. list-table:: User interface specification for OptimalControlGuess.FileName.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlGuess
    * - Field Name
      - FileName
    * - Data Type
      - File name and path.
    * - Default Value
      - No default
    * - Allowed Values
      - File consistent with Type defined in Type field.
    * - Description
      - The file containing the guess data, formatted as an Optimal Control History file, relative to the directory in which the GMAT executable is located.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - Type
    * - Access
      - set
    * - Interfaces
      - script

|

CustomLinkageConstraint
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_CustomLinkageConstraint_ConstraintMode:
.. list-table:: User interface specification for CustomLinkageConstraint.ConstraintMode.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - CustomLinkageConstraint
    * - Field Name
      - ConstraintMode
    * - Data Type
      - String
    * - Default Value
      - N/A
    * - Allowed Values
      - Difference, Absolute
    * - Description
      - Applies a constraint on the difference between parameters (specifed in using SetModelParameter) for two phases (Difference mode) or applies an absolute constraint on quantities for a single phase specified in the InitialPhase field (Absolute mode).
    * - Notes
      - None
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_CustomLinkageConstraint_InitialPhase:
.. list-table:: User interface specification for CustomLinkageConstraint.InitialPhase.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - CustomLinkageConstraint
    * - Field Name
      - InitialPhase
    * - Data Type
      - String
    * - Default Value
      - N/A
    * - Allowed Values
      - Phase resource
    * - Description
      - The first phase in the linkage for a Difference-mode constraint. The only phase in the linkage for an Absolute-mode constraint.
    * - Notes
      - None
    * - Units
      - N/A
    * - Field Couplings
      - ConstraintMode
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_CustomLinkageConstraint_InitialPhaseBoundaryType:
.. list-table:: User interface specification for CustomLinkageConstraint.InitialPhaseBoundaryType.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - CustomLinkageConstraint
    * - Field Name
      - InitialPhaseBoundaryType
    * - Data Type
      - String
    * - Default Value
      - N/A
    * - Allowed Values
      - Start, End
    * - Description
      - The boundary of InitialPhase at which the constraint is calculated.
    * - Notes
      - None
    * - Units
      - N/A
    * - Field Couplings
      - ConstraintMode
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_CustomLinkageConstraint_FinalPhase:
.. list-table:: User interface specification for CustomLinkageConstraint.FinalPhase.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - CustomLinkageConstraint
    * - Field Name
      - FinalPhase
    * - Data Type
      - String
    * - Default Value
      - N/A
    * - Allowed Values
      - Phase resource
    * - Description
      - The second phase in the linkage for a Difference-mode constraint.
    * - Notes
      - None
    * - Units
      - N/A
    * - Field Couplings
      - ConstraintMode
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_CustomLinkageConstraint_FinalPhaseBoundaryType:
.. list-table:: User interface specification for CustomLinkageConstraint.FinalPhaseBoundaryType.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - CustomLinkageConstraint
    * - Field Name
      - FinalPhaseBoundaryType
    * - Data Type
      - String
    * - Default Value
      - N/A
    * - Allowed Values
      - Start, End
    * - Description
      - The boundary of FinalPhase at which the constraint is calculated.
    * - Notes
      - None
    * - Units
      - N/A
    * - Field Couplings
      - ConstraintMode
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_CustomLinkageConstraint_SetModelParameter():
.. list-table:: User interface specification for CustomLinkageConstraint.SetModelParameter().
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - CustomLinkageConstraint
    * - Field Name
      - SetModelParameter()
    * - Data Type
      - Tuple
    * - Default Value
      - N/A
    * - Allowed Values
      - See :numref:`CSALT_ConstraintTypes`
    * - Description
      - SetModelParameter() is an overloaded function for setting model data. In general, the argument is a tuple. The first element of the tuple is a string describing the parameter to be set, and the second element of the tuple is the value of the parameter.
    * - Notes
      - None
    * - Units
      - See :numref:`CSALT_ConstraintTypes`
    * - Field Couplings
      - ConstraintMode
    * - Access
      - set
    * - Interfaces
      - script

|

EMTGSpacecraft
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_EMTGSpacecraft_SpacecraftFile:
.. list-table:: User interface specification for EMTGSpacecraft.SpacecraftFile.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - EMTGSpacecraft
    * - Field Name
      - SpacecraftFile
    * - Data Type
      - String
    * - Default Value
      - N/A
    * - Allowed Values
      - Full file path (can be relative to ../data/emtg) AND name of *.emtg_spacecraftopt file
    * - Description
      - A valid EMTG spacecraft file
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_EMTGSpacecraft_SpacecraftStage:
.. list-table:: User interface specification for EMTGSpacecraft.SpacecraftStage.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - EMTGSpacecraft
    * - Field Name
      - SpacecraftStage
    * - Data Type
      - IntegerArray
    * - Default Value
      - [ 1 ]
    * - Allowed Values
      - Integers that are less than or equal to the number of stages contained in the EMTG options file that is set using the "EMTGSpacecraftFile" field on EMTGInterface Resource
    * - Description
      - The EMTG spacecraft stage for each phase, where each position in the array corresponds to the phase index on the trajectory. (I.e., the second element in this array corresponds to the second phase's stage). If there are more phases than listed stages that use this EmtgInterface object, the phases after the last stage will continue to use the last stage in the list. The elements of EMTGSpacecraftStage are 1-indexed.
    * - Notes
      - The array of stages does NOT skip coast phases. I.e., if the phase structure is coast-thrust-thrust, then [2,1] will result in stage 2 applying to the coast phase (and not being used) and stage 1 being applied to both thrust phases. [2,2,1] results in the first thrust phase using stage 2 and the second thrust phase using stage 1. If the length of the array is longer than the number of phases, then trailing elements of the array are not used. I.e., if there are i phases, then only the first i elements of EMTGSpacecraftStage are used.
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_EMTGSpacecraft_DutyCycle:
.. list-table:: User interface specification for EMTGSpacecraft.DutyCycle.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - EMTGSpacecraft
    * - Field Name
      - DutyCycle
    * - Data Type
      - Real
    * - Default Value
      - 1
    * - Allowed Values
      - :math:`0 \leq \texttt{Real} \leq 1`
    * - Description
      - Field to set the averaged duty cycle factor of the propulsion system.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

OptimalControlFunction
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_OptimalControlFunction_Function:
.. list-table:: User interface specification for OptimalControlFunction.Function.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction
    * - Field Name
      - Function
    * - Data Type
      - String
    * - Default Value
      - Expression
    * - Allowed Values
      - "Expression", "PatchedConicLaunch", "IntegratedFlyby", "PatchedConicFlyby", "CelestialBodyRendezvous"
    * - Description
      - Sets type of function of the OptimalControlFunction
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

OptimalControlFunction for Function Field = Expression
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_OptimalControlFunction_Expression_Type:
.. list-table:: User interface specification for OptimalControlFunction.Type for OptimalControlFunction.Function = Expression.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:Expression
    * - Field Name
      - Type
    * - Data Type
      - String
    * - Default Value
      - AlgebraicConstraint
    * - Allowed Values
      - AlgebraicConstraint
    * - Description
      - Sets the "meaning" of the OptimalControlFunction. I.e., is the function to be interpreted as a constraint or a cost function, etc.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_Expression_PhaseList:
.. list-table:: User interface specification for OptimalControlFunction.PhaseList for OptimalControlFunction.Function = Expression.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:Expression
    * - Field Name
      - PhaseList
    * - Data Type
      - GMAT List
    * - Default Value
      - No default
    * - Allowed Values
      - Name of a single existing Phase resource and either Initial or Final. E.g., :math:`\texttt{PhaseList} = \left\{\texttt{Phase1.Initial}\right\}` or :math:`\texttt{PhaseList} = \left\{\texttt{Phase2.Final}\right\}`.
    * - Description
      - Sets on which phase and at which boundary of that phase the OptimalControlFunction Expression is to be evaluated.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_Expression_SetModelParameter:
.. list-table:: User interface specification for OptimalControlFunction.SetModelParameter for OptimalControlFunction.Function = Expression.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:Expression
    * - Field Name
      - SetModelParameter
    * - Data Type
      - Tuple
    * - Default Value
      - No default
    * - Allowed Values
      - ('LowerBounds', <Real>); ('UpperBounds', <Real>); ('ScaleFactor', <Real>); ('Expression', <GMAT Equation as a string>)
    * - Description
      - Used to set parameters of an OptimalControlFunction. If OptimalControlFunction.Type = AlgebraicConstraint, then the tuple can be used to set the bounds on the constraint ('LowerBounds' or 'UpperBounds'); to set the scale factor for the constraint ('ScaleFactor'); and to set the constrained expression itself ('Expression').
    * - Notes
      - N/A
    * - Units
      - Default GMAT units for the expression being set
    * - Field Couplings
      - Type, Function
    * - Access
      - set
    * - Interfaces
      - script

|

OptimalControlFunction for Function Field = PatchedConicLaunch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_OptimalControlFunction_PatchedConicLaunch_Type:
.. list-table:: User interface specification for OptimalControlFunction.Type for OptimalControlFunction.Function = PatchedConicLaunch.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:PatchedConicLaunch
    * - Field Name
      - Type
    * - Data Type
      - String
    * - Default Value
      - AlgebraicConstraint
    * - Allowed Values
      - AlgebraicConstraint
    * - Description
      - When the user instantiates a PatchedConicLaunch OptimalControlFunction, it must be of type AlgebraicConstraint.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - Function
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_PatchedConicLaunch_PhaseList:
.. list-table:: User interface specification for OptimalControlFunction.PhaseList for OptimalControlFunction.Function = PatchedConicLaunch.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:PatchedConicLaunch
    * - Field Name
      - PhaseList
    * - Data Type
      - GMAT List
    * - Default Value
      - Empty list
    * - Allowed Values
      - Name of a single existing Phase resource and either Initial or Final. E.g., :math:`\texttt{PhaseList} = \left\{\texttt{Phase1.Initial}\right\}` or :math:`\texttt{PhaseList} = \left\{\texttt{Phase2.Final}\right\}`.
    * - Description
      - Sets on which phase and at which boundary of that phase the PatchedConicLaunch is to be evaluated.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_PatchedConicLaunch_SetModelParameter('CentralBody', 'body name'):
.. list-table:: User interface specification for OptimalControlFunction.SetModelParameter('CentralBody', 'body name') for OptimalControlFunction.Function = PatchedConicLaunch.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:PatchedConicLaunch
    * - Field Name
      - SetModelParameter('CentralBody', 'body name')
    * - Data Type
      - Tuple
    * - Default Value
      - N/A
    * - Allowed Values
      - The first element of the tuple must be a string: CentralBody. The second element of the tuple is a string whose value is a celestial body that an be interpreted by GMAT and whose ephemeris is accessible.
    * - Description
      - Sets the central body of the patched-conic launch.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_PatchedConicLaunch_SetModelParameter('EMTGLaunchVehicleOptionsFile', 'launch vehicle options file'):
.. list-table:: User interface specification for OptimalControlFunction.SetModelParameter('EMTGLaunchVehicleOptionsFile', 'launch vehicle options file') for OptimalControlFunction.Function = PatchedConicLaunch.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:PatchedConicLaunch
    * - Field Name
      - SetModelParameter('EMTGLaunchVehicleOptionsFile', 'launch vehicle options file')
    * - Data Type
      - Tuple
    * - Default Value
      - N/A
    * - Allowed Values
      - The first element of the tuple must be a string: EMTGLaunchVehicleOptionsFile. The second element of the tuple is a string containing the name (including path relative to the GMAT executable directory) of an EMTG launch vehicle options file.
    * - Description
      - Sets the EMTG launch vehicle file in which the launch vehicle characteristics are described.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_PatchedConicLaunch_SetModelParameter('VehicleName', 'name of launch vehicle'):
.. list-table:: User interface specification for OptimalControlFunction.SetModelParameter('VehicleName', 'name of launch vehicle') for OptimalControlFunction.Function = PatchedConicLaunch.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:PatchedConicLaunch
    * - Field Name
      - SetModelParameter('VehicleName', 'name of launch vehicle')
    * - Data Type
      - Tuple
    * - Default Value
      - N/A
    * - Allowed Values
      - The first element of the tuple must be a string: VehicleName. The second element of the tuple is the name of a launch vehicle described in the specified EMTG launch vehicle options file.
    * - Description
      - Sets the specific name of the launch vehicle within the provided EMTG launch vehicle options file.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - SetModelParameter('EMTGLaunchVehicleOptionsFile', 'launch vehicle options file')
    * - Access
      - set
    * - Interfaces
      - script

|

OptimalControlFunction for Function Field = CelestialBodyRendezvous
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_OptimalControlFunction_CelestialBodyRendezvous_Type:
.. list-table:: User interface specification for OptimalControlFunction.Type for OptimalControlFunction.Function = CelestialBodyRendezvous.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:CelestialBodyRendezvous
    * - Field Name
      - Type
    * - Data Type
      - String
    * - Default Value
      - AlgebraicConstraint
    * - Allowed Values
      - AlgebraicConstraint
    * - Description
      - When the user instantiates a CelestialBodyRendezvous OptimalControlFunction, it must be of type AlgebraicConstraint.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_CelestialBodyRendezvous_PhaseList:
.. list-table:: User interface specification for OptimalControlFunction.PhaseList for OptimalControlFunction.Function = CelestialBodyRendezvous.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:CelestialBodyRendezvous
    * - Field Name
      - PhaseList
    * - Data Type
      - GMAT List
    * - Default Value
      - Empty list
    * - Allowed Values
      - Name of a single existing Phase resource and either Initial or Final. E.g., :math:`\texttt{PhaseList} = \left\{\texttt{Phase1.Initial}\right\}` or :math:`\texttt{PhaseList} = \left\{\texttt{Phase2.Final}\right\}`.
    * - Description
      - Sets on which phase and at which boundary of that phase the CelestialBodyRendezvous is to be evaluated.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_CelestialBodyRendezvous_SetModelParameter('CelestialBody', 'body name'):
.. list-table:: User interface specification for OptimalControlFunction.SetModelParameter('CelestialBody', 'body name') for OptimalControlFunction.Function = CelestialBodyRendezvous.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:CelestialBodyRendezvous
    * - Field Name
      - SetModelParameter('CelestialBody', 'body name')
    * - Data Type
      - Tuple
    * - Default Value
      - N/A
    * - Allowed Values
      - The first element of the tuple must be a string: CelestialBody. The second element of the tuple is a string whose value is a celestial body that an be interpreted by GMAT and whose ephemeris is accessible.
    * - Description
      - Sets the celestial body with which the spacecraft is to rendezvous.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

OptimalControlFunction for Function Field = IntegratedFlyby
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. _GMATOC_UISpec_OptimalControlFunction_IntegratedFlyby_Type:
.. list-table:: User interface specification for OptimalControlFunction.Type for OptimalControlFunction.Function = IntegratedFlyby.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:IntegratedFlyby
    * - Field Name
      - Type
    * - Data Type
      - String
    * - Default Value
      - AlgebraicConstraint
    * - Allowed Values
      - AlgebraicConstraint
    * - Description
      - When the user instantiates an IntegratedFlyby OptimalControlFunction, it must be of type AlgebraicConstraint.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_IntegratedFlyby_PhaseList:
.. list-table:: User interface specification for OptimalControlFunction.PhaseList for OptimalControlFunction.Function = IntegratedFlyby.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:IntegratedFlyby
    * - Field Name
      - PhaseList
    * - Data Type
      - GMAT List
    * - Default Value
      - empty list
    * - Allowed Values
      - List must consist of exactly 2 existing Phase resources and either Initial and Final on both. E.g., :math:`\texttt{PhaseList} = \left\{\texttt{Phase1.Final, Phase2.Initial}\right\}`.
    * - Description
      - Sets the phase preceeding the flyby event and the phase following the flyby event.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_IntegratedFlyby_SetModelParameter('CelestialBody', 'body name'):
.. list-table:: User interface specification for OptimalControlFunction.SetModelParameter('CelestialBody', 'body name') for OptimalControlFunction.Function = IntegratedFlyby.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:IntegratedFlyby
    * - Field Name
      - SetModelParameter('CelestialBody', 'body name')
    * - Data Type
      - Tuple
    * - Default Value
      - N/A
    * - Allowed Values
      - The first element of the tuple must be a string: CelestialBody. The second element of the tuple is a string whose value is a celestial body that an be interpreted by GMAT and whose ephemeris is accessible.
    * - Description
      - Sets the celestial body that is the central body of the integrated flyby.
    * - Notes
      - N/A
    * - Units
      - N/A
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_IntegratedFlyby_SetModelParameter('PeriapsisRadiusLowerBound', value):
.. list-table:: User interface specification for OptimalControlFunction.SetModelParameter('PeriapsisRadiusLowerBound', value) for OptimalControlFunction.Function = IntegratedFlyby.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:IntegratedFlyby
    * - Field Name
      - SetModelParameter('PeriapsisRadiusLowerBound', value)
    * - Data Type
      - Tuple
    * - Default Value
      - 6978
    * - Allowed Values
      - First element of tuple must be a string: PeriapsisRadiusLowerBound. The second element of the tuple is a :math:`0 < \texttt{Real} < \texttt{Inf}`.
    * - Description
      - Lower bound of periapse of flyby with respect to the central body's center of mass.
    * - Notes
      - N/A
    * - Units
      - km
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

|

.. _GMATOC_UISpec_OptimalControlFunction_IntegratedFlyby_SetModelParameter('PeriapsisRadiusUpperBound', value):
.. list-table:: User interface specification for OptimalControlFunction.SetModelParameter('PeriapsisRadiusUpperBound', value) for OptimalControlFunction.Function = IntegratedFlyby.
    :widths: 50 50
    :header-rows: 0

    * - Resource/Command Name
      - OptimalControlFunction:IntegratedFlyby
    * - Field Name
      - SetModelParameter('PeriapsisRadiusUpperBound', value)
    * - Data Type
      - Tuple
    * - Default Value
      - 1000000
    * - Allowed Values
      - First element of tuple must be a string: PeriapsisRadiusUpperBound. The second element of the tuple is a :math:`0 < \texttt{Real} < \texttt{Inf}`.
    * - Description
      - Upper bound of periapse of flyby with respect to the central body's center of mass.
    * - Notes
      - N/A
    * - Units
      - km
    * - Field Couplings
      - N/A
    * - Access
      - set
    * - Interfaces
      - script

