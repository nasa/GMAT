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
 * TRAJECTORY The trajectory class is the executive for the optimal
 * control subsystem
 *
 *   Regarding Scaling:  All data internally is stored in dimensional
 *   units.  Scaling and unscaling is performed using the PUBLIC
 *   interface to the class. All protected and private data interfaces
 *   use dimensional data.  All public interfaces assume dimensional
 *   data if flag is not passed in.
 */
//------------------------------------------------------------------------------

#ifndef Trajectory_hpp
#define Trajectory_hpp

//#include "Optimizer.hpp"
#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "Phase.hpp"
#include "ScaleUtility.hpp"
#include "SparseMatrixUtil.hpp"
#include "UserPathFunction.hpp"
#include "UserPointFunction.hpp"
#include "UserPointFunctionManager.hpp"
#include "OCHTrajectoryData.hpp"
#include "ExecutionInterface.hpp"

class SnoptOptimizer;
class CSALT_API Trajectory
{
public:

   Trajectory();
   Trajectory(const Trajectory &copy);
   Trajectory& operator=(const Trajectory &copy);
   virtual ~Trajectory();

   /// Initialize methods
   virtual void                Initialize();
   virtual void                InitializeScaleUtility();
   virtual void                InitializePointFunctions();
   
   /// Optimization methods
   virtual void                PrepareToOptimize();
   virtual bool                PrepareMeshGuess(Integer exitFlag);
   virtual void                SetDecisionVector(Rvector decVec);
   virtual void                UpdateGuessForMesh();
   virtual void                Optimize(const std::string &optFile = "");
   virtual void                Optimize(Rvector &decVec,   
                                        Rvector &F,
                                        Rvector &xmul,
                                        Rvector &Fmul,
                                        Integer &exitFlag,
                                        const std::string &optFile = "");
   
   /// Accessor methods
   virtual RSMatrix            GetJacobian();
   virtual Rvector             GetCostConstraintFunctions();
   //virtual RSMatrix            GetJacobian(Rvector decVec);
   //virtual RSMatrix            GetCostJacobian(Rvector decVec);
   //virtual RSMatrix            GetConstraintJacobian(Rvector decVec);
   //virtual Rvector             GetCostConstraintFunctions(Rvector decVec);
   //virtual Rvector             GetAllConstraintFunctions(Rvector decVec);
   //virtual Rvector             GetConstraintVector(Rvector decVec);
   //virtual Real                GetCostFunction(Rvector decVec);

   virtual Rvector             GetScaledConstraintFunctions(
                                                   BooleanArray &ifEquality);
   virtual StringArray         GetConstraintVectorNames();
   
   virtual Real                GetCostLowerBound();
   virtual Real                GetCostUpperBound();
   virtual RealArray           GetAllConLowerBound();
   virtual RealArray           GetAllConUpperBound();
   virtual UserPathFunction*   GetUserPathFunction();
   virtual UserPointFunction*  GetUserPointFunction();
   virtual std::string         GetGuessFunctionName();
   virtual std::vector<Phase*> GetPhaseList();
   virtual Integer             GetNumPhases();
   virtual Integer             GetMeshRefinementCount();
   virtual Integer             GetMaxMeshRefinementCount();
   virtual Real                GetCostScaling();
   virtual RSMatrix            GetSparsityPattern();
   virtual RSMatrix            GetCostSparsityPattern();
   virtual RSMatrix            GetConstraintSparsityPattern();
   virtual RSMatrix            GetHessianSparsityPattern(); // YK mod IPOPT
   virtual Rvector             GetDecisionVector();
   virtual Rvector             GetDecisionVectorLowerBound();
   virtual Rvector             GetDecisionVectorUpperBound();
   virtual bool                GetIfScaling();
   virtual bool                GetIfStopping();
   virtual void                GetFeasibleSolution(
                                             std::vector<Rvector> &feasDecVecs,
                                             Real &feasOfSolution,
                                             Real &optOfSolution,
                                             bool &isSolFeasible);
   
   virtual void                SetCostLowerBound(Real costLower);
   virtual void                SetCostUpperBound(Real costUpper);
   
   virtual void                SetUserPathFunction(UserPathFunction *func);
   virtual void                SetUserPointFunction(UserPointFunction *func);
   
   virtual void                SetGuessFunctionName(const std::string &toName);
   virtual void                SetPhaseList(std::vector<Phase*> pList);
   virtual void                SetMaxMeshRefinementCount(Integer toCount);
   virtual void                SetMeshRefinementGuessMode(
                                               const std::string &toGuessMode);
   virtual void                SetFailedMeshOptimizationAllowance(
                                               bool toAllowance);
   virtual void                SetCostScaling(Real toScaling);
   virtual void                SetInitialGuess();
   
   /// Methods to set SNOPT inputs
   virtual void                SetFeasibilityTolerances(const Rvector &tol);
   virtual void                SetOptimalityTolerances(const Rvector &tol);
   virtual void                SetMajorIterationsLimit(const IntegerArray &iter);
   virtual void                SetTotalIterationsLimit(const IntegerArray &iter);
   virtual void                SetOptimizationMode(const StringArray &optMode);
   virtual void                SetApplicationType(const std::string &appType);
   virtual void SetSnoptConsoleOutputLevel(const Integer level);
   virtual Integer GetSnoptConsoleOutputLevel();
   
   virtual std::string         GetApplicationType();
   virtual ScaleUtility*  	    GetScaleHelper();

   virtual void                SetExecutionInterface(ExecutionInterface *execInt);

   virtual void                WriteToFile(std::string fileName);
   virtual void                ReportBoundData(Rvector boundData);   // YK mod IPOPT
   virtual void                ComputeHessianSparsityPattern();
   virtual RSMatrix            ComputeHessianContraction(Rvector decVec, 
                                                         Rvector lambdaVec);
   virtual void                ComputeMaxConstraintViolation(
                                                         Real &maxConViolation,
                                                         Real &costValue);
   virtual void                CheckSolutionFeasibility(Real feas, Real opt);
   virtual Integer             GetSimplifiedSNOPTExitFlag(Integer snoptExitFlag);
   virtual std::string         GetSNOPTExitFlagMessage(Integer snoptExitFlag);
   
protected:  
   
   // 'public' data
   /// The lower bound on the cost function
   Real                costLowerBound;
   /// The upper bound on the cost function
   Real                costUpperBound;
   /// The name of the user guess function   - TBD
   std::string         guessFunctionName;
   /// The name of the user plot function   - TBD
   std::string         plotFunctionName;
   /// The path function object
   UserPathFunction    *pathFunction;
   /// The point function object
   UserPointFunction   *pointFunction;
   /// Flag indicating whether or not to show the plot
   bool                showPlot;
   /// Number of function evals between plot updates
   Integer             plotUpdateRate;

   /// The array of Phases for the problem
   std::vector<Phase*> phaseList;
   /// Maximum allowed number of mesh iterations
   Integer             maxMeshRefinementCount;
   /// Flag to indicate if the problem will be solved in auto-scaled
   /// units or not
   bool                ifScaling;
   /// The scaling of the cost function (If the auto-scaling utility is used)
   Real                costScaling;

   // 'private' data
   /// Total decision vector for optimization problem containing chunks for
   /// all phases.
   Rvector             decisionVector;
   /// The lower bound vector for the decision vector
   Rvector             decisionVecLowerBound;
   /// The upper bound vector for the decision vector
   Rvector             decisionVecUpperBound;
   /// Decision vector to be used as a guess for mesh refinement
   Rvector             meshGuessDecVec;
   /// Whether a new mesh has been sent to the optimizer
   bool                isNewMesh;
   /// The ith entry is the start of the decision vector chunk for the ith phase
   IntegerArray        decVecStartIdx;
   /// The ith entry is the end of the decision vector chunk for the ith phase
   IntegerArray        decVecEndIdx;
   /// Number of elements in the trajectory decision vector
   Integer             totalNumDecisionParams;
   /// The ith entry is the number of decision parameters for the ith phase
   IntegerArray        numPhaseDecisionParams;
   /// The number of phases in the problem
   Integer             numPhases;
   /// The ith element is the number of constraints in the ith phase.
   IntegerArray        numPhaseConstraints;
   /// The total number of constraints including all phases and linkages
   Integer             totalNumConstraints;
   /// Total number or boundary functions
   Integer             numBoundFunctions;
   /// The lower bound vector for all constraints in the problem (i.e.
   /// for all phases).
   RealArray           allConLowerBound;
   /// The upper bound vector for all constraints in the problem (i.e.
   /// for all phases).
   RealArray           allConUpperBound;
   /// The ith element is the index indicates where the constraints for the ith
   /// phase start in the concatenated constraint vector
   IntegerArray        conPhaseStartIdx;
   /// The vector of constraint values
   Rvector             constraintVector;
   
   /// ==== SNOPT inputs
   Rvector             feasibilityTolerances;
   /// Feasibilty toleranceas
   Rvector             optimalityTolerances;
   /// Major iterations limits
   IntegerArray        majorIterationsLimit;
   /// Total iterations limits
   IntegerArray        totalIterationsLimit;
   /// Optimization mode
   StringArray         optimizationMode;
   /// How much should SNOPT write to screen?
   Integer snoptConsoleOutputLevel;

   /// ==== Feasible solution storage
   /// Boolean of whether a feasible solution was ever found on any major
   /// iteration of the current mesh
   bool                wasFeasiblityAchieved;
   /// The best optimizer fesibility achieved in the current mesh
   Real                bestFeasCurrMesh;
   /// The best cost achieved from a feasible solution in the current mesh
   Real                bestCostCurrMesh;
   /// The optimizer optimality achieved from the lowest cost feasible 
   /// solution in the current mesh
   Real                bestOptCurrMesh;
   /// The decision vector of the best feasible solution
   std::vector<Rvector> bestFeasibleDecVec;
   
            
   /// ==== Sparsity pattern data members
            
   /// sparsity pattern for the cost function
   RSMatrix            sparsityCost;
   /// Sparsity pattern for the constraints
   RSMatrix            sparsityConstraints;
   /// The sparsity pattern for the complete problem including cost and
   /// constraints.
   RSMatrix            sparsityPattern;
   /// // YK mod IPOPT; hessian pattern
   RSMatrix            hessianPattern;
                     
   /// ==== Helper classes
                     
   /// Optimizer helper class

   SnoptOptimizer           *trajOptimizer;   /// Manager for point function evaluation and partials
   UserPointFunctionManager *pointFunctionManager;
                        
   /// Non-dimensionalizer helper class
   ScaleUtility             *scaleHelper;
   /// which state variable corresponds to each defect constraint
   IntegerArray             whichStateVar;
   /// vector to track whether constraints are defect or not
   BooleanArray             ifDefect;
                        
   /// ==== Housekeeping parameters
                        
   /// Flag indicating whether or not optimization is under way
   bool                     isOptimizing;
   /// Flag indicating that optimization is complete
   bool                     isFinished;
   /// Number of func evals since the last plot update
   Integer                  plotUpdateCounter;
   /// Flag to write out debug data
   bool                     displayDebugStatus;
   /// Flag for when perturbing for finite differencing
   bool                     isPerturbing;
   /// Number of function evals during optimization
   Integer                  numFunEvals;
   /// Indicates if mode is mesh refinement
   bool                     isMeshRefining;
   /// Number of mesh refinements performed
   Integer                  meshRefinementCount;
   /// Flag indicating whether or not to continue with mesh refinement even
   /// if a previous optimization failed
   bool                     allowFailedMeshOptimizations;
   /// Indicates which method to use to create an initial guess for the next
   /// mesh refinement
   std::string              meshGuessMode;
   /// Current max constraint violation of solution potentially being used 
   /// for a future mesh guess
   Real                     guessMaxConViol;
   /// Current cost function value of solution potentially being used 
   /// for a future mesh guess
   Real                     guessCostVal;
   /// Pointer to ExecutionInterface base class for sending optimization data to
   ExecutionInterface       *csaltExecInterface;
   /// The current state of optimization which is then sent to the Publisher
   std::string              csaltState;
   /// The application type being used, determines how optimizer data printing
   /// is handled
   std::string              applicationType;
   /// The exit flag taken from the optimizer
   Integer                  optExitFlag;
   
   /// === data for output parameters; YK 2018.02.01
   
   /// 
   /// The cost function value
   Real                 costFunction;
   /// The value of of the largest constraint violation
   Real                 maxConViolation;
   /// The cost gradient jacobian
   /// Note that IPOPT only uses cost/constraintJacobian
   RSMatrix             costJacobian;
   /// The constraint gradient jacobian
   /// Note that IPOPT only uses cost/constraintJacobian
   RSMatrix             constraintJacobian;
   /// All jacobian including cost and constraints
   /// Note that SNOPT7 only uses jacobian
   RSMatrix             jacobian;

   /// Compute methods newly implemented by YK 2018.02.01
   //virtual Real                ComputeCostFunction();
   //virtual Rvector             ComputeAllConstraintFunctions();
   //virtual RSMatrix            ComputeJacobian();
   //virtual RSMatrix            ComputeCostJacobian();
   //virtual RSMatrix            ComputeConstraintJacobian();
   
   /// Copy all the array data
   virtual void             CopyArrays(const Trajectory &copy);
   
   virtual void             InitializePhases();
   virtual Rvector          GetConstraintVector();
   virtual Real             GetCostFunction();
   virtual void             SetChunkIndexes();
   virtual void             SetBounds();
   virtual void             SetConstraintBounds();
   virtual void             SetDecisionVectorBounds();
   virtual void             SetSparsityPattern();
   
   virtual void             SetSNOPTIterationDependentSettings(Integer iterNum);

   virtual void             ResetBestFeasibleSolution();
};

#endif // Trajectory_hpp
