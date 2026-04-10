//$Id$
//------------------------------------------------------------------------------
//                                If
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
// Author:  Joey Gurganus/GSFC
// Created: 2004/01/30
//
/**
 * Definition for the If command class
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "gmatdefs.hpp"
#include "If.hpp"
#include "Parameter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_IF_EXEC
//#define DEBUG_IF_APPEND


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string
If::PARAMETER_TEXT[IfParamCount - ConditionalBranchParamCount] =
{
   "NestLevel",
};

const Gmat::ParameterType
If::PARAMETER_TYPE[IfParamCount - ConditionalBranchParamCount] =
{
   Gmat::INTEGER_TYPE,     
};

//------------------------------------------------------------------------------
//  If()
//------------------------------------------------------------------------------
/**
 * Creates a If command.  (default constructor)
 */
//------------------------------------------------------------------------------
If::If() :
   ConditionalBranch  ("If"),
   nestLevel          (0)
{
}


//------------------------------------------------------------------------------
//  If(const If &ic)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates an If command.  (Copy constructor)
 */
//------------------------------------------------------------------------------
If::If(const If &ic) :
   ConditionalBranch  (ic),
   nestLevel          (0)
{
}


//------------------------------------------------------------------------------
//  If& operator=(const If &ic)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the If command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
If& If::operator=(const If &ic)
{
   if (this == &ic)
      return *this;
   ConditionalBranch::operator=(ic);
   nestLevel = ic.nestLevel;

   return *this;
}


//------------------------------------------------------------------------------
//  ~If()
//------------------------------------------------------------------------------
/**
 * Destroys the If command.  (destructor)
 */
//------------------------------------------------------------------------------
If::~If()
{
}

    
//------------------------------------------------------------------------------
//  bool Append(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the IF statement.
 *
 * This method calls the BranchCommand base class method that adds a command
 * to the command sequence that branches off of the main mission sequence.  This
 * extension was needed so that the EndIf command can be set to point back 
 * to the head of the IF statement.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool If::Append(GmatCommand *cmd)
{
    if (!ConditionalBranch::Append(cmd))
        return false;
        
     #ifdef DEBUG_IF_APPEND
        MessageInterface::ShowMessage("If::Append .... type being appended is %s\n",
        (cmd->GetTypeName()).c_str());
     #endif

    // Check for the end of "If" branch, point that end back to this command
    if (cmd->GetTypeName() == "EndIf" || cmd->GetTypeName() == "Else" ||
        cmd->GetTypeName() == "ElseIf")
    {
     #ifdef DEBUG_IF_APPEND
        MessageInterface::ShowMessage("If::Append (if) .... nestLevel = %d\n",
        nestLevel);
     #endif
       if ((nestLevel== 0) && (branchToFill != -1))
       {
          cmd->Append(this);
          // IF statement is complete; -1 points us back to the main sequence.
          if (cmd->GetTypeName() == "EndIf")
             branchToFill = -1;
          else // "Else" or "ElseIf" starts another branch
             ++branchToFill;
       }
       else
       {
         // only decrease the nextLevel if we've reached the actual end of the 
         // If command
          if (cmd->GetTypeName() == "EndIf")  --nestLevel;
       }
    }

    if (cmd->GetTypeName() == "If")
    {
       ++nestLevel;
    }

    return true;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Execute the proper branch for this IF statement.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool If::Execute()
{
   bool retval = true;
   
   if (branchExecuting)
   {
      #ifdef DEBUG_IF_EXEC
      MessageInterface::ShowMessage
      ("In If::Execute - Branch Executing -------------\n");
      #endif
      retval = ExecuteBranch(branchToExecute);
      #ifdef DEBUG_IF_EXEC
         MessageInterface::ShowMessage
         ("In If:: retval returned from ExecuteBranch = %s\n", (retval? "true" : "false"));
         MessageInterface::ShowMessage
         ("        branchExecuting=%d\n", branchExecuting);
      #endif
      if (!branchExecuting)
      {
         commandComplete  = true;
         commandExecuting = false;
      }
   }
   else 
   {
      #ifdef DEBUG_IF_EXEC
      MessageInterface::ShowMessage
      ("In If::Execute - Branch NOT Executing -------------\n");
      #endif
      if (!commandExecuting)
         ConditionalBranch::Execute();
      
      if (EvaluateAllConditions()) 
      {
         #ifdef DEBUG_IF_EXEC
         MessageInterface::ShowMessage
         ("In If::Execute all conditions are true - executing first branch\n");
         #endif
         branchToExecute = 0;
         branchExecuting = true;
         commandComplete = false;
         commandExecuting = true;
      }
      else if ((Integer)branch.size() > 1)  // there could be an 'Else'
      {
         #ifdef DEBUG_IF_EXEC
         MessageInterface::ShowMessage
         ("In If::Execute some conditions are FALSE - executing second branch\n");
         #endif
         branchExecuting = true;
         branchToExecute = 1; // @todo - add ElseIf (more than two branches)
         commandComplete = false;
         commandExecuting = true;
      }
      else
      {
         #ifdef DEBUG_IF_EXEC
         MessageInterface::ShowMessage
         ("In If::Execute - conditions are FALSE - no other branch to execute\n");
         #endif
         branchToExecute = 0;
         commandComplete  = true;
         commandExecuting = false;
         branchExecuting = false;  // try this - shouldn't matter
      }
   }
   
   BuildCommandSummary(true);
   #ifdef DEBUG_IF_EXEC
   MessageInterface::ShowMessage
      ("If::BuildCommandSummary completed\n");
   #endif
   return retval;
} // Execute()


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string If::GetParameterText(const Integer id) const
{
   if (id >= ConditionalBranchParamCount && id < IfParamCount)
      return PARAMETER_TEXT[id - ConditionalBranchParamCount];
   return ConditionalBranch::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer If::GetParameterID(const std::string &str) const
{
   for (Integer i = ConditionalBranchParamCount; i < IfParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ConditionalBranchParamCount])
         return i;
   }
   
   return ConditionalBranch::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType If::GetParameterType(const Integer id) const
{
   if (id >= ConditionalBranchParamCount && id < IfParamCount)
      return PARAMETER_TYPE[id - ConditionalBranchParamCount];
   
   return ConditionalBranch::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string If::GetParameterTypeString(const Integer id) const
{
   return ConditionalBranch::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer If::GetIntegerParameter(const Integer id) const
{
   if (id == NEST_LEVEL)          return nestLevel;
   
   return ConditionalBranch::GetIntegerParameter(id); 
}

//------------------------------------------------------------------------------
//  Integer  SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * This method sets the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Integer value for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer If::SetIntegerParameter(const Integer id,
                                const Integer value) 
{
   if (id == NEST_LEVEL)          return (nestLevel  = value);
   
   return ConditionalBranch::SetIntegerParameter(id,value);
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer If::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Integer  SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * This method sets the Integer parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 * @param <value> Integer value for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer If::SetIntegerParameter(const std::string &label,
                                const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the If.
 *
 * @return clone of the If.
 *
 */
//------------------------------------------------------------------------------
GmatBase* If::Clone() const
{
   return (new If(*this));
}

//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                         const std::string &prefix,
//                                         const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name.  (Used to
 *                indent commands)
 * @param useName Name that replaces the object's name.  (Not used in
 *                commands)
 *
 * @return The script line that, when interpreted, defines this If command.
 */
//------------------------------------------------------------------------------
const std::string& If::GetGeneratingString(Gmat::WriteMode mode,
                                           const std::string &prefix,
                                           const std::string &useName)
{
   if (mode == Gmat::NO_COMMENTS)
   {
      generatingString = "If " + GetConditionalString();
	  InsertCommandName(generatingString);
      return generatingString;
   }
   
   // Build the local string
   generatingString = prefix + "If " + GetConditionalString();
   return ConditionalBranch::GetGeneratingString(mode, prefix, useName);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
