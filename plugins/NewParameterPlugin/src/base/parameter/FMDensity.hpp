//$Id$
//------------------------------------------------------------------------------
//                             FMDensity
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/29
//
/**
 * Declares force model based density parameter class.
 */
//------------------------------------------------------------------------------
#ifndef FMDensity_hpp
#define FMDensity_hpp

#include "OdeReal.hpp"
#include "OdeData.hpp"

//==============================================================================
//                              FMDensity
//==============================================================================
/**
 * Declares atmospheric density parameter class.
 */
//------------------------------------------------------------------------------

class NEW_PARAMETER_API FMDensity : public OdeReal
{
public:

   FMDensity(const std::string &name = "", GmatBase *obj = NULL);
   FMDensity(const FMDensity &copy);
   const FMDensity& operator=(const FMDensity &right);
   virtual ~FMDensity();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:

};

#endif //FMDensity_hpp
