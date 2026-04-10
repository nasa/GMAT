//$Id$
//------------------------------------------------------------------------------
//                                  Stop
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
 * Stop function for the command sequence -- typically used while debugging
 */
//------------------------------------------------------------------------------



#ifndef Stop_hpp
#define Stop_hpp

#include "GmatCommand.hpp" // inheriting class's header file

/**
 * Default command used to initialize the command sequence lists in the 
 * Moderator
 */
class GMAT_API Stop : public GmatCommand
{
public:
   Stop();
   virtual ~Stop();
   Stop(const Stop& noop);
   Stop&                   operator=(const Stop &noop);

   bool                    Execute();

   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual const std::string&
                     GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                         const std::string &prefix = "",
                                         const std::string &useName = "");

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS
};


#endif // Stop_hpp
