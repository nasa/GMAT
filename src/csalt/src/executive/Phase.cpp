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
// Created: 2016.02.29
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include "GmatConstants.hpp"
#include "Phase.hpp"
#include "DecVecTypeBetts.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"

//#define DEBUG_PHASE_INIT
//#define DEBUG_PHASE_PATH_INIT
//#define DEBUG_GUESS
//#define DEBUG_PHASE_SPARSE
//#define DEBUG_PHASE
//#define DEBUG_BOUNDS_VALUES
//#define DEBUG_DESTRUCT
//#define DEBUG_REPORT_DATA

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
Phase::Phase() :
   decVector                   (NULL),
   config                      (NULL),
   initialGuessMode            (""),
   pathFunction                (NULL),
   userGuessClass              (NULL),
   phaseNum                    (-999),
   numAlgPathConNLP            (0),
   costFunctionIntegral        (-999.99),
   defectConStartIdx           (0),
   algPathConStartIdx          (0),
   defectConEndIdx             (0),
   algPathConEndIdx            (0),
   recomputeUserFunctions      (true),
   isInitialized               (false),
   recomputeNLPFunctions       (true),
   isRefining                  (false),
   constraintTimeOffset        (0),
   dynFunctionProps            (NULL),
   costFunctionProps           (NULL),
   algFunctionProps            (NULL),
   guessGen                    (NULL),
   algPathNLPFuncUtil          (NULL),
   transUtil                   (NULL),
   guessArrayData              (NULL),
   scaleUtil                   (NULL),
   relativeErrorTol            (1.0e-5)
{
   // this is specific and should really be set in a child class!!
   decVector             = new DecVecTypeBetts();
   config                = new ProblemCharacteristics();
   pathFunctionManager   = new UserPathFunctionManager();
   pathFunctionInputData = new FunctionInputData();
   guessGen              = new GuessGenerator();
   
   // This was moved from InitializeNLPHelpers to here - a run-time malloc error
   // was generated when it was created in InitializeNLPHelpers
   algPathNLPFuncUtil    = new NLPFuncUtil_AlgPath();
   
   scaleUtil = new ScalingUtility();

   // Set size initially for RSMatrix items (trying to avoid a weird malloc/free
   // error message)
   SparseMatrixUtil::SetSize(nlpConstraintJacobian, 1, 1);
   SparseMatrixUtil::SetSize(nlpCostJacobian, 1, 1);
   SparseMatrixUtil::SetSize(conSparsityPattern, 1, 1);
   SparseMatrixUtil::SetSize(costSparsityPattern, 1, 1);
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
Phase::Phase(const Phase &copy) :
//   decVector                   (NULL),
//   config                      (NULL),
   initialGuessMode            (copy.initialGuessMode),
//   pathFunction                (NULL),
   userGuessClass              (copy.userGuessClass),
   phaseNum                    (copy.phaseNum),
   numAlgPathConNLP            (copy.numAlgPathConNLP),
   costFunctionIntegral        (copy.costFunctionIntegral),
   defectConStartIdx           (copy.defectConStartIdx),
   algPathConStartIdx          (copy.algPathConStartIdx),
   defectConEndIdx             (copy.defectConEndIdx),
   algPathConEndIdx            (copy.algPathConEndIdx),
   recomputeUserFunctions      (copy.recomputeUserFunctions),
   isInitialized               (copy.isInitialized),
   recomputeNLPFunctions       (copy.recomputeNLPFunctions),
   isRefining                  (copy.isRefining),
   constraintTimeOffset        (copy.constraintTimeOffset),
   relativeErrorTol            (copy.relativeErrorTol)
//   dynFunctionProps            (NULL),
//   costFunctionProps           (NULL),
//   algFunctionProps            (NULL),
//   pathFunctionManager         (NULL),  // clone/copy here?
//   pathFunctionInputData       (NULL),  // clone/copy here?
//   guessGen                    (NULL),
//   algPathNLPFuncUtil          (NULL),
//   transUtil                   (NULL)
{
   DecVecTypeBetts   *dv = (DecVecTypeBetts*) copy.decVector;
   if (dv)
      decVector             = new DecVecTypeBetts(*dv);
   if (copy.config)
      config                = new ProblemCharacteristics(*(copy.config));
//   pathFunctionManager   =
//            new UserPathFunctionManager(*(copy.pathFunctionManager)); // ??
//   pathFunctionInputData =
   //         new FunctionInputData(*(copy.pathFunctionInputData));     // ??
   guessGen              = new GuessGenerator(*(copy.guessGen));

   if (pathFunction) delete pathFunction;
   pathFunction   = copy.pathFunction; // or clone?
   
   if (copy.pathFunctionManager) 
   {
      if (pathFunctionManager) delete pathFunctionManager;
      pathFunctionManager   =
                      new UserPathFunctionManager(*(copy.pathFunctionManager));
   }
   
   if (copy.pathFunctionInputData)
   {
      if (pathFunctionInputData) delete pathFunctionInputData;
      pathFunctionInputData =
                          new FunctionInputData(*(copy.pathFunctionInputData));
   }
   
   if (copy.algPathNLPFuncUtil)
   {
      if (algPathNLPFuncUtil) delete algPathNLPFuncUtil;
      algPathNLPFuncUtil = new NLPFuncUtil_AlgPath(*(copy.algPathNLPFuncUtil));
   }
   
   /* moved to subclass
   if (copy.transUtil)
   {
      if (transUtil) delete transUtil;
      transUtil = new NLPFuncUtil_Coll(*(copy.transUtil));
   }
   */

   if (copy.guessArrayData)
   {
      if (guessArrayData) delete guessArrayData;
      guessArrayData = new ArrayTrajectoryData(*(copy.guessArrayData));
   }

   if (copy.scaleUtil)
   {
      if (scaleUtil) delete scaleUtil;
      scaleUtil = new ScalingUtility(*(copy.scaleUtil));
   }

   if (dynFunctionProps) delete dynFunctionProps;
   dynFunctionProps = new UserFunctionProperties(*(copy.dynFunctionProps));

   if (costFunctionProps) delete costFunctionProps;
   costFunctionProps = new UserFunctionProperties(*(copy.costFunctionProps));

   if (algFunctionProps) delete algFunctionProps;
   algFunctionProps = new UserFunctionProperties(*(copy.algFunctionProps));

   MessageInterface::ShowMessage(
         "COPY: decVector <%p>, config <%p>, pathFM <%p>, pFID <%p>, gg <%p>\n",
         decVector, config, pathFunctionManager,
         pathFunctionInputData, guessGen);

   CopyArrays(copy);
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
Phase& Phase::operator=(const Phase &copy)
{
   if (&copy == this)
      return *this;
   
   initialGuessMode            = copy.initialGuessMode;
   pathFunction                = copy.pathFunction;
   userGuessClass              = copy.userGuessClass;
   phaseNum                    = copy.phaseNum;
   numAlgPathConNLP            = copy.numAlgPathConNLP;
   costFunctionIntegral        = copy.costFunctionIntegral;
   defectConStartIdx           = copy.defectConStartIdx;
   algPathConStartIdx          = copy.algPathConStartIdx;
   defectConEndIdx             = copy.defectConEndIdx;
   algPathConEndIdx            = copy.algPathConEndIdx;
   recomputeUserFunctions      = copy.recomputeUserFunctions;
   isInitialized               = copy.isInitialized;
   recomputeNLPFunctions       = copy.recomputeNLPFunctions;
   isRefining                  = copy.isRefining;
   constraintTimeOffset        = copy.constraintTimeOffset;
   relativeErrorTol            = copy.relativeErrorTol;
   
   if (pathFunction) delete pathFunction;
   pathFunction   = copy.pathFunction;
   

//   if (decVector) delete decVector;
   DecVecTypeBetts *dv = (DecVecTypeBetts*) copy.decVector;
   decVector           = new DecVecTypeBetts(*dv);
   
   if (config) delete config;
   ProblemCharacteristics *pc = copy.config;
   config                     = new ProblemCharacteristics(*pc);

   if (pathFunctionManager) delete pathFunctionManager;
   pathFunctionManager   = new UserPathFunctionManager(
                               *(copy.pathFunctionManager));
   
   if (pathFunctionInputData) delete pathFunctionInputData;
   pathFunctionInputData = new FunctionInputData(*(copy.pathFunctionInputData));
   
   if (guessGen) delete guessGen;
   guessGen = new GuessGenerator(*(copy.guessGen));
   
   if (algPathNLPFuncUtil) delete algPathNLPFuncUtil;
   algPathNLPFuncUtil = new NLPFuncUtil_AlgPath(*(copy.algPathNLPFuncUtil));
   
   /* moved to subclass
   if (transUtil) delete transUtil;
   transUtil = new NLPFuncUtilRadau(*(copy.transUtil));
   */

   if (guessArrayData) delete guessArrayData;
   guessArrayData = new ArrayTrajectoryData(*(copy.guessArrayData));

   if (scaleUtil) delete scaleUtil;
   scaleUtil = new ScalingUtility(*(copy.scaleUtil));

   if (dynFunctionProps) delete dynFunctionProps;
   dynFunctionProps = new UserFunctionProperties(*(copy.dynFunctionProps));

   if (costFunctionProps) delete costFunctionProps;
   costFunctionProps = new UserFunctionProperties(*(copy.costFunctionProps));

   if (algFunctionProps) delete algFunctionProps;
   algFunctionProps = new UserFunctionProperties(*(copy.algFunctionProps));

   MessageInterface::ShowMessage(
         "OP=: decVector <%p>, config <%p>, pathFM <%p>, pFID <%p>, gg <%p>\n",
         decVector, config, pathFunctionManager,
         pathFunctionInputData, guessGen);

   CopyArrays(copy);
   // @todo - handle other pointers here - need to clone?

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
Phase::~Phase()
{
   #ifdef DEBUG_DESTRUCT
      MessageInterface::ShowMessage("DESTRUCTING CSALT Phase\n");
   #endif
   if (decVector)             delete decVector; // if using one created here!!!
   if (config)                delete config;
   if (pathFunctionManager)   delete pathFunctionManager;
   if (pathFunctionInputData) delete pathFunctionInputData;
   if (guessGen)              delete guessGen;
   if (algPathNLPFuncUtil)    delete algPathNLPFuncUtil;
   if (transUtil)             delete transUtil; // or in leaf class????
   if (scaleUtil)             delete scaleUtil;
   if (guessArrayData)        delete guessArrayData;
   if (dynFunctionProps)      delete dynFunctionProps;
   if (costFunctionProps)     delete costFunctionProps;
   if (algFunctionProps)      delete algFunctionProps;

   for (Integer i = 0; i < funcData.size(); ++i)
   {
      if (funcData.at(i))
         delete funcData.at(i);
}
   /*
   for (Integer i = 0; i < userDynFunctionData.size(); ++i)
   {
      if (userDynFunctionData.at(i))
         delete userDynFunctionData.at(i);
   }
   for (Integer i = 0; i < userAlgFunctionData.size(); ++i)
   {
      if (userAlgFunctionData.at(i))
         delete userAlgFunctionData.at(i);
   }
   for (Integer i = 0; i < costIntFunctionData.size(); ++i)
   {
      if (costIntFunctionData.at(i))
         delete costIntFunctionData.at(i);
   }
   */
   funcData.clear();
   userDynFunctionData.clear();
   userAlgFunctionData.clear();
   costIntFunctionData.clear();

   // delete anything else that is created in this class here
   // BUT NOT anything managed somewhere else (e.g. dynFunctionProps, etc.)
   #ifdef DEBUG_DESTRUCT
      MessageInterface::ShowMessage("Done DESTRUCTING CSALT Phase\n");
   #endif
}

//------------------------------------------------------------------------------
//  void RefineMesh(bool         ifUpdateMesh,
//                  bool         &isMeshRefined,
//                  IntegerArray &newMeshIntervalNumPoints,
//                  Rvector      &newMeshIntervalFractions,
//                  Rvector      &maxRelErrorVec,
//                  Rmatrix      &newStateArray,
//                  Rmatrix      &newControlArray)
//------------------------------------------------------------------------------
/**
* This method performs the mesh refnement
*
* @param <ifUpdateMesh>             flag to update the mesh or not
* @param <isMeshRefined>            output - has the mesh refinement been done
* @param <newMeshIntervalNumPoints> output - number of mesh interval mesh pts
* @param <newMeshIntervalFractions> output - mesh interval fractions
* @param <newStateArray>            output - state 
* @param <newControlArray>          output - control array
* @param <maxRelErrorVec>           output - maximum relative error per the mesh
*
*/
//------------------------------------------------------------------------------
void  Phase::RefineMesh(bool         ifUpdateMesh,
                        bool         &isCurrentMeshRefined)
{

   IntegerArray newMeshIntervalNumPoints;
   Rvector      newMeshIntervalFractions;

   isRefining = true;
   // Computes new mesh intervals and orders, state and control guesses

   transUtil->RefineMesh((DecVecTypeBetts*)decVector, pathFunctionManager,
                        &maxRelErrorVec,
                        isCurrentMeshRefined, newMeshIntervalNumPoints,
                        newMeshIntervalFractions,
                        maxRelErrorVec, newStateGuess, newControlGuess);
   if (isCurrentMeshRefined && ifUpdateMesh)
   {      
      SetMeshIntervalFractions(newMeshIntervalFractions);
      SetMeshIntervalNumPoints(newMeshIntervalNumPoints);
      recomputeUserFunctions = true;
      recomputeNLPFunctions = true;
   }
}

//------------------------------------------------------------------------------
// Rvector ComputeMaxMeshError()
//------------------------------------------------------------------------------
/**
 * Compute the max mesh interval errors for the current stored mesh
 *
 * @return Vector of max mesh errors
 */
//------------------------------------------------------------------------------
Rvector Phase::ComputeMaxMeshError()
{
   return transUtil->ComputeMaxMeshError((DecVecTypeBetts*)decVector,
      pathFunctionManager);
}

//------------------------------------------------------------------------------
// void SetInitialGuessMode(const std::string &toMode)
//------------------------------------------------------------------------------
/**
 * Set the initial guess mode
 *
 * @param <toMode>    input initial guess mode
 */
//------------------------------------------------------------------------------
void Phase::SetInitialGuessMode(const std::string &toMode)
{
   initialGuessMode = toMode;
   // @TODO add validation here
}

//------------------------------------------------------------------------------
// void SetUserGuessClass(TrajectoryData *inputClass)
//------------------------------------------------------------------------------
/**
 * Set the guess function class
 *
 * @param <inputClass>    A pointer to the user's class which derives 
 *                        from TrajectoryData
 */
//------------------------------------------------------------------------------
void Phase::SetUserGuessClass(TrajectoryData *inputClass)
{
   userGuessClass = inputClass;
}

//------------------------------------------------------------------------------
// void SetGuessFileName(const std::string &toName)
//------------------------------------------------------------------------------
/**
 * Set the guess file name
 *
 * @param <toMode>    input initial guess file name
 */
//------------------------------------------------------------------------------
void Phase::SetGuessFileName(const std::string &toName)
{
   guessFileName = toName;
}

//------------------------------------------------------------------------------
// void SetInitialGuessArrays(Rvector timeArray,Rmatrix stateArray,
//                            Rmatrix controlArray)
//------------------------------------------------------------------------------
/**
 * Set the guess file name
 *
 * @param <toMode>    input initial guess file name
 */
//------------------------------------------------------------------------------
void Phase::SetInitialGuessArrays(Rvector timeArray,Rmatrix stateArray,
                                  Rmatrix controlArray)
{
    guessArrayData = new ArrayTrajectoryData();
    TrajectoryDataStructure localData;

    Integer numStateTimes,numStateParams,numControlTimes,numControlParams;
    Integer numTimes;

    numTimes = timeArray.GetSize();

    guessArrayData->SetNumSegments(1);

    // Get the number of control parameters and size the 
    // array data object and the local structure
    controlArray.GetSize(numControlTimes,numControlParams);
    guessArrayData->SetNumControlParams(0,numControlParams);
    localData.controls.SetSize(numControlParams);

    // Get the number of state parameters and size the 
    // och data object and the local structure
    stateArray.GetSize(numStateTimes,numStateParams);
    guessArrayData->SetNumStateParams(0,numStateParams);
    localData.states.SetSize(numStateParams);

    localData.integrals.SetSize(0);

    // Loop through each time value
    for (Integer idx = 0; idx < numTimes; idx++)
    {
        // Set the time value
        localData.time = timeArray(idx);

        // Make sure that we have a control value at this time
        if (idx < numControlTimes)
            // Set all of the control values
            for (Integer jdx = 0; jdx < numControlParams; jdx++)
                localData.controls(jdx) = controlArray(idx,jdx);
        else
            // There are no control values here, so set them to be zero
            for (Integer jdx = 0; jdx < numControlParams; jdx++)
                localData.controls(jdx) = 0.0;

        // Make sure that we have a state value at this time
        if (idx < numStateTimes)
            // Set all of the state values
            for (Integer jdx = 0; jdx < numStateParams; jdx++)
                localData.states(jdx) = stateArray(idx,jdx);
        else
            // There are no state values here, so set them to be zero
            for (Integer jdx = 0; jdx < numStateParams; jdx++)
                localData.states(jdx) = 0.0;
        
        // Add this data point to the OCH Data object
       try
       {
          guessArrayData->AddDataPoint(0,localData);
       }
       catch (LowThrustException &lt)
       {
          // Assume exception thrown is for non-monotonic times
          throw LowThrustException("ERROR setting initial guess array: "
                                   "data points are not in the correct "
                                   "temporal order\n");
       }
    }
}


//------------------------------------------------------------------------------
// void SetInitialGuessArrays(Rvector timeArray,Rmatrix stateArray,
//                            Rmatrix controlArray, Rvector staticParams)
//------------------------------------------------------------------------------
/**
* Set the initial guess; YK mod static params
* TBD: explain paramaters 
*/
//------------------------------------------------------------------------------
void Phase::SetInitialGuessArrays(Rvector timeArray, Rmatrix stateArray,
                                  Rmatrix controlArray, Rvector staticParams)
{
   SetInitialGuessArrays(timeArray, stateArray, controlArray);
   // TBD: modify guessArrayData to handle static params.
}

//------------------------------------------------------------------------------
// void SetPathFunction(UserPathFunction* f)
//------------------------------------------------------------------------------
/**
 * Set the path function
 *
 * @param <toMode>    input path function
 */
//------------------------------------------------------------------------------
void Phase::SetPathFunction(UserPathFunction* f)
{
   #ifdef DEBUG_PHASE_INIT
      std::cout << "Entering SetPathFunction ...\n";
   #endif
   pathFunction = f;
   #ifdef DEBUG_PHASE_INIT
      std::cout << "Exiting SetPathFunction ...\n";
   #endif
}

//------------------------------------------------------------------------------
// void PrepareToOptimize()
//------------------------------------------------------------------------------
/**
 * Prepares to optimize
 *
 */
//------------------------------------------------------------------------------
void Phase::PrepareToOptimize()
{
   #ifdef DEBUG_PHASE_SPARSE
         MessageInterface::ShowMessage(
                                 "Entering Phase::PrepareToOptimize ...\n");
   #endif
   // Init to be done after all phases are initialized but BEFORE optimization
   ComputeSparsityPattern();
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the Phase
 *
 */
//------------------------------------------------------------------------------
void Phase::Initialize()
{
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("Entering Phase::Initialize ...\n");
      MessageInterface::ShowMessage("    isRefining = %s\n",
                                    (isRefining? "true" : "false"));
   #endif
   // This is required for re-initialization during mesh refinement
   isInitialized          = false;
   recomputeUserFunctions = true;
   
   // Check the user's configuration before proceeding
   config->ValidateMeshConfig();
   config->ValidateStateProperties();

   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage(
                           "   config validated mesh and properties ...\n");
      std::cout << "   config validated mesh and properties ...\n";
   #endif
   
   // save the current unscaled mesh points vector, state, control; YK
   if (isRefining)
      PrepareForMeshRefinement();
   
   // Initialize transcription and other helper classes; 
   InitializeTranscription();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("   InitializeTranscription done ...\n");
      std::cout << "   InitializeTranscription done ...\n";
   #endif
   InitializeDecisionVector();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("   InitializeDecisionVector done ...\n");
      std::cout << "   InitializeDecisionVector done ...\n";
   #endif
   InitializeTimeVector();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("   InitializeTimeVector done ...\n");
      std::cout << "   InitializeTimeVector done ...\n";
   #endif
   InitializeUserFunctions();
   
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("   InitializeUserFunctions done ...\n");
      std::cout << "   InitializeUserFunctions done ...\n";
   #endif
   SetProblemCharacteristics();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("   SetProblemCharacteristics done ...\n");
      std::cout << "   SetProblemCharacteristics done ...\n";
   #endif
   
   SetConstraintProperties();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("   SetConstraintProperties done ...\n");
      std::cout << "   SetConstraintProperties done ...\n";
   #endif

   SetInitialGuess();
   #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage(
               "   stuff has been initialized ... now will set bounds ...\n");
         std::cout << "   stuff has been initialized ... now will set bounds ...\n";
   #endif

   // Dimension and initialize bounds and other quantities
   SetConstraintBounds();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("   SetConstraintBounds done ...\n");
      std::cout << "   SetConstraintBounds done ...\n";
   #endif
   SetDecisionVectorBounds();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("   SetDecisionVectorBounds done ...\n");
      std::cout << "   SetDecisionVectorBounds done ...\n";
   #endif
   IntializeJacobians();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("   IntializeJacobians done ...\n");
      std::cout << "   IntializeJacobians done ...\n";
   #endif
   InitializePathFunctionInputData();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage(
                           "   InitializePathFunctionInputData done ...\n");
      std::cout << "   InitializePathFunctionInputData done ...\n";
   #endif
   InitializeNLPHelpers();

   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("   Initialization complete ...\n");
      std::cout << "   Initialization complete ...\n";
   #endif
   
   if (!transUtil)
   {
      throw LowThrustException("ERROR setting relative error tolerance on Phase!\n");
   }
   else
   {
      transUtil->SetRelativeErrorTol(relativeErrorTol);
   }

   // We're done intializing!
   isInitialized = true;
}

//------------------------------------------------------------------------------
//  bool SetDecisionVector(const Rvector &decVec)
//------------------------------------------------------------------------------
/**
* This method sets the decision vector
*
* @param <decVec>  input decision vector
*
* @return true if successful
*
*/
//------------------------------------------------------------------------------
bool Phase::SetDecisionVector(const Rvector &newDecVec)
{
   Integer numVarsNLP = config->GetNumDecisionVarsNLP();
   if (newDecVec.GetSize() == numVarsNLP)
   {
      if (!decVector)
         throw LowThrustException("ERROR setting decision vector on Phase!\n");

      Rvector oldDecVec = decVector->GetDecisionVector();

      bool isNew = false;
      for (Integer idx = 0; idx < oldDecVec.GetSize(); idx++)
      {
         if (oldDecVec(idx) != newDecVec(idx))
         {
            isNew = true;
            break;
         }
      }

      if (!decVector->SetDecisionVector(newDecVec))
      {
         std::stringstream errmsg("");
         errmsg << "Phase::SetDecisionVector: Error setting ";
         errmsg << "decision vector on DecisionVector object.\n";
         throw LowThrustException(errmsg.str());
      }

      // Update the time data on the Coll util helper
      Rvector timeVector = decVector->GetTimeVector();
      transUtil->SetTimeVector(timeVector(0), timeVector(1));
      SetTimeInitialGuess(timeVector(0));
      SetTimeFinalGuess(timeVector(1));

      if (isNew == true)
      {
         recomputeUserFunctions = true;
         recomputeNLPFunctions = true;
         // Reset cost Jacobian to zeros so that summing elements
         // does not sum across iterations
         SparseMatrixUtil::SetSize(nlpCostJacobian, 1, numVarsNLP);
      }
      else
      {
         // do nothing. No reason to change flags
         /*
         recomputeUserFunctions = false;
         recomputeNLPFunctions = false;
         recomputeAlgPathConstraints = false;
         recomputeAlgCostFunction = false;
         */
         
      }


      return true;
   }
   else
   {
      std::stringstream errmsg("");
      errmsg << "Phase::SetDecisionVector: Length of decision vector must be ";
      errmsg << "equal to totalnumDecisionVarsNLP\n";
      throw LowThrustException(errmsg.str());
   }
};

//------------------------------------------------------------------------------
// DecisionVector* GetDecisionVector()
//------------------------------------------------------------------------------
/**
 * Returns the decision vector object of the Phase
 *
 * @return  the decision vector object
 *
 */
//------------------------------------------------------------------------------
DecisionVector* Phase::GetDecisionVector()
{
   return decVector;
}


//------------------------------------------------------------------------------
// Rvector GetDecVector()
//------------------------------------------------------------------------------
/**
 * Returns the decision vector
 *
 * @return  the decision vector
 *
 */
//------------------------------------------------------------------------------
Rvector Phase::GetDecVector()
{
   if (!decVector)
      throw LowThrustException("ERROR getting decision vector from Phase!\n");
   
   return decVector->GetDecisionVector();
}

//------------------------------------------------------------------------------
// void SetNumStateVars(Integer toNum)
//------------------------------------------------------------------------------
/**
 * Sets the number of state variables
 *
 */
//------------------------------------------------------------------------------
void Phase::SetNumStateVars(Integer toNum)
{
   if (!config)
      throw LowThrustException("ERROR setting num state vars on Phase!\n");
   config->SetNumStateVars(toNum);
}

//------------------------------------------------------------------------------
// void SetNumControlVars(Integer toNum)
//------------------------------------------------------------------------------
/**
 * Sets the number of control variables
 *
 */
//------------------------------------------------------------------------------
void Phase::SetNumControlVars(Integer toNum)
{
   if (!config)
      throw LowThrustException("ERROR setting num control vars on Phase!\n");
   config->SetNumControlVars(toNum);
}


//------------------------------------------------------------------------------
// void SetRelativeErrorTol(Real toNum)
//------------------------------------------------------------------------------
/**
 * Sets the maximum realtive error tolerance
 *
 */
//------------------------------------------------------------------------------
void Phase::SetRelativeErrorTol(Real toNum)
{
   relativeErrorTol = toNum;
}

//------------------------------------------------------------------------------
// Integer GetNumStateVars()
//------------------------------------------------------------------------------
/**
 * Returns the number of state variables
 *
 * @return the number of state variables
 */
//------------------------------------------------------------------------------
Integer Phase::GetNumStateVars()
{
   if (!config)
      throw LowThrustException("ERROR getting num state vars from Phase!\n");
   return config->GetNumStateVars();
}

//------------------------------------------------------------------------------
// Integer GetNumControlVars()
//------------------------------------------------------------------------------
/**
 * Returns the number of control variables
 *
 * @return the number of control variables
 */
//------------------------------------------------------------------------------
Integer Phase::GetNumControlVars()
{
   if (!config)
      throw LowThrustException("ERROR getting num control vars from Phase!\n");
   return config->GetNumControlVars();
}

//------------------------------------------------------------------------------
// Integer GetDefectConStartIdx()
//------------------------------------------------------------------------------
/**
 * Returns the start index of the defect constraint
 *
 * @return the start index of the defect constraint
 */
//------------------------------------------------------------------------------
Integer Phase::GetDefectConStartIdx()
{
   return defectConStartIdx;
}

//------------------------------------------------------------------------------
// Integer GetDefectConEndIdx()
//------------------------------------------------------------------------------
/**
 * Returns the end index of the defect constraint
 *
 * @return the end index of the defect constraint
 */
//------------------------------------------------------------------------------
Integer Phase::GetDefectConEndIdx()
{
   return defectConEndIdx;
}

//------------------------------------------------------------------------------
// void SetPhaseNumber(Integer toNum)
//------------------------------------------------------------------------------
/**
 * Sets the phase number for this phase
 */
//------------------------------------------------------------------------------
void Phase::SetPhaseNumber(Integer toNum)
{
   phaseNum = toNum;
}

//------------------------------------------------------------------------------
// Integer GetPhaseNumber()
//------------------------------------------------------------------------------
/**
 * Returns the phase number for this phase
 * 
 * @return the phase number
 */
//------------------------------------------------------------------------------
Integer Phase::GetPhaseNumber()
{
   return phaseNum;
}

//------------------------------------------------------------------------------
// Rvector GetAllConLowerBound()
//------------------------------------------------------------------------------
/**
 * Returns the lower bound vector for all constraints
 *
 * @return the lower bounds for all constraints
 */
//------------------------------------------------------------------------------
Rvector Phase::GetAllConLowerBound()
{
   return allConLowerBound;
}

//------------------------------------------------------------------------------
// Rvector GetAllConUpperBound()
//------------------------------------------------------------------------------
/**
 * Returns the upper bound vector for all constraints
 *
 * @return the upper bounds for all constraints
 */
//------------------------------------------------------------------------------
Rvector Phase::GetAllConUpperBound()
{
   return allConUpperBound;
}


//------------------------------------------------------------------------------
// Rvector GetDecVecLowerBound()
//------------------------------------------------------------------------------
/**
 * Returns the lower bound vector for the decision vector
 *
 * @return the lower bounds for the decision vector
 */
//------------------------------------------------------------------------------
Rvector Phase::GetDecVecLowerBound()
{
   return decisionVecLowerBound;
}

//------------------------------------------------------------------------------
// Rvector GetDecVecUpperBound()
//------------------------------------------------------------------------------
/**
 * Returns the upper bound vector for the decision vector
 *
 * @return the upper bounds for the decision vector
 */
//------------------------------------------------------------------------------
Rvector Phase::GetDecVecUpperBound()
{
   return decisionVecUpperBound;
}

//------------------------------------------------------------------------------
// void SetMeshIntervalFractions(const Rvector &fractions)
//------------------------------------------------------------------------------
/**
 * Sets the mesh interval fractions
 *
 * @param <fractions> the mesh interval fractions
 */
//------------------------------------------------------------------------------
void Phase::SetMeshIntervalFractions(const Rvector &fractions)
{
   config->SetMeshIntervalFractions(fractions);
}

//------------------------------------------------------------------------------
// Rvector GetMeshIntervalFractions()
//------------------------------------------------------------------------------
/**
 * Returns the mesh interval fractions
 *
 * @return the mesh interval fractions
 */
//------------------------------------------------------------------------------
Rvector Phase::GetMeshIntervalFractions()
{
   return config->GetMeshIntervalFractions();
}

//------------------------------------------------------------------------------
// void SetMeshIntervalNumPoints(const IntegerArray &toNum)
//------------------------------------------------------------------------------
/**
 * Sets the mesh interval number of points
 *
 * @param <toNum> the mesh interval number of points
 */
//------------------------------------------------------------------------------
void Phase::SetMeshIntervalNumPoints(const IntegerArray &toNum)
{
   config->SetMeshIntervalNumPoints(toNum);
}

//------------------------------------------------------------------------------
// IntegerArray GetMeshIntervalNumPoints()
//------------------------------------------------------------------------------
/**
 * Returns the mesh interval number of points
 *
 * @return the mesh interval number of points
 */
//------------------------------------------------------------------------------
IntegerArray Phase::GetMeshIntervalNumPoints()
{
   return config->GetMeshIntervalNumPoints();
}

//------------------------------------------------------------------------------
// void SetStateLowerBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * Sets the state lower bound vector
 *
 * @param <bound> the state lower bound vector
 */
//------------------------------------------------------------------------------
void Phase::SetStateLowerBound(const Rvector &bound)
{
   #ifdef DEBUG_BOUNDS_VALUES
      std::stringstream debugMsg;
      debugMsg << "In Phase::SetStateLowerBound, bound = ";
      for (Integer ii = 0; ii < bound.GetSize(); ii++)
      {
         debugMsg << bound[ii] << "   ";
      }
      std::cout << debugMsg.str().c_str() << std::endl;
   #endif
   config->SetStateLowerBound(bound);
}

//------------------------------------------------------------------------------
// Rvector GetStateLowerBound()
//------------------------------------------------------------------------------
/**
 * Returns the state lower bound vector
 *
 * @return the state lower bound vector
 */
//------------------------------------------------------------------------------
Rvector Phase::GetStateLowerBound()
{
   return config->GetStateLowerBound();
}

//------------------------------------------------------------------------------
// void SetStateUpperBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * Sets the state upper bound vector
 *
 * @param <bound> the state upper bound vector
 */
//------------------------------------------------------------------------------
void Phase::SetStateUpperBound(const Rvector &bound)
{
   #ifdef DEBUG_BOUNDS_VALUES
      std::stringstream debugMsg;
      debugMsg << "In Phase::SetStateUpperBound, bound = ";
      for (Integer ii = 0; ii < bound.GetSize(); ii++)
      {
         debugMsg << bound[ii] << "   ";
      }
      std::cout << debugMsg.str().c_str() << std::endl;
   #endif
   config->SetStateUpperBound(bound);
}

//------------------------------------------------------------------------------
// Rvector GetStateUpperBound()
//------------------------------------------------------------------------------
/**
 * Returns the state upper bound vector
 *
 * @return the state upper bound vector
 */
//------------------------------------------------------------------------------
Rvector Phase::GetStateUpperBound()
{
   return config->GetStateUpperBound();
}

//------------------------------------------------------------------------------
// void SetStateInitialGuess(const Rvector &guess)
//------------------------------------------------------------------------------
/**
 * Sets the state initial guess
 *
 * @param <guess> the state initial guess
 */
//------------------------------------------------------------------------------
void Phase::SetStateInitialGuess(const Rvector &guess)
{
   #ifdef DEBUG_BOUNDS_VALUES
      std::stringstream debugMsg;
      debugMsg << "In Phase::SetStateInitialGuess, guess = ";
      for (Integer ii = 0; ii < guess.GetSize(); ii++)
      {
         debugMsg << guess[ii] << "   ";
      }
      std::cout << debugMsg.str().c_str() << std::endl;
   #endif
   config->SetStateInitialGuess(guess);
}

//------------------------------------------------------------------------------
// Rvector GetStateInitialGuess()
//------------------------------------------------------------------------------
/**
 * Returns the state initial guess
 *
 * @return the state initial guess
 */
//------------------------------------------------------------------------------
Rvector Phase::GetStateInitialGuess()
{
   return config->GetStateInitialGuess();
}

//------------------------------------------------------------------------------
// void SetStateFinalGuess(const Rvector &guess)
//------------------------------------------------------------------------------
/**
 * Sets the state final guess
 *
 * @param <guess> the state final guess
 */
//------------------------------------------------------------------------------
void Phase::SetStateFinalGuess(const Rvector &guess)
{
   config->SetStateFinalGuess(guess);
}

//------------------------------------------------------------------------------
// Rvector GetStateFinalGuess()
//------------------------------------------------------------------------------
/**
 * Returns the state final guess
 *
 * @return the state final guess
 */
//------------------------------------------------------------------------------
Rvector Phase::GetStateFinalGuess()
{
   return config->GetStateFinalGuess();
}

//------------------------------------------------------------------------------
// void SetTimeLowerBound(Real bound)
//------------------------------------------------------------------------------
/**
 * Sets the lower bound for the time
 *
 * @param <bound> the time lower bound
 */
//------------------------------------------------------------------------------
void Phase::SetTimeLowerBound(Real bound)
{
   config->SetTimeLowerBound(bound);
}

//------------------------------------------------------------------------------
// Real GetTimeLowerBound()
//------------------------------------------------------------------------------
/**
 * Returns the lower bound for the time
 *
 * @return the time lower bound
 */
//------------------------------------------------------------------------------
Real Phase::GetTimeLowerBound()
{
   return config->GetTimeLowerBound();
}

//------------------------------------------------------------------------------
// void SetTimeUpperBound(Real bound)
//------------------------------------------------------------------------------
/**
 * Sets the upper bound for the time
 *
 * @param <bound> the time upper bound
 */
//------------------------------------------------------------------------------
void Phase::SetTimeUpperBound(Real bound)
{
   config->SetTimeUpperBound(bound);
}

//------------------------------------------------------------------------------
// Real GetTimeUpperBound()
//------------------------------------------------------------------------------
/**
 * Returns the upper bound for the time
 *
 * @return the time upper bound
 */
//------------------------------------------------------------------------------
Real Phase::GetTimeUpperBound()
{
   return config->GetTimeUpperBound();
}

//------------------------------------------------------------------------------
// void SetTimeInitialGuess(Real guess)
//------------------------------------------------------------------------------
/**
 * Sets the initial guess for the time
 *
 * @param <guess> the time initial guess
 */
//------------------------------------------------------------------------------
void Phase::SetTimeInitialGuess(Real guess)
{
   config->SetTimeInitialGuess(guess);
}

//------------------------------------------------------------------------------
// Real GetTimeInitialGuess()
//------------------------------------------------------------------------------
/**
 * Returns the initial guess for the time
 *
 * @return the time initial guess
 */
//------------------------------------------------------------------------------
Real Phase::GetTimeInitialGuess()
{
   return config->GetTimeInitialGuess();
}

//------------------------------------------------------------------------------
// void SetTimeFinalGuess(Real guess)
//------------------------------------------------------------------------------
/**
 * Sets the final guess for the time
 *
 * @param <guess> the time final guess
 */
//------------------------------------------------------------------------------
void Phase::SetTimeFinalGuess(Real guess)
{
   config->SetTimeFinalGuess(guess);
}

//------------------------------------------------------------------------------
// Real GetTimeFinalGuess()
//------------------------------------------------------------------------------
/**
 * Returns the final guess for the time
 *
 * @return the time final guess
 */
//------------------------------------------------------------------------------
Real Phase::GetTimeFinalGuess()
{
   return config->GetTimeFinalGuess();
}

//------------------------------------------------------------------------------
// void SetControlLowerBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * Sets the lower bound vector for control
 *
 * @param <bound> the control lower bound
 */
//------------------------------------------------------------------------------
void Phase::SetControlLowerBound(const Rvector &bound)
{
   config->SetControlLowerBound(bound);
}

//------------------------------------------------------------------------------
// Rvector GetControlLowerBound()
//------------------------------------------------------------------------------
/**
 * Returns the lower bound vector for control
 *
 * @return  the control lower bound
 */
//------------------------------------------------------------------------------
Rvector Phase::GetControlLowerBound()
{
   return config->GetControlLowerBound();
}

//------------------------------------------------------------------------------
// void SetControlUpperBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * Sets the upper bound vector for control
 *
 * @param <bound> the control upper bound
 */
//------------------------------------------------------------------------------
void Phase::SetControlUpperBound(const Rvector &bound)
{
   config->SetControlUpperBound(bound);
}

//------------------------------------------------------------------------------
// Rvector GetControlUpperBound()
//------------------------------------------------------------------------------
/**
 * Returns the upper bound vector for control
 *
 * @return  the control upper bound
 */
//------------------------------------------------------------------------------
Rvector Phase::GetControlUpperBound()
{
   return config->GetControlUpperBound();
}

//------------------------------------------------------------------------------
// Integer GetNumTotalConNLP()
//------------------------------------------------------------------------------
/**
 * Returns the total number of constraints
 *
 * @return  the total number of constraints
 */
//------------------------------------------------------------------------------
Integer Phase::GetNumTotalConNLP()
{
   return config->GetNumTotalConNLP();
}

//------------------------------------------------------------------------------
// Integer GetNumDecisionVarsNLP()
//------------------------------------------------------------------------------
/**
 * Returns the number of decision variables
 *
 * @return  the number of decision variables
 */
//------------------------------------------------------------------------------
Integer Phase::GetNumDecisionVarsNLP()
{
   return config->GetNumDecisionVarsNLP();
}


//------------------------------------------------------------------------------
// IntegerArray GetNumNLPNonZeros()
//------------------------------------------------------------------------------
/**
 * Returns the array of number of non-zero elements
 *
 * @return  array of number of non-zero elements
 */
//------------------------------------------------------------------------------
IntegerArray Phase::GetNumNLPNonZeros()
{
   // Get number of defect contraints
   IntegerArray numNonZeros;
   Integer numAZerosTot = 0;
   Integer numBZerosTot = 0;
   Integer numQZerosTot = 0;
   if (config->HasAlgPathCons())
   {
      IntegerArray nums = algPathNLPFuncUtil->GetMatrixNumNonZeros();
      numAZerosTot     += nums[0];
      numBZerosTot     += nums[1];
      numQZerosTot     += nums[2];
   }
   if (config->HasDefectCons())
   {
      IntegerArray nums = transUtil->GetDefectMatrixNumNonZeros();
      numAZerosTot     += nums[0];
      numBZerosTot     += nums[1];
      numQZerosTot     += nums[2];
   }
   numNonZeros.push_back(numAZerosTot);
   numNonZeros.push_back(numBZerosTot);
   numNonZeros.push_back(numQZerosTot);
   
   return numNonZeros;
}

//------------------------------------------------------------------------------
// void SetNumStaticVars(Integer toNum)
//------------------------------------------------------------------------------
/**
* Sets the number of state variables; YK mod static params
*
*/
//------------------------------------------------------------------------------
void Phase::SetNumStaticVars(Integer toNum)
{
   if (!config)
      throw LowThrustException("ERROR setting num state vars on Phase!\n");
   config->SetNumStaticVars(toNum);
}

//------------------------------------------------------------------------------
// Integer GetNumStaticVars()
//------------------------------------------------------------------------------
/**
* Returns the number of state variables; YK mod static params
*
* @return the number of state variables
*/
//------------------------------------------------------------------------------
Integer Phase::GetNumStaticVars()
{
   if (!config)
      throw LowThrustException("ERROR getting num state vars from Phase!\n");
   return config->GetNumStaticVars();
}

//------------------------------------------------------------------------------
// void SetStaticLowerBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
* Sets the state lower bound vector; YK mod static params
*
* @param <bound> the state lower bound vector
*/
//------------------------------------------------------------------------------
void Phase::SetStaticLowerBound(const Rvector &bound)
{
#ifdef DEBUG_BOUNDS_VALUES
   std::stringstream debugMsg;
   debugMsg << "In Phase::SetStaticLowerBound, bound = ";
   for (Integer ii = 0; ii < bound.GetSize(); ii++)
   {
      debugMsg << bound[ii] << "   ";
   }
   std::cout << debugMsg.str().c_str() << std::endl;
#endif
   config->SetStaticLowerBound(bound);
}

//------------------------------------------------------------------------------
// Rvector GetStaticLowerBound()
//------------------------------------------------------------------------------
/**
* Returns the state lower bound vector; YK mod static params
*
* @return the state lower bound vector
*/
//------------------------------------------------------------------------------
Rvector Phase::GetStaticLowerBound()
{
   return config->GetStaticLowerBound();
}

//------------------------------------------------------------------------------
// void SetStaticUpperBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
* Sets the state upper bound vector; YK mod static params
*
* @param <bound> the state upper bound vector
*/
//------------------------------------------------------------------------------
void Phase::SetStaticUpperBound(const Rvector &bound)
{
#ifdef DEBUG_BOUNDS_VALUES
   std::stringstream debugMsg;
   debugMsg << "In Phase::SetStaticUpperBound, bound = ";
   for (Integer ii = 0; ii < bound.GetSize(); ii++)
   {
      debugMsg << bound[ii] << "   ";
   }
   std::cout << debugMsg.str().c_str() << std::endl;
#endif
   config->SetStaticUpperBound(bound);
}

//------------------------------------------------------------------------------
// Rvector GetStaticUpperBound()
//------------------------------------------------------------------------------
/**
* Returns the state upper bound vector; YK mod static params
*
* @return the state upper bound vector
*/
//------------------------------------------------------------------------------
Rvector Phase::GetStaticUpperBound()
{
   return config->GetStaticUpperBound();
}

//------------------------------------------------------------------------------
// void SetStaticInitialGuess(const Rvector &guess)
//------------------------------------------------------------------------------
/**
* Sets the static vector initial guess; YK mod static params
*
* @param <guess> the state initial guess
*/
//------------------------------------------------------------------------------
void Phase::SetStaticGuess(const Rvector &guess)
{
#ifdef DEBUG_BOUNDS_VALUES
   std::stringstream debugMsg;
   debugMsg << "In Phase::SetStaticInitialGuess, guess = ";
   for (Integer ii = 0; ii < guess.GetSize(); ii++)
   {
      debugMsg << guess[ii] << "   ";
   }
   std::cout << debugMsg.str().c_str() << std::endl;
#endif
   // YK mod static vars: there are two copies of static vector. 
   /// first, decision vector. second, problem configuration. 
   
   config->SetStaticVector(guess);   
   // now, dec vector is not defined yet. 
   // thus, handle it at InitializeDecisionVector() method
   ///decVector->SetStaticVector(guess);
}

//------------------------------------------------------------------------------
// Rvector GetTimeVector()
//------------------------------------------------------------------------------
/**
 * Returns the time vector
 *
 * @return  the time vector
 */
//------------------------------------------------------------------------------
Rvector Phase::GetTimeVector()
{
   return transUtil->GetTimeVector();
}

//------------------------------------------------------------------------------
// void InitializeNLPHelpers()
//------------------------------------------------------------------------------
/**
 * Initializes the NLP helper classes
 *
 */
//------------------------------------------------------------------------------
void Phase::InitializeNLPHelpers()
{
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("Entering  InitializeNLPHelpers  ...\n");
   #endif
   // Initialization for quadrature type functions
   if (dynFunctionProps)
   {
      delete dynFunctionProps;
      dynFunctionProps = NULL;
   }
   if (pathFunctionManager->HasDynFunctions())
       dynFunctionProps = new UserFunctionProperties(pathFunctionManager->
                              GetDynFunctionProperties());

   if (costFunctionProps)
   {
      delete costFunctionProps;
      costFunctionProps = NULL;
}
   if (pathFunctionManager->HasCostFunction())
       costFunctionProps = new UserFunctionProperties(pathFunctionManager->
                               GetCostFunctionProperties());

   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage(
                                 "About to call ComputeUserFunctions  ...\n");
   #endif
   ComputeUserFunctions();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("About to call PrepareToOptimize  ...\n");
      if (!transUtil)
         MessageInterface::ShowMessage("transUtil is NULL!!!!\n");
      MessageInterface::ShowMessage(" --- dynFunctionProps (%s)  = <%p>\n", 
            (pathFunctionManager->HasDynFunctions()? "true" : "false"),
                                    dynFunctionProps);
      MessageInterface::ShowMessage(" --- costFunctionProps (%s) = <%p>\n", 
            (pathFunctionManager->HasCostFunction()? "true" : "false"),
                                    costFunctionProps);
   #endif
   if (pathFunctionManager->HasCostFunction())  // TEMPORARY!!!
   {
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("-- there IS a cost function  ...\n");
      #endif
      transUtil->PrepareToOptimize(*dynFunctionProps, userDynFunctionData,
                                   *costFunctionProps,costIntFunctionData); 
   }
   else
   {
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("-- there IS NO cost function  ...\n");
      #endif
      transUtil->PrepareToOptimize(*dynFunctionProps, userDynFunctionData);
   }

   transUtil->SetPhaseNum(phaseNum);

   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("AFTER call to PrepareToOptimize  ...\n");
   #endif
   // Initialization for algebraic path functions
   if (pathFunctionManager->HasAlgFunctions())
   {
      // Set up the properties for the algebraic functions
      if (algFunctionProps)
         delete algFunctionProps;
      algFunctionProps = new UserFunctionProperties(pathFunctionManager->
                                                    GetAlgFunctionProperties());
      algFunctionProps->SetHasStateVars(config->HasStateVars());
      algFunctionProps->SetHasControlVars(config->HasControlVars());
      // YK mod static params
      algFunctionProps->SetHasStaticVars(config->HasStaticVars());
      
      // Build array needed by NLP util but computed by Coll util
      Integer numPathConPoints = transUtil->GetNumPathConstraintPoints();
      Rvector dTimedTi(numPathConPoints);
      Rvector dTimedTf(numPathConPoints);
      for (Integer ptIdx = 0; ptIdx < numPathConPoints; ptIdx++)
      {
         // mod by YK for RK collocation implementation; as in MATLAB prototype
         Integer meshIdx = userAlgFunctionData.at(ptIdx)->GetMeshIdx();
         Integer stageIdx = userAlgFunctionData.at(ptIdx)->GetStageIdx();

         dTimedTi(ptIdx)  = transUtil->GetdCurrentTimedTI(meshIdx, stageIdx);
         dTimedTf(ptIdx)  = transUtil->GetdCurrentTimedTF(meshIdx, stageIdx);
      }
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage(
            "In InitializeNLPHelpers, initializing algPathNLPFuncUtil ...\n");
         MessageInterface::ShowMessage(" numPathConPoints =  %d\n",
                                       numPathConPoints);
         Integer r;
         r = dTimedTi.GetSize();
         MessageInterface::ShowMessage(" size of dTimedTi is %d \n", r);
         r = dTimedTf.GetSize();
         MessageInterface::ShowMessage(" size of dTimedTf is %d \n", r);
      #endif
      // Intialize the NLP util
      Integer numConstraintPts = transUtil->GetNumPathConstraintPoints();
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage(" numConstraintPts =  %d\n",
                                       numConstraintPts);
         MessageInterface::ShowMessage("  algFunctionProps   = <%p>\n",
                                       algFunctionProps);
         MessageInterface::ShowMessage("  algPathNLPFuncUtil = <%p>\n",
                                       algPathNLPFuncUtil);
      #endif
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("  NOW algPathNLPFuncUtil created\n"); 
         MessageInterface::ShowMessage("  NOW algPathNLPFuncUtil = <%p>\n",
                                       algPathNLPFuncUtil);
      #endif
      algPathNLPFuncUtil->Initialize(algFunctionProps,userAlgFunctionData,
                                     decVector->GetNumDecisionParams(),
                                     numConstraintPts,dTimedTi,dTimedTf);
   }
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("LEAVING InitializeNLPHelpers  ...\n");
   #endif
}


//------------------------------------------------------------------------------
// Rvector GetConstraintVector()
//------------------------------------------------------------------------------
/**
 * Assembles the total constraint vector for the phase (note: assumes these are
 * in order ???)
 *
 * @return the constraint vector
 *
 */
//------------------------------------------------------------------------------
Rvector Phase::GetConstraintVector()
{
   Rvector result(algPathConEndIdx - defectConStartIdx + 1);
   //  Update functions if they need to be recomputed
   CheckFunctions();

   // Assemble the total constraint vector
   if (pathFunctionManager->HasDynFunctions())
   {
      Integer jj = 0;
      for (Integer ii = defectConStartIdx; ii <= defectConEndIdx; ii++)
         result(ii) = defectConVec(jj++);
   }
   if (pathFunctionManager->HasAlgFunctions())
   {
      Integer jj = 0;
      for (Integer ii = algPathConStartIdx; ii <= algPathConEndIdx; ii++)
         result(ii) = algPathConVec(jj++);
   }
   return result;
}

//------------------------------------------------------------------------------
// StringArray GetConstraintVectorNames()
//------------------------------------------------------------------------------
/**
 * Assembles the total constraint vector descriptions cell array for the phase
 *
 * @return an array of the constraint vector names
 *
 */
//------------------------------------------------------------------------------
StringArray Phase::GetConstraintVectorNames()
{
   StringArray constraintNames;
   // Assemble the total constraint vector
   Integer meshIdx    = 0;
   Integer nStateVars = 0;
   Integer nAlgFuns   = 0;
   if (pathFunctionManager->HasDynFunctions())
   {
      for (Integer ii = defectConStartIdx; ii <= defectConEndIdx; ii++)
      {
         nStateVars++;
         std::stringstream phaseStr("");
         phaseStr << "Phase ";
         phaseStr << phaseNum << ", Mesh Index ";
         phaseStr << meshIdx  << ": State Variable ";
         phaseStr << nStateVars;
         constraintNames.push_back(phaseStr.str());
         if (config->GetNumStateVars() == nStateVars)
         {
            meshIdx++;
            nStateVars = 0;
         }
      }
   }
   if (pathFunctionManager->HasAlgFunctions())
   {
      meshIdx  = 0;
      nAlgFuns = 0;
      for (Integer ii = algPathConStartIdx; ii <= algPathConEndIdx; ii++)
      {
         std::stringstream phaseStr("");
         phaseStr << "Phase ";
         phaseStr << phaseNum << ", Mesh Index " << meshIdx;
         if (algPathConVecNames.empty())
         {
            phaseStr << ": User Path Constraint ";
            phaseStr << nAlgFuns;
         }
         else
         {
            phaseStr << ": ";
            phaseStr << algPathConVecNames.at(nAlgFuns);
         }
         nAlgFuns++;
         constraintNames.push_back(phaseStr.str());
         if (pathFunctionManager->GetNumAlgFunctions() == nAlgFuns)
         {
            meshIdx++;
            nAlgFuns = 0;
         }
      }
   }
   return constraintNames;
}


//------------------------------------------------------------------------------
// Real GetCostFunction()
//------------------------------------------------------------------------------
/**
 * Computes and returns the cost function value
 *
 * @return  the cost function value
 *
 */
//------------------------------------------------------------------------------
Real Phase::GetCostFunction()
{
   #ifdef DEBUG_PHASE
      MessageInterface::ShowMessage("Entering Phase::GetCostFunction ...\n");
   #endif

   CheckFunctions();
   Real costFunction = 0;
   if (pathFunctionManager->HasCostFunction())
      costFunction += costFunctionIntegral;
   #ifdef DEBUG_PHASE
      MessageInterface::ShowMessage("LEAVING Phase::GetCostFunction ...\n");
   #endif
   return costFunction;
}

//------------------------------------------------------------------------------
// void ComputeDefectConstraints()
//------------------------------------------------------------------------------
/**
 * Inserts the defect constraints and Jacobian into the constraint vector
 *
 */
//------------------------------------------------------------------------------
void Phase::ComputeDefectConstraints()
{
   #ifdef DEBUG_PHASE
      MessageInterface::ShowMessage(
                              "Entering Phase::ComputeDefectConstraints ...\n");
      MessageInterface::ShowMessage("   isInitialized = %s\n",
                                    (isInitialized? "true" : "false"));
   #endif

   // If not intialized, cannot compute functions.
   if (!isInitialized)
      return;
   Rvector  fData;
   RSMatrix  jac;
   
   #ifdef DEBUG_PHASE
      MessageInterface::ShowMessage("In Phase::ComputeDefectConstraints, calling ComputeDefectFunAnfJac ...\n");
   #endif
   transUtil->ComputeDefectFunAndJac(userDynFunctionData,
                                     (DecVecTypeBetts*) decVector,
                                     fData, jac);
   // resize defectConVec for MR; YK mod
   defectConVec.SetSize(fData.GetSize());
   defectConVec = fData;
   IntegerArray idxs;
   idxs.push_back(defectConStartIdx);
   idxs.push_back(defectConEndIdx);
   InsertJacobianRowChunk(jac,idxs);  
   #ifdef DEBUG_PHASE
      MessageInterface::ShowMessage(
                              "LEAVING Phase::ComputeDefectConstraints ...\n");
   #endif
}

//------------------------------------------------------------------------------
// void ComputeIntegralCost()
//------------------------------------------------------------------------------
/**
 * Inserts the integral cost  and Jacobian into the appropriatelocations
 *
 */
//------------------------------------------------------------------------------
void Phase::ComputeIntegralCost()
{
   #ifdef DEBUG_PHASE
      MessageInterface::ShowMessage(
                                 "Entering Phase::ComputeIntegralCost ...\n");
   #endif
   // Insert the integral cost  and Jacobian into the appropriatelocations

   // If not intialized, cannot compute functions.
   if (!isInitialized)
      return;

   Rvector  fData;
   RSMatrix  jac;
   
   transUtil->ComputeCostFunAndJac(costIntFunctionData,
                                   (DecVecTypeBetts*) decVector, fData,
                                   nlpCostJacobian);
   #ifdef DEBUG_PHASE
      MessageInterface::ShowMessage(
                                 "--- done calling ComputeCostFunAndJac ...\n");
   #endif
   costFunctionIntegral  = fData[0];
   #ifdef DEBUG_PHASE
      MessageInterface::ShowMessage("LEAVING Phase::ComputeIntegralCost ...\n");
   #endif
}

//------------------------------------------------------------------------------
// void ComputeAlgebraicPathConstraints()
//------------------------------------------------------------------------------
/**
 * Inserts algebraic path constraints and the Jacobian into the constraint 
 * vector
 *
 */
//------------------------------------------------------------------------------
void Phase::ComputeAlgebraicPathConstraints()
{
   #ifdef DEBUG_PHASE_SPARSE
      MessageInterface::ShowMessage(
                     "Phase: ComputeAlgebraicPathConstraints ...\n");
      MessageInterface::ShowMessage(
                     "     isInitialized? %s\n", (isInitialized? "YES" : "no"));
      MessageInterface::ShowMessage(
                     "     algPathConStartIdx = %d\n", algPathConStartIdx);
      MessageInterface::ShowMessage(
                     "     algPathConEndIdx   = %d\n", algPathConEndIdx);
      MessageInterface::ShowMessage("     size of userAlgFunctionData = %d\n", 
                                   (Integer) userAlgFunctionData.size());
   #endif

   // If not intialized, cannot compute functions.
   if (!isInitialized)
      return;

   Rvector   fVal;
   RSMatrix jacValues;
   IntegerArray idxs;
   for (Integer ii = algPathConStartIdx; ii <= algPathConEndIdx; ii++)
      idxs.push_back(ii);
   #ifdef DEBUG_PHASE_SPARSE
         MessageInterface::ShowMessage(
                                    "Phase: calling ComputeFuncAndJac ...\n");
   #endif
   algPathNLPFuncUtil->ComputeFuncAndJac(userAlgFunctionData, fVal, jacValues);
   // resizing for MR; YK mod
   algPathConVec.SetSize(fVal.GetSize());
   algPathConVec = fVal;
   #ifdef DEBUG_PHASE_SPARSE
         MessageInterface::ShowMessage(
                                 "Phase: inserting jacobian row chunk ...\n");
   #endif
   InsertJacobianRowChunk(jacValues,idxs);
   #ifdef DEBUG_PHASE_SPARSE
         MessageInterface::ShowMessage(
                        "LEAVING Phase: ComputeAlgebraicPathConstraints ...\n");
   #endif
}

//------------------------------------------------------------------------------
// void SetRecomputeUserFunctions(bool toFlag)
//------------------------------------------------------------------------------
/**
 * Sets the flag indicating whether or not the user functions need to be 
 * recomputed.  For debugging, this allows you to set user functions to 
 * recompute even when there is no change in decision vector.
 *
 * @param <toFlag>   input flag value
 *
 */
//------------------------------------------------------------------------------
void Phase::SetRecomputeUserFunctions(bool toFlag)
{
   recomputeUserFunctions = toFlag;
}

//------------------------------------------------------------------------------
// RSMatrix GetCostJacobian()
//------------------------------------------------------------------------------
/**
 * Returns the total Jacobian of the cost function by summing
 * Jacobian of algebraic and quadrature terms
 *
 * @return total jacobian of the cost function
 *
 */
//------------------------------------------------------------------------------
RSMatrix Phase::GetCostJacobian()
{
   return nlpCostJacobian;
}

//------------------------------------------------------------------------------
// RSMatrix GetConJacobian()
//------------------------------------------------------------------------------
/**
 * Returns the constraint Jacobian
 *
 * @return the constraint jacobian
 *
 */
//------------------------------------------------------------------------------
RSMatrix Phase::GetConJacobian()
{
   return nlpConstraintJacobian;
}
   
//------------------------------------------------------------------------------
// RSMatrix GetCostSparsityPattern()
//------------------------------------------------------------------------------
/**
 * Returns the cost sparsity pattern
 *
 * @return the cost sparsity pattern
 *
 */
//------------------------------------------------------------------------------
RSMatrix Phase::GetCostSparsityPattern()
{
   return costSparsityPattern;
}
   
//------------------------------------------------------------------------------
// RSMatrix GetConSparsityPattern()
//------------------------------------------------------------------------------
/**
 * Returns the constraint sparsity pattern
 *
 * @return the constraint sparsity pattern
 *
 */
//------------------------------------------------------------------------------
RSMatrix Phase::GetConSparsityPattern()
{
   return conSparsityPattern;
}

//------------------------------------------------------------------------------
// Rvector GetInitialFinalTime()
//------------------------------------------------------------------------------
/**
 * Returns the initial final time
 *
 * @return the initial final time
 *
 */
//------------------------------------------------------------------------------
Rvector Phase::GetInitialFinalTime()
{
   Rvector times(2, decVector->GetFirstTime(), decVector->GetLastTime());
   return times;
}

//------------------------------------------------------------------------------
// void DebugPathFunction()
//------------------------------------------------------------------------------
/**
 * Debugs the path function
 *
 */
//------------------------------------------------------------------------------
void Phase::DebugPathFunction()
{
   ComputePathFunctions();
}
   
//------------------------------------------------------------------------------
// bool HasAlgPathCons()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not there is an algebraic path 
 * constraint
 *
 * @return  flag indicating whether or not there is an algebraic path
 *          constraint
 *
 */
//------------------------------------------------------------------------------
bool Phase::HasAlgPathCons()
{
   return config->HasAlgPathCons();
}

//------------------------------------------------------------------------------
// void ComputeAlgFuncAndJac(Rvector  &funcValues,RSMatrix &jacArray)
//------------------------------------------------------------------------------
/**
 * Calls the NLP utility to compute the function and Jacobian
 *
 * @param <funcValues>  output - computed function values
 * @param <jacArray>    output - computed Jacobian matrix
 *
 *
 */
//------------------------------------------------------------------------------
void Phase::ComputeAlgFuncAndJac(Rvector  &funcValues,
                                 RSMatrix &jacArray)
{
   algPathNLPFuncUtil->ComputeFuncAndJac(userAlgFunctionData, 
                                         funcValues, jacArray);
}

//------------------------------------------------------------------------------
// Rmatrix GetStateArray()
//------------------------------------------------------------------------------
/**
 * Returns the decision vector state matrix
 *
 * @return the decision vector state matrix
 */
//------------------------------------------------------------------------------
Rmatrix Phase::GetStateArray()
{
   if (!decVector)
      throw LowThrustException("ERROR - decision vector is NULL!\n");
   return decVector->GetStateArray();
}

//------------------------------------------------------------------------------
// Rmatrix GetControlArray()
//------------------------------------------------------------------------------
/**
 * Returns the decision vector control matrix
 *
 * @return the decision vector control matrix
 */
//------------------------------------------------------------------------------
Rmatrix Phase::GetControlArray()
{
   if (!decVector)
      throw LowThrustException("ERROR - decision vector is NULL!\n");
   return decVector->GetControlArray();
}

//------------------------------------------------------------------------------
// Rmatrix GetStaticVector()
//------------------------------------------------------------------------------
/**
* Returns the decision vector static matrix
*
* @return the decision vector static matrix
*/
//------------------------------------------------------------------------------
Rvector Phase::GetStaticVector()
{
   if (!decVector)
      throw LowThrustException("ERROR - decision vector is NULL!\n");
   return decVector->GetStaticVector();
}

//------------------------------------------------------------------------------
// void SetStateArray(Rmatrix stateArray)
//------------------------------------------------------------------------------
/**
* Sets the decision vector state matrix
*
* @param <stateArray>  the array of state guesses
*
*/
//------------------------------------------------------------------------------
void Phase::SetStateArray(Rmatrix stateArray)
{
    if (transUtil->GetNumStatePoints() != stateArray.GetNumRows() ||
        GetNumStateVars() != stateArray.GetNumColumns())
        throw LowThrustException("ERROR - State Array dimensions are not valid!\n");
    if (!decVector)
        throw LowThrustException("ERROR - decision vector is NULL!\n");
    recomputeUserFunctions = true;
    recomputeNLPFunctions = true;    
    return decVector->SetStateArray(stateArray);
}

//------------------------------------------------------------------------------
// void SetControlArray(Rmatrix controlArray)
//------------------------------------------------------------------------------
/**
* Sets the decision vector control matrix
*
* @param <controlArray>  the array of control guesses
*/
//------------------------------------------------------------------------------
void Phase::SetControlArray(Rmatrix controlArray)
{
    if (transUtil->GetNumControlPoints() != controlArray.GetNumRows() ||
        GetNumControlVars() != controlArray.GetNumColumns())
        throw LowThrustException("ERROR - State Array dimensions are not valid!\n");
    if (!decVector)
        throw LowThrustException("ERROR - decision vector is NULL!\n");
    recomputeUserFunctions = true;
    recomputeNLPFunctions = true;
    return decVector->SetControlArray(controlArray);
}

//------------------------------------------------------------------------------
// Rvector GetMaxRelativeMeshErrorVector()
//------------------------------------------------------------------------------
/**
* Gets the vector of max relative error values of the mesh intervals
*
* @return The max relative error vector
*/
//------------------------------------------------------------------------------
Rvector Phase::GetMaxRelativeMeshErrorVector()
{
   return maxRelErrorVec;
}

//------------------------------------------------------------------------------
// void UpdateStaticVector()
//------------------------------------------------------------------------------
/**
* Update the static vector stored in prblem characteristic using decision vector
*
*/
//------------------------------------------------------------------------------
void Phase::UpdateStaticVector()
{
   // YK mod static vars: there are two copies of static vector. 
   /// first, decision vector. second, problem configuration.    
   config->SetStaticVector(decVector->GetStaticVector());
   return;
}

//------------------------------------------------------------------------------
// Real GetFirstTime()
//------------------------------------------------------------------------------
/**
 * Returns the decision vector initial time
 *
 * @return the decision vector initial time
 */
//------------------------------------------------------------------------------
Real Phase::GetFirstTime()
{
   if (!decVector)
      throw LowThrustException("ERROR - decision vector is NULL!\n");
   return decVector->GetFirstTime();
}

//------------------------------------------------------------------------------
// Real GetLastTime()
//------------------------------------------------------------------------------
/**
 * Returns the decision vector final time
 *
 * @return the decision vector final time
 */
//------------------------------------------------------------------------------
Real Phase::GetLastTime()
{
   if (!decVector)
      throw LowThrustException("ERROR - decision vector is NULL!\n");
   return decVector->GetLastTime();
}

//------------------------------------------------------------------------------
// ScalingUtility* GetScaleUtility()
//------------------------------------------------------------------------------
/**
 * Returns the pointer to the scale utility object
 *
 * @return the scale utility object
 */
//------------------------------------------------------------------------------
ScalingUtility* Phase::GetScaleUtility()
{
   if (!scaleUtil)
      throw LowThrustException("ERROR - scale utility is NULL!\n");
   return scaleUtil;
}

//------------------------------------------------------------------------------
// void ReportBoundsData(bool addHeader = true)
//------------------------------------------------------------------------------
/**
 * Reports Bounds data to the log. 
 * TBD: review report output changes due to static variables
 *
 * NOTE: the following assumptions are made:
 *       size of lower and upper bounds are the same for state
 *       size of lower and upper bounds are the same for control
 *       size of lower and upper bounds are the same for static
 *       size of any of these vectors does not exceed 99
 *       size of the time vector does not exceed perLine
 *
 */
//------------------------------------------------------------------------------
void Phase::ReportBoundsData(bool addHeader)
{
   Integer perLine = 5;
   
   Real    timeLow       = GetTimeLowerBound();
   Real    timeUp        = GetTimeUpperBound();
   Rvector stateLow      = GetStateLowerBound();
   Rvector controlLow    = GetControlLowerBound();
   Rvector staticLow     = GetStaticLowerBound();
   Rvector stateUp       = GetStateUpperBound();
   Rvector controlUp     = GetControlUpperBound();
   Rvector staticUp      = GetStaticUpperBound();
   
   // Assume size of lower and upper bounds are the same for state
   // Assume size of lower and upper bounds are the same for control
   // Assume size of lower and upper bounds are the same for static params
   // Assume that the size of any of these vectors does not exceed 99
   // Assume that the size of the time vector does not exceed perLine
   Integer szStateLow    = stateLow.GetSize();
   Integer szControlLow  = controlLow.GetSize();
   Integer szStaticLow   = staticLow.GetSize();
   Integer szStateUp     = stateUp.GetSize();
   Integer szControlUp   = controlUp.GetSize();
   Integer szStaticUp    = staticUp.GetSize();
   
   Integer stateIdx   = 0;
   Integer controlIdx = 0;
   Integer staticIdx = 0;
   std::stringstream boundsMsg;
   std::stringstream boundsHeader;
   
   boundsMsg.width(12);
   boundsMsg.fill(' ');
   boundsMsg.setf(std::ios::left);
   boundsMsg.precision(7);
   boundsMsg.setf(std::ios::fixed,std::ios::floatfield);
   
   // Write the header if requested
   // Write the header if requested
   if (addHeader)
      boundsMsg << ReportHeaderData();
   
   // Write the section header and divider
   boundsMsg << "  =============================================== Bounds " <<
   "=================================================" << std::endl;
   boundsMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;

   // Figure out the header based on the sizes of the vectors
   boundsHeader << "                          ";
   
   // Write the header
   Integer count = 0;
   for (Integer ii = 0; ii < szStateLow; ii++)
   {
      boundsHeader << "STATE";
      if (ii < 10)
         boundsHeader << "0";
      boundsHeader << ii << "        ";
      count++;
      if (count >= perLine && ii < (szStateLow-1))
      {
         boundsHeader << std::endl;
         boundsHeader << "                          ";
         count = 0;
      }
   }
   for (Integer ii = 0; ii < szControlLow; ii++)
   {
      boundsHeader << "CNTRL";
      if (ii < 10)
         boundsHeader << "0";
      boundsHeader << ii << "        ";
      count++;
      if (count >= perLine && ii < (szControlLow-1))
      {
         boundsHeader << std::endl;
         boundsHeader << "                          ";
         count = 0;
      }
   }
   // YK mod static params
   for (Integer ii = 0; ii < szStaticLow; ii++)
   {
      boundsHeader << "STATIC";
      if (ii < 10)
         boundsHeader << "0";
      boundsHeader << ii << "        ";
      count++;
      if (count >= perLine && ii < (szStaticLow - 1))
      {
         boundsHeader << std::endl;
         boundsHeader << "                          ";
         count = 0;
      }
   }
      
   boundsMsg << boundsHeader.str() << std::endl;
   boundsMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   
   // Write the state and control data here
   count = 0;
   boundsMsg << "    Lower Bounds         ";
   for (Integer ii = 0; ii < szStateLow; ii++)
   {
      boundsMsg << GmatStringUtil::BuildNumber(stateLow(ii), true, 12) << "   ";
      count++;
      if (count >= perLine && ii < (szStateLow-1))
      {
         boundsMsg << std::endl;
         boundsMsg << "                         ";
         count = 0;
      }
   }
   for (Integer ii = 0; ii < szControlLow; ii++)
   {
      boundsMsg << GmatStringUtil::BuildNumber(controlLow(ii), true, 12) << "   ";
      count++;
      if (count >= perLine && ii < (szControlLow-1))
      {
         boundsMsg << std::endl;
         boundsMsg << "                         ";
         count = 0;
      }
   }
   // YK mod static params
   for (Integer ii = 0; ii < szStaticLow; ii++)
   {
      boundsMsg << GmatStringUtil::BuildNumber(staticLow(ii), true, 12) << "   ";
      count++;
      if (count >= perLine && ii < (szStaticLow - 1))
      {
         boundsMsg << std::endl;
         boundsMsg << "                         ";
         count = 0;
      }
   }
   count = 0;
   boundsMsg << std::endl << "    Upper Bounds         ";
   for (Integer ii = 0; ii < szStateUp; ii++)
   {
      boundsMsg << GmatStringUtil::BuildNumber(stateUp(ii), true, 12) << "   ";
      count++;
      if (count >= perLine && ii < (szStateUp-1))
      {
         boundsMsg << std::endl;
         boundsMsg << "                         ";
         count = 0;
      }
   }
   for (Integer ii = 0; ii < szControlUp; ii++)
   {
      boundsMsg << GmatStringUtil::BuildNumber(controlUp(ii), true, 12) << "   ";
      count++;
      if (count >= perLine && ii < (szControlUp-1))
      {
         boundsMsg << std::endl;
         boundsMsg << "                         ";
         count = 0;
      }
   }
   for (Integer ii = 0; ii < szStaticUp; ii++)
   {
      boundsMsg << GmatStringUtil::BuildNumber(staticUp(ii), true, 12) << "   ";
      count++;
      if (count >= perLine && ii < (szStaticUp - 1))
      {
         boundsMsg << std::endl;
         boundsMsg << "                         ";
         count = 0;
      }
   }
   
   boundsMsg << std::endl;
   boundsMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   boundsMsg << "                          Time" << std::endl;
   boundsMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;

   // Write the time data here
   boundsMsg << "    Lower Bounds       ";
   boundsMsg << GmatStringUtil::BuildNumber(timeLow, true);
   boundsMsg << std::endl;
   boundsMsg << "    Upper Bounds       ";
   boundsMsg << GmatStringUtil::BuildNumber(timeUp, true);
   boundsMsg << std::endl;
   
   MessageInterface::ShowMessage("%s\n", boundsMsg.str().c_str());
}

//------------------------------------------------------------------------------
// void ReportDecisionVectorData(bool addHeader = true)
//------------------------------------------------------------------------------
/**
 * Reports Decision Vector data (parameters and dynamic variables) to the log.
 * TBD: review report changes due to static params
 *
 */
//------------------------------------------------------------------------------
void Phase::ReportDecisionVectorData(bool addHeader)
{
   Integer perLine = 5;
   
   // Get initial and final times
   Real tInit  = GetFirstTime();
   Real tFinal = GetLastTime();
   
   // Get State and Control Data
   Rmatrix stateData   = GetStateArray();
   Rmatrix controlData = GetControlArray();
   
   // Get the time vector
   Rvector times    = GetTimeVector();
   Integer numTimes = times.GetSize();

   IntegerArray timeTypes = transUtil->GetTimeVectorType();
   Integer numTimeTypes = timeTypes.size();
   
   // Get static parameters
   Rvector staticVec = GetStaticVector();

   #ifdef DEBUG_REPORT_DATA
      MessageInterface::ShowMessage("--- size of times is %d\n", numTimes);
      MessageInterface::ShowMessage("--- size of time types is %d\n", numTimeTypes);
   #endif
   // How big are these arrays?
   Integer stateX, stateY, controlX, controlY;
   stateData.GetSize(stateX, stateY);
   controlData.GetSize(controlX, controlY);
   
   std::stringstream decVecMsg;
   std::stringstream decVecHeader;
   
   decVecMsg.width(12);
   decVecMsg.fill(' ');
   //   decVecMsg(std::ios::scientific);
   decVecMsg.setf(std::ios::left);
   decVecMsg.precision(7);
   decVecMsg.setf(std::ios::fixed,std::ios::floatfield);

   // Write the header if requested
   if (addHeader)
      decVecMsg << ReportHeaderData();
   
   // Write the Parameters section header and divider and data
   decVecMsg << "  ============================================= Parameters " <<
   "===============================================" << std::endl;
   decVecMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   decVecMsg << "           TINIT          TFINAL" << std::endl;
   decVecMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   decVecMsg << "           ";
   decVecMsg << tInit << "  " << tFinal << std::endl;
   
   // Write the Static Variables section header and data; YK mod static params
   decVecMsg << "  ========================================== Static Variables " <<
      "===========================================" << std::endl;
   decVecMsg << "  ---------------------------------------------------------" <<
      "-----------------------------------------------" << std::endl;

   Integer count = 0;
   for (Integer ii = 0; ii < staticVec.GetSize(); ii++)
   {
      decVecHeader << "STATIC";
      if (ii < 10)
         decVecHeader << "0";
      decVecHeader << ii << "        ";
      count++;
      if (count >= perLine && ii < (staticVec.GetSize() - 1))
      {
         decVecHeader << std::endl;
         decVecHeader << "                            ";
         count = 0;
      }
   }

   decVecMsg << decVecHeader.str() << std::endl;
   decVecMsg << "  ---------------------------------------------------------" <<
      "-----------------------------------------------" << std::endl;

   // Write static parameter data; YK mod static params
   count = 0;
   Integer staticIdx = 0;
   for (Integer jj = 0; jj < staticVec.GetSize(); jj++)
   {
      decVecMsg << GmatStringUtil::BuildNumber(staticVec(jj), true, 13) << "  ";
      count++;
      if (count >= perLine && jj < (stateY - 1))
      {
         decVecMsg << std::endl;
         decVecMsg << "                          ";
         count = 0;
      }
   }
      
   decVecMsg << std::endl;


   // Write the Dynamic Variables section header and data
   decVecMsg << "  ========================================== Dynamic Variables " <<
   "===========================================" << std::endl;
   decVecMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   decVecMsg << "    Point    TIME           ";

   count = 0;
   for (Integer ii = 0; ii < stateY; ii++)
   {
      decVecHeader << "STATE";
      if (ii < 10)
         decVecHeader << "0";
      decVecHeader << ii << "        ";
      count++;
      if (count >= perLine && ii < (stateY-1))
      {
         decVecHeader << std::endl;
         decVecHeader << "                            ";
         count = 0;
      }
   }
   for (Integer ii = 0; ii < controlY; ii++)
   {
      decVecHeader << "CNTRL";
      if (ii < 10)
         decVecHeader << "0";
      decVecHeader << ii << "        ";
      count++;
      if (count >= perLine && ii < (controlY-1))
      {
         decVecHeader << std::endl;
         decVecHeader << "                            ";
         count = 0;
      }
   }
   
   decVecMsg << decVecHeader.str() << std::endl;
   decVecMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   
   // Write point number, time, state and control data
   count = 0;
   Integer stateIdx = 0;
   Integer cntrlIdx = 0;
   for (Integer ii = 0; ii < numTimes; ii++)
   {
      count = 0;
      decVecMsg << "    " << std::setw(5) << std::setfill(' ') << ii;
      decVecMsg << "  " <<
                GmatStringUtil::BuildNumber(times(ii), true, 13) << "  ";

      Integer itsType = timeTypes.at(ii);
      
      if ((itsType == 1) || (itsType == 2)) // if not, need blanks here??????
      {
         for (Integer jj = 0; jj < stateY; jj++)
         {
            decVecMsg << GmatStringUtil::BuildNumber(stateData(stateIdx,jj), true, 13) << "  ";
            count++;
            if (count >= perLine && jj < (stateY-1))
            {
               decVecMsg << std::endl;
               decVecMsg << "                          ";
               count = 0;
            }
         }
         stateIdx++;
      }
      if ((itsType == 1) || (itsType == 3))
      {
         for (Integer jj = 0; jj < controlY; jj++)
         {
            decVecMsg << GmatStringUtil::BuildNumber(controlData(cntrlIdx,jj), true, 13) << "  ";
            count++;
            if (count >= perLine && jj < (controlY-1))
            {
               decVecMsg << std::endl;
               decVecMsg << "                          ";
               count = 0;
            }
         }
         cntrlIdx++;
      }
      decVecMsg << std::endl;
   }
   
   decVecMsg << std::endl;
   MessageInterface::ShowMessage(decVecMsg.str().c_str());
}

//------------------------------------------------------------------------------
// void ReportDefectConstraintData(bool addHeader)
//------------------------------------------------------------------------------
/**
 * Reports Differential Constraint data to the log.
 *
 *
 */
//------------------------------------------------------------------------------
void Phase::ReportDefectConstraintData(bool addHeader)
{
   std::stringstream defectMsg;
   Integer           perLine = 6;
   
   // Write the header if requested
   if (addHeader)
      defectMsg << ReportHeaderData();
   
   #ifdef DEBUG_REPORT_DATA
      MessageInterface::ShowMessage("ABOUT to get the number of states and stuff ...\n");
   #endif

   Integer numStates = GetNumStateVars();
   // Get the time vector
   Rvector times     = GetTimeVector();
   Integer numTimes  = times.GetSize();
   
   IntegerArray timeTypes    = transUtil->GetTimeVectorType();
   Integer      numTimeTypes = timeTypes.size();
   
   #ifdef DEBUG_REPORT_DATA
      MessageInterface::ShowMessage("ABOUT to set the width etc. ...\n");
      MessageInterface::ShowMessage("   numStates = %d, numTimes = %d, numTimeTypes = %d\n",
                                    numStates, numTimes, numTimeTypes);
      MessageInterface::ShowMessage("   perLine = %d\n", perLine);
      MessageInterface::ShowMessage("   size of defectConVec = %d\n",
                                    defectConVec.GetSize());
   #endif

   defectMsg.width(12);
   defectMsg.fill(' ');
   defectMsg.setf(std::ios::scientific);
   defectMsg.setf(std::ios::left);
   defectMsg.precision(7);

   // Write the header
   defectMsg << "  ====================================== Differential Constraints " <<
   "========================================" << std::endl;
   defectMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   defectMsg << "   Intrvl   ";

   // Write the rest of the header
   Integer count = 0;
   for (Integer ii = 0; ii < numStates; ii++)
   {
      #ifdef DEBUG_REPORT_DATA
         MessageInterface::ShowMessage("IN THE LOOP (numStates) ...\n");
         MessageInterface::ShowMessage("   ii = %d, count = %d\n",
                                       ii, count);
      #endif
      defectMsg << "DEFECT";
      if (ii < 10)
         defectMsg << "0";
      defectMsg << ii << "       ";
      count++;
      if ((count >= perLine) && ii < (numStates-1))
      {
         defectMsg << std::endl;
         defectMsg << "                           ";
         count = 0;
      }
   }
   defectMsg << std::endl;
   defectMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   // Write the data
   // Only time vector type 1 has these constraints
   Integer startIdx = 0;
   Integer stopIdx  = numStates - 1;
   
   for (Integer ii = 0; ii < numTimeTypes - constraintTimeOffset; ii++)
   {
      count = 0;
      #ifdef DEBUG_REPORT_DATA
         MessageInterface::ShowMessage("IN THE LOOP (numTimeTypes) ...\n");
         MessageInterface::ShowMessage("   ii = %d, count = %d\n",
                                       ii, count);
         MessageInterface::ShowMessage("   constraintTimeOffset = %d\n",
                                       constraintTimeOffset);
      #endif
      if (timeTypes.at(ii) == 1)
      {
         defectMsg << "    " << std::setw(5) << std::setfill(' ') << ii;
         defectMsg << " ";
//         << GmatStringUtil::BuildNumber(times(ii), true, 13) << " ";
         for (Integer jj = 0; jj < numStates; jj++)
         {
            #ifdef DEBUG_REPORT_DATA
               MessageInterface::ShowMessage("IN THE LOOP (numStates) ...\n");
               MessageInterface::ShowMessage("   jj = %d, count = %d\n",
                                             jj, count);
            #endif
            defectMsg << GmatStringUtil::BuildNumber(defectConVec(startIdx + jj), true, 12) << "   ";
            count++;
            // if we've reached the end of the line and there are more things to write
            if ((count >= perLine) && jj < (numStates-1))
            {
               defectMsg << std::endl;
               defectMsg << "                        ";
               count = 0;
            }
         }
         defectMsg << std::endl;
      }
      startIdx += numStates;
      stopIdx  += numStates;
      #ifdef DEBUG_REPORT_DATA
         MessageInterface::ShowMessage("AT END OF THE LOOP (numTimeTypes) ...\n");
         MessageInterface::ShowMessage("   startIdx = %d, stopIdx = %d\n",
                                       startIdx, stopIdx);
      #endif
   }
   
   defectMsg << std::endl;
   MessageInterface::ShowMessage(defectMsg.str().c_str());
}

//------------------------------------------------------------------------------
// void ReportAlgebraicConstraintData(bool addHeader = true)
//------------------------------------------------------------------------------
/**
 * Reports Algebraic Constraint data to the log.
 *
 *
 */
//------------------------------------------------------------------------------
void Phase::ReportAlgebraicConstraintData(bool addHeader)
{
   std::stringstream algebraicMsg;
   Integer           perLine = 6;
   
   // Get the time vector
   Rvector times     = GetTimeVector();
   Integer numTimes  = times.GetSize();
   
   IntegerArray timeTypes    = transUtil->GetTimeVectorType();
   Integer      numTimeTypes = timeTypes.size();
   
   algebraicMsg.width(12);
   algebraicMsg.fill(' ');
   algebraicMsg.setf(std::ios::scientific);
   algebraicMsg.setf(std::ios::left);
   algebraicMsg.precision(7);
   
   // Write the header if requested
   if (addHeader)
      algebraicMsg << ReportHeaderData();

   // Write the header
   algebraicMsg << "  ======================================== Algebraic Constraints " <<
   "=========================================" << std::endl;
   algebraicMsg << "  ---------------------------------------------------------" <<
   "-----------------------------------------------" << std::endl;
   algebraicMsg << "   Point    TIME           ";
//   algebraicMsg << "QNORM_a" << std::endl; // switch to ALGPATH1, etc.
//   algebraicMsg << "  ---------------------------------------------------------" <<
//   "-----------------------------------------------" << std::endl;
   // Write the data
   
   Integer count = 0;
   
   // First check to see if there are alg. path functions
   if (pathFunctionManager->HasAlgFunctions())
   {
      Integer numAlgFuncsPerPoint = pathFunctionManager->GetNumAlgFunctions();
      // Finish the header, based on the number of alg functions
      count = 0;
      for (Integer ii = 0; ii < numAlgFuncsPerPoint; ii++)
      {
         algebraicMsg << "ALGPATH";
         if (ii < 10)
            algebraicMsg << "0";
         algebraicMsg << ii << "       ";
         count++;
         // if we've reached the end of the line and there are more things to write
         if ((count >= perLine) && ii << (numAlgFuncsPerPoint-1))
         {
            algebraicMsg << std::endl;
            algebraicMsg << "                          ";
            count = 0;
         }
      }
      algebraicMsg << std::endl;
      algebraicMsg << "  ---------------------------------------------------------" <<
      "-----------------------------------------------" << std::endl;
      
      Integer startIdx = 0;
      Integer stopIdx  = numAlgFuncsPerPoint - 1;
      for (Integer pointIdx = 0; pointIdx < numTimeTypes; pointIdx++)
      {
         count = 0;
         // Only have defect constraints at points that are type 1
         if (timeTypes.at(pointIdx) == 1)
         {
            algebraicMsg << "    " << std::setw(5) << std::setfill(' ') << pointIdx;
            algebraicMsg << "  " <<
            GmatStringUtil::BuildNumber(times(pointIdx), true, 13) << "  ";
            for (Integer jj = 0; jj < numAlgFuncsPerPoint; jj++)
            {
               algebraicMsg << GmatStringUtil::BuildNumber(algPathConVec(startIdx + jj), true, 12) << "   ";
               count++;
               if (count > perLine && jj < (numAlgFuncsPerPoint - 1))
               {
                  algebraicMsg << std::endl;
                  //               algebraicMsg << "                            "; // ??
                  count = 0;
               }
            }
         }
         algebraicMsg << std::endl;
   
         // Update the bookkeeping parameters
         startIdx += numAlgFuncsPerPoint;
         stopIdx  += numAlgFuncsPerPoint;
      }
   }
   else
   {
      algebraicMsg << std::endl;
      algebraicMsg << "  ---------------------------------------------------------" <<
      "-----------------------------------------------" << std::endl;
      algebraicMsg << "   *** No algebraic functions ***" << std::endl;
   }
   
   algebraicMsg << std::endl;
   MessageInterface::ShowMessage(algebraicMsg.str().c_str());
}

//------------------------------------------------------------------------------
// void ReportAllData()
//------------------------------------------------------------------------------
/**
 * Reports all data to the log.
 *
 *
 */
//------------------------------------------------------------------------------
void Phase::ReportAllData()
{
   // only write the header info at the top of the entire report
   #ifdef DEBUG_REPORT_DATA
      MessageInterface::ShowMessage("ABOUT to report Bounds Data ...\n");
   #endif
   ReportBoundsData(true);
   #ifdef DEBUG_REPORT_DATA
      MessageInterface::ShowMessage("ABOUT to report Decision Vector Data ...\n");
   #endif
   ReportDecisionVectorData(false);
   #ifdef DEBUG_REPORT_DATA
      MessageInterface::ShowMessage("ABOUT to report Defect Constraint Data ...\n");
   #endif
   ReportDefectConstraintData(false);
   #ifdef DEBUG_REPORT_DATA
      MessageInterface::ShowMessage("ABOUT to report Algebraic Constraint Data ...\n");
   #endif
   ReportAlgebraicConstraintData(false);
   #ifdef DEBUG_REPORT_DATA
      MessageInterface::ShowMessage("DONE reporting...\n");
   #endif
}

//------------------------------------------------------------------------------
// std::string ReportHeaderData()
//------------------------------------------------------------------------------
/**
 * Returns a string containing the header information for the data reports
 *
 *
 */
//------------------------------------------------------------------------------
std::string Phase::ReportHeaderData()
{
   std::stringstream headerMsg;
   
   headerMsg << "  =======================================================" <<
   "=================================================" << std::endl;
   headerMsg << "  ====                                            Phase " <<
   phaseNum <<
   "                                             ====" << std::endl;
   headerMsg << "  =======================================================" <<
   "=================================================" << std::endl;
   headerMsg << std::endl;
   
   return headerMsg.str();
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void InitializePathFunctionInputData()
//------------------------------------------------------------------------------
/**
 * Initializes the path function input data
 *
 *
 */
//------------------------------------------------------------------------------
void Phase::InitializePathFunctionInputData()
{
   pathFunctionInputData->Initialize(config->GetNumStateVars(),
                                     GetNumControlVars());
}

//------------------------------------------------------------------------------
// void IntializeJacobians()
//------------------------------------------------------------------------------
/**
 * Initializes the jacobians
 *
 */
//------------------------------------------------------------------------------
void Phase::IntializeJacobians()
{
   SparseMatrixUtil::SetSize(nlpConstraintJacobian, config->GetNumTotalConNLP(),
                             config->GetNumDecisionVarsNLP());
   SparseMatrixUtil::SetSize(nlpCostJacobian, 1,
                             config->GetNumDecisionVarsNLP());
}


//------------------------------------------------------------------------------
// void PrepareForMeshRefinement()
//------------------------------------------------------------------------------
/**
 * Prepares fot the mesh refinement
 *
 */
//------------------------------------------------------------------------------
void Phase::PrepareForMeshRefinement()
{
   // Previously, old state and control copied here
   // Now, it is handled by generating new state and control at MR code.
   // So, do nothing here. 
   // However, this place may be useful in the future.
   // So, keep the method. 
   // YK 2017.08.22
}

//------------------------------------------------------------------------------
// void SetInitialGuess()
//------------------------------------------------------------------------------
/**
 * Sets the initial guess
 *
 * NOTE: this currently only sets the initial guess from the GuessGenerator
 *
 */
//------------------------------------------------------------------------------
void Phase::SetInitialGuess()
{
   // no mesh-refinement related things are done here
   /// but subclass can rewrite this method so that a proper initial guess 
   /// is provided based on the collocation 
  
   if (!isRefining)
   {
      SetInitialGuessFromGuessGen();
   }   
   else
   { 
      // updated by YK for MR; 2017.08.14
      decVector->SetStateArray(newStateGuess);
      decVector->SetControlArray(newControlGuess);
   }
}

//------------------------------------------------------------------------------
// void InitializeDecisionVector()
//------------------------------------------------------------------------------
/**
 * Initializes the decision vector helper class
 *
 */
//------------------------------------------------------------------------------
void Phase::InitializeDecisionVector()
{
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage(
                  "In InitDecVec, numStatePoints = %d, numCcontrolPoints = %d,"
                  "numStateStagePoints = %d, numControlStagePoints = %d\n",
                  transUtil->GetNumStatePoints(),
                  transUtil->GetNumControlPoints(),
                  transUtil->GetNumStateStagePointsPerMesh(),
                  transUtil->GetNumControlStagePointsPerMesh());
   #endif
   decVector->Initialize(config->GetNumStateVars(), 
                         GetNumControlVars(),config->GetNumIntegralVars(),
                         config->GetNumStaticVars(),
                         transUtil->GetNumStatePoints(),
                         transUtil->GetNumControlPoints(),
                         transUtil->GetNumStateStagePointsPerMesh(),
                         transUtil->GetNumControlStagePointsPerMesh());
   
   // YK mod static params: at initilization, use the static guess in config
   if (config->GetNumStaticVars() > 0)
      decVector->SetStaticVector(config->GetStaticVector());
}

//------------------------------------------------------------------------------
// void CheckFunctions()
//------------------------------------------------------------------------------
/**
 * Recomputes the user functions and/or quadratures if they are stale
 *
 */
//------------------------------------------------------------------------------
void Phase::CheckFunctions()
{
   #ifdef DEBUG_PHASE
      MessageInterface::ShowMessage("Entering Phase::CheckFunctions ...\n");
      MessageInterface::ShowMessage("   recomputeUserFunctions      = %s\n",
                                    (recomputeUserFunctions? "true" : "false"));
      MessageInterface::ShowMessage("   recomputeNLPFunctions        = %s\n",
                                    (recomputeNLPFunctions? "true" : "false"));
      MessageInterface::ShowMessage("   recomputeAlgPathConstraints = %s\n", (recomputeAlgPathConstraints? "true" : "false"));
      MessageInterface::ShowMessage("   recomputeAlgCostFunction    = %s\n", (recomputeAlgCostFunction? "true" : "false"));
   #endif

   // Update functions if necessary.
   if (recomputeUserFunctions)
      ComputeUserFunctions();

   // Update quadratures if necessary
   if (recomputeNLPFunctions)
   {
      if (config->HasDefectCons())
         ComputeDefectConstraints();

      if (pathFunctionManager->HasCostFunction())
      {
         costFunctionIntegral = 0;
         ComputeIntegralCost();
      }
      //  Update Algebraic path constraints
      if (config->HasAlgPathCons())
      {
         ComputeAlgebraicPathConstraints();
      }
      // Update cost function
      if (config->HasAlgebraicCost())
      {
         // what should be done here? YK 2018.02.01
      }
      recomputeNLPFunctions = false;
   }
   
   #ifdef DEBUG_PHASE
         MessageInterface::ShowMessage("LEAVING Phase::CheckFunctions ...\n");
   #endif
}

//------------------------------------------------------------------------------
// void ComputeUserFunctions()
//------------------------------------------------------------------------------
/**
 * Computes user path and point functions
 *
 */
//------------------------------------------------------------------------------
void Phase::ComputeUserFunctions()
{
   #ifdef DEBUG_PHASE
      MessageInterface::ShowMessage(
                                 "Entering Phase::ComputeUserFunctions ...\n");
   #endif
   // YK mod static vars: before computing user functions, 
   // update static vars in problem characteristics according to decision vector
   UpdateStaticVector();

   ComputePathFunctions();
   recomputeUserFunctions = false;
   recomputeNLPFunctions  = true;
}

//------------------------------------------------------------------------------
// void ComputePathFunctions()
//------------------------------------------------------------------------------
/**
 * Computes user path functions
 *
 */
//------------------------------------------------------------------------------
void Phase::ComputePathFunctions()
{
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("Entering ComputePathFunctions  ...\n");
   #endif
   // Computes user path functions

   // Compute user functions at each point
   IntegerArray tvTypes    = transUtil->GetTimeVectorType();
   Integer      numTimePts = transUtil->GetNumTimePoints();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("numTimePts = %d  ...\n", numTimePts);
      for (Integer ii = 0; ii < tvTypes.size(); ii++)
         MessageInterface::ShowMessage("    tvTypes(%d) = %d\n",
                                       ii, tvTypes.at(ii));
   #endif
   
   // clear the arrays
   for (Integer i = 0; i < funcData.size(); ++i)
   {
      if (funcData.at(i))
         delete funcData.at(i);
   }
   /*
   for (Integer i = 0; i < userDynFunctionData.size(); ++i)
   {
      if (userDynFunctionData.at(i))
         delete userDynFunctionData.at(i);
   }
   for (Integer i = 0; i < userAlgFunctionData.size(); ++i)
   {
      if (userAlgFunctionData.at(i))
         delete userAlgFunctionData.at(i);
   }
   for (Integer i = 0; i < costIntFunctionData.size(); ++i)
   {
      if (costIntFunctionData.at(i))
         delete costIntFunctionData.at(i);
   }
   */
   funcData.clear();
   userDynFunctionData.clear();
   userAlgFunctionData.clear();
   costIntFunctionData.clear();
   
   //YK mod static vars: save static idxs vector, here, and use it for loop
   IntegerArray stcIdxs = decVector->GetStaticIdxs();

   for (Integer pt = 0; pt < numTimePts; pt++)
   {
      // Extract info on the current mesh/stage point
      Integer pointType   = tvTypes.at(pt);
      Integer meshIdx     = transUtil->GetMeshIndex(pt); 
      Integer stageIdx    = transUtil->GetStageIndex(pt);
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage(
                           "Before PreparePathFunction, meshIdx  = %d  ...\n",
                           meshIdx);
         MessageInterface::ShowMessage(
                           "                            stageIdx = %d  ...\n",
                           stageIdx);
      #endif
      IntegerArray tIdxs  = decVector->GetTimeIdxs();
      IntegerArray stIdxs = decVector->GetStateIdxsAtMeshPoint(meshIdx,
                                                               stageIdx);
      IntegerArray clIdxs = decVector->GetControlIdxsAtMeshPoint(meshIdx,
                                                                 stageIdx);
      // Prepare the user function data structures then call the
      // user function
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("tIdxs  size = %d  ...\n",
                                       (Integer) tIdxs.size());
         MessageInterface::ShowMessage("stIdxs size = %d  ...\n",
                                       (Integer) stIdxs.size());
         MessageInterface::ShowMessage("clIdxs size = %d  ...\n",
                                       (Integer) clIdxs.size());
      #endif
      PreparePathFunction(meshIdx,stageIdx,pointType,pt);
      
      // CREATING NEW ONE HERE????  Is that what we want to do?
      funcData.push_back(new PathFunctionContainer());
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage(
                                 "INITIALIZING PathFunctionContainer ... \n");
      #endif
      funcData.back()->Initialize();

      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("Calling EvaluateUserFunction with phaseNum = %d, pathFunctionInputData = <%p> ... \n",
                                       pathFunctionInputData->GetPhaseNum(), pathFunctionInputData);
      #endif
      // Evaluate user functions and Jacobians
      funcData.back() = pathFunctionManager->EvaluateUserFunction(
                                      pathFunctionInputData,funcData.back());
      funcData.back() = pathFunctionManager->EvaluateUserJacobian(
                                      pathFunctionInputData,funcData.back()); 
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage(
                              "AFTER calling EvalUserF and EvaluUserJ ... \n");
         MessageInterface::ShowMessage("   dyn?  %s\n",
                                       (pathFunctionManager->HasDynFunctions()?
                                        "true" : "false"));
         MessageInterface::ShowMessage("   cost? %s\n",
                                       (pathFunctionManager->HasCostFunction()?
                                        "true" : "false"));
         MessageInterface::ShowMessage("   alg?  %s\n",
                                       (pathFunctionManager->HasAlgFunctions()?
                                        "true" : "false"));
      #endif

      // Handle defect constraints
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("HasDynFunctions??? ... \n");
      #endif
      if (pathFunctionManager->HasDynFunctions())
      {
         FunctionOutputData *dyn = funcData.back()->GetDynData();
         userDynFunctionData.push_back(dyn);
         dyn->SetNLPData(meshIdx, stageIdx, stIdxs, clIdxs, stcIdxs);
      }

      // Handle cost function
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("HasCostFunction??? ... \n");
      #endif
      if (pathFunctionManager->HasCostFunction())
      {
         FunctionOutputData *cost = funcData.back()->GetCostData();
         costIntFunctionData.push_back(cost);
         cost->SetNLPData(meshIdx, stageIdx, stIdxs, clIdxs, stcIdxs);
      }

      // Handle algebraic constraints
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("HasAlgFunction??? ... \n");
      #endif
      if (pathFunctionManager->HasAlgFunctions())
      {
         FunctionOutputData *alg = funcData.back()->GetAlgData();
         userAlgFunctionData.push_back(alg);
         alg->SetNLPData(meshIdx, stageIdx, stIdxs, clIdxs, stcIdxs);
      }
   }
   // @TODO delete local pointers??
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("LEAVING ComputePathFunctions \n");
   #endif
}

//------------------------------------------------------------------------------
// void ComputeSparsityPattern()
//------------------------------------------------------------------------------
/**
 * Computes the sparsity of the Phase NLP problem.
 *
 */
//------------------------------------------------------------------------------
void Phase::ComputeSparsityPattern()
{
   #ifdef DEBUG_PHASE_SPARSE
      MessageInterface::ShowMessage(
                              "Entering Phase::ComputeSparsityPattern ...\n");
      MessageInterface::ShowMessage(" size of conSparsityPattern = %d x %d\n", 
                                    (Integer) conSparsityPattern.size1(),
                                    (Integer) conSparsityPattern.size2());
      MessageInterface::ShowMessage(" size of costSparsityPattern = %d x %d\n", 
                                    (Integer) costSparsityPattern.size1(),
                                    (Integer) costSparsityPattern.size2());
      MessageInterface::ShowMessage("config = %sNULL\n", (config? "NOT " : ""));
      MessageInterface::ShowMessage(
                     "  attempting to set conSparsityPattern to size %d x %d\n",
                     config->GetNumTotalConNLP(),
                     config->GetNumDecisionVarsNLP());
      MessageInterface::ShowMessage(
                     "  attempting to set costSparsityPattern to size 1 x %d\n",
                     config->GetNumDecisionVarsNLP());
   #endif
   
   SparseMatrixUtil::SetSize(conSparsityPattern, config->GetNumTotalConNLP(),
                             config->GetNumDecisionVarsNLP());
   SparseMatrixUtil::SetSize(costSparsityPattern, 1,
                             config->GetNumDecisionVarsNLP());
   #ifdef DEBUG_PHASE_SPARSE
      MessageInterface::ShowMessage("Entering Phase:: after setsize ...\n");
   #endif
   if (config->HasAlgPathCons())
   {
      #ifdef DEBUG_PHASE_SPARSE
         MessageInterface::ShowMessage(
                              "CSP: conSparsityPattern size (row) = %d\n",
                              config->GetNumTotalConNLP());
         MessageInterface::ShowMessage(
                              "CSP: conSparsityPattern size (col) = %d\n",
                              config->GetNumDecisionVarsNLP());
         MessageInterface::ShowMessage(
                           "CSP: About to call SetSparseBLockMatrix (alg)\n");
      #endif
      SparseMatrixUtil::SetSparseBLockMatrix(conSparsityPattern,
                                       algPathConStartIdx, 0,
                                       algPathNLPFuncUtil->ComputeSparsity());
   }
   if (config->HasDefectCons())
   {
      #ifdef DEBUG_PHASE_SPARSE
            MessageInterface::ShowMessage(
                        "CSP: About to call SetSparseBLockMatrix (defect)\n");
      #endif
      SparseMatrixUtil::SetSparseBLockMatrix(conSparsityPattern,
                                    defectConStartIdx, 0,
                                    transUtil->ComputeDefectSparsityPattern());
   }
   if (config->HasIntegralCost())
   {
      #ifdef DEBUG_PHASE_SPARSE
            MessageInterface::ShowMessage(
                           "CSP: About to call SetSparseBLockMatrix (cost)\n");
      #endif
      SparseMatrixUtil::SetSparseBLockMatrix(costSparsityPattern, 0, 0,
                                    transUtil->ComputeCostSparsityPattern(),
                                    false);
      // -----> how to add these together?
      // @TODO - this will need fixing when we have Cost
      costSparsityPattern = SparseMatrixUtil::CopySparseMatrix(
                                       transUtil->ComputeCostSparsityPattern());
   }
   #ifdef DEBUG_PHASE_SPARSE
      MessageInterface::ShowMessage("CSP: LEAVING\n");
   #endif
}

//------------------------------------------------------------------------------
// void SetProblemCharacteristics()
//------------------------------------------------------------------------------
/**
 * Sets variable dependencies and function type flags
 *
 */
//------------------------------------------------------------------------------
void Phase::SetProblemCharacteristics()
{
   if (pathFunctionManager->HasAlgFunctions())
   {
      config->SetHasAlgPathCons(true);
   }
   if (pathFunctionManager->HasDynFunctions())
   {
      config->SetHasDefectCons(true);
   }
   if (pathFunctionManager->HasCostFunction())
   {
      config->SetHasIntegralCost(true);
   }
}

//------------------------------------------------------------------------------
// void InitializeUserFunctions()
//------------------------------------------------------------------------------
/**
 * Initializes the user functions
 *
 */
//------------------------------------------------------------------------------
void Phase::InitializeUserFunctions()
{
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("Entering InitializeUserFunctions  ...\n");
      MessageInterface::ShowMessage("numStateVars   = %d\n",
                                    GetNumStateVars());
      MessageInterface::ShowMessage("numControlVars = %d\n",
                                    GetNumControlVars());
      std::cout << "Entering InitializeUserFunctions  ...\n";
      std::cout << " numStateVars, numControlVars = ";
      std::cout << GetNumStateVars() << " " << GetNumControlVars() << std::endl;
   #endif
   // Initialize the path function
   pathFunctionInputData->Initialize(GetNumStateVars(), GetNumControlVars(), 
                                     GetNumStaticVars());
   pathFunctionInputData->SetStateVector(GetStateUpperBound());
   pathFunctionInputData->SetTime(GetTimeUpperBound());

   if (GetNumControlVars() > 0)
      pathFunctionInputData->SetControlVector(GetControlUpperBound());
   
   if (GetNumStaticVars() > 0)
      pathFunctionInputData->SetStaticVector(GetStaticUpperBound());

   pathFunctionInputData->SetPhaseNum(phaseNum);
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("--- path function initialized  ...\n");
      std::cout << "--- path function initialized  ...\n";
   #endif

   // Create bounds data needed for initialization
   BoundData *boundData = new BoundData();
   boundData->SetStateLowerBound(GetStateLowerBound());
   boundData->SetStateUpperBound(GetStateUpperBound());
   if (GetNumControlVars() > 0)
   {
      boundData->SetControlLowerBound(GetControlLowerBound());
      boundData->SetControlUpperBound(GetControlUpperBound());
   }
   if (GetNumStaticVars() > 0)
   {
      boundData->SetStaticLowerBound(GetStaticLowerBound());
      boundData->SetStaticUpperBound(GetStaticUpperBound());
   }
   Rvector timeUpper(1, GetTimeUpperBound());
   Rvector timeLower(1, GetTimeLowerBound());
   boundData->SetTimeUpperBound(timeUpper);
   boundData->SetTimeLowerBound(timeLower);

   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("--- about to create and initialize the PathFunctionContainer  ...\n");
      std::cout << "--- about to create and initialize the PathFunctionContainer  ...\n";
   #endif
   // @TODO - do we really want a new one here??
   // Switched to adding the new pointer to the list rather than a
   // separate pointer to avoid memory leak
   funcData.push_back(new PathFunctionContainer());
   funcData.back()->Initialize();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage(
                        "--- PathFunctionContainer  is initialized...\n");
      std::cout << "--- PathFunctionContainer  is initialized...\n";
   #endif

   pathFunctionManager->Initialize(pathFunction,pathFunctionInputData,
                                   funcData.back(),boundData);
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage(
                              "--- pathFunctionManager initialized  ...\n");
      std::cout << "--- pathFunctionManager initialized  ...\n";
   #endif

   algPathConVecNames = (funcData.back()->GetAlgData())->GetFunctionNames();
   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("numState = %d\n",
                                    pathFunctionInputData->GetNumStateVars());
      MessageInterface::ShowMessage("LEAVING InitializeUserFunctions  ...\n");
   #endif

   // Delete the boundData pointer that is no longer needed
   delete boundData;
}

//------------------------------------------------------------------------------
// void SetConstraintProperties()
//------------------------------------------------------------------------------
/**
 * Computes the number of constraints and start/end indexes
 *
 */
//------------------------------------------------------------------------------
void Phase::SetConstraintProperties()
{
   // Computes number of constraints and start/end indeces

   //  Set chunk values and bounds.
   numAlgPathConNLP = transUtil->GetNumPathConstraintPoints() *
                      pathFunctionManager->GetNumAlgFunctions();
   config->SetNumTotalConNLP(config->GetNumDefectConNLP() + numAlgPathConNLP);

   // Set start and end indeces of different constraint
   //  parameter types.  These describe where different chunks
   //  of the constraint vector begin and end.
   defectConStartIdx = 0; // was 1
   defectConEndIdx  = defectConStartIdx + config->GetNumDefectConNLP() - 1;
   // TODO.  If certain type of constraint does not exist, indeces
   // are populated with useless data that makes it look like they
   // exist.  May want to change that to be zeros or something.
   algPathConStartIdx = defectConEndIdx + 1;
   algPathConEndIdx   = algPathConStartIdx + numAlgPathConNLP -1;
}

//------------------------------------------------------------------------------
// void SetDecisionVectorBounds()
//------------------------------------------------------------------------------
/**
 * Sets the upper and lower bounds on the decision vector
 *
 */
//------------------------------------------------------------------------------
void Phase::SetDecisionVectorBounds()
{
   // Dimension the vectors
   decisionVecLowerBound.SetSize(config->GetNumDecisionVarsNLP());
   decisionVecUpperBound.SetSize(config->GetNumDecisionVarsNLP());

   // Use a temporary decision vector object for bookkeeping
   DecVecTypeBetts *boundVector = new DecVecTypeBetts();

   boundVector->Initialize(config->GetNumStateVars(), 
                GetNumControlVars(),config->GetNumIntegralVars(),
                config->GetNumStaticVars(),
                transUtil->GetNumStatePoints(),transUtil->GetNumControlPoints(),
                transUtil->GetNumStateStagePointsPerMesh(),
                transUtil->GetNumControlStagePointsPerMesh());

   // Assemble the state bound array
   Rmatrix upperBoundStateArray(decVector->GetNumStatePoints(), 
                                config->GetNumStateVars());
   Rmatrix lowerBoundStateArray(decVector->GetNumStatePoints(),
                                config->GetNumStateVars());
   Rvector stateLower = GetStateLowerBound();
   Rvector stateUpper = GetStateUpperBound();
   
   for (Integer rowIdx = 0; rowIdx < decVector->GetNumStatePoints(); rowIdx++)
   {
      for (Integer jj = 0; jj < stateLower.GetSize(); jj++)
         lowerBoundStateArray(rowIdx,jj) = stateLower(jj);
      for (Integer jj = 0; jj < stateUpper.GetSize(); jj++)
         upperBoundStateArray(rowIdx,jj) = stateUpper(jj);
   }

   Rmatrix upperBoundControlArray;
   Rmatrix lowerBoundControlArray;
   //  Assemble the control bound array
   if (config->HasControlVars())
   {
      upperBoundControlArray.SetSize(decVector->GetNumControlPoints(),
                    GetNumControlVars());
      lowerBoundControlArray.SetSize(decVector->GetNumControlPoints(),
                    GetNumControlVars());
      Rvector controlLower = GetControlLowerBound();
      Rvector controlUpper = GetControlUpperBound();
      for (Integer rowIdx = 0; rowIdx < decVector->GetNumControlPoints();
           rowIdx++)
      {
         for (Integer jj = 0; jj < controlLower.GetSize(); jj++)
            lowerBoundControlArray(rowIdx,jj) = controlLower(jj);
         for (Integer jj = 0; jj < controlUpper.GetSize(); jj++)
            upperBoundControlArray(rowIdx,jj) = controlUpper(jj);
      }
   }

   //  Set time vector bounds
   Rvector upperBoundTimeArray(2, GetTimeUpperBound(), GetTimeUpperBound());
   Rvector lowerBoundTimeArray(2, GetTimeLowerBound(), GetTimeLowerBound());

   //  Set static vector bounds
   
   Rvector staticUpper, staticLower;
   if ((config->HasStaticVars()) && (config->GetNumStaticVars()>0))
   {      
      staticLower.SetSize(config->GetNumStaticVars());
      staticUpper.SetSize(config->GetNumStaticVars());
      staticUpper = GetStaticUpperBound();
      staticLower = GetStaticLowerBound();      
   }

   //  Set lower bound vector
   boundVector->SetStateArray(lowerBoundStateArray);
   if (config->HasControlVars())
   {
      boundVector->SetControlArray(lowerBoundControlArray);
   }
   boundVector->SetTimeVector(lowerBoundTimeArray);   
   if ((config->HasStaticVars()) && (config->GetNumStaticVars()>0))
      boundVector->SetStaticVector(staticLower);
   decisionVecLowerBound = boundVector->GetDecisionVector();

   //  Set upper bound vector
   boundVector->SetStateArray(upperBoundStateArray);
   if (config->HasControlVars())
   {
      boundVector->SetControlArray(upperBoundControlArray);
   }
   boundVector->SetTimeVector(upperBoundTimeArray);   
   if ((config->HasStaticVars()) && (config->GetNumStaticVars()>0))
      boundVector->SetStaticVector(staticUpper);
   decisionVecUpperBound = boundVector->GetDecisionVector();

   delete boundVector;
}

//------------------------------------------------------------------------------
// void InitializeTimeVector()
//------------------------------------------------------------------------------
/**
 *Initializes the dimensional time vector
 *
 */
//------------------------------------------------------------------------------
void Phase::InitializeTimeVector()
{
   Rvector timeV(2, GetTimeInitialGuess(), GetTimeFinalGuess());
   decVector->SetTimeVector(timeV);
   transUtil->SetTimeVector(GetTimeInitialGuess(), GetTimeFinalGuess());
}

//------------------------------------------------------------------------------
// void SetConstraintBounds()
//------------------------------------------------------------------------------
/**
 * Sets bounds for all constraint types
 *
 */
//------------------------------------------------------------------------------
void Phase::SetConstraintBounds()
{
   // Computes the bounds for each constraint type.
   SetDefectConstraintBounds();
   SetPathConstraintBounds();

   // Now insert chunks into the complete bounds vector
   Integer dStartIdx = defectConStartIdx;
   Integer dEndIdx   = defectConEndIdx;
   Integer pStartIdx = algPathConStartIdx;
   Integer pEndIdx   = algPathConEndIdx;

   #ifdef DEBUG_PHASE_INIT
      MessageInterface::ShowMessage("--- dStartIdx = %d\n", dStartIdx);
      MessageInterface::ShowMessage("--- dEndIdx   = %d\n", dEndIdx);
      MessageInterface::ShowMessage("--- pStartIdx = %d\n", pStartIdx);
      MessageInterface::ShowMessage("--- pEndIdx   = %d\n", pEndIdx);
      if (!allConLowerBound.IsSized())
         MessageInterface::ShowMessage("allConLowerBound is NOT SIZED!\n");
      else
      {
         MessageInterface::ShowMessage(" --- size of allConLowerBound = %d\n",
                                       (Integer) allConLowerBound.GetSize());
      }
      if (!defectConLowerBound.IsSized())
         MessageInterface::ShowMessage("defectConLowerBound is NOT SIZED!\n");
      else
      {
         MessageInterface::ShowMessage(
                                    " --- size of defectConLowerBound = %d\n",
                                    (Integer) defectConLowerBound.GetSize());
      }
   #endif

   
   allConLowerBound.SetSize(algPathConEndIdx - defectConStartIdx + 1);
   allConUpperBound.SetSize(algPathConEndIdx - defectConStartIdx + 1);

   // Concatenate defect constraints bounds if any
   if (pathFunctionManager->HasDynFunctions())
   {
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("   HAS dyn functions ...\n");
         MessageInterface::ShowMessage("   defectConLowerBound = %s\n",
                                    defectConLowerBound.ToString(12).c_str());
         MessageInterface::ShowMessage("   defectConUpperBound = %s\n",
                                    defectConUpperBound.ToString(12).c_str());
      #endif
      Integer conIdx = 0;
      for (Integer ii = dStartIdx; ii <= dEndIdx; ii++)
      {
         allConLowerBound(ii) = defectConLowerBound(conIdx);
         allConUpperBound(ii) = defectConUpperBound(conIdx);
         conIdx++;
      }
   }

   // Concatenate algebraic path constraint bounds if any
   if (pathFunctionManager->HasAlgFunctions())
   {
      #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("   HAS alg functions ...\n");
      #endif
      Integer algIdx = 0;
      for (Integer ii = pStartIdx; ii <= pEndIdx; ii++)
      {
         allConLowerBound(ii) = algPathConLowerBound(algIdx);
         allConUpperBound(ii) = algPathConUpperBound(algIdx);
         algIdx++;
      }
   }
}

//------------------------------------------------------------------------------
// void SetDefectConstraintBounds()
//------------------------------------------------------------------------------
/**
 * Sets bounds on the defect constraint
 *
 */
//------------------------------------------------------------------------------
void Phase::SetDefectConstraintBounds()
{
   Integer sz = config->GetNumDefectConNLP();
   // Sets bounds on defect constraints
   defectConLowerBound.SetSize(sz);
   defectConUpperBound.SetSize(sz);
   for (Integer ii = 0; ii < sz; ii++)
   {
      defectConLowerBound[ii] = 0.0;
      defectConUpperBound[ii] = 0.0;
   }
}

//------------------------------------------------------------------------------
// void SetPathConstraintBounds()
//------------------------------------------------------------------------------
/**
 * Sets bounds on the path constraint
 *
 */
//------------------------------------------------------------------------------
void Phase::SetPathConstraintBounds()
{
   if (pathFunctionManager->HasAlgFunctions())
   {
      algPathConLowerBound.SetSize(numAlgPathConNLP);
      algPathConUpperBound.SetSize(numAlgPathConNLP);
      Integer lowIdx = 0;   // was 1;
      for (Integer conIdx = 0; conIdx < transUtil->GetNumPathConstraintPoints();
           conIdx++)
      {
         Integer highIdx = lowIdx + pathFunctionManager->GetNumAlgFunctions()-1;
         Rvector lowB    = pathFunctionManager->GetAlgFunctionsLowerBounds();
         Rvector highB   = pathFunctionManager->GetAlgFunctionsUpperBounds();
         Integer theIdx  = 0;
         for (Integer ii = lowIdx; ii <= highIdx; ii++)
         {
            algPathConLowerBound(ii) = lowB(theIdx);
            algPathConUpperBound(ii) = highB(theIdx);
            theIdx++;
         }
         lowIdx += pathFunctionManager->GetNumAlgFunctions();
      }
   }
   else
   {
      algPathConLowerBound.SetSize(0);
      algPathConUpperBound.SetSize(0);
   }
}

//------------------------------------------------------------------------------
// void SetInitialGuessFromGuessGen()
//------------------------------------------------------------------------------
/**
 * Calls guess utility to compute guess for state and control
 *
 */
//------------------------------------------------------------------------------
void Phase::SetInitialGuessFromGuessGen()
{
   #ifdef DEBUG_GUESS
      MessageInterface::ShowMessage("Entering SetInitialGuessFromGuessGen\n");
   #endif
   //  Calls guess utility to compute guess for state and control

   //  Intialize the guess generator class
   guessGen->Initialize(transUtil->GetTimeVector(),
                        decVector->GetNumStateVars(),
                        decVector->GetNumStatePoints(),
                        decVector->GetNumControlVars(),
                        decVector->GetNumControlPoints(),
                        initialGuessMode);

   #ifdef DEBUG_GUESS
      MessageInterface::ShowMessage("   guessGen initialized\n");
   #endif
   Rmatrix xGuessMat;
   Rmatrix uGuessMat;
   if ((initialGuessMode == "LinearNoControl")    ||
       (initialGuessMode == "LinearUnityControl") ||
       (initialGuessMode == "LinearCoast"))
   {
      Rvector init  = GetStateInitialGuess();
      Rvector final = GetStateFinalGuess();
      #ifdef DEBUG_GUESS
         MessageInterface::ShowMessage("   init guess  = %s\n",
                                       init.ToString(12).c_str());
         MessageInterface::ShowMessage("   final guess = %s\n",
                                       final.ToString(12).c_str());
      #endif
      guessGen->ComputeLinearGuess(init, final,
                                   xGuessMat, uGuessMat);
   }
   else if ((initialGuessMode == "UserGuessClass"))
   {
       guessGen->ComputeUserFunctionGuess(userGuessClass, scaleUtil,
                                          "dummyTimeType", // TODO: fix this!
                                          xGuessMat, uGuessMat);
   }
   else if ((initialGuessMode == "OCHFile"))
   {
       guessGen->ComputeGuessFromOCHFile(guessFileName,
                                         "dummyTimeType", // TODO: fix this!
                                         xGuessMat,uGuessMat);
   }
   else if ((initialGuessMode == "GuessArrays"))
   {
       guessGen->ComputeGuessFromArrayData(guessArrayData,
                                           "dummyTimeType", // TODO: fix this!
                                           xGuessMat,uGuessMat);
   }
   else
   {
      throw LowThrustException("Invalid InitialGuessMode!\n");
   }
   // Call the decision vector and populate with the guess
   #ifdef DEBUG_GUESS
      MessageInterface::ShowMessage("   xGuessMat  = %s\n",
                                    xGuessMat.ToString(12).c_str());
      MessageInterface::ShowMessage("   uGuessMat  = %s\n",
                                    uGuessMat.ToString(12).c_str());
   #endif
   decVector->SetStateArray(xGuessMat);
   decVector->SetControlArray(uGuessMat);
   #ifdef DEBUG_GUESS
      MessageInterface::ShowMessage("   decisionVectorGuess  = %s\n",
                         decVector->GetDecisionVector().ToString(12).c_str());
      MessageInterface::ShowMessage("LEAVING SetInitialGuessFromGuessGen\n");
   #endif
}

//------------------------------------------------------------------------------
// void PreparePathFunction(Integer meshIdx,   Integer stageIdx,
//                          Integer pointType, Integer pointIdx)
//------------------------------------------------------------------------------
/**
 * Calls guess utility to compute guess for state and control
 *
 * @param <meshIdx>   the mesh index
 * @param <stageIdx>  the stage index
 * @param <pointType> the point type
 * @param <pointIdx>  the point index
 *
 */
//------------------------------------------------------------------------------
void Phase::PreparePathFunction(Integer meshIdx,   Integer stageIdx,
                                Integer pointType, Integer pointIdx)
{
   #ifdef DEBUG_PHASE_PATH_INIT
      MessageInterface::ShowMessage("ENTERING PreparePathFunction\n");
      MessageInterface::ShowMessage(
            "   meshIdx = %d, stageIdx = %d, pointType = %d, pointIdx = %d, phaseNum = %d\n",
            meshIdx, stageIdx, pointType, pointIdx, phaseNum);
   #endif
   // Prepares user path function evaluation at a specific point

   // This function extracts the state, control, and time from decision vector
   pathFunctionInputData->SetPhaseNum(phaseNum);
   if (pointType == 1 || pointType == 2)
   {
      pathFunctionInputData->SetStateVector(
                             decVector->GetStateAtMeshPoint(meshIdx,stageIdx));
   }
   else
   {
      Rvector ones(GetNumStateVars());
      ones = ones * GmatMathConstants::QUIET_NAN;
      pathFunctionInputData->SetStateVector(ones);
   }
   if (pointType == 1 || pointType == 3)
   {
      pathFunctionInputData->SetControlVector(
                             decVector->GetControlAtMeshPoint(meshIdx,
                                                              stageIdx));
   }
   else
   {
      Rvector ones(GetNumControlVars());
      ones = ones * GmatMathConstants::QUIET_NAN;
      pathFunctionInputData->SetControlVector(ones); 
   }
   pathFunctionInputData->SetTime(transUtil->GetTimeAtMeshPoint(pointIdx));

   // YK mod static params; is it right to place this line here?
   pathFunctionInputData->SetStaticVector(decVector->GetStaticVector());

   #ifdef DEBUG_PHASE_PATH_INIT
         MessageInterface::ShowMessage("LEAVING PreparePathFunction\n");
   #endif
}
 
//------------------------------------------------------------------------------
// void InsertJacobianRowChunk(const RSMatrix &jacChunk,
//                             const IntegerArray &idxs)
//------------------------------------------------------------------------------
/**
 * Inserts a chunk into the jacobian
 *
 * @param <jacChunk>   the input jacobian chunk
 * @param <idxs>       the input array of indexes
 *
 */
//------------------------------------------------------------------------------
void Phase::InsertJacobianRowChunk(const RSMatrix &jacChunk,
                                   const IntegerArray &idxs)
{
   SparseMatrixUtil::SetSparseBLockMatrix(nlpConstraintJacobian,
                                          idxs[0], 0, &jacChunk);
}


//------------------------------------------------------------------------------
// void CopyArrays(const Phase &copy)
//------------------------------------------------------------------------------
/**
 * Copies all of the array/matrix data from the input object to this object
 *
 * @param <copy>   the object whose data to copy
 *
 */
//------------------------------------------------------------------------------
void Phase::CopyArrays(const Phase &copy)
{
   #ifdef DEBUG_PHASE_INIT
         MessageInterface::ShowMessage("In Phase::CopyArrays!!!!!!!!!!!!\n");
   #endif
   Integer sz = (Integer) copy.initialGuessControl.GetSize();
   initialGuessControl.SetSize(sz);
   initialGuessControl = copy.initialGuessControl;

   sz = (Integer) copy.algPathConVec.GetSize();
   algPathConVec.SetSize(sz);
   algPathConVec = copy.algPathConVec;

   sz = (Integer) copy.defectConVec.GetSize();
   defectConVec.SetSize(sz);
   defectConVec = copy.defectConVec;

   algPathConVecNames = copy.algPathConVecNames;

   // mesh-refinement related things. YK 2017.08.22
   Integer sz1, sz2;
   sz1 = (Integer)copy.newStateGuess.GetNumRows();
   sz2 = (Integer)copy.newStateGuess.GetNumColumns();
   newStateGuess.SetSize(sz1,sz2);
   newStateGuess = copy.newStateGuess;

   sz1 = (Integer)copy.newControlGuess.GetNumRows();
   sz2 = (Integer)copy.newControlGuess.GetNumColumns();
   newControlGuess.SetSize(sz1, sz2);
   newControlGuess = copy.newControlGuess;
   sz = (Integer)copy.maxRelErrorVec.GetSize();
   maxRelErrorVec.SetSize(sz);
   maxRelErrorVec = copy.maxRelErrorVec;

   // @todo figure out how to copy RSMatrix !!!!
   SparseMatrixUtil::CopySparseMatrix(copy.nlpConstraintJacobian,
                                      nlpConstraintJacobian);

   SparseMatrixUtil::CopySparseMatrix(copy.nlpCostJacobian, nlpCostJacobian);


   SparseMatrixUtil::CopySparseMatrix(copy.conSparsityPattern,
                                      conSparsityPattern);

   SparseMatrixUtil::CopySparseMatrix(copy.costSparsityPattern,
                                      costSparsityPattern);

   sz = (Integer) copy.defectConLowerBound.GetSize();
   defectConLowerBound.SetSize(sz);
   defectConLowerBound = copy.defectConLowerBound;

   sz = (Integer) copy.defectConUpperBound.GetSize();
   defectConUpperBound.SetSize(sz);
   defectConUpperBound = copy.defectConUpperBound;

   sz = (Integer) copy.algPathConLowerBound.GetSize();
   algPathConLowerBound.SetSize(sz);
   algPathConLowerBound = copy.algPathConLowerBound;

   sz = (Integer) copy.algPathConUpperBound.GetSize();
   algPathConUpperBound.SetSize(sz);
   algPathConUpperBound = copy.algPathConUpperBound;

   sz = (Integer) copy.algEventConLowerBound.GetSize();
   algEventConLowerBound.SetSize(sz);
   algEventConLowerBound = copy.algEventConLowerBound;

   sz = (Integer) copy.algEventConUpperBound.GetSize();
   algEventConUpperBound.SetSize(sz);
   algEventConUpperBound = copy.algEventConUpperBound;

   sz = (Integer) copy.allConLowerBound.GetSize();
   allConLowerBound.SetSize(sz);
   allConLowerBound = copy.allConLowerBound;

   sz = (Integer) copy.allConUpperBound.GetSize();
   allConUpperBound.SetSize(sz);
   allConUpperBound = copy.allConUpperBound;

   sz = (Integer) copy.decisionVecLowerBound.GetSize();
   decisionVecLowerBound.SetSize(sz);
   decisionVecLowerBound = copy.decisionVecLowerBound;

   sz = (Integer) copy.decisionVecUpperBound.GetSize();
   decisionVecUpperBound.SetSize(sz);
   decisionVecUpperBound = copy.decisionVecUpperBound;

   sz = (Integer) copy.decisionVecUpperBound.GetSize();
   decisionVecUpperBound.SetSize(sz);
   decisionVecUpperBound = copy.decisionVecUpperBound;

   userDynFunctionData = copy.userDynFunctionData; // clone pointers?
   userAlgFunctionData = copy.userAlgFunctionData; // clone pointers?
   costIntFunctionData = copy.costIntFunctionData; // clone pointers?
}

