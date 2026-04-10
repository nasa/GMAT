//------------------------------------------------------------------------------
//                              UserPointFunctionManager
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
// Created: 2016.07.14
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "csaltdefs.hpp"
#include "UserPointFunctionManager.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"
#include "DecisionVector.hpp"
#include "SparseMatrixUtil.hpp"
#include "Phase.hpp"
#include "RealUtilities.hpp"

//#define DEBUG_USER_FUNCTION
//#define DEBUG_USER_FUNCTION_EVALUATE
//#define DEBUG_COST
//#define DEBUG_DESTRUCT

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
UserPointFunctionManager::UserPointFunctionManager() :
   UserFunctionManager(),
   numBoundaryFunctions       (0),
   hasBoundaryFunctions       (false),
   optControlFuncsInitialized (false),
   numPhases                  (0),
   totalNumDecisionParams     (0),
   pfContainer                (NULL),
   userObject                 (NULL),
   boundJacobianData          (NULL),
   costJacobianData           (NULL),
   boundNLPUtil               (NULL),
   costNLPUtil                (NULL)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
UserPointFunctionManager::UserPointFunctionManager(const UserPointFunctionManager &copy) :
   UserFunctionManager(copy),
   numBoundaryFunctions       (copy.numBoundaryFunctions),
   hasBoundaryFunctions       (copy.hasBoundaryFunctions),
   optControlFuncsInitialized (copy.optControlFuncsInitialized),
   numPhases                  (copy.numPhases),
   totalNumDecisionParams     (copy.totalNumDecisionParams),
   pfContainer                (copy.pfContainer),  // Should be CLONED? or NULL?
   userObject                 (copy.userObject),   // Should be CLONED? or NULL?
   boundJacobianData          (NULL),
   costJacobianData           (NULL),
   boundNLPUtil               (NULL),
   costNLPUtil                (NULL)
{
   Integer r;
   r = copy.conUpperBound.GetSize();
   conUpperBound.SetSize(r);
   conUpperBound    = copy.conUpperBound;

   r = copy.conLowerBound.GetSize();
   conLowerBound.SetSize(r);
   conLowerBound    = copy.conLowerBound;

   phaseList        = copy.phaseList;  // or CLONES?
   
   decVecStartIdxs  = copy.decVecStartIdxs;
   
//   if (boundJacobianData) delete boundJacobianData;
//   boundJacobianData = copy.boundJacobianData;   // CLONE this!!! or NULL?????
//   
//   if (costJacobianData) delete costJacobianData;
//   costJacobianData = copy.costJacobianData;   // CLONE this!!!
//   
//   if (boundNLPUtil) delete boundNLPUtil;
//   boundNLPUtil = copy.boundNLPUtil;   // CLONE this!!!
//   
//   if (costNLPUtil) delete costNLPUtil;
//   costNLPUtil = copy.costNLPUtil;   // CLONE this!!!
   
   for (UnsignedInt ii = 0; ii < initialInputData.size(); ii++)
      if (initialInputData.at(ii))
         delete initialInputData.at(ii);
   initialInputData.clear();
   
   for (UnsignedInt ii = 0; ii < finalInputData.size(); ii++)
      if (finalInputData.at(ii))
         delete finalInputData.at(ii);  
   finalInputData.clear();
   
   initialInputData = copy.initialInputData;  // CLONES? <- or contents cloned?
   finalInputData   = copy.finalInputData;    // CLONES? <- or contents cloned?
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
UserPointFunctionManager& UserPointFunctionManager::operator=(
                                          const UserPointFunctionManager &copy)
{
   if (&copy == this)
      return *this;
   
   UserFunctionManager::operator=(copy);

   numBoundaryFunctions       = copy.numBoundaryFunctions;
   hasBoundaryFunctions       = copy.hasBoundaryFunctions;
   optControlFuncsInitialized = copy.optControlFuncsInitialized;
   numPhases                  = copy.numPhases;
   totalNumDecisionParams     = copy.totalNumDecisionParams;
   pfContainer                = copy.pfContainer;  // Should any be CLONED?
   userObject                 = copy.userObject;
   if (boundJacobianData) delete boundJacobianData;
   boundJacobianData          = copy.boundJacobianData;   // CLONE this!!! 
   if (costJacobianData)  delete costJacobianData;
   costJacobianData           = copy.costJacobianData;    // CLONE this!!!
   if (boundNLPUtil)      delete boundNLPUtil;
   boundNLPUtil               = copy.boundNLPUtil;        // CLONE this!!!
   if (costNLPUtil)       delete costNLPUtil;
   costNLPUtil                = copy.costNLPUtil;         // CLONE this!!!

   Integer r;
   r = copy.conUpperBound.GetSize();
   conUpperBound.SetSize(r);
   conUpperBound    = copy.conUpperBound;

   r = copy.conLowerBound.GetSize();
   conLowerBound.SetSize(r);
   conLowerBound    = copy.conLowerBound;

   phaseList        = copy.phaseList;  // or CLONES?
   
   decVecStartIdxs  = copy.decVecStartIdxs;
   
   initialInputData = copy.initialInputData;  // CLONES? or contents cloned?
   finalInputData   = copy.finalInputData;    // CLONES? or contents cloned?
  
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
UserPointFunctionManager::~UserPointFunctionManager()
{
   #ifdef DEBUG_DESTRUCT
      std::cout << "In UPointFM destructor ..." << std::endl;
   #endif
   
   if (boundJacobianData)  delete boundJacobianData;
   if (costJacobianData)   delete costJacobianData;
   
   #ifdef DEBUG_DESTRUCT
      std::cout << "In UPointFM destructor ... jacobian data deleted" << std::endl;
   #endif
   
   if (boundNLPUtil)       delete boundNLPUtil;
   if (costNLPUtil)        delete costNLPUtil;
   
   #ifdef DEBUG_DESTRUCT
      std::cout << "In UPointFM destructor ... nlputils deleted" << std::endl;
   #endif
   
   if (!initialInputData.empty())
   {
   for (UnsignedInt ii = 0; ii < initialInputData.size(); ii++)
      if (initialInputData.at(ii))
         delete initialInputData.at(ii);
   }
   #ifdef DEBUG_DESTRUCT
      std::cout << "In UPointFM destructor ... initialInputData" << std::endl;
   #endif
   if (!finalInputData.empty())
   {
   for (UnsignedInt ii = 0; ii < finalInputData.size(); ii++)
      if (finalInputData.at(ii))
         delete finalInputData.at(ii);
   }
   #ifdef DEBUG_DESTRUCT
      std::cout << "In UPointFM destructor ... finalInputData" << std::endl;
   #endif
   
   // assume pfContainer, phaseList do not need to be deleted here
}

//------------------------------------------------------------------------------
// void Initialize(UserPointFunction          *uData,
//                 const std::vector<Phase*>  &pList,
//                 Integer                    numDecParams,
//                 const IntegerArray         &startIdxs)
//------------------------------------------------------------------------------
/**
 * This method initializes the point function manager
 *
 * @param <uData>        the input user point function
 * @param <pList>        the input array of phase pointers
 * @param <numDecParams> the input number of decision parameters
 * @param <startIdxs>    the input array of start indexes for the phases
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::Initialize(
                                    UserPointFunction          *uData,
                                    const std::vector<Phase*>  &pList,
                                    Integer                    numDecParams,
                                    const IntegerArray         &startIdxs)
{
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                  "In UPFM::Initialize ... numPhases = %d, uData is %sNULL \n",
                  numPhases, (uData? "NOT " : ""));
      std::cout << "In UPFM::Initialize ...\n";
   #endif
   if (!uData)
   {
      hasFunction = false;
      return;
   }
   hasFunction = true;
      
   // Set pointers to input objects
   decVecStartIdxs        = startIdxs;
   totalNumDecisionParams = numDecParams;
   phaseList              = pList;
   numPhases              = phaseList.size();
   userObject             = uData;

   // Create JacobianData objects
   if (boundJacobianData)
      delete boundJacobianData;
   boundJacobianData      = new JacobianData();
   if (costJacobianData)
      delete costJacobianData;
   costJacobianData       = new JacobianData();
   
   // Create MultiPoint utilities
   if (boundNLPUtil)
      delete boundNLPUtil;
   boundNLPUtil           = new NLPFuncUtil_MultiPoint();
   if (costNLPUtil)
      delete costNLPUtil;
   costNLPUtil            = new NLPFuncUtil_MultiPoint();
   
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                       "In UPFM::Initialize ... Multipoint utilities created ...\n");
      std::cout << "In UPFM::Initialize ... Multipoint utilities created ...\n";
   #endif

   // Call the function, then determine what was populated and set
   // properties accordingly.
   InitializeInputData(); // print something here??
   
   PrepareInputData();
   InitializeUserObject();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                "In UPFM::Initialize ... calling EvaluateUserFunction ...\n");
      std::cout << "In UPFM::Initialize ... calling EvaluateUserFunction ...\n";
   #endif
   
   EvaluateUserFunction();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                  "In UPFM::Initialize ... DONE calling EvaluateUserFunction ...\n");
      std::cout << "In UPFM::Initialize ... DONE calling EvaluateUserFunction ...\n";
   #endif
   InitializeUserFunctionProperties();
   boundJacobianData->Initialize(numBoundaryFunctions,hasBoundaryFunctions,
                                 phaseList,decVecStartIdxs);
   Integer numFunctions = 0;
   if (hasCostFunction)
       numFunctions = 1;
   
   costJacobianData->Initialize(numFunctions,hasCostFunction,phaseList,
                                decVecStartIdxs);
   // EvaluateUserJacobian();
   
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                           "    **** calling ComputeSparsityPatterns ... \n");
   #endif
   ComputeSparsityPatterns();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                     "    **** calling DetermineFunctionDependencies ... \n");
   #endif
   DetermineFunctionDependencies();
   isInitializing = false;
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                           "    **** calling InitializeBoundNLPUtil ... \n");
   #endif
   InitializeBoundNLPUtil();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                              "    **** calling InitializeCostNLPUtil ... \n");
   #endif
   if (hasCostFunction)
      InitializeCostNLPUtil();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("LEAVING UPFM::Initialize\n");
   #endif
}

//------------------------------------------------------------------------------
// void InitializeUserFunctionProperties()
//------------------------------------------------------------------------------
/**
 * This method initializes the function flags for the manager
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::InitializeUserFunctionProperties()
{
   // Initializes basic properties based on user probelm
   hasCostFunction      = pfContainer->GetCostData()->HasUserFunction();
   hasBoundaryFunctions = pfContainer->GetAlgData()->HasUserFunction();
   numBoundaryFunctions = pfContainer->GetAlgData()->GetNumFunctions();
}

//------------------------------------------------------------------------------
// void InitializeUserObject()
//------------------------------------------------------------------------------
/**
 * This method initializes the user object
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::InitializeUserObject()
{
   // Initializes the users point function object
   userObject->Initialize(initialInputData,finalInputData);
}

//------------------------------------------------------------------------------
// void InitializeInputData()
//------------------------------------------------------------------------------
/**
 * This method creates and initializes the input data objects
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::InitializeInputData()
{
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                        "In UPFM::InitializeInputData, numPhases = %d ... \n",
                        numPhases);
      std::cout << "In UPFM::InitializeInputData\n";
   #endif
   // Creates and intializes input data objects
   for (Integer ii = 0; ii < initialInputData.size(); ++ii)
      if (initialInputData.at(ii))
         delete (initialInputData.at(ii));
   for (Integer ii = 0; ii < finalInputData.size(); ++ii)
      if (finalInputData.at(ii))
         delete (finalInputData.at(ii));

   initialInputData.clear();
   finalInputData.clear();
   optControlFuncVec.clear();

   //optControlFuncVec = userObject->GetAlgFunctionObjects();

   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      // TODO:  Add config to class to avoid call to phase
      Phase *currentPhase = phaseList.at(phaseIdx);
      numStateVars        = currentPhase->GetNumStateVars();
      numControlVars      = currentPhase->GetNumControlVars();
      numStaticVars       = currentPhase->GetNumStaticVars();
      Integer pNum        = currentPhase->GetPhaseNumber();
      
      FunctionInputData *newInputInit = new FunctionInputData();
      newInputInit->Initialize(numStateVars, numControlVars, numStaticVars);
      newInputInit->SetPhaseNum(pNum);
      initialInputData.push_back(newInputInit);
      
      FunctionInputData *newInputFinal = new FunctionInputData();
      newInputFinal->Initialize(numStateVars, numControlVars, numStaticVars);
      newInputInit->SetPhaseNum(pNum);
      finalInputData.push_back(newInputFinal);
   }
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("EXITING UPFM::InitializeInputData\n");
      std::cout << "EXITING UPFM::InitializeInputData\n";
   #endif
}

//------------------------------------------------------------------------------
// bool HasBoundaryFunctions()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not this manager has 
 * boundary functions
 *
 * @return  true of it has boundary functions; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool UserPointFunctionManager::HasBoundaryFunctions()
{
   return hasBoundaryFunctions;
}

//------------------------------------------------------------------------------
// Integer GetNumberBoundaryFunctions()
//------------------------------------------------------------------------------
/**
 * This method returns the number of boundary functions
 *
 * @return the number of boundary functions
 *
 */
//------------------------------------------------------------------------------
Integer UserPointFunctionManager::GetNumberBoundaryFunctions()
{
   return numBoundaryFunctions;
}


//------------------------------------------------------------------------------
// StringArray GetFunctionNames()
//------------------------------------------------------------------------------
/**
 * This method returns an array of the function names
 *
 * @return an array of function names
 *
 */
//------------------------------------------------------------------------------
StringArray UserPointFunctionManager::GetFunctionNames()
{
   FunctionOutputData *alg = pfContainer->GetAlgData();
   return alg->GetFunctionNames();
}


//------------------------------------------------------------------------------
// void PrepareInputData()
//------------------------------------------------------------------------------
/**
 * This method prepares the input data
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::PrepareInputData()
{
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                  "In UPFM::PrepareInputData ... numPhases (size of phaseList) "
                  "= %d (%d), size of initialInputData = %d, size of "
                  "finalInputData = %d \n",
                  numPhases, (Integer)phaseList.size(),
                  (Integer) initialInputData.size(),
                  (Integer) finalInputData.size());
      std::cout << "In UPFM::PrepareInputData ...\n";
   #endif
   // Updates state, time, etc. of input data objects
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      Phase          *currentPhase = phaseList.at(phaseIdx);
      DecisionVector *dv           = currentPhase->GetDecisionVector();
      #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage(
                     "      setting state and time for initialInputData(%d)\n",
                     phaseIdx);
      #endif
      // Data at initial point
      initialInputData.at(phaseIdx)->SetStateVector(dv->GetFirstStateVector());
      initialInputData.at(phaseIdx)->SetTime(dv->GetFirstTime());
      // Data at final point
      #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage(
                        "      setting state and time for finalInputData(%d)\n",
                        phaseIdx);
      #endif
      finalInputData.at(phaseIdx)->SetStateVector(dv->GetLastStateVector());
      finalInputData.at(phaseIdx)->SetTime(dv->GetLastTime());

      if (numStaticVars > 0)
      {
         // static parameters are same at the initial and final points but
         // we put the same data into 2 places in accordance with the convention 
         initialInputData.at(phaseIdx)->SetStaticVector(dv->GetStaticVector());
         finalInputData.at(phaseIdx)->SetStaticVector(dv->GetStaticVector());
      }
   }
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("Exiting UPFM::PrepareInputData ... \n");
      std::cout << "Exiting UPFM::PrepareInputData ... \n";
   #endif
}

//------------------------------------------------------------------------------
// void EvaluateUserJacobian()
//------------------------------------------------------------------------------
/**
 * This method evaluates the user jacobian
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::EvaluateUserJacobian()
{
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                              "Entering UPFM::EvaluateUserJacobian ... \n");
   #endif

   if (!optControlFuncsInitialized)
   {
      optControlFuncVec = userObject->GetOptControlFunctionObjects();
      optControlFuncsInitialized = true;
   }

   // Adds in missing partials
   // Evaluate at the nominal point before finite differencing
   EvaluateUserFunction();
   ComputeStateJacobian();
   ComputeTimeJacobian();

   // YK mod static params
   if (numStaticVars > 0)
      ComputeStaticJacobian();
   // userClass->EvaluateUserJacobian();
   //}
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                              "LEAVING UPFM::EvaluateUserJacobian ... \n");
   #endif
}

//------------------------------------------------------------------------------
// void EvaluateUserFunction()
//------------------------------------------------------------------------------
/**
 * This method evaluates the user function but does not add in mission partials
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::EvaluateUserFunction()
{
   #ifdef DEBUG_USER_FUNCTION_EVALUATE
      MessageInterface::ShowMessage("In UPFM::EvaluateUserFunction ... \n");
      MessageInterface::ShowMessage("  userObject is %sNULL!!!\n",
                                    (userObject? " NOT " : ""));
   #endif

   try
   {
       PrepareInputData();
       userObject->EvaluateUserFunction();
       pfContainer = userObject->GetFunctionData();
   }
   catch (BaseException &be)
   {
      std::string errmsg = "For UserPointFunctionManager::";
      errmsg += "EvaluateUserFunction, there was an error\n";
      errmsg += be.GetDetails();
      throw LowThrustException(errmsg);
   }
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                              "EXITING UPFM::EvaluateUserFunction ... \n");
   #endif
}

//------------------------------------------------------------------------------
// void EvaluatePreparedUserFunction()
//------------------------------------------------------------------------------
/**
 * This method evaluates the prepared user function(s) assuming data is
 * preconfigured.  Used for Jacobian evaluation especially, as perts are set
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::EvaluatePreparedUserFunction()
{
   try
   {
      #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage(
                     "   CALLING the userObject EvaluateUserFunction *** \n");
      #endif
      userObject->EvaluateUserFunction();
      #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage("   about to set pfContainer ... \n");
      #endif
      pfContainer = userObject->GetFunctionData();
   }
   catch (BaseException &be)
   {
      std::string errmsg = "For UserPointFunctionManager::";
      errmsg += "EvaluatePreparedUserFunction, there was an error.\n" +
         be.GetDetails();
      throw LowThrustException(errmsg);
   }
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                           "EXITING UPFM::EvaluatePreparedUserFunction ... \n");
   #endif
}


//------------------------------------------------------------------------------
// Rvector GetConLowerBound()
//------------------------------------------------------------------------------
/**
 * This method returns the lower bounds on the bound functions
 *
 * @return lower bounds on the bound functions
 *
 */
//------------------------------------------------------------------------------
Rvector UserPointFunctionManager::GetConLowerBound()
{
   FunctionOutputData *alg = pfContainer->GetAlgData();
   return alg->GetLowerBounds();
}

//------------------------------------------------------------------------------
// Rvector GetConUpperBound()
//------------------------------------------------------------------------------
/**
 * This method returns the upper bounds on the bound functions
 *
 * @return  upper bounds on bound functions
 *
 */
//------------------------------------------------------------------------------
Rvector UserPointFunctionManager::GetConUpperBound()
{
   FunctionOutputData *alg = pfContainer->GetAlgData();
   return alg->GetUpperBounds();
}

//------------------------------------------------------------------------------
// Rvector GetStateLowerBound(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the lower bound on the state vector
 *
 * @return  lower bound on state vector
 *
 */
//------------------------------------------------------------------------------
Rvector UserPointFunctionManager::GetStateLowerBound(Integer phaseIdx)
{
   if ((phaseIdx < 0) || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds!!\n");
   return phaseList.at(phaseIdx)->GetStateLowerBound();
}

//------------------------------------------------------------------------------
// Rvector GetStateUpperBound(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the upper bound on the state vector
 *
 * @return  upper bound on state vector
 *
 */
//------------------------------------------------------------------------------
Rvector UserPointFunctionManager::GetStateUpperBound(Integer phaseIdx)
{
   if ((phaseIdx < 0) || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds!!\n");
   // Returns upper bound on the state vector
   return phaseList.at(phaseIdx)->GetStateUpperBound();
}

//------------------------------------------------------------------------------
// Rvector GetStaticLowerBound(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
* This method returns the lower bound on the Static vector
*
* @return  lower bound on Static vector
*
*/
//------------------------------------------------------------------------------
Rvector UserPointFunctionManager::GetStaticLowerBound(Integer phaseIdx)
{
   if ((phaseIdx < 0) || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds!!\n");
   return phaseList.at(phaseIdx)->GetStaticLowerBound();
}

//------------------------------------------------------------------------------
// Rvector GetStaticUpperBound(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
* This method returns the upper bound on the Static vector
*
* @return  upper bound on Static vector
*
*/
//------------------------------------------------------------------------------
Rvector UserPointFunctionManager::GetStaticUpperBound(Integer phaseIdx)
{
   if ((phaseIdx < 0) || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds!!\n");
   // Returns upper bound on the Static vector
   return phaseList.at(phaseIdx)->GetStaticUpperBound();
}

//------------------------------------------------------------------------------
// Rvector GetTimeLowerBound(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the lower bound on the time vector
 *
 * @return  lower bound on time vector
 *
 */
//------------------------------------------------------------------------------
Real UserPointFunctionManager::GetTimeLowerBound(Integer phaseIdx)
{
   if ((phaseIdx < 0) || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds!!\n");
   // Returns upper bound on the time vector
   return phaseList.at(phaseIdx)->GetTimeLowerBound();
}

//------------------------------------------------------------------------------
// Rvector GetTimeUpperBound(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the upper bound on the time vector
 *
 * @return  upper bound on time vector
 *
 */
//------------------------------------------------------------------------------
Real UserPointFunctionManager::GetTimeUpperBound(Integer phaseIdx)
{
   if ((phaseIdx < 0) || (phaseIdx >= numPhases))
      throw LowThrustException("Phase index out-of-bounds!!\n");
   // Returns upper bound on the time vector
   return phaseList.at(phaseIdx)->GetTimeUpperBound();
}


//------------------------------------------------------------------------------
// Rvector ComputeBoundNLPFunctions()
//------------------------------------------------------------------------------
/**
 * This method computes the bound function values
 *
 * @return  bound function values
 *
 */
//------------------------------------------------------------------------------
Rvector UserPointFunctionManager::ComputeBoundNLPFunctions()
{
   #ifdef DEBUG_USER_FUNCTION_EVALUATE
      MessageInterface::ShowMessage("In ComputeBoundNLPFunctions\n");
   #endif
   // returns bound function values
   EvaluateUserFunction();
   Rvector  f;
   RSMatrix jac;
   FunctionOutputData *out = pfContainer->GetAlgData();
   boundNLPUtil->FillUserNLPMatrices(*out, boundJacobianData, f, jac);
   boundNLPUtil->ComputeFuncAndJac(*out, boundJacobianData, f, jac);
   return f;
}

//------------------------------------------------------------------------------
// RSMatrix ComputeBoundNLPJacobian()
//------------------------------------------------------------------------------
/**
 * This method computes the bound function jacobian
 *
 * @return  bound function jacobian
 *
 */
//------------------------------------------------------------------------------
RSMatrix UserPointFunctionManager::ComputeBoundNLPJacobian()
{
   Rvector  f;
   RSMatrix jac;
   FunctionOutputData *out = pfContainer->GetAlgData();
   boundNLPUtil->FillUserNLPMatrices(*out, boundJacobianData, f, jac);
   boundNLPUtil->ComputeFuncAndJac(*out, boundJacobianData, f, jac);
   return jac;
}

//------------------------------------------------------------------------------
// RSMatrix* ComputeBoundNLPSparsityPattern()
//------------------------------------------------------------------------------
/**
 * This method computes the bound sparsity pattern
 *
 * @return  bound sparsity pattern
 *
 */
//------------------------------------------------------------------------------
RSMatrix* UserPointFunctionManager::ComputeBoundNLPSparsityPattern()
{
   // YK mod so that it computes sparsity pattern
   RSMatrix* jac;
   jac = boundNLPUtil->ComputeSparsityPattern();
   return jac;
}


//------------------------------------------------------------------------------
// Rvector ComputeCostNLPFunctions()
//------------------------------------------------------------------------------
/**
 * This method computes the cost function value
 *
 * @return  cost function value
 *
 */
//------------------------------------------------------------------------------
Rvector UserPointFunctionManager::ComputeCostNLPFunctions()
{
   // Returns cost function value
   EvaluateUserFunction();
   Rvector  f;
   RSMatrix jac;
   FunctionOutputData *out = pfContainer->GetCostData();
   #ifdef DEBUG_COST
      Rvector c = out->GetFunctionValues();
      MessageInterface::ShowMessage(
                        "cost function values = %s\n", c.ToString(12).c_str());
   #endif
   costNLPUtil->FillUserNLPMatrices(*out, costJacobianData, f, jac);
   costNLPUtil->ComputeFuncAndJac(*out, costJacobianData, f, jac);
   #ifdef DEBUG_COST
      MessageInterface::ShowMessage(
                     "cost function values (f) = %s\n", f.ToString(12).c_str());
   #endif
   return f;
}

//------------------------------------------------------------------------------
// RSMatrix ComputeCostNLPJacobian()
//------------------------------------------------------------------------------
/**
 * This method computes the cost function jacobian
 *
 * @return  cost function jacobian
 *
 */
//------------------------------------------------------------------------------
RSMatrix UserPointFunctionManager::ComputeCostNLPJacobian()
{
   Rvector  f;
   RSMatrix jac;
   FunctionOutputData *out = pfContainer->GetCostData();
   costNLPUtil->FillUserNLPMatrices(*out, costJacobianData, f, jac);
   costNLPUtil->ComputeFuncAndJac(*out, costJacobianData, f, jac);
   return jac;
}

//------------------------------------------------------------------------------
// RSMatrix* ComputeCostNLPSparsityPattern()
//------------------------------------------------------------------------------
/**
 * This method computes the cost sparsity pattern
 *
 * @return  cost sparsity pattern
 *
 */
//------------------------------------------------------------------------------
RSMatrix* UserPointFunctionManager::ComputeCostNLPSparsityPattern()
{
   // YK mod so that it computes sparsity pattern
   RSMatrix* jacPattern;
   jacPattern = costNLPUtil->ComputeSparsityPattern();
   return jacPattern;
}


//------------------------------------------------------------------------------
// void ComputeSparsityPatterns()
//------------------------------------------------------------------------------
/**
 * This method computes the function's pattern w/r/t problem params
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::ComputeSparsityPatterns()
{
   // Compute the function's pattern w/r/t problem params
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("Entering UPFM::ComputeSparsityPatterns\n");
   #endif
   
   if (!optControlFuncsInitialized)
   {
      optControlFuncVec = userObject->GetOptControlFunctionObjects();
      optControlFuncsInitialized = true;
   }

   /* Mod by YK
   // the original code passing over dummy values. 
   // Fix it to use time bounds of each phases
   Real upper, lower;
   ComputeTimeSparsity(upper, lower);
   ComputeStateSparsity();
   */
   ComputeTimeSparsity();
   ComputeStateSparsity();
   
   // YK mod static params
   if (numStaticVars > 0)
      ComputeStaticSparsity();

   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("EXITING UPFM::ComputeSparsityPatterns\n");
   #endif
}


// --- should these be protected? ---
//------------------------------------------------------------------------------
// void ComputeTimeJacobian()
//------------------------------------------------------------------------------
/**
 * This method computes the time jacobian
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::ComputeTimeJacobian()
{
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("Entering UPFM::ComputeTimeJacobian\n");
   #endif
   // Save nominal values for later use
   PrepareInputData();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("--- calling EPUF\n");
   #endif
   EvaluatePreparedUserFunction();
   Rvector nominalCostValue;
   Rvector nominalBoundValues;
   Real    nominalTime;
   Real deltaTime = userObject->GetTimePerturbation();

   Integer timeIdx   = 0;
   
   if (hasCostFunction)
       nominalCostValue = pfContainer->GetCostData()->GetFunctionValues();

   if (hasBoundaryFunctions)
       nominalBoundValues = pfContainer->GetAlgData()->GetFunctionValues();
   
   userObject->EvaluateUserJacobian();

   // Create arrays to store the Jacobians for each phase and an array to
   // represent the positions along the phase for each Jacobian
   std::vector<std::vector<Rmatrix>> phaseJacs;
   phaseJacs.resize(numPhases);
   std::vector<std::vector<std::string>> phasePos;
   phasePos.resize(numPhases);

   FillJacobianRows(OptimalControlFunction::TIME, phaseJacs, phasePos);

   Integer numAlgFuncs = 0;
   for (Integer ii = 0; ii < optControlFuncVec.size(); ++ii)
      numAlgFuncs += optControlFuncVec.at(ii)->GetNumFunctions();

   // Check if any point functions were created through a point function
   // object rather than an optimal control function object
   if (numAlgFuncs < numBoundaryFunctions)
   {
      for (Integer phaseIdx = 0; phaseIdx < phaseJacs.size(); ++phaseIdx)
      {
         // Initial point
         nominalTime = initialInputData.at(phaseIdx)->GetTime();
         initialInputData.at(phaseIdx)->SetTime(nominalTime + deltaTime);

         EvaluatePreparedUserFunction();

         Rvector jacValue = (pfContainer->GetAlgData()->GetFunctionValues() -
            nominalBoundValues) / deltaTime;

         for (Integer ii = 0; ii < (numBoundaryFunctions - numAlgFuncs); ++ii)
            phaseJacs.at(phaseIdx).at(0)(ii, 0) = jacValue(ii);

         initialInputData.at(phaseIdx)->SetTime(nominalTime);
      }

      for (Integer phaseIdx = 0; phaseIdx < phaseJacs.size(); ++phaseIdx)
      {
         // Final point
         nominalTime = finalInputData.at(phaseIdx)->GetTime();
         finalInputData.at(phaseIdx)->SetTime(nominalTime + deltaTime);

         EvaluatePreparedUserFunction();

         Rvector jacValue2 = (pfContainer->GetAlgData()->GetFunctionValues() -
            nominalBoundValues) / deltaTime;

         for (Integer ii = 0; ii < (numBoundaryFunctions - numAlgFuncs); ++ii)
            phaseJacs.at(phaseIdx).at(1)(ii, 0) = jacValue2(ii);

         finalInputData.at(phaseIdx)->SetTime(nominalTime);
      }
   }

   // Loop through each element in the Jacobian and set them to the
   // Jacobian data objects
   for (Integer ii = 0; ii < phaseJacs.size(); ++ii)
   {
      for (Integer jj = 0; jj < phaseJacs.at(ii).size(); ++jj)
      {
         #ifdef DEBUG_USER_FUNCTION
            if (phasePos.at(ii).at(jj) == "Initial")
            {
               MessageInterface::ShowMessage("Initial Bound Time Jacobian "
                  "Size = (%d, %d)\n", phaseJacs.at(ii).at(jj).GetNumRows(),
                  phaseJacs.at(ii).at(jj).GetNumColumns());
               MessageInterface::ShowMessage("Initial Bound Time Jacobian:\n");
            }
            else
            {
               MessageInterface::ShowMessage("Final Bound Time Jacobian "
                  "Size = (%d, %d)\n", phaseJacs.at(ii).at(jj).GetNumRows(),
                  phaseJacs.at(ii).at(jj).GetNumColumns());
               MessageInterface::ShowMessage("Final Bound Time Jacobian:\n");
            }

            for (Integer i = 0; i < phaseJacs.at(ii).at(jj).GetNumRows(); ++i)
            {
               MessageInterface::ShowMessage("%s \n",
                  phaseJacs.at(ii).at(jj).GetRow(i).ToString().c_str());
            }
            MessageInterface::ShowMessage("\n");
         #endif

         for (Integer rowIdx = 0;
            rowIdx < phaseJacs.at(ii).at(jj).GetNumRows(); ++rowIdx)
         {
            for (Integer colIdx = 0;
               colIdx < phaseJacs.at(ii).at(jj).GetNumColumns(); ++colIdx)
            {
               if (phasePos.at(ii).at(jj) == "Initial")
                  boundJacobianData->SetInitTimeJacobian(ii, rowIdx,
                  colIdx, phaseJacs.at(ii).at(jj)(rowIdx, colIdx));
               else if (phasePos.at(ii).at(jj) == "Final")
                  boundJacobianData->SetFinalTimeJacobian(ii, rowIdx,
                  colIdx, phaseJacs.at(ii).at(jj)(rowIdx, colIdx));
            }
         }
      }
   }
   
   // Loop over the phases to compute Jacobians w/r/t initial time
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      // Perturb the Time and recompute user functions
      nominalTime    = initialInputData.at(phaseIdx)->GetTime();
      initialInputData.at(phaseIdx)->SetTime(nominalTime + deltaTime);
      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage("--- calling EPUF (2)\n");
      #endif
      EvaluatePreparedUserFunction();
       
      // Compute the Jacobian for initial time
      if (hasCostFunction)
      {
          Rvector jacValue2 = (pfContainer->GetCostData()->GetFunctionValues() -
                               nominalCostValue)/deltaTime;
 
         costJacobianData->SetInitTimeJacobian(phaseIdx, 0, timeIdx,
                                               jacValue2(0));
      }
       
      // Set time back to nominal value and evaluate.
      initialInputData.at(phaseIdx)->SetTime(nominalTime);     
   }
     
   // Loop over the phases to compute Jacobians w/r/t final time
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {      
      // Perturb the Time and recompute user functions
       nominalTime = finalInputData.at(phaseIdx)->GetTime();
       finalInputData.at(phaseIdx)->SetTime(nominalTime + deltaTime);
      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage("--- calling EPUF(3)\n");
      #endif
       EvaluatePreparedUserFunction();
       
      // Compute the Jacobian for final time
      if (hasCostFunction)
      {
           Rvector jacValue4 = (pfContainer->GetCostData()->GetFunctionValues()
                                - nominalCostValue)/deltaTime;

           costJacobianData->SetFinalTimeJacobian(phaseIdx, 0, timeIdx,
                                                  jacValue4(0));
      }
       
      // Set time back to nominal value and evaluate.
      finalInputData.at(phaseIdx)->SetTime(nominalTime);
       
   }
   // This computes the correct analytic partials (for those provided)
   // and avoids noise due to finite of constraints that required it.
   PrepareInputData();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("--- calling EPUF(4)\n");
   #endif
   EvaluatePreparedUserFunction();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("LEAVING UPFM::ComputeTimeJacobian\n");
   #endif
}

//------------------------------------------------------------------------------
// void ComputeStateJacobian()
//------------------------------------------------------------------------------
/**
 * This method computes the jacobian with respect to state
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::ComputeStateJacobian()
{
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("In UPFM::ComputeStateJacobian ... \n");
   #endif
   
   // Save nominal values for later use
   PrepareInputData();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("In UPFM::ComputeStateJacobian, done with "
                                    "PrepareInputData \n");
   #endif

   EvaluatePreparedUserFunction();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                        "In UPFM::ComputeStateJacobian, done with "
                        "EvaluatePreparedUserFunction \n");
   #endif
   Rvector nominalCostValue;
   Rvector nominalBoundValues;
   Real    deltaVec = userObject->GetStatePerturbation();
   
   if (hasCostFunction)
      nominalCostValue = pfContainer->GetCostData()->GetFunctionValues();

   if (hasBoundaryFunctions)
      nominalBoundValues = pfContainer->GetAlgData()->GetFunctionValues();
   
   userObject->EvaluateUserJacobian();

   std::vector<std::vector<Rmatrix>> phaseJacs;
   phaseJacs.resize(numPhases);
   std::vector<std::vector<std::string>> phasePos;
   phasePos.resize(numPhases);

   FillJacobianRows(OptimalControlFunction::STATE, phaseJacs, phasePos);

   Integer numAlgFuncs = 0;
   for (Integer ii = 0; ii < optControlFuncVec.size(); ++ii)
      numAlgFuncs += optControlFuncVec.at(ii)->GetNumFunctions();

   // Check if any point functions were created through a point function
   // object rather than an optimal control function object
   if (numAlgFuncs < numBoundaryFunctions)
   {
      for (Integer phaseIdx = 0; phaseIdx < phaseJacs.size(); ++phaseIdx)
      {
         // Initial point
         Integer numStVars =
            initialInputData.at(phaseIdx)->GetNumStateVars();
         Rvector nominalStateVector =
            initialInputData.at(phaseIdx)->GetStateVector();

         Rvector pertVec(numStVars);
         for (Integer ii = 0; ii < numStVars; ii++)
            pertVec(ii) = deltaVec;

         for (Integer stateIdx = 0; stateIdx < numStateVars; stateIdx++)
         {
            // Perturb the state and recompute user functions
            Rvector deltaStateVec(numStateVars);          // or numStVars????
            deltaStateVec(stateIdx) = pertVec(stateIdx);  // set the one element
            initialInputData.at(phaseIdx)->SetStateVector(nominalStateVector +
               deltaStateVec);
            EvaluatePreparedUserFunction();

            Rvector jacValue = (pfContainer->GetAlgData()->GetFunctionValues()
               - nominalBoundValues) / pertVec(stateIdx);

            for (Integer ii = 0; ii < (numBoundaryFunctions - numAlgFuncs);
               ++ii)
               phaseJacs.at(phaseIdx).at(0)(ii, stateIdx) = jacValue(ii);
         }

         initialInputData.at(phaseIdx)->SetStateVector(nominalStateVector);
      }

      for (Integer phaseIdx = 0; phaseIdx < phaseJacs.size(); ++phaseIdx)
      {
         // Final point
         Integer numStVars =
            finalInputData.at(phaseIdx)->GetNumStateVars();
         Rvector nominalStateVector =
            finalInputData.at(phaseIdx)->GetStateVector();

         Rvector pertVec(numStVars);
         for (Integer ii = 0; ii < numStVars; ii++)
            pertVec(ii) = deltaVec;

         for (Integer stateIdx = 0; stateIdx < numStateVars; stateIdx++)
         {
            // Perturb the state and recompute user functions
            Rvector deltaStateVec(numStateVars);          // or numStVars????
            deltaStateVec(stateIdx) = pertVec(stateIdx);  // set the one element
            finalInputData.at(phaseIdx)->SetStateVector(nominalStateVector +
               deltaStateVec);
            EvaluatePreparedUserFunction();

            Rvector jacValue = (pfContainer->GetAlgData()->GetFunctionValues()
               - nominalBoundValues) / pertVec(stateIdx);

            for (Integer ii = 0; ii < (numBoundaryFunctions - numAlgFuncs);
               ++ii)
               phaseJacs.at(phaseIdx).at(1)(ii, stateIdx) = jacValue(ii);
         }

         finalInputData.at(phaseIdx)->SetStateVector(nominalStateVector);
      }
   }

   for (Integer ii = 0; ii < phaseJacs.size(); ++ii)
   {
      for (Integer jj = 0; jj < phaseJacs.at(ii).size(); ++jj)
      {
         #ifdef DEBUG_USER_FUNCTION
            if (phasePos.at(ii).at(jj) == "Initial")
            {
               MessageInterface::ShowMessage("Initial Bound State Jacobian "
                  "Size = (%d, %d)\n", phaseJacs.at(ii).at(jj).GetNumRows(),
                  phaseJacs.at(ii).at(jj).GetNumColumns());
               MessageInterface::ShowMessage("Initial Bound State Jacobian:\n");
            }
            else
            {
               MessageInterface::ShowMessage("Final Bound State Jacobian "
                  "Size = (%d, %d)\n", phaseJacs.at(ii).at(jj).GetNumRows(),
                  phaseJacs.at(ii).at(jj).GetNumColumns());
               MessageInterface::ShowMessage("Final Bound State Jacobian:\n");
            }

            for (Integer i = 0; i < phaseJacs.at(ii).at(jj).GetNumRows(); ++i)
            {
               MessageInterface::ShowMessage("%s \n",
                  phaseJacs.at(ii).at(jj).GetRow(i).ToString().c_str());
            }
            MessageInterface::ShowMessage("\n");
         #endif

         for (Integer rowIdx = 0;
            rowIdx < phaseJacs.at(ii).at(jj).GetNumRows(); ++rowIdx)
         {
            for (Integer colIdx = 0;
               colIdx < phaseJacs.at(ii).at(jj).GetNumColumns(); ++colIdx)
            {
               if (phasePos.at(ii).at(jj) == "Initial")
                  boundJacobianData->SetInitStateJacobian(ii, rowIdx,
                     colIdx, phaseJacs.at(ii).at(jj)(rowIdx, colIdx));
               else if (phasePos.at(ii).at(jj) == "Final")
                  boundJacobianData->SetFinalStateJacobian(ii, rowIdx,
                     colIdx, phaseJacs.at(ii).at(jj)(rowIdx, colIdx));
            }
         }
      }
   }
   

   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
               "In UPFM::ComputeStateJacobian, about to loop over phases \n");
      MessageInterface::ShowMessage(
               "   numPhases = %d, size of initialInputData = %d, size of "
               "finalInputData = %d\n",
               numPhases, (Integer) initialInputData.size(),
                                    finalInputData.size());
   #endif
   // Loop over the phases to compute Jacobians w/r/t initial state
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      // Loop initializations for phase dependent data
      Integer numStVars          =
                           initialInputData.at(phaseIdx)->GetNumStateVars();
      Rvector nominalStateVector =
                           initialInputData.at(phaseIdx)->GetStateVector();

      Rvector pertVec(numStVars);
      for (Integer ii = 0; ii < numStVars; ii++)
         pertVec(ii) = deltaVec;

      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage("--- numStVars           = %d \n",
                                       numStVars);
         MessageInterface::ShowMessage("   size of state vector = %d \n",
                                       nominalStateVector.GetSize());
         MessageInterface::ShowMessage("--- nominalStateVector  = %s \n",
                                       nominalStateVector.ToString(12).c_str());
      #endif
      // Loop over state values and perturb them
      // or use numStVars here????
      for (Integer stateIdx = 0; stateIdx < numStateVars; stateIdx++)
      {          
         // Perturb the state and recompute user functions
         Rvector deltaStateVec(numStateVars);          // or numStVars????
         deltaStateVec(stateIdx) = pertVec(stateIdx);  // set the one element
         initialInputData.at(phaseIdx)->SetStateVector(nominalStateVector +
                                                       deltaStateVec);
         EvaluatePreparedUserFunction();
           
         #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage("--- hasBoundaryFunction = %s \n",
                                    (hasBoundaryFunctions? "true" : "false"));
            MessageInterface::ShowMessage("--- hasCostFunction = %s \n",
                                    (hasCostFunction? "true" : "false"));
         #endif
         // Compute and save current row of Jacobian
         // TODO: Fix to test if user has provided Jacobain on a
         // phase level.
         if (hasCostFunction)
         {
            Rvector costVals = pfContainer->GetCostData()->GetFunctionValues();
            #ifdef DEBUG_USER_FUNCTION
               MessageInterface::ShowMessage(
                                    "cost function values (size %d)  = %s\n",
                                    costVals.GetSize(),
                                    costVals.ToString(12).c_str());
               MessageInterface::ShowMessage(
                                    "nominal cost function (size %d) = %s\n",
                                    nominalCostValue.GetSize(),
                                    nominalCostValue.ToString(12).c_str());
               MessageInterface::ShowMessage("pertVec(stateIdx) = %12.10f\n",
                                             pertVec(stateIdx));
            #endif
            Rvector jacValue2(1);
            jacValue2(0) = (costVals(0) -
                            nominalCostValue(0))/pertVec(stateIdx);

            #ifdef DEBUG_USER_FUNCTION
               MessageInterface::ShowMessage("--- jacValue2 = %s \n",
                                             jacValue2.ToString(12).c_str());
               MessageInterface::ShowMessage("   size of jacValue2 = %d\n",
                                             jacValue2.GetSize());
            #endif
            costJacobianData->SetInitStateJacobian(phaseIdx, 0, stateIdx,
                                                   jacValue2(0));
         }          
      }
       
      // Set state back to nominal value and evaluate.  This computes
      // the correct analytic partials (for those provided)
      // and avoids noise due to finite of constraints taht required it.
      initialInputData.at(phaseIdx)->SetStateVector(nominalStateVector);
      #ifdef DEBUG_USER_FUNCTION
         Rmatrix sj = boundJacobianData->GetInitStateJacobian(phaseIdx);
         MessageInterface::ShowMessage(
                  "AFTER setting back to nominal, bound init state jac "
                  "(for phase %d) =\n %s\n",
                  phaseIdx, sj.ToString(12).c_str());
      #endif
   }

   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("In UPFM::ComputeStateJacobian, about to "
                                    "loop over phases again \n");
   #endif
   // Loop over the phases to compute Jacobians w/r/t final state
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      // Loop initializations for phase dependent data
      Integer numStVars           =
                            finalInputData.at(phaseIdx)->GetNumStateVars();
      Rvector nominalStateVector  =
                            finalInputData.at(phaseIdx)->GetStateVector();
       
      Rvector pertVec(numStVars);
      for (Integer ii = 0; ii < numStVars; ii++)
         pertVec(ii) = deltaVec;
      
      // Loop over state values and perturb them
      for (Integer stateIdx = 0; stateIdx < numStVars; stateIdx++)
      {          
         // Perturb the state and recompute user functions
         Rvector deltaStateVec(numStVars);          // zeros initially
         deltaStateVec(stateIdx) = pertVec(stateIdx);  // set the one element
         finalInputData.at(phaseIdx)->SetStateVector(nominalStateVector +
                                                     deltaStateVec);
         EvaluatePreparedUserFunction();
         #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage(
                        "In ComputeStateJacobian, about to check for boundary "
                        "functions ...\n");
         #endif
         if (hasCostFunction)
         {
            Rvector jacValue4 = (pfContainer->GetCostData()->GetFunctionValues()
                        - nominalCostValue)/pertVec(stateIdx);

            costJacobianData->SetFinalStateJacobian(phaseIdx, 0, stateIdx,
                                                    jacValue4(0));
         }          
      }
      // Set state back to nominal value and evaluate.
      finalInputData.at(phaseIdx)->SetStateVector(nominalStateVector);       
   }

   // This computes the correct analytic partials (for those provided)
   // and avoids noise due to finite of constraints that required it.
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                  "In UPFM::ComputeStateJacobian, calling PerpareInputData \n");
   #endif
   PrepareInputData();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("In UPFM::ComputeStateJacobian, "
                                    "calling EvaluatePreparedUserFunction \n");
   #endif
   EvaluatePreparedUserFunction();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("Leaving UPFM::ComputeStateJacobian\n");
   #endif
}

//------------------------------------------------------------------------------
// void ComputeStaticJacobian()
//------------------------------------------------------------------------------
/**
* This method computes the jacobian with respect to Static
*
*/
//------------------------------------------------------------------------------
void UserPointFunctionManager::ComputeStaticJacobian()
{
#ifdef DEBUG_USER_FUNCTION
   MessageInterface::ShowMessage("In UPFM::ComputeStaticJacobian ... \n");
#endif

   // Save nominal values for later use
   PrepareInputData();
#ifdef DEBUG_USER_FUNCTION
   MessageInterface::ShowMessage("In UPFM::ComputeStaticJacobian, done with "
      "PrepareInputData \n");
#endif

   EvaluatePreparedUserFunction();
#ifdef DEBUG_USER_FUNCTION
   MessageInterface::ShowMessage(
      "In UPFM::ComputeStaticJacobian, done with "
      "EvaluatePreparedUserFunction \n");
#endif
   Rvector nominalCostValue;
   Rvector nominalBoundValues;
   // YK: using deltaVec here makes implementing scaling easier
   Real    deltaVec = userObject->GetStaticPerturbation();

   if (hasCostFunction)
      nominalCostValue = pfContainer->GetCostData()->GetFunctionValues();

   if (hasBoundaryFunctions)
      nominalBoundValues = pfContainer->GetAlgData()->GetFunctionValues();
   
   
   userObject->EvaluateUserJacobian();

   std::vector<std::vector<Rmatrix>> phaseJacs;
   phaseJacs.resize(numPhases);
   std::vector<std::vector<std::string>> phasePos;
   phasePos.resize(numPhases);

   FillJacobianRows(OptimalControlFunction::STATIC, phaseJacs, phasePos);

   Integer numAlgFuncs = 0;
   for (Integer ii = 0; ii < optControlFuncVec.size(); ++ii)
      numAlgFuncs += optControlFuncVec.at(ii)->GetNumFunctions();

   // Check if any point functions were created through a point function
   // object rather than an optimal control function object
   if (numAlgFuncs < numBoundaryFunctions)
   {
      for (Integer phaseIdx = 0; phaseIdx < phaseJacs.size(); ++phaseIdx)
      {
         // Loop initializations for phase dependent data
         Integer numStVars =
            initialInputData.at(phaseIdx)->GetNumStaticVars();

         // compute jacobian only when it is necessary
         if (numStVars > 0)
         {
            Rvector nominalStaticVector =
               initialInputData.at(phaseIdx)->GetStaticVector();

            Rvector pertVec(numStVars);
            for (Integer ii = 0; ii < numStVars; ii++)
               pertVec(ii) = deltaVec;

            for (Integer staticIdx = 0; staticIdx < numStaticVars; staticIdx++)
            {
               // Perturb the Static and recompute user functions
               Rvector deltaStaticVec(numStaticVars);          // or numStVars????
               deltaStaticVec(staticIdx) = pertVec(staticIdx);// set the one element
               initialInputData.at(phaseIdx)->SetStaticVector(nominalStaticVector +
                  deltaStaticVec);
               EvaluatePreparedUserFunction();

               Rvector jacValue = (pfContainer->GetAlgData()->GetFunctionValues()
                  - nominalBoundValues) / pertVec(staticIdx);

               for (Integer ii = 0; ii < (numBoundaryFunctions - numAlgFuncs); ++ii)
                  phaseJacs.at(phaseIdx).at(0)(ii, staticIdx) = jacValue(ii);
            }

            initialInputData.at(phaseIdx)->SetStaticVector(nominalStaticVector);
         }
      }
   }

   // Loop through each element in the Jacobian and set them to the
   // Jacobian data objects
   for (Integer ii = 0; ii < phaseJacs.size(); ++ii)
   {
      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage("Initial Bound Static Jacobian "
            "Size = (%d, %d)\n", phaseJacs.at(ii).at(0).GetNumRows(),
            phaseJacs.at(ii).at(0).GetNumColumns());
         MessageInterface::ShowMessage("Initial Bound Static Jacobian:\n");

         for (Integer i = 0; i < phaseJacs.at(ii).at(0).GetNumRows(); ++i)
         {
            MessageInterface::ShowMessage("%s \n",
               phaseJacs.at(ii).at(0).GetRow(i).ToString().c_str());
         }
         MessageInterface::ShowMessage("\n");
      #endif

      for (Integer rowIdx = 0;
         rowIdx < phaseJacs.at(ii).at(0).GetNumRows(); ++rowIdx)
      {
         for (Integer colIdx = 0;
            colIdx < phaseJacs.at(ii).at(0).GetNumColumns(); ++colIdx)
         {
            boundJacobianData->SetStaticJacobian(ii, rowIdx,
               colIdx, phaseJacs.at(ii).at(0)(rowIdx, colIdx));
         }
      }
   }
   
   
#ifdef DEBUG_USER_FUNCTION
   MessageInterface::ShowMessage(
      "In UPFM::ComputeStaticJacobian, about to loop over phases \n");
   MessageInterface::ShowMessage(
      "   numPhases = %d, size of initialInputData = %d, size of "
      "finalInputData = %d\n",
      numPhases, (Integer)initialInputData.size(),
      finalInputData.size());
#endif
   // static parameters are same at the initial and final time points
   // so only one loop is necessary. YK

   // Loop over the phases to compute Jacobians w/r/t Static
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      // Loop initializations for phase dependent data
      Integer numStVars =
         initialInputData.at(phaseIdx)->GetNumStaticVars();

      // compute jacobian only when it is necessary
      if (numStVars > 0)
      {
         Rvector nominalStaticVector =
            initialInputData.at(phaseIdx)->GetStaticVector();

         Rvector pertVec(numStVars);
         for (Integer ii = 0; ii < numStVars; ii++)
            pertVec(ii) = deltaVec;

         #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage("--- numStVars           = %d \n",
               numStVars);
            MessageInterface::ShowMessage("   size of Static vector = %d \n",
               nominalStaticVector.GetSize());
            MessageInterface::ShowMessage("--- nominalStaticVector  = %s \n",
               nominalStaticVector.ToString(12).c_str());
         #endif
         // Loop over Static values and perturb them
         // or use numStVars here????
         for (Integer staticIdx = 0; staticIdx < numStaticVars; staticIdx++)
         {
            // Perturb the Static and recompute user functions
            Rvector deltaStaticVec(numStaticVars);          // or numStVars????
            deltaStaticVec(staticIdx) = pertVec(staticIdx);// set the one element
            initialInputData.at(phaseIdx)->SetStaticVector(nominalStaticVector +
                                                           deltaStaticVec);
            EvaluatePreparedUserFunction();

            #ifdef DEBUG_USER_FUNCTION
               MessageInterface::ShowMessage("--- hasBoundaryFunction = %s \n",
                  (hasBoundaryFunctions ? "true" : "false"));
               MessageInterface::ShowMessage("--- hasCostFunction = %s \n",
                  (hasCostFunction ? "true" : "false"));
            #endif
            // Compute and save current row of Jacobian
            // TODO: Fix to test if user has provided Jacobain on a
            // phase level.
            if (hasCostFunction)
            {
               Rvector costVals = pfContainer->GetCostData()->GetFunctionValues();
               #ifdef DEBUG_USER_FUNCTION
                  MessageInterface::ShowMessage(
                     "cost function values (size %d)  = %s\n",
                     costVals.GetSize(),
                     costVals.ToString(12).c_str());
                  MessageInterface::ShowMessage(
                     "nominal cost function (size %d) = %s\n",
                     nominalCostValue.GetSize(),
                     nominalCostValue.ToString(12).c_str());
                  MessageInterface::ShowMessage("pertVec(staticIdx) = %12.10f\n",
                     pertVec(staticIdx));
               #endif
               Rvector jacValue2(1);
               jacValue2(0) = (costVals(0) -
                  nominalCostValue(0)) / pertVec(staticIdx);

               #ifdef DEBUG_USER_FUNCTION
                  MessageInterface::ShowMessage("--- jacValue2 = %s \n",
                     jacValue2.ToString(12).c_str());
                  MessageInterface::ShowMessage("   size of jacValue2 = %d\n",
                     jacValue2.GetSize());
               #endif
               costJacobianData->SetStaticJacobian(phaseIdx, 0, staticIdx,
                                                   jacValue2(0));
         }
      }
         // Set Static back to nominal value and evaluate.  This computes
         // the correct analytic partials (for those provided)
         // and avoids noise due to finite of constraints that required it.
         initialInputData.at(phaseIdx)->SetStaticVector(nominalStaticVector);
      }
      #ifdef DEBUG_USER_FUNCTION
         Rmatrix sj = boundJacobianData->GetInitStaticJacobian(phaseIdx);
         MessageInterface::ShowMessage(
            "AFTER setting back to nominal, bound init Static jac "
            "(for phase %d) =\n %s\n",
            phaseIdx, sj.ToString(12).c_str());
      #endif
   }

   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("In UPFM::ComputeStaticJacobian, about to "
         "loop over phases again \n");
   #endif   

   // This computes the correct analytic partials (for those provided)
   // and avoids noise due to finite of constraints that required it.
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
         "In UPFM::ComputeStaticJacobian, calling PerpareInputData \n");
   #endif
   PrepareInputData();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("In UPFM::ComputeStaticJacobian, "
         "calling EvaluatePreparedUserFunction \n");
   #endif
   EvaluatePreparedUserFunction();
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("Leaving UPFM::ComputeStaticJacobian\n");
   #endif
}

//------------------------------------------------------------------------------
// void ComputeTimeSparsity(Real &timeUpperBound, Real &timeLowerBound)
//------------------------------------------------------------------------------
/**
 * This method computes the sparsity pattern of user functions w/r/t to time
 *
 * @param <timeUpperBound>  the time upper bound 
 * @param <timeLowerBound>  the time lower bound
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::ComputeTimeSparsity()
{
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("Entering UPFM::ComputeTmeSparsity\n");
      MessageInterface::ShowMessage("   size of initialInputData = %d\n",
                                    (Integer) initialInputData.size());
      MessageInterface::ShowMessage("   size of finalInputData   = %d\n",
                                    (Integer) finalInputData.size());
   #endif
   // Computes sparsity pattern of user functions w/r/t to time
   Integer numRandEvals = 3;
   
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      Real nominalTime = initialInputData.at(phaseIdx)->GetTime();
      Real timeLowerBound   = GetTimeLowerBound(phaseIdx);
      Real timeUpperBound   = GetTimeUpperBound(phaseIdx);

      // yk mod on state setting to resolve sparsity pattern bug
      Rvector nominalState = initialInputData.at(phaseIdx)->GetStateVector();
      Rvector stateLowerBound = GetStateLowerBound(phaseIdx);
      Rvector stateUpperBound = GetStateUpperBound(phaseIdx);
       
      // Evaluate at lower bound
      initialInputData.at(phaseIdx)->SetTime(timeLowerBound);
      EvaluatePreparedUserFunction();
      ComputeTimeJacobian();
      UpdateTimeSparsityPattern(phaseIdx);
      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage(
                     "In UPFM::ComputeTmeSparsity, evaluated at lower bound\n");
      #endif
      
      // Evaluate at upper bound
      initialInputData.at(phaseIdx)->SetTime(timeUpperBound);
      EvaluatePreparedUserFunction();
      ComputeTimeJacobian();
      UpdateTimeSparsityPattern(phaseIdx);
      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage(
                     "In UPFM::ComputeTmeSparsity, evaluated at upper bound\n");
      #endif
      
      // Evalaute at random points
      for (Integer randIdx = 0; randIdx < numRandEvals; randIdx++)
      {

         // yk mod on state setting to resolve sparsity pattern bug
         Rvector stateVec = GetRandomVector(stateUpperBound, stateLowerBound);
         initialInputData.at(phaseIdx)->SetStateVector(stateVec);


         Rvector timeLow(1, timeLowerBound);
         Rvector timeHigh(1, timeUpperBound);
         Rvector time = GetRandomVector(timeHigh,timeLow);
         #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage("--- time vector has size %d\n",
                                          time.GetSize());
         #endif
         initialInputData.at(phaseIdx)->SetTime(time(0));
         #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage(
                           "--- time set for initialInputData (%d)\n", randIdx);
         #endif
         EvaluatePreparedUserFunction();
         #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage(
                                    "--- about to call ComputeTimeJacobian\n");
         #endif
         ComputeTimeJacobian();
         #ifdef DEBUG_USER_FUNCTION
            MessageInterface::ShowMessage(
                              "--- about to call UpdateTimeSparsityPattern\n");
         #endif
         UpdateTimeSparsityPattern(phaseIdx);
      }
      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage(
                  "In UPFM::ComputeTmeSparsity, evaluated at random points\n");
      #endif
      // Set back to nominal values
      initialInputData.at(phaseIdx)->SetTime(nominalTime);
      initialInputData.at(phaseIdx)->SetStateVector(nominalState);
   }
   
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {       
      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage(
                              " about to evaluate final at lower bound\n");
      #endif
      Real nominalTime = finalInputData.at(phaseIdx)->GetTime();
      Real timeLowerBound = GetTimeLowerBound(phaseIdx);
      Real timeUpperBound = GetTimeUpperBound(phaseIdx);

      // yk mod on state setting to resolve sparsity pattern bug
      Rvector nominalState = finalInputData.at(phaseIdx)->GetStateVector();
      Rvector stateLowerBound = GetStateLowerBound(phaseIdx);
      Rvector stateUpperBound = GetStateUpperBound(phaseIdx);

      // Evaluate at lower bound
      finalInputData.at(phaseIdx)->SetTime(timeLowerBound);
      EvaluatePreparedUserFunction();
      ComputeTimeJacobian();
      UpdateTimeSparsityPattern(phaseIdx);
       
      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage(
                                 " about to evaluate final at upper bound\n");
      #endif
      // Evaluate at upper bound
      finalInputData.at(phaseIdx)->SetTime(timeUpperBound);
      EvaluatePreparedUserFunction();
      ComputeTimeJacobian();
      UpdateTimeSparsityPattern(phaseIdx);
       
      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage(
                              " about to evaluate final at random points\n");
      #endif
      //  Evalaute at random points
      for (Integer randIdx = 0; randIdx < numRandEvals; randIdx++)
      {
         // yk mod on state setting to resolve sparsity pattern bug
         Rvector stateVec = GetRandomVector(stateUpperBound, stateLowerBound);
         finalInputData.at(phaseIdx)->SetStateVector(stateVec);

         Rvector timeLow(1, timeLowerBound);
         Rvector timeHigh(1, timeUpperBound);
         Rvector time = GetRandomVector(timeHigh,timeLow);
         finalInputData.at(phaseIdx)->SetTime(time(0));

         EvaluatePreparedUserFunction();
         ComputeTimeJacobian();
         UpdateTimeSparsityPattern(phaseIdx);
      }
       
      // Set back to nominal values
      finalInputData.at(phaseIdx)->SetTime(nominalTime);      
      finalInputData.at(phaseIdx)->SetStateVector(nominalState);
   }   
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("LEAVING UPFM::ComputeTmeSparsity\n");
   #endif
}

//------------------------------------------------------------------------------
// void ComputeStateSparsity()
//------------------------------------------------------------------------------
/**
 * This method computes the sparsity pattern of user functions w/r/t to state
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::ComputeStateSparsity()
{
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                  "In UPFM::ComputeStateSparsity ..., numPhases = %d\n",
                  numPhases);
   #endif
   

   // Computes sparsity pattern of user functions w/r/t to state
   
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      Real timeLowerBound = GetTimeLowerBound(phaseIdx);
      Real timeUpperBound = GetTimeUpperBound(phaseIdx);
      Rvector tLB(1, timeLowerBound);
      Rvector tUB(1, timeUpperBound);

      Rvector nominalState    = initialInputData.at(phaseIdx)->GetStateVector();
      Rvector stateLowerBound = GetStateLowerBound(phaseIdx);
      Rvector stateUpperBound = GetStateUpperBound(phaseIdx);
       
      // Evaluate at lower bound
      initialInputData.at(phaseIdx)->SetStateVector(stateLowerBound);
      EvaluatePreparedUserFunction();
      ComputeStateJacobian();
      UpdateStateSparsityPattern(phaseIdx);
       
      // Evaluate at upper bound
      initialInputData.at(phaseIdx)->SetStateVector(stateUpperBound);
      EvaluatePreparedUserFunction();
      ComputeStateJacobian();
      UpdateStateSparsityPattern(phaseIdx);
       
      // Evalaute at random points
      Integer numRandEvals = 3;
      Real nominalTime = initialInputData.at(phaseIdx)->GetTime();

      for (Integer randIdx = 0; randIdx < numRandEvals; randIdx++)
      {
         // mod by YK on random time setting to resolve sparsity pattern bug
         Rvector timeVec = GetRandomVector(tUB, tLB);
         initialInputData.at(phaseIdx)->SetTime(timeVec(0));

         Rvector stateVec = GetRandomVector(stateUpperBound,stateLowerBound);
         initialInputData.at(phaseIdx)->SetStateVector(stateVec);
         EvaluatePreparedUserFunction();
         ComputeStateJacobian();
         UpdateStateSparsityPattern(phaseIdx);
      }
      // Set back to nominal values      
      initialInputData.at(phaseIdx)->SetTime(nominalTime);
      initialInputData.at(phaseIdx)->SetStateVector(nominalState);
   }
   
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {       
      Real timeLowerBound = GetTimeLowerBound(phaseIdx);
      Real timeUpperBound = GetTimeUpperBound(phaseIdx);
      Rvector tLB(1, timeLowerBound);
      Rvector tUB(1, timeUpperBound);

      Rvector nominalState    = finalInputData.at(phaseIdx)->GetStateVector();
      Rvector stateLowerBound = GetStateLowerBound(phaseIdx);
      Rvector stateUpperBound = GetStateUpperBound(phaseIdx);
      // Evaluate at lower bound
      finalInputData.at(phaseIdx)->SetStateVector(stateLowerBound);
      EvaluatePreparedUserFunction();
      ComputeStateJacobian();
      UpdateStateSparsityPattern(phaseIdx);
       
      // Evaluate at upper bound
      finalInputData.at(phaseIdx)->SetStateVector(stateUpperBound);
      EvaluatePreparedUserFunction();
      ComputeStateJacobian();
      UpdateStateSparsityPattern(phaseIdx);
       
      // Evalaute at random points
      Integer numRandEvals = 3;
      Real nominalTime = finalInputData.at(phaseIdx)->GetTime();

      for (Integer randIdx = 0; randIdx < numRandEvals; randIdx++)
      {
         // mod by YK on random time setting to resolve sparsity pattern bug
         Rvector timeVec = GetRandomVector(tUB, tLB);
         finalInputData.at(phaseIdx)->SetTime(timeVec(0));

         Rvector stateVec = GetRandomVector(stateUpperBound,stateLowerBound);
         finalInputData.at(phaseIdx)->SetStateVector(stateVec);
         EvaluatePreparedUserFunction();
         ComputeStateJacobian();
         UpdateStateSparsityPattern(phaseIdx);
      }
       
      // Set back to nominal values      
      finalInputData.at(phaseIdx)->SetTime(nominalTime);
      finalInputData.at(phaseIdx)->SetStateVector(nominalState);
   }  
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage("LEAVING UPFM::ComputeStateSparsity\n");
   #endif
}


//------------------------------------------------------------------------------
// void ComputeStateSparsity()
//------------------------------------------------------------------------------
/**
* This method computes the sparsity pattern of user functions w/r/t to static
* parameters
*/
//------------------------------------------------------------------------------
void UserPointFunctionManager::ComputeStaticSparsity()
{
#ifdef DEBUG_USER_FUNCTION
   MessageInterface::ShowMessage(
      "In UPFM::ComputeStaticSparsity ..., numPhases = %d\n",
      numPhases);
#endif


   // Computes sparsity pattern of user functions w/r/t to static
   // note that static parameters are same at the initial and final time points
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      Real timeLowerBound = GetTimeLowerBound(phaseIdx);
      Real timeUpperBound = GetTimeUpperBound(phaseIdx);
      Rvector tLB(1, timeLowerBound);
      Rvector tUB(1, timeUpperBound);

      Rvector nominalStatic = initialInputData.at(phaseIdx)->GetStaticVector();
      Rvector staticLowerBound = GetStaticLowerBound(phaseIdx);
      Rvector staticUpperBound = GetStaticUpperBound(phaseIdx);

      // Evaluate at lower bound
      initialInputData.at(phaseIdx)->SetStaticVector(staticLowerBound);
      EvaluatePreparedUserFunction();
      ComputeStaticJacobian();
      UpdateStaticSparsityPattern(phaseIdx);

      // Evaluate at upper bound
      initialInputData.at(phaseIdx)->SetStaticVector(staticUpperBound);
      EvaluatePreparedUserFunction();
      ComputeStaticJacobian();
      UpdateStaticSparsityPattern(phaseIdx);

      // Evalaute at random points
      Integer numRandEvals = 3;
      Real nominalTime = initialInputData.at(phaseIdx)->GetTime();

      for (Integer randIdx = 0; randIdx < numRandEvals; randIdx++)
      {
         // mod by YK on random time setting to resolve sparsity pattern bug
         Rvector timeVec = GetRandomVector(tUB, tLB);
         initialInputData.at(phaseIdx)->SetTime(timeVec(0));

         Rvector staticVec = GetRandomVector(staticUpperBound, staticLowerBound);
         initialInputData.at(phaseIdx)->SetStaticVector(staticVec);
         EvaluatePreparedUserFunction();
         ComputeStaticJacobian();
         UpdateStaticSparsityPattern(phaseIdx);
      }
      // Set back to nominal values      
      initialInputData.at(phaseIdx)->SetTime(nominalTime);
      initialInputData.at(phaseIdx)->SetStaticVector(nominalStatic);
   }
#ifdef DEBUG_USER_FUNCTION
   MessageInterface::ShowMessage("LEAVING UPFM::ComputeStaticSparsity\n");
#endif
}

//------------------------------------------------------------------------------
// void UpdateStateSparsityPattern(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method adds elements to the sparsity pattern for state Jacobians
 *
 * @param <phaseIdx>  the phase index
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::UpdateStateSparsityPattern(Integer phaseIdx)
{
   // Add elements to sparsity pattern for state Jacobians.
   // Dynamics Function
   if (hasBoundaryFunctions)
   {
      FunctionInputData *phaseData    = initialInputData.at(phaseIdx);
      Integer           numSVars      = phaseData->GetNumStateVars();
      Rmatrix           initJacobian  =
                        boundJacobianData->GetInitStateJacobian(phaseIdx);
      Rmatrix           finalJacobian =
                        boundJacobianData->GetFinalStateJacobian(phaseIdx);
      for (Integer funcIdx = 0; funcIdx < numBoundaryFunctions; funcIdx++)
      {
         for (Integer stateIdx = 0; stateIdx < numSVars; stateIdx++)
         {
            if (!GmatMathUtil::IsEqual(initJacobian(funcIdx,stateIdx), 0.0))
               boundJacobianData->SetInitStateJacobianPattern(phaseIdx,
                                  funcIdx,stateIdx, 1.0);

            if (!GmatMathUtil::IsEqual(finalJacobian(funcIdx,stateIdx), 0.0))
              boundJacobianData->SetFinalStateJacobianPattern(phaseIdx,
                                 funcIdx,stateIdx, 1.0);
         }
      }
   }
   // Cost Function
   if (hasCostFunction)
   {
      FunctionInputData *phaseData    = initialInputData.at(phaseIdx);
      Rmatrix           initJacobian  =
                        costJacobianData->GetInitStateJacobian(phaseIdx);
      Rmatrix           finalJacobian =
                        costJacobianData->GetFinalStateJacobian(phaseIdx);
      for (Integer stateIdx = 0; stateIdx < phaseData->GetNumStateVars();
           stateIdx++)
      {
         if (!GmatMathUtil::IsEqual(initJacobian(0,stateIdx), 0.0))
            costJacobianData->SetInitStateJacobianPattern(phaseIdx,0,
                                                          stateIdx, 1.0);

         if (!GmatMathUtil::IsEqual(finalJacobian(0,stateIdx), 0.0))
           costJacobianData->SetFinalStateJacobianPattern(phaseIdx,0,
                                                          stateIdx, 1.0);
      }
   }
}

//------------------------------------------------------------------------------
// void UpdateStaticSparsityPattern(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
* This method adds elements to the sparsity pattern for Static Jacobians
*
* @param <phaseIdx>  the phase index
*
*/
//------------------------------------------------------------------------------
void UserPointFunctionManager::UpdateStaticSparsityPattern(Integer phaseIdx)
{
   // Add elements to sparsity pattern for Static Jacobians.
   // Dynamics Function
   if (hasBoundaryFunctions)
   {
      FunctionInputData *phaseData = initialInputData.at(phaseIdx);
      Integer           numSVars = phaseData->GetNumStaticVars();
      Rmatrix           initJacobian =
                                 boundJacobianData->GetStaticJacobian(phaseIdx);
      for (Integer funcIdx = 0; funcIdx < numBoundaryFunctions; funcIdx++)
      {
         for (Integer staticIdx = 0; staticIdx < numSVars; staticIdx++)
         {
            if (!GmatMathUtil::IsEqual(initJacobian(funcIdx, staticIdx), 0.0))
               boundJacobianData->SetStaticJacobianPattern(phaseIdx, funcIdx,
                                                           staticIdx, 1.0);
         }
      }
   }
   // Cost Function
   if (hasCostFunction)
   {
      FunctionInputData *phaseData = initialInputData.at(phaseIdx);
      Rmatrix           initJacobian =
                                 costJacobianData->GetStaticJacobian(phaseIdx);
      for (Integer staticIdx = 0; staticIdx < phaseData->GetNumStaticVars();
         staticIdx++)
      {
         if (!GmatMathUtil::IsEqual(initJacobian(0, staticIdx), 0.0))
            costJacobianData->SetStaticJacobianPattern(phaseIdx, 0,
                                                       staticIdx, 1.0);
      }
   }
}

//------------------------------------------------------------------------------
// void UpdateTimeSparsityPattern(Integer phaseIdx)
//------------------------------------------------------------------------------
/**
 * This method adds elements to the sparsity pattern for time Jacobians
 *
 * @param <phaseIdx>  the phase index
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::UpdateTimeSparsityPattern(Integer phaseIdx)
{
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                  "Entering UPFM::UpdateTimeSparsityPattern, phaseIdx = %d\n",
                  phaseIdx);
      MessageInterface::ShowMessage(
                  "   hasBoundaryFunctions = %s, hasCostFunction = %s\n",
                  (hasBoundaryFunctions? "true" : "false"),
                  (hasCostFunction? "true" : "false"));
   #endif
   // Dynamics Function
   if (hasBoundaryFunctions)
   {
      Rmatrix initJacobian  = boundJacobianData->GetInitTimeJacobian(phaseIdx);
      Rmatrix finalJacobian = boundJacobianData->GetFinalTimeJacobian(phaseIdx);
      #ifdef DEBUG_USER_FUNCTION
         Integer r, c;
         initJacobian.GetSize(r, c);
         MessageInterface::ShowMessage("BOUNDARY: initJacobian (%d, %d) = %s\n",
                                       r, c,
                                       initJacobian.ToString(12).c_str());
         finalJacobian.GetSize(r, c);
         MessageInterface::ShowMessage(
                              "BOUNDARY : finalJacobian (%d, %d) = %s\n", r, c,
                              finalJacobian.ToString(12).c_str());
      #endif
      for (Integer funcIdx = 0; funcIdx < numBoundaryFunctions; funcIdx++)
      {
         if (!GmatMathUtil::IsEqual(initJacobian(funcIdx,0), 0.0))
            boundJacobianData->SetInitTimeJacobianPattern(phaseIdx,funcIdx,
                                                          0, 1.0);

         if (!GmatMathUtil::IsEqual(finalJacobian(funcIdx,0), 0.0))
           boundJacobianData->SetFinalTimeJacobianPattern(phaseIdx,funcIdx,
                                                          0, 1.0);
      }
   }
   // Cost Function
   if (hasCostFunction)
   {
      #ifdef DEBUG_USER_FUNCTION
         MessageInterface::ShowMessage(
                           "In part where we check the cost function stuff\n");
      #endif
      Rmatrix initJacobian  = costJacobianData->GetInitTimeJacobian(phaseIdx);
      Rmatrix finalJacobian = costJacobianData->GetFinalTimeJacobian(phaseIdx);
      #ifdef DEBUG_USER_FUNCTION
         Integer r, c;
         initJacobian.GetSize(r, c);
         MessageInterface::ShowMessage("COST: initJacobian (%d, %d) = %s\n",
                                       r, c,
                                       initJacobian.ToString(12).c_str());
         finalJacobian.GetSize(r, c);
         MessageInterface::ShowMessage("COST: finalJacobian (%d, %d) = %s\n",
                                       r, c,
                                       finalJacobian.ToString(12).c_str());
      #endif
      if (!GmatMathUtil::IsEqual(initJacobian(0,0), 0.0))
         costJacobianData->SetInitTimeJacobianPattern(phaseIdx,0,0, 1.0);

      if (!GmatMathUtil::IsEqual(finalJacobian(0,0), 0.0))
        costJacobianData->SetFinalTimeJacobianPattern(phaseIdx,0,0, 1.0);
   }
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                                 "LEAVING UPFM::UpdateTimeSparsityPattern\n");
   #endif
}

// --- should these be protected? ---


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void InitializeBoundNLPUtil()
//------------------------------------------------------------------------------
/**
 * This method initializes the bound function NLP utility helper class
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::InitializeBoundNLPUtil()
{
   if (pfContainer->GetAlgData()->GetNumFunctions() == 0)
      throw LowThrustException("For UserPointFunctionManager::"
         "InitializeBoundNLPUtil(), there was an error in "
         "initializing bound data, zero point functions were provided\n");
   boundNLPUtil->Initialize(phaseList, *pfContainer->GetAlgData(),
                            boundJacobianData,totalNumDecisionParams);
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                           "****** LEAVING UPFM::InitializeBoundNLPUtil\n");
   #endif
}

//------------------------------------------------------------------------------
// void InitializeCostNLPUtil()
//------------------------------------------------------------------------------
/**
 * This method initializes the cost function NLP utility helper class
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::InitializeCostNLPUtil()
{
   costNLPUtil->Initialize(phaseList, *pfContainer->GetCostData(),
                           costJacobianData,totalNumDecisionParams);
   #ifdef DEBUG_USER_FUNCTION
      MessageInterface::ShowMessage(
                              "****** LEAVING UPFM::InitializeCostNLPUtil\n");
   #endif
}

//------------------------------------------------------------------------------
// void DetermineFunctionDependencies()
//------------------------------------------------------------------------------
/**
 * This method determines the dependency of bound and cost functions on state
 * and time
 *
 */
//------------------------------------------------------------------------------
void UserPointFunctionManager::DetermineFunctionDependencies()
{
   // Determines dependency of bound and cost functions on state
   // and time
   for (Integer phaseIdx = 0; phaseIdx < numPhases; phaseIdx++)
   {
      if (GetMax(boundJacobianData->GetInitTimeJacobianPattern(phaseIdx)) != 0)
         boundJacobianData->SetInitialTimeDependency(phaseIdx,true);
      else
         boundJacobianData->SetInitialTimeDependency(phaseIdx,false);

       
      if (GetMax(boundJacobianData->GetInitStateJacobianPattern(phaseIdx)) != 0)
         boundJacobianData->SetInitialStateDependency(phaseIdx,true);
      else
         boundJacobianData->SetInitialStateDependency(phaseIdx,false);

       
      if (GetMax(boundJacobianData->GetFinalTimeJacobianPattern(phaseIdx)) != 0)
         boundJacobianData->SetFinalTimeDependency(phaseIdx,true);
      else
         boundJacobianData->SetFinalTimeDependency(phaseIdx,false);

       
      if (GetMax(boundJacobianData->GetFinalStateJacobianPattern(phaseIdx))
          != 0)
         boundJacobianData->SetFinalStateDependency(phaseIdx,true);
      else
         boundJacobianData->SetFinalStateDependency(phaseIdx,false);

      if (GetMax(costJacobianData->GetInitTimeJacobianPattern(phaseIdx)) != 0)
         costJacobianData->SetInitialTimeDependency(phaseIdx,true);
      else
         costJacobianData->SetInitialTimeDependency(phaseIdx,false);

       
      if (GetMax(costJacobianData->GetInitStateJacobianPattern(phaseIdx)) != 0)
         costJacobianData->SetInitialStateDependency(phaseIdx,true);
      else
         costJacobianData->SetInitialStateDependency(phaseIdx,false);

       
      if (GetMax(costJacobianData->GetFinalTimeJacobianPattern(phaseIdx)) != 0)
         costJacobianData->SetFinalTimeDependency(phaseIdx,true);
      else
         costJacobianData->SetFinalTimeDependency(phaseIdx,false);

       
      if (GetMax(costJacobianData->GetFinalStateJacobianPattern(phaseIdx)) != 0)
         costJacobianData->SetFinalStateDependency(phaseIdx,true);
      else
         costJacobianData->SetFinalStateDependency(phaseIdx,false);


      if (GetMax(costJacobianData->GetStaticJacobianPattern(phaseIdx)) != 0)
         costJacobianData->SetStaticDependency(phaseIdx,true);
      else
         costJacobianData->SetStaticDependency(phaseIdx,false);
   }
}

//------------------------------------------------------------------------------
// void FillJacobianRows(const OptimalControlFunction::VariableType jacType,
//                       std::vector<std::vector<Rmatrix>> &phaseJacs,
//                       std::vector<std::vector<std::string>> &phasePos)
//------------------------------------------------------------------------------
/**
* This method fills rows of Jacobians to contain the total number of functions
* in a phase when the user has split the functions into multiple 
*
* @param jacType The type of Jacobian being modified
* @param phaseJacs Matrix of Jacobians with each row representing each phase in
*        the trajectory, this will be modified to contain just the initial and
*        final Jacobian for each phase after the Jacobians are combined
* @param phasePos Matrix of strings dictating whether a Jacobian is at the
*        initial or final point of the phase, where the phase number is
*        determined by the row number, this will be modified to contain 
*        "Initial" and "Final" for each row to match phaseJacs
*/
//------------------------------------------------------------------------------
void UserPointFunctionManager::FillJacobianRows(
   const OptimalControlFunction::VariableType jacType,
   std::vector<std::vector<Rmatrix>> &phaseJacs,
   std::vector<std::vector<std::string>> &phasePos)
{
   // Begin by forming arrays to store which phases are used in each row of
   // the Jacobian
   std::vector<IntegerArray> initRowPhaseIdxs, finalRowPhaseIdxs;
   initRowPhaseIdxs.resize(numBoundaryFunctions);
   finalRowPhaseIdxs.resize(numBoundaryFunctions);
   Integer numPoints, numAlgFuncs = 0, currJacRow = 0;
   std::string position;

   // Start filling the Jacobians at the rows that use the optimal control
   // function method of setting boundary functions
   for (Integer ii = 0; ii < optControlFuncVec.size(); ++ii)
      numAlgFuncs += optControlFuncVec.at(ii)->GetNumFunctions();
   currJacRow = numBoundaryFunctions - numAlgFuncs;
   numAlgFuncs = numBoundaryFunctions - numAlgFuncs;

   // Loop through each optimal control function object, collecting the 
   // Jacobian from each and storing info on which phase and position they
   // pertain to.  These are later combined together when necessary so that
   // there is only an initial Jacobian and final Jacobian for each phase
   for (Integer ii = 0; ii < optControlFuncVec.size(); ++ii)
   {
      Integer phaseIdx;
      numPoints = optControlFuncVec.at(ii)->GetNumPoints();
      numAlgFuncs += optControlFuncVec.at(ii)->GetNumFunctions();
      for (Integer pointIdx = 0; pointIdx < numPoints; ++pointIdx)
      {
         optControlFuncVec.at(ii)->GetPointData(pointIdx, phaseIdx, position);

         // If there is a state dependency, calculate the state Jacobian
         Rmatrix jacValues = optControlFuncVec.at(ii)->GetJacobian(
            jacType, pointIdx);

         phaseJacs.at(phaseIdx).push_back(jacValues);
         phasePos.at(phaseIdx).push_back(position);

         for (Integer jacRowIdx = currJacRow; jacRowIdx < numAlgFuncs;
            ++jacRowIdx)
         {
            if (position == "Initial" ||
               jacType == OptimalControlFunction::STATIC)
               initRowPhaseIdxs.at(jacRowIdx).push_back(phaseIdx);
            else
               finalRowPhaseIdxs.at(jacRowIdx).push_back(phaseIdx);
         }
      }
      currJacRow = numAlgFuncs;
   }

   /*
   IntegerArray rowUsingOldFuncSetup;
   rowUsingOldFuncSetup.push_back(-1);

   for (Integer i = 0; i < numBoundaryFunctions - numAlgFuncs; ++i)
   {
      initRowPhaseIdxs.at(i) = rowUsingOldFuncSetup;
      finalRowPhaseIdxs.at(i) = rowUsingOldFuncSetup;
   }
   */

   // Store the number of variables depending on the Jacobian type being
   // stored, this will be the number of columns in the Jacobian
   Integer numVars;
   switch (jacType)
   {
   case OptimalControlFunction::STATE:
      numVars = numStateVars;
      break;
   case OptimalControlFunction::CONTROL:
      numVars = numControlVars;
      break;
   case OptimalControlFunction::TIME:
      numVars = 1;
      break;
   case OptimalControlFunction::STATIC:
      numVars = numStaticVars;
      break;
   }

   for (Integer ii = 0; ii < numPhases; ++ii)
   {
      // Check if any functions have been added by the user through the old
      // point object method
      Integer numOptCtrlFuncs = 0;
      for (Integer jj = 0; jj < optControlFuncVec.size(); ++jj)
         numOptCtrlFuncs += optControlFuncVec.at(jj)->GetNumFunctions();

      // Begin filling the initial and final Jacobians from this phase with
      // the data from all other Jacobians in this phase
      Rmatrix initJac(numBoundaryFunctions, numVars),
         finalJac(numBoundaryFunctions, numVars);
      Integer currentInitialRow = 0, currentFinalRow = 0;
      for (Integer jj = 0; jj < phaseJacs.at(ii).size(); ++jj)
      {
         Integer currentRow = 0;
         if (phasePos.at(ii).at(jj) == "Initial" ||
            jacType == OptimalControlFunction::STATIC)
            currentRow = currentInitialRow;
         else
            currentRow = currentFinalRow;
         Integer initRow = 0, finalRow = 0;
         Integer oldNumRows = phaseJacs.at(ii).at(jj).GetNumRows();
         for (Integer rowIdx = currentRow;
            rowIdx < numBoundaryFunctions; ++rowIdx)
         {
            if ((find(initRowPhaseIdxs.at(rowIdx).begin(),
               initRowPhaseIdxs.at(rowIdx).end(), ii) !=
               initRowPhaseIdxs.at(rowIdx).end()) &&
               (phasePos.at(ii).at(jj) == "Initial" ||
               jacType == OptimalControlFunction::STATIC))
            {
               for (Integer colIdx = 0; colIdx < numVars; ++colIdx)
               {
                  initJac(rowIdx, colIdx) =
                     phaseJacs.at(ii).at(jj)(initRow, colIdx);
               }

               ++initRow;
            }

            else if ((find(finalRowPhaseIdxs.at(rowIdx).begin(),
               finalRowPhaseIdxs.at(rowIdx).end(), ii) !=
               finalRowPhaseIdxs.at(rowIdx).end()) &&
               (phasePos.at(ii).at(jj) == "Final"))
            {
               for (Integer colIdx = 0; colIdx < numVars; ++colIdx)
               {
                  finalJac(rowIdx, colIdx) =
                     phaseJacs.at(ii).at(jj)(finalRow, colIdx);
               }

               ++finalRow;
            }

            if (initRow >= oldNumRows || finalRow >= oldNumRows)
            {
               if (phasePos.at(ii).at(jj) == "Initial" ||
                  jacType == OptimalControlFunction::STATIC)
                  currentInitialRow = rowIdx + 1;
               else
                  currentFinalRow = rowIdx + 1;
               break;
            }
         }
      }

      // Remove the old Jacobians and replace them with the completed
      // initial and final Jacobians of the current phase
      phaseJacs.at(ii).clear();
      phaseJacs.at(ii).push_back(initJac);
      phaseJacs.at(ii).push_back(finalJac);
      phasePos.at(ii).clear();
      phasePos.at(ii).push_back("Initial");
      phasePos.at(ii).push_back("Final");
   }
}

//------------------------------------------------------------------------------
// Real GetMax(const Rmatrix &mat)
//------------------------------------------------------------------------------
/**
 * This method determines the maximum value in the input matrix
 *
 */
//------------------------------------------------------------------------------
Real UserPointFunctionManager::GetMax(const Rmatrix &mat)
{
   Real max = -999.99;
   Integer r, c;
   mat.GetSize(r,c);
   for (Integer ii = 0; ii < r; ii++)
      for (Integer jj = 0; jj < c; jj++)
         if (mat(ii,jj) > max) max = mat(ii,jj);
   return max;
}
