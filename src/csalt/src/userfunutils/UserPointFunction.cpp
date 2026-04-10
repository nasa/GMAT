//------------------------------------------------------------------------------
//                              UserPointFunction
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
// Created: 2015.08.14
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------
#include <iostream>
#include "UserPointFunction.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_USER_POINT_FUNCTION

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
UserPointFunction::UserPointFunction() :
   UserFunction(),
   numPhases        (0),
   pfContainer      (NULL)
{
   // std::vector containers are empty initially
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
UserPointFunction::UserPointFunction(const UserPointFunction &copy):
   UserFunction(copy),
   numPhases        (copy.numPhases),
   pfContainer      (NULL)
{
   Integer szInit  = copy.initialInputData.size();
   Integer szFinal = copy.finalInputData.size();
   
   initialInputData.clear();   // delete all of the pointers first???
   finalInputData.clear();     // delete all of the pointers first???
   
   for (Integer ii = 0; ii < szInit; ii++)
      initialInputData.push_back(copy.initialInputData.at(ii));  // or CLONE??
   for (Integer ii = 0; ii < szFinal; ii++)
      finalInputData.push_back(copy.finalInputData.at(ii));      // or CLONE??
   // commented out to avoid double-delete (and crash)  GMT-7025
//   for (Integer ii = 0; ii < optControlFuncVec.size(); ii++)
//      optControlFuncVec.push_back(copy.optControlFuncVec.at(ii));
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
UserPointFunction& UserPointFunction::operator=(const UserPointFunction &copy)
{
   if (&copy == this)
      return *this;
   
   UserFunction::operator=(copy);
   
   numPhases         = copy.numPhases;
   pfContainer       = NULL; // copy.pfContainer; // or CLONE?????  GMT-7025
   
   Integer szInit  = copy.initialInputData.size();
   Integer szFinal = copy.finalInputData.size();
   
   initialInputData.clear();   // delete all of the pointers first???
   finalInputData.clear();     // delete all of the pointers first???
   
   for (Integer ii = 0; ii < szInit; ii++)
      initialInputData.push_back(copy.initialInputData.at(ii));  // or CLONE??
   for (Integer ii = 0; ii < szFinal; ii++)
      finalInputData.push_back(copy.finalInputData.at(ii));      // or CLONE??
// commented out to avoid double-delete (and crash)  GMT-7025
//   for (Integer ii = 0; ii < optControlFuncVec.size(); ii++)
//      optControlFuncVec.push_back(copy.optControlFuncVec.at(ii));

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
UserPointFunction::~UserPointFunction()
{
   initialInputData.clear();   // delete all of the pointers first???
   finalInputData.clear();     // delete all of the pointers first???

   if (pfContainer) delete pfContainer;

   // This is the "double delete crash."  See GMT-6720.
   // Once memory issues are addressed, this commented out code should be
   // either removed or uncommented.  If you see it and this message, the issue
   // hasn't been (fully) addressed.
   // This also contributes to GMT-7025 when un-commented

//   for (Integer ii = 0; ii < optControlFuncVec.size(); ++ii)
//      if (optControlFuncVec.at(ii))
//      {
//         MessageInterface::ShowMessage("   TRYING to delete optControlFuncVec <%p>\n",
//                                       optControlFuncVec.at(ii)); // **********
//         delete optControlFuncVec.at(ii);
//      }
//   optControlFuncVec.clear();
}
//------------------------------------------------------------------------------
//  void Initialize(const std::vector<FunctionInputData*> &initData,
//                  const std::vector<FunctionInputData*> &finalData)
//------------------------------------------------------------------------------
/**
 * This method initializes the UserPointFunction.
 *
 * @param <initData>     the initial data
 * @param <finalData>    the final data
 */
//------------------------------------------------------------------------------
void UserPointFunction::Initialize(
                        const std::vector<FunctionInputData*> &initData,
                        const std::vector<FunctionInputData*> &finalData)
{
   #ifdef DEBUG_USER_POINT_FUNCTION
      MessageInterface::ShowMessage("In UserPointFunction::Initialize\n");
      std::cout << "In UserPointFunction::Initialize\n";
   #endif
   isInitializing  = true;
   
   // Update pointers to data objects
   Integer szInit  = initData.size();
   Integer szFinal = finalData.size();
   
   #ifdef DEBUG_USER_POINT_FUNCTION
      MessageInterface::ShowMessage("   - szInit    =  %d ... \n",
                                    (Integer) szInit);
      MessageInterface::ShowMessage("   - szFinal   =  %d ... \n",
                                    (Integer) szFinal);
   #endif
   initialInputData.clear();   // delete all of the pointers first???
   finalInputData.clear();     // delete all of the pointers first???
   #ifdef DEBUG_USER_POINT_FUNCTION
      MessageInterface::ShowMessage("  Arrays have been cleared!!\n");
      std::cout << "  Arrays have been cleared!!\n";
   #endif
   
   for (Integer ii = 0; ii < szInit; ii++)
   {
      initialInputData.push_back(initData.at(ii));
   }
   for (Integer ii = 0; ii < szFinal; ii++)
      finalInputData.push_back(finalData.at(ii));
   #ifdef DEBUG_USER_POINT_FUNCTION
      MessageInterface::ShowMessage(
               "In UserPointFunction::Initialize, data are set!!!! (%d, %d)\n",
               szInit, szFinal);
      std::cout << "In UserPointFunction::Initialize, data are set!!!!\n";
   #endif

   numPhases = szInit;
   
   // Create data containers
   if (pfContainer) delete pfContainer;
   pfContainer = new PointFunctionContainer();
   pfContainer->Initialize();
   
   #ifdef DEBUG_USER_POINT_FUNCTION
      MessageInterface::ShowMessage("In UserPointFunction::Initialize, "
                                    "calling EvaluateFunctions ...\n");
      std::cout << "      calling EvaluateFunctions ...\n";
   #endif
   // Call evaluate on the user's function to populate data
   //EvaluateFunctions();
   EvaluateUserFunction();
   #ifdef DEBUG_USER_POINT_FUNCTION
      MessageInterface::ShowMessage("In UserPointFunction::Initialize, "
                                    "calling EvaluateJacobians ...\n");
      std::cout << "      calling EvaluateJacobians ...\n";
   #endif
   EvaluateJacobians();
   
   // Set IsInitializing to false
   FunctionOutputData *cost = pfContainer->GetCostData();
   FunctionOutputData *alg  = pfContainer->GetAlgData();
   
   cost->SetIsInitializing(false);
   alg->SetIsInitializing(false);
   
   // Set flags according to user configuration
   hasCostFunction = cost->HasUserFunction();
   hasAlgFunctions = alg->HasUserFunction();
   numAlgFunctions = alg->GetNumFunctions();
   
   isInitializing  = false;
   #ifdef DEBUG_USER_POINT_FUNCTION
      MessageInterface::ShowMessage("EXITING UserPointFunction::Initialize\n");
      std::cout << "EXITING UserPointFunction::Initialize\n";
   #endif
}

//------------------------------------------------------------------------------
// void AddFunctions(std::vector<OptimalControlFunction*> funcList)
//------------------------------------------------------------------------------
/**
* This method adds OptimalControlFunction objects to the total list of
* functions
*
* @param funcList The list of OptimalControlFunction objects to add
*/
//------------------------------------------------------------------------------
void UserPointFunction::AddFunctions(
   std::vector<OptimalControlFunction*> funcList)
{
   for (Integer i = 0; i < funcList.size(); ++i)
      optControlFuncVec.push_back(funcList.at(i));
}

//------------------------------------------------------------------------------
// bool EvaluateUserFunction();
//------------------------------------------------------------------------------
/**
 * This method evaluates the user function
 *
 * @return   true if successful; false, otherwise
 */
//------------------------------------------------------------------------------
bool UserPointFunction::EvaluateUserFunction()
{
   try
   {
      #ifdef DEBUG_USER_POINT_FUNCTION
         MessageInterface::ShowMessage(
                  "*** In UserPointFunction::EvaluateUserFunction, "
                  "calling EvaluateFunctions ...\n");
      #endif

      // Initialize functions to an empty vector
      Rvector emptyVec(0);
      SetFunctions(ALGEBRAIC, emptyVec);
      SetFunctionBounds(ALGEBRAIC, LOWER, emptyVec);
      SetFunctionBounds(ALGEBRAIC, UPPER, emptyVec);

      Integer numFuncs = 0;
      std::vector<Rvector> funcArray, lowerBoundsArray, upperBoundsArray;
      EvaluateFunctions();

      FunctionOutputData *algData = pfContainer->GetAlgData();
      funcArray.push_back(algData->GetFunctionValues());
      lowerBoundsArray.push_back(algData->GetLowerBounds());
      upperBoundsArray.push_back(algData->GetUpperBounds());
      numFuncs += algData->GetFunctionValues().GetSize();
      
      Integer numPoints, phaseIdx;
      std::string position;
      // Loop through each optimal control function object to evaluate
      // functions and accumulate their values and bounds
      for (Integer i = 0; i < optControlFuncVec.size(); ++i)
      {
         numPoints = optControlFuncVec.at(i)->GetNumPoints();

         // Send the required data for each point in the optimal control function
         for (Integer j = 0; j < numPoints; ++j)
         {
            Rvector time(1), state, param;
            optControlFuncVec.at(i)->GetPointData(j, phaseIdx, position);
            if (position == "Initial")
            {
               time(0) = GetInitialTime(phaseIdx);
               state = GetInitialStateVector(phaseIdx);
            }
            else if (position == "Final")
            {
               time(0) = GetFinalTime(phaseIdx);
               state = GetFinalStateVector(phaseIdx);
            }
            param = GetStaticVector(phaseIdx);

            optControlFuncVec.at(i)->SetData(OptimalControlFunction::TIME, j,
               time);
            optControlFuncVec.at(i)->SetData(OptimalControlFunction::STATE, j,
               state);
            optControlFuncVec.at(i)->SetData(OptimalControlFunction::STATIC, j,
               param);
         }

         funcArray.push_back(optControlFuncVec.at(i)->EvaluateFunctions());
         numFuncs += funcArray.back().GetSize();

         lowerBoundsArray.push_back(optControlFuncVec.at(i)->GetLowerBounds());
         upperBoundsArray.push_back(optControlFuncVec.at(i)->GetUpperBounds());
      }
      
      // Combine all accumulated functions to one vector
      Rvector allFuncs(numFuncs), allLowerBounds(numFuncs),
         allUpperBounds(numFuncs);
      Integer funcIdx = 0;
      for (Integer i = 0; i < funcArray.size(); ++i)
      {
         for (Integer j = 0; j < funcArray.at(i).GetSize(); ++j)
         {
            allFuncs(funcIdx) = funcArray.at(i)(j);
            allLowerBounds(funcIdx) = lowerBoundsArray.at(i)(j);
            allUpperBounds(funcIdx) = upperBoundsArray.at(i)(j);
            ++funcIdx;
         }
      }
      SetFunctions(ALGEBRAIC, allFuncs);
      SetFunctionBounds(ALGEBRAIC, LOWER, allLowerBounds);
      SetFunctionBounds(ALGEBRAIC, UPPER, allUpperBounds);
   }
   catch (BaseException &be)
   {
      throw LowThrustException("User Point Function failed to evaluate.\n" +
         be.GetDetails());
   }
   return true;
}

//------------------------------------------------------------------------------
// bool EvaluateUserJacobian()
//------------------------------------------------------------------------------
/**
 * This method evaluates the user jacobian
 *
 * @return   true if successful; false, otherwise
 */
//------------------------------------------------------------------------------
bool UserPointFunction::EvaluateUserJacobian()
{
   // Evaluate the functions at the nominal point
   try
   {
      #ifdef DEBUG_USER_POINT_FUNCTION
         MessageInterface::ShowMessage("In UserPointFunction::"
                                       "EvaluateUserJacobian, calling "
                                       "EvaluateUserFunction ...\n");
      #endif
      EvaluateUserFunction();
      #ifdef DEBUG_USER_POINT_FUNCTION
         MessageInterface::ShowMessage("In UserPointFunction::"
                                       "EvaluateUserJacobian, calling "
                                       "EvaluateJacobians ...\n");
      #endif
      //EvaluateJacobians();
      
      Integer numPoints, phaseIdx, numFuncs = 0;
      std::string position;
      for (Integer i = 0; i < optControlFuncVec.size(); ++i)
      {
         numPoints = optControlFuncVec.at(i)->GetNumPoints();

         // Send the required data for each point in the algebraic function
         for (Integer j = 0; j < numPoints; ++j)
         {
            Rvector time(1), state, param;
            optControlFuncVec.at(i)->GetPointData(j, phaseIdx, position);
            if (position == "Initial")
            {
               time(0) = GetInitialTime(phaseIdx);
               state = GetInitialStateVector(phaseIdx);
            }
            else if (position == "Final")
            {
               time(0) = GetFinalTime(phaseIdx);
               state = GetFinalStateVector(phaseIdx);
            }
            if (initialInputData.at(phaseIdx)->GetNumStaticVars() > 0)
               param = GetStaticVector(phaseIdx);
            else
               param.SetSize(0);

            optControlFuncVec.at(i)->SetData(OptimalControlFunction::TIME, j,
               time);
            optControlFuncVec.at(i)->SetData(OptimalControlFunction::STATE, j,
               state);
            optControlFuncVec.at(i)->SetData(OptimalControlFunction::STATIC, j,
               param);
         }

         optControlFuncVec.at(i)->EvaluateJacobian();
      }
   }
   catch (BaseException &be)
   {
      throw LowThrustException(
                     "User Point Function failed to evaluate jacobians.\n");
   }
   return true;
}

//------------------------------------------------------------------------------
// bool ValidatePhaseIndex(Integer idx);
//------------------------------------------------------------------------------
/**
 * This method validates the given phase index
 *
 * @return   true if successful; false, otherwise
 */
//------------------------------------------------------------------------------
bool UserPointFunction::ValidatePhaseIndex(Integer idx)
{
   if ((idx < 0) || (idx >= numPhases))
      return false;
   return true;
}

//------------------------------------------------------------------------------
// void SetFunctionData(PointFunctionContainer *toData);
//------------------------------------------------------------------------------
/**
 * This method sets the function data
 *
 * @param <toData>  the data to use
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetFunctionData(PointFunctionContainer *toData)
{
   pfContainer = toData; // @todo Add validation
}



//------------------------------------------------------------------------------
// PointFunctionContainer*    GetFunctionData();
//------------------------------------------------------------------------------
/**
 * This method returns the function data
 *
 * @return  the function data
 */
//------------------------------------------------------------------------------
PointFunctionContainer* UserPointFunction::GetFunctionData()
{
   return pfContainer;
}

//------------------------------------------------------------------------------
// Rvector GetInitialStateVector(Integer phaseIdx);
//------------------------------------------------------------------------------
/**
 * This method returns the initial state vector for the given phase
 *
 * @param <phaseIdx>  the phase index
 *
 * @return  the initial state vector
 */
//------------------------------------------------------------------------------
const Rvector& UserPointFunction::GetInitialStateVector(Integer phaseIdx)
{
   if (!ValidatePhaseIndex(phaseIdx))
   {
      throw LowThrustException(
                               "User Point Function - invalid phase index.\n");
   }
   return initialInputData.at(phaseIdx)->GetStateVector();
}

//------------------------------------------------------------------------------
// Rvector GetFinalStateVector(Integer phaseIdx);
//------------------------------------------------------------------------------
/**
 * This method returns the final state vector for the given phase
 *
 * @param <phaseIdx>  the phase index
 *
 * @return  the final state vector
 */
//------------------------------------------------------------------------------
const Rvector& UserPointFunction::GetFinalStateVector(Integer phaseIdx)
{
   if (!ValidatePhaseIndex(phaseIdx))
   {
      throw LowThrustException(
                               "User Point Function - invalid phase index.\n");
   }
   return finalInputData.at(phaseIdx)->GetStateVector();
}

//------------------------------------------------------------------------------
//  const Rvector& GetStaticVector()
//------------------------------------------------------------------------------
/**
* This method returns the Static vector
*
* @return the Static vector of the object's paramData
*/
//------------------------------------------------------------------------------
const Rvector& UserPointFunction::GetStaticVector(Integer phaseIdx)
{
   return initialInputData.at(phaseIdx)->GetStaticVector();
}

//------------------------------------------------------------------------------
//  const Rvector& GetCostFunctionValue()
//------------------------------------------------------------------------------
/**
* This method returns the cost function value
*
* @return the cost value
*/
//------------------------------------------------------------------------------
const Rvector& UserPointFunction::GetCostFunctionValue()
{
   FunctionOutputData *costData = pfContainer->GetCostData();
   return costData->GetFunctionValues();
}

//------------------------------------------------------------------------------
// Real GetInitialTime(Integer phaseIdx);
//------------------------------------------------------------------------------
/**
 * This method returns the initial time for the given phase
 *
 * @param <phaseIdx>  the phase index
 *
 * @return  the initial time
 */
//------------------------------------------------------------------------------
Real UserPointFunction::GetInitialTime(Integer phaseIdx)
{
   if (!ValidatePhaseIndex(phaseIdx))
   {
      throw LowThrustException(
                               "User Point Function - invalid phase index.\n");
   }
   return initialInputData.at(phaseIdx)->GetTime();
}

//------------------------------------------------------------------------------
// Real GetFinalTime(Integer phaseIdx);
//------------------------------------------------------------------------------
/**
 * This method returns the final time for the given phase
 *
 * @param <phaseIdx>  the phase index
 *
 * @return  the final time
 */
//------------------------------------------------------------------------------
Real UserPointFunction::GetFinalTime(Integer phaseIdx)
{
   if (!ValidatePhaseIndex(phaseIdx))
   {
      throw LowThrustException(
                               "User Point Function - invalid phase index.\n");
   }
   return finalInputData.at(phaseIdx)->GetTime();
}

//------------------------------------------------------------------------------
// const StringArray& GetAlgFunctionNames()
//------------------------------------------------------------------------------
/**
 * This method returns the algebraic function names
 *
 * @return  the algebraic function names
 */
//------------------------------------------------------------------------------
const StringArray& UserPointFunction::GetAlgFunctionNames()
{
   FunctionOutputData *algData  = pfContainer->GetAlgData();
   
   return algData->GetFunctionNames();
}

//------------------------------------------------------------------------------
// const std::vector<OptimalControlFunction*>& GetOptControlFunctionObjects()
//------------------------------------------------------------------------------
/**
* This method returns the optimal control function objects
*
* @return  the optimal control function objects
*/
//------------------------------------------------------------------------------
const std::vector<OptimalControlFunction*>& UserPointFunction::GetOptControlFunctionObjects()
{
   return optControlFuncVec;
}

//------------------------------------------------------------------------------
// void SetAlgFunctions(const Rvector &toValues);
//------------------------------------------------------------------------------
/**
 * This method sets the algebraic functions
 *
 * @param <toValues>  the algebraic functions
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetAlgFunctions(const Rvector &toValues)
{
   FunctionOutputData *alg  = pfContainer->GetAlgData();
   alg->SetFunctions(toValues);
}

//------------------------------------------------------------------------------
// void SetFunction(const Rvector& toValues)
//------------------------------------------------------------------------------
/**
 * This method sets functions - currently, either dynamics or cost.
 *
 * @param <type> the type of function to set (DYNAMICS or COST)
 * @param <toValues> the cost function
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetFunctions(FunctionType type, const Rvector& toValues)
{
   if (type == COST)
   {
      FunctionOutputData *costData = pfContainer->GetCostData();
      costData->SetFunctions(toValues);
   } else if (type == ALGEBRAIC)
   {
      FunctionOutputData *algData = pfContainer->GetAlgData();
      algData->SetFunctions(toValues);
   } 
}

//------------------------------------------------------------------------------
// void SetAlgUpperBounds(const Rvector &toValues);
//------------------------------------------------------------------------------
/**
 * This method sets the algebraic functions upper bounds
 *
 * @param <toValues>  the algebraic functions upper bounds
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetAlgUpperBounds(const Rvector &toValues)
{
   FunctionOutputData *alg  = pfContainer->GetAlgData();
   alg->SetUpperBounds(toValues);
}

//------------------------------------------------------------------------------
// void SetAlgLowerBounds(const Rvector &toValues);
//------------------------------------------------------------------------------
/**
 * This method sets the algebraic functions lower bounds
 *
 * @param <toValues>  the algebraic functions lower bounds
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetAlgLowerBounds(const Rvector &toValues)
{
   #ifdef DEBUG_USER_POINT_FUNCTION
      MessageInterface::ShowMessage(
                        "In UserPointFunction::SetAlgLowerBounds\n");
      std::cout << "In UserPointFunction::SetAlgLowerBounds\n";
   #endif
   FunctionOutputData *alg  = pfContainer->GetAlgData();
   #ifdef DEBUG_USER_POINT_FUNCTION
   if (!alg)
   {
      MessageInterface::ShowMessage("alg is NULL!!!!!\n");
      std::cout << "alg is NULL!!!!!\n";
   }
   #endif
   alg->SetLowerBounds(toValues);
}

//------------------------------------------------------------------------------
// void SetFunctionBounds(FunctionType type, FunctionBound bound, const Rvector& toValues)
//------------------------------------------------------------------------------
/**
 * This method sets function bounds
 *
 * @param <type> the type of function being set
 * @param <bound> the type of bound being set (UPPER or LOWER)
 * @param <toValues> the lower bounds of the algebraic functions
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetFunctionBounds(FunctionType type, FunctionBound bound, const Rvector& toValues)
{
   if (type == ALGEBRAIC && bound == UPPER) {
      FunctionOutputData *algData = pfContainer->GetAlgData();
      algData->SetUpperBounds(toValues);
   } else if (type == ALGEBRAIC && bound == LOWER) {
      FunctionOutputData *algData  = pfContainer->GetAlgData();
      algData->SetLowerBounds(toValues);
   }
}

//------------------------------------------------------------------------------
// void SetAlgFunctionNames(const StringArray& toNames)
//------------------------------------------------------------------------------
/**
 * This method sets the algebraic functions names
 *
 * @param <toNames>  the algebraic functions names
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetAlgFunctionNames(const StringArray& toNames)
{
   FunctionOutputData *algData  = pfContainer->GetAlgData();
   
   algData->SetFunctionNames(toNames);
}

//------------------------------------------------------------------------------
// void SetFunctionNames(FunctionType type, const StringArray& toNames)
//------------------------------------------------------------------------------
/**
 * This method sets functions names for various types of functions. Currently only
 * supports Algebraic
 *
 * @param <type> the type of function. Currently, only ALGEBRAIC.
 * @param <toNames> the names of the algebraic functions
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetFunctionNames(FunctionType type, const StringArray& toNames)
{
   if (type == ALGEBRAIC) {
      FunctionOutputData *algData = pfContainer->GetAlgData();
         algData->SetFunctionNames(toNames);
   }
}

//------------------------------------------------------------------------------
// void SetAlgStateJacobian(const Rmatrix &toValue);
//------------------------------------------------------------------------------
/**
 * This method sets the algebraic state jacobian
 *
 * @param <toValue>  the algebraic state jacobian
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetAlgStateJacobian(const Rmatrix &toValue)
{
   FunctionOutputData *alg  = pfContainer->GetAlgData();
   alg->SetJacobian(UserFunction::STATE, toValue);
}

//------------------------------------------------------------------------------
// void SetJacobian(const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
 * This method sets jacobians - currently, either Cost State or Cost Control.
 *
 * @param <funcType> the type of function. Currently, only COST.
 * @param <jacType> the type of jacobian (STATE or CONTROL)
 * @param <toValues> the cost control jacobian
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetJacobian(FunctionType funcType, JacobianType jacType,
      const Rmatrix& toJacobian)
{
   if (funcType == COST)
   {
      FunctionOutputData *costData = pfContainer->GetCostData();
      costData->SetJacobian(jacType, toJacobian);
   } 
   else if (funcType == ALGEBRAIC)
   {
      FunctionOutputData *algData = pfContainer->GetAlgData();
      algData->SetJacobian(jacType, toJacobian);
   }
   else if (funcType == DYNAMICS)
   {
      throw LowThrustException(
         "User Point Function - Invalid function type 'DYNAMICS'.\n");
   }
}

//------------------------------------------------------------------------------
// void SetAlgTimeJacobian(const Rmatrix &toValue);
//------------------------------------------------------------------------------
/**
 * This method sets the algebraic time jacobian
 *
 * @param <toValue>  the algebraic time jacobian
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetAlgTimeJacobian(const Rmatrix &toValue)
{
   FunctionOutputData *alg  = pfContainer->GetAlgData();
   alg->SetJacobian(UserFunction::TIME, toValue);
}

//------------------------------------------------------------------------------
// void SetCostFunction(const Rvector &toValue);
//------------------------------------------------------------------------------
/**
 * This method sets the cost function
 *
 * @param <toValue>  the cost function
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetCostFunction(const Rvector &toValue)
{
   FunctionOutputData *cost  = pfContainer->GetCostData();
   cost->SetFunctions(toValue);
}

//------------------------------------------------------------------------------
// void SetCostStateJacobian(const Rmatrix &toValue);
//------------------------------------------------------------------------------
/**
 * This method sets the cost state jacobian
 *
 * @param <toValue>  the cost state jacobian
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetCostStateJacobian(const Rmatrix &toValue)
{
   FunctionOutputData *cost  = pfContainer->GetCostData();
   cost->SetJacobian(UserFunction::STATE, toValue);
}

//------------------------------------------------------------------------------
// void SetCostTimeJacobian(const Rmatrix &toValue);
//------------------------------------------------------------------------------
/**
 * This method sets the cost time jacobian
 *
 * @param <toValue>  the cost time jacobian
 */
//------------------------------------------------------------------------------
void UserPointFunction::SetCostTimeJacobian(const Rmatrix &toValue)
{
   FunctionOutputData *cost  = pfContainer->GetCostData();
   cost->SetJacobian(UserFunction::TIME, toValue);
}
