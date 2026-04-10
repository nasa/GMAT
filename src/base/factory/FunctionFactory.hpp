//$Id$
//------------------------------------------------------------------------------
//                              FunctionFactory
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
// Author: Linda Jun, NASA/GSFC
// Created: 2006/04/05
//
/**
 * Implementation code for the FunctionFactory class, responsible for creating 
 * Function objects.
 */
//------------------------------------------------------------------------------
#ifndef FunctionFactory_hpp
#define FunctionFactory_hpp

#include "Function.hpp"
#include "Factory.hpp"

class GMAT_API FunctionFactory : public Factory
{
public:
   Function* CreateObject(const std::string &ofType,
                          const std::string &withName = "");
   Function* CreateFunction(const std::string &ofType,
                            const std::string &withName = "");
   
   FunctionFactory();
   virtual ~FunctionFactory();
   FunctionFactory(StringArray createList);
   FunctionFactory(const FunctionFactory &fact);
   FunctionFactory& operator= (const FunctionFactory &fact);
   
protected:
   void BuildCreatables();
   
};

#endif // FunctionFactory_hpp
