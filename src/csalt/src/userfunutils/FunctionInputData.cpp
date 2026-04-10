//------------------------------------------------------------------------------
//                              FunctionInputData
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
// Created: 2015.07.15
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "FunctionInputData.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
FunctionInputData::FunctionInputData() :
   numStateVars       (0),
   numControlVars     (0),
   numStaticVars      (0),
   time               (0.0),
   phaseNum           (-1),
   isPerturbing       (false),
   isSparsity         (false)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
FunctionInputData::FunctionInputData(const FunctionInputData &copy) :
   numStateVars       (copy.numStateVars),
   numControlVars     (copy.numControlVars),
   numStaticVars      (copy.numStaticVars),
   time               (copy.time),
   phaseNum           (copy.phaseNum),
   isPerturbing       (copy.isPerturbing),
   isSparsity         (copy.isSparsity)
{
   Integer sz = copy.state.GetSize();
   state.SetSize(sz);
   state = copy.state;
   
   sz = copy.control.GetSize();
   control.SetSize(sz);
   control = copy.control;

   sz = copy.staticVars.GetSize();
   staticVars.SetSize(sz);
   staticVars = copy.staticVars;
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
FunctionInputData& FunctionInputData::operator=(
                                      const FunctionInputData &copy)
{
   if (&copy == this)
      return *this;
   
   numStateVars   = copy.numStateVars;
   numControlVars = copy.numControlVars;
   numStaticVars  = copy.numStaticVars;
   time           = copy.time;
   phaseNum       = copy.phaseNum;
   isPerturbing   = copy.isPerturbing;
   isSparsity     = copy.isSparsity;
   
   Integer sz = copy.state.GetSize();
   state.SetSize(sz);
   state          = copy.state;
 
   sz = copy.control.GetSize();
   control.SetSize(sz);
   control = copy.control;

   sz = copy.staticVars.GetSize();
   staticVars.SetSize(sz);
   staticVars = copy.staticVars;

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
FunctionInputData::~FunctionInputData()
{
   // nothing to do here at the moment
}

//------------------------------------------------------------------------------
// void Initialize(Integer numState, Integer numControl)
//------------------------------------------------------------------------------
/**
 * This method initializes the FunctionInputData
 *
 * @param <numState>    number of state variables
 * @param <numControl>  number of control variables
 *
 *
 */
//------------------------------------------------------------------------------
void FunctionInputData::Initialize(Integer numState, Integer numControl, 
                                   Integer numStatic)
{
   numStateVars     = numState;
   state.SetSize(numState);
   numControlVars   = numControl;
   control.SetSize(numControl);
   numStaticVars = numStatic;
   if (numStatic == 0)
      staticVars.SetSize(1);
   else
      staticVars.SetSize(numStatic);
}


//------------------------------------------------------------------------------
// void SetStateVector(const Rvector &toState);
//------------------------------------------------------------------------------
/**
 * This method sets the state vector
 *
 * @param <toState>    the state vector
 *
 */
//------------------------------------------------------------------------------
void FunctionInputData::SetStateVector(const Rvector &toState)
{
   Integer sz = toState.GetSize();
   state.SetSize(sz);
   state      = toState;
}

//------------------------------------------------------------------------------
// void  SetControlVector(const Rvector& toControl)
//------------------------------------------------------------------------------
/**
 * This method sets the control vector
 *
 * @param <toControl>    the control vector
 *
 */
//------------------------------------------------------------------------------
void FunctionInputData::SetControlVector(const Rvector& toControl)
{
   Integer sz = toControl.GetSize();
   control.SetSize(sz);
   control    = toControl;
   
}

//------------------------------------------------------------------------------
// void SetPhaseNum(Integer toNum);
//------------------------------------------------------------------------------
/**
 * This method sets phase number
 *
 * @param <toNum>    the phase number
 *
 */
//------------------------------------------------------------------------------
void FunctionInputData::SetPhaseNum(Integer toNum)
{
   phaseNum = toNum;
}

//------------------------------------------------------------------------------
// void SetIsPerturbing(bool isPerturb);
//------------------------------------------------------------------------------
/**
 * This method sets the isPerturbing flag
 *
 * @param <isPerturb>    the isPerturbing flag value
 *
 */
//------------------------------------------------------------------------------
void FunctionInputData::SetIsPerturbing(bool isPerturb)
{
   isPerturbing = isPerturb;
}

//------------------------------------------------------------------------------
// void SetIsSparsity(bool isSparse);
//------------------------------------------------------------------------------
/**
 * This method sets the isSparsity flag
 *
 * @param <isSparse>    the isSparsity flag value
 *
 */
//------------------------------------------------------------------------------
void FunctionInputData::SetIsSparsity(bool isSparse)
{
   isSparsity = isSparse;
}

//------------------------------------------------------------------------------
// void SetTime(Real toTime);
//------------------------------------------------------------------------------
/**
 * This method sets the time
 *
 * @param <toTime>    the time value
 *
 */
//------------------------------------------------------------------------------
void FunctionInputData::SetTime(Real toTime)
{
   time = toTime;
}

//------------------------------------------------------------------------------
// const Rvector& GetStateVector();
//------------------------------------------------------------------------------
/**
 * This method returns the state vector
 *
 * @return the state vector
 *
 */
//------------------------------------------------------------------------------
const Rvector& FunctionInputData::GetStateVector()
{
   return state;
}

//------------------------------------------------------------------------------
// const Rvector& GetControlVector()
//------------------------------------------------------------------------------
/**
 * This method returns the control vector
 *
 * @return the control vector
 *
 */
//------------------------------------------------------------------------------
const Rvector& FunctionInputData::GetControlVector()
{
   return control;
}

//------------------------------------------------------------------------------
// Integer GetNumStateVars();
//------------------------------------------------------------------------------
/**
 * This method returns the tnumber of state variables
 *
 * @return the number of state variables
 *
 */
//------------------------------------------------------------------------------
Integer FunctionInputData::GetNumStateVars()
{
   return numStateVars;
}

//------------------------------------------------------------------------------
// Integer GetNumControlVars()
//------------------------------------------------------------------------------
/**
 * This method returns the tnumber of control variables
 *
 * @return the number of control variables
 *
 */
//------------------------------------------------------------------------------
Integer FunctionInputData::GetNumControlVars()
{
   return numControlVars;
}

//------------------------------------------------------------------------------
// Integer GetPhaseNum();
//------------------------------------------------------------------------------
/**
 * This method returns the phase number
 *
 * @return the phase number
 *
 */
//------------------------------------------------------------------------------
Integer FunctionInputData::GetPhaseNum()
{
   return phaseNum;
}

//------------------------------------------------------------------------------
// bool IsPerturbing();
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not the data is perturbing
 *
 * @return isPerturbing flag
 *
 */
//------------------------------------------------------------------------------
bool FunctionInputData::IsPerturbing()
{
   return isPerturbing;
}

//------------------------------------------------------------------------------
// bool IsSparsity();
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not the data is sparsity
 *
 * @return isSparsity flag
 *
 */
//------------------------------------------------------------------------------
bool FunctionInputData::IsSparsity()
{
   return isSparsity;
}

//------------------------------------------------------------------------------
// Real GetTime();
//------------------------------------------------------------------------------
/**
 * This method returns the time
 *
 * @return the time
 *
 */
//------------------------------------------------------------------------------
Real FunctionInputData::GetTime()
{
   return time;
}

//------------------------------------------------------------------------------
// void SetStaticVector(const Rvector &toStatic);
//------------------------------------------------------------------------------
/**
* This method sets the static vector
*
* @param <toStatic>    the static vector
*
*/
//------------------------------------------------------------------------------
void FunctionInputData::SetStaticVector(const Rvector &toStatic)
{
   if (toStatic.IsSized() == true)
   {
      Integer sz = toStatic.GetSize();
      staticVars.SetSize(sz);
      staticVars = toStatic;
   }
}

//------------------------------------------------------------------------------
// const Rvector& GetStaticVector();
//------------------------------------------------------------------------------
/**
* This method returns the static vector
*
* @return the static vector
*
*/
//------------------------------------------------------------------------------
const Rvector& FunctionInputData::GetStaticVector()
{
   return staticVars;
}

//------------------------------------------------------------------------------
// Integer GetNumStaticVars();
//------------------------------------------------------------------------------
/**
* This method returns the tnumber of static variables
*
* @return the number of static variables
*
*/
//------------------------------------------------------------------------------
Integer FunctionInputData::GetNumStaticVars()
{
   return numStaticVars;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
