//$Id$
//------------------------------------------------------------------------------
//                                PlotCommand
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
// Author: Linda Jun
// Created: 2011/09/08
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Class implementation for the PlotCommand
 */
//------------------------------------------------------------------------------


#include "PlotCommand.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"

//#define DEBUG_PLOT_IA
//#define DEBUG_PLOT_INIT
//#define DEBUG_TAKE_ACTION

//---------------------------------
// static data
//---------------------------------
const std::string
PlotCommand::PARAMETER_TEXT[PlotCommandParamCount - GmatCommandParamCount] =
{
   "Subscriber",
};

const Gmat::ParameterType
PlotCommand::PARAMETER_TYPE[PlotCommandParamCount - GmatCommandParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,   // "Subscriber",
};


//------------------------------------------------------------------------------
// PlotCommand(const std::string &plotTypeName)
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
PlotCommand::PlotCommand(const std::string &plotTypeName) :
   GmatCommand(plotTypeName)
{
   objectTypeNames.push_back("SubscriberCommand");
   objectTypeNames.push_back("PlotCommand");
}


//------------------------------------------------------------------------------
// ~PlotCommand()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PlotCommand::~PlotCommand()
{
   plotNameList.clear();
   thePlotList.clear();
}


//------------------------------------------------------------------------------
// PlotCommand(const PlotCommand &c)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <c> The instance that gets copied.
 */
//------------------------------------------------------------------------------
PlotCommand::PlotCommand(const PlotCommand &c) :
   GmatCommand    (c),
   plotNameList   (c.plotNameList),
   thePlotList    (c.thePlotList)
{
   thePlotList.clear();
}

//------------------------------------------------------------------------------
// PlotCommand& operator=(const PlotCommand &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <c> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
PlotCommand& PlotCommand::operator=(const PlotCommand &c)
{
   if (&c != this)
   {
      GmatCommand::operator=(c);
      plotNameList = c.plotNameList;
      thePlotList.clear();
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,  
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs action.
 *
 * @param <action> action to perform
 * @param <actionData> action data associated with action
 *
 * @return true if action successfully performed
 */
//------------------------------------------------------------------------------
bool PlotCommand::TakeAction(const std::string &action, const std::string &actionData)
{
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("PlotCommand::TakeAction() entered, action=%s, actionData=%s, plotNameList.size=%d, "
       "thePlotList.size=%d\n", action.c_str(), actionData.c_str(), plotNameList.size(),
       thePlotList.size());
   #endif
   
   if (action == "Clear")
   {
      plotNameList.clear();
      thePlotList.clear();
   }
   
   #ifdef DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("PlotCommand::TakeAction() returning GmatCommand::TakeAction()\n");
   #endif
   
   return GmatCommand::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Achieve.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& PlotCommand::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SUBSCRIBER);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Accesses arrays of names for referenced objects.
 * 
 * @param type Type of object requested.
 * 
 * @return the StringArray containing the referenced object names.
 */
//------------------------------------------------------------------------------
const StringArray& PlotCommand::GetRefObjectNameArray(const UnsignedInt type)
{
   // There are only subscribers, so ignore object type
   return plotNameList;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool PlotCommand::RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("PlotCommand::RenameConfiguredItem() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::SUBSCRIBER)
      return true;
   
   for (unsigned int i=0; i<plotNameList.size(); i++)
   {
      if (plotNameList[i] == oldName)
         plotNameList[i] = newName;
   }

   return true;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string PlotCommand::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < PlotCommandParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   else
      return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer PlotCommand::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < PlotCommandParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType PlotCommand::GetParameterType(const Integer id) const
{
    if (id >= GmatCommandParamCount && id < PlotCommandParamCount)
        return PARAMETER_TYPE[id - GmatCommandParamCount];
    else
       return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string PlotCommand::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < PlotCommandParamCount)
      return PlotCommand::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatCommand::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Adds value to the list if value not found.
 */
//------------------------------------------------------------------------------
bool PlotCommand::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_PLOT_SET
   MessageInterface::ShowMessage
      ("PlotCommand::SetStringParameter() id=%d, value=%s\n", id, value.c_str());
   #endif
   
   if (value == "")
      return false;
   
   if (id == SUBSCRIBER)
   {
      if (find(plotNameList.begin(), plotNameList.end(), value) == plotNameList.end())
         plotNameList.push_back(value);
      
      return true;
   }
   
   return GmatCommand::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
std::string PlotCommand::GetStringParameter(const Integer id,
                                            const Integer index) const
{
   if (id == SUBSCRIBER)
   {
      if (index < 0 || index >= (Integer) plotNameList.size())
         throw CommandException(
                  "Index out-of-range for subscriber names list for PlotCommand command.\n");
      return plotNameList.at(index); 
   }
   return GmatCommand::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
bool PlotCommand::SetStringParameter(const Integer id, 
                                     const std::string &value,
                                     const Integer index)
{
   if (value == "")
      return false;
   
   if (id == SUBSCRIBER)
   {
      if (index < 0 || index > (Integer) plotNameList.size())
         throw CommandException(
                  "Index out-of-range for subscriber names list for PlotCommand command.\n");
      else
      {
         if (index == (Integer) plotNameList.size())  plotNameList.push_back(value);
         else                   plotNameList.at(index) = value;
         
         //@todo Do we need this code? Commented out for now (LOJ: 2009.06.01)
         //if (publisher == NULL)
         //   publisher = Publisher::Instance();         
         //streamID = publisher->RegisterPublishedData(this, plotNameList, plotNameList);
      }
      
      return true;
   }
   return GmatCommand::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode, ...)
//------------------------------------------------------------------------------
const std::string& PlotCommand::GetGeneratingString(Gmat::WriteMode mode,
                                                    const std::string &prefix,
                                                    const std::string &useName)
{
   #ifdef DEBUG_GEN_STR
   MessageInterface::ShowMessage("PlotCommand::GetGeneratingString() entered\n");
   #endif
   
   generatingString = prefix + typeName + " ";
   int plotCount = plotNameList.size();
   for (int i = 0; i < plotCount; i++)
   {
      if (i == plotCount-1)
         generatingString += plotNameList[i] + ";";
      else
         generatingString += plotNameList[i] + " ";
   }
   
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
bool PlotCommand::InterpretAction()
{
   #ifdef DEBUG_PLOT_IA
   MessageInterface::ShowMessage("PlotCommand::InterpretAction() entered\n");
   #endif
   
   plotNameList.clear();
   thePlotList.clear();
   
   std::string genStr = generatingString;
   
   // Trim it first
   genStr = GmatStringUtil::Trim(genStr, GmatStringUtil::BOTH, true, true);
   #ifdef DEBUG_PLOT_IA
   MessageInterface::ShowMessage("   genStr = '%s'\n", genStr.c_str());
   #endif
   
   Integer loc = genStr.find(typeName, 0) + typeName.size();
   const char *str = genStr.c_str();
   while (str[loc] == ' ')
      ++loc;

   // this command, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(str, false))
   {
      std::string msg = 
         "The PlotCommand command is not allowed to contain brackets, braces, or "
         "parentheses";
      throw CommandException(msg);
   }

   // Find the Subscriber list
   std::string sub = genStr.substr(loc, genStr.size()-loc);
   StringArray parts = GmatStringUtil::SeparateBy(sub," ", false);
   Integer partsSz = (Integer) parts.size();
   #ifdef DEBUG_PLOT_IA
      MessageInterface::ShowMessage("In PlotCommand::InterpretAction, parts = \n");
      for (Integer jj = 0; jj < partsSz; jj++)
         MessageInterface::ShowMessage("   %s\n", parts.at(jj).c_str());
   #endif
   if (partsSz < 1) // 'PlotCommand' already found
      throw CommandException("Missing field in PlotCommand command");
   for (Integer ii = 0; ii < partsSz; ii++)
      plotNameList.push_back(parts.at(ii));
   
   #ifdef DEBUG_PLOT_IA
      MessageInterface::ShowMessage("Plots to be %sed:\n", typeName.c_str());
      for (unsigned int ii = 0; ii < plotNameList.size(); ii++)
         MessageInterface::ShowMessage("   %s\n", (plotNameList.at(ii)).c_str());
   #endif
      
   #ifdef DEBUG_PLOT_IA
   MessageInterface::ShowMessage("PlotCommand::InterpretAction() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Method that initializes the internal data structures.
 * 
 * @return true if initialization succeeds.
 */
//------------------------------------------------------------------------------
bool PlotCommand::Initialize()
{
   #ifdef DEBUG_PLOT_INIT
   MessageInterface::ShowMessage
      ("PlotCommand::Initialize() entered for %s\n", typeName.c_str());
   #endif
   
   GmatCommand::Initialize();
   
   if (plotNameList.size() == 0)
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** %s command nas no associated plots; command has no effect.\n",
          typeName.c_str());
      return false;      
   }
   
   #ifdef DEBUG_PLOT_INIT
   MessageInterface::ShowMessage("PlotCommand::Initialize() returning true\n");
   #endif
   return true;
}
