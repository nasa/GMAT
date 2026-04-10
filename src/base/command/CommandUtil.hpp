//$Id$
//------------------------------------------------------------------------------
//                                 CommandUtil
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/11/28
//
/**
 * This file provides methods to get whole mission sequence string.
 */
//------------------------------------------------------------------------------
#ifndef CommandUtil_hpp
#define CommandUtil_hpp

#include "gmatdefs.hpp"
#include "GmatCommand.hpp"

namespace GmatCommandUtil
{
   GmatCommand GMAT_API *GetFirstCommand(GmatCommand *cmd);
   GmatCommand GMAT_API *GetLastCommand(GmatCommand *cmd);
   GmatCommand GMAT_API *GetNextCommand(GmatCommand *cmd);
   GmatCommand GMAT_API *GetPreviousCommand(GmatCommand *from, GmatCommand *cmd);
   GmatCommand GMAT_API *GetMatchingEnd(GmatCommand *cmd, bool getMatchingElse = false);
   GmatCommand GMAT_API *GetParentCommand(GmatCommand *top, GmatCommand *cmd);
   GmatCommand GMAT_API *GetSubParent(GmatCommand *brCmd, GmatCommand *cmd);
   GmatCommand GMAT_API *RemoveCommand(GmatCommand *seq, GmatCommand *cmd);
   bool GMAT_API IsElseFoundInIf(GmatCommand *ifCmd);
   bool GMAT_API ClearCommandSeq(GmatCommand *seq, bool leaveFirstCmd = true,
                     bool callRunComplete = true);
   bool GMAT_API IsAfter(GmatCommand *cmd1, GmatCommand *cmd2);
   void GMAT_API ResetCommandSequenceChanged(GmatCommand *cmd);
   void GMAT_API ResetBranchCommandChanged(GmatCommand *brCmd, Integer level);
   bool GMAT_API HasCommandSequenceChanged(GmatCommand *cmd);
   bool GMAT_API HasBranchCommandChanged(GmatCommand *brCmd, Integer level);
   bool GMAT_API FindObject(GmatCommand *cmd, UnsignedInt objType,
                     const std::string &objName, std::string &cmdName,
                     GmatCommand **cmdUsing, bool checkWrappers = false);
   bool GMAT_API FindObjectFromSubCommands(GmatCommand *brCmd, Integer level,
                     UnsignedInt objType, const std::string &objName,
                     std::string &cmdName, GmatCommand **cmdUsing, bool checkWrappers = false);
   std::string GMAT_API GetCommandSeqString(GmatCommand *cmd, bool showAddr = true,
                     bool showGenStr = false, bool showSummaryName = false,
                     const std::string &indentStr = "---");
   void GMAT_API GetSubCommandString(GmatCommand* brCmd, Integer level,
                     std::string &cmdseq, bool showAddr = true,
                     bool showGenStr = false, bool showSummaryName = false,
                     const std::string &indentStr = "---");
   void GMAT_API ShowCommand(const std::string &title1, GmatCommand *cmd1,
                     const std::string &title2 = "", GmatCommand *cmd2 = NULL);
}

#endif // CommandUtil_hpp
