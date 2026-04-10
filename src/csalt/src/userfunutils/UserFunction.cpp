//------------------------------------------------------------------------------
//                              UserFunction
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

#include "UserFunction.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
UserFunction::UserFunction() :
   numAlgFunctions   (0),
   hasAlgFunctions   (false),
   hasCostFunction   (false),
   isInitializing    (false),
   statePert         (1e-7),
   controlPert       (1e-7),
   timePert          (1e-7),
   staticPert        (1e-7)
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
UserFunction::UserFunction(const UserFunction &copy) :
   numAlgFunctions   (copy.numAlgFunctions),
   hasAlgFunctions   (copy.hasAlgFunctions),
   hasCostFunction   (copy.hasCostFunction),
   isInitializing    (copy.isInitializing),
   statePert         (copy.statePert),
   controlPert       (copy.controlPert),
   timePert          (copy.timePert),
   staticPert        (copy.statePert)
{
   phaseScaleUtilList.clear();
   for (UnsignedInt ii = 0; ii < copy.phaseScaleUtilList.size(); ii++)
      phaseScaleUtilList.push_back(copy.phaseScaleUtilList.at(ii));
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
UserFunction& UserFunction::operator=(const UserFunction &copy)
{
   if (&copy == this)
      return *this;
   
   numAlgFunctions    = copy.numAlgFunctions;
   hasAlgFunctions    = copy.hasAlgFunctions;
   hasCostFunction    = copy.hasCostFunction;
   isInitializing     = copy.isInitializing;
   statePert          = copy.statePert;
   controlPert        = copy.controlPert;
   timePert           = copy.timePert;
   staticPert         = copy.staticPert;
      
   phaseScaleUtilList.clear();
   for (UnsignedInt ii = 0; ii < copy.phaseScaleUtilList.size(); ii++)
      phaseScaleUtilList.push_back(copy.phaseScaleUtilList.at(ii));

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
UserFunction::~UserFunction()
{
   // do nothing here 
}

//------------------------------------------------------------------------------
// void SetIsInitializing(bool isInit)
//------------------------------------------------------------------------------
/**
 * This method sets the isInitializing flag
 *
 */
//------------------------------------------------------------------------------
void UserFunction::SetIsInitializing(bool isInit)
{
   isInitializing = isInit;
}


//------------------------------------------------------------------------------
// bool IsInitializing()
//------------------------------------------------------------------------------
/**
 * This method returns the isInitializing flag
 *
 * @return true if the function is initializing; false, otherwise
 */
//------------------------------------------------------------------------------
bool UserFunction::IsInitializing()
{
   return isInitializing;
}

//------------------------------------------------------------------------------
// void SetPhaseList(std::vector<Phase*> pList)
//------------------------------------------------------------------------------
/**
* This method sets the list of phase objects in the trajectory
*
* @param <pList> The phase list
*/
//------------------------------------------------------------------------------
void UserFunction::SetPhaseScaleUtilList(std::vector<ScalingUtility*> scalingList)
{
   phaseScaleUtilList.clear();
   for (UnsignedInt ii = 0; ii < scalingList.size(); ii++)
      phaseScaleUtilList.push_back(scalingList.at(ii));
}

//------------------------------------------------------------------------------
// void SetStatePerturbation(Real statePertIn)
//------------------------------------------------------------------------------
/**
* This method sets the perturbation for state variables 
*
* @param <statePertIn> Perturbation step size for state variables
*/
//------------------------------------------------------------------------------
void UserFunction::SetStatePerturbation(Real statePertIn)
{
   statePert = statePertIn;
}

//------------------------------------------------------------------------------
// void SetTimePerturbation(Real timePertIn)
//------------------------------------------------------------------------------
/**
* This method sets the perturbation for time variables
*
* @param <timePertIn> Perturbation step size for state variables
*/
//------------------------------------------------------------------------------
void UserFunction::SetTimePerturbation(Real timePertIn)
{
   timePert = timePertIn;
}

//------------------------------------------------------------------------------
// void SetControlPerturbation(Real controlPertIn)
//------------------------------------------------------------------------------
/**
* This method sets the perturbation for control variables
*
* @param <controlPertIn> Perturbation step size for state variables
*/
//------------------------------------------------------------------------------
void UserFunction::SetControlPerturbation(Real controlPertIn)
{
   controlPert = controlPertIn;
}

//------------------------------------------------------------------------------
// Real GetStatePerturbation()
//------------------------------------------------------------------------------
/**
* This method returns state perturbation step size
*
* @return the state perturbation step size
*/
//------------------------------------------------------------------------------
Real UserFunction::GetStatePerturbation()
{
   return statePert;
}

//------------------------------------------------------------------------------
// Real GetTimePerturbation()
//------------------------------------------------------------------------------
/**
* This method returns time perturbation step size
*
* @return the time perturbation step size
*/
//------------------------------------------------------------------------------
Real UserFunction::GetTimePerturbation()
{
   return timePert;
}

//------------------------------------------------------------------------------
// Real GetControlPerturbation()
//------------------------------------------------------------------------------
/**
* This method returns control perturbation step size
*
* @return the control perturbation step size
*/
//------------------------------------------------------------------------------
Real UserFunction::GetControlPerturbation()
{
   return controlPert;
}

//------------------------------------------------------------------------------
// Real GetStaticPerturbation()
//------------------------------------------------------------------------------
/**
* This method returns static perturbation step size
*
* @return the static perturbation step size
*/
//------------------------------------------------------------------------------
Real UserFunction::GetStaticPerturbation()
{
   return staticPert;
}