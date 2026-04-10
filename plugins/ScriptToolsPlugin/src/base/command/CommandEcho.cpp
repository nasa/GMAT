//------------------------------------------------------------------------------
//                                  CommandEcho
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Joshua J. Raymond
// Created: June 23, 2017
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
* Class implementation for the CommandEcho command
*/
//------------------------------------------------------------------------------

#include "CommandEcho.hpp"
#include "FileManager.hpp"      // for GmatGlobal::Instance()
#include "MessageInterface.hpp"
#include "GmatGlobal.hpp"


//------------------------------------------------------------------------------
// CommandEcho()
//------------------------------------------------------------------------------
/**
* Default constructor
*/
//------------------------------------------------------------------------------
CommandEcho::CommandEcho() :
GmatCommand("CommandEcho")
{
   echoSetting = "Off";
   echoStatus = false;
}


//------------------------------------------------------------------------------
// CommandEcho(const CommandEcho& commandEcho)
//------------------------------------------------------------------------------
/**
* Copy constructor.
*
* @param commandEcho The instance that is copied.
*/
//------------------------------------------------------------------------------
CommandEcho::CommandEcho(const CommandEcho& commandEcho) :
GmatCommand(commandEcho),
echoStatus(commandEcho.echoStatus)
{}


//------------------------------------------------------------------------------
// CommandEcho& operator=(const CommandEcho& commandEcho)
//------------------------------------------------------------------------------
/**
* Assignmant operator.
*
* @param commandEcho The instance that is copied.
*
* @return this instance, with internal data set to match the input command.
*/
//------------------------------------------------------------------------------
CommandEcho& CommandEcho::operator=(const CommandEcho& commandEcho)
{
   if (this == &commandEcho)
	{
      echoStatus = commandEcho.echoStatus;
	}
      return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone ()
//------------------------------------------------------------------------------
/**
* This method returns a clone of CommandEcho
*
* @return clone of CommandEcho
*/
//------------------------------------------------------------------------------
GmatBase*   CommandEcho::Clone() const
{
   return new CommandEcho();
}


//------------------------------------------------------------------------------
// SettingInput(std::string)
//------------------------------------------------------------------------------
/**
* Determines whether the debug setting is turned on or off
*
* @param echoSetting String input from the script
* @param echoStatus Bool variable used to keep track of whether the debug
* setting is on (true) or off (false)
*/
//------------------------------------------------------------------------------
bool CommandEcho::SettingInput(std::string eSetting)
{
   echoSetting = eSetting;
   if (echoSetting == "On")
      echoStatus = true;
   else if (echoSetting == "Off")
      echoStatus = false;
   else
      throw CommandException("The value of \"" + echoSetting + "\" for field " 
      "\"echoSetting\" on command \"CommandEcho\" is not an allowed value. "  
      "The allowed values are: [On Off]");

   return echoStatus;
}


//------------------------------------------------------------------------------
// ~CommandEcho()
//------------------------------------------------------------------------------
/**
* Destructor.
*/
//------------------------------------------------------------------------------
CommandEcho::~CommandEcho()
{}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type,
//						const std::string &oldName,
//						const std::string &newName)
//------------------------------------------------------------------------------
/**
* This method updates object names when the user changes them
*
* @param type Type of object that is renamed.
* @param oldName Old name for the object.
* @param newName New name for the object.
*
* @return true on success.
*/
//------------------------------------------------------------------------------

bool         CommandEcho::RenameRefObject(const UnsignedInt type,
   const std::string &oldName,
   const std::string &newName)
{
   return true;
}


//------------------------------------------------------------------------------
// Execute()
//------------------------------------------------------------------------------
/**
* Executes the CommandEcho plugin and sets CommandEchoMode either on or off.
*/
//------------------------------------------------------------------------------


bool CommandEcho::Execute()
{
   GmatGlobal::Instance()->SetCommandEchoMode(echoStatus);
   return true;
}


//------------------------------------------------------------------------------
// InterpretAction()
//------------------------------------------------------------------------------
/**
* Method called to parse the string in the script
*/
//------------------------------------------------------------------------------

bool CommandEcho::InterpretAction()
{
   StringArray chunks = InterpretPreface();
   if (chunks.size() < 2)
      throw CommandException("Missing input for CommandEcho command. Allowed inputs are: [On Off]\n");
   echoSetting = chunks[1];
   if (echoSetting.length() > 3)
   {
      std::size_t newNameStart = echoSetting.find("'");
      std::size_t newNameEnd = echoSetting.find("'", newNameStart + 1);
      if ((newNameStart == std::string::npos) || (newNameEnd == std::string::npos))
         throw CommandException("The value of \"" + echoSetting + "\" for field "
         "\"echoSetting\" on command \"CommandEcho\" is not an allowed value. "
         "The allowed values are: [On Off]");
      echoSetting = echoSetting.erase(newNameStart, newNameEnd - newNameStart + 2);
   }

   SettingInput(echoSetting);
   return true;
}


//------------------------------------------------------------------------------
// Initialize()
//------------------------------------------------------------------------------
/**
* Stores the inital setting of EchoCommand
*
* @param initval Bool variable that stores inital setting of EchoCommands
*/
//------------------------------------------------------------------------------

bool CommandEcho::Initialize()
{
   initval = GmatGlobal::Instance()->EchoCommands();

   return true;
}


//------------------------------------------------------------------------------
// RunComplete()
//------------------------------------------------------------------------------
/**
* The echo command is reset to its default state at the end of the mission
* run.
*/
//------------------------------------------------------------------------------
void CommandEcho::RunComplete()
{
   GmatGlobal::Instance()->SetCommandEchoMode(initval);
}


//------------------------------------------------------------------------------
//    virtual const std::string&
//						   GetGeneratingString(Gmat::WriteMode mode,
//											   const std::string &prefix,
//											   const std::string &useName);
//------------------------------------------------------------------------------
/**
* Generates string from GUI mission sequence to script upon save.  This method
* also allows the user to input ON or OFF in the GUI pop-up rather than typing
* directly into the script.
*
* @param mode    Specifies the type of serialization requested.  (Not yet used
*                in commands)
* @param prefix  Optional prefix appended to the object's name.  (Not yet used
*                in commands)
* @param useName Name that replaces the object's name.  (Not yet used in
*                commands)
*
* @return The script line that, when interpreted, defines this Save command.
*/
//------------------------------------------------------------------------------
const std::string&
CommandEcho::GetGeneratingString(Gmat::WriteMode mode,
const std::string &prefix,
const std::string &useName)
{
   generatingString = prefix + "CommandEcho ";
   generatingString += echoSetting;

   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}