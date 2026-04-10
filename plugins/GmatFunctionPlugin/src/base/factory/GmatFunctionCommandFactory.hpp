//$Id$
//------------------------------------------------------------------------------
//                        GmatFunctionCommandFactory
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
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 14, 2013
/**
 * Factory for commands associated with GMAT functions
 */
//------------------------------------------------------------------------------

#ifndef GmatFunctionCommandFactory_hpp
#define GmatFunctionCommandFactory_hpp

#include "GmatFunction_defs.hpp"
#include "Factory.hpp"

/**
 * Factory that creates commands used in GMAT functions
 */
class GMATFUNCTION_API GmatFunctionCommandFactory : public Factory
{
public:
   GmatFunctionCommandFactory();
   virtual ~GmatFunctionCommandFactory();
   GmatFunctionCommandFactory(const GmatFunctionCommandFactory& elf);
   GmatFunctionCommandFactory& operator=(const GmatFunctionCommandFactory& elf);

   virtual GmatCommand*     CreateCommand(const std::string &ofType,
                                          const std::string &withName = "");
};

#endif /* GmatFunctionCommandFactory_hpp */
