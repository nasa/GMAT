//------------------------------------------------------------------------------
//                           UserPathFunctionManager
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
// Created: 2015.12.16
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------
#include <cstdlib>   // for rand
#include "UserPathFunctionManager.hpp"
#include "MessageInterface.hpp"
#include "LowThrustException.hpp"
#include "UserFunctionProperties.hpp"

//#define DEBUG_MANAGER
//#define DEBUG_MANAGER_VALUES
//#define DEBUG_MANAGER_FUNCTIONS
//#define DEBUG_WRITE_DATA

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
UserPathFunctionManager::UserPathFunctionManager() :
   UserFunctionManager(),
   paramData                      (NULL),
   pfContainer                    (NULL)
{
   
   for (Integer idx1 = UserFunction::DYNAMICS; 
        idx1 < UserFunction::ALLFUNCTIONS; idx1++)
   {
      hasFunctions.push_back(false);
      numFunctions.push_back(0);
   }
   
   bool isPreserving = false;
   
   needsJacobianFiniteDiff.resize(UserFunction::ALLFUNCTIONS, 
                                  UserFunction::ALLJACOBIANS, isPreserving);
   
   for (Integer idx2 = UserFunction::STATE; 
       idx2 < UserFunction::ALLJACOBIANS; idx2++)
   {
      numVars.push_back(0);
   }

   for (Integer idx1 = UserFunction::DYNAMICS;
      idx1 < UserFunction::ALLFUNCTIONS; idx1++)
   {
      
      std::vector<Rmatrix> tmpJacPattern;
      std::vector<Rmatrix> tmpJacobian;
      for (Integer idx2 = UserFunction::STATE; 
         idx2 < UserFunction::ALLJACOBIANS; idx2++)
      {
         // to prevent memory allocation error, 
         // make the defualt matrix size as (1,1); YK 18.01.23
         Rmatrix tmpMat(1,1);
         tmpJacobian.push_back(tmpMat);
         tmpJacPattern.push_back(tmpMat);
         needsJacobianFiniteDiff(idx1, idx2) = false;
      }
      jacPattern.push_back(tmpJacPattern);
      jacobian.push_back(tmpJacobian);
   }
}


//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
UserPathFunctionManager::UserPathFunctionManager(const UserPathFunctionManager &copy) :
   UserFunctionManager(copy),
   paramData                      (NULL),
   pfContainer                    (NULL)
{
   hasFunctions.clear();
   numFunctions.clear();
   for (Integer idx1 = UserFunction::DYNAMICS; idx1 < UserFunction::ALLFUNCTIONS; idx1++)
   {      
      hasFunctions.push_back(copy.hasFunctions[idx1]);
      numFunctions.push_back(copy.numFunctions[idx1]);
   }

   bool isPreserving = false;
   jacobian.clear();
   jacPattern.clear();
   needsJacobianFiniteDiff.resize(UserFunction::ALLFUNCTIONS, UserFunction::ALLJACOBIANS, isPreserving);

   for (Integer idx2 = UserFunction::STATE; idx2 < UserFunction::ALLJACOBIANS; idx2++)
   {
      numVars[idx2] = copy.numVars[idx2];
   }

   for (Integer idx1 = UserFunction::DYNAMICS; idx1 < UserFunction::ALLFUNCTIONS; idx1++)
   {
      std::vector<Rmatrix> tmpJacPattern;
      std::vector<Rmatrix> tmpJacobian;

      for (Integer idx2 = UserFunction::STATE; idx2 < UserFunction::ALLJACOBIANS; idx2++)
      {
         needsJacobianFiniteDiff(idx1, idx2) = copy.needsJacobianFiniteDiff(idx1, idx2);
         if (copy.jacobian[idx1][idx2].IsSized())
            tmpJacobian.push_back(copy.jacobian[idx1][idx2]);

         if (copy.jacPattern[idx1][idx2].IsSized())
            tmpJacPattern.push_back(copy.jacPattern[idx1][idx2]);
         
      }
      jacPattern.push_back(tmpJacPattern);
      jacobian.push_back(tmpJacobian);
   }
   
   if (copy.algFuncUpperBound.IsSized())
      algFuncUpperBound              = copy.algFuncUpperBound;
   if (copy.algFuncLowerBound.IsSized())
      algFuncLowerBound              = copy.algFuncLowerBound;
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
UserPathFunctionManager& UserPathFunctionManager::operator=(
                         const UserPathFunctionManager &copy)
{
   if (&copy == this)
      return *this;
   
   UserFunctionManager::operator=(copy);
   
   paramData                      = NULL;
   pfContainer                    = NULL;

   hasFunctions.clear();
   numFunctions.clear();
   for (Integer idx1 = UserFunction::DYNAMICS; idx1 < UserFunction::ALLFUNCTIONS; idx1++)
   {
      hasFunctions.push_back(copy.hasFunctions[idx1]);
      numFunctions.push_back(copy.numFunctions[idx1]);
   }

   bool isPreserving = false;
   jacobian.clear();
   jacPattern.clear();
   needsJacobianFiniteDiff.resize(UserFunction::ALLFUNCTIONS, UserFunction::ALLJACOBIANS, isPreserving);

   for (Integer idx2 = UserFunction::STATE; idx2 < UserFunction::ALLJACOBIANS; idx2++)
   {
      numVars[idx2] = copy.numVars[idx2];
   }

   for (Integer idx1 = UserFunction::DYNAMICS; idx1 < UserFunction::ALLFUNCTIONS; idx1++)
   {
      std::vector<Rmatrix> tmpJacPattern;
      std::vector<Rmatrix> tmpJacobian;

      for (Integer idx2 = UserFunction::STATE; idx2 < UserFunction::ALLJACOBIANS; idx2++)
      {
         needsJacobianFiniteDiff(idx1, idx2) = copy.needsJacobianFiniteDiff(idx1, idx2);
         if (copy.jacobian[idx1][idx2].IsSized())
            tmpJacobian.push_back(copy.jacobian[idx1][idx2]);

         if (copy.jacPattern[idx1][idx2].IsSized())
            tmpJacPattern.push_back(copy.jacPattern[idx1][idx2]);

      }
      jacPattern.push_back(tmpJacPattern);
      jacobian.push_back(tmpJacobian);
   }
   
   if (copy.algFuncUpperBound.IsSized())
   {
      Integer row= copy.algFuncUpperBound.GetSize();
      algFuncUpperBound.SetSize(row);
      algFuncUpperBound = copy.algFuncUpperBound;
   }
   if (copy.algFuncLowerBound.IsSized())
   {
      Integer row = copy.algFuncLowerBound.GetSize();
      algFuncLowerBound.SetSize(row);
      algFuncLowerBound = copy.algFuncLowerBound;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
UserPathFunctionManager::~UserPathFunctionManager()
{
   // do I need to delete the functionData and the pfContainer here?
   // (probably not as they are passed in)
}

//------------------------------------------------------------------------------
// void Initialize(UserPathFunction          *uData,
//                 FunctionInputData         *pData,
//                 PathFunctionContainer     *fData,
//                 BoundData                 *bData)
//------------------------------------------------------------------------------
/**
 * This method initializes the user path function manager
 *
 * @param <uData>  the user path function
 * @param <pData>  the function input data
 * @param <fData>  the path function container
 * @param <bData>  the bound data
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::Initialize(UserPathFunction          *uData,
                                         FunctionInputData         *pData,
                                         PathFunctionContainer     *fData,
                                         BoundData                 *bData)
{

   isInitializing =true;
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage("Entering UPFM::Initialize\n");
      MessageInterface::ShowMessage(
            "    uData is <%p>, pData is <%p>, fData is <%p>, bData is <%p>\n",
            uData, pData, fData, bData);
      if (!uData) MessageInterface::ShowMessage("uData passed in is NULL?!\n");
   #endif

   // Dimension the optimization parameters - was SetParameterDimensions
   numStateVars   = pData->GetNumStateVars();
   numControlVars = pData->GetNumControlVars();
   numTimeVars    = 1;
   numStaticVars = pData->GetNumStaticVars();

   numVars.clear();
   numVars.push_back(numStateVars);
   numVars.push_back(numControlVars);
   numVars.push_back(numTimeVars);
   numVars.push_back(numStaticVars);

   // Set pointers to the data
   SetParamData(pData);
   SetFunctionData(fData);
   
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
                              "In UPFM::Initialize, about to set userData\n");
   #endif
   // If there is no function, set the flag and return
   if (!uData)
   {
      hasFunction = false;
      return;
   }
   
   hasFunction = true;
   userData    = uData;
   
   SetIsInitializing(true);
   
   // Initialize the user's path function object
   try
   {
      #ifdef DEBUG_MANAGER
         MessageInterface::ShowMessage(
                        "In UPFM::Initialize, about to initialize userData\n");
      #endif
      userData->Initialize(pData, fData);
   } catch (BaseException &be)
   {
      throw LowThrustException(
                  "ERROR initializing user data in UserPathFunctionManager:\n" +
                  be.GetFullMessage());
   }
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
                     "In UPFM::Initialize, about to call InitFunctionData\n");
   #endif
   // Initialize the data members according to the user's problem
   InitFunctionData(fData);
   
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
               "In UPFM::Initialize, about to call ComputeSparsityPatterns\n");
   #endif
   // Compute the sparsity pattern for the user's problem
   ComputeSparsityPatterns(pData, fData, bData);
   
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
                  "In UPFM::Initialize, about to call EvaluateUserFunction\n");
   #endif
   // Re-evaluate at the input values ... sparsity determination
   // made random changes and now we should set back to be
   // consistent with the input
   EvaluateUserFunction(pData, fData);
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
               "In UPFM::Initialize, EvaluateUserFunction has COMPLETED!!\n");
   #endif
   
   isInitializing = false;
   SetIsInitializing(false);
   
   if (!fData->GetAlgData()->BoundsSet())  // something wrong here <<<<<
   {
      MessageInterface::ShowMessage("--- ERROR Alg bounds are not set!!!\n");
   }
   else
   {
      algFuncUpperBound = fData->GetAlgData()->GetUpperBounds();
      algFuncLowerBound = fData->GetAlgData()->GetLowerBounds();
   }
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
               "In UPFM::Initialize, calling CheckIfNeedsFiniteDiffJacobian\n");
   #endif
   CheckIfNeedsFiniteDiffJacobian();
   
   #ifdef DEBUG_MANAGER
      if (jacobian[DYNAMICS][STATE].IsSized())
      {
         MessageInterface::ShowMessage(
               "At end of UPFM::Initialize, jacobian[DYNAMICS][STATE] is SIZED!\n");
         Integer r, c;
         jacobian[DYNAMICS][STATE].GetSize(r,c);
         MessageInterface::ShowMessage("--- size is %d x %d\n", r, c);
         for (Integer rr = 0; rr <r; rr++)
            for (Integer cc = 0; cc < c; cc++)
               MessageInterface::ShowMessage(
                           " ------ jacobian[DYNAMICS][STATE](%d, %d) = %12.10f\n",
                           rr, cc, jacobian[DYNAMICS][STATE](rr, cc));
              
      }
      else
      {
         MessageInterface::ShowMessage("jacobian[DYNAMICS][STATE] is NOT SIZED!\n");
      }
      if (jacPattern[DYNAMICS][STATE].IsSized())
      {
         MessageInterface::ShowMessage("At end of UPFM::Initialize, "
                                 "jacPattern[DYNAMICS][STATE] is SIZED!\n");
         Integer r, c;
         jacPattern[DYNAMICS][STATE].GetSize(r,c);
         MessageInterface::ShowMessage("--- size is %d x %d\n", r, c);
         for (Integer rr = 0; rr <r; rr++)
            for (Integer cc = 0; cc < c; cc++)
               MessageInterface::ShowMessage(
                     " ------ jacPattern[DYNAMICS][STATE](%d, %d) = %12.10f\n",
                     rr, cc, jacobian[DYNAMICS][STATE](rr, cc));
              
      }
      else
      {
         MessageInterface::ShowMessage(
                              "jacPattern[DYNAMICS][STATE] is NOT SIZED!\n");
      }
           
   #endif
   #ifdef DEBUG_WRITE_DATA
      WriteDebugData();
   #endif
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage("Exiting UPFM::Initialize\n");
   #endif
}

//------------------------------------------------------------------------------
// void WriteDebugData()
//------------------------------------------------------------------------------
/**
 * This method writes debug data to the log
 *
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::WriteDebugData()
{
   Integer r, c;

   for (Integer idx1 = UserFunction::DYNAMICS; idx1 < UserFunction::ALLFUNCTIONS; idx1++)
   {
      for (Integer idx2 = UserFunction::STATE; idx2 < UserFunction::ALLJACOBIANS; idx2++)
      {
         MessageInterface::ShowMessage("Jacobian Sparsity of %d-th function type w/r/t %d-th variable type:\n", 
            idx1, idx2);
         jacPattern[idx1][idx2].GetSize(r, c);
         for (Integer rr = 0; rr < r; rr++)
            for (Integer cc = 0; cc < c; cc++)
               MessageInterface::ShowMessage("(%d, %d) = %12.8f\n", rr, cc,
               jacPattern[idx1][idx2](rr, cc));
      }
   }   
}


//------------------------------------------------------------------------------
// PathFunctionContainer* EvaluateUserFunction(FunctionInputData     *pData,
//                                             PathFunctionContainer *fData)
//------------------------------------------------------------------------------
/**
 * This method evaluates the user function given the input data
 *
 * @param <pData>   function input data
 * @param <fData>   path function container
 *
 * @return the updated path function container
 *
 */
//------------------------------------------------------------------------------
PathFunctionContainer* UserPathFunctionManager::EvaluateUserFunction(
                                                FunctionInputData     *pData,
                                                PathFunctionContainer *fData,
                                                UserFunction::FunctionType fType)
{
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
               "Entering UPFM::EvalUserFunction with pData <%p>, fData <%p>\n",
               pData, fData);
      if (!userData)
         MessageInterface::ShowMessage("userData is NULL!!!\n");
   #endif
   try
   {
      pData->SetIsPerturbing(false);
      return userData->EvaluateUserFunction(pData, fData);
      //TBD: return userData->EvaluateUserFunction(pData, fData, fType);
   }
   catch (BaseException &be)
   {
      throw LowThrustException(
                              "User Path Function object failed to evaluate.\n" +
                              be.GetDetails());
   }
}


//------------------------------------------------------------------------------
// PathFunctionContainer* EvaluateUserJacobian(FunctionInputData      *pData,
//                                             PathFunctionContainer  *fData)
//------------------------------------------------------------------------------
/**
 * This method evaluates the user jacobian given the input data
 *
 * @param <pData>   function input data
 * @param <fData>   path function container
 *
 * @return the updated path function container
 */
//------------------------------------------------------------------------------
PathFunctionContainer* UserPathFunctionManager::EvaluateUserJacobian(
                                                FunctionInputData      *pData,
                                                PathFunctionContainer  *fData,
                                                UserFunction::FunctionType fType,
                                                bool isComputingFunctions)
{
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage("Entering UPFM::EvalUserJacobian\n");
      MessageInterface::ShowMessage(
               "   calling EvaluateUserFunction with pData <%p>, fData <%p>\n",
               pData, fData);
   #endif
   if (isComputingFunctions == true)
   {
      // compute user function only when necessary
      fData = EvaluateUserFunction(pData, fData, fType);
   }

   if (!isInitializing)
   {
      pData->SetIsPerturbing(true);
      #ifdef DEBUG_MANAGER
         MessageInterface::ShowMessage("   calling ComputeJacobian\n");
      #endif
      //TBD: compute only required jacobian based on fType
         for (Integer idx1 = UserFunction::DYNAMICS; idx1< UserFunction::ALLFUNCTIONS; idx1++)
      {
         auto funcType = static_cast<UserFunction::FunctionType> (idx1);
         ComputeAll(funcType, pData, fData);
      }      
      
      #ifdef DEBUG_MANAGER
         MessageInterface::ShowMessage(
                                 "   calling userData->EvaluateUserJacobian\n");
      #endif
      userData->EvaluateUserJacobian(pData, fData);
      /* TBD: for the future, the following should be given instead of the above line
      userData->EvaluateUserJacobian(pData, fData, fType, ALLJACOBIANS);
      */
   }
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage("LEAVING UPFM::EvalUserJacobian\n");
   #endif
   return fData;
}

//------------------------------------------------------------------------------
// UserFunctionProperties GetDynFunctionProperties()
//------------------------------------------------------------------------------
/**
 * This method returns the dynamic function properties
 *
 * @return    the dynamic function properties
 *
 */
//------------------------------------------------------------------------------
UserFunctionProperties UserPathFunctionManager::GetDynFunctionProperties()
{
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
                                 "Entering UPFM::GetDynFunctionProperties\n");
   #endif
   UserFunctionProperties dyn;
   for (Integer idx1 = UserFunction::STATE; idx1 < UserFunction::ALLJACOBIANS; idx1++)
   {
      auto jacType = static_cast<UserFunction::JacobianType> (idx1);
      dyn.SetJacobianPattern(jacType, jacPattern[UserFunction::DYNAMICS][jacType]);      
   }
   dyn.SetNumberOfFunctions(numFunctions[UserFunction::DYNAMICS]);
   return dyn;
}

//------------------------------------------------------------------------------
// UserFunctionProperties GetAlgFunctionProperties()
//------------------------------------------------------------------------------
/**
 * This method returns the algebraic function properties
 *
 * @return    the algebraic function properties
 *
 */
//------------------------------------------------------------------------------
UserFunctionProperties UserPathFunctionManager::GetAlgFunctionProperties()
{
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
                                 "Entering UPFM::GetAlgFunctionProperties\n");
   #endif
   UserFunctionProperties alg;
   for (Integer idx1 = UserFunction::STATE; idx1 < UserFunction::ALLJACOBIANS; idx1++)
   {
      auto jacType = static_cast<UserFunction::JacobianType> (idx1);
      alg.SetJacobianPattern(jacType, jacPattern[UserFunction::ALGEBRAIC][jacType]);      
   }
   alg.SetNumberOfFunctions(numFunctions[UserFunction::ALGEBRAIC]);
   return alg;
}

//------------------------------------------------------------------------------
// UserFunctionProperties GetCostFunctionProperties()
//------------------------------------------------------------------------------
/**
 * This method returns the cost function properties
 *
 * @return    the cost function properties
 *
 */
//------------------------------------------------------------------------------
UserFunctionProperties UserPathFunctionManager::GetCostFunctionProperties()
{
   UserFunctionProperties cost;
   for (Integer idx1 = UserFunction::STATE; idx1 < UserFunction::ALLJACOBIANS; idx1++)
   {
      auto jacType = static_cast<UserFunction::JacobianType> (idx1);
      cost.SetJacobianPattern(jacType, jacPattern[UserFunction::COST][jacType]);
   }
   cost.SetNumberOfFunctions(1);
   return cost;
}

//------------------------------------------------------------------------------
// bool HasDynFunctions()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not the manager has dynamic
 * functions
 *
 * @return    true if has dynamic functions; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool UserPathFunctionManager::HasDynFunctions()
{
   return hasFunctions[UserFunction::DYNAMICS];
}

//------------------------------------------------------------------------------
// bool HasAlgFunctions()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not the manager has
 * algebraic functions
 *
 * @return    true if has algebraic functions; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool UserPathFunctionManager::HasAlgFunctions()
{
   return hasFunctions[UserFunction::ALGEBRAIC];
}

//------------------------------------------------------------------------------
// Integer GetNumAlgFunctions()
//------------------------------------------------------------------------------
/**
 * This method returns the number of algebraic functions
 *
 * @return   the number of algebraic functions
 *
 */
//------------------------------------------------------------------------------
Integer UserPathFunctionManager::GetNumAlgFunctions()
{
   return numFunctions[UserFunction::ALGEBRAIC];
}

//------------------------------------------------------------------------------
// const Rvector& GetAlgFunctionsUpperBounds()
//------------------------------------------------------------------------------
/**
 * This method returns the algebraic function upper bounds vector
 *
 * @return   the algebraic function upper bounds vector
 *
 */
//------------------------------------------------------------------------------
const Rvector& UserPathFunctionManager::GetAlgFunctionsUpperBounds()
{
   return algFuncUpperBound;
}

//------------------------------------------------------------------------------
// const Rvector& GetAlgFunctionsLowerBounds()
//------------------------------------------------------------------------------
/**
 * This method returns the algebraic function lower bounds vector
 *
 * @return   the algebraic function lower bounds vector
 *
 */
//------------------------------------------------------------------------------
const Rvector& UserPathFunctionManager::GetAlgFunctionsLowerBounds()
{
   return algFuncLowerBound;
}

//------------------------------------------------------------------------------
// void SetParamData(FunctionInputData *pData)
//------------------------------------------------------------------------------
/**
 * This method sets the function input data
 *
 * @param <pData>   the input function data
 *
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::SetParamData(FunctionInputData *pData)
{
   paramData = pData; // should this be a const reference?
}

//------------------------------------------------------------------------------
// void SetFunctionData(PathFunctionContainer *fData)
//------------------------------------------------------------------------------
/**
 * This method sets the function data
 *
 * @param <fData>   the input path function container
 *
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::SetFunctionData(PathFunctionContainer *fData)
{
   pfContainer = fData;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void CheckIfNeedsFiniteDiffJacobian()
//------------------------------------------------------------------------------
/**
 * This method checks to see if there are any state jacobians that
 * require finite differencing
 *
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::CheckIfNeedsFiniteDiffJacobian()
{
   // Sets flags for which Jacobians need finite differencing
   // Check if there are any state Jacobians that require finite differencing
   bool isPreserving = false;
   needsJacobianFiniteDiff.resize(UserFunction::ALLFUNCTIONS, UserFunction::ALLJACOBIANS, isPreserving);
   for (Integer idx1 = UserFunction::DYNAMICS; idx1 < UserFunction::ALLFUNCTIONS; idx1++)
   {
      for (Integer idx2 = UserFunction::STATE; idx2 < UserFunction::ALLJACOBIANS; idx2++)
      {
         needsJacobianFiniteDiff(idx1, idx2) = false;
      }
      if (hasFunctions[idx1])
      {
         FunctionOutputData *dyn = pfContainer->GetData(static_cast<UserFunction::FunctionType>(idx1));
         for (Integer idx2 = UserFunction::STATE; idx2 < UserFunction::ALLJACOBIANS; idx2++)
         {
            if (!dyn->HasUserJacobian(static_cast<UserFunction::JacobianType>(idx2)))
               needsJacobianFiniteDiff(static_cast<UserFunction::FunctionType>(idx1),
                                       static_cast<UserFunction::JacobianType>(idx2)) = true;
         }         
      }
   }
}

//------------------------------------------------------------------------------
// void InitFunctionData(PathFunctionContainer *toData)
//------------------------------------------------------------------------------
/**
 * This method initializes the path function container
 *
 * @param <toData> the input path function container
 *
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::InitFunctionData(PathFunctionContainer *toData)
{
   InitFunctionsData(UserFunction::DYNAMICS, toData);
   InitFunctionsData(UserFunction::ALGEBRAIC, toData);
   InitFunctionsData(UserFunction::COST, toData);
}

//------------------------------------------------------------------------------
// void InitDynFunctionsData(FunctionType fType, PathFunctionContainer *toData)
//------------------------------------------------------------------------------
/**
 * This method initializes the dynamics path function container
 *
 * @param <fType>  the function type of interest
 * @param <toData> the input dynamics path function container
 *
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::InitFunctionsData(UserFunction::FunctionType fType,
                              PathFunctionContainer *toData)
{
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
                     "Entering UPFM::InitFunctionsData for %s with toData <%p>\n",
                     FunctionTypeNames[fType].c_str(), toData);
   #endif
   FunctionOutputData *funcPt = NULL;
   if (fType == UserFunction::DYNAMICS)
      funcPt = toData->GetDynData();
   else if (fType == UserFunction::ALGEBRAIC)
      funcPt = toData->GetAlgData();
   else if (fType == UserFunction::COST)
   {
      funcPt = toData->GetCostData();
      hasCostFunction = funcPt->HasUserFunction();
   }
   else
   {
      throw LowThrustException("Error initialziing user path function "
         "data, unknown function type received.  Valid function types "
         "are DYNAMICS, ALGEBRAIC, and COST.");
   }

   hasFunctions[fType] = funcPt->HasUserFunction();

   #ifdef DEBUG_MANAGER_FUNCTIONS
      MessageInterface::ShowMessage(
                           "In InitFunctionsData for %s, hasFunctions = %s\n",
                           FunctionTypeNames[fType].c_str(),
                           (hasFunctions[DYNAMICS]? "true" : "false"));
      MessageInterface::ShowMessage(
            "   numFunctions = %d, numStateVars = %d, numControlVars = %d\n",
            numFunctions[fType], numStateVars, numControlVars);
   #endif
   if (hasFunctions[fType])
   {
      Rvector dynData = funcPt->GetFunctionValues();
      numFunctions[fType] = dynData.GetSize();
      for (Integer idx1 = UserFunction::STATE; idx1 < UserFunction::ALLJACOBIANS; idx1++)
      {
         auto jacType = static_cast<UserFunction::JacobianType>(idx1);
         jacPattern[fType][idx1].SetSize(numFunctions[fType], numVars[idx1]);
         jacobian[fType][idx1].SetSize(numFunctions[fType], numVars[idx1]);
      }      
   }
   #ifdef DEBUG_MANAGER
      Integer r, c;
      jacPattern[fType][UserFunction::STATE].GetSize(r,c);
      MessageInterface::ShowMessage(
                           "   jacPattern (%s, %s) size = %d x %d\n",
                           usrData->GetFunctionTypeNames(fType),
                           "state",
                           r, c);
      jacPattern[fType][UserFunction::CONTROL].GetSize(r,c);
      MessageInterface::ShowMessage(
                           "   jacPattern (%s, %s) size = %d x %d\n",
                           usrData->GetFunctionTypeNames(fType),
                           "control",
                           r, c);
      jacPattern[fType][UserFunction::TIME].GetSize(r,c);
      MessageInterface::ShowMessage(
                           "   jacPattern (%s, %s) size = %d x %d\n",
                           usrData->GetFunctionTypeNames(fType),
                           "time",
                           r, c);
      jacPattern[fType][UserFunction::STATIC].GetSize(r,c);
      MessageInterface::ShowMessage(
                           "   jacPattern (%s, %s) size = %d x %d\n",
                           usrData->GetFunctionTypeNames(fType),
                           "static",
                           r, c);
      MessageInterface::ShowMessage("EXITING UPFM::InitFunctionsData for %s\n", 
                                    FunctionTypeNames[fType].c_str());
   #endif
}
   

//------------------------------------------------------------------------------
// void ComputeAll(FunctionInputData      *pData,
//                 PathFunctionContainer  *fData, bool isComputingHess)
//------------------------------------------------------------------------------
/**
 * This method computes the first and second derivatives given the input data
 *
 * @param <pData> the input function data
 * @param <fData> the input path function container
 * @param <isComputingHess> do we need to handle hessian?
 *
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::ComputeAll(UserFunction::FunctionType fType,
                                         FunctionInputData      *pData,
                                         PathFunctionContainer  *fData,
                                         bool isComputingHess)
{
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
            "Entering UPFM::ComputeJacobian for %s with pData <%p>, fData <%p>\n",
            FunctionTypeNames[fType].c_str(), pData, fData);
      MessageInterface::ShowMessage(
            "   and isInitializing = %s, needsJacobianFiniteDiff: %s = %s, %s = %s, %s = %s\n",
            (isInitializing?  "true" : "false"),
            JacobianTypeNames[STATE].c_str(),
            (needsJacobianFiniteDiff(fType,STATE)?  "true" : "false"),
            JacobianTypeNames[CONTROL].c_str(),
            (needsJacobianFiniteDiff(fType, CONTROL) ? "true" : "false"),
            JacobianTypeNames[TIME].c_str(),
            (needsJacobianFiniteDiff(fType, TIME) ? "true" : "false"));
   #endif
   if ((!pData) || (!fData))
      throw LowThrustException(
         "ERROR!  fData or pData passed into ComputeStateJacobian is NULL!\n");
   // If not initializing and all Jacobians are provided, nothing to do
   if ((!isInitializing) && (!needsJacobianFiniteDiff(fType, UserFunction::STATE))
      && (!needsJacobianFiniteDiff(fType, UserFunction::CONTROL))
      && (!needsJacobianFiniteDiff(fType, UserFunction::TIME))
      && (!needsJacobianFiniteDiff(fType, UserFunction::STATIC)))
      return;
   // Save nominal values for later use
   FunctionOutputData *funcPt;
   std::vector<bool> hasUserJacobian;
   hasUserJacobian = fData->GetData(fType)->HasUserJacobian();
   funcPt = fData->GetData(fType);

   Rvector nomValues = funcPt->GetFunctionValues();

   Real nomTime = pData->GetTime();
   Rvector nomStateVector  = pData->GetStateVector();
   Rvector nomControlVector = pData->GetControlVector();
   Rvector nomStaticVector  = pData->GetStaticVector();

   #ifdef DEBUG_MANAGER_VALUES
      if (hasFunctions[COST])
      MessageInterface::ShowMessage(
                        "In ComputeJacobian, nominal %s Value = %s\n",
                        FunctionTypeNames[fType].c_str(),
                        nomValues.ToString(12).c_str());      
   #endif

   // loop-over through jacobian types
   for (Integer idx1 = UserFunction::STATE; idx1 < UserFunction::ALLJACOBIANS; idx1++)
   {
      if (hasUserJacobian[idx1] && !isInitializing)
      {
         // if there exists the user-provided jacobian, do nothing
         continue;
      }
      Integer numVars;
      Real pertSize;
      if (idx1 == UserFunction::STATE)
      {
         numVars = numStateVars;
         pertSize = userData->GetStatePerturbation();
      }
      if (idx1 == UserFunction::CONTROL)
      {
         numVars = numControlVars;
         pertSize = userData->GetControlPerturbation();
      }
      if (idx1 == UserFunction::TIME)
      {
         numVars = 1;
         pertSize = userData->GetTimePerturbation();
      }
      if (idx1 == UserFunction::STATIC)
      {
         numVars = numStaticVars;
         pertSize = userData->GetStaticPerturbation();
      }

      Rvector pertVector(numVars);
      for (Integer ii = 0; ii < numVars; ii++)
         pertVector(ii) = 1.0e-07;

      // Loop over state values and perturb them


      for (Integer ss = 0; ss < numVars; ss++)
      {
         // Perturb the state and recompute user functions
         Rvector deltaVector(numVars);  // values are zeros by default
         deltaVector(ss) = pertVector(ss);
         if (idx1 == UserFunction::STATE)
            pData->SetStateVector(nomStateVector + deltaVector);
         if (idx1 == UserFunction::CONTROL)
            pData->SetControlVector(nomControlVector + deltaVector);
         if (idx1 == UserFunction::TIME)
            pData->SetTime(nomTime + deltaVector[0]);
         if (idx1 == UserFunction::STATIC)
            pData->SetStaticVector(nomStaticVector + deltaVector);
         
         EvaluateUserFunction(pData, fData);

         // Compute and save current row of Jacobian
         Rvector pertValues = funcPt->GetFunctionValues();
         Rvector jacValue = (pertValues - nomValues) / pertVector(ss);
         for (Integer dd = 0; dd < numFunctions[fType]; dd++)
            jacobian[fType][idx1](dd, ss) = jacValue(dd);
         
         if (isComputingHess == true)
         {
            // do something for hessian here
            // saving pertValues??
         }
      }
      
      // Now set the computed Jacobians on the fData object;
      // Set state back to nominal vlaue and evaluate.  This computes the correct
      // analytic partials (for those provided) and avoids noise due to finite ??
      // of constratins that required it.
      if (idx1 == UserFunction::STATE)
      {
         pData->SetStateVector(nomStateVector);
         funcPt->SetJacobian(UserFunction::STATE, jacobian[fType][idx1]);
      }
      if (idx1 == UserFunction::CONTROL)
      {
         pData->SetControlVector(nomControlVector);
         funcPt->SetJacobian(UserFunction::CONTROL, jacobian[fType][idx1]);
      }
      if (idx1 == UserFunction::TIME)
      {
         pData->SetTime(nomTime);
         funcPt->SetJacobian(UserFunction::TIME, jacobian[fType][idx1]);
      }
      if (idx1 == UserFunction::STATIC)
      {
         pData->SetStaticVector(nomStaticVector);
         funcPt->SetJacobian(UserFunction::STATIC, jacobian[fType][idx1]);
      }
   }  
#ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage("EXITING UPFM::ComputeControlJacobian\n");
#endif
}


//------------------------------------------------------------------------------
// void ComputeSparsityPatterns(FunctionInputData     *pData,
//                              PathFunctionContainer *fData,
//                              BoundData             *bData,
//                              bool isComputingHess)
//------------------------------------------------------------------------------
/**
 * This method computes the sparsity patterns
 *
 * @param <pData> the input function data
 * @param <fData> the input path function container
 * @param <bData> the bounds data
 * @param <isComputingHess> do we need to handle hessian?
 *
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::ComputeSparsityPatterns(
                              FunctionInputData     *pData,
                              PathFunctionContainer *fData,
                              BoundData             *bData,
                              bool isComputingHess)
{
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage(
            "Entering ComputeSParsityPattern, pData is <%p>, fData is <%p>, "
            "bData is <%p>\n", pData, fData, bData);
   #endif

   paramData->SetIsSparsity(true);

   // Set analytic sparsity patterns that are already provided
   std::vector<std::vector<Rmatrix>> newJacPatternData =
      userData->EvaluateJacobianPattern();
   for (Integer idx1 = UserFunction::DYNAMICS; idx1 < UserFunction::ALLFUNCTIONS; ++idx1)
   {
      for (Integer idx2 = UserFunction::STATE; idx2 < UserFunction::ALLJACOBIANS; ++idx2)
      {
         Integer numR, numC;
         newJacPatternData[idx1][idx2].GetSize(numR, numC);
         if (numR == 0 && numC == 0) // no user pattern provided
            continue;
         else if (numR != numFunctions[idx1] || numC != numVars[idx2])
            throw LowThrustException("Sparsity pattern failed to evaluate.  "
               "The provided size for a user sparsity pattern does not match "
               "the number of functions and variables.\n");

         jacPattern[idx1][idx2] = newJacPatternData[idx1][idx2];
      }
   }

   //MessageInterface::ShowMessage("Dynamics w.r.t State Before Random Method:\n");
   //for (Integer ii = 0; ii < 7; ++ii)
   //   MessageInterface::ShowMessage(jacPattern[UserFunction::DYNAMICS][UserFunction::STATE].GetRow(ii).ToString() + "\n");

   // YK mod for sparsity pattern determination
   for (Integer idx1 = UserFunction::DYNAMICS; idx1 < UserFunction::ALLFUNCTIONS; ++idx1)
   {
      // convert idx1 to enumeration type.
      auto funcType = static_cast<UserFunction::FunctionType>(idx1);
      ComputeSparsity(funcType, pData, fData,
                           bData->GetTimeUpperBound(),
                           bData->GetTimeLowerBound(),
                           bData->GetStateUpperBound(),
                           bData->GetStateLowerBound(),
                           bData->GetControlUpperBound(),
                           bData->GetControlLowerBound(), 
                           bData->GetStaticUpperBound(),
                           bData->GetStaticLowerBound(),
                           isComputingHess);
   }

   //MessageInterface::ShowMessage("Dynamics w.r.t State After Random Method:\n");
   //for (Integer ii = 0; ii < 7; ++ii)
   //   MessageInterface::ShowMessage(jacPattern[UserFunction::DYNAMICS][UserFunction::STATE].GetRow(ii).ToString() + "\n");

   if (isComputingHess == true)
   {
      // do something for hessian here
   }
   paramData->SetIsSparsity(false);
}

   
//------------------------------------------------------------------------------
// void ComputeSparsity(FunctionType fType, 
//                           FunctionInputData       *pData,
//                           PathFunctionContainer   *uData,
//                           const Rvector           &tUpper,
//                           const Rvector           &tLower,
//                           const Rvector           &stUpper,
//                           const Rvector           &stLower,
//                           const Rvector           &ctrUpper,
//                           const Rvector           &ctrLower,
//                           isComputingHess)
//------------------------------------------------------------------------------
/**
 * This method computes the state sparsity pattern
 *
 * @param <fType>     the function type of the interest
 * @param <pData>     the input function data
 * @param <uData>     the input path function container
 * @param <tUpper>    the time upper bounds
 * @param <tLower>    the time lower bounds
 * @param <stUpper>   the state upper bounds
 * @param <stLower>   the state lower bounds
 * @param <ctrUpper>  the control upper bounds
 * @param <ctrLower>  the control lower bounds
 * @param <isComputingHess> do we need to compute hessian pattern?
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::ComputeSparsity(UserFunction::FunctionType fType,
                                             FunctionInputData       *pData,
                                             PathFunctionContainer   *uData,
                                             const Rvector           &tUpper,
                                             const Rvector           &tLower,
                                             const Rvector           &stUpper,
                                             const Rvector           &stLower,
                                             const Rvector           &ctrUpper,
                                             const Rvector           &ctrLower,
                                             const Rvector           &stcUpper,
                                             const Rvector           &stcLower,
                                             bool isComputingHess)
{
   Real nomTime = pData->GetTime(); // is NOT in MATLAB
   Rvector nomState = pData->GetStateVector(); // is NOT in MATLAB
   Rvector nomControl = pData->GetControlVector(); // is NOT in MATLAB
   Rvector nomStatic;
   if (numStaticVars > 0)
   {
      nomStatic.SetSize(numStaticVars);
      nomStatic = pData->GetStaticVector(); // is NOT in MATLAB
   }
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage("Entering UPFM::ComputeStateSparsity\n");
      MessageInterface::ShowMessage("   -- Evaluate at Lower bound\n");
   #endif

   // Select random points as perturbation points to check for changes in
   // function values
   Rvector randTime, randState, randControl, randStatic;
   randTime = GetRandomVector(tLower, tUpper);
   randState = GetRandomVector(stLower, stUpper);
   if (numControlVars > 0)
      randControl = GetRandomVector(ctrLower, ctrUpper);
   if (numStaticVars > 0)
      randStatic = GetRandomVector(stcLower, stcUpper);

   // Evaluate at lower bound
   pData->SetStateVector(stLower);
   if (numControlVars > 0)
      pData->SetControlVector(ctrLower);
   if (numStaticVars > 0)
      pData->SetStaticVector(stcLower);

   EvaluateUserFunction(pData, uData);
   UpdateSparsityPattern(fType, pData, uData, randTime, randState,
      randControl, randStatic, isComputingHess);
   
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage("   -- Evaluate at Upper bound\n");
   #endif

   // Select random points as perturbation points to check for changes in
   // function values
   randTime = GetRandomVector(tLower, tUpper);
   randState = GetRandomVector(stLower, stUpper);
   if (numControlVars > 0)
      randControl = GetRandomVector(ctrLower, ctrUpper);
   if (numStaticVars > 0)
      randStatic = GetRandomVector(stcLower, stcUpper);

   // Evaluate at upper bound
   pData->SetStateVector(stUpper);
   if (numControlVars > 0)
      pData->SetControlVector(ctrUpper);
   if (numStaticVars > 0)
      pData->SetStaticVector(stcUpper);

   EvaluateUserFunction(pData, uData);
   UpdateSparsityPattern(fType, pData, uData, randTime, randState,
      randControl, randStatic, isComputingHess);
   
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage("   -- Evaluate at random points\n");
   #endif
   // Evaluate at random points
   Integer numRand = 100;
   for (Integer ii = 0; ii < numRand; ii++)
   {
      // Set new random perturbation points
      randTime = GetRandomVector(tLower, tUpper);
      randState = GetRandomVector(stLower, stUpper);
      if (numControlVars > 0)
         randControl = GetRandomVector(ctrLower, ctrUpper);
      if (numStaticVars > 0)
         randStatic = GetRandomVector(stcLower, stcUpper);

      // Set new random nominal points
      Rvector rand = GetRandomVector(tLower, tUpper);
      pData->SetTime(rand[0]);
      pData->SetStateVector(GetRandomVector(stLower, stUpper));
      if (numControlVars > 0)
         pData->SetControlVector(GetRandomVector(ctrLower, ctrUpper));
      if (numStaticVars > 0)
         pData->SetStaticVector(GetRandomVector(stcLower, stcUpper));
      EvaluateUserFunction(pData, uData);
      UpdateSparsityPattern(fType, pData, uData, randTime, randState,
         randControl, randStatic, isComputingHess);
   }

   pData->SetTime(nomTime); // is NOT in MATLAB
   pData->SetStateVector(nomState); // is NOT in MATLAB
   pData->SetControlVector(nomControl); // is NOT in MATLAB
   if (numStaticVars > 0)
      pData->SetStaticVector(nomStatic); // is NOT in MATLAB
   #ifdef DEBUG_MANAGER
      MessageInterface::ShowMessage("EXITing UPFM::ComputeStateSparsity\n");
   #endif
}


//------------------------------------------------------------------------------
// void UpdateStateSparsityPattern(UserFunction::FunctionType fType,
//                                FunctionInputData *pData,
//                                PathFunctionContainer *uData,
//                                const Rvector &randTime,
//                                const Rvector &randState,
//                                const Rvector &randControl,
//                                const Rvector &randStatic,
//                                bool isComputingHess)
//------------------------------------------------------------------------------
/**
 * This method updates the state sparsity pattern
 *
 * @param <fType>        the function type of the interest
 * @param <pData>        the input function data
 * @param <uData>        the input path function container
 * @param <randTime>     vector with a random time value, used as time
 *                       perturbation value
 * @param <randState>    vector with random state values, used as state
 *                       perturbation values
 * @param <randControl>  vector with random control values, used as control
 *                       perturbation values
 * @param <randStatic>   vector with random static values, used as static
 *                       perturbation values
 * @param <isComputingHess> do we need to compute hessian pattern?
 */
//------------------------------------------------------------------------------
void UserPathFunctionManager::UpdateSparsityPattern(UserFunction::FunctionType fType,
                                                   FunctionInputData *pData,
                                                   PathFunctionContainer *uData,
                                                   const Rvector &randTime,
                                                   const Rvector &randState,
                                                   const Rvector &randControl,
                                                   const Rvector &randStatic,
                                                   bool isComputingHess)
{
   if (hasFunctions[fType])
   {
      for (Integer jType = UserFunction::STATE;
         jType < UserFunction::ALLJACOBIANS; jType++)
      {
         if (numVars[jType] > 0)
         {
            // Get the nominal values for the currect jType
            FunctionOutputData *funcPt = uData->GetData(fType);
            Rvector nomVars(numVars[jType]);
            if (jType == UserFunction::STATE)
               nomVars = pData->GetStateVector();
            if (jType == UserFunction::CONTROL)
               nomVars = pData->GetControlVector();
            if (jType == UserFunction::TIME)
               nomVars(0) = pData->GetTime();
            if (jType == UserFunction::STATIC)
               nomVars = pData->GetStaticVector();

            Rvector nomFuncVals = funcPt->GetFunctionValues();
            Rvector deltaVars(numVars[jType]);

            for (Integer ss = 0; ss < numVars[jType]; ++ss)
            {
               // Reset the delta vector to nominal variable values before
               // changing another variable value
               deltaVars = nomVars;

               if (jType == UserFunction::STATE)
               {
                  deltaVars(ss) = randState(ss);
                  pData->SetStateVector(deltaVars);
               }
               if (jType == UserFunction::CONTROL)
               {
                  deltaVars(ss) = randControl(ss);
                  pData->SetControlVector(deltaVars);
               }
               if (jType == UserFunction::TIME)
               {
                  deltaVars(ss) = randTime(ss);
                  pData->SetTime(deltaVars(0));
               }
               if (jType == UserFunction::STATIC)
               {
                  deltaVars(ss) = randStatic(ss);
                  pData->SetStaticVector(deltaVars);
               }

               EvaluateUserFunction(pData, uData);
               Rvector pertFuncVals = funcPt->GetFunctionValues();

               // Look for a change in function values, this indicates places
               // in the Jacobian sparsity pattern that will not be constantly
               // zero
               for (Integer ff = 0; ff < numFunctions[fType]; ++ff)
               {
                  if (nomFuncVals(ff) != pertFuncVals(ff))
                     jacPattern[fType][jType](ff, ss) = 1.0;
               }
            }
         }
      }
   }
}


