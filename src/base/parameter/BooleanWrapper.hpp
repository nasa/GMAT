//$Id$
//------------------------------------------------------------------------------
//                                  BooleanWrapper
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun/GSFC
// Created: 2007/07/24
//
/**
 * Declares BooleanWrapper class.
 */
//------------------------------------------------------------------------------
#ifndef BooleanWrapper_hpp
#define BooleanWrapper_hpp

#include "gmatdefs.hpp"
#include "ElementWrapper.hpp"

class GMAT_API BooleanWrapper : public ElementWrapper
{
public:

   BooleanWrapper();
   BooleanWrapper(const BooleanWrapper &copy);
   const BooleanWrapper& operator=(const BooleanWrapper &right);
   virtual ~BooleanWrapper();
   
   virtual ElementWrapper*     Clone() const;

   virtual Gmat::ParameterType GetDataType() const;
   
   virtual Real         EvaluateReal() const;
   virtual bool         SetReal(const Real val);
   
   virtual bool         EvaluateBoolean() const;
   virtual bool         SetBoolean(const bool val);
   
protected:  

   // the bool value
   bool value;
   
   virtual void         SetupWrapper();
};
#endif // BooleanWrapper_hpp
