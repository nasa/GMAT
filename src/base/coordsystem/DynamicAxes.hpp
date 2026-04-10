//$Id$
//------------------------------------------------------------------------------
//                                  DynamicAxes
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2004/12/28
//
/**
 * Definition of the DynamicAxes class.  This is the base class for those
 * AxisSystem classes that implement dynamic systems.
 *
 */
//------------------------------------------------------------------------------

#ifndef DynamicAxes_hpp
#define DynamicAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "A1Mjd.hpp"

class GMAT_API DynamicAxes : public AxisSystem
{
public:

   // default constructor
   DynamicAxes(const std::string &itsType,
               const std::string &itsName = "");
   // copy constructor
   DynamicAxes(const DynamicAxes &dyn);
   // operator = for assignment
   const DynamicAxes& operator=(const DynamicAxes &dyn);
   // destructor
   virtual ~DynamicAxes();
   
   // initializes the DynamicAxes
   virtual bool Initialize(); // needed?  maybe later
      
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   //virtual GmatBase*       Clone(void) const;

   
protected:

   enum
   {
      DynamicAxesParamCount = AxisSystemParamCount
   };

};
#endif // DynamicAxes_hpp
