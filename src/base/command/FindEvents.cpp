//$Id$
//------------------------------------------------------------------------------
//                            FindEvents
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
//
// Author: Wendy Shoan / GSFC/GSSB/583
// Created: 2015.03.11
//
/**
 *  Implementation code for the FindEvents command.
 */
//------------------------------------------------------------------------------

#include "FindEvents.hpp"


//#define DEBUG_FIND_EVENTS

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

#include "MessageInterface.hpp"
#include "CommandException.hpp"
#include "StringUtil.hpp"

//------------------------------------------------------------------------------
//  static data
//------------------------------------------------------------------------------
const std::string FindEvents::PARAMETER_TEXT[FindEventsParamCount -
                                             GmatCommandParamCount] =
{
   "EventLocator",
   "Append",
};

const Gmat::ParameterType FindEvents::PARAMETER_TYPE[FindEventsParamCount -
                                                     GmatCommandParamCount] =
{
   Gmat::OBJECT_TYPE,
   Gmat::BOOLEAN_TYPE,
};

//------------------------------------------------------------------------------
// FindEvents()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
FindEvents::FindEvents() :
   GmatCommand        ("FindEvents"),
   eventLocatorName   (""),
   eventLocator       (NULL),
   appendFlag         (false)
{
   objectTypeNames.push_back("FindEventsCommand");
   physicsBasedCommand = false;
}


//------------------------------------------------------------------------------
// ~FindEvents()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
FindEvents::~FindEvents()
{

}


//------------------------------------------------------------------------------
// FindEvents(const FindEvents& copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param copy The command that gets copied.
 */
//------------------------------------------------------------------------------
FindEvents::FindEvents(const FindEvents& copy) :
   GmatCommand        (copy),
   eventLocatorName   (copy.eventLocatorName),
   eventLocator       (copy.eventLocator),
   appendFlag         (copy.appendFlag)
{
}


//------------------------------------------------------------------------------
// FindEvents& operator=(const FindEvents& copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param copy The command that gets copied.
 *
 * @return this instance, with internal data structures set to match the input
 *         instance.
 */
//------------------------------------------------------------------------------
FindEvents& FindEvents::operator=(const FindEvents& copy)
{
   if (&copy != this)
   {
      GmatCommand::operator=(copy);
      eventLocatorName = copy.eventLocatorName;
      eventLocator     = copy.eventLocator;
      appendFlag       = copy.appendFlag;
   }

   return *this;
}


//------------------------------------------------------------------------------
// virtual bool TakeAction(const std::string &action,
//                         const std::string &actionData = "");
//------------------------------------------------------------------------------
/**
 * This method performs a custom action.
 *
 * FindEvents commands implement an action, "Clear", that clears the list
 * of maneuvering spacecraft.
 *
 * @param action action to perform
 * @param actionData action data associated with action
 *
 * @return true if action successfully performed
 */
//------------------------------------------------------------------------------
bool FindEvents::TakeAction(const std::string &action,
                                 const std::string &actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("FindEvents::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif

// not sure what may need to be done here
   return true;
}

//---------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//---------------------------------------------------------------------------
std::string FindEvents::GetParameterText(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < FindEventsParamCount))
      return PARAMETER_TEXT[id - GmatCommandParamCount];

   return GmatCommand::GetParameterText(id);
}


//---------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//---------------------------------------------------------------------------
Integer FindEvents::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < FindEventsParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }

   return GmatCommand::GetParameterID(str);
}


//---------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
Gmat::ParameterType FindEvents::GetParameterType(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < FindEventsParamCount))
      return PARAMETER_TYPE[id - GmatCommandParamCount];

    return GmatCommand::GetParameterType(id);
}


//---------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//---------------------------------------------------------------------------
std::string FindEvents::GetParameterTypeString(const Integer id) const
{
   return GmatCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string FindEvents::GetStringParameter(const Integer id) const
{
   if (id == EVENT_LOCATOR)
      return eventLocatorName;

   return GmatCommand::GetStringParameter(id);
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string FindEvents::GetStringParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetStringParameter(id);
}


//---------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool FindEvents::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == EVENT_LOCATOR)
   {
      eventLocatorName = value;
      return true;
   }

   return GmatCommand::SetStringParameter(id, value);
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool FindEvents::SetStringParameter(const std::string &label, const std::string &value)
{
   Integer id = GetParameterID(label);
   return SetStringParameter(id, value);
}

bool FindEvents::GetBooleanParameter(const Integer id) const
{
   if (id == APPEND_FLAG)
      return appendFlag;

   return GmatCommand::GetBooleanParameter(id);
}

bool FindEvents::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

bool FindEvents::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == APPEND_FLAG)
   {
      appendFlag = value;
      return appendFlag;
   }

   return GmatCommand::SetBooleanParameter(id, value);
}

bool FindEvents::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Accesses names for referenced objects.
 *
 * @param type Type of object requested.
 *
 * @return the referenced object's name.
 */
//------------------------------------------------------------------------------
std::string FindEvents::GetRefObjectName(const UnsignedInt type) const
{
   switch (type)
   {
      case Gmat::EVENT_LOCATOR:
         return eventLocatorName;

      default:
         break;
   }

   return GmatCommand::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the FindEvents.
 *
 * @return the list of object types.
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& FindEvents::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::EVENT_LOCATOR);
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
const StringArray& FindEvents::GetRefObjectNameArray(
      const UnsignedInt type)
{

   refObjectNames.clear();

   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::EVENT_LOCATOR)
   {
      refObjectNames.push_back(eventLocatorName);
   }

   return refObjectNames;
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets referenced objects.
 *
 * @param obj reference object pointer.
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool FindEvents::SetRefObject(GmatBase *obj, const UnsignedInt type,
                              const std::string &name)
{
   if (type == Gmat::EVENT_LOCATOR)
   {
      if (eventLocatorName == obj->GetName())
      {
         eventLocator = (EventLocator*)obj;
         return true;
      }
      return false;
   }
   return GmatCommand::SetRefObject(obj, type, name);
}



//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets names for referenced objects.
 *
 * @param type Type of the object.
 * @param name Name of the object.
 *
 * @return true if the name was set, false if not.
 */
//------------------------------------------------------------------------------
bool FindEvents::SetRefObjectName(const UnsignedInt type,
                                  const std::string &name)
{
   switch (type)
   {
      case Gmat::EVENT_LOCATOR:
         eventLocatorName = name;
         return true;

      default:
         break;
   }

   return GmatCommand::SetRefObjectName(type, name);
}



//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the FindEvents command.
 *
 * @return clone of the FindEvents.
 */
//------------------------------------------------------------------------------
GmatBase* FindEvents::Clone() const
{
   return new FindEvents(*this);
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool FindEvents::RenameRefObject(const UnsignedInt type,
                                      const std::string &oldName,
                                      const std::string &newName)
{
   // FindEvents needs to know about Burn and Spacecraft only
   if (type != Gmat::EVENT_LOCATOR)
      return true;

   if (eventLocatorName == oldName)
      eventLocatorName = newName;

   return true;
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
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
 * @param prefix  Optional prefix appended to the object's name. (Used for
 *                indentation)
 * @param useName Name that replaces the object's name (Not yet used
 *                in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& FindEvents::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   generatingString = prefix + "FindEvents " + eventLocatorName + " {Append = ";
   if (appendFlag)
      generatingString += "true";
   else
      generatingString += "false";
   generatingString += "};";

   // return generatingString;   // don't I want this here?

   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the FindEvents structures at the start of a run.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool FindEvents::Initialize()
{
   bool retval = GmatCommand::Initialize();

   GmatBase *mapObj = NULL;
   if (retval)
   {
      // Look up the event locator object
      if ((mapObj = FindObject(eventLocatorName)) == NULL)
         throw CommandException("Unknown event locator \"" + eventLocatorName + "\"\n");
      if (mapObj->IsOfType("EventLocator") == false)
         throw CommandException(eventLocatorName + " is not a EventLocator\n");
      eventLocator = (EventLocator*) mapObj;
   }

   isInitialized = true;

   return isInitialized;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * The method that is fired to turn on thrusters.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool FindEvents::Execute()
{
   #ifdef DEBUG_FIND_EVENTS
      MessageInterface::ShowMessage("FindEvents::Execute:: Setting append flag to %s\n",
            (appendFlag? "true" : "false"));
   #endif
   eventLocator->SetAppend(appendFlag);
   eventLocator->LocateEvents("");

//   BuildCommandSummary(true);  // do I need this?
   return true;
}

//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The FindEvents command has the following syntax:
 *
 *     FindEvents evLoc(true/false);
 *
 * where evLoc is an EventLocator, and the append flag is set (boolean).
 *
 * This method breaks the script line into the corresponding pieces, and stores
 * the name of the Event Locator so it can be set to point to the correct object
 * during initialization.  It also sets the append flag on the Event Locator.
 *
 * @return true on successful parsing of the command.
 */
//------------------------------------------------------------------------------
bool FindEvents::InterpretAction()
{
   StringArray blocks = parser.DecomposeBlock(generatingString);

   StringArray chunks = parser.SeparateBrackets(blocks[0], "{}", " ", false);

   #ifdef DEBUG_FIND_EVENTS
      MessageInterface::ShowMessage("Chunks from \"%s\":\n",
            blocks[0].c_str());
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
   #endif

   if (chunks.size() < 2)
      throw CommandException(typeName + "::InterpretAction() cannot identify "
            "the Event Locator -- is it missing? -- in line\n" + generatingString);

   if (chunks.size() > 3)
      throw CommandException(typeName +
            "::InterpretAction() found too many components to parse in the "
            "line\n" + generatingString);

   if (chunks[0] != typeName)
      throw CommandException(typeName + "::InterpretAction() does not identify "
            "the correct Solver type in line\n" + generatingString);

   eventLocatorName = chunks[1];

   if (chunks.size() == 3)
      CheckForOptions(chunks[2]);

   #ifdef DEBUG_FIND_EVENTS
      MessageInterface::ShowMessage("%s::InterpretAction for \"%s\", type = %s\n",
            typeName.c_str(), generatingString.c_str(), typeName.c_str());
      MessageInterface::ShowMessage("   Event Locator name:     \"%s\"\n",
            eventLocatorName.c_str());
   #endif

   return true;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void CheckForOptions(std::string &opts)
//------------------------------------------------------------------------------
void FindEvents::CheckForOptions(std::string &opts)
{
   StringArray chunks = parser.SeparateBrackets(opts, "{}", ", ", true);

   #ifdef DEBUG_FIND_EVENTS
      MessageInterface::ShowMessage("Chunks from \"%s\":\n", opts.c_str());
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
   #endif

   for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
   {
      StringArray option = parser.SeparateBy(*i, "= ");

      #ifdef DEBUG_FIND_EVENTS
         MessageInterface::ShowMessage("Options from \"%s\":\n", i->c_str());
         for (StringArray::iterator i = option.begin(); i != option.end(); ++i)
            MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
      #endif

      if (option.size() != 2)
         throw CommandException(typeName + "::InterpretAction() Event Locator option "
               "is not in the form option = value in line\n" +
               generatingString);

      std::string opt0 = GmatStringUtil::ToUpper(option[0]);
      std::string opt1 = GmatStringUtil::ToUpper(option[1]);
      if (opt0 == "APPEND")
      {
         if (opt1 == "TRUE")
            appendFlag = true;
         else if (opt1 == "FALSE")
            appendFlag = false;
         else
            throw CommandException(typeName + "::InterpretAction() Event Locator "
                  "Append option " + option[1] +
                  " is not a recognized value on line\n" + generatingString +
                  "\nAllowed values are \"true\" and \"false\"\n");
      }
      else
      {
         throw CommandException(typeName +
               "::InterpretAction() Event Locator option " + option[0] +
               " is not a recognized option on line\n" + generatingString +
                                 "\nAllowed options are \"true\" and "
               "\"false\"\n");
      }
   }
   #ifdef DEBUG_FIND_EVENTS
      MessageInterface::ShowMessage("FindEvents::CheckForOptions:: Append flag is %s\n",
            (appendFlag? "true" : "false"));
   #endif
}

