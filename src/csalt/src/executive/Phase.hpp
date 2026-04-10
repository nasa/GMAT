//------------------------------------------------------------------------------
//                              Phase
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
// Created: 2016.02.26
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef Phase_hpp
#define Phase_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "DecisionVector.hpp"
#include "GuessGenerator.hpp"
#include "ProblemCharacteristics.hpp"
#include "UserFunctionProperties.hpp"
#include "UserPathFunction.hpp"
#include "UserPathFunctionManager.hpp"
#include "FunctionOutputData.hpp"
#include "FunctionInputData.hpp"
#include "NLPFuncUtilRadau.hpp"
#include "NLPFuncUtil_AlgPath.hpp"
#include "LowThrustException.hpp"
#include "SparseMatrixUtil.hpp"
#include "ArrayTrajectoryData.hpp"
#include "ScalingUtility.hpp"

class CSALT_API Phase
{
public:
   
   Phase();
   Phase(const Phase &copy);
   Phase& operator=(const Phase &copy);
   virtual ~Phase();
   
   /// Sets the initial guess mode
   virtual void            SetInitialGuessMode(const std::string &toMode);
   /// Sets the guess function name
   virtual void            SetUserGuessClass(TrajectoryData *userGuessClass);
   /// Sets the guess file name
   virtual void            SetGuessFileName(const std::string &toName);
   /// Sets the guess array data
   virtual void            SetInitialGuessArrays(Rvector timeArray,
                                                 Rmatrix stateArray,
                                                 Rmatrix controlArray);
   /// Sets the guess array data; YK mod static params
   virtual void            SetInitialGuessArrays(Rvector timeArray,
                                                 Rmatrix stateArray,
                                                 Rmatrix controlArray, 
                                                 Rvector staticVec);

   /// Sets the path function name
   virtual void            SetPathFunction(UserPathFunction* f);
   // Prepare to Optimize
   virtual void            PrepareToOptimize();

   /// Initializes the Phase
   virtual void            Initialize();
   /// Sets the decision vector as Rvector
   virtual bool            SetDecisionVector(const Rvector &decVec);
   /// returns pointer to decision vector object
   virtual DecisionVector* GetDecisionVector();
   /// returns decision vector value (as RVector)
   virtual Rvector         GetDecVector();
   /// Set the number of state variables
   virtual void            SetNumStateVars(Integer toNum);
   /// Set the number of control variables
   virtual void            SetNumControlVars(Integer toNum);
   
   virtual void            SetRelativeErrorTol(Real toNum);


   /// Get the number of state variables
   virtual Integer         GetNumStateVars();
   /// Get the number of control variables
   virtual Integer         GetNumControlVars();

   /// Get the defect constraint start index
   virtual Integer         GetDefectConStartIdx();
   /// Get the defect constraint end index
   virtual Integer         GetDefectConEndIdx();
   
   /// Set the phase number
   virtual void            SetPhaseNumber(Integer toNum);
   /// Get the phase number
   virtual Integer         GetPhaseNumber();

   /// Get the lower bounds for all of the constraints
   virtual Rvector         GetAllConLowerBound();
   /// Get the upper bounds for all of the constraints
   virtual Rvector         GetAllConUpperBound();
   
   /// Get the lower bounds for the decision vector
   virtual Rvector         GetDecVecLowerBound();
   /// Get the upper bounds for the decision vector
   virtual Rvector         GetDecVecUpperBound();

   /// Set the mesh interval fractions
   virtual void            SetMeshIntervalFractions(const Rvector &fractions);
   /// Get the mesh interval fractions
   virtual Rvector         GetMeshIntervalFractions();
   /// Set the number of points for the mesh intervals
   virtual void            SetMeshIntervalNumPoints(const IntegerArray &toNum);
   /// Get the number of points for the mesh intervals
   virtual IntegerArray    GetMeshIntervalNumPoints();
   
   /// Set the state lower bounds
   virtual void            SetStateLowerBound(const Rvector &bound);
   /// Get the state lower bounds
   virtual Rvector         GetStateLowerBound();
   /// Set the state upper bound
   virtual void            SetStateUpperBound(const Rvector &bound);
   /// Get the state upper bound
   virtual Rvector         GetStateUpperBound();
   /// Set the initial guess for the state
   virtual void            SetStateInitialGuess(const Rvector &guess);
   /// Get the initial guess for the state
   virtual Rvector         GetStateInitialGuess();
   /// Set the final guess for the state
   virtual void            SetStateFinalGuess(const Rvector &guess);
   /// Get the final guess for the state
   virtual Rvector         GetStateFinalGuess();
   /// Set the lower bound for the time
   virtual void            SetTimeLowerBound(Real bound);
   /// Get the lower bound for the time
   virtual Real            GetTimeLowerBound();
   /// Set the upper bound for the time
   virtual void            SetTimeUpperBound(Real bound);
   /// Get the upper bound for the time
   virtual Real            GetTimeUpperBound();
   /// Set the initial guess for the time
   virtual void            SetTimeInitialGuess(Real guess);
   /// Get the initial guess for the time
   virtual Real            GetTimeInitialGuess();
   /// Set the final guess for the time
   virtual void            SetTimeFinalGuess(Real guess);
   /// Get the final guess for the time
   virtual Real            GetTimeFinalGuess();
   /// Set the lower bound for the control
   virtual void            SetControlLowerBound(const Rvector &bound);
   /// Get the lower bound for the control
   virtual Rvector         GetControlLowerBound();
   /// Set the upper bound for the control
   virtual void            SetControlUpperBound(const Rvector &bound);
   /// Get the upper bound for the conrol
   virtual Rvector         GetControlUpperBound();
   /// Get the total number of constraints
   virtual Integer         GetNumTotalConNLP();
   /// Get the number of decision variables
   virtual Integer         GetNumDecisionVarsNLP();
   /// Get the number of non-zeros
   virtual IntegerArray    GetNumNLPNonZeros();
   
   // YK mod static params
   /// Set the number of static variables; YK mod static params
   virtual void            SetNumStaticVars(Integer toNum);
   /// Set the number of static variables; YK mod static params
   virtual Integer         GetNumStaticVars();
   /// Set the static parameter lower bounds
   virtual void            SetStaticLowerBound(const Rvector &bound);
   /// Get the static parameter lower bounds
   virtual Rvector         GetStaticLowerBound();
   /// Set the static parameter upper bound
   virtual void            SetStaticUpperBound(const Rvector &bound);
   /// Get the static parameter upper bound
   virtual Rvector         GetStaticUpperBound();
   /// Set the static parameter guess for the state
   virtual void            SetStaticGuess(const Rvector &guess);
   /// Get the static parameter guess for the state
   virtual Rvector         GetStaticVector();


   /// Get the time vector
   virtual Rvector         GetTimeVector();
   
   /// Initialize the NLP helpers
   virtual void            InitializeNLPHelpers();
   
   /// Get the constaint vector
   virtual Rvector         GetConstraintVector();

   /// Get the constraint vector names
   virtual StringArray     GetConstraintVectorNames();
   /// Compute the defect constraints
   virtual void            ComputeDefectConstraints();
   /// Compute the integral cost
   virtual void            ComputeIntegralCost();
   /// COmpute the algebraic path constraints
   virtual void            ComputeAlgebraicPathConstraints();

   /// Set the flag to recompute the user functions
   virtual void            SetRecomputeUserFunctions(bool toFlag);

   /// Get the cost function
   virtual Real            GetCostFunction();
   /// Get the cost jacobian
   virtual RSMatrix        GetCostJacobian();
   /// Get the constraint jacobian
   virtual RSMatrix        GetConJacobian();
   /// Get the cost sparsity pattern
   virtual RSMatrix        GetCostSparsityPattern();
   /// Get the constraint sparsity pattern
   virtual RSMatrix        GetConSparsityPattern();
   /// Get the initial final time
   virtual Rvector         GetInitialFinalTime();

   /// Debug the path function
   virtual void            DebugPathFunction();  // needed?
   
   /// Are there algenraic path constraints?
   virtual bool            HasAlgPathCons();
   
   /// Compute the algebraic function and jacobian
   virtual void            ComputeAlgFuncAndJac(Rvector  &funcValues,
                                                RSMatrix &jacArray);

   /// Perform mesh refinement
   virtual void            RefineMesh(bool        ifUpdateMesh,
                                      bool         &isMeshRefined);

   /// Compute the max interval error of the current mesh
   virtual Rvector ComputeMaxMeshError();
   
   /// Data access and reporting methods
   virtual Rmatrix         GetStateArray();
   virtual Rmatrix         GetControlArray();
   virtual Real            GetFirstTime();
   virtual Real            GetLastTime();
   virtual ScalingUtility* GetScaleUtility();
   virtual void            SetStateArray(Rmatrix stateArray);
   virtual void            SetControlArray(Rmatrix controlArray);   
   virtual Rvector         GetMaxRelativeMeshErrorVector();
   
   virtual void            ReportBoundsData(bool addHeader = true);
   virtual void            ReportDecisionVectorData(bool addHeader = true);
   virtual void            ReportDefectConstraintData(bool addHeader = true);
   virtual void            ReportAlgebraicConstraintData(bool addHeader = true);
   virtual void            ReportAllData();
   virtual std::string     ReportHeaderData();


protected:
   /// The decision vector - helper class for managing data in dec. vec.
   DecisionVector         *decVector;
   /// The config object - problem settings and shared info
   ProblemCharacteristics *config;
   
   /// The model for the initial guess; options:
   /// LinearNoControl, LinearUnityControl, Propagated, File
   std::string            initialGuessMode;
   /// Guess for control  @todo should this be Rmatrix??
   Rvector                initialGuessControl;
   /// The user's path function
   UserPathFunction       *pathFunction;
   /// The name of the user's guess function
   TrajectoryData         *userGuessClass;
   /// The name of the user's initial guess file
   std::string            guessFileName;

   /// The assigned phase number from Trajectory
   Integer                phaseNum;
   /// Number of algebraic path constraints in the NLP
   Integer                numAlgPathConNLP;
   /// Vector of algebraic path constraint values   @todo - an Rmatrix?
   Rvector                algPathConVec;
   /// Vector of defect constraint values
   Rvector                defectConVec;
   /// Array of algebraic path constraint descriptions
   StringArray            algPathConVecNames;
   /// Integral contribution to cost function
   Real                   costFunctionIntegral;
   /// Sparse Matrix: the jacobian of the NLP constraints  
   RSMatrix               nlpConstraintJacobian;

   /// Sparse matrix: the Jacbian of the NLP cost (algebraic + quadrature)
   RSMatrix               nlpCostJacobian;
   /// Sparse matrix: the sparsity pattern for the phase NLP contraints
   RSMatrix               conSparsityPattern;
   /// Sparse matrix: sparsity pattern for the cost function
   RSMatrix               costSparsityPattern;
   /// Lower bound vector on defect constraints
   Rvector                defectConLowerBound; // Rmatrix?
   /// Upper bound vector on defect constraints
   Rvector                defectConUpperBound; // Rmatrix?
   /// Lower bound vector on algebraic path constraints
   Rvector                algPathConLowerBound;// Rmatrix?
   /// Upper bound vector on algebraic path constraints
   Rvector                algPathConUpperBound; // Rmatrix?
   /// Lower bound vector on algebraic event constraints
   Rvector                algEventConLowerBound; // Rmatrix?
   /// Upper bound vector on algebraic event constraints
   Rvector                algEventConUpperBound; // Rmatrix?
   /// Lower bound vector for all constraints concatenated
   Rvector                allConLowerBound; // Rmatrix?
   /// Upper bound vector for all constraints concatenated
   Rvector                allConUpperBound; // Rmatrix?
   /// Lower bound on decision vector
   Rvector                decisionVecLowerBound; // Rmatrix?
   /// Upper bound on decision vector
   Rvector                decisionVecUpperBound; // Rmatrix?
   /// The index for the first defect constraint
   Integer                defectConStartIdx;
   /// The index for the first alg path constraint
   Integer                algPathConStartIdx;
   /// The index for the last defect constraint
   Integer                defectConEndIdx;
   /// The index for the last defect constraint
   Integer                algPathConEndIdx;

   ///  Flag to indicate if functions must be recomputed
   ///  This is set to true whenever decision vector is reset.
   ///  This is set to false after call to ComputeUserFunctions()
   bool                   recomputeUserFunctions;
   /// Indicates Initialization is complete.
   bool                   isInitialized;
   /// Flag to indicate if quadratures must be recomputed.
   ///  This is set to true whenever decision vector is set or
   ///  ComputeUserFunctions() is called.  Set to false in
   ///  ComputeNLPFunctions()
   bool                   recomputeNLPFunctions;
   /// Flag to indicate that the phase has been run once and is now in
   ///  mesh refinement mode.
   bool                   isRefining;
   /// Offset between number of constraints and number of time points
   Integer                constraintTimeOffset;


   ///  Array of PathFunctionContainer pointers.  Contains user path
   ///  functions.
   std::vector<PathFunctionContainer*>  funcData;
   ///  Array of UserFunctionData pointers.  Contains user dynamics
   ///  functions evaluated at path points.
   std::vector<FunctionOutputData*>     userDynFunctionData;           
   ///  Array of UserFunctionData pointers.  Contains albegraic path
   ///  functions evaluated at path points.
   std::vector<FunctionOutputData*>     userAlgFunctionData;
   /// Array of UserFunctionData pointers.  Contains user cost
   ///  integrand evaluated at path points.
   std::vector<FunctionOutputData*>     costIntFunctionData;
   /// Properties of dynamic functions
   UserFunctionProperties               *dynFunctionProps;
   /// Properties of cost functions
   UserFunctionProperties               *costFunctionProps;
   /// Properties of algebraic functions
   UserFunctionProperties               *algFunctionProps;
   /// Helper class for evaluating user path functions
   /// Set in constructor
   UserPathFunctionManager              *pathFunctionManager;
   /// Input data for user path functions
   FunctionInputData                    *pathFunctionInputData;
   /// GuessGenerator.  Helper class for computing the intitial guess
   GuessGenerator                       *guessGen;
   /// NLPFuncUtil_AlgPath object.  Helper for NLP functions/Jacobian
   /// @TODO - should this be a pointer to a parent class -
   /// NLPFuncUtil_Path (or above, if we need Point func utils at some point?)
   NLPFuncUtil_AlgPath                  *algPathNLPFuncUtil;
   /// Collocation helper class
   /// @TODO - We should have a pointer to a NLPFuncUtil_Coll object here;
   /// then, in RadauPhase, e.g.,we should check to make sure the type is
   /// correct (or actually create the correct type there)
   NLPFuncUtil_Coll                     *transUtil;

   ArrayTrajectoryData                  *guessArrayData;

   ScalingUtility                       *scaleUtil;

   ///  Relative error tolerance.  Pass to transUtil Object, Not used by phase
   Real relativeErrorTol;


   void     InitializePathFunctionInputData();
   void     IntializeJacobians();
   
   void     PrepareForMeshRefinement();
   void     InitializeDecisionVector();

   void     CheckFunctions();
   void     ComputeUserFunctions();
   void     ComputePathFunctions();
   void     ComputeSparsityPattern();
   void     SetProblemCharacteristics();
   void     InitializeUserFunctions();
   void     SetConstraintProperties();
   void     SetDecisionVectorBounds();
   void     InitializeTimeVector();
   void     SetConstraintBounds();
   void     SetDefectConstraintBounds();
   void     SetPathConstraintBounds();
   void     SetInitialGuessFromGuessGen();
   void     PreparePathFunction(Integer meshIdx,Integer stageIdx,
                                Integer pointType, Integer pointIdx);
   
   void     InsertJacobianRowChunk(const RSMatrix &jacChunk,
                                   const IntegerArray &idxs);

   void     CopyArrays(const Phase &copy);

   void     SetInitialGuess();

   void     UpdateStaticVector();
   /// abstract methods
   virtual void     InitializeTranscription()     = 0;

   
   

   /// mesh refinement related; mod by YK 2017.08.14
   Rmatrix newStateGuess;
   Rmatrix newControlGuess;
   Rvector maxRelErrorVec;
};

#endif // Phase_hpp
