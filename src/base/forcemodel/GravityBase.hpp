//$Id$
//------------------------------------------------------------------------------
//                                   GravityBase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// contract, Task Order 28
//
// Author:  Darrel J. Conway, Thinking Systems, Inc.
// Created: Oct 2, 2012
//
/**
 * Definition for a base class for sophisticated gravity models.
 */
//------------------------------------------------------------------------------

#ifndef GravityBase_hpp
#define GravityBase_hpp

#include "PhysicalModel.hpp"

/**
 * Base class defining some gravity model attributes and interfaces.
 *
 * Base class put in place to make ODEModel detections of (potential) central
 * body gravity settings simpler, and to allow for different types of gravity
 * field modeling in the ODEModel.  Eventually, we'll want to refactor the
 * derived models to consolidate common gravity field modeling operations and
 * attributes.
 */
class GMAT_API GravityBase : public PhysicalModel
{
public:
   GravityBase(const std::string &typeStr, const std::string &name = "");
   virtual ~GravityBase();
   GravityBase(const GravityBase& gb);
   GravityBase& operator=(const GravityBase& gb);

protected:
   // When user attributes are added to this class, be sure to move
   // GravityBaseParamCount to the end of the list!
   enum
   {
      GravityBaseParamCount = PhysicalModelParamCount
   };

// Not yet needed
//   static const std::string PARAMETER_TEXT[
//      GravityBaseParamCount - PhysicalModelParamCount];
//
//   static const Gmat::ParameterType PARAMETER_TYPE[
//      GravityBaseParamCount - PhysicalModelParamCount];

};

#endif /* GravityBase_hpp */
