//$Id$
//------------------------------------------------------------------------------
//                                  PrintUtility
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
// Author: Wendy Shoan
// Created: 2004/11/23
//
/**
 * Contains static methods for printing, including a method that prints out the
 * entire sequence, given the first command.
 *
 * @note - Currently prints to standard output only.
 */
//------------------------------------------------------------------------------


#ifndef PrintUtility_hpp
#define PrintUtility_hpp


#include "gmatdefs.hpp"
#include "GmatCommand.hpp"

// No GMAT_API here because this class is used in the exe, not in a DLL
class PrintUtility
{
public:

   static PrintUtility* Instance();
   bool PrintEntireSequence(GmatCommand* firstCmd);
   ~PrintUtility();

protected:

private:
   PrintUtility();
   PrintUtility(const PrintUtility &pu);
   PrintUtility& operator=(const PrintUtility &pu);
   
   bool PrintBranch(GmatCommand* brCmd, Integer level);
   
   static PrintUtility* onlyInstance;

};
#endif // PrintUtility_hpp
