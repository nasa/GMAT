//------------------------------------------------------------------------------
//                           UserFunctionProperties
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
// Created: 2015.12.04
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------
#include "UserFunctionProperties.hpp"
#include "MessageInterface.hpp"
#include "LowThrustException.hpp" // for validation (TBD)

//#define DEBUG_PROPERTIES

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
UserFunctionProperties::UserFunctionProperties() :
   numFunctions   (0)
{
   for (Integer idx1 = UserFunction::STATE; idx1 < UserFunction::ALLJACOBIANS; idx1++)
   { 
      Rmatrix tmpMat(1,1,0.0);
      hasVars.push_back(false);
      jacobianPattern.push_back(tmpMat);
   }
}


//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
UserFunctionProperties::UserFunctionProperties(
                        const UserFunctionProperties &copy) :
   numFunctions   (copy.numFunctions)
{
   hasVars.clear();
   jacobianPattern.clear();
   Integer row, col;
   for (Integer idx1 = UserFunction::STATE; idx1 < UserFunction::ALLJACOBIANS; idx1++)
   { 
      hasVars.push_back(copy.hasVars[idx1]);
      if (copy.jacobianPattern[idx1].IsSized())
      {
         copy.jacobianPattern[idx1].GetSize(row, col);
         Rmatrix tmpMat = copy.jacobianPattern[idx1];
         jacobianPattern.push_back(tmpMat);
      }
   }   
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
UserFunctionProperties& UserFunctionProperties::operator=(
                        const UserFunctionProperties &copy)
{
   if (&copy == this)
      return *this;
   
   numFunctions       = copy.numFunctions;
   hasVars.clear();
   jacobianPattern.clear();
   Integer row, col;
   for (Integer idx1 = UserFunction::STATE; idx1 < UserFunction::ALLJACOBIANS; idx1++)
   {
      hasVars.push_back(copy.hasVars[idx1]);
      if (copy.jacobianPattern[idx1].IsSized())
      {
         copy.jacobianPattern[idx1].GetSize(row, col);
         Rmatrix tmpMat = copy.jacobianPattern[idx1];
         jacobianPattern.push_back(tmpMat);
      }
   }
  
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
UserFunctionProperties::~UserFunctionProperties()
{
   // nothing to do here at the moment
}


//------------------------------------------------------------------------------
// void SetStateJacobianPattern(const Rmatrix &pattern)
//------------------------------------------------------------------------------
/**
 * This method sets the state jacobian pattern
 *
 * @param <pattern>     the input state jacobin pattern
 *
 */
//------------------------------------------------------------------------------
void UserFunctionProperties::SetStateJacobianPattern(const Rmatrix &pattern)
{
   if (!pattern.IsSized())
      throw LowThrustException("Input StateJacobianPattern is unsized!\n");
   
   #ifdef DEBUG_PROPERTIES
      Integer r, c;
      pattern.GetSize(r, c);
      MessageInterface::ShowMessage(
                     "UFP::SetStateJacPat setting with size %d x %d\n", r, c);
   #endif

   jacobianPattern[UserFunction::STATE] = pattern; // what if different sizes?
}

//------------------------------------------------------------------------------
// void SetTimeJacobianPattern(const Rmatrix &pattern)
//------------------------------------------------------------------------------
/**
 * This method sets the time jacobian pattern
 *
 * @param <pattern>     the input state jacobin pattern
 *
 */
//------------------------------------------------------------------------------
void UserFunctionProperties::SetTimeJacobianPattern(const Rmatrix &pattern)
{
   if (!pattern.IsSized())
      throw LowThrustException("Input TimeJacobianPattern is unsized!\n");
   
   #ifdef DEBUG_PROPERTIES
      Integer r, c;
      pattern.GetSize(r, c);
      MessageInterface::ShowMessage(
                     "UFP::SetTimeJacPat setting with size %d x %d\n", r, c);
   #endif

   jacobianPattern[UserFunction::TIME] = pattern; // what if different sizes?
}

//------------------------------------------------------------------------------
// void SetControlJacobianPattern(const Rmatrix &pattern)
//------------------------------------------------------------------------------
/**
 * This method sets the control jacobian pattern
 *
 * @param <pattern>     the input state jacobin pattern
 *
 */
//------------------------------------------------------------------------------
void UserFunctionProperties::SetControlJacobianPattern(const Rmatrix &pattern)
{
   if (!pattern.IsSized())
      throw LowThrustException("Input ControlJacobianPattern is unsized!\n");
   
   #ifdef DEBUG_PROPERTIES
      Integer r, c;
      pattern.GetSize(r, c);
      MessageInterface::ShowMessage(
                     "UFP::SetControlJacPat setting with size %d x %d\n", r, c);
   #endif

   jacobianPattern[UserFunction::CONTROL] = pattern; // what if different sizes?
}

//------------------------------------------------------------------------------
// void SetJacobianPattern(const Rmatrix &pattern)
//------------------------------------------------------------------------------
/**
 * This method sets the jacobian pattern
 *
 * @param <pattern>     the input jacobin pattern
 *
 */
//------------------------------------------------------------------------------
void UserFunctionProperties::SetJacobianPattern(UserFunction::JacobianType idx, const Rmatrix &pattern)
{
   if (!pattern.IsSized())
      throw LowThrustException("Input JacobianPattern is unsized!\n");
   
   #ifdef DEBUG_PROPERTIES
      Integer r, c;
      pattern.GetSize(r, c);
      MessageInterface::ShowMessage(
                     "UFP::SetJacPat setting with size %d x %d\n", r, c);
   #endif
   Integer row, col;
   pattern.GetSize(row, col);
   jacobianPattern[idx].SetSize(row,col);
   jacobianPattern[idx] = pattern;
}

//------------------------------------------------------------------------------
// void SetNumberOfFunctions(Integer numFun)
//------------------------------------------------------------------------------
/**
 * This method sets the number of functions
 *
 * @param <numFun>     the input number of functions
 *
 */
//------------------------------------------------------------------------------
void UserFunctionProperties::SetNumberOfFunctions(Integer numFun)
{
   if (numFun < 0)   // is this correct?
      throw LowThrustException("Number of Functions must be >= 0\n");
   numFunctions = numFun;
}


//------------------------------------------------------------------------------
// void SetHasStateVars(bool hasVars)
//------------------------------------------------------------------------------
/**
 * This method sets the flag indicating whether or not there are state 
 * variables for this object.
 *
 * @param <hasVars>     the input flag
 *
 */
//------------------------------------------------------------------------------
void UserFunctionProperties::SetHasStateVars(bool hasVar)
{
   hasVars[UserFunction::STATE] = hasVar;
}

//------------------------------------------------------------------------------
// void SetHasControlVars(bool hasVars)
//------------------------------------------------------------------------------
/**
 * This method sets the flag indicating whether or not there are control
 * variables for this object.
 *
 * @param <hasVars>     the input flag
 *
 */
//------------------------------------------------------------------------------
void UserFunctionProperties::SetHasControlVars(bool hasVar)
{
   hasVars[UserFunction::CONTROL] = hasVar;
}


//------------------------------------------------------------------------------
// void SetHasVars(bool hasVars)
//------------------------------------------------------------------------------
/**
* This method sets the flag indicating whether or not there are 
* variables for this object.
*
* @param <hasVars>     the input flag
*
*/
//------------------------------------------------------------------------------
void UserFunctionProperties::SetHasVars(UserFunction::JacobianType idx, bool hasVar)
{
   hasVars[idx] = hasVar;
}

//------------------------------------------------------------------------------
// const Rmatrix& GetStateJacobianPattern()
//------------------------------------------------------------------------------
/**
 * This method returns the state jacobian pattern
 *
 * @return   the state jacobian pattern
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& UserFunctionProperties::GetStateJacobianPattern()
{
   return jacobianPattern[UserFunction::STATE];
}

//------------------------------------------------------------------------------
// const Rmatrix& GetTimeJacobianPattern()
//------------------------------------------------------------------------------
/**
 * This method returns the time jacobian pattern
 *
 * @return   the time jacobian pattern
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& UserFunctionProperties::GetTimeJacobianPattern()
{
   return jacobianPattern[UserFunction::TIME];
}

//------------------------------------------------------------------------------
// const Rmatrix& GetControlJacobianPattern()
//------------------------------------------------------------------------------
/**
 * This method returns the control jacobian pattern
 *
 * @return   the control jacobian pattern
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& UserFunctionProperties::GetControlJacobianPattern()
{
   return jacobianPattern[UserFunction::CONTROL];
}

//------------------------------------------------------------------------------
// const Rmatrix& GetJacobianPattern()
//------------------------------------------------------------------------------
/**
* This method returns the jacobian pattern
*
* @return   the jacobian pattern
*
*/
//------------------------------------------------------------------------------
const Rmatrix& UserFunctionProperties::GetJacobianPattern(UserFunction::JacobianType idx)
{
   return jacobianPattern[idx];
}


//------------------------------------------------------------------------------
// Integer GetNumberOfFunctions()
//------------------------------------------------------------------------------
/**
 * This method returns the number of functions
 *
 * @return   the number of functions
 *
 */
//------------------------------------------------------------------------------
Integer UserFunctionProperties::GetNumberOfFunctions()
{
   return numFunctions;
}

//------------------------------------------------------------------------------
// bool HasStateVars()
//------------------------------------------------------------------------------
/**
 * This method returns the flag indicating whether or not this object has
 * state variables
 *
 * @return   true if there are state variables; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool UserFunctionProperties::HasStateVars()
{
   return hasVars[UserFunction::STATE];
}

//------------------------------------------------------------------------------
// bool HasControlVars()
//------------------------------------------------------------------------------
/**
 * This method returns the flag indicating whether or not this object has
 * control variables
 *
 * @return   true if there are control variables; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool UserFunctionProperties::HasControlVars()
{
   return hasVars[UserFunction::CONTROL];
}

//------------------------------------------------------------------------------
// bool HasVars()
//------------------------------------------------------------------------------
/**
* This method returns the flag indicating whether or not this object has
* variables
*
* @return   true if there are state variables; false otherwise
*
*/
//------------------------------------------------------------------------------
bool UserFunctionProperties::HasVars(UserFunction::JacobianType idx)
{
   return hasVars[idx];
}

//------------------------------------------------------------------------------
// void SetStaticJacobianPattern(const Rmatrix &pattern)
//------------------------------------------------------------------------------
/**
* This method sets the static jacobian pattern
*
* @param <pattern>     the input static jacobin pattern
*
*/
//------------------------------------------------------------------------------
void UserFunctionProperties::SetStaticJacobianPattern(const Rmatrix &pattern)
{
   if (!pattern.IsSized())
      throw LowThrustException("Input StaticJacobianPattern is unsized!\n");

#ifdef DEBUG_PROPERTIES
   Integer r, c;
   pattern.GetSize(r, c);
   MessageInterface::ShowMessage(
      "UFP::SetStaticJacPat setting with size %d x %d\n", r, c);
#endif

   jacobianPattern[UserFunction::STATE] = pattern; // what if different sizes?
}

//------------------------------------------------------------------------------
// void SetHasStaticVars(bool hasVars)
//------------------------------------------------------------------------------
/**
* This method sets the flag indicating whether or not there are static
* variables for this object.
*
* @param <hasVars>     the input flag
*
*/
//------------------------------------------------------------------------------
void UserFunctionProperties::SetHasStaticVars(bool hasVar)
{
   hasVars[UserFunction::STATIC] = hasVar;
}

//------------------------------------------------------------------------------
// const Rmatrix& GetStaticJacobianPattern()
//------------------------------------------------------------------------------
/**
* This method returns the static jacobian pattern
*
* @return   the static jacobian pattern
*
*/
//------------------------------------------------------------------------------
const Rmatrix& UserFunctionProperties::GetStaticJacobianPattern()
{
   return jacobianPattern[UserFunction::STATIC];
}

//------------------------------------------------------------------------------
// bool HasStaticVars()
//------------------------------------------------------------------------------
/**
* This method returns the flag indicating whether or not this object has
* static variables
*
* @return   true if there are static variables; false otherwise
*
*/
//------------------------------------------------------------------------------
bool UserFunctionProperties::HasStaticVars()
{
   return hasVars[UserFunction::STATIC];
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
