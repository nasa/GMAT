//$Id$
//------------------------------------------------------------------------------
//                             EndIf
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/01/30
//
/**
 * Definition for the closing line of the IF Statement
 */
//------------------------------------------------------------------------------


#ifndef EndIf_hpp
#define EndIf_hpp


#include "GmatCommand.hpp"


class GMAT_API EndIf : public GmatCommand
{
public:
   EndIf();
   EndIf(const EndIf& ic);
   EndIf&                  operator=(const EndIf& ic);
   virtual ~EndIf();

   virtual bool            Initialize();
   virtual bool            Execute();

   virtual bool            Insert(GmatCommand *cmd, GmatCommand *prev);

   // inherited from GmatBase
   virtual bool            RenameRefObject(const UnsignedInt type,
                                           const std::string &oldName,
                                           const std::string &newName);   
   virtual GmatBase*       Clone() const;
   const std::string&      GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName);

   DEFAULT_TO_NO_CLONES
};


#endif // EndIf_hpp
