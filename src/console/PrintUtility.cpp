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
 * entire sequence, given the first command - Implementation.
 *
 * @note - Currently prints to standard output only.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatCommand.hpp"
#include "PrintUtility.hpp"

#include <iostream>
using namespace std;

PrintUtility* PrintUtility::onlyInstance = NULL;

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Instance()
//------------------------------------------------------------------------------
/**
 * Returns (and creates initially) the only instance allowed for this singleton
 * class. 
 *
 * @return Instance of the PrintUtility.
 */
//------------------------------------------------------------------------------
PrintUtility* PrintUtility::Instance()
{
   if (onlyInstance == NULL)
      onlyInstance = new PrintUtility();

   return onlyInstance;
}

bool PrintUtility::PrintEntireSequence(GmatCommand* firstCmd)
{
   cout << ".................... Print out the whole sequence ........................................" << endl;
   GmatCommand *current = firstCmd;
   while (current != NULL)
   {
      cout << "   Command::" << current->GetTypeName() << endl;
      if ((current->GetChildCommand(0))!=NULL)
         PrintBranch(current, 0);
      current = current->GetNext();
   }
   cout << ".................... End sequence ........................................................" << endl;
   return true;
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

//------------------------------------------------------------------------------
//  PrintUtility()
//------------------------------------------------------------------------------
/**
 * Constructs a PrintUtility object (default constructor).
 */
//------------------------------------------------------------------------------
PrintUtility::PrintUtility()
{
}

// copy constructor and operator= not implemented

bool PrintUtility::PrintBranch(GmatCommand* brCmd, Integer level)
{
   GmatCommand* current = brCmd;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   
   while((child = current->GetChildCommand(childNo))!=NULL)
   {
      nextInBranch = child;
      while ((nextInBranch != NULL) && (nextInBranch != current))
      {
         cout << "   ";
         for (int i = 0; i <= level ; i++)
            cout << "...";
         cout << " branch " << childNo << "::" << nextInBranch->GetTypeName() << endl;
         if (nextInBranch->GetChildCommand() != NULL)
         {
            PrintBranch(nextInBranch, level+1);
         }
         nextInBranch = nextInBranch->GetNext();
      }
      ++childNo;
   }
   
   return true;
}

