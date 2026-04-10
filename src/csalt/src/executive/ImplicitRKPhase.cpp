//------------------------------------------------------------------------------
//                              ImplicitRKPhase
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
// Created: 2016.05.18
//
/**
* From original MATLAB prototype:
* Author: S. Hughes.  steven.p.hughes@nasa.gov
*/
//------------------------------------------------------------------------------

#include "ImplicitRKPhase.hpp"

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
ImplicitRKPhase::ImplicitRKPhase() :
   Phase()
{
   // For ImplicitRK, the number of defect constraints is one less than the number
   // of time points
   constraintTimeOffset = 1;
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
ImplicitRKPhase::ImplicitRKPhase(const ImplicitRKPhase &copy) :
   Phase(copy),
   collocationMethod (copy.collocationMethod)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
ImplicitRKPhase& ImplicitRKPhase::operator=(const ImplicitRKPhase &copy)
{
   if (this == &copy)
      return *this;
   
   Phase::operator=(copy);

   collocationMethod = copy.collocationMethod;

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ImplicitRKPhase::~ImplicitRKPhase()
{
}

//------------------------------------------------------------------------------
//  void InitializeTranscription()
//------------------------------------------------------------------------------
/**
 * This method initializes the ImplicitRKPhase
 *
 *
 */
//------------------------------------------------------------------------------
void  ImplicitRKPhase::InitializeTranscription()
{
   // default collocation is RungeKutta 8
   if (strcmp(collocationMethod.c_str(),"") == 0)
   {
      SetTranscription("RungeKutta8");
      if (transUtil)
         delete transUtil;
      transUtil = new NLPFuncUtil_ImplicitRK(collocationMethod);
      transUtil->Initialize(config);
   }
   else
   {
      if (transUtil)
         delete transUtil;
      transUtil = new NLPFuncUtil_ImplicitRK(collocationMethod);
      transUtil->Initialize(config);
   }
}

//------------------------------------------------------------------------------
//  void SetTranscription(std::string type)
//------------------------------------------------------------------------------
/**
 * This method sets the transcription
 *
 * @param <type>       the input collocation type
 *
 */
//------------------------------------------------------------------------------
void ImplicitRKPhase::SetTranscription(std::string type)
{
   collocationMethod = type;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none

