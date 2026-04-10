//$Id$
//------------------------------------------------------------------------------
//                                   BuiltinGmatFunction
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.05.02
//
/**
 * Defines the BuiltinGmatFunction class.
 */
//------------------------------------------------------------------------------
#ifndef BuiltinGmatFunction_hpp
#define BuiltinGmatFunction_hpp

#include "ObjectManagedFunction.hpp"

class GMAT_API BuiltinGmatFunction : public ObjectManagedFunction
{
public:
   BuiltinGmatFunction(const std::string &typeStr, const std::string &nomme);
   virtual ~BuiltinGmatFunction();
   BuiltinGmatFunction(const BuiltinGmatFunction &f);
   BuiltinGmatFunction& operator=(const BuiltinGmatFunction &f);
   
   // inherited from Function
   virtual bool         Initialize(ObjectInitializer *objInit,
                                   bool reinitialize = false,
                                   bool resetLocalObjects = true);
   
   // inherited from GmatBase
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
};


#endif // BuiltinGmatFunction_hpp
