//------------------------------------------------------------------------------
//                              Trajectory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan / NASA
// Created: 2016.09.14
//
/**
 * From original MATLAB prototype:
 */
//------------------------------------------------------------------------------
#include "SnoptOptimizer.hpp"
#include "Trajectory.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BOUNDS
//#define DEBUG_TRAJECTORY
//#define DEBUG_TRAJECTORY_INIT
//#define DEBUG_SPARSITY
//#define DEBUG_TRAJECTORY_BOUNDS
//#define DEBUG_WRITE_DATA
//#define DEBUG_DECONSTRUCT
//#define DEBUG_PHASE_DATA   // reporting phase data (constraints, etc.)
//#define DEBUG_REPORT_DATA  // reporting Bound Data//#define DEBUG_HESSIAN // YK mod IPOPT branch

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
Trajectory::Trajectory() :
   costLowerBound         (0.0),
   costUpperBound         (0.0),
   guessFunctionName      (""),
   plotFunctionName       (""),
   pathFunction           (NULL),
   pointFunction          (NULL),
   showPlot               (false), // needed?
   plotUpdateRate         (5),     // needed?
   maxMeshRefinementCount (0),
   ifScaling              (false),
   costScaling            (1.0),
   isNewMesh              (false),
   totalNumDecisionParams (0),
   numPhases              (0),
   totalNumConstraints    (0),
   numBoundFunctions      (0),
   costFunction           (0.0),
   maxConViolation        (0.0),
   wasFeasiblityAchieved  (false),
   bestFeasCurrMesh       (std::numeric_limits<Real>::infinity()),
   bestCostCurrMesh       (std::numeric_limits<Real>::infinity()),
   bestOptCurrMesh        (std::numeric_limits<Real>::infinity()),
   trajOptimizer          (NULL),
   pointFunctionManager   (NULL),
   scaleHelper            (NULL),
   isOptimizing           (false),
   isFinished             (false),
   plotUpdateCounter      (1),     // needed?
   displayDebugStatus     (false),
   isPerturbing           (false),
   numFunEvals            (0),
   isMeshRefining         (false),
   meshRefinementCount    (0),
   allowFailedMeshOptimizations(false),
   meshGuessMode          ("LastSolutionMostRecentMesh"),
   guessMaxConViol        (std::numeric_limits<Real>::infinity()),
   guessCostVal           (std::numeric_limits<Real>::infinity()),
   csaltExecInterface     (NULL),
   csaltState             ("Initializing"),
   applicationType        ("Console"),
   optExitFlag            (-1),
   snoptConsoleOutputLevel (1)
{
   pointFunctionManager = new UserPointFunctionManager();
   trajOptimizer        = new SnoptOptimizer(this);}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
Trajectory::Trajectory(const Trajectory &copy) :
   costLowerBound         (copy.costLowerBound),
   costUpperBound         (copy.costUpperBound),
   guessFunctionName      (copy.guessFunctionName),
   plotFunctionName       (copy.plotFunctionName),
   pathFunction           (NULL), // copy.pathFunction),  GMT-7025
   pointFunction          (NULL), // copy.pointFunction), GMT-7025
   showPlot               (copy.showPlot), // needed?
   plotUpdateRate         (copy.plotUpdateRate),     // needed?
   maxMeshRefinementCount (copy.maxMeshRefinementCount),
   ifScaling              (copy.ifScaling),
   costScaling            (copy.costScaling),
   isNewMesh              (copy.isNewMesh),
   totalNumDecisionParams (copy.totalNumDecisionParams),
   numPhases              (copy.numPhases),
   totalNumConstraints    (copy.totalNumConstraints),
   numBoundFunctions      (copy.numBoundFunctions),
   costFunction           (copy.costFunction),
   maxConViolation        (copy.maxConViolation),
   wasFeasiblityAchieved  (copy.wasFeasiblityAchieved),
   bestFeasCurrMesh       (copy.bestFeasCurrMesh),
   bestCostCurrMesh       (copy.bestCostCurrMesh),
   bestOptCurrMesh        (copy.bestOptCurrMesh),
   trajOptimizer          (NULL),
   pointFunctionManager   (NULL),  // WRONG? Clone?
   scaleHelper            (NULL),
   isOptimizing           (copy.isOptimizing),
   isFinished             (copy.isFinished),
   plotUpdateCounter      (copy.plotUpdateCounter),     // needed?
   displayDebugStatus     (copy.displayDebugStatus),
   isPerturbing           (copy.isPerturbing),
   numFunEvals            (copy.isPerturbing),
   isMeshRefining         (copy.isMeshRefining),
   meshRefinementCount    (copy.meshRefinementCount),
   allowFailedMeshOptimizations(copy.allowFailedMeshOptimizations),
   meshGuessMode          (copy.meshGuessMode),
   guessMaxConViol        (copy.guessMaxConViol),
   guessCostVal           (copy.guessCostVal),
   csaltExecInterface     (copy.csaltExecInterface),
   csaltState             (copy.csaltState),
   applicationType        (copy.applicationType),
   optExitFlag            (copy.optExitFlag),
   snoptConsoleOutputLevel (copy.snoptConsoleOutputLevel)
{
   pointFunctionManager = new UserPointFunctionManager();
   trajOptimizer        = new SnoptOptimizer(this);
   CopyArrays(copy);
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
Trajectory& Trajectory::operator=(const Trajectory &copy)
{
   if (&copy == this)
      return *this;
   costLowerBound         = copy.costLowerBound;
   costUpperBound         = copy.costUpperBound;
   guessFunctionName      = copy.guessFunctionName;
   plotFunctionName       = copy.plotFunctionName;
   pathFunction           = NULL; // copy.pathFunction;  GMT-7025
   pointFunction          = NULL; // copy.pointFunction;  GMT-7025
   showPlot               = copy.showPlot; // needed?
   plotUpdateRate         = copy.plotUpdateRate;     // needed?
   maxMeshRefinementCount = copy.maxMeshRefinementCount;
   ifScaling              = copy.ifScaling;
   costScaling            = copy.costScaling;
   isNewMesh              = copy.isNewMesh;
   totalNumDecisionParams = copy.totalNumDecisionParams;
   numPhases              = copy.numPhases;
   totalNumConstraints    = copy.totalNumConstraints;
   numBoundFunctions      = copy.numBoundFunctions;
   costFunction           = copy.costFunction;
   maxConViolation        = copy.maxConViolation;
   wasFeasiblityAchieved  = copy.wasFeasiblityAchieved;
   bestFeasCurrMesh       = copy.bestFeasCurrMesh;
   bestCostCurrMesh       = copy.bestCostCurrMesh;
   bestOptCurrMesh        = copy.bestOptCurrMesh;
   // trajOptimizer          = NULL; // copy.trajOptimizer;         // WRONG?? Clone? NULL?
   // pointFunctionManager   = NULL; // copy.pointFunctionManager;  // WRONG?? Clone? NULL?
   scaleHelper            = copy.scaleHelper;    // Or Clone??
   isOptimizing           = copy.isOptimizing;
   isFinished             = copy.isFinished;
   plotUpdateCounter      = copy.plotUpdateCounter;     // needed?
   displayDebugStatus     = copy.displayDebugStatus;
   isPerturbing           = copy.isPerturbing;
   numFunEvals            = copy.isPerturbing;
   isMeshRefining         = copy.isMeshRefining;
   meshRefinementCount    = copy.meshRefinementCount;
   allowFailedMeshOptimizations = copy.allowFailedMeshOptimizations;
   meshGuessMode          = copy.meshGuessMode;
   guessMaxConViol        = copy.guessMaxConViol;
   guessCostVal           = copy.guessCostVal;
   csaltExecInterface     = copy.csaltExecInterface;
   csaltState             = copy.csaltState;
   applicationType        = copy.applicationType;
   optExitFlag            = copy.optExitFlag;
   snoptConsoleOutputLevel = copy.snoptConsoleOutputLevel;

   // Need new ones of these
   if (pointFunctionManager)
      delete pointFunctionManager;
   pointFunctionManager = new UserPointFunctionManager();
   if (trajOptimizer)
      delete trajOptimizer;
   trajOptimizer        = new SnoptOptimizer(this);
   CopyArrays(copy);

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
Trajectory::~Trajectory()
{
   #ifdef DEBUG_DECONSTRUCT
      std::cout << "In Trajectory deconstructor:" << std::endl;
      std::cout << "  pathFunction         = <" << pathFunction << ">\n";
      std::cout << "  pointFunction        = <" << pointFunction << ">\n";
      std::cout << "  trajOptimizer        = <" << trajOptimizer << ">\n";
      std::cout << "  pointFunctionManager = <" << pointFunctionManager << ">\n";
      std::cout << "  scaleHelper          = <" << scaleHelper << ">\n";
   #endif
   
   // Commented out to avoid double-delete (and crash) GMT-7025
   /*
   if (pathFunction)
   {
      #ifdef DEBUG_DECONSTRUCT
         std::cout << "  deleting pathFunction at <" << pathFunction << ">\n";
      #endif
      delete pathFunction;
   }
   if (pointFunction)
   {
      #ifdef DEBUG_DECONSTRUCT
         std::cout << "  deleting pointFunction at <" << pointFunction << ">\n";
      #endif
      delete pointFunction;
   }
   */
   if (trajOptimizer)
   {
      #ifdef DEBUG_DECONSTRUCT
         std::cout << "  deleting trajOptimizer at <" << trajOptimizer << ">\n";
      #endif
      delete trajOptimizer;
   }
   if (pointFunctionManager)
   {
      #ifdef DEBUG_DECONSTRUCT
         std::cout << "  deleting pointFunctionManager at <" << pointFunctionManager << ">\n";
      #endif
      delete pointFunctionManager;
   }
   if (scaleHelper)
   {
      #ifdef DEBUG_DECONSTRUCT
         std::cout << "  deleting scaleHelper at <" << scaleHelper << ">\n";
      #endif
      delete scaleHelper;
   }

   #ifdef DEBUG_DECONSTRUCT
      std::cout << "  exiting destructor for Trajectory!\n";
   #endif
}


//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the Trajectory, all phases, and helper classes
 *
 */
//------------------------------------------------------------------------------
void Trajectory::Initialize()
{
   #ifdef DEBUG_TRAJECTORY_INIT
   MessageInterface::ShowMessage("Entering Trajectory::Initialize ...\n");
   std::cout << "Entering Trajectory::Initialize ...\n";
   #endif
   // Assign phase numbers as they are required early in the
   // intialization process.
   
   for (UnsignedInt phaseIdx = 0; phaseIdx < phaseList.size(); phaseIdx++)
      phaseList.at(phaseIdx)->SetPhaseNumber(phaseIdx);
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage("In Trajectory::Initialize, "
                                 "optimizer created and phase numbers set\n");
      std::cout << "optimizer created and phase numbers set\n";
   #endif
   
   InitializePhases();
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                        "In Trajectory::Initialize, phases are initialized\n");
      std::cout << "In Trajectory::Initialize, phases are initialized\n";
   #endif
   InitializePointFunctions();
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
               "In Trajectory::Initialize, point functions are initialized\n");
      std::cout << "In Trajectory::Initialize, point functions are initialized\n";
   #endif
   numBoundFunctions = pointFunctionManager->GetNumberBoundaryFunctions();
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                           "In Trajectory::Initialize, about to set bounds\n");
   #endif
   SetBounds();
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                     "In Trajectory::Initialize, about to set initial guess\n");
   #endif
   SetInitialGuess();
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                  "In Trajectory::Initialize, about to initialize optimizer\n");
   #endif


      delete trajOptimizer;
      trajOptimizer        = new SnoptOptimizer(this);

   trajOptimizer->Initialize();
   trajOptimizer->SetSnoptConsoleOutputLevel(this->snoptConsoleOutputLevel);
   // %obj.WriteSetupReport();
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                     "In Trajectory::Initialize, calling PrepareToOptimize\n");
   #endif
   PrepareToOptimize();
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                  "In Trajectory::Initialize, calling InitializeJacHelper\n");
   #endif
   if (ifScaling)
   {
   #ifdef DEBUG_TRAJECTORY_INIT
         MessageInterface::ShowMessage(
               "In Trajectory::Initialize, calling InitializeScaleUtility\n");
   #endif
      InitializeScaleUtility();
   }

   if (csaltExecInterface)
      csaltExecInterface->SetPhaseList(phaseList);

   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                        "In Trajectory::Initialize, INITIALIZATION COMPLETE\n");
   #endif
}


//------------------------------------------------------------------------------
// void InitializeScaleUtility()
//------------------------------------------------------------------------------
/**
 * Initializes the scale utility if the problem is being auto-scaled.
 *
 */
//------------------------------------------------------------------------------
void Trajectory::InitializeScaleUtility()
{
   // Get the sparsity pattern
   IntegerArray jacRowIdx, jacColIdx;
   Rvector      s;
   SparseMatrixUtil::GetThreeVectorForm(&sparsityPattern, jacRowIdx,
                                           jacColIdx, s);
   scaleHelper = new ScaleUtility();

   // Initialize the utility's arrays
   scaleHelper->Initialize(totalNumDecisionParams,totalNumConstraints,
                           jacRowIdx,jacColIdx);
   // Set the decision vector scaling based on its bounds
   scaleHelper->SetDecVecScalingBounds(decisionVecUpperBound,
                                       decisionVecLowerBound);
   // Scale the cost function
   scaleHelper->SetCostScalingUserDefined(costScaling);
			
   // Set the scale utility's helper arrays from the traj class
   scaleHelper->SetWhichStateVar(whichStateVar);
   scaleHelper->SetIfDefect(ifDefect);
			
   // Set the decision vector to get an initial Jacobian
   SetDecisionVector(decisionVector); // ,true);
   RSMatrix J = GetJacobian();  // true);
			
   // Set the constraint scaling
   scaleHelper->SetConstraintScalingDefectAndUser(J);
}

//------------------------------------------------------------------------------
// void InitializePointFunctions()
//------------------------------------------------------------------------------
/**
 * Intializes point function manager and user point function
 *
 */
//------------------------------------------------------------------------------
void Trajectory::InitializePointFunctions()
{
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                                 "In Trajectory::InitializePointFunctions\n");
      std::cout << "In Trajectory::InitializePointFunctions\n";
      if (!pointFunctionManager)
         MessageInterface::ShowMessage("pointFunctionManager is NULL!!!\n");
      MessageInterface::ShowMessage("=== isMeshRefining = %s\n",
                                    (isMeshRefining? "true" : "false"));
   #endif

   // Create vector of pointers to all phases - N/N in C++
   if (isMeshRefining)
      pointFunctionManager->SetIsInitializing(true);

   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                        "In Trajectory::InitializePointFunctions, "
                        "pointFunctionManager is about to be initialized!\n");
      std::cout << "pointFunctionManager is about to be initialized!\n";
   #endif

   pointFunctionManager->Initialize(pointFunction,phaseList,
                                    totalNumDecisionParams,decVecStartIdx);
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage("In Trajectory::InitializePointFunctions, "
                                    "pointFunctionManager is initialized!\n");
      std::cout << "pointFunctionManager is initialized!\n";
   #endif
   numBoundFunctions = pointFunctionManager->GetNumberBoundaryFunctions();
   
   totalNumConstraints += numBoundFunctions;
			
   for (Integer ii = 0; ii < numBoundFunctions; ii++)
   {
      ifDefect.push_back(false);
      whichStateVar.push_back(0);
   }
   #ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                                    "Exiting Trajectory::InitializePointFunctions\n");
      std::cout << "Exiting Trajectory::InitializePointFunctions\n";
   #endif
}

//------------------------------------------------------------------------------
// void PrepareToOptimize()
//------------------------------------------------------------------------------
/**
 * Performs actions that must be done after Initialize() but before Optimize()
 *
 */
//------------------------------------------------------------------------------
void Trajectory::PrepareToOptimize()
{
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
      phaseList.at(phaseIdx)->PrepareToOptimize();
   SetSparsityPattern();
}

//------------------------------------------------------------------------------
// bool PrepareMeshGuess(Integer exitFlag)
//------------------------------------------------------------------------------
/**
* Prepares optimization for mesh refinement using user set options
*
* @param <exitFlag> The exitFlag received from the most recent optimization run
*
*/
//------------------------------------------------------------------------------
bool Trajectory::PrepareMeshGuess(Integer exitFlag)
{
   // If optimizer failed for unrecoverable reason an failed mesh optimizations
   // are not allowed, exit and issue warning.
   // positive exitFlag indicates convergence, exitFlag = 0 indicates
   // possibly recoverable failure. (i.e. try mesh refinement)
   // exitFlag < 0 means unrecoverable failure
   if (exitFlag <= 0 && !allowFailedMeshOptimizations)
   {
      std::stringstream warningMessage("");
      warningMessage << std::endl;
      warningMessage << " WARNING: ";
      warningMessage << "The optimizer encountered an unrecoverable ";
      warningMessage << "issue. Terminating CSALT iteration without ";
      warningMessage << "testing mesh criteria. Check the problem ";
      warningMessage << "formulation and/or scaling." << std::endl;
      warningMessage << std::endl;
      MessageInterface::ShowMessage(warningMessage.str());
      return false;
   }
   else
   {
      // Use the best solution found during optimization
      if (meshGuessMode != "LastSolutionMostRecentMesh")
      {
         SetDecisionVector(meshGuessDecVec);
         GetCostConstraintFunctions();
      }
      // Otherwise the solution returned on exit is used automatically (the 
      // LastSolutionMostRecentMesh option) 
      return true;
   }
}

//------------------------------------------------------------------------------
// void SetDecisionVector(Rvector decVec)
//------------------------------------------------------------------------------
/**
 * Sets decision vector on the phases.  If scaling, the incoming vector is
 * non-dimensional; if not scaling, it is non-dimensional.
 *
 * @param <decVec>   the input decision vector
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetDecisionVector(Rvector decVec)
{
   if (decisionVector.GetSize() != totalNumDecisionParams)
   {
      std::string errmsg = "Length of decisionVector must be ";
      errmsg += "equal to totalNumDecisionParams\n";
      throw LowThrustException(errmsg);
   }
   
   // Now loop over phases and set on each phase
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      Integer sz = decVecEndIdx.at(phaseIdx) - decVecStartIdx.at(phaseIdx) + 1;
      Rvector dv(sz);
      for (Integer ii = 0; ii < sz; ii++)
         dv(ii) = decVec(decVecStartIdx.at(phaseIdx) + ii);
      phaseList.at(phaseIdx)->SetDecisionVector(dv);
   }
   
//   Handle plotting - TBD
}

//------------------------------------------------------------------------------
// void UpdateGuessForMesh()
//------------------------------------------------------------------------------
/**
* Checks if the new optimization solution is better than the current best by
* comparing which solution has a smaller cost function value.  If the solution
* was found to be infeasible, then max constraint violations are checked
* instead.  Depending on the mesh guess mode being used, this may be saved as
* the new guess for the next mesh iteration.
* 
* LastSolutionMostRecentMesh: The last solution from the previous mesh is used
* BestSolutionMostRecentMesh: The best solution of the most recent mesh 
* iteration is used
* BestSolutionAnyMesh: The best solution found in any mesh during optimization
* is used
*/
//------------------------------------------------------------------------------
void Trajectory::UpdateGuessForMesh()
{
   Rvector decVec(totalNumDecisionParams);
   Integer lowIdx = 0;
   Integer highIdx;
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      highIdx = lowIdx + numPhaseDecisionParams.at(phaseIdx) - 1;
      DecisionVector *dv = phaseList.at(phaseIdx)->GetDecisionVector();
      Rvector        dv2 = dv->GetDecisionVector();
      Integer idx = 0;
      for (Integer ii = lowIdx; ii <= highIdx; ii++)
      {
         decVec(ii) = dv2(idx++);
      }
      lowIdx = highIdx + 1;
   }

   if (meshGuessMode == "LastSolutionMostRecentMesh")
   {
      // If we are just using the last solution found, update the decision
      // vector and return
      if (meshGuessDecVec.GetSize() != decVec.GetSize())
         meshGuessDecVec.SetSize(decVec.GetSize());
      meshGuessDecVec = decVec;
      return;
   }
   else if (meshGuessMode == "BestSolutionMostRecentMesh" && isNewMesh)
   {
      // Reset max violation and best cost function values
      guessMaxConViol = std::numeric_limits<Real>::infinity();
      guessCostVal = std::numeric_limits<Real>::infinity();
      isNewMesh = false;
   }

   Real currentMaxConViolation;
   Real currentCost;
   ComputeMaxConstraintViolation(currentMaxConViolation, currentCost);

   // If we are maximizing, change the sign on cost
   Integer numOptModes = optimizationMode.size();
   std::string currOptMode;
   if (meshRefinementCount < numOptModes)
      currOptMode = optimizationMode.at(meshRefinementCount);
   else
      currOptMode = optimizationMode.at(numOptModes - 1);
   if (currOptMode == "Maximize")
      currentCost = -currentCost;

   Real currentFeasibilityTol;
   if (feasibilityTolerances.IsSized())
   {
      Integer numFeasibilityTols = feasibilityTolerances.GetSize();
      if (meshRefinementCount < numFeasibilityTols)
         currentFeasibilityTol = feasibilityTolerances(meshRefinementCount);
      else
         currentFeasibilityTol = feasibilityTolerances(numFeasibilityTols - 1);
   }
   else
      currentFeasibilityTol = 1e-6;

   if (currentMaxConViolation < currentFeasibilityTol)
   {
      // Solution is feasible, pick the one with the best cost, or if the cost
      // didn't see change, check the max constraint violations
      if (currentCost < guessCostVal || 
         ((GmatMathUtil::Abs(currentCost - guessCostVal) <= 1e-17) &&
         currentMaxConViolation < guessMaxConViol))
      {
         if (meshGuessDecVec.GetSize() != decVec.GetSize())
            meshGuessDecVec.SetSize(decVec.GetSize());
         meshGuessDecVec = decVec;
         guessMaxConViol = currentMaxConViolation;
         guessCostVal = currentCost;
      }
   }
   else
   {
      // Solution is not feasible, pick the one with the smallest max
      // constraint violation
      if (currentMaxConViolation < guessMaxConViol)
      {
         if (meshGuessDecVec.GetSize() != decVec.GetSize())
            meshGuessDecVec.SetSize(decVec.GetSize());
         meshGuessDecVec = decVec;
         guessMaxConViol = currentMaxConViolation;
         guessCostVal = currentCost;
      }
   }
}

//------------------------------------------------------------------------------
// void Trajectory::Optimize(const std::string &optFile)
//------------------------------------------------------------------------------
/**
* Initializes the trajectory and performs optimization
* 
* @param optFile    Text file for optimizer output.  If this string is empty,
*                   the file is not written.
*/
//------------------------------------------------------------------------------
void Trajectory::Optimize(const std::string &optFile)
{
    Initialize();
    Rvector decVector = GetDecisionVector();
    Rvector funcVector = GetCostConstraintFunctions();
    Rvector xmul(decVector.GetSize());
    Rvector Fmul(funcVector.GetSize());
    Integer exitFlag;
    Optimize(decVector, funcVector, xmul, Fmul, exitFlag, optFile);
}

//------------------------------------------------------------------------------
// void Optimize(Rvector &decVec, Rvector &F,
//               Rvector &xmul,   Rvector &Fmul,
//               const std::string &optFile = "")
//------------------------------------------------------------------------------
/**
 * Performs optimization on initialized trajectory.
 *
 * @param <decVec>   the decision vector
 * @param <F>        the objective and constraint function values
 * @param <xmul>     the lagrange multipliers for the KKT optimality
 *                   conditions of the state bounds
 * @param <Fmul>     the lagrange multipliers for the KKT optimality
 *                   conditions of the objective and constraint functions
 * @param optFile    Text file for optimizer output.  If this string is empty,
 *                   the file is not written
 *
 * NOTE: Changing the input parameters to this functoin also requires changing
 *       the overloaded version Trajectory::Optimize(const std::string &optFile)
 */
//------------------------------------------------------------------------------
void Trajectory::Optimize(Rvector &decVec,
                          Rvector &F,
                          Rvector &xmul,
                          Rvector &Fmul,
                          Integer &exitFlag,
                          const std::string &optFile)
{
   //   Initialize();  // not necessary
   meshRefinementCount = 0;
   bool isMeshRefinementFinished = false;
   
   bool isMeshRefined; // YK mod
   IntegerArray newMeshIntervalNumPoints;
   Rvector      newMeshIntervalFractions;
   //Real         maxRelErrorInMesh;

   // Set the output file option
   if (optFile != "")
      trajOptimizer->SetOptimizerOutputFile(optFile);

   while (meshRefinementCount <= maxMeshRefinementCount)
   {
      
      // Optimize on the current mesh (first loop is on user's mesh
      isOptimizing      = true;
//      plotUpdateCounter = 1;
      numFunEvals       = 0;
      isMeshRefining    = false;
      
      Integer dvSz = decVec.GetSize();  // or do I use decisionVector here?

      Rvector dvLower;
      Rvector dvUpper;
      
      Rvector funLower;
      Rvector funUpper;
         
      if (ifScaling)
      {
         trajOptimizer->SetScaling(true);
         scaleHelper->ScaleDecisionVector(decVec);
         dvLower.SetSize(dvSz);
         dvUpper.SetSize(dvSz);
         
         for (Integer ii = 0; ii < dvSz; ii++)
         {
            dvLower(ii) = -0.5;
            dvUpper(ii) =  0.5;
         }
         
         Real       costWt = scaleHelper->GetCostWeight();
         Rvector    conWts = scaleHelper->GetConVecWeights();
         Integer    wtsSz  = conWts.GetSize();
         funLower.SetSize(1+wtsSz);
         funUpper.SetSize(1+wtsSz);
         funLower(0) = costLowerBound * costWt;
         funUpper(0) = costUpperBound * costWt;
         for (Integer ii = 0; ii < wtsSz; ii++)
         {
            funLower(ii+1) = allConLowerBound.at(ii) * conWts(ii);
            funUpper(ii+1) = allConUpperBound.at(ii) * conWts(ii);
         }
         
         #ifdef DEBUG_SPARSITY
            Integer rJac = sparsityPattern.size1();
            Integer cJac = sparsityPattern.size2();
            MessageInterface::ShowMessage(
                           " Before call to Optimize(), sparsityPattern size "
                           "= (%d, %d)\n",
                                                rJac, cJac);
            for (Integer cc = 0; cc < cJac; cc++)
            {
               for (Integer rr = 0; rr < rJac; rr++)
               {
                  Real jacTmp = sparsityPattern(rr,cc);
                  if (jacTmp != 0.0)
                  MessageInterface::ShowMessage(
                                       " sparsityPattern(%d, %d) =  %12.10f\n",
                                       rr+1, cc+1, jacTmp);
               }
            }
         #endif


         // Call the optimizer and set deciscion vector based on the output.
         csaltState = "Optimizing";
         if (csaltExecInterface)
            csaltExecInterface->Publish(csaltState);
         // Set the tolerances, etc. here
         SetSNOPTIterationDependentSettings(meshRefinementCount);

         // Reset best feasible solution
         ResetBestFeasibleSolution();
         isNewMesh = true;

         // Optimize
         trajOptimizer->Optimize(decVec, dvLower, dvUpper, funLower, funUpper,
                                 sparsityPattern, F, xmul, Fmul, exitFlag);
         SetDecisionVector(decVec);
         optExitFlag = exitFlag;

         if (exitFlag == -2)
            break;

         if (!PrepareMeshGuess(GetSimplifiedSNOPTExitFlag(exitFlag)))
            return;
         
         #ifdef DEBUG_SPARSITY
            rJac = sparsityPattern.size1();
            cJac = sparsityPattern.size2();
            MessageInterface::ShowMessage(
                                 " AFTER call to Optimizer (with scaling), "
                                 "sparsityPattern size = (%d, %d)\n",
                                          rJac, cJac);
            for (Integer cc = 0; cc < cJac; cc++)
            {
               for (Integer rr = 0; rr < rJac; rr++)
               {
                  Real jacTmp = sparsityPattern(rr,cc);
                  if (jacTmp != 0.0)
                  MessageInterface::ShowMessage(
                                       " sparsityPattern(%d, %d) =  %12.10f\n",
                                       rr+1, cc+1, jacTmp);
               }
            }
         #endif
         // Unscale the problem and answer!
         scaleHelper->UnScaleDecisionVector(decVec);
         F(0) = scaleHelper->UnScaleCostFunction(F(0));  // to here?
         Rvector Ftmp(F.GetSize()-1);
         for (Integer ii = 1; ii < F.GetSize(); ii++)
         {
            Ftmp(ii-1) = F(ii);
         }
         scaleHelper->UnScaleConstraintVector(Ftmp);
         for (Integer ii = 1; ii < F.GetSize(); ii++)
         {
            F(ii) = Ftmp(ii-1);
         }
      }
      else  // no scaling
      {
         trajOptimizer->SetScaling(false);
         dvLower = decisionVecLowerBound;
         dvUpper = decisionVecUpperBound;
         funLower.SetSize(1+allConLowerBound.size());
         funUpper.SetSize(1+allConUpperBound.size());
         funLower(0) = costLowerBound;
         funUpper(0) = costUpperBound;
         // assume allConLowerBound is same size as allConUpperBound
         for (UnsignedInt ii = 0; ii < allConLowerBound.size(); ii++)
         {
            funLower(ii+1) = allConLowerBound.at(ii);
            funUpper(ii+1) = allConUpperBound.at(ii);
         }
         
         #ifdef DEBUG_SPARSITY
            Integer rJac = sparsityPattern.size1();
            Integer cJac = sparsityPattern.size2();
            MessageInterface::ShowMessage(
                              " Before call to Optimizer (with no scaling), "
                              "sparsityPattern size = (%d, %d)\n",
                              rJac, cJac);
            for (Integer cc = 0; cc < cJac; cc++)
            {
               for (Integer rr = 0; rr < rJac; rr++)
               {
                  Real jacTmp = sparsityPattern(rr,cc);
                  if (jacTmp != 0.0)
                  MessageInterface::ShowMessage(
                                       " sparsityPattern(%d, %d) =  %12.10f\n",
                                       rr+1, cc+1, jacTmp);
               }
            }
         #endif
            
         #ifdef DEBUG_SPARSITY
            Integer cJac2 = decVec.GetSize();
            MessageInterface::ShowMessage(
                              " Before call to Optimize(), decVec size = %d\n",
                              cJac2);
            for (Integer cc = 0; cc < cJac2; cc++)
            {
               Real jacTmp = decVec(cc);
               MessageInterface::ShowMessage(" decVec(%d) =  %12.10f\n",
                                             cc+1, jacTmp);
            
            }
         #endif

         #ifdef DEBUG_SPARSITY
            RSMatrix  tmpData  = GetJacobian();
            Integer rJac3      = tmpData.size1();
            Integer cJac3      = tmpData.size2();
            MessageInterface::ShowMessage(
                           " Before call to Optimizer (with no scaling), "
                           "Jacobian size = (%d, %d)\n", rJac3, cJac3);
            for (Integer cc = 0; cc < cJac3; cc++)
            {
               for (Integer rr = 0; rr < rJac3; rr++)
               {
                  Real jacTmp = tmpData(rr, cc);
                  if (jacTmp != 0.0)
                     MessageInterface::ShowMessage(
                                             "Jacobian(%d, %d) =  %12.10f\n",
                                             rr + 1, cc + 1, jacTmp);
               }
            }
            tmpData = GetSparsityPattern();
            rJac = tmpData.size1();
            cJac = tmpData.size2();
            MessageInterface::ShowMessage(
                           " Before call to Optimizer (with no scaling), "
                           "SparsityPattern size = (%d, %d)\n",
                           rJac, cJac);
            for (Integer cc = 0; cc < cJac; cc++)
            {
               for (Integer rr = 0; rr < rJac; rr++)
               {
                  Real jacTmp = tmpData(rr, cc);
                  if (jacTmp != 0.0)
                     MessageInterface::ShowMessage(
                                       "SparsityPattern(%d, %d) =  %12.10f\n",
                                       rr + 1, cc + 1, jacTmp);
               }
            }
         #endif
#ifdef DEBUG_PHASE_DATA
            for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
            {
               phaseList.at(phaseIdx)->ReportAllData();
            };
#endif
                   
         // Call the optimizer and set deciscion vector based on the output.
         csaltState = "Optimizing";
         if (csaltExecInterface)
            csaltExecInterface->Publish(csaltState);
         // Set the tolerances, etc. here
         SetSNOPTIterationDependentSettings(meshRefinementCount);

         #ifdef DEBUG_BOUNDS
            MessageInterface::ShowMessage(
            "SNOPTFunctionWrapper --- dvLower = %s\n",
            dvLower.ToString(16).c_str());
         #endif

         #ifdef DEBUG_BOUNDS
            MessageInterface::ShowMessage(
               "SNOPTFunctionWrapper --- dvUpper = %s\n",
               dvUpper.ToString(16).c_str());
         #endif

         #ifdef DEBUG_BOUNDS
            MessageInterface::ShowMessage(
               "SNOPTFunctionWrapper --- funLower = %s\n",
               funLower.ToString(16).c_str());
         #endif

         #ifdef DEBUG_BOUNDS
            MessageInterface::ShowMessage(
               "SNOPTFunctionWrapper --- funUpper = %s\n",
               funUpper.ToString(16).c_str());
         #endif

         // Reset best feasible solution
         ResetBestFeasibleSolution();
         isNewMesh = true;

         // Optimize
         trajOptimizer->Optimize(decVec, dvLower, dvUpper, funLower, funUpper,

                                 sparsityPattern, F, xmul, Fmul, exitFlag);
         SetDecisionVector(decVec);
         optExitFlag = exitFlag;

         if (GetSimplifiedSNOPTExitFlag(exitFlag) == -2)
            break;

#ifdef DEBUG_PHASE_DATA
         for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
         {
            phaseList.at(phaseIdx)->ReportAllData();
         };
#endif
         if (!PrepareMeshGuess(GetSimplifiedSNOPTExitFlag(exitFlag)))
            return;


         #ifdef DEBUG_SPARSITY
            rJac = sparsityPattern.size1();
            cJac = sparsityPattern.size2();
            MessageInterface::ShowMessage(
                     " AFTER call to Optimizer (with no scaling), "
                     "sparsityPattern size = (%d, %d)\n", rJac, cJac);
            for (Integer cc = 0; cc < cJac; cc++)
            {
               for (Integer rr = 0; rr < rJac; rr++)
               {
                  Real jacTmp = sparsityPattern(rr,cc);
                  if (jacTmp != 0.0)
                  MessageInterface::ShowMessage(
                                       " sparsityPattern(%d, %d) =  %12.10f\n",
                                       rr+1, cc+1, jacTmp);
               }
            }
         #endif
      }
      
      bool ifUpdateMeshInterval = true;
      // Check to see if mesh refinement is required
      if (meshRefinementCount == maxMeshRefinementCount)
         ifUpdateMeshInterval = false;

      isMeshRefinementFinished = true;
      isMeshRefining           = true;
      if (ifUpdateMeshInterval)
      {
         MessageInterface::ShowMessage(
            "\n Mesh refinement iteration %i\n", meshRefinementCount + 1);
         csaltState = "MeshRefining";
         if (csaltExecInterface)
            csaltExecInterface->Publish(csaltState);
         for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
         {
            MessageInterface::ShowMessage(" Refining mesh for phase number ");
            MessageInterface::ShowMessage("%i \n", phaseIdx + 1);
            phaseList.at(phaseIdx)->RefineMesh(ifUpdateMeshInterval,
               isMeshRefined);
            if (isMeshRefined)
               isMeshRefinementFinished = false;
         }
      }
      
      // Get ready to optimize again, or break out if done.
      if (isMeshRefinementFinished)
      {
         // @todo  add message here
         // Done, so break out of the loop and finalize
//         disp(' ')
//         disp(['Success: Mesh Refinement Criteria Were Satisfied in ' ...
//               num2str(obj.meshRefinementCount) ' Iterations'])
         csaltState = "Finalizing";
         break;
      }
      else if (ifUpdateMeshInterval)
      {
         // Not done, so prepare for next optimization
         csaltState = "ReInitializingMesh";
         if (csaltExecInterface)
            csaltExecInterface->Publish(csaltState);
         Initialize();
         isOptimizing      = true;
         numFunEvals       = 0;

         // add by YK, resize all the optimization related things
         dvSz = decisionVector.GetSize();
         decVec.SetSize(dvSz);
         decVec = decisionVector;         
         Rvector  allConVec = this->GetCostConstraintFunctions();

         F.SetSize(allConVec.GetSize());
         xmul.SetSize(dvSz);
         Fmul.SetSize(allConVec.GetSize());

         #ifdef DEBUG_SPARSITY
            RSMatrix  tmpData = GetJacobian();
            Integer   rJac    = tmpData.size1();
            Integer   cJac    = tmpData.size2();
            MessageInterface::ShowMessage(
                        " Before call to Optimizer (with no scaling), "
                        "Jacobian size = (%d, %d)\n", rJac, cJac);
            for (Integer cc = 0; cc < cJac; cc++)
            {
               for (Integer rr = 0; rr < rJac; rr++)
               {
                  Real jacTmp = tmpData(rr, cc);
                  if (jacTmp != 0.0)
                     MessageInterface::ShowMessage(
                                             "Jacobian(%d, %d) =  %12.10f\n",
                                             rr + 1, cc + 1, jacTmp);
               }
            }
            tmpData =  GetSparsityPattern();
            rJac = tmpData.size1();
            cJac = tmpData.size2();
            MessageInterface::ShowMessage(
                            " Before call to Optimizer (with no scaling), "
                            "SparsityPattern size = (%d, %d)\n",
                            rJac, cJac);
            for (Integer cc = 0; cc < cJac; cc++)
            {
               for (Integer rr = 0; rr < rJac; rr++)
               {
                  Real jacTmp = tmpData(rr, cc);
                  if (jacTmp != 0.0)
                     MessageInterface::ShowMessage(
                                       "SparsityPattern(%d, %d) =  %12.10f\n",
                                       rr + 1, cc + 1, jacTmp);
               }
            }
         #endif
         meshRefinementCount++;
         // @todo write message here?
//         disp(' ')
//         disp('===========================================')
//         disp(['====  Mesh Refinement Status:  Iteration ' ...
//               num2str(obj.meshRefinementCount)]);
//         disp('===========================================')
      }
      else
      {
         meshRefinementCount++;
      }

   }
         
   // Write out messages for mesh refinement convergence criter
   if (!isMeshRefinementFinished && (maxMeshRefinementCount >= 1))
   {
      // @todo write message here?
//      disp(' ')
//      disp(['Warning: Mesh Refinement Criteria Was Not ' ...
//            'Satisfied Before Maximum Iterations Were Reached'])
      ; // TBD
   }
   else if (!isMeshRefinementFinished && (maxMeshRefinementCount == 0))
   {
      // @todo write message here?
//      disp(' ')
//      disp(['Warning: Mesh Refinement Was Not Performed ' ...
//            'Because Max Iterations Are Set To Zero'])
      ; // TBD
   }

   // If convergence was not achieved, set the decision vector back to 
   // a solution that was the most feasible
   Integer simpleFlag = GetSimplifiedSNOPTExitFlag(optExitFlag);
   if (simpleFlag != 1)
   {
      for (UnsignedInt ii = 0; ii < numPhases; ++ii)
         phaseList.at(ii)->SetDecisionVector(bestFeasibleDecVec.at(ii));
   }
      
   // Store cost and max constraint violation
   ComputeMaxConstraintViolation(maxConViolation, costFunction);

   // Configure house-keeping data
   csaltState = "Finalizing";
   if (csaltExecInterface)
      csaltExecInterface->Publish(csaltState);
   isMeshRefining = false;
   isOptimizing = false;
   isFinished   = true;
//      PlotUserFunction();
}

//------------------------------------------------------------------------------
// RSMatrix GetJacobian()
//------------------------------------------------------------------------------
/**
 * Returns the sparse Jacobian.
 *
 * @return   the sparse Jacobian
 *
 */
//------------------------------------------------------------------------------
RSMatrix Trajectory::GetJacobian()
{
   //  Returns the sparse Jacobian
   Integer rowLow      = 0;
   Integer colLow      = 0;
   Integer funcIdxLow  = 0;
   Integer funcIdxHigh = 0;
   RSMatrix jac = SparseMatrixUtil::GetSparsityPattern(&sparsityPattern, true);
   

   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      // Plus one is for cost function offset.
      rowLow  = conPhaseStartIdx.at(phaseIdx) + 1;
      colLow  = decVecStartIdx.at(phaseIdx);
      
      // @todo need validation of matrix sizes here!

      RSMatrix tmpMatrix = phaseList.at(phaseIdx)->GetCostJacobian();
      SparseMatrixUtil::SetSparseBLockMatrix(jac, 0, colLow,
                                             &tmpMatrix);
      RSMatrix tmpMatrix2 = phaseList.at(phaseIdx)->GetConJacobian();
      SparseMatrixUtil::SetSparseBLockMatrix(jac, rowLow, colLow,
                                 &tmpMatrix2);


      // move out the point function lines out of the loop. YK 2017.09.26
   }

   // handle point functions now
   funcIdxLow = totalNumConstraints - numBoundFunctions + 1;
   /*
   funcIdxHigh = totalNumConstraints;
   IntegerArray funcIdxs;
   for (Integer ii = funcIdxLow; ii <= funcIdxHigh; ii++)
      funcIdxs.push_back(ii);
      */
   pointFunctionManager->EvaluateUserJacobian();
   if (pointFunctionManager->HasBoundaryFunctions())
   {
      IntegerArray rowIdxVec, colIdxVec;
      std::vector<Real> valueVec;
      RSMatrix tmpMatrix3 = pointFunctionManager->ComputeBoundNLPJacobian();

      SparseMatrixUtil::SetSparseBLockMatrix(jac, funcIdxLow, 0, &tmpMatrix3);
      /*
      SparseMatrixUtil::GetThreeVectorForm(
         &tmpMatrix3,
         rowIdxVec, colIdxVec, valueVec);

      // @todo need validation of matrix sizes here!
      for (UnsignedInt ii = 0; ii < valueVec.size(); ii++)
      {
         if (valueVec[ii] != 0.0)
            SparseMatrixUtil::SetElement(jac, funcIdxs.at(rowIdxVec[ii]),
               colIdxVec[ii], valueVec[ii]);
      }
      */
   }
   if (pointFunctionManager->HasCostFunction())
   {
      RSMatrix tmpMatrix4 = pointFunctionManager->ComputeCostNLPJacobian();
      SparseMatrixUtil::SetSparseBLockMatrix(jac, 0, 0, &tmpMatrix4, false);
   }

   return jac;
}


//------------------------------------------------------------------------------
// Rvector GetCostConstraintFunctions()
//------------------------------------------------------------------------------
/**
 * Compute cost and constraint functions
 *
 * @return   the cost and constraint function values
 *
 */
//------------------------------------------------------------------------------
Rvector Trajectory::GetCostConstraintFunctions()
{
   Rvector allFunctions;
   numFunEvals++;
   csaltState = "Optimizing";
   if (csaltExecInterface) 
	   csaltExecInterface->Publish(csaltState);
   
   Rvector conVec     = GetConstraintVector();
   Real    costFun    = GetCostFunction();
   Integer conVecSize = conVec.GetSize();
   if (pointFunctionManager->HasBoundaryFunctions())
   {
      Rvector bound     = pointFunctionManager->ComputeBoundNLPFunctions();
      #ifdef DEBUG_REPORT_DATA
         ReportBoundData(bound);
      #endif
      Integer boundSize = bound.GetSize();
      allFunctions.SetSize(1 + conVecSize + boundSize);
      
      allFunctions[0]   = costFun;
      for (Integer idx = 0; idx < conVecSize; idx++)
         allFunctions[idx+1] = conVec[idx];
      Integer newIdx = 1 + conVecSize;
      for (Integer ii = 0; ii < boundSize; ii++)
         allFunctions[newIdx++] = bound[ii];
   }
   else
   {
      allFunctions.SetSize(conVecSize + 1);
      allFunctions[0] = costFun;
      for (Integer idx = 0; idx < conVecSize; idx++)
         allFunctions[idx+1] = conVec[idx];
   }
   return allFunctions;
}


//------------------------------------------------------------------------------
// Rvector GetScaledConstraintFunctions(BooleanArray &ifEquality)
//------------------------------------------------------------------------------
/**
 * Returns the scaled constraint functions
 *
 * @param <ifEquality>  output - array of booleans indicating if, for each 
 *                      constraint function, lower bound = upper bound
 *
 * @return   the scaled constraint functions
 *
 */
//------------------------------------------------------------------------------
Rvector Trajectory::GetScaledConstraintFunctions(BooleanArray &ifEquality)
{
   Rvector allFunctions = GetCostConstraintFunctions();
   Integer allSize      = allFunctions.GetSize();
	Rvector constraintFunctions(allSize-1);
   // Initialize the boolean array
   ifEquality.clear();
   for (Integer ii = 0; ii < allSize-1; ii++)
      ifEquality.push_back(false);
   
   for (Integer ii = 0; ii < allSize -1; ii++)
   {
      constraintFunctions(ii) = allFunctions(ii+1) - allConLowerBound.at(ii);
      if (allConLowerBound.at(ii) == allConUpperBound.at(ii))
      {
         ifEquality.at(ii) = true;
      }
      else
      {
         constraintFunctions(ii) /=
                          (allConUpperBound.at(ii)-allConLowerBound.at(ii));
      }
   }
   return constraintFunctions;
}

//------------------------------------------------------------------------------
// StringArray GetConstraintVectorNames()
//------------------------------------------------------------------------------
/**
 * Returns the constraint vector names
 *
 * @return   the array of constraint vector name
 *
 */
//------------------------------------------------------------------------------
StringArray Trajectory::GetConstraintVectorNames()
{
   // Loop over the phases and concatenate the constraint vectors
   StringArray constraintVecNames;
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      StringArray newNames = phaseList.at(phaseIdx)->GetConstraintVectorNames();
      for (UnsignedInt ii = 0; ii < newNames.size(); ii++)
         constraintVecNames.push_back(newNames.at(ii));
   }
   Integer numConstraints = 0;
   if (pointFunctionManager->HasBoundaryFunctions())
   {
      StringArray boundNames = pointFunctionManager->GetFunctionNames();
      if (boundNames.empty())
      {
         Rvector bound  = pointFunctionManager->ComputeBoundNLPFunctions();
         numConstraints = bound.GetSize();

         for (Integer ii = 0;  ii < numConstraints; ii++)
         {
            std::stringstream ss("");
            ss << "User Point Function Constraint " << ii+1;  // or just ii?
            constraintVecNames.push_back(ss.str());
         }
      }
      else
      {
         for (UnsignedInt ii = 0; ii < boundNames.size(); ii++)
            constraintVecNames.push_back(boundNames.at(ii));
      }
   }
   return constraintVecNames;
}



//------------------------------------------------------------------------------
// Real GetCostLowerBound()
//------------------------------------------------------------------------------
/**
 * Returns the cost lower bound
 *
 * @return   the cost lower bound
 *
 */
//------------------------------------------------------------------------------
Real Trajectory::GetCostLowerBound()
{
   return costLowerBound;
}

//------------------------------------------------------------------------------
// Real GetCostUpperBound()
//------------------------------------------------------------------------------
/**
 * Returns the cost upper bound
 *
 * @return   the cost upper bound
 *
 */
//------------------------------------------------------------------------------
Real Trajectory::GetCostUpperBound()
{
   return costUpperBound;
}

//------------------------------------------------------------------------------
// RealArray GetAllConLowerBound()
//------------------------------------------------------------------------------
/**
 * Returns the lower bounds for all constraints
 *
 * @return   the lower bounds for all constraints
 *
 */
//------------------------------------------------------------------------------
RealArray Trajectory::GetAllConLowerBound()
{
   return allConLowerBound;
}
   
//------------------------------------------------------------------------------
// RealArray GetAllConUpperBound()
//------------------------------------------------------------------------------
/**
 * Returns the upper bounds for all constraints
 *
 * @return   the upper bounds for all constraints
 *
 */
//------------------------------------------------------------------------------
RealArray Trajectory::GetAllConUpperBound()
{
   return allConUpperBound;
}
   

//------------------------------------------------------------------------------
// UserPathFunction* GetUserPathFunction()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the user path function
 *
 * @return   the user path functions
 *
 */
//------------------------------------------------------------------------------
UserPathFunction* Trajectory::GetUserPathFunction()
{
   return pathFunction;
}

//------------------------------------------------------------------------------
// UserPointFunction* GetUserPointFunction()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the user point function
 *
 * @return   the user point functions
 *
 */
//------------------------------------------------------------------------------
UserPointFunction* Trajectory::GetUserPointFunction()
{
   return pointFunction;
}

//------------------------------------------------------------------------------
// std::string GetGuessFunctionName()
//------------------------------------------------------------------------------
/**
 * Returns the guess function name
 *
 * @return   the guess function name
 *
 */
//------------------------------------------------------------------------------
std::string Trajectory::GetGuessFunctionName()
{
   return guessFunctionName;
}

//------------------------------------------------------------------------------
// std::vector<Phase*> GetPhaseList()
//------------------------------------------------------------------------------
/**
 * Returns the array of Phase pointers
 *
 * @return   the array of Phase pointers
 *
 */
//------------------------------------------------------------------------------
std::vector<Phase*> Trajectory::GetPhaseList()
{
   return phaseList;
}

//------------------------------------------------------------------------------
// Integer GetNumPhases()
//------------------------------------------------------------------------------
/**
 * Returns the number of phases in the trajectory
 *
 * @return   the number of phases
 *
 */
//------------------------------------------------------------------------------
Integer Trajectory::GetNumPhases()
{
   return numPhases;
}

//------------------------------------------------------------------------------
// Integer GetMeshRefinementCount()
//------------------------------------------------------------------------------
/**
 * Returns the mesh refinement iteration the trajectory is currently on
 *
 * @return   the mesh refinement iteration number
 *
 */
//------------------------------------------------------------------------------
Integer Trajectory::GetMeshRefinementCount()
{
   return meshRefinementCount;
}

//------------------------------------------------------------------------------
// Integer GetMaxMeshRefinementCount()
//------------------------------------------------------------------------------
/**
 * Returns the mesh refinement count
 *
 * @return   the mess refinement count
 *
 */
//------------------------------------------------------------------------------
Integer Trajectory::GetMaxMeshRefinementCount()
{
   return maxMeshRefinementCount;
}

//------------------------------------------------------------------------------
// Real GetCostScaling()
//------------------------------------------------------------------------------
/**
 * Returns the cost scaling value
 *
 * @return   the cost scaling value
 *
 */
//------------------------------------------------------------------------------
Real Trajectory::GetCostScaling()
{
   return costScaling;
}

//------------------------------------------------------------------------------
// RSMatrix GetSparsityPattern()
//------------------------------------------------------------------------------
/**
 * Returns the sparsity pattern
 *
 * @return   the sparsity pattern
 *
 */
//------------------------------------------------------------------------------
RSMatrix Trajectory::GetSparsityPattern()
{
   return sparsityPattern;
}

//------------------------------------------------------------------------------
// RSMatrix GetCostSparsityPattern()
//------------------------------------------------------------------------------
/**
* Returns the Cost sparsity pattern
*
* @return   the Cost sparsity pattern
*
*/
//------------------------------------------------------------------------------
RSMatrix Trajectory::GetCostSparsityPattern()
{
   return sparsityCost;
}


//------------------------------------------------------------------------------
// RSMatrix GetConstraintSparsityPattern()
//------------------------------------------------------------------------------
/**
* Returns the Constraint sparsity pattern
*
* @return   the Constraint sparsity pattern
*
*/
//------------------------------------------------------------------------------
RSMatrix Trajectory::GetConstraintSparsityPattern()
{
   return sparsityConstraints;
}


//------------------------------------------------------------------------------
// RSMatrix GetHessianSparsityPattern()
//------------------------------------------------------------------------------
/**
* return the hessian sparsity pattern.
* YK mod IPOPT branch
* @return hessian pattern
*/
//------------------------------------------------------------------------------
RSMatrix Trajectory::GetHessianSparsityPattern()
{
   return hessianPattern;
}
//------------------------------------------------------------------------------
// RSMatrix ComputeHessianSparsityPattern()
//------------------------------------------------------------------------------
/**
* Compute the hessian sparsity pattern obtained by summing up hessian sparisty 
* tensor into single matrix (or, tensor contraction).
* The hessian sparsity pattern is a lower triangular matrix. 
* YK mod IPOPT branch
*
*/
//------------------------------------------------------------------------------
void Trajectory::ComputeHessianSparsityPattern()
{
   // temporary measure for dense hessian
   SparseMatrixUtil::SetSize(hessianPattern,
      totalNumDecisionParams, totalNumDecisionParams);
   for (Integer idx = 0; idx < totalNumDecisionParams; idx++)
   {
      for (Integer idx2 = 0; idx2 < idx + 1; idx2++)
         SparseMatrixUtil::SetElement(hessianPattern, idx, idx2, 1.0);
   }
   return;
}

//------------------------------------------------------------------------------
// RSMatrix    ComputeHessianContraction(Rvector lambdaVec);
//------------------------------------------------------------------------------
/**
* Returns the decision vector
*
* @return   the decision vector
*
*/
//------------------------------------------------------------------------------
RSMatrix    Trajectory::ComputeHessianContraction(Rvector decVecNow, 
                                                  Rvector lambdaVec)
{
   RSMatrix hessMat = SparseMatrixUtil::GetSparsityPattern(&hessianPattern, true);

   // do we need this to confirm decVec is up-to-date?
   SetDecisionVector(decVecNow);
   Rvector normFunc = GetCostConstraintFunctions();
   // the number of variables and functions in this problem
   Integer numFuncs = normFunc.GetSize();
   Integer numVars = decVecNow.GetSize();

   // hessian pattern is provided
   hessMat = SparseMatrixUtil::GetSparsityPattern(&hessianPattern, true);

   // Define the perturbation for the forward diff.
   // warning: changing perturbation size can cause a serious problem.
   // now hessian pert. size is tuned to match jacobian pert. size 1e-7
   Real pert = 7.0E-5;
   
   // the tensor containing perturbed jacobian results
   std::vector<Rvector> deltaMatrix;
   std::vector<std::vector<Rvector>> deltaTensor;
   
   for (Integer varIdx = 0; varIdx < numVars; varIdx++)
   {
      Rvector decVec(numVars);
      for (Integer idx = 0; idx < numVars; idx++)
      {
         if (varIdx != idx)
            decVec(idx) = decVecNow(idx);
         else
         {
            decVec(idx) = decVecNow(idx) + pert;
         }
      }

      SetDecisionVector(decVec);
      Rvector currFunc = GetCostConstraintFunctions();
      deltaMatrix.push_back(currFunc);

      std::vector<Rvector> accumulator;
      for (Integer varIdx2 = 0; varIdx2 < varIdx + 1; varIdx2++)
      {
         // Perturb the state and recompute user functions
         // the second moment of decVec.  
         Rvector decVec2(numVars);
         for (Integer idx = 0; idx < numVars; idx++)
         {
            if (varIdx2 != idx)
               decVec2(idx) = decVec(idx);
            else
            {               
               decVec2(idx) = decVec(idx) + pert;
            }
         }

         SetDecisionVector(decVec2);
         Rvector currFunc2 = GetCostConstraintFunctions();
         accumulator.push_back(currFunc2);
      }
      deltaTensor.push_back(accumulator);
   }
   
   // sparsity pattern initialization and keeping is a problem. 
   for (Integer idx = 0; idx < lambdaVec.GetSize(); idx++)
   {
      if (lambdaVec(idx) == 0.0)
         continue;
      else
      {
         // compute hessian
         for (Integer rowIdx = 0; rowIdx < numVars; rowIdx++)
         {
            for (Integer colIdx = 0; colIdx < rowIdx + 1; colIdx++)
            { 
               Real value = lambdaVec(idx) / (pert*pert)
                    * (deltaTensor[rowIdx][colIdx](idx)
                  - deltaMatrix[rowIdx](idx) - deltaMatrix[colIdx](idx) + normFunc(idx));
               if (value != 0.0)
                  SparseMatrixUtil::SetElement(hessMat, rowIdx, colIdx,
                                                hessMat(rowIdx, colIdx) + value);
            }
         }
      }
   }
#ifdef DEBUG_HESSIAN
   MessageInterface::ShowMessage("The current hessian contraction matrix is \n");
   SparseMatrixUtil::PrintNonZeroElements(&hessMat, true);
#endif
   SetDecisionVector(decVecNow);
   return hessMat;
}

//------------------------------------------------------------------------------
// void ComputeMaxConstraintViolation(Real &maxConViol, Real &costValue)
//------------------------------------------------------------------------------
/**
* Computes the max constraint violation from the current set of constraints
* along with the current cost function value
*
* @param <maxConViol> The max constraint violation
* @param <costValue> The current cost function value
*
*/
//------------------------------------------------------------------------------
void Trajectory::ComputeMaxConstraintViolation(Real &maxConViol,
                                               Real &costValue)
{
   costValue = GetCostFunction();
   Rvector conVec = GetCostConstraintFunctions();
   RealArray testConVec = conVec.GetRealArray();

   maxConViol = 0.0;
   for (Integer i = 0; i < allConLowerBound.size(); ++i)
   {
      Real conViol = 0.0;
      if (conVec(i + 1) < allConLowerBound.at(i))
         conViol = allConLowerBound.at(i) - conVec(i + 1);
      else if (conVec(i + 1) > allConUpperBound.at(i))
         conViol = conVec(i + 1) - allConUpperBound.at(i);

      if (conViol > maxConViol)
         maxConViol = conViol;
   }
}

//------------------------------------------------------------------------------
// void CheckSolutionFeasibility()
//------------------------------------------------------------------------------
/**
* Check the feasiblity of the current decision vector.  For feasible solutions
* that are found, the one with the best cost value is saved.  This will be used
* as the final solution if the optimality tolerance is never met (i.e. solution
* did not converge)
*
* @param feas The current feasiblity of the decision vector
* @param opt The current optimality of the decision vector
*/
//------------------------------------------------------------------------------
void Trajectory::CheckSolutionFeasibility(Real feas, Real opt)
{
   Rvector emptyVec(0);
   if (bestFeasibleDecVec.size() != numPhases)
   {
      throw LowThrustException("ERROR - Trajectory: The vector to store the "
         "most feasible solution found has not been initialized.\n");
   }

   // Get the cost value
   Real maxConViol, costVal;
   ComputeMaxConstraintViolation(maxConViol, costVal);

   // If we are maximizing, change the sign on cost
   Integer numOptModes = optimizationMode.size();
   std::string currOptMode;
   if (meshRefinementCount < numOptModes)
      currOptMode = optimizationMode.at(meshRefinementCount);
   else
      currOptMode = optimizationMode.at(numOptModes - 1);
   if (currOptMode == "Maximize")
      costVal = -costVal;

   // Get the current feasibility tolerance
   Integer numFeasTols = feasibilityTolerances.GetSize();
   Real currFeasTol;
   if (meshRefinementCount < numFeasTols)
      currFeasTol = feasibilityTolerances(meshRefinementCount);
   else
      currFeasTol = feasibilityTolerances(numFeasTols - 1);

   // Check for feasibility or improved cost a feasible solution is already 
   // stored
   if ((wasFeasiblityAchieved && feas < currFeasTol &&
      costVal < bestCostCurrMesh) ||
      (!wasFeasiblityAchieved && feas < bestFeasCurrMesh))
   {
      for (Integer ii = 0; ii < numPhases; ++ii)
      {
         Rvector phaseDecVec =
            phaseList.at(ii)->GetDecisionVector()->GetDecisionVector();
         if (bestFeasibleDecVec.at(ii).GetSize() != phaseDecVec.GetSize())
            bestFeasibleDecVec.at(ii).SetSize(phaseDecVec.GetSize());
         bestFeasibleDecVec.at(ii) = phaseDecVec;
      }
      bestFeasCurrMesh = feas;
      bestOptCurrMesh = opt;
      bestCostCurrMesh = costVal;
   }

   if (!wasFeasiblityAchieved && feas < currFeasTol)
      wasFeasiblityAchieved = true;
}

//------------------------------------------------------------------------------
// Integer InterpretSNOPTExitFlag(Integer snoptExitFlag,
//                                Integer &simplifiedExitFlag,
//                                std::string &exitMessage)
//------------------------------------------------------------------------------
/**
* Interpret the SNOPT exit code to a simplified version of whether the 
* optimizer converged, did not converge, optimization was stopped by the user,
* or an unrecoverable failure occurred.
*
* @param snoptExitFlag The exit flag directly from SNOPT
* 
* @return The simplified exit code
*         1 = optimizer converged
*         0 = numerical difficulties encountered
*        -1 = unrecoverable failure
*        -2 = optimization stopped by user
*/
//------------------------------------------------------------------------------
Integer Trajectory::GetSimplifiedSNOPTExitFlag(Integer snoptExitFlag)
{
   // See SNOPT User guide V7 pgs 93-99 for detailed explanation of exit codes.

   // Optimizer finished successfully
   Integer simplifiedExitFlag;
   if (snoptExitFlag >= 1 && snoptExitFlag <= 9)
   {
      simplifiedExitFlag = 1;
   }
   // Optimizer did not converge, but mesh refinement may resolve the issue
   else if ((snoptExitFlag >= 31 && snoptExitFlag <= 39) ||  // Resource limit error
      (snoptExitFlag == 41 && snoptExitFlag <= 49))  // Terminated after numerical difficulties
   {
      simplifiedExitFlag = 0;
   }
   // Optimizer was stopped by the user
   else if (snoptExitFlag == 71 || snoptExitFlag == 74)
   {
      simplifiedExitFlag = -2;
   }
   // Unrecoverable failure
   else
   {
      simplifiedExitFlag = -1;
   }

   return simplifiedExitFlag;
}

//------------------------------------------------------------------------------
// std::string InterpretSNOPTExitFlag(Integer snoptExitFlag)
//------------------------------------------------------------------------------
/**
* Interpret the SNOPT exit code to tell whether the optimizer converged, did
* not converge, optimization was stopped by the user, or an unrecoverable
* failure occurred.
*
* @param snoptExitFlag The exit flag directly from SNOPT
*
* @return The exit message
*/
//------------------------------------------------------------------------------
std::string Trajectory::GetSNOPTExitFlagMessage(Integer snoptExitFlag)
{
   // See SNOPT User guide V7 pgs 93-99 for detailed explanation of exit codes.

   // Optimizer finished successfully
   std::string exitMessage;

   // First check if it is one of the special cases that doesn't have defined
   // messages for the
   if (snoptExitFlag >= 100 && snoptExitFlag <= 109)
      snoptExitFlag = 100;
   else if (snoptExitFlag >= 110 && snoptExitFlag <= 119)
      snoptExitFlag = 110;
   else if (snoptExitFlag >= 120 && snoptExitFlag <= 129)
      snoptExitFlag = 120;
   else if (snoptExitFlag >= 130 && snoptExitFlag <= 139)
      snoptExitFlag = 130;

   // Set the exit message
   switch (snoptExitFlag)
   {
   case 1:
      exitMessage += "optimizer converged";
      break;
   case 2:
      exitMessage = "feasible point found";
      break;
   case 3:
      exitMessage = "requested accuracy could not be achieved";
      break;
   case 11:
      exitMessage = "infeasible linear constraints";
      break;
   case 12:
      exitMessage = "infeasible linear equalities";
      break;
   case 13:
      exitMessage = "nonlinear infeasibilities minimized";
      break;
   case 14:
      exitMessage = "infeasibilities minimized";
      break;
   case 21:
      exitMessage = "the objective appears to be unnbounded";
      break;
   case 22:
      exitMessage = "the problem appears to be undbounded, constraint "
         "violation limit reached";
      break;
   case 31:
      exitMessage = "minor iteration limit reached";
      break;
   case 32:
      exitMessage = "major iteration limit reached";
      break;
   case 33:
      exitMessage = "the superbasics limit is too small";
      break;
   case 41:
      exitMessage = "current point cannot be improved";
      break;
   case 42:
      exitMessage = "numerical difficulties due to singular basis";
      break;
   case 43:
      exitMessage = "cannot satisfy the general constraints";
      break;
   case 44:
      exitMessage = "numerical difficulties due to ill-conditioned null space";
      break;
   case 51:
      exitMessage = "incorrect objective derivatives";
      break;
   case 52:
      exitMessage = "incorrect constraint derivatives";
      break;
   case 61:
      exitMessage = "undefined user-supplied function at the first feasible "
         "point";
      break;
   case 62:
      exitMessage = "undefined user-supplied function at the initial point";
      break;
   case 63:
      exitMessage = "undefined user-suppliedd functions, unable to proceed "
         "into undefined region";
      break;
   case 71:
      exitMessage = "optimization terminated by user during function "
         "evaluation";
      break;
   case 72:
      exitMessage = "optimization terminated by user during constraint "
         "evaluation";
      break;
   case 73:
      exitMessage = "optimization terminated by user during objective "
         "evaluation";
      break;
   case 74:
      exitMessage = "optimization terminated by user from monitor routine";
      break;
   case 81:
      exitMessage = "insufficient storage allocated for work arrays, must "
         "have at least 500";
      break;
   case 82:
      exitMessage = "insufficient character storage allocated";
      break;
   case 83:
      exitMessage = "insufficient integer storage allocated";
      break;
   case 84:
      exitMessage = "insufficient real storage allocated";
      break;
   case 91:
      exitMessage = "invalid inout argument";
      break;
   case 92:
      exitMessage = "basis file dimensions do not match this problem";
      break;
   case 141:
      exitMessage = "system error, wrong number of basic variables";
      break;
   case 142:
      exitMessage = "system error in basis package";
      break;
   default:
      exitMessage = "unknown failure";
   }

   // Add trailing message on whether a feasible solution was found during
   // optimization of the last mesh iteration for non-converging cases
   if (snoptExitFlag > 2 || snoptExitFlag < 1)
   {
      if (wasFeasiblityAchieved)
      {
         exitMessage += ", optimizer did not converge, feasible solution "
            "found during final mesh";
      }
      else
      {
         exitMessage += ", optimizer did not converge, no feasible solutions "
            "found during final mesh";
      }
   }

   return exitMessage;
}

//------------------------------------------------------------------------------
// Rvector GetDecisionVector()
//------------------------------------------------------------------------------
/**
 * Returns the decision vector
 *
 * @return   the decision vector
 *
 */
//------------------------------------------------------------------------------
Rvector Trajectory::GetDecisionVector()
{
   return decisionVector;
}
   
//------------------------------------------------------------------------------
// Rvector GetDecisionVectorLowerBound()
//------------------------------------------------------------------------------
/**
 * Returns the decision vector lower bound
 *
 * @return   the decision vector lower bound
 *
 */
//------------------------------------------------------------------------------
Rvector Trajectory::GetDecisionVectorLowerBound()
{
   return decisionVecLowerBound;
}

//------------------------------------------------------------------------------
// Rvector GetDecisionVectorUpperBound()
//------------------------------------------------------------------------------
/**
 * Returns the decision vector upper bound
 *
 * @return   the decision vector upper bound
 *
 */
//------------------------------------------------------------------------------
Rvector Trajectory::GetDecisionVectorUpperBound()
{
   return decisionVecUpperBound;
}

//------------------------------------------------------------------------------
// void SetCostLowerBound(Real costLower)
//------------------------------------------------------------------------------
/**
 * Sets the cost lower bound
 *
 * @param <costLower>   the cost lower bound
 *
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetCostLowerBound(Real costLower)
{
   costLowerBound = costLower;
}

//------------------------------------------------------------------------------
// void SetCostUpperBound(Real costUpper)
//------------------------------------------------------------------------------
/**
 * Sets the cost upper bound
 *
 * @param <costLower>   the cost upper bound
 *
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetCostUpperBound(Real costUpper)
{
   costUpperBound = costUpper;
}

//------------------------------------------------------------------------------
// void SetUserPathFunction(UserPathFunction *func)
//------------------------------------------------------------------------------
/**
 * Sets the user path function
 *
 * @param <func>   the user path function
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetUserPathFunction(UserPathFunction *func)
{
   pathFunction = func;
}

//------------------------------------------------------------------------------
// void SetUserPointFunction(UserPointFunction *func)
//------------------------------------------------------------------------------
/**
 * Sets the user point function
 *
 * @param <func>   the user points function
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetUserPointFunction(UserPointFunction *func)
{
   pointFunction = func;
}


//------------------------------------------------------------------------------
// void SetGuessFunctionName(const std::string &toName)
//------------------------------------------------------------------------------
/**
 * Sets the guess function name
 *
 * @param <toName>   the guess function name
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetGuessFunctionName(const std::string &toName)
{
   guessFunctionName = toName;
}

//------------------------------------------------------------------------------
// void SetPhaseList(std::vector<Phase*> pList)
//------------------------------------------------------------------------------
/**
 * Sets the list of Phase pointers
 *
 * @param <pList>   the array of Phase pointers
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetPhaseList(std::vector<Phase*> pList)
{
   phaseList = pList;
   for (UnsignedInt phaseIdx = 0; phaseIdx < phaseList.size(); phaseIdx++)
      phaseList.at(phaseIdx)->SetPhaseNumber(phaseIdx);
}

//------------------------------------------------------------------------------
// void SetMaxMeshRefinementCount(Integer toCount)
//------------------------------------------------------------------------------
/**
 * Sets the maximum mesh refinement count
 *
 * @param <toCount>   the maximum mesh refinement count
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetMaxMeshRefinementCount(Integer toCount)
{
   maxMeshRefinementCount = toCount;
}

//------------------------------------------------------------------------------
// void SetMeshRefinementGuessMode(const std::string &toGuessMode)
//------------------------------------------------------------------------------
/**
* Sets the method to use for selecting an initial guess to a new mesh
* refinement iteration
*
* @param <toGuessMode>   the new guess mode to use
*
*/
//------------------------------------------------------------------------------
void Trajectory::SetMeshRefinementGuessMode(const std::string &toGuessMode)
{
   if (toGuessMode != "LastSolutionMostRecentMesh" &&
      toGuessMode != "BestSolutionMostRecentMesh" &&
      toGuessMode != "BestSolutionAnyMesh")
   {
      throw LowThrustException("ERROR - Trajectory: Invalid mesh refinement "
         "guess mode \"" + toGuessMode + "\" received.  Valid modes are:\n"
         "[LastSolutionMostRecentMesh, BestSolutionMostRecentMesh, "
         "BestSolutionAnyMesh]\n");
   }
   meshGuessMode = toGuessMode;
}

//------------------------------------------------------------------------------
// void SetFailedMeshOptimizationAllowance(bool toAllowance)
//------------------------------------------------------------------------------
/**
* Sets whether to allow mesh refinement to continue even if the previous
* optimization attempt failed
*
* @param <toAllowance>   the new allowance setting
*
*/
//------------------------------------------------------------------------------
void Trajectory::SetFailedMeshOptimizationAllowance(bool toAllowance)
{
   allowFailedMeshOptimizations = toAllowance;
}

//------------------------------------------------------------------------------
// void SetCostScaling(Real toScaling)
//------------------------------------------------------------------------------
/**
 * Sets the cost scaling value
 *
 * @param <toCount>   the cost scaling value
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetCostScaling(Real toScaling)
{
   costScaling = toScaling;
}



//------------------------------------------------------------------------------
// void SetInitialGuess()
//------------------------------------------------------------------------------
/**
 * Computes the initial guess for the complete decision vector
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetInitialGuess()
{
   decisionVector.SetSize(totalNumDecisionParams);
   Integer lowIdx = 0;
   Integer highIdx;
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      highIdx = lowIdx + numPhaseDecisionParams.at(phaseIdx)-1;
      DecisionVector *dv = phaseList.at(phaseIdx)->GetDecisionVector();
      Rvector        dv2 = dv->GetDecisionVector();
      Integer idx = 0;
      for (Integer ii = lowIdx; ii <= highIdx; ii++)
      {
         decisionVector(ii)  = dv2(idx++);
      }
      lowIdx = highIdx + 1;
   }

   meshGuessDecVec.SetSize(totalNumDecisionParams);
   meshGuessDecVec = decisionVector;
}

//------------------------------------------------------------------------------
// bool GetIfScaling()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not it is scaling
 *
 * @return  true if scaling; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool Trajectory::GetIfScaling()
{
	return ifScaling;	
}

//------------------------------------------------------------------------------
// bool GetIfStopping()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not CSALT's optimization is stopping
 *
 * @return  true if stopping; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool Trajectory::GetIfStopping()
{
   if (csaltState == "Stopping")
      return true;
   else
      return false;
}

//------------------------------------------------------------------------------
// void GetFeasibleSolution(std::vector<Rvector> &feasDecVecs,
//                          Real &feasOfSolution,
//                          Real &optOfSolution)
//------------------------------------------------------------------------------
/**
 * Returns the best feasible solution that was found during the current point
 * in optimization.  If there was never a feasible solution, the solution 
 * closest to feasibility was stored.  If there were feasible solutions, the 
 * solution with the lowest cost value was stored.
 *
 * @param feaasDecVecs A vector of Rvectors containing the decision vector for 
 *        each phase for the solution
 * @param feasOfSolution The feasibility of the solution
 * @param optOfSolution The optimality of the solution
 * @param isSolFeasible Is the currently stored solution feasible
 *
 */
 //------------------------------------------------------------------------------
void Trajectory::GetFeasibleSolution(std::vector<Rvector> &feasDecVecs,
                                     Real &feasOfSolution,
                                     Real &optOfSolution,
                                     bool &isSolFeasible)
{
   feasDecVecs.clear();
   for (UnsignedInt ii = 0; ii < numPhases; ++ii)
   {
      feasDecVecs.push_back(bestFeasibleDecVec.at(ii));
      feasOfSolution = bestFeasCurrMesh;
      optOfSolution = bestOptCurrMesh;
      isSolFeasible = wasFeasiblityAchieved;
   }
}

//------------------------------------------------------------------------------
// void SetFeasibilityTolerances(const Rvector &tol)
//------------------------------------------------------------------------------
/**
 * Sets the array of feasibility tolerance values (iteration-dependent).
 *
 * @param  tol    array of feasibility tolerances
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetFeasibilityTolerances(const Rvector &tol)
{
   if ((!tol.IsSized()) || (tol.GetSize() == 0)) // todo - check > 0.0 here
   {
      std::string errmsg = "Invalid input feasibility tolerance vector\n";
      throw LowThrustException(errmsg);
   }
   Integer sz = tol.GetSize();
   feasibilityTolerances.SetSize(sz);
   feasibilityTolerances = tol;
}

//------------------------------------------------------------------------------
// void SetOptimalityTolerances(const Rvector &tol)
//------------------------------------------------------------------------------
/**
 * Sets the array of optimality tolerance values (iteration-dependent).
 *
 * @param  tol    array of optimality tolerances
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetOptimalityTolerances(const Rvector &tol)
{
   if ((!tol.IsSized()) || (tol.GetSize() == 0)) // todo - check > 0.0 here
   {
      std::string errmsg = "Invalid input optimality tolerance vector\n";
      throw LowThrustException(errmsg);
   }
   Integer sz = tol.GetSize();
   optimalityTolerances.SetSize(sz);
   optimalityTolerances = tol;
}

//------------------------------------------------------------------------------
// void SetMajorIterationsLimit(const IntegerArray &iter)
//------------------------------------------------------------------------------
/**
 * Sets the array of major iteration limit values (iteration-dependent).
 *
 * @param  iter    array of major iteration limits
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetMajorIterationsLimit(const IntegerArray &iter)
{
   if (iter.size() == 0)
   {
      std::string errmsg = "Empty Iterations Limit \n";
      throw LowThrustException(errmsg);
   }
   majorIterationsLimit.clear();
   for (Integer ii = 0; ii < iter.size(); ii++)
      majorIterationsLimit.push_back(iter.at(ii));
}

//------------------------------------------------------------------------------
// void SetTotalIterationsLimit(const IntegerArray &iter)
//------------------------------------------------------------------------------
/**
 * Sets the array of total iteration limit values (iteration-dependent).
 *
 * @param  iter    array of total iteration limits
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetTotalIterationsLimit(const IntegerArray &iter)
{
   if (iter.size() == 0)
   {
      std::string errmsg = "Empty Iterations Limit \n";
      throw LowThrustException(errmsg);
   }
   totalIterationsLimit.clear();
   for (Integer ii = 0; ii < iter.size(); ii++)
      totalIterationsLimit.push_back(iter.at(ii));
}

//------------------------------------------------------------------------------
// void SetOptimizationMode(const StringArray &optMode)
//------------------------------------------------------------------------------
/**
 * Sets the array of optimization mode values (iteration-dependent).
 *
 * @param  optMode    array of optimization modes
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetOptimizationMode(const StringArray &optMode)
{
   if (optMode.size() == 0)
   {
      std::string errmsg = "Empty Optimization Mode array \n";
      throw LowThrustException(errmsg);
   }
   optimizationMode.clear();
   for (Integer ii = 0; ii < optMode.size(); ii++)
   {
      if ((optMode.at(ii) != "Minimize") &&
          (optMode.at(ii) != "Feasible point") &&
          (optMode.at(ii) != "Maximize"))
      {
         std::string errmsg = "Invalid Optimization Mode \"";
         errmsg += optMode.at(ii) + "\".  Valid modes are:\n";
         errmsg += "[\"Minimize\" \"Feasible point\" \"Maximize\"]\n";
         throw LowThrustException(errmsg);
      }
      optimizationMode.push_back(optMode.at(ii));
   }
}

//------------------------------------------------------------------------------
// void SetApplicationType(const std::string &appType)
//------------------------------------------------------------------------------
/**
 * Set the application type that is being used
 *
 * @param appType the application type being used
 */
 //-----------------------------------------------------------------------------
void Trajectory::SetApplicationType(const std::string &appType)
{
   applicationType = appType;
}

//------------------------------------------------------------------------------
// void SetSnoptConsoleOutputLevel(const Integer level)
//------------------------------------------------------------------------------
/**
 * Set how much information SNOPT should print to screen
 *
 * @param level the level of output to print to screen
 * Current valid options for level:
 * 0: no output (major print level = 0, minor print level = 0, print no = 0, 
 * summary file = 0, suppress parameters)
 * 1: default output (default choice)
 * Any value other than 0 is interpreted as 1.
 */
 //-----------------------------------------------------------------------------
void Trajectory::SetSnoptConsoleOutputLevel(const Integer level)
{
	switch (level)
	{
	case 0:
		this->snoptConsoleOutputLevel = level;
		break;
	default:
		this->snoptConsoleOutputLevel = 1;
	}
}

//------------------------------------------------------------------------------
// Integer GetSnoptConsoleOutputLevel()
//------------------------------------------------------------------------------
/**
 * Get how much information SNOPT should print to screen
 *
 * @return level the level of output to print to screen
 *
 */
 //-----------------------------------------------------------------------------
Integer Trajectory::GetSnoptConsoleOutputLevel()
{
	return this->snoptConsoleOutputLevel;
}

//------------------------------------------------------------------------------
// std::string GetApplicationType()
//------------------------------------------------------------------------------
/**
 * Get the application type that is being used
 */
 //-----------------------------------------------------------------------------
std::string Trajectory::GetApplicationType()
{
   return applicationType;
}

//------------------------------------------------------------------------------
// ScaleUtility* GetScaleHelper()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the scale helper class
 *
 * @return  the scale helper
 *
 */
//------------------------------------------------------------------------------
ScaleUtility* Trajectory::GetScaleHelper()
{
	return scaleHelper;
}

//------------------------------------------------------------------------------
// void SetExecutionInterface(ExecutionInterface *execInt)
//------------------------------------------------------------------------------
/**
* Sets the ExecutionInterface pointer to the desired ExecutionInterface
*/
//------------------------------------------------------------------------------
void Trajectory::SetExecutionInterface(ExecutionInterface *execInt)
{
   csaltExecInterface = execInt;
}

//------------------------------------------------------------------------------
// void WriteToFile(std::string fileName)
//------------------------------------------------------------------------------
/**
 * Writes the trajectory states and controls to an OCH file
 *
 * @param<fileName> The name of the output file
 *
 */
//------------------------------------------------------------------------------
void Trajectory::WriteToFile(std::string fileName)
{
   #ifdef DEBUG_WRITE_DATA
      MessageInterface::ShowMessage("Entering Trajectory::WriteToFile\n");
   #endif
    // Create an OCH Trajectory data object
    OCHTrajectoryData* ochData = new OCHTrajectoryData();
   #ifdef DEBUG_WRITE_DATA
   MessageInterface::ShowMessage("--- OCHTrajectoryData created <%p>...\n",
                                 ochData);
   #endif

    // Create needed local variables
    TrajectoryDataStructure dataPoint;
    Integer numStateTimes,numStateParams,numControlTimes,numControlParams;

    // TODO: Allow integral data to be written
    dataPoint.integrals.SetSize(0);

    // Set the number of segments (phases)
    ochData->SetNumSegments(phaseList.size());

    // Set exit condition data
    std::string exitMsg = GetSNOPTExitFlagMessage(optExitFlag);
    Integer simpleFlag = GetSimplifiedSNOPTExitFlag(optExitFlag);

    ochData->SetFeasibility(bestFeasCurrMesh);
    ochData->SetOptimality(bestOptCurrMesh);
    ochData->SetCostValue(costFunction);
    ochData->SetMaxConstraintViolation(maxConViolation);
    ochData->SetOptimizerExitStatus(exitMsg, optExitFlag);

    std::string solType;
    if (simpleFlag == 1)
       solType = "Converged solution";
    else
    {
       if (wasFeasiblityAchieved)
          solType = "Feasible solution";
       else
          solType = "Solution closest to feasibility";
    }
    ochData->SetSolutionType(solType);

    // Get the interval with the max mesh relative error from each phase
    Real maxMeshRelError = 0.0;
    for (UnsignedInt p = 0; p < phaseList.size(); ++p)
    {
       Rvector phaseMeshErrors =
          phaseList.at(p)->ComputeMaxMeshError();
       for (UnsignedInt ii = 0; ii < phaseMeshErrors.GetSize(); ++ii)
          if (phaseMeshErrors(ii) > maxMeshRelError)
             maxMeshRelError = phaseMeshErrors(ii);
    }

    ochData->SetMaxMeshRelativeError(maxMeshRelError);

    // Loop through each phase
    for (UnsignedInt p = 0; p < phaseList.size(); p++)
    {
      #ifdef DEBUG_WRITE_DATA
      MessageInterface::ShowMessage("--- Processing phase %d\n", (Integer) p);
      #endif
        // Get the time, state and control data for this phase
        Rvector timeVector = phaseList[p]->GetTimeVector();
        DecisionVector *dv = phaseList[p]->GetDecisionVector();
        Rmatrix stateSol   = dv->GetStateArray();
        Rmatrix controlSol = dv->GetControlArray();
        
        // Get the number of control parameters and size the 
        // och data object and the local structure
        controlSol.GetSize(numControlTimes,numControlParams);
        ochData->SetNumControlParams(p,numControlParams);

        // Get the number of state parameters and size the 
        // och data object and the local structure
        stateSol.GetSize(numStateTimes,numStateParams);
        ochData->SetNumStateParams(p,numStateParams);

        // Loop through each time value
        for (Integer idx = 0; idx < timeVector.GetSize(); idx++)
        {
            // Set the time value
            dataPoint.time = timeVector(idx);

            // Make sure that we have a control value at this time
            if (idx < numControlTimes)
            {
                dataPoint.controls.SetSize(numControlParams);
                // Set all of the control values
                for (Integer jdx = 0; jdx < numControlParams; jdx++)
                    dataPoint.controls(jdx) = controlSol(idx,jdx);
            }
            else
                // There are no control values here, so set them to be zero
                dataPoint.controls.SetSize(0);

            // Make sure that we have a state value at this time
            if (idx < numStateTimes)
            {
                dataPoint.states.SetSize(numStateParams);
                // Set all of the state values
                for (Integer jdx = 0; jdx < numStateParams; jdx++)
                    dataPoint.states(jdx) = stateSol(idx,jdx);
            }
            else
                // There are no state values here, so set them to be zero
                dataPoint.states.SetSize(0);
            
            // Add this data point to the OCH Data object
           try
           {
              ochData->AddDataPoint(p,dataPoint);
           }
           catch (LowThrustException &lt)
           {
              // Do nothing, in writing data, time not in the correct
              // temporal order is allowed
           }
        }
    }
   #ifdef DEBUG_WRITE_DATA
   MessageInterface::ShowMessage("--- About to call ochData to write file %s...\n",
                                 fileName.c_str());
   #endif
    // Write the och data to file
    ochData->WriteToFile(fileName);

    // Delete the och object
    delete ochData;
   #ifdef DEBUG_WRITE_DATA
   MessageInterface::ShowMessage("EXITing Trajectory::WriteToFile\n");
   #endif
}

//------------------------------------------------------------------------------
// void ReportBoundData(Rvector boundData)
//------------------------------------------------------------------------------
/**
 * Reports Trajectory Bounds data to the log.
 *
 */
//------------------------------------------------------------------------------
void Trajectory::ReportBoundData(Rvector boundData)
{
   Integer perLine = 5;
   
   Integer sz      = boundData.GetSize();
   
   std::stringstream boundsMsg;
   
   boundsMsg.width(12);
   boundsMsg.fill(' ');
   boundsMsg.setf(std::ios::left);
   boundsMsg.precision(7);
   boundsMsg.setf(std::ios::fixed,std::ios::floatfield);
   
   // Write the section header and divider
   boundsMsg << "  ====================================== Trajectory Bounds Data " <<
   "==========================================" << std::endl;
   boundsMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   
   
   // Write the state and control data here
   Integer count = 0;
   boundsMsg << "    bound         ";
   for (Integer ii = 0; ii < sz; ii++)
   {
      boundsMsg << GmatStringUtil::BuildNumber(boundData(ii), true, 12) << "   ";
      count++;
      if (count >= perLine && ii < (sz-1))
      {
         boundsMsg << std::endl;
         boundsMsg << "                  ";
         count = 0;
      }
   }
   boundsMsg << std::endl;
   boundsMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   
   
   MessageInterface::ShowMessage("%s\n", boundsMsg.str().c_str());
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void CopyArrays(const Trajectory &copy)
//------------------------------------------------------------------------------
/**
 * Copies the data from the input Trajectory to this one
 *
 * @param <toCount>   the Trajectory obejct to copy
 *
 */
//------------------------------------------------------------------------------
void Trajectory::CopyArrays(const Trajectory &copy)
{
   
   
   //   MessageInterface::ShowMessage(
   //                  "WARNING - calling Trajectory::CopyArrays!!!!!!\n"); // ******
   phaseList.clear();    // delete Phase objects first? probably not
   for (UnsignedInt ii = 0; ii < copy.phaseList.size(); ii++)
      phaseList.push_back(copy.phaseList.at(ii));
   Integer sz = 0;
   if (decisionVector.IsSized())
   {
      sz = copy.decisionVector.GetSize();
      decisionVector.SetSize(sz);
      decisionVector = copy.decisionVector;
   }
   if (decisionVecLowerBound.IsSized())
   {
      sz = copy.decisionVecLowerBound.GetSize();
      decisionVecLowerBound.SetSize(sz);
      decisionVecLowerBound = copy.decisionVecLowerBound;
   }
   if (decisionVecUpperBound.IsSized())
   {
      sz = copy.decisionVecUpperBound.GetSize();
      decisionVecUpperBound.SetSize(sz);
      decisionVecUpperBound = copy.decisionVecUpperBound;
   }
   if (meshGuessDecVec.IsSized())
   {
      sz = copy.meshGuessDecVec.GetSize();
      meshGuessDecVec.SetSize(sz);
      meshGuessDecVec = copy.meshGuessDecVec;
   }
   
   decVecStartIdx.clear();
   for (UnsignedInt ii = 0; ii < copy.decVecStartIdx.size(); ii++)
      decVecStartIdx.push_back(copy.decVecStartIdx.at(ii));
   decVecEndIdx.clear();
   for (UnsignedInt ii = 0; ii < copy.decVecEndIdx.size(); ii++)
      decVecEndIdx.push_back(copy.decVecEndIdx.at(ii));
   
   numPhaseDecisionParams.clear();
   for (UnsignedInt ii = 0; ii < copy.numPhaseDecisionParams.size(); ii++)
      numPhaseDecisionParams.push_back(copy.numPhaseDecisionParams.at(ii));
   numPhaseConstraints.clear();
   for (UnsignedInt ii = 0; ii < copy.numPhaseConstraints.size(); ii++)
      numPhaseConstraints.push_back(copy.numPhaseConstraints.at(ii));
   
   allConLowerBound.clear();
   for (UnsignedInt ii = 0; ii < copy.allConLowerBound.size(); ii++)
      allConLowerBound.push_back(copy.allConLowerBound.at(ii));
   allConUpperBound.clear();
   for (UnsignedInt ii = 0; ii < copy.allConUpperBound.size(); ii++)
      allConUpperBound.push_back(copy.allConUpperBound.at(ii));
   
   conPhaseStartIdx.clear();
   for (UnsignedInt ii = 0; ii < copy.conPhaseStartIdx.size(); ii++)
      conPhaseStartIdx.push_back(copy.conPhaseStartIdx.at(ii));
   
   if (feasibilityTolerances.IsSized())
   {
      if (copy.feasibilityTolerances.IsSized())
      {
         sz = copy.feasibilityTolerances.GetSize();
         feasibilityTolerances.SetSize(sz);
         feasibilityTolerances = copy.feasibilityTolerances;
      }
   }
   
   if (optimalityTolerances.IsSized())
   {
      if (copy.optimalityTolerances.IsSized())
      {
         sz = copy.optimalityTolerances.GetSize();
         optimalityTolerances.SetSize(sz);
         optimalityTolerances = copy.optimalityTolerances;
      }
   }
   
   majorIterationsLimit.clear();
   for (UnsignedInt ii = 0; ii < copy.majorIterationsLimit.size(); ii++)
      majorIterationsLimit.push_back(copy.majorIterationsLimit.at(ii));
   
   totalIterationsLimit.clear();
   for (UnsignedInt ii = 0; ii < copy.totalIterationsLimit.size(); ii++)
      totalIterationsLimit.push_back(copy.totalIterationsLimit.at(ii));
   
   optimizationMode.clear();
   for (UnsignedInt ii = 0; ii < copy.optimizationMode.size(); ii++)
      optimizationMode.push_back(copy.optimizationMode.at(ii));
   
   if (constraintVector.IsSized())
   {
      sz = copy.constraintVector.GetSize();
      constraintVector.SetSize(sz);
      constraintVector = copy.constraintVector;
   }
   
   // Done by YK 2017.09.28
   sparsityCost = SparseMatrixUtil::CopySparseMatrix(&copy.sparsityCost);
   sparsityConstraints = SparseMatrixUtil::CopySparseMatrix(&copy.sparsityConstraints);
   sparsityPattern = SparseMatrixUtil::CopySparseMatrix(&copy.sparsityPattern);
   
   whichStateVar.clear();
   for (UnsignedInt ii = 0; ii < copy.whichStateVar.size(); ii++)
      whichStateVar.push_back(copy.whichStateVar.at(ii));
   
   ifDefect.clear();
   for (UnsignedInt ii = 0; ii < copy.ifDefect.size(); ii++)
      ifDefect.push_back(copy.ifDefect.at(ii));

   bestFeasibleDecVec.clear();
   for (UnsignedInt ii = 0; ii < copy.bestFeasibleDecVec.size(); ++ii)
      bestFeasibleDecVec.push_back(copy.bestFeasibleDecVec.at(ii));
}

//------------------------------------------------------------------------------
// void InitializePhases()
//------------------------------------------------------------------------------
/**
 * Initializes the phases
 *
 *
 */
//------------------------------------------------------------------------------
void    Trajectory::InitializePhases()
{
#ifdef DEBUG_TRAJECTORY_INIT
   MessageInterface::ShowMessage("--- initializing phases ...\n");
   std::cout << "--- initializing phases ...\n";
#endif
   // Get properties of each phase for later book-keeping
   numPhases = phaseList.size();
   if (numPhases <= 0)
   {
      std::string errmsg = "Trajectory requires at least one phase.\n";
      throw LowThrustException(errmsg);
   }
   
   // Initialize phases and a few trajectory data members
   for (Integer ii = 0; ii < numPhases; ii++)
   {
      numPhaseConstraints.push_back(1);
      numPhaseDecisionParams.push_back(0);
      decVecStartIdx.push_back(-1);
      decVecEndIdx.push_back(-1);
      conPhaseStartIdx.push_back(-1);
   }
   totalNumDecisionParams = 0;
   totalNumConstraints    = 0;
			
   // List of scaling utilities created by the phases to be send to the user
   // functions
   std::vector<ScalingUtility*> phaseScaleUtils;
   for (Integer ii = 0; ii < numPhases; ++ii)
      phaseScaleUtils.push_back(phaseList.at(ii)->GetScaleUtility());
   pathFunction->SetPhaseScaleUtilList(phaseScaleUtils);
   pointFunction->SetPhaseScaleUtilList(phaseScaleUtils);

   // Define an index to track location in the constraint vector
   Integer constraintStartIdx = 0;
   if (ifScaling)                  // ??
      constraintStartIdx = 0;

   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
#ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                                    "--- now attempting to initialize phase %d out of %d\n",
                                    phaseIdx, numPhases);
      std::cout << "--- now attempting to initialize phase %d out of %d\n";
#endif
      Phase *currentPhase = phaseList.at(phaseIdx);
      //  Configure user functions
      /*
       currentPhase->SetGuessFunctionName(guessFunctionName);
       currentPhase->SetPathFunction(pathFunction);
       Integer defectConStartIdx = currentPhase->GetDefectConStartIdx();
       Integer defectConEndIdx   = currentPhase->GetDefectConEndIdx();
       Integer totalNumConNLP    = currentPhase->GetNumTotalConNLP();
       Integer numStVars         = currentPhase->GetNumStateVars();
       
       #ifdef DEBUG_TRAJECTORY_INIT
       MessageInterface::ShowMessage("------- now calling initialize\n");
       #endif
       // Intialize the current phase
       currentPhase->Initialize();
       */
      //   currentPhase->SetGuessFunctionName(guessFunctionName);
      currentPhase->SetPathFunction(pathFunction);
#ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage("------- now calling initialize on phase\n");
      std::cout << "------- now calling initialize on phase\n";
#endif
      // Intialize the current phase
      currentPhase->Initialize();
      
      Integer defectConStartIdx = currentPhase->GetDefectConStartIdx();
      Integer defectConEndIdx   = currentPhase->GetDefectConEndIdx();
      Integer totalNumConNLP = currentPhase->GetNumTotalConNLP();
      Integer numStVars = currentPhase->GetNumStateVars();
      
#ifdef DEBUG_TRAJECTORY_INIT
      MessageInterface::ShowMessage(
                                    "------- phase %d has been initialized\n", phaseIdx);
      std::cout << "------- phase has been initialized\n";
#endif
      // Extract decision vector and constraint properties
      numPhaseConstraints.at(phaseIdx)    = currentPhase->GetNumTotalConNLP();
      
      numPhaseDecisionParams.at(phaseIdx) =
      currentPhase->GetNumDecisionVarsNLP();
      totalNumDecisionParams += numPhaseDecisionParams.at(phaseIdx);
      totalNumConstraints    += numPhaseConstraints.at(phaseIdx);
      
      if (ifScaling)
      {
#ifdef DEBUG_TRAJECTORY_INIT
         MessageInterface::ShowMessage("------- we're scaling\n");
#endif
         // Initialize the scale utility helper arrays
         if (ifDefect.size() == 0)
            ifDefect.resize(totalNumConstraints + 1);
         if (whichStateVar.size() == 0)
            whichStateVar.resize(totalNumConstraints + 1);
         Integer sz = totalNumConstraints - constraintStartIdx + 1;
         for (Integer idx = 0; idx < sz; idx++)
         {
            ifDefect.at(constraintStartIdx + idx)   = false;
            whichStateVar.at(constraintStartIdx + idx) = 0;
         }
         
         for (Integer idx = defectConStartIdx; idx <= defectConEndIdx; idx++)
         {
            // Use the phase accessor functions to determine which constraints
            // are defects
            ifDefect.at(idx + constraintStartIdx) = true;
         }
         
         // Loop through each mesh point in order to figure out the relevant
         // state variable for each defect constraint
         Integer nMeshPts     = totalNumConNLP / numStVars;
         Integer defectConIdx = constraintStartIdx;
         for (Integer meshIdx = 0; meshIdx < nMeshPts; meshIdx++)
         {
            Integer counter = 0;   // 0 to numStVars
            Integer endAt   = defectConIdx+numStVars-1;
            for (Integer idx = defectConIdx; idx <= endAt; idx++)
            {
               whichStateVar.at(idx) = counter;
               counter++;
               //defectConIdx += numStVars;
            }
            defectConIdx += numStVars;
         }
         
         // Update the constraint vector index
         constraintStartIdx = totalNumConstraints; //  + 1;
      }
   }
   
   SetChunkIndexes();
#ifdef DEBUG_TRAJECTORY_INIT
   MessageInterface::ShowMessage("-- leaving InitializePhases ...\n");
   std::cout << "-- leaving InitializePhases ...\n";
#endif
}

//------------------------------------------------------------------------------
// Rvector GetConstraintVector()
//------------------------------------------------------------------------------
/**
 * Returns the constraint vector
 *
 * @return the constraint vector
 */
//------------------------------------------------------------------------------
Rvector Trajectory::GetConstraintVector()
{
   // Loop over the phases and concatenate the constraint vectors
   RealArray conVec;
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      Rvector rv = phaseList.at(phaseIdx)->GetConstraintVector();
      Integer sz = rv.GetSize();
      for (Integer ii = 0; ii < sz; ii++)
         conVec.push_back(rv[ii]);
   }
   
   // Set the constraint vector on the class for use in partial
   //  derivatives later
   constraintVector.SetSize(conVec.size());
   for (UnsignedInt ii = 0; ii < conVec.size(); ii++)
      constraintVector(ii) = conVec.at(ii);
   return constraintVector;
}

//------------------------------------------------------------------------------
// Real GetCostFunction()
//------------------------------------------------------------------------------
/**
 * Returns the cost function value
 *
 * @return the cost function value
 */
//------------------------------------------------------------------------------
Real Trajectory::GetCostFunction()
{
   Real cf = 0.0;
   if (pointFunctionManager->HasCostFunction())
   {
      Rvector cnlpf = pointFunctionManager->ComputeCostNLPFunctions();
      cf = cnlpf[0];
   }
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      cf += phaseList.at(phaseIdx)->GetCostFunction();
   }
   
   // Set the cost function for use in partials
   costFunction = cf;
   return costFunction;
}

//------------------------------------------------------------------------------
// void SetChunkIndexes()
//------------------------------------------------------------------------------
/**
 * Sets the indexes for the chunks
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetChunkIndexes()
{
#ifdef DEBUG_TRAJECTORY_INIT
   MessageInterface::ShowMessage("In Trajectory::SetChunkIndexes\n");
#endif
   // Loop over phases and set start and end indeces for different
   // chunks of the decision vector
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      // Indeces for start and end of decision vector chunks
      Integer sumParams = 0;
      for (Integer ii = phaseIdx; ii < numPhases; ii++)
         sumParams += numPhaseDecisionParams.at(ii);
      decVecStartIdx.at(phaseIdx) = totalNumDecisionParams -
      sumParams; //  + 1;     // +1 ?????
      decVecEndIdx.at(phaseIdx) = decVecStartIdx.at(phaseIdx) +
      numPhaseDecisionParams.at(phaseIdx) - 1;
      
      // Function indeces
      Integer sumCons = 0;
      for (Integer ii = 0; ii < phaseIdx; ii++)
         sumCons += numPhaseConstraints.at(ii);
      
      conPhaseStartIdx.at(phaseIdx) = sumCons; //  + 1;   // +1?
   }
#ifdef DEBUG_TRAJECTORY_INIT
   MessageInterface::ShowMessage("LEAVING Trajectory::SetChunkIndexes\n");
#endif
}

//------------------------------------------------------------------------------
// void SetBounds()
//------------------------------------------------------------------------------
/**
 * Sets the bounds for the constraints and decision vector
 *
 */
//------------------------------------------------------------------------------
void    Trajectory::SetBounds()
{
#ifdef DEBUG_TRAJECTORY_INIT
   MessageInterface::ShowMessage("In Trajectory::SetBounds\n");
#endif
   SetConstraintBounds();
#ifdef DEBUG_TRAJECTORY_INIT
   MessageInterface::ShowMessage(
                                 "In Trajectory::SetBounds setting dec vec bounds\n");
#endif
   SetDecisionVectorBounds();
}

//------------------------------------------------------------------------------
// void SetConstraintBounds()
//------------------------------------------------------------------------------
/**
 * Sets the bounds for the constraints
 *
 */
//------------------------------------------------------------------------------
void    Trajectory::SetConstraintBounds()
{
   // Loop over all phases concatenating constraint vectors
   allConLowerBound.clear();
   allConUpperBound.clear();
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      Rvector allCLower = phaseList.at(phaseIdx)->GetAllConLowerBound();
      Rvector allCUpper = phaseList.at(phaseIdx)->GetAllConUpperBound();
#ifdef DEBUG_TRAJECTORY_BOUNDS
      MessageInterface::ShowMessage(
                                    "For phase %d, allConLowerBound = %s\n",
                                    phaseIdx, allCLower.ToString(12).c_str());
#endif
#ifdef DEBUG_TRAJECTORY_BOUNDS
      MessageInterface::ShowMessage("          and allConUpperBound = %s\n",
                                    allCUpper.ToString(12).c_str());
#endif
      for (Integer ii = 0; ii < allCLower.GetSize(); ii++)
      {
         allConLowerBound.push_back(allCLower[ii]);
      }
      for (Integer ii = 0; ii < allCUpper.GetSize(); ii++)
      {
         allConUpperBound.push_back(allCUpper[ii]);
      }
   }
   
   if (pointFunctionManager->HasBoundaryFunctions())
   {
      Rvector lowB  = pointFunctionManager->GetConLowerBound();
      Rvector highB = pointFunctionManager->GetConUpperBound();
      for (Integer ii = 0; ii < lowB.GetSize(); ii++)
         allConLowerBound.push_back(lowB[ii]);
      for (Integer ii = 0; ii < highB.GetSize(); ii++)
         allConUpperBound.push_back(highB[ii]);
   }
}

//------------------------------------------------------------------------------
// void SetDecisionVectorBounds()
//------------------------------------------------------------------------------
/**
 * Sets the upper and lower bounds on the complete decision
 *
 */
//------------------------------------------------------------------------------
void Trajectory::SetDecisionVectorBounds()
{
#ifdef DEBUG_TRAJECTORY_INIT
   MessageInterface::ShowMessage(
                                 "In Trajectory::SetDecisionVectorBounds\n");
   MessageInterface::ShowMessage(
                                 "   totalNumDecisionParams = %d\n",
                                 totalNumDecisionParams);
#endif
   
   decisionVecLowerBound.SetSize(totalNumDecisionParams);
   decisionVecUpperBound.SetSize(totalNumDecisionParams);
   Integer lowIdx = 0;
   Integer highIdx;
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      highIdx = lowIdx + numPhaseDecisionParams.at(phaseIdx)-1;
      Integer idx = 0;
      for (Integer ii = lowIdx; ii <= highIdx; ii++)
      {
         Rvector dvLow = phaseList.at(phaseIdx)->GetDecVecLowerBound();
         Rvector dvUpp = phaseList.at(phaseIdx)->GetDecVecUpperBound();
         decisionVecLowerBound(ii)  = dvLow(idx);
         decisionVecUpperBound(ii)  = dvUpp(idx++);
      }
      lowIdx = highIdx + 1;
   }
#ifdef DEBUG_TRAJECTORY_INIT
   MessageInterface::ShowMessage(
                                 "LEAVING Trajectory::SetDecisionVectorBounds\n");
#endif
}

//------------------------------------------------------------------------------
// void SetSparsityPattern()
//------------------------------------------------------------------------------
/**
 * Sets the sparsity pattern
 *
 */
//------------------------------------------------------------------------------
void    Trajectory::SetSparsityPattern()
{
   // updated so that cost, constraint patterns are calculated. YK 2017.09.28
#ifdef DEBUG_SPARSITY
   MessageInterface::ShowMessage(
                                 "Entering SetSparsityPattern, totalNumConstraints = %d, "
                                 "totalNumDecisionParams = %d\n", totalNumConstraints,
                                 totalNumDecisionParams);
#endif
   // Set the sparsity pattern for the  NLP problem
   
   
   //  Initialize the sparse matrix
   SparseMatrixUtil::SetSize(sparsityCost, 1 , totalNumDecisionParams);
   SparseMatrixUtil::SetSize(sparsityConstraints, totalNumConstraints,
                             totalNumDecisionParams);
   SparseMatrixUtil::SetSize(sparsityPattern, 1+totalNumConstraints,
                             totalNumDecisionParams);
   
   // Handle the path constraints
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      // Determine the indeces for the diagonal block
      Phase   *currentPhase = phaseList.at(phaseIdx);
      Integer pNum          = currentPhase->GetPhaseNumber();
      // Add one because cost function is first
      Integer lowConIdx  = conPhaseStartIdx.at(phaseIdx) +1; //  + 1;
      Integer lowVarIdx  = decVecStartIdx.at(pNum); // phaseIdx); // pNum);
      
      // Insert the current block in the right location
      RSMatrix costSparse = currentPhase->GetCostSparsityPattern();
      RSMatrix conSparse  = currentPhase->GetConSparsityPattern();
      
#ifdef DEBUG_SPARSITY
      Integer highConIdx = lowConIdx + currentPhase->GetNumTotalConNLP() - 1;
      Integer highVarIdx = decVecEndIdx.at(pNum);   // phaseIdx); // pNum);
      MessageInterface::ShowMessage(" --- lowConIdx  = %d\n", lowConIdx);
      MessageInterface::ShowMessage(" --- highConIdx = %d\n", highConIdx);
      MessageInterface::ShowMessage(" --- lowVarIdx  = %d\n", lowVarIdx);
      MessageInterface::ShowMessage(" --- highVarIdx = %d\n", highVarIdx);
      
      MessageInterface::ShowMessage(
                                    "Cost sparsity pattern (for phase %d):\n",
                                    phaseIdx);
      Integer rJac = costSparse.size1();
      Integer cJac = costSparse.size2();
      for (Integer cc = 0; cc < cJac; cc++)
      {
         for (Integer rr = 0; rr < rJac; rr++)
         {
            Real jacTmp = costSparse(rr,cc);
            if (jacTmp != 0.0)
               MessageInterface::ShowMessage(
                                             " CostSparse(%d, %d)   %12.10f\n",
                                             rr+1, cc+1, jacTmp);
         }
      }
      MessageInterface::ShowMessage("Con sparsity pattern (for phase %d):\n",
                                    phaseIdx);
      rJac = conSparse.size1();
      cJac = conSparse.size2();
      for (Integer cc = 0; cc < cJac; cc++)
      {
         for (Integer rr = 0; rr < rJac; rr++)
         {
            Real jacTmp = conSparse(rr,cc);
            if (jacTmp != 0.0)
               MessageInterface::ShowMessage(
                                             " ConSparse(%d, %d)   %12.10f\n",
                                             rr+1, cc+1, jacTmp);
         }
      }
#endif
      
      SparseMatrixUtil::SetSparseBLockMatrix(sparsityCost, 0, lowVarIdx,
                                             &costSparse);
      
      SparseMatrixUtil::SetSparseBLockMatrix(sparsityPattern, 0, lowVarIdx,
                                             &costSparse);
      
      SparseMatrixUtil::SetSparseBLockMatrix(sparsityConstraints, lowConIdx - 1,
                                             lowVarIdx, &conSparse);
      SparseMatrixUtil::SetSparseBLockMatrix(sparsityPattern, lowConIdx,
                                             lowVarIdx, &conSparse);
   }
   
   // Handle boundary constraints/linkage constraints
   Integer funcIdxLow  = totalNumConstraints - numBoundFunctions + 1;
   Integer funcIdxHigh = totalNumConstraints;
   IntegerArray funcIdxs;
   for (Integer ii = funcIdxLow; ii <= funcIdxHigh; ii++)
      funcIdxs.push_back(ii);
   
   if (pointFunctionManager->HasBoundaryFunctions())
   {
      RSMatrix *boundSparse = pointFunctionManager->
      ComputeBoundNLPSparsityPattern();
#ifdef DEBUG_SPARSITY
      MessageInterface::ShowMessage(" --- funcIdxLow  = %d\n", funcIdxLow);
      MessageInterface::ShowMessage(" --- funcIdxHigh = %d\n", funcIdxHigh);
      
      MessageInterface::ShowMessage("Bound NLP sparsity pattern:\n");
      Integer rJac = boundSparse->size1();
      Integer cJac = boundSparse->size2();
      for (Integer cc = 0; cc < cJac; cc++)
      {
         for (Integer rr = 0; rr < rJac; rr++)
         {
            Real jacTmp = (*boundSparse)(rr,cc);
            if (jacTmp != 0.0)
               MessageInterface::ShowMessage(" (%d, %d)   %12.10f\n",
                                             rr+1, cc+1, jacTmp);
         }
      }
#endif
      
      SparseMatrixUtil::SetSparseBLockMatrix(sparsityConstraints, funcIdxs.at(0) - 1,
                                             0, boundSparse);
      SparseMatrixUtil::SetSparseBLockMatrix(sparsityPattern, funcIdxs.at(0), 0,
                                             boundSparse);
   }
   
   // Handle cost function
   if (pointFunctionManager->HasCostFunction())
   {
      RSMatrix *costSparse2 = pointFunctionManager->
      ComputeCostNLPSparsityPattern();
#ifdef DEBUG_SPARSITY
      MessageInterface::ShowMessage("Cost NLP sparsity pattern :\n");
      Integer rJac = costSparse2->size1();
      Integer cJac = costSparse2->size2();
      for (Integer cc = 0; cc < cJac; cc++)
      {
         for (Integer rr = 0; rr < rJac; rr++)
         {
            Real jacTmp = (*costSparse2)(rr,cc);
            if (jacTmp != 0.0)
               MessageInterface::ShowMessage(" (%d, %d)   %12.10f\n",
                                             rr+1, cc+1, jacTmp);
         }
      }
#endif
      SparseMatrixUtil::SetSparseBLockMatrix(sparsityCost, 0, 0,
                                             costSparse2);
      SparseMatrixUtil::SetSparseBLockMatrix(sparsityPattern, 0, 0,
                                             costSparse2);
   }
   
   // YK mod IPOPT; compute hessian pattern here.
   ComputeHessianSparsityPattern();
}


//------------------------------------------------------------------------------
// void SetSNOPTIterationDependentSettings(Integer iterNum)
//------------------------------------------------------------------------------
/**
 * Sets the iteration-dependent settings on th SNOPT optimizer.  This method
 * will set the default value for any quantity that has not been specified.
 *
 * @param iterNum   iteration number
 */
//------------------------------------------------------------------------------
void Trajectory::SetSNOPTIterationDependentSettings(Integer iterNum)
{
   Integer     sz       = 0;
   Real        theTol   = 0.0;
   Integer     theLimit = 0;
   std::string theMode  = "";

   // Set feasibility tolerances
   if (!feasibilityTolerances.IsSized())
      trajOptimizer->SetFeasibilityTolerance(1.0e-06); // 1.0e-07);
   else
   {
      sz = feasibilityTolerances.GetSize();
      theTol = iterNum < sz? feasibilityTolerances(iterNum) :
                             feasibilityTolerances(sz-1);
      trajOptimizer->SetFeasibilityTolerance(theTol);
   }
   // Set optimality tolerances
   if (!optimalityTolerances.IsSized())
      trajOptimizer->SetMajorOptimalityTolerance(1.0e-04);
   else
   {
      sz = optimalityTolerances.GetSize();
      theTol = iterNum < sz? optimalityTolerances(iterNum) :
                             optimalityTolerances(sz-1);
      trajOptimizer->SetMajorOptimalityTolerance(theTol);
   }

   // Set major iterations limit
   if (majorIterationsLimit.empty())
      trajOptimizer->SetMajorIterationsLimit(1000);
   else
   {
      sz = majorIterationsLimit.size();
      theLimit = iterNum < sz? majorIterationsLimit.at(iterNum) :
                               majorIterationsLimit.at(sz-1);
      trajOptimizer->SetMajorIterationsLimit(theLimit);
   }
   
   // Set total iterations limit
   if (totalIterationsLimit.empty())
      trajOptimizer->SetTotalIterationsLimit(20000);
   else
   {
      sz = totalIterationsLimit.size();
      theLimit = iterNum < sz? totalIterationsLimit.at(iterNum) :
                               totalIterationsLimit.at(sz-1);
      trajOptimizer->SetTotalIterationsLimit(theLimit);
   }
   
   // Set optimization mode
   if (optimizationMode.empty())
   {
      trajOptimizer->SetOptimizationMode("Minimize");
   }
   else
   {
      sz = optimizationMode.size();
      theMode = iterNum < sz? optimizationMode.at(iterNum) :
                              optimizationMode.at(sz-1);
      trajOptimizer->SetOptimizationMode(theMode);
   }
}

//------------------------------------------------------------------------------
// void ResetBestFeasibleSolution()
//------------------------------------------------------------------------------
/**
 * Resets the saved best solution to default feasibility and optimality values.
 * The decision vector is set to the current stored values in the phases.  This
 * method should be called whenever a new mesh iteration has begun.
 */
//------------------------------------------------------------------------------
void Trajectory::ResetBestFeasibleSolution()
{
   bestFeasCurrMesh = std::numeric_limits<Real>::infinity();
   bestOptCurrMesh = std::numeric_limits<Real>::infinity();
   wasFeasiblityAchieved = false;

   Rvector emptyVec(0);
   if (bestFeasibleDecVec.size() != numPhases)
   {
      bestFeasibleDecVec.clear();
      for (UnsignedInt ii = 0; ii < numPhases; ++ii)
         bestFeasibleDecVec.push_back(emptyVec);
   }

   for (Integer ii = 0; ii < numPhases; ++ii)
   {
      Rvector phaseDecVec =
         phaseList.at(ii)->GetDecisionVector()->GetDecisionVector();
      if (bestFeasibleDecVec.at(ii).GetSize() != phaseDecVec.GetSize())
         bestFeasibleDecVec.at(ii).SetSize(phaseDecVec.GetSize());
      bestFeasibleDecVec.at(ii) = phaseDecVec;
   }

   Real maxConViol, costVal;
   ComputeMaxConstraintViolation(maxConViol, costVal);
   bestCostCurrMesh = costVal;
}
