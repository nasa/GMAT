//$Id: EnvParameters.hpp 9513 2011-04-30 21:23:06Z djcinsb $
//------------------------------------------------------------------------------
//                             File: EnvParameters.hpp
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/12/10
//
/**
 * Declares Environment related parameter classes.
 *   AtmosDensity
 */
//------------------------------------------------------------------------------
#ifndef EnvParameters_hpp
#define EnvParameters_hpp

#include "newparameter_defs.hpp"
#include "EnvReal.hpp"

//==============================================================================
//                              AtmosDensity
//==============================================================================
/**
 * Declares atmospheric density parameter class.
 */
//------------------------------------------------------------------------------

class NEW_PARAMETER_API AtmosDensity : public EnvReal
{
public:

   AtmosDensity(const std::string &name = "", GmatBase *obj = NULL);
   AtmosDensity(const AtmosDensity &copy);
   const AtmosDensity& operator=(const AtmosDensity &right);
   virtual ~AtmosDensity();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

#endif //EnvParameters_hpp
