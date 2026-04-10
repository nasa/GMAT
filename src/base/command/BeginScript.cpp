//$Id$
//------------------------------------------------------------------------------
//                              BeginScript
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
// Created: 2004/02/25
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Script tag used to indicate a block of script that shows up verbatim in a
 * ScriptEvent panel on the GUI.
 */
//------------------------------------------------------------------------------


#include "BeginScript.hpp" 
#include "TextParser.hpp"     // for DecomposeBlock()
#include "CommandUtil.hpp"    // for GetNextCommand()
#include <sstream>

#include "MessageInterface.hpp"

//#define DEBUG_BEGINSCRIPT
//#define DBGLVL_GEN_STRING 1

//------------------------------------------------------------------------------
//  BeginScript()
//------------------------------------------------------------------------------
/**
 * Constructs the BeginScript command (default constructor).
 */
//------------------------------------------------------------------------------
BeginScript::BeginScript() :
   GmatCommand("BeginScript")
{
   generatingString = "BeginScript";
}


//------------------------------------------------------------------------------
//  ~BeginScript()
//------------------------------------------------------------------------------
/**
 * Destroys the BeginScript command (destructor).
 */
//------------------------------------------------------------------------------
BeginScript::~BeginScript()
{
}


//------------------------------------------------------------------------------
//  BeginScript(const BeginScript& copy)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the BeginScript command (copy constructor).
 *
 * @param copy The original used to create this one.
 */
//------------------------------------------------------------------------------
BeginScript::BeginScript(const BeginScript& copy) :
    GmatCommand       (copy)
{
   generatingString = copy.generatingString;
}


//------------------------------------------------------------------------------
//  BeginScript& operator=(const BeginScript& copy)
//------------------------------------------------------------------------------
/**
 * Sets this BeginScript to match another one (assignment operator).
 * 
 * @param copy The original used to set parameters for this one.
 *
 * @return this object.
 */
//------------------------------------------------------------------------------
BeginScript& BeginScript::operator=(const BeginScript& copy)
{
   if (this == &copy)
      return *this;

   GmatCommand::operator=(copy);
   generatingString = copy.generatingString;
   
   return *this;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes the BeginScript command.
 * 
 * During mission execution, BeginScript is a null operation -- nothing is done
 * in this command.  It functions as a marker in the script, indicating to the
 * GUI the start of a block of commands that should all be grouped together on a
 * ScriptEvent panel.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool BeginScript::Execute()
{
   BuildCommandSummary(true);
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BeginScript.
 *
 * @return clone of the BeginScript.
 */
//------------------------------------------------------------------------------
GmatBase* BeginScript::Clone() const
{
   return (new BeginScript(*this));
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//                                        const std::string &prefix,
//                                        const std::string &useName)
//------------------------------------------------------------------------------
/**
 * This method returns the generating string of the BeginScript.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name.
 * @param <useName> Name that replaces the object's name.
 *
 * @return The string that reproduces this command.
 */
//------------------------------------------------------------------------------
const std::string& BeginScript::GetGeneratingString(Gmat::WriteMode mode,
                                                    const std::string &prefix,
                                                    const std::string &useName)
{
   //Note: This method is called only once from the ScriptInterpreter::WriteScript(),
   // so all nested ScriptEvent generating strings should be handled here.
   
   std::stringstream gen;
   std::string indent;
   std::string commentLine = GetCommentLine();
   std::string inlineComment = GetInlineComment();
   std::string beginPrefix = prefix;

   if (mode != Gmat::GUI_EDITOR)
   {
      if (mode == Gmat::NO_COMMENTS)
      {
         gen << prefix << "BeginScript" << "\n";
      }
      else
      {
         IndentComment(gen, commentLine, prefix);
         
         // Insert command name (Fix for GMT-2612, LOJ: 2012.10.22)
         //gen << prefix << "BeginScript";
         std::string tempString = prefix + "BeginScript";
         
         InsertCommandName(tempString);
         gen << tempString;
         
         if (inlineComment != "")
            gen << inlineComment << "\n";
         else
            gen << "\n";
      }
   }
   
   #if DBGLVL_GEN_STRING
   MessageInterface::ShowMessage
      ("BeginScript::GetGeneratingString() this=(%p)%s, mode=%d, prefix='%s', "
       "useName='%s'\n", this, this->GetTypeName().c_str(), mode, prefix.c_str(),
       useName.c_str());
   #endif
   
   if (mode == Gmat::GUI_EDITOR)
      indent = "";
   else
      indent = "   ";
   
   GmatCommand *current = next;
   while (current != NULL)
   {      
      #if DBGLVL_GEN_STRING > 1
      MessageInterface::ShowMessage
         ("BeginScript::GetGeneratingString() current=(%p)%s\n", current,
          current->GetTypeName().c_str());
      #endif
      
      if (current->GetTypeName() != "EndScript")
      {
         // Indent whole block within Begin/EndScript
         IndentChildString(gen, current, indent, mode, prefix, useName, false);
         
         // Get command after EndScript
         current = GmatCommandUtil::GetNextCommand(current);
         
         if (current == NULL)
            IndentChildString(gen, current, indent, mode, beginPrefix, useName, true);
      }
      else
      {
         if (mode != Gmat::GUI_EDITOR)
         {
            // Indent whole block within Begin/EndScript
            IndentChildString(gen, current, indent, mode, beginPrefix, useName, true);
         }
         else
         {
            std::string comment = current->GetCommentLine();
            #if DBGLVL_GEN_STRING
            MessageInterface::ShowMessage("   EndScript comment = '%s'\n", comment.c_str());
            #endif
            // Only indent inline comment of EndScript (LOJ: 2013.03.27)
            gen << indent << comment;
         }
         current = NULL;
      }
   }
   
   generatingString = gen.str();
   
   #if DBGLVL_GEN_STRING
   MessageInterface::ShowMessage
      ("BeginScript::GetGeneratingString() returnning generatingString\n");
   MessageInterface::ShowMessage("<<<\n%s>>>\n\n", generatingString.c_str());
   #endif
   
   return generatingString;
}

//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type    Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool BeginScript::RenameRefObject(const UnsignedInt type,
                                  const std::string &oldName,
                                  const std::string &newName)
{   
   GmatCommand *current = next;
   
   while (current != NULL)
   {
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("BeginScript::RenameRefObject() current=%s\n",
          current->GetTypeName().c_str());
      #endif
      
      if (current->GetTypeName() != "EndScript")
      {
         current->RenameRefObject(type, oldName, newName);
         current = current->GetNext();
      }
      else
      {
         current = NULL;
      }
   }
   
   return true;
}


//------------------------------------------------------------------------------
//void IndentChildString(std::stringstream &gen, GmatCommand* cmd, 
//                       std::string &indent, Gmat::WriteMode mode,
//                       const std::string &prefix, const std::string &useName,
//                       bool indentCommentOnly)
//------------------------------------------------------------------------------
/**
 * Indents the child string part(s) of the generatingString.
 *
 * @param <gen>               Generating string
 * @param <cmd>               Command for which to get the child string
 * @param <indent>            Indent string to use
 * @param <mode>              Specifies the type of serialization requested.
 * @param <prefix>            Optional prefix appended to the object's name.
 * @param <useName>           Name that replaces the object's name.
 * @param <indentCommentOnly> Flag specifying whether or not to ONLY indent the
 *                            comment
 */
//------------------------------------------------------------------------------
void BeginScript::IndentChildString(std::stringstream &gen, GmatCommand* cmd, 
                                    std::string &indent, Gmat::WriteMode mode,
                                    const std::string &prefix,
                                    const std::string &useName,
                                    bool indentCommentOnly)
{
   TextParser tp;
   
   #if DBGLVL_GEN_STRING
   ShowCommand("", "BeginScript::IndentChildString() cmd = ", cmd);
   MessageInterface::ShowMessage
      ("BeginScript::IndentChildString() indent='%s', mode=%d, prefix='%s', "
       "useName='%s', indentCommentOnly=%d\n", indent.c_str(), mode, prefix.c_str(),
       useName.c_str(), indentCommentOnly);
   #endif
   
   std::string cmdstr;
   if (indentCommentOnly)
      cmdstr = cmd->GetCommentLine();
   else
      cmdstr = cmd->GetGeneratingString(mode, prefix, useName);
   
   StringArray textArray = tp.DecomposeBlock(cmdstr);
   UnsignedInt size = textArray.size();
   
   #if DBGLVL_GEN_STRING
   MessageInterface::ShowMessage("   There are %d text lines\n", size);
   #endif
   
   if (size > 0 && textArray[0] != "")
   {
      for (UnsignedInt i=0; i<size; i++)
      {
         if (indentCommentOnly)
            gen << indent << prefix << textArray[i];
         else
            gen << indent << textArray[i];
         
         if (textArray[i].find("\n") == cmdstr.npos &&
             textArray[i].find("\r") == cmdstr.npos)
         {
            gen << "\n";
         }
      }
   }
   
   if (indentCommentOnly)
      gen << prefix << cmd->GetTypeName() << ";" << cmd->GetInlineComment();
}


//------------------------------------------------------------------------------
//void IndentComment(std::stringstream &gen, std::string &comment,
//                   const std::string &prefix)
//------------------------------------------------------------------------------
/**
 * Indents the child string part(s) of the generatingString.
 *
 * @param <gen>               Generating string
 * @param <comment>           Comment to indent
 * @param <prefix>            Optional prefix appended to the object's name.
 */
//------------------------------------------------------------------------------
void BeginScript::IndentComment(std::stringstream &gen, std::string &comment,
                                const std::string &prefix)
{
   TextParser tp;
   
   #if DBGLVL_GEN_STRING
   MessageInterface::ShowMessage
      ("BeginScript::IndentComment() comment='%s', prefix='%s'\n",
       comment.c_str(), prefix.c_str());
   #endif
   
   StringArray textArray = tp.DecomposeBlock(comment);
   UnsignedInt size = textArray.size();
   
   if (size > 0 && textArray[0] != "")
   {
      for (UnsignedInt i=0; i<size; i++)
      {
         gen << prefix << textArray[i];
         
         if (textArray[i].find("\n") == comment.npos &&
             textArray[i].find("\r") == comment.npos)
         {
            gen << "\n";
         }
      }
   }
}

