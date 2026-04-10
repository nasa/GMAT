//$Id$
//------------------------------------------------------------------------------
//                                  SaveMission
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
// Author: Linda Jun (NASA/GSFC)
// Created: 2010/08/03
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class implementation for the SaveMission command
 */
//------------------------------------------------------------------------------

#include "SaveMission.hpp"
#include "CommandUtil.hpp"         // for GetLastCommand()
#include "FileManager.hpp"         // for GetPathname()
#include "FileUtil.hpp"            // for ParseFileExtension()
#include "StringUtil.hpp"          // for RemoveEnclosingString()
#include "Moderator.hpp"           // for GetScript()
#include "MessageInterface.hpp"    // for ShowMessage()
#include <fstream>                 // for fopen(), <<

//#define DEBUG_SAVEMISSION_IA
//#define DEBUG_SAVEMISSION_EXE

//---------------------------------
//  static data
//---------------------------------
const std::string
SaveMission::PARAMETER_TEXT[SaveMissionParamCount - GmatCommandParamCount] = 
{
   "Filename",
};

const Gmat::ParameterType
SaveMission::PARAMETER_TYPE[SaveMissionParamCount - GmatCommandParamCount] =
{
   Gmat::FILENAME_TYPE,   // "Filename",
};


//------------------------------------------------------------------------------
//  SaveMission()
//------------------------------------------------------------------------------
/**
 * Constructs the SaveMission command (default constructor).
 */
//------------------------------------------------------------------------------
SaveMission::SaveMission() :
    GmatCommand("SaveMission")
{
   parameterCount = SaveMissionParamCount;
}


//------------------------------------------------------------------------------
//  ~SaveMission()
//------------------------------------------------------------------------------
/**
 * Destroys the SaveMission command (default constructor).
 */
//------------------------------------------------------------------------------
SaveMission::~SaveMission()
{
}


//------------------------------------------------------------------------------
//  SaveMission(const SaveMission& noop)
//------------------------------------------------------------------------------
/**
 * Makes a copy of the SaveMission command (copy constructor).
 *
 * @param noop The original used to set parameters for this one.
 */
//------------------------------------------------------------------------------
SaveMission::SaveMission(const SaveMission& noop) :
    GmatCommand (noop)
{
}


//------------------------------------------------------------------------------
//  SaveMission& operator=(const SaveMission&)
//------------------------------------------------------------------------------
/**
 * Sets this SaveMission to match another one (assignment operator).
 * 
 * @param noop The original used to set parameters for this one.
 *
 * @return this instance.
 */
//------------------------------------------------------------------------------
SaveMission& SaveMission::operator=(const SaveMission& noop)
{
   return *this;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Executes the SaveMission command (copy constructor).
 * 
 * Like the name implies, SaveMission is a null operation -- nothing is done in this 
 * command.  It functions as a place holder, and as the starting command in the 
 * command sequence managed by the Moderator.
 *
 * @return true always.
 */
//------------------------------------------------------------------------------
bool SaveMission::Execute()
{
   // If this command is not in a function and is the last command, write
   // whole mission to a file
   if (currentFunction == NULL && GmatCommandUtil::GetLastCommand(this) == this)
   {
      FileManager *fm = FileManager::Instance();
      std::string outPath = fm->GetAbsPathname(FileManager::OUTPUT_PATH);
      std::string fname = fileName;
      
      // add output path if there is no path
      if (fileName.find("/") == fileName.npos &&
          fileName.find("\\") == fileName.npos)
      {
         fname = outPath + fileName;
      }
      
      // If file name has no extension, add .script
      if (GmatFileUtil::ParseFileExtension(fname) == "")
         fname = fname + ".script";
      
      // Call Moderator::GetScript();
      std::string script = Moderator::Instance()->GetScript();
      
      #ifdef DEBUG_SAVEMISSION_EXE
      MessageInterface::ShowMessage
         ("SaveMission::Execute() '%s' should save mission to a file\n",
          GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      MessageInterface::ShowMessage("   fileName='%s'\n", fname.c_str());
      //MessageInterface::ShowMessage(script);
      #endif
      
      std::ofstream dstream;
      dstream.open(fname.c_str());
      dstream << script;
   }
   
   BuildCommandSummary(true);
   return true;
}


//------------------------------------------------------------------------------
//  void InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The SaveMission command has the following syntax:
 *
 *     SaveMission 'file name'
 */
//------------------------------------------------------------------------------
bool SaveMission::InterpretAction()
{
   #ifdef DEBUG_SAVEMISSION_IA
   MessageInterface::ShowMessage
      ("SaveMission::InterpretAction(), generatingString = %s\n",
       generatingString.c_str());
   #endif
   
   StringArray chunks = InterpretPreface();
   
   #ifdef DEBUG_SAVEMISSION_IA
   for (UnsignedInt i=0; i<chunks.size(); i++)
      MessageInterface::ShowMessage("   %d: '%s'\n", i, chunks[i].c_str());
   #endif
   
   if (chunks.size() < 2)
      throw CommandException("Missing information for MissionSave command.\n");
   
   fileName = chunks[1];
   
   // Remove single quotes
   fileName = GmatStringUtil::RemoveEnclosingString(chunks[1], "'");
   
   #ifdef DEBUG_SAVEMISSION_IA
   MessageInterface::ShowMessage("   fileName = '%s'\n", fileName.c_str());
   MessageInterface::ShowMessage
      ("SaveMission::InterpretAction() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the SaveMission.
 *
 * @return clone of the SaveMission.
 */
//------------------------------------------------------------------------------
GmatBase* SaveMission::Clone() const
{
   return (new SaveMission(*this));
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool SaveMission::RenameRefObject(const UnsignedInt type,
                                  const std::string &oldName,
                                  const std::string &newName)
{
   // There are no objects to be renamed here
   return true;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string SaveMission::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < SaveMissionParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer SaveMission::GetParameterID(const std::string &str) const
{
   for (int i=GmatCommandParamCount; i<SaveMissionParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType SaveMission::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < SaveMissionParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string SaveMission::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < SaveMissionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatCommand::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string SaveMission::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case FILE_NAME:
      return fileName;
   default:
      return GmatCommand::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string SaveMission::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
bool SaveMission::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case FILE_NAME:
      fileName = value;
      return true;
   default:
      return GmatCommand::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool SaveMission::SetStringParameter(const std::string &label, const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
//                    const std::string &prefix = "",
//                    const std::string &useName = "");
//------------------------------------------------------------------------------
const std::string& SaveMission::GetGeneratingString(Gmat::WriteMode mode,
                                                    const std::string &prefix,
                                                    const std::string &useName)
{
   // Build the local string
   generatingString = prefix + "SaveMission";
   generatingString += " '" + fileName + "';";
   
   // Then call the base class method for comments
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}

