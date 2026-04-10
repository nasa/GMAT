//$Id$
//------------------------------------------------------------------------------
//                                  NoOp
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
// Author: Darrel J. Conway
// Created: 2003/10/24
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Null operator for the command sequence -- typically used as a place holder
 */
//------------------------------------------------------------------------------



#ifndef NoOp_hpp
#define NoOp_hpp

#include "GmatCommand.hpp" // inheriting class's header file

/**
 * Default command used to initialize the command sequence lists in the 
 * Moderator
 */
class GMAT_API NoOp : public GmatCommand
{
public:
   NoOp();
   virtual ~NoOp();
   NoOp(const NoOp& noop);
   NoOp&                   operator=(const NoOp &noop);

   bool                    Execute();

   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS
};


#endif // NoOp_hpp
