//------------------------------------------------------------------------------
//                           BeginFileThrust
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
 * Toggle command used to tell GMAT to use a thrust data file
 */
//------------------------------------------------------------------------------


#include "BeginFileThrust.hpp"
#include "CommandException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BEGIN_MANEUVER
//#define DEBUG_BEGIN_MANEUVER_EXE
//#define DEBUG_INITIALIZATION


//------------------------------------------------------------------------------
// BeginFileThrust(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the command
 */
//------------------------------------------------------------------------------
BeginFileThrust::BeginFileThrust(const std::string &name) :
   GmatCommand             ("BeginFileThrust"),
   burnForce               (NULL),
   transientForces         (NULL),
   thfName                 (""),
   thrustFile              (NULL)
{
   objectTypeNames.push_back("BurnCommand");
   physicsBasedCommand = true;
}

//------------------------------------------------------------------------------
// ~BeginFileThrust()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
BeginFileThrust::~BeginFileThrust()
{
}

//------------------------------------------------------------------------------
// BeginFileThrust(const EndFileThrust& bft) :
//------------------------------------------------------------------------------
/**
 * Copy Constructor
 *
 * @param bft The command copied here
 */
//------------------------------------------------------------------------------
BeginFileThrust::BeginFileThrust(const BeginFileThrust& bft) :
   GmatCommand             (bft),
   burnForce               (NULL),
   transientForces         (bft.transientForces),
   thfName                 (bft.thfName),
   thrustFile              (NULL),
   satNames                (bft.satNames)
{
   sats.clear();
}

//------------------------------------------------------------------------------
// BeginFileThrust& operator=(const BeginFileThrust& bft)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param The command copied here
 *
 * @return
 */
//------------------------------------------------------------------------------
BeginFileThrust& BeginFileThrust::operator=(const BeginFileThrust& bft)
{
   if (this != &bft)
   {
      GmatCommand::operator =(bft);

      burnForce =NULL;
      transientForces = bft.transientForces;
      thfName = bft.thfName;
      thrustFile = NULL;
      satNames = bft.satNames;
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
GmatBase* BeginFileThrust::Clone() const
{
   return new BeginFileThrust(*this);
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
void BeginFileThrust::SetTransientForces(std::vector<PhysicalModel*>* tf)
{
   #ifdef DEBUG_TRANSIENT_FORCES
   MessageInterface::ShowMessage
      ("BeginFileThrust::SetTransientForces() tf=<%p>\n", tf);
   #endif
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
bool BeginFileThrust::TakeAction(const std::string& action,
      const std::string& actionData)
{
   #if DEBUG_TAKE_ACTION
      MessageInterface::ShowMessage
         ("BeginFiniteBurn::TakeAction() action=%s, actionData=%s\n",
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
std::string BeginFileThrust::GetRefObjectName(const UnsignedInt type) const
{
   switch (type)
   {
//      case Gmat::INTERFACE:
      case Gmat::FINITE_BURN:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("Getting BeginFiniteBurn reference burn names\n");
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
const ObjectTypeArray& BeginFileThrust::GetRefObjectTypeArray()
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
const StringArray& BeginFileThrust::GetRefObjectNameArray(
      const UnsignedInt type)
{

   refObjectNames.clear();

   if ((type == Gmat::UNKNOWN_OBJECT) ||
       (type == Gmat::SPACECRAFT))
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
bool BeginFileThrust::SetRefObjectName(const UnsignedInt type,
      const std::string& name)
{
   switch (type)
   {
      case Gmat::SPACECRAFT:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("Setting BeginFileThrust reference spacecraft \"%s\"\n",
                name.c_str());
         #endif
         if (find(satNames.begin(), satNames.end(), name) != satNames.end())
         {
            MessageInterface::ShowMessage("In the BeginFileThrust command %s, "
                  "the spacecraft %s is set more than once.  Only one instance "
                  "will be used.\n",
                  GetGeneratingString(Gmat::NO_COMMENTS).c_str(), name.c_str());
            return true;
         }
         satNames.push_back(name);
         if (satNames.size() > 1)
            throw CommandException("BeginFileThrust commands do not currently "
                  "support multiple Spacecraft; please toggle thrust history "
                  "file burns on one spacecraft at a time.");
         return true;

      // Note:  ThrustHistoryFile looks like FINITE_BURN to the Interpreter
      case Gmat::FINITE_BURN:
      case Gmat::INTERFACE:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("Setting BeginFileThrust reference burn \"%s\"\n",
                     name.c_str());
         #endif
         thfName = name;
         return true;

      default:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("BeginFileThrust reference object \"%s\" not set!\n",
                name.c_str());
         #endif
         break;
   }

   return GmatCommand::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// GmatBase* GetGmatObject(const UnsignedInt type,
//       const std::string objName)
//------------------------------------------------------------------------------
/**
 * Retrieves a GMAT object
 *
 * @param type The type of the requested object
 * @param objName The name of the object
 *
 * @return The object
 */
//------------------------------------------------------------------------------
GmatBase* BeginFileThrust::GetGmatObject(const UnsignedInt type,
      const std::string objName)
{
   if (type == Gmat::TRANSIENT_FORCE)
      return burnForce;
   return GmatCommand::GetGmatObject(type, objName);
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
bool BeginFileThrust::RenameRefObject(const UnsignedInt type,
      const std::string& oldName, const std::string& newName)
{
   // BeginFiniteBurn needs to know about Burn and Spacecraft only
   if (type != Gmat::FINITE_BURN && type != Gmat::SPACECRAFT)
      return true;

   if (thfName == oldName)
      thfName = newName;

   for (UnsignedInt i = 0; i < satNames.size(); ++i)
      if (satNames[i] == oldName)
         satNames[i] = newName;

   return true;
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
const std::string& BeginFileThrust::GetGeneratingString(Gmat::WriteMode mode,
      const std::string& prefix, const std::string& useName)
{
   generatingString = prefix + "BeginFileThrust " + thfName + "(";
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
// bool BeginFileThrust::Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares for inserting a thrust history file into the force model
 *
 * @return True is all of the membner structures are in place, false if not
 */
//------------------------------------------------------------------------------
bool BeginFileThrust::Initialize()
{
   bool retval = GmatCommand::Initialize();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("BeginFileThrust::Initialize() entered; "
            "GmatCommand %s\n", (retval ? "initialized" :
             "failed initialization"));
   #endif

   GmatBase *mapObj = NULL;
   if (retval)
   {
      // Look up the ThrustHistoryFile object
      if ((mapObj = FindObject(thfName)) == NULL)
         throw CommandException("Unknown ThrustHistoryFile \"" + thfName + "\"\n");
      if (mapObj->IsOfType("ThrustHistoryFile") == false)
         throw CommandException(thfName + " is not a ThrustHistoryFile\n");

      thrustFile = (ThrustHistoryFile*)mapObj;

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Initializing the thrust history file "
               "object\n");
      #endif

      if (thrustFile->Initialize())
      {
         GmatBase *burn = thrustFile->GetForce();
         if (burn != NULL)
         {
            burnForce = (FileThrust*)(burn->Clone());
            retval = true;
         }
         else
            throw CommandException("Unable to load the thrust history file "
                  "force");

         if (retval)
         {
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

               #ifdef DEBUG_BEGIN_MANEUVER
                  MessageInterface::ShowMessage(
                     "BeginFiniteBurn::Initialize() found %s with type %s\n",
                     scName->c_str(), sc->GetTypeName().c_str());
               #endif

               sats.push_back(sc);
            }

            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (burnForce, burnForce->GetName(), "BeginFiniteBurn::Initialize()",
                "deleting burn force");
            #endif

            if (transientForces != NULL)
            {
               std::vector<PhysicalModel *>::iterator transient = find(
                  transientForces->begin(), transientForces->end(), burnForce);
               if (transient != transientForces->end())
                  transientForces->erase(transient);
            }
         }

         UnsignedInt type = Gmat::SPACECRAFT;
         StringArray::iterator iter;

         // load up the spacecraft name list
         for (iter = satNames.begin(); iter != satNames.end(); ++iter)
         {
            #ifdef DEBUG_BEGIN_MANEUVER
               MessageInterface::ShowMessage(
                  "BeginFiniteBurn::Initialize() setting %s on %s\n",
                  iter->c_str(), thrustName.c_str());
            #endif
            burnForce->SetRefObjectName(type, *iter);
         }
      }
   }
   return retval;
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
bool BeginFileThrust::Execute()
{
   bool retval = true; // false;
   StringArray segsNotLoaded;

   // Check that all ThrustSegments were loaded from the file
   if (!thrustFile->AllDataSegmentsLoaded(segsNotLoaded)) {
      std::string msg = "Not all ThrustSegments have data loaded for ThrustHistoryFile '" +
         thrustFile->GetName() + "', missing segments are: ";
      for (int i = 0; i < segsNotLoaded.size() - 1; i++)
         msg += "'" + segsNotLoaded.at(i) + "', ";
      msg += "'" + segsNotLoaded.at(segsNotLoaded.size() - 1) + "'";
      throw ODEModelException(msg);
   }

   // Tell active spacecraft that they are now firing
   for (std::vector<Spacecraft*>::iterator s=sats.begin(); s!=sats.end(); ++s)
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("Setting maneuvering state on %s\n",
               (*s)->GetName().c_str());
      #endif
      (*s)->IsManeuvering(true);
   }

   if (transientForces == NULL)
      throw CommandException("Transient force list was NOT initialized; "
            "ABORTING RUN!!!\n\n");

   if (burnForce == NULL)
      throw CommandException("Thrust history file burn was NOT initialized; "
            "ABORTING RUN!!!\n\n");

   thrustFile->ActivateSegments();

   // Insert the force into the list of transient forces if not found
   bool fileForceConfigured = false;
   bool alreadyThere = false;
   if (transientForces->size() == 0)
   {
      #ifdef DEBUG_TRANSIENT_FORCES
      MessageInterface::ShowMessage
         ("BeginFiniteBurn::Execute() Adding first burnForce<%p>'%s' to "
               "transientForces\n", burnForce, burnForce->GetName().c_str());
      #endif
      transientForces->push_back(burnForce);
   }
   else
   {
      for (std::vector<PhysicalModel*>::iterator i = transientForces->begin();
            i !=transientForces->end(); ++i)
      {
         if ((*i)->IsOfType("FileThrust"))
         {
            FileThrust *transient = (FileThrust*)(*i);

            if (transient == burnForce)
            {
               alreadyThere = true;
               fileForceConfigured = true;
            }
            else
            if ((*transient) == (*burnForce))
            {
               MessageInterface::ShowMessage("Burn activated by the line\n   "
                     "%s\noverlaps with an active file burn.  No new file "
                     "burn will be applied.\n",
                     GetGeneratingString(Gmat::NO_COMMENTS).c_str());
               alreadyThere = true;
               fileForceConfigured = true;
            }
         }
      }
      if (alreadyThere == false)
      {
         #ifdef DEBUG_TRANSIENT_FORCES
         MessageInterface::ShowMessage("BeginFileBurn::Execute() Adding "
               "burnForce<%p>'%s' to transientForces\n", burnForce,
               burnForce->GetName().c_str());
         #endif
         transientForces->push_back(burnForce);
      }
   }

   if (fileForceConfigured == false)
   {
      // Set up the referenced coordinate systems
      StringArray csList = burnForce->GetRefObjectNameArray(Gmat::COORDINATE_SYSTEM);

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%d coordinate systems needed:\n",
               csList.size());
      #endif

      GmatBase *mapObj = NULL;
      for (UnsignedInt i = 0; i < csList.size(); ++i)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("   %s\n", csList[i].c_str());
         #endif

         if ((mapObj = FindObject(csList[i])) == NULL)
            throw CommandException("Unknown Coordinate System \"" +
                  csList[i] + "\"");

         if (mapObj->IsOfType(Gmat::COORDINATE_SYSTEM) == false)
            throw CommandException((csList[i]) + " is not a Coordinate System");

         burnForce->SetRefObject(mapObj, Gmat::COORDINATE_SYSTEM, csList[i]);
      }
   }

   // Set maneuvering to Publisher so that any subscriber can do its own action
   if (!sats.empty())
   {
      // could do: if (sats[0]->HasPrecisionTime()) { GmatTime epoch = sats[0]->GetEpochGT(); publisher->SetManeuvering(...); }
      // but publisher->SetManeuvering() only takes a Real epoch, it doesn't have a precision time overload
      Real epoch = sats[0]->GetEpoch();
      publisher->SetManeuvering(this, true, epoch, satNames,
         "begin of thrust history file maneuver");
   }

   #ifdef DEBUG_BEGIN_MANEUVER_EXE
      MessageInterface::ShowMessage
         ("There are %d transient force(s)\n", transientForces->size());
      MessageInterface::ShowMessage("Current TransientForces list:\n");
      for (std::vector<PhysicalModel*>::iterator j = transientForces->begin();
           j != transientForces->end(); ++j)
         MessageInterface::ShowMessage("   %s\n", (*j)->GetName().c_str());
   #endif

   BuildCommandSummary(true);
   return retval;
}
