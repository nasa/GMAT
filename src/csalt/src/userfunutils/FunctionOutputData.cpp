//------------------------------------------------------------------------------
//                              FunctionOutputData
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
// Created: 2015.07.13
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 *
 *  This class stores user function data for the optimal control
 *  problem.  This data includes the function values and Jacobians.
 *  The data on this class is used to form NLP function values.
 *
 */
//------------------------------------------------------------------------------
#include <iostream>
#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "FunctionOutputData.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_OUTPUT_DATA

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
FunctionOutputData::FunctionOutputData() :
   hasUserFunction         (false),
   //hasUserStateJacobian    (false),
   //hasUserControlJacobian  (false),
   //hasUserTimeJacobian     (false),
   meshIndex               (0),
   stageIndex              (0),
   isInitializing          (true),
   numFunctions            (0),
   boundsSet               (false)
{
   stateIndexes.clear();
   controlIndexes.clear();
   functionValues.SetSize(0);
   upperBounds.SetSize(0);
   lowerBounds.SetSize(0);
   
   functionNames.clear();

   for (Integer idx1 = UserFunction::STATE; idx1 < UserFunction::ALLJACOBIANS; idx1++)
      hasJacobian.push_back(false);
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
FunctionOutputData::FunctionOutputData(const FunctionOutputData &copy) :
   hasUserFunction         (copy.hasUserFunction),
   //hasUserStateJacobian    (copy.hasUserStateJacobian),
   //hasUserControlJacobian  (copy.hasUserControlJacobian),
   //hasUserTimeJacobian     (copy.hasUserTimeJacobian),
   meshIndex               (copy.meshIndex),
   stageIndex              (copy.stageIndex),
   isInitializing          (copy.isInitializing),
   numFunctions            (copy.numFunctions),
   boundsSet               (copy.boundsSet)
{
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
         "Entering FunctionOutputData copy constructor ...................\n");
   #endif
   stateIndexes.clear();
   controlIndexes.clear();
   for (UnsignedInt ii = 0; ii < copy.stateIndexes.size(); ii++)
      stateIndexes.push_back(copy.stateIndexes.at(ii));
   for (UnsignedInt ii = 0; ii < copy.controlIndexes.size(); ii++)
      controlIndexes.push_back(copy.controlIndexes.at(ii));

   jacobian = copy.jacobian;

   //Integer row, col;
   //Rmatrix tmp = (Rmatrix)(copy.GetJacobian(STATE));
   //if (copy.jacobian[STATE].IsSized())
   //{
   //   copy.jacobian[STATE].GetSize(row, col);
   //   jacobian[STATE].SetSize(row, col);
   //   jacobian[STATE] = copy.jacobian[STATE];
   //}
   //else
   //   jacobian[STATE].SetSize(0, 0);
   //
   //if (copy.controlJacobian.IsSized())
   //{
   //   copy.controlJacobian.GetSize(row, col);
   //   controlJacobian.SetSize(row, col);
   //   controlJacobian = copy.controlJacobian;
   //}
   //else
   //   controlJacobian.SetSize(0, 0);
   //
   //if (copy.timeJacobian.IsSized())
   //{
   //   copy.timeJacobian.GetSize(row, col);
   //   timeJacobian.SetSize(row, col);
   //   timeJacobian = copy.timeJacobian;
   //}
   //else
   //   timeJacobian.SetSize(0, 0);

   functionNames  = copy.functionNames;

   // @todo - these should all be of the same size, correct?
   Integer sz = copy.functionValues.GetSize();
   functionValues.SetSize(sz);
   functionValues  = copy.functionValues;
   
   if (copy.upperBounds.IsSized())
   {
      sz = copy.upperBounds.GetSize();
      upperBounds.SetSize(sz);
      upperBounds     = copy.upperBounds;
   }
   else
   {
      upperBounds.SetSize(0);  // do we want to do this here?
   }
   if (copy.lowerBounds.IsSized())
   {
      sz = copy.lowerBounds.GetSize();
      lowerBounds.SetSize(sz);
      lowerBounds     = copy.lowerBounds;
   }
   else
   {
      lowerBounds.SetSize(0);  // do we want to do this here?
   }

   hasJacobian              = copy.hasJacobian;
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
FunctionOutputData& FunctionOutputData::operator=(
                                        const FunctionOutputData &copy)
{
   
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
            "Entering FunctionOutputData operator= ...................\n");
   #endif
   if (&copy == this)
      return *this;

   hasUserFunction          = copy.hasUserFunction;
   //hasUserStateJacobian     = copy.hasUserStateJacobian;
   //hasUserControlJacobian   = copy.hasUserControlJacobian;
   //hasUserTimeJacobian      = copy.hasUserTimeJacobian;
   meshIndex                = copy.meshIndex;
   stageIndex               = copy.stageIndex;
   isInitializing           = copy.isInitializing;
   numFunctions             = copy.numFunctions;
   boundsSet                = copy.boundsSet;

   hasJacobian              = copy.hasJacobian;
   
   stateIndexes.clear();
   controlIndexes.clear();
   for (UnsignedInt ii = 0; ii < copy.stateIndexes.size(); ii++)
      stateIndexes.push_back(copy.stateIndexes.at(ii));
   for (UnsignedInt ii = 0; ii < copy.controlIndexes.size(); ii++)
      controlIndexes.push_back(copy.controlIndexes.at(ii));
   
   jacobian = copy.jacobian;

   //Integer row, col;
   //if (copy.jacobian[STATE].IsSized())
   //{
   //   copy.jacobian[STATE].GetSize(row, col);
   //   jacobian[STATE].SetSize(row, col);
   //   jacobian[STATE] = copy.jacobian[STATE];
   //}
   //else
   //   jacobian[STATE].SetSize(0, 0);
   //
   //
   //if (copy.controlJacobian.IsSized())
   //{
   //   copy.controlJacobian.GetSize(row, col);
   //   controlJacobian.SetSize(row, col);
   //   controlJacobian = copy.controlJacobian;
   //}
   //else
   //   controlJacobian.SetSize(0, 0);
   //
   //if (copy.timeJacobian.IsSized())
   //{
   //   copy.timeJacobian.GetSize(row, col);
   //   timeJacobian.SetSize(row, col);
   //   timeJacobian = copy.timeJacobian;
   //}
   //else
   //   timeJacobian.SetSize(0, 0);
   
   
   functionNames  = copy.functionNames;

   // @todo - these should all be of the same size, correct?
   Integer sz = copy.functionValues.GetSize();
   functionValues.SetSize(sz);
   functionValues  = copy.functionValues;
   
   if (copy.upperBounds.IsSized())
   {
      sz = copy.upperBounds.GetSize();
      upperBounds.SetSize(sz);
      upperBounds     = copy.upperBounds;
   }
   else
   {
      upperBounds.SetSize(0);  // do we want to do this here?
   }
   if (copy.lowerBounds.IsSized())
   {
      sz = copy.lowerBounds.GetSize();
      lowerBounds.SetSize(sz);
      lowerBounds     = copy.lowerBounds;
   }
   else
   {
      lowerBounds.SetSize(0);  // do we want to do this here?
   }

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
FunctionOutputData::~FunctionOutputData()
{
   // nothing to do here at the moment
}

//------------------------------------------------------------------------------
// void SetFunctionNames(StringArray fNames)
//------------------------------------------------------------------------------
/**
 * This method sets the function names to the input array of names
 *
 * @param <fNames>    array of function names
 *
 */
//------------------------------------------------------------------------------
void FunctionOutputData::SetFunctionNames(StringArray fNames)
{
   functionNames = fNames;
}

//------------------------------------------------------------------------------
// void SetFunctionValues(Integer numFuncs, const Rvector &funcValues)
//------------------------------------------------------------------------------
/**
 * This method sets the function values and number of functions
 *
 * @param <numFuncs>    number of function values
 * @param <funcValues>  array of function values
 *
 */
//------------------------------------------------------------------------------
void FunctionOutputData::SetFunctionValues(Integer numFuncs,
                                           const Rvector &funcValues)
{
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
      "Entering FunctionOutputData::SetFunctionValues ...................:\n");
      MessageInterface::ShowMessage("   %s\n", funcValues.ToString().c_str());
      std::cout << "In SetFunctionValues, numFuncs = " << numFuncs;
      std::cout << " funcValues = " << funcValues.ToString().c_str() << std::endl;
   #endif
   numFunctions = numFuncs;
   Integer sz   = funcValues.GetSize();
   if (sz != numFuncs)
   {
      std::string errmsg = "For FunctionOutputData::SetFunctionValues, ";
      errmsg += "function Array is not valid dimension\n";
      throw LowThrustException(errmsg);
   }
   functionValues.SetSize(sz);
   functionValues = funcValues;
   
   if (isInitializing)
      hasUserFunction = true;
}

//------------------------------------------------------------------------------
// void SetFunctions(const Rvector &funcValues)
//------------------------------------------------------------------------------
/**
 * This method sets the function values
 *
 * @param <funcValues>  array of function values
 *
 */
//------------------------------------------------------------------------------
void FunctionOutputData::SetFunctions(const Rvector &funcValues)
{
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
            "Entering FunctionOutputData::SetFunctions ...................:\n");
      MessageInterface::ShowMessage("   %s\n", funcValues.ToString(12).c_str());
   MessageInterface::ShowMessage("     isInitializing = %s\n",
                                 (isInitializing? "true" : "false"));
   #endif
   Integer sz   = funcValues.GetSize();
   functionValues.SetSize(sz);
   functionValues = funcValues;
   // Shouldn't the size be set here anyway???
   if ((sz != 0) && isInitializing)
   {
      numFunctions    = sz;
      hasUserFunction = true;
   }
}

//------------------------------------------------------------------------------
// void SetNumFunctions(Integer numFuncs)
//------------------------------------------------------------------------------
/**
 * This method sets the number of functions
 *
 * @param <numFuncs>  number of functions
 *
 */
//------------------------------------------------------------------------------
void FunctionOutputData::SetNumFunctions(Integer numFuncs)
{
   /// @todo - do I need validation here?
   numFunctions = numFuncs;
}

//------------------------------------------------------------------------------
// void SetIsInitializing(bool isInit)
//------------------------------------------------------------------------------
/**
 * This method sets the isInitializing flag
 *
 * @param <isInit>  input isInitializing flag
 *
 */
//------------------------------------------------------------------------------
void FunctionOutputData::SetIsInitializing(bool isInit)
{
   isInitializing = isInit;
}

//------------------------------------------------------------------------------
// bool SetUpperBounds(const Rvector &toUpper)
//------------------------------------------------------------------------------
/**
 * This method sets the upper bounds vector
 *
 * @param <toUpper>  upper bounds vector
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::SetUpperBounds(const Rvector &toUpper)
{
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
           "Entering FunctionOutputData::SetUpperBounds ...................\n");
   #endif
   Integer sz = toUpper.GetSize();
   if (sz != functionValues.GetSize())
   {
      std::string errmsg = "For FunctionOutputData::SetFunctionValues, ";
      errmsg += "UpperBounds Array is not valid dimension\n";
      throw LowThrustException(errmsg);
   }
   upperBounds.SetSize(sz);
   upperBounds = toUpper;
   return true;
}

//------------------------------------------------------------------------------
// bool SetLowerBounds(const Rvector &toLower)
//------------------------------------------------------------------------------
/**
 * This method sets the lower bounds vector
 *
 * @param <toUpper>  lower bounds vector
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::SetLowerBounds(const Rvector &toLower)
{
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
           "Entering FunctionOutputData::SetLowerBounds ...................\n");
      std::cout << "Entering FunctionOutputData::SetLowerBounds ...................\n";
      std::cout << "toLower = " << toLower.ToString().c_str() << std::endl;
      std::cout << "functionValues size = " << functionValues.GetSize() << std::endl;
   #endif
   Integer sz = toLower.GetSize();
   if (sz != functionValues.GetSize())
   {
      std::string errmsg = "For FunctionOutputData::SetFunctionValues, ";
      errmsg += "LowerBounds Array is not valid dimension\n";
      throw LowThrustException(errmsg);
   }
   lowerBounds.SetSize(sz);
   lowerBounds = toLower;
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
      "Exiting FunctionOutputData::SetLowerBounds ...................\n");
      std::cout << "Exiting FunctionOutputData::SetLowerBounds ...................\n";
   #endif
   return true;
}

//------------------------------------------------------------------------------
// bool HasUserFunction()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not this class has a
 * user function
 *
 * @return flag indicating whether or not this class had a user function
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::HasUserFunction() const
{
   return hasUserFunction;
}


//------------------------------------------------------------------------------
// Integer GetNumFunctions() const
//------------------------------------------------------------------------------
/**
 * This method returns the number of functions
 *
 * @return the number of functions
 *
 */
//------------------------------------------------------------------------------
Integer FunctionOutputData::GetNumFunctions() const
{
   return numFunctions;
}

//------------------------------------------------------------------------------
// const StringArray& GetFunctionNames() const
//------------------------------------------------------------------------------
/**
 * This method returns an array of function names
 *
 * @return array of function names
 *
 */
//------------------------------------------------------------------------------
const StringArray& FunctionOutputData::GetFunctionNames() const
{
   return functionNames;
}

//------------------------------------------------------------------------------
// const Rvector& GetFunctionValues() const
//------------------------------------------------------------------------------
/**
 * This method returns an array of function values
 *
 * @return array of function values
 *
 */
//------------------------------------------------------------------------------
const Rvector& FunctionOutputData::GetFunctionValues() const
{
   return functionValues;
}

//------------------------------------------------------------------------------
// const Rvector& GetUpperBounds()
//------------------------------------------------------------------------------
/**
 * This method returns an array of upper bounds
 *
 * @return array of upper bounds
 *
 */
//------------------------------------------------------------------------------
const Rvector& FunctionOutputData::GetUpperBounds()
{
   return upperBounds;
}

//------------------------------------------------------------------------------
// const Rvector& GetLowerBounds()
//------------------------------------------------------------------------------
/**
 * This method returns an array of lower bounds
 *
 * @return array of lower bounds
 *
 */
//------------------------------------------------------------------------------
const Rvector& FunctionOutputData::GetLowerBounds()
{
   return lowerBounds;
}


//------------------------------------------------------------------------------
// bool         SetNLPData(Integer meshIdx, Integer stageIdx,
//                         const IntegerArray &stateIdxes,
//                         const IntegerArray &controlIdxes)
//                         const IntegerArray &staticIdxes)
//------------------------------------------------------------------------------
/**
 * This method sets the NLP data
 *
 * @param <meshIdx>      mesh index
 * @param <stageIdx>     stage index
 * @param <stateIdxes>   array of state indexes
 * @param <controlIdxes> array of control indexes
 * @param <staticIdxes>   array of static indexes
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::SetNLPData(Integer meshIdx, Integer stageIdx,
                                    const IntegerArray &stateIdxs,
                                    const IntegerArray &controlIdxs, 
                                    const IntegerArray &staticIdxs)
{
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
               "Entering FunctionOutputData::SetNLPData ...................\n");
      MessageInterface::ShowMessage("   meshIdx             = %d\n", meshIdx);
      MessageInterface::ShowMessage("   stageIdx            = %d\n", stageIdx);
      MessageInterface::ShowMessage("   size of stateIdxs   = %d\n",
                                    (Integer) stateIdxs.size());
      MessageInterface::ShowMessage("   size of controlIdxs = %d\n",
                                    (Integer) controlIdxs.size());
   #endif
   meshIndex     = meshIdx;
   stageIndex    = stageIdx;
   
   stateIndexes.clear();
   Integer sz = stateIdxs.size();
   for (Integer ii = 0; ii < sz; ii++)
      stateIndexes.push_back(stateIdxs.at(ii));
   controlIndexes.clear();
   sz = controlIdxs.size();
   for (Integer ii = 0; ii < sz; ii++)
      controlIndexes.push_back(controlIdxs.at(ii));

   staticIndexes.clear();
   sz = staticIdxs.size();
   for (Integer ii = 0; ii < sz; ii++)
      staticIndexes.push_back(staticIdxs.at(ii));


   return true;
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
               "LEAVING FunctionOutputData::SetNLPData ...................\n");
   #endif
}

//------------------------------------------------------------------------------
// bool SetStateJacobian(const Rmatrix& sj)
//------------------------------------------------------------------------------
/**
 * This method sets state jacobian
 *
 * @param <sj>      state jacobian
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::SetStateJacobian(const Rmatrix& sj)
{
   Integer row, col;
   sj.GetSize(row, col);
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
           "Entering FunctionOutputData::SetStateJacobian to size (%d x %d)\n",
           row, col);
   #endif
   //stateJacobian.SetSize(row, col);
   //stateJacobian = sj;

   jacobian[UserFunction::STATE].SetSize(row, col);
   jacobian[UserFunction::STATE] = sj;

   if (isInitializing)
   {
      //hasUserStateJacobian = true;
      hasJacobian[UserFunction::STATE] = true;
   }
   return true;
}

//------------------------------------------------------------------------------
// bool SetControlJacobian(const Rmatrix& cj)
//------------------------------------------------------------------------------
/**
 * This method sets control jacobian
 *
 * @param <cj>      control jacobian
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::SetControlJacobian(const Rmatrix& cj)
{
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
      "Entering FunctionOutputData::SetControlJacobian ...................\n");
   #endif
   Integer row, col;
   cj.GetSize(row, col);
   //controlJacobian.SetSize(row, col);
   //controlJacobian = cj;

   jacobian[UserFunction::CONTROL].SetSize(row, col);
   jacobian[UserFunction::CONTROL] = cj;
   if (isInitializing)
   {
      //hasUserControlJacobian = true;
      hasJacobian[UserFunction::CONTROL] = true;
   }
   return true;
}

//------------------------------------------------------------------------------
// bool SetTimeJacobian(const Rmatrix& tj)
//------------------------------------------------------------------------------
/**
 * This method sets time jacobian
 *
 * @param <tj>      time jacobian
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::SetTimeJacobian(const Rmatrix& tj)
{
   Integer row, col;
   tj.GetSize(row, col);
   #ifdef DEBUG_OUTPUT_DATA
      MessageInterface::ShowMessage(
               "Entering FunctionOutputData::SetTimeJacobian to size %d x %d\n",
               row, col);
   #endif
   //timeJacobian.SetSize(row, col);
   //timeJacobian = tj;

   jacobian[UserFunction::TIME].SetSize(row, col);
   jacobian[UserFunction::TIME] = tj;
   if (isInitializing)
   {
      //hasUserTimeJacobian = true;
      hasJacobian[UserFunction::TIME] = true;
   }
   return true;
}

//------------------------------------------------------------------------------
// bool SetTimeJacobian(JacobianType jacType, const Rmatrix& tj)
//------------------------------------------------------------------------------
/**
 * This method sets jacobians
 *
 * @param <jacType>      type of jacobian to set
 * @param <tj>      jacobian input
 *
 * @return true if successful; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::SetJacobian(UserFunction::JacobianType jacType, const Rmatrix& j)
{
   Integer row, col;
   j.GetSize(row, col);

   bool retval = false;
        
   // is it valid type?
   if (jacType >= UserFunction::STATE && jacType < UserFunction::ALLJACOBIANS)
   {
      jacobian[jacType].SetSize(row, col);
      jacobian[jacType] = j;

      if (isInitializing)
      {
         //hasUserStateJacobian = true;
         hasJacobian[jacType] = true;
      }
      retval = true;
   }

   return retval;
}

//------------------------------------------------------------------------------
// const Rmatrix& GetStateJacobian()
//------------------------------------------------------------------------------
/**
 * This method returns the state jacobian
 *
 * @return the state jacobian
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& FunctionOutputData::GetStateJacobian()
{
   #ifdef DEBUG_OUTPUT_DATA
      if (jacobian[STATE].IsSized())
      {
         Integer r, c;
         jacobian[STATE].GetSize(r, c);
         MessageInterface::ShowMessage(" state jacobian size is %d x %d\n",
                                       r, c);
      }
      else
      {
         MessageInterface::ShowMessage(" state jacobian is NOT sized!!!\n");
      }
   #endif
   //return stateJacobian;
   return jacobian[UserFunction::STATE];
}

//------------------------------------------------------------------------------
// const Rmatrix& GetControlJacobian()
//------------------------------------------------------------------------------
/**
 * This method returns the control jacobian
 *
 * @return the control jacobian
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& FunctionOutputData::GetControlJacobian()
{
   #ifdef DEBUG_OUTPUT_DATA
      if (jacobian[CONTROL].IsSized())
      {
         Integer r, c;
         jacobian[CONTROL].GetSize(r, c);
         MessageInterface::ShowMessage(" control jacobian size is %d x %d\n",
                                       r, c);
      }
      else
      {
         MessageInterface::ShowMessage(" control jacobian is NOT sized!!!\n");
      }
   #endif
   return jacobian[UserFunction::CONTROL];
}

//------------------------------------------------------------------------------
// const Rmatrix& GetTimeJacobian()
//------------------------------------------------------------------------------
/**
 * This method returns the time jacobian
 *
 * @return the time jacobian
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& FunctionOutputData::GetTimeJacobian()
{
   #ifdef DEBUG_OUTPUT_DATA
      if (jacobian[TIME].IsSized())
      {
         Integer r, c;
         jacobian[TIME].GetSize(r, c);
         MessageInterface::ShowMessage(" time jacobian size is %d x %d\n",
                                       r, c);
      }
      else
      {
         MessageInterface::ShowMessage(" time jacobian is NOT sized!!!\n");
      }
   #endif
   return jacobian[UserFunction::TIME];
}

//------------------------------------------------------------------------------
// const Rmatrix& GetJacobian(JacobianType jacType)
//------------------------------------------------------------------------------
/**
 * This method returns jacobians
 *
 * @param <jacType> type of jacobian to return
 *
 * @return the time jacobian
 *
 * @note A better implementation would return by value rather than reference,
 *       obviating the need for a static here.
 */
//------------------------------------------------------------------------------
const Rmatrix& FunctionOutputData::GetJacobian(UserFunction::JacobianType jacType)
{
   static Rmatrix unsetMatrix;

   if (jacType >= UserFunction::STATE && jacType < UserFunction::ALLJACOBIANS)
   {
      #ifdef DEBUG_OUTPUT_DATA
         //if (stateJacobian.IsSized())
         if (jacobian[jacType].IsSized())
         {
            Integer r, c;
            //stateJacobian.GetSize(r, c);
            jacobian[jacType].GetSize(r, c);
            MessageInterface::ShowMessage(" %d-type jacobian size is %d x %d\n",
                                          int(jacType), r, c);
         }
         else
         {
            MessageInterface::ShowMessage(" %d-type jacobian is NOT sized!!!\n", int(jacType));
         }
      #endif
      //return stateJacobian;
      return jacobian[jacType];
   }

   return unsetMatrix;
}

//------------------------------------------------------------------------------
// bool HasUserStateJacobian()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not this class has a
 * state jacobian
 *
 * @return true if there is a state jacobian; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::HasUserStateJacobian()
{
   //return hasUserStateJacobian;
   return hasJacobian[UserFunction::STATE];
}

//------------------------------------------------------------------------------
// bool HasUserControlJacobian()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not this class has a
 * control jacobian
 *
 * @return true if there is a control jacobian; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::HasUserControlJacobian()
{
   //return hasUserControlJacobian;
   return hasJacobian[UserFunction::CONTROL];
}

//------------------------------------------------------------------------------
// bool HasUserTimeJacobian()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not this class has a
 * time jacobian
 *
 * @return true if there is a time jacobian; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::HasUserTimeJacobian()
{
   //return hasUserTimeJacobian;
   return hasJacobian[UserFunction::TIME];
}

//------------------------------------------------------------------------------
// bool HasUserJacobian(JacobianType jacType)
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not this class has a
 * given type of jacobian
 *
 * @param <jacType> The type of jacobian to detect
 *
 * @return true if there is the given type of jacobian; false, otherwise
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::HasUserJacobian(UserFunction::JacobianType jacType)
{
   bool retval = false;

   if (jacType >= UserFunction::STATE && jacType < UserFunction::ALLJACOBIANS)
   {
      retval = hasJacobian[jacType];
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool HasUserJacobian()
//------------------------------------------------------------------------------
/**
* This method returns a flag indicating whether or not this class has a
* given type of jacobian
*
* @param <jacType> The type of jacobian to detect
*
* @return true if there is the given type of jacobian; false, otherwise
*
*/
//------------------------------------------------------------------------------
std::vector<bool> FunctionOutputData::HasUserJacobian()
{
   return hasJacobian;
}

//------------------------------------------------------------------------------
// const IntegerArray& GetStateIdxs()
//------------------------------------------------------------------------------
/**
 * This method returns an array of state indexes
 *
 * @return array of state indexes
 *
 */
//------------------------------------------------------------------------------
const IntegerArray& FunctionOutputData::GetStateIdxs()
{
   return stateIndexes;
}

//------------------------------------------------------------------------------
// const IntegerArray& GetControlIdxs()
//------------------------------------------------------------------------------
/**
 * This method returns an array of control indexes
 *
 * @return array of control indexes
 *
 */
//------------------------------------------------------------------------------
const IntegerArray& FunctionOutputData::GetControlIdxs()
{
   return controlIndexes;
}

//------------------------------------------------------------------------------
// const IntegerArray& GetStateIdxs()
//------------------------------------------------------------------------------
/**
* This method returns an array of state indexes
*
* @return array of state indexes
*
*/
//------------------------------------------------------------------------------
const IntegerArray& FunctionOutputData::GetStaticIdxs()
{
   return staticIndexes;
}

//------------------------------------------------------------------------------
// Integer GetMeshIdx()
//------------------------------------------------------------------------------
/**
 * This method returns the mesh index
 *
 * @return mesh index
 *
 */
//------------------------------------------------------------------------------
Integer FunctionOutputData::GetMeshIdx()
{
   return meshIndex;
}

//------------------------------------------------------------------------------
// Integer GetStageIdx()
//------------------------------------------------------------------------------
/**
 * This method returns the stage index
 *
 * @return stage index
 *
 */
//------------------------------------------------------------------------------
Integer FunctionOutputData::GetStageIdx()
{
   return stageIndex;
}

//------------------------------------------------------------------------------
// bool BoundsSet()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not the bounds have been set
 *
 * @return flag indicating whether or not the bounds have been set
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::BoundsSet()
{
   if ((upperBounds.IsSized()) && (lowerBounds.IsSized()))
         return true;
   return false;
}

//------------------------------------------------------------------------------
// bool IsInitializing() const
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not this is initializing
 *
 * @return flag indicating whether or not this is initializing
 *
 */
//------------------------------------------------------------------------------
bool FunctionOutputData::IsInitializing() const
{
   return isInitializing;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
