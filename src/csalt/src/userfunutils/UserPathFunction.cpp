//------------------------------------------------------------------------------
//                              UserPathFunction
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
// Created: 2015.07.16
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "UserPathFunction.hpp"
#include "LowThrustException.hpp"
#include "FunctionOutputData.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_USER_PATH_FUNCTION
//#define DEBUG_USER_PATH_JACOBIAN

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
UserPathFunction::UserPathFunction() :
   UserFunction(),
   numDynFunctions   (0),
   hasDynFunctions   (false),
   paramData         (NULL),
   pfContainer       (NULL)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
UserPathFunction::UserPathFunction(const UserPathFunction &copy):
   UserFunction(copy),
   numDynFunctions   (copy.numDynFunctions),
   hasDynFunctions   (copy.hasDynFunctions),
   paramData         (NULL),
   pfContainer       (NULL)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
UserPathFunction& UserPathFunction::operator=(const UserPathFunction &copy)
{
   if (&copy == this)
      return *this;
   
   UserFunction::operator=(copy);
   
   numDynFunctions   = copy.numDynFunctions;
   hasDynFunctions   = copy.hasDynFunctions;
   paramData         = copy.paramData;       // or CLONE?????
   pfContainer       = copy.pfContainer;     // or CLONE?????
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
UserPathFunction::~UserPathFunction()
{
//   if (paramData)    delete paramData;  // should we do this?
//   if (pfContainer)  delete pfContainer;
}

//------------------------------------------------------------------------------
//  void Initialize(FunctionInputData     *pd,
//                  PathFunctionContainer *pfc)
//------------------------------------------------------------------------------
/**
 * This method initializes the UserPathFunction.
 *
 * @param <pd>     the function input data
 * @param <pfc>    the path function container
 */
//------------------------------------------------------------------------------
void UserPathFunction::Initialize(FunctionInputData     *pd,
                                  PathFunctionContainer *pfc)
{
   #ifdef DEBUG_USER_PATH_FUNCTION
      MessageInterface::ShowMessage(
                              "Entering UserPathFunction::Initialize ... \n");
   #endif
   
   isInitializing = true;
   
   paramData     = pd;
   pfContainer   = pfc;
   
   // Call evaluate on the user's Function to populate data
   paramData->SetIsPerturbing(false);

   #ifdef DEBUG_USER_PATH_FUNCTION
      MessageInterface::ShowMessage("In UserPathFunction::Initialize, "
                                    "about to call Evaluate methods ... \n");
   #endif
   EvaluateFunctions();
   EvaluateJacobians();
   
   #ifdef DEBUG_USER_PATH_FUNCTION
      MessageInterface::ShowMessage("In UserPathFunction::Initialize, "
                                    "getting dyn, alg, cost data ... \n");
   #endif
   FunctionOutputData *dynData  = pfContainer->GetDynData();
   FunctionOutputData *costData = pfContainer->GetCostData();
   FunctionOutputData *algData  = pfContainer->GetAlgData();
   
   costData->SetIsInitializing(false);
   dynData->SetIsInitializing(false);
   algData->SetIsInitializing(false);
   
   hasCostFunction = costData->HasUserFunction();
   hasDynFunctions = dynData->HasUserFunction();
   numDynFunctions = dynData->GetNumFunctions();
   hasAlgFunctions = algData->HasUserFunction();
   numAlgFunctions = algData->GetNumFunctions();
   
   isInitializing  = false;

   #ifdef DEBUG_USER_PATH_FUNCTION
      MessageInterface::ShowMessage(
            "EXITING In UserPathFunction::Initialize ... \n");
   #endif
}


//------------------------------------------------------------------------------
//  PathFunctionContainer* EvaluateUserFunction(FunctionInputData     *pd,
//                                              PathFunctionContainer *pfc)
//------------------------------------------------------------------------------
/**
 * This method evaluates the user function
 *
 * @param <pd>     the function input data
 * @param <pfc>    the path function container
 */
//------------------------------------------------------------------------------
PathFunctionContainer* UserPathFunction::EvaluateUserFunction(
                                         FunctionInputData     *pd,
                                         PathFunctionContainer *pfc)
{
   #ifdef DEBUG_USER_PATH_FUNCTION
      MessageInterface::ShowMessage(
            "Entering UserPathFunction::EvaluateUserFunction ... \n");
      MessageInterface::ShowMessage("   pd <%p>, pfc <%p>\n", pd, pfc);
   #endif

   // Old code that does not seem to be used anymore, should it be deleted?
   /*
   FunctionOutputData *alg = pfContainer->GetAlgData();
   if (!alg->BoundsSet())
   {
//      MessageInterface::ShowMessage(
//               "Lower and/or upper alg bounds have not been set.\n");
//      throw LowThrustException(
//               "Lower and upper bounds for algebraic data are not set.\n");
   }
   */

   paramData     = pd;
   pfContainer   = pfc;

   try
   {
      #ifdef DEBUG_USER_PATH_FUNCTION
         MessageInterface::ShowMessage("In UserPathFunction::"
                                       "EvaluateUserFunction, about to call "
                                       "EvaluateFunctions ... \n");
      #endif
      EvaluateFunctions();
   }
   catch (BaseException &be)
   {
      throw LowThrustException("User Path Function failed to evaluate.\n" +
         be.GetDetails());
   }
   return pfContainer;
}


//------------------------------------------------------------------------------
//  PathFunctionContainer* EvaluateUserJacobian(FunctionInputData     *pd,
//                                              PathFunctionContainer *pfc)
//------------------------------------------------------------------------------
/**
 * This method evaluates the user jacobian
 *
 * @param <pd>     the function input data
 * @param <pfc>    the path function container
 */
//------------------------------------------------------------------------------
PathFunctionContainer* UserPathFunction::EvaluateUserJacobian(
                                         FunctionInputData *pd,
                                         PathFunctionContainer *pfc)
{
   #ifdef DEBUG_USER_PATH_JACOBIAN
      MessageInterface::ShowMessage(
                              "Entering UPF::EvaluateUserJacobian ...\n");
   #endif
   try
   {
      EvaluateUserFunction(pd, pfc);
      #ifdef DEBUG_USER_PATH_JACOBIAN
         MessageInterface::ShowMessage(
                              " --- Done with EvaluateUserFunciton ...\n");
      #endif
      EvaluateJacobians();
   }
   catch (BaseException &be)
   {
      throw LowThrustException(
            "User Path Function failed to evaluate jacobians.\n");
   }
   #ifdef DEBUG_USER_PATH_JACOBIAN
      MessageInterface::ShowMessage("LEAVING UPF::EvaluateUserJacobian ...\n");
   #endif
   return pfContainer;
}

//------------------------------------------------------------------------------
//  void SetParamData(FunctionInputData *pd)
//------------------------------------------------------------------------------
/**
 * This method sets the parameter data
 *
 * @param <pd>     the function input data
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetParamData(FunctionInputData *pd)
{
   paramData = pd;  /// @todo Add validation
}

//------------------------------------------------------------------------------
//  void SetFunctionData(PathFunctionContainer *pfc)
//------------------------------------------------------------------------------
/**
 * This method sets the function data
 *
 * @param <pfc>    the path function container
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetFunctionData(PathFunctionContainer *pfc)
{
   pfContainer = pfc;  // @todo Add validation
}

//------------------------------------------------------------------------------
//  const Rvector& GetStateVector()
//------------------------------------------------------------------------------
/**
 * This method returns the state vector
 *
 * @return the state vector of the object's paramData
 */
//------------------------------------------------------------------------------
const Rvector& UserPathFunction::GetStateVector()
{
   return paramData->GetStateVector();
}

//------------------------------------------------------------------------------
// const Rvector& GetControlVector()
//------------------------------------------------------------------------------
/**
 * This method returns the control vector
 *
 * @return the control vector of the object's paramData
 */
//------------------------------------------------------------------------------
const Rvector& UserPathFunction::GetControlVector()
{
   return paramData->GetControlVector();
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
const Rvector& UserPathFunction::GetStaticVector()
{
   return paramData->GetStaticVector();
}

//------------------------------------------------------------------------------
// Real GetTime()
//------------------------------------------------------------------------------
/**
 * This method returns the time
 *
 * @return the time of the object's paramData
 */
//------------------------------------------------------------------------------
Real UserPathFunction::GetTime()
{
   return paramData->GetTime();
}

//------------------------------------------------------------------------------
// Integer GetPhaseNumber()
//------------------------------------------------------------------------------
/**
 * This method returns the phase number
 *
 * @return the phase number of the object's paramData
 */
//------------------------------------------------------------------------------
Integer UserPathFunction::GetPhaseNumber()
{
   return paramData->GetPhaseNum();
}

//------------------------------------------------------------------------------
// std::vector<std::vector<Rmatrix>> EvaluateJacobianPattern()
//------------------------------------------------------------------------------
/**
 * This method sets known analytic values for the sparsity pattern of the
 * various Jacobians
 *
 * @return The analytic sparsity patterns
 */
//------------------------------------------------------------------------------
std::vector<std::vector<Rmatrix>> UserPathFunction::EvaluateJacobianPattern()
{
   // If this version of the method is called, an analytic pattern was not 
   // provided, so do nothing here
   Rmatrix emptyMat(0, 0);
   std::vector<std::vector<Rmatrix>> jacPattern;

   // Set sizing of the various Jacobian types
   for (Integer ii = DYNAMICS; ii < ALLFUNCTIONS; ++ii)
   {
      std::vector<Rmatrix> tmpVec;
      for (Integer jj = STATE; jj < ALLJACOBIANS; ++jj)
      {
         tmpVec.push_back(emptyMat);
      }
      jacPattern.push_back(tmpVec);
   }

   return jacPattern;
}

//------------------------------------------------------------------------------
// void SetAlgFunctions(const Rvector& toValues)
//------------------------------------------------------------------------------
/**
 * This method sets the algebraic functions
 *
 * @param <toValues> the algebraic functions
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetAlgFunctions(const Rvector& toValues)
{
   FunctionOutputData *algData  = pfContainer->GetAlgData();
   
   algData->SetFunctions(toValues);
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
void UserPathFunction::SetFunctions(FunctionType type, const Rvector& toValues)
{
	if (type == DYNAMICS)
	{
		FunctionOutputData *dynData = pfContainer->GetDynData();
		dynData->SetFunctions(toValues);
	} 
   else if (type == COST)
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
// void SetAlgFunctionsUpperBounds(const Rvector& toValues)
//------------------------------------------------------------------------------
/**
 * This method sets the algebraic functions upper bounds
 *
 * @param <toValues> the upper bounds of the algebraic functions
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetAlgFunctionsUpperBounds(const Rvector& toValues)
{
   FunctionOutputData *algData  = pfContainer->GetAlgData();

   algData->SetUpperBounds(toValues);
}

//------------------------------------------------------------------------------
// void SetAlgFunctionsLowerBounds(const Rvector& toValues)
//------------------------------------------------------------------------------
/**
 * This method sets the algebraic functions lower bounds
 *
 * @param <toValues> the lower bounds of the algebraic functions
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetAlgFunctionsLowerBounds(const Rvector& toValues)
{
   FunctionOutputData *algData  = pfContainer->GetAlgData();
   
   algData->SetLowerBounds(toValues);
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
void UserPathFunction::SetFunctionBounds(FunctionType type, FunctionBound bound, const Rvector& toValues)
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
 * @param <toNames> the names of the algebraic functions
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetAlgFunctionNames(const StringArray& toNames)
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
void UserPathFunction::SetFunctionNames(FunctionType type, const StringArray& toNames)
{
	if (type == ALGEBRAIC) {
		FunctionOutputData *algData = pfContainer->GetAlgData();
   		algData->SetFunctionNames(toNames);
	}
}

//------------------------------------------------------------------------------
// void SetAlgStateJacobian(const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
* This method sets the algebraic state jacobian
*
* @param <toJacobian> the algebraic state jacobian
*/
//------------------------------------------------------------------------------
void UserPathFunction::SetAlgStateJacobian(const Rmatrix& toJacobian)
{
  FunctionOutputData *algData  = pfContainer->GetAlgData();

  algData->SetJacobian(UserFunction::STATE, toJacobian);
}

//------------------------------------------------------------------------------
// void SetAlgControlJacobian(const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
* This method sets the algebraic control jacobian
*
* @param <toJacobian> the algebraic control jacobian
*/
//------------------------------------------------------------------------------
void UserPathFunction::SetAlgControlJacobian(const Rmatrix& toJacobian)
{
  FunctionOutputData *algData  = pfContainer->GetAlgData();

  algData->SetJacobian(UserFunction::CONTROL, toJacobian);
}

//------------------------------------------------------------------------------
// void SetAlgTimeJacobian(const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
* This method sets the algebraic time jacobian
*
* @param <toJacobian> the algebraic time jacobian
*/
//------------------------------------------------------------------------------
void UserPathFunction::SetAlgTimeJacobian(const Rmatrix& toJacobian)
{
  FunctionOutputData *algData  = pfContainer->GetAlgData();

  algData->SetJacobian(UserFunction::TIME, toJacobian);
}

//------------------------------------------------------------------------------
// void SetDynFunctions(const Rvector& toValues)
//------------------------------------------------------------------------------
/**
* This method sets the dynamic functions
*
* @param <toValues> the dynamic functions
*/
//------------------------------------------------------------------------------
void UserPathFunction::SetDynFunctions(const Rvector& toValues)
{
  FunctionOutputData *dynData  = pfContainer->GetDynData();

  dynData->SetFunctions(toValues);
}

//------------------------------------------------------------------------------
// void SetDynStateJacobian(const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
* This method sets the dynamic state jacobian
*
* @param <toJacobian> the dynamic state jacobian
*/
//------------------------------------------------------------------------------
void UserPathFunction::SetDynStateJacobian(const Rmatrix& toJacobian)
{
  FunctionOutputData *dynData  = pfContainer->GetDynData();

  dynData->SetJacobian(UserFunction::STATE, toJacobian);
}

//------------------------------------------------------------------------------
// void SetDynControlJacobian(const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
* This method sets the dynamic control jacobian
*
* @param <toJacobian> the dynamic control jacobian
*/
//------------------------------------------------------------------------------
void UserPathFunction::SetDynControlJacobian(const Rmatrix& toJacobian)
{
  FunctionOutputData *dynData  = pfContainer->GetDynData();

  dynData->SetJacobian(UserFunction::CONTROL, toJacobian);
}

//------------------------------------------------------------------------------
// void SetDynTimeJacobian(const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
* This method sets the dynamic time jacobian
*
* @param <toJacobian> the dynamic time jacobian
*/
//------------------------------------------------------------------------------
void UserPathFunction::SetDynTimeJacobian(const Rmatrix& toJacobian)
{
  FunctionOutputData *dynData  = pfContainer->GetDynData();

  dynData->SetJacobian(UserFunction::TIME, toJacobian);
}

//------------------------------------------------------------------------------
// void SetJacobian(FunctionType funcType, JacobianType jacType,
//                  const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
 * This method sets Jacobian data.
 *
 * @param <funcType> the type of function. Currently, only COST.
 * @param <jacType> the type of jacobian (STATE or CONTROL)
 * @param <toValues> the cost control jacobian
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetJacobian(FunctionType funcType, JacobianType jacType,
      const Rmatrix& toJacobian)
{
	if (funcType == COST)
	{
		FunctionOutputData *costData = pfContainer->GetCostData();
      costData->SetJacobian(jacType, toJacobian);
	} 
   else if (funcType == DYNAMICS)
	{
		FunctionOutputData *dynData = pfContainer->GetDynData();
      dynData->SetJacobian(jacType, toJacobian);
	} 
   else if (funcType == ALGEBRAIC)
	{
		FunctionOutputData *algData = pfContainer->GetAlgData();
      algData->SetJacobian(jacType, toJacobian);
	}
}

//------------------------------------------------------------------------------
// void SetCostFunction(const Rvector& toValues)
//------------------------------------------------------------------------------
/**
 * This method sets the cost function
 *
 * @param <toValues> the cost function
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetCostFunction(const Rvector& toValues)
{
   FunctionOutputData *costData  = pfContainer->GetCostData();

   costData->SetFunctions(toValues);
}

//------------------------------------------------------------------------------
// void SetCostStateJacobian(const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
 * This method sets the cost state jacobian
 *
 * @param <toValues> the cost state jacobian
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetCostStateJacobian(const Rmatrix& toJacobian)
{
   FunctionOutputData *costData  = pfContainer->GetCostData();

   costData->SetJacobian(UserFunction::STATE, toJacobian);
}

//------------------------------------------------------------------------------
// void SetCostControlJacobian(const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
 * This method sets the cost control jacobian
 *
 * @param <toValues> the cost control jacobian
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetCostControlJacobian(const Rmatrix& toJacobian)
{
   FunctionOutputData *costData  = pfContainer->GetCostData();

   costData->SetJacobian(UserFunction::CONTROL, toJacobian);
}

//------------------------------------------------------------------------------
// void SetCostTimeJacobian(const Rmatrix& toJacobian)
//------------------------------------------------------------------------------
/**
 * This method sets the cost time jacobian
 *
 * @param <toValues> the cost time jacobian
 */
//------------------------------------------------------------------------------
void UserPathFunction::SetCostTimeJacobian(const Rmatrix& toJacobian)
{
   FunctionOutputData *costData  = pfContainer->GetCostData();

   costData->SetJacobian(UserFunction::TIME, toJacobian);
}

//------------------------------------------------------------------------------
// Rvector GetAlgFunctions()
//------------------------------------------------------------------------------
/**
 * This method returns the algebraic functions
 *
 * @return  the algebraic functions
 */
//------------------------------------------------------------------------------
Rvector UserPathFunction::GetAlgFunctions()
{
   FunctionOutputData *algData  = pfContainer->GetAlgData();
   
   return algData->GetFunctionValues();
}

//------------------------------------------------------------------------------
// Rvector GetFunction(FunctionType type)
//------------------------------------------------------------------------------
/**
 * This method returns the algebraic functions
 *
 * @param <type> The type of function to retrieve
 * @return  the functions
 */
//------------------------------------------------------------------------------
Rvector UserPathFunction::GetFunction(FunctionType type)
{
   Rvector unsetVect;

	if (type == ALGEBRAIC)
	{
		FunctionOutputData *algData = pfContainer->GetAlgData();
		return algData->GetFunctionValues();
	}

	return unsetVect;
}

//------------------------------------------------------------------------------
// Rvector GetAlgFunctionsUpperBounds()
//------------------------------------------------------------------------------
/**
 * This method returns the algebraic functions upper bounds
 *
 * @return  the algebraic functions upper bounds
 */
//------------------------------------------------------------------------------
Rvector UserPathFunction::GetAlgFunctionsUpperBounds()
{
   FunctionOutputData *algData  = pfContainer->GetAlgData();
   
   return algData->GetUpperBounds();
}

//------------------------------------------------------------------------------
// Rvector GetAlgFunctionsLowerBounds()
//------------------------------------------------------------------------------
/**
 * This method returns the algebraic functions lower bounds
 *
 * @return  the functions lower bounds
 */
//------------------------------------------------------------------------------
Rvector UserPathFunction::GetAlgFunctionsLowerBounds()
{
   FunctionOutputData *algData  = pfContainer->GetAlgData();
   
   return algData->GetLowerBounds();
}

//------------------------------------------------------------------------------
// Rvector GetFunctionBounds(FunctionType type, FunctionBound bound)
//------------------------------------------------------------------------------
/**
 * This method returns functions bounds
 *
 * @param <type> The type of function to retrieve the bounds for
 * @param <bound> The type of bound to retrieve (UPPER or LOWER)
 * @return  the algebraic functions upper bounds
 */
//------------------------------------------------------------------------------
Rvector UserPathFunction::GetFunctionBounds(FunctionType type, FunctionBound bound)
{
   Rvector unsetVect;

	if (type == ALGEBRAIC && bound == UPPER)
	{
		FunctionOutputData *algData = pfContainer->GetAlgData();
		return algData->GetUpperBounds();
	}

	if (type == ALGEBRAIC && bound == LOWER)
	{
		FunctionOutputData *algData = pfContainer->GetAlgData();
		return algData->GetLowerBounds();
	}

	return unsetVect;
}

//------------------------------------------------------------------------------
// const StringArray& GetAlgFunctionNames()
//------------------------------------------------------------------------------
/**
 * This method returns the algebraic functions names
 *
 * @return  the algebraic functions names
 */
//------------------------------------------------------------------------------
const StringArray& UserPathFunction::GetAlgFunctionNames()
{
   FunctionOutputData *algData  = pfContainer->GetAlgData();
   
   return algData->GetFunctionNames();
}

//------------------------------------------------------------------------------
// const StringArray& GetFunctionNames(FunctionType type)
//------------------------------------------------------------------------------
/**
 * This method returns functions names
 *
 * @param <type> The type of function to retrieve the Names for
 *
 * @return  the algebraic functions names
 *
 * @note A better implementation would return by value rather than reference,
 *       obviating the need for a static here.
 */
//------------------------------------------------------------------------------
const StringArray& UserPathFunction::GetFunctionNames(FunctionType type)
{
   static StringArray noNames;

	if (type == ALGEBRAIC)
	{
		FunctionOutputData *algData = pfContainer->GetAlgData();
		return algData->GetFunctionNames();
	}

	return noNames;
}

//------------------------------------------------------------------------------
// Integer GetNumControlVars()
//------------------------------------------------------------------------------
/**
 * This method returns the number of control variables
 *
 * @return  the number of control variables
 */
//------------------------------------------------------------------------------
Integer UserPathFunction::GetNumControlVars()
{
   return paramData->GetNumControlVars();
}

//------------------------------------------------------------------------------
// bool IsPerturbing()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not the function
 * is perturbing
 *
 * @return  true if perturbing; false, otherwise
 */
//------------------------------------------------------------------------------
bool UserPathFunction::IsPerturbing()
{
   return paramData->IsPerturbing();
}

//------------------------------------------------------------------------------
// bool IsSparsity()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not the function
 * is sparsity
 *
 * @return  true if is sparsity; false, otherwise
 */
//------------------------------------------------------------------------------
bool UserPathFunction::IsSparsity()
{
   return paramData->IsSparsity();
}
