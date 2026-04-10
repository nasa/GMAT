//$Id$
//------------------------------------------------------------------------------
//                            EndFunction
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// order 124.
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/08/30
//
/**
 * Definition code for the EndFunction command, a wrapper that marks the end
 * of the commands in a GMAT function.
 */
//------------------------------------------------------------------------------
 

#ifndef EndFunction_hpp
#define EndFunction_hpp


#include "GmatCommand.hpp"


class GMAT_API EndFunction : public GmatCommand
{
public:
   EndFunction();
   virtual ~EndFunction();
   EndFunction(const EndFunction& ef);
   EndFunction&         operator=(const EndFunction& ef);
   
   virtual GmatBase*    Clone() const;
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name);
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual bool         Execute();
   
   DEFAULT_TO_NO_CLONES

protected:   
   /// Name of the function
   std::string          functionName;
};

#endif /* EndFunction_hpp */
