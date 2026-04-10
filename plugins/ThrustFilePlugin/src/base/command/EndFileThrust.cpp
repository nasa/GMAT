//------------------------------------------------------------------------------
//                           EndFileThrust
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * Toggle command used to tell GMAT to stop using a thrust data file
 */
//------------------------------------------------------------------------------


#include "EndFileThrust.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// EndFileThrust(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the command
 */
//------------------------------------------------------------------------------
EndFileThrust::EndFileThrust(const std::string &name) :
   GmatCommand             ("EndFileThrust"),
   transientForces         (NULL),
   thfName                 (""),
   burnName                (""),
   thrustFile              (NULL)
{
   objectTypeNames.push_back("BurnCommand");
   physicsBasedCommand = true;
}

//------------------------------------------------------------------------------
// ~EndFileThrust()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EndFileThrust::~EndFileThrust()
{
}

//------------------------------------------------------------------------------
// EndFileThrust(const EndFileThrust& eft) :
//------------------------------------------------------------------------------
/**
 * Copy Constructor
 *
 * @param eft The command copied here
 */
//------------------------------------------------------------------------------
EndFileThrust::EndFileThrust(const EndFileThrust& eft) :
   GmatCommand             (eft),
   transientForces         (eft.transientForces),
   thfName                 (eft.thfName),
   burnName                (eft.burnName),
   thrustFile              (NULL),
   satNames                (eft.satNames)
{
}

//------------------------------------------------------------------------------
// EndFileThrust& operator=(const EndFileThrust& eft)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param The command copied here
 *
 * @return
 */
//------------------------------------------------------------------------------
EndFileThrust& EndFileThrust::operator =(const EndFileThrust& eft)
{
   if (this != &eft)
   {
      GmatCommand::operator=(eft);

      transientForces = eft.transientForces;
      thfName = eft.thfName;
      burnName = eft.burnName;
      thrustFile = NULL;
      satNames = eft.satNames;
      sats.clear();
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this command
 *
 * @return The copy
 */
//------------------------------------------------------------------------------
GmatBase* EndFileThrust::Clone() const
{
   return new EndFileThrust(*this);
}

//------------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type, const std::string& oldName,
//       const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Manages changes to names for reference objects
 *
 * @param type The type of the object changing names
 * @param oldname The old name of the object
 * @param The new object name
 *
 * @return true if a name change happened
 */
//------------------------------------------------------------------------------
bool EndFileThrust::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = false;

   // EndFiniteBurn needs to know about Burn and Spacecraft only
   if ((type != Gmat::SPACECRAFT) && (type != Gmat::FINITE_BURN))
      return true;

   if (thfName == oldName)
   {
      thfName = newName;
      retval = true;
   }

   for (UnsignedInt i=0; i<satNames.size(); i++)
      if (satNames[i] == oldName)
      {
         satNames[i] = newName;
         retval = true;
      }

   return retval;
}


//------------------------------------------------------------------------------
// void SetTransientForces(std::vector<PhysicalModel*>* tf)
//------------------------------------------------------------------------------
/**
 * Sets the list of transient forces
 *
 * @param tf The list
 */
//------------------------------------------------------------------------------
void EndFileThrust::SetTransientForces(std::vector<PhysicalModel*>* tf)
{
   transientForces = tf;
}

//------------------------------------------------------------------------------
// bool TakeAction(const std::string& action, const std::string& actionData)
//------------------------------------------------------------------------------
/**
 * Performs custom actions in teh command
 *
 * @param action The action to be taken
 * @param actionData Information needed to implement the action
 *
 * @return true if an action was taken
 */
//------------------------------------------------------------------------------
bool EndFileThrust::TakeAction(const std::string& action,
      const std::string& actionData)
{
   #if DEBUG_TAKE_ACTION
   MessageInterface::ShowMessage
      ("EndFiniteBurn::TakeAction() action=%s, actionData=%s\n",
       action.c_str(), actionData.c_str());
   #endif

   if (action == "Clear")
   {
      satNames.clear();
      return true;
   }

   return false;
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a reference object
 *
 * @param type The type of the object
 *
 * @return The name
 */
//------------------------------------------------------------------------------
std::string EndFileThrust::GetRefObjectName(const UnsignedInt type) const
{
   switch (type)
   {
//      case Gmat::INTERFACE:
      case Gmat::FINITE_BURN:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("Getting EndFileThrust history file name\n");
         #endif
         return thfName;

      default:
         break;
   }

   return GmatCommand::GetRefObjectName(type);
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * retrieves a list of reference object types
 *
 * @return The types of reference object used
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& EndFileThrust::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::INTERFACE);
   refObjectTypes.push_back(Gmat::SPACECRAFT);

   return refObjectTypes;
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of the names of reference objects used.
 *
 * @param type The type of reference object names wanted
 *
 * @return The list of reference objects
 */
//------------------------------------------------------------------------------
const StringArray& EndFileThrust::GetRefObjectNameArray(
      const UnsignedInt type)
{
   refObjectNames.clear();

   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::SPACECRAFT)
   {
      refObjectNames.insert(refObjectNames.end(), satNames.begin(),
            satNames.end());
   }

   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::INTERFACE)
   {
      refObjectNames.push_back(thfName);
   }

   return refObjectNames;
}

//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets the name for a reference object
 *
 * @param type The type of the reference object
 * @param name The reference object name
 *
 * @return true if the reference was set
 */
//------------------------------------------------------------------------------
bool EndFileThrust::SetRefObjectName(const UnsignedInt type,
      const std::string& name)
{
   switch (type)
   {
      case Gmat::SPACECRAFT:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("Setting EndFileThrust reference spacecraft \"%s\"\n",
                name.c_str());
         #endif
         if (find(satNames.begin(), satNames.end(), name) != satNames.end())
         {
            MessageInterface::ShowMessage("In the EndFileThrust command %s, "
                  "the spacecraft %s is set more than once.  Only one instance "
                  "will be used.\n",
                  GetGeneratingString(Gmat::NO_COMMENTS).c_str(), name.c_str());
            return true;
         }
         satNames.push_back(name);
         if (satNames.size() > 1)
            throw CommandException("EndFileThrust commands do not currently "
                  "support multiple Spacecraft; please toggle finite burns off "
                  "one spacecraft at a time.");
         return true;

         // Note:  ThrustHistoryFile looks like FINITE_BURN to the Interpreter
      case Gmat::FINITE_BURN:
      case Gmat::INTERFACE:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("Setting EndFileThrust reference burn \"%s\"\n", name.c_str());
         #endif
            thfName = name;
         return true;

      default:
         #ifdef DEBUG_END_MANEUVER
            MessageInterface::ShowMessage
               ("EndFileThrust reference object \"%s\" not set!\n",
                name.c_str());
         #endif
         break;
   }

   return GmatCommand::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//       const std::string& prefix, const std::string& useName)
//------------------------------------------------------------------------------
/**
 * Retrieves the sctring used to script this command
 *
 * @param mode The mode used to generate the string
 * @param prefix A prefix for the string
 * @param useName The name to use for the object type
 *
 * @return The string
 */
//------------------------------------------------------------------------------
const std::string& EndFileThrust::GetGeneratingString(Gmat::WriteMode mode,
      const std::string& prefix, const std::string& useName)
{
   generatingString = prefix + "EndFileThrust " + thfName + "(";
   for (StringArray::iterator satIt = satNames.begin();
        satIt != satNames.end(); ++satIt)
   {
      if (satIt != satNames.begin())
         generatingString += ", ";
      generatingString += *satIt;
   }

   generatingString += ");";

   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the EndFileThrust structures at the start of a run.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool EndFileThrust::Initialize()
{
   bool retval = GmatCommand::Initialize();

   GmatBase *mapObj = NULL;

   if (retval)
   {
      // Look up the ThrustHistoryFile object
      if ((mapObj = FindObject(thfName)) == NULL)
         throw CommandException("Unknown ThrustHistoryFile \"" + thfName + "\"\n");
      if (mapObj->IsOfType("ThrustHistoryFile") == false)
         throw CommandException(thfName + " is not a ThrustHistoryFile\n");

      thrustFile = (ThrustHistoryFile*)mapObj;

      burnName = thrustFile->GetForce()->GetName();

      // find all of the spacecraft
      StringArray::iterator scName;
      Spacecraft *sc;
      sats.clear();
      for (scName = satNames.begin(); scName != satNames.end(); ++scName)
      {
         if ((mapObj = FindObject(*scName)) == NULL)
            throw CommandException("Unknown SpaceObject \"" +
                  (*scName) + "\"");

         if (mapObj->IsOfType(Gmat::SPACECRAFT) == false)
            throw CommandException((*scName) + " is not a Spacecraft");
         sc = (Spacecraft*)mapObj;

         sats.push_back(sc);
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * The method that is fired to turn off thrusters.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool EndFileThrust::Execute()
{
   bool retval = true; // false;

   // Only do this if the FileThrust is the one this command controls...
   bool forceActive = false;
   for (std::vector<PhysicalModel*>::iterator j = transientForces->begin();
        j != transientForces->end(); ++j)
   {
      if (((*j)->GetName()) == burnName)
      {
         // ... and if it is set for the right spacecraft
         StringArray burnSatNames = (*j)->GetRefObjectNameArray(Gmat::SPACECRAFT);

         bool foundSats = false;
         UnsignedInt numberFound = 0;
         for (UnsignedInt i = 0; i < satNames.size(); ++i)
         {
            if (find(burnSatNames.begin(), burnSatNames.end(), satNames[i]) != burnSatNames.end())
            {
               foundSats = true;
               ++numberFound;
            }
         }
         if (foundSats)
         {
            #ifdef DEBUG_TRANSIENT_FORCES
               MessageInterface::ShowMessage("EndFiniteBurn::Execute(): The burn "
                     "is active\n");
            #endif

            forceActive = true;

            if (numberFound != satNames.size())
               MessageInterface::ShowMessage("*** WARNING *** Turning off the "
                     "file thrust %s, but the EndFileThrust command did not "
                     "list all of the spacecraft that are no longer "
                     "maneuvering.\n", burnName.c_str());

            break;
         }
      }
   }

   if (forceActive)
   {
      // Tell active spacecraft that they are not firing
      for (std::vector<Spacecraft*>::iterator s=sats.begin(); s!=sats.end(); ++s)
      {
         (*s)->IsManeuvering(false);
      }

      thrustFile->DeactivateSegments();

      // Remove the force from the list of transient forces
      for (std::vector<PhysicalModel*>::iterator j = transientForces->begin();
           j != transientForces->end(); ++j)
      {
         if (((*j)->GetName()) == burnName)
         {
            #ifdef DEBUG_TRANSIENT_FORCES
            MessageInterface::ShowMessage
               ("EndFiniteBurn::Execute() Removing burnForce<%p>'%s' from "
                     "transientForces\n", *j, (*j)->GetName().c_str());
            #endif
            transientForces->erase(j);
            break;
         }
      }
      // Reset maneuvering to Publisher so that any subscriber can do its own action
      if (!sats.empty())
      {
         // could do: if (sats[0]->HasPrecisionTime()) { GmatTime epoch = sats[0]->GetEpochGT(); publisher->SetManeuvering(...); }
         // but publisher->SetManeuvering() only takes a Real epoch, it doesn't have a precision time overload
         Real epoch = sats[0]->GetEpoch();
         publisher->SetManeuvering(this, false, epoch, satNames, "end of file "
               "based maneuver");
      }
   }

   BuildCommandSummary(true);
   return retval;
}
