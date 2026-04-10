//$Id$
//------------------------------------------------------------------------------
//                            BeginFiniteBurn
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS task
// order 124.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2005/01/04
//
/**
 *  Implementation code for the BeginFiniteBurn command.
 */
//------------------------------------------------------------------------------

#include "BeginFiniteBurn.hpp"


//#define DEBUG_BEGIN_MANEUVER
//#define DEBUG_BEGIN_MANEUVER_EXE
//#define DEBUG_TRANSIENT_FORCES
//#define DEBUG_BFB_THRUSTER
//#define DEBUG_TRANSIENTFORCE_MANAGEMENT

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

#include "MessageInterface.hpp"
#include "CommandException.hpp"


//------------------------------------------------------------------------------
// BeginFiniteBurn()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
BeginFiniteBurn::BeginFiniteBurn() :
   GmatCommand        ("BeginFiniteBurn"),
   burnName           (""),
   maneuver           (NULL),
   burnForce          (NULL),
   transientForces    (NULL)
{
   objectTypeNames.push_back("BurnCommand");
   physicsBasedCommand = true;
}


//------------------------------------------------------------------------------
// ~BeginFiniteBurn()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BeginFiniteBurn::~BeginFiniteBurn()
{
   #ifdef DEBUG_TRANSIENT_FORCES
      MessageInterface::ShowMessage
         ("BeginFiniteBurn destructor entered, this=<%p>, burnForce=<%p>'%s'\n",
          this, burnForce, burnForce ? burnForce->GetName().c_str() : "NULL");
   #endif
   
   if (burnForce)
   {
      #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (burnForce, burnForce->GetName(), "BeginFiniteBurn::"
             "~BeginFiniteBurn() deleting burn force");
      #endif
      if (transientForces != NULL)
      {
         std::vector<PhysicalModel *>::iterator transient = find(
            transientForces->begin(), transientForces->end(), burnForce);
         if (transient != transientForces->end())
         {
            #ifdef DEBUG_TRANSIENTFORCE_MANAGEMENT
               MessageInterface::ShowMessage("Removing burn force <%p> from "
                  "the transient force list\n", burnForce);
            #endif
            transientForces->erase(transient);
         }
      }
      delete burnForce;
   }

   #ifdef DEBUG_TRANSIENT_FORCES
      MessageInterface::ShowMessage
         ("BeginFiniteBurn destructor finished\n");
   #endif
}


//------------------------------------------------------------------------------
// BeginFiniteBurn(const BeginFiniteBurn& begman)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param begman The command that gets copied.
 */
//------------------------------------------------------------------------------
BeginFiniteBurn::BeginFiniteBurn(const BeginFiniteBurn& begman) :
   GmatCommand        (begman),
   burnName           (begman.burnName),
   maneuver           (NULL),
   burnForce          (NULL),
   transientForces    (NULL),
   satNames           (begman.satNames)
{
   sats.clear();
   thrusters.clear();
}


//------------------------------------------------------------------------------
// BeginFiniteBurn& operator=(const BeginFiniteBurn& begman)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param begman The command that gets copied.
 * 
 * @return this instance, with internal data structures set to match the input
 *         instance.
 */
//------------------------------------------------------------------------------
BeginFiniteBurn& BeginFiniteBurn::operator=(const BeginFiniteBurn& begman)
{
   if (&begman != this)
   {
      GmatCommand::operator=(begman);
      burnName = begman.burnName;
      maneuver = NULL;
      if (burnForce != NULL)
      {
         if (transientForces != NULL)
         {
            std::vector<PhysicalModel *>::iterator transient = find(
               transientForces->begin(), transientForces->end(), burnForce);
            if (transient != transientForces->end())
            {
               #ifdef DEBUG_TRANSIENTFORCE_MANAGEMENT
                  MessageInterface::ShowMessage("Removing burn force <%p> from "
                     "the transient force list\n", burnForce);
               #endif
               transientForces->erase(transient);
            }
         }
         delete burnForce;
         burnForce = NULL;
      }
      transientForces = NULL;
      satNames = begman.satNames;

      sats.clear();
      thrusters.clear();
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
 * BeginFiniteBurn commands implement an action, "Clear", that clears the list
 * of maneuvering spacecraft.
 *
 * @param action action to perform
 * @param actionData action data associated with action
 *
 * @return true if action successfully performed
 */
//------------------------------------------------------------------------------
bool BeginFiniteBurn::TakeAction(const std::string &action,
                                 const std::string &actionData)
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
 * Accesses names for referenced objects.
 * 
 * @param type Type of object requested.
 * 
 * @return the referenced object's name.
 */
//------------------------------------------------------------------------------
std::string BeginFiniteBurn::GetRefObjectName(const UnsignedInt type) const
{
   switch (type)
   {
      case Gmat::FINITE_BURN:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("Getting BeginFiniteBurn reference burn names\n");
         #endif
         return burnName;
         
      default:
         break;
   }
   
   return GmatCommand::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the BeginFiniteBurn.
 *
 * @return the list of object types.
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& BeginFiniteBurn::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::FINITE_BURN);
   refObjectTypes.push_back(Gmat::SPACECRAFT);
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
const StringArray& BeginFiniteBurn::GetRefObjectNameArray(
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
       type == Gmat::FINITE_BURN)
   {
      refObjectNames.push_back(burnName);
   }

   return refObjectNames;
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
bool BeginFiniteBurn::SetRefObjectName(const UnsignedInt type,
                                       const std::string &name)
{
   switch (type)
   {
      case Gmat::SPACECRAFT:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("Setting BeginFiniteBurn reference spacecraft \"%s\"\n", 
                name.c_str());
         #endif
         if (find(satNames.begin(), satNames.end(), name) != satNames.end())
         {
            MessageInterface::ShowMessage("In the BeginFiniteBurn command %s, "
                  "the spacecraft %s is set more than once.  Only one instance "
                  "will be used.\n",
                  GetGeneratingString(Gmat::NO_COMMENTS).c_str(), name.c_str());
            return true;
         }
         satNames.push_back(name);
         if (satNames.size() > 1)
            throw CommandException("BeginFiniteBurn commands do not currently "
                  "support multiple Spacecraft; please toggle finite burns on "
                  "one spacecraft at a time.");
         return true;
      
      case Gmat::FINITE_BURN:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("Setting BeginFiniteBurn reference burn \"%s\"\n",
                     name.c_str());
         #endif
         burnName = name;
         return true;
         
      default:
         #ifdef DEBUG_BEGIN_MANEUVER
            MessageInterface::ShowMessage
               ("BeginFiniteBurn reference object \"%s\" not set!\n", 
                name.c_str());
         #endif
         break;
   }
   
   return GmatCommand::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// GmatBase* GetObject(const UnsignedInt type, const std::string objName)
//------------------------------------------------------------------------------
/**
 * Access an object used by this instance.
 * 
 * This command constructs a transient force that can be accessed using this 
 * method.
 * 
 * @param type Type of the object.
 * @param objName Name of the object.
 *
 * @return The object.
 */
//------------------------------------------------------------------------------
GmatBase* BeginFiniteBurn::GetGmatObject(const UnsignedInt type,
                                   const std::string objName)
{
   if (type == Gmat::TRANSIENT_FORCE)
      return burnForce;
   return GmatCommand::GetGmatObject(type, objName);
}


//------------------------------------------------------------------------------
//  void BeginFiniteBurn::SetTransientForces(std::vector<PhysicalModel*> *tf)
//------------------------------------------------------------------------------
/**
 * Sets the array of transient forces for the command.
 *
 * @param tf The transient force vector.
 */
//------------------------------------------------------------------------------
void BeginFiniteBurn::SetTransientForces(std::vector<PhysicalModel*> *tf)
{
   #ifdef DEBUG_TRANSIENT_FORCES
   MessageInterface::ShowMessage
      ("BeginFiniteBurn::SetTransientForces() tf=<%p>\n", tf);
   #endif
   transientForces = tf;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BeginFiniteBurn command.
 *
 * @return clone of the BeginFiniteBurn.
 */
//------------------------------------------------------------------------------
GmatBase* BeginFiniteBurn::Clone() const
{
   return new BeginFiniteBurn(*this);
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
bool BeginFiniteBurn::RenameRefObject(const UnsignedInt type,
                                      const std::string &oldName,
                                      const std::string &newName)
{
   // BeginFiniteBurn needs to know about Burn and Spacecraft only
   if (type != Gmat::FINITE_BURN && type != Gmat::SPACECRAFT)
      return true;
   
   if (burnName == oldName)
      burnName = newName;
   
   for (UnsignedInt i = 0; i < satNames.size(); ++i)
      if (satNames[i] == oldName)
         satNames[i] = newName;
   
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
const std::string& BeginFiniteBurn::GetGeneratingString(Gmat::WriteMode mode,
                                                  const std::string &prefix,
                                                  const std::string &useName)
{
   generatingString = prefix + "BeginFiniteBurn " + burnName + "(";
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
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the BeginFiniteBurn structures at the start of a run.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool BeginFiniteBurn::Initialize()
{
   bool retval = GmatCommand::Initialize();
   
   #ifdef DEBUG_BEGIN_MANEUVER
      MessageInterface::ShowMessage
         ("BeginFiniteBurn::Initialize() entered. burnName=%s\n", burnName.c_str());
   #endif
   
   GmatBase *mapObj = NULL;
   if (retval)
   {
      // Look up the maneuver object
      if ((mapObj = FindObject(burnName)) == NULL) 
         throw CommandException("Unknown finite burn \"" + burnName + "\"\n");
      if (mapObj->IsOfType("FiniteBurn") == false)
         throw CommandException(burnName + " is not a FiniteBurn\n");

      maneuver = (FiniteBurn*)mapObj;

      #ifdef DEBUG_BEGIN_MANEUVER
         MessageInterface::ShowMessage(
            "BeginFiniteBurn::Initialize() found %s with type %s\n", 
            maneuver->GetName().c_str(), maneuver->GetTypeName().c_str());
      #endif      

      // find all of the spacecraft
      StringArray::iterator scName;
      Spacecraft *sc;
      sats.clear();
      for (scName = satNames.begin(); scName != satNames.end(); ++scName)
      {
         if ((mapObj = FindObject(*scName)) == NULL) 
            throw CommandException("Unknown SpaceObject \"" + (*scName) + "\"");

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
      
      // Delete old burnForce
      if (burnForce != NULL)
      {
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
            {
               #ifdef DEBUG_TRANSIENTFORCE_MANAGEMENT
                  MessageInterface::ShowMessage("Removing burn force <%p> from "
                        "the transient force list\n", burnForce);
               #endif
               transientForces->erase(transient);
            }
         }
         delete burnForce;
         burnForce = NULL;
      }
      
      // If all is okay, create the FiniteThrust object and configure it.
      std::string thrustName = burnName + "_FiniteThrust";
      burnForce = new FiniteThrust(thrustName);
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (burnForce, thrustName, "BeginFiniteBurn::Initialize()",
          "burnForce = new FiniteThrust()");
      #endif
      
      burnForce->SetRefObject(maneuver, maneuver->GetType(),
                              maneuver->GetName());

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
   
   #ifdef DEBUG_BEGIN_MANEUVER
   MessageInterface::ShowMessage
      ("BeginFiniteBurn::Initialize() returning %d\n", isInitialized);
   #endif
   
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
bool BeginFiniteBurn::Execute()
{
   ValidateThrusters();
   
   // Turn on all of the referenced thrusters
   #ifdef DEBUG_BEGIN_MANEUVER_EXE
   MessageInterface::ShowMessage
      ("BeginFiniteBurn::Execute() <%p>'%s' entered\n   "
       "There are %d thruster(s) in use\n", this,
       GetGeneratingString(Gmat::NO_COMMENTS).c_str(), thrusters.size());
   #endif
   for (std::vector<Thruster*>::iterator i = thrusters.begin(); 
        i != thrusters.end(); ++i)
   {
      Thruster *th = *i;
      #ifdef DEBUG_BEGIN_MANEUVER_EXE
         MessageInterface::ShowMessage
            ("Activating engine <%p>'%s'\n", th, th->GetName().c_str());
      #endif
      th->SetBooleanParameter(th->GetParameterID("IsFiring"), true);
      
      #ifdef DEBUG_BEGIN_MANEUVER_EXE
         MessageInterface::ShowMessage
            ("Checking to see if engine is active: returned %s\n", 
             (th->GetBooleanParameter(th->GetParameterID("IsFiring")) ? 
              "true" : "false"));
      #endif
   }
   
   // Tell active spacecraft that they are now firing
   for (std::vector<Spacecraft*>::iterator s=sats.begin(); s!=sats.end(); ++s)
   {
      (*s)->IsManeuvering(true);
      // Set the spacecraft maneuvering here so parameters evaluate at maneuver start
      maneuver->SetSpacecraftToManeuver(*s);
      maneuver->TakeAction("SetData");
   }

   if (transientForces == NULL)
      throw CommandException("Transient force list was NOT initialized; "
            "ABORTING RUN!!!\n\n");
   
   // Insert the force into the list of transient forces if not found
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
      bool alreadyThere = false;
      for (std::vector<PhysicalModel*>::iterator i = transientForces->begin();
            i !=transientForces->end(); ++i)
      {
         if ((*i)->IsOfType("FiniteThrust"))
         {
            FiniteThrust *transient = (FiniteThrust*)(*i);
            if (transient == burnForce)
            {
               alreadyThere = true;
            }

            if ((*transient) == (*burnForce))
            {
               MessageInterface::ShowMessage("Burn activated by the line\n   "
                     "%s\noverlaps with an active finite burn.  No new finite "
                     "burn will be applied.\n",
                     GetGeneratingString(Gmat::NO_COMMENTS).c_str());
               alreadyThere = true;
            }
         }
      }
      if (alreadyThere == false)
      {
         #ifdef DEBUG_TRANSIENT_FORCES
         MessageInterface::ShowMessage("BeginFiniteBurn::Execute() Adding "
               "burnForce<%p>'%s' to transientForces\n", burnForce,
               burnForce->GetName().c_str());
         #endif
         transientForces->push_back(burnForce);
      }
   }
   
   // Set maneuvering to Publisher so that any subscriber can do its own action
   if (!sats.empty())
   {
      Real epoch = sats[0]->GetEpoch();
      publisher->SetManeuvering(this, true, epoch, satNames,
            "begin of finite maneuver");
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
   return true;
}


//------------------------------------------------------------------------------
// void ValidateThrusters()
//------------------------------------------------------------------------------
/**
 * Validate that the spacecrafts have the thrusters they need.
 */
//------------------------------------------------------------------------------
void BeginFiniteBurn::ValidateThrusters()
{
   thrusters.clear();
   for (std::vector<Spacecraft*>::iterator current = sats.begin(); 
        current != sats.end(); ++current)
   {
      #ifdef DEBUG_BFB_THRUSTER
      MessageInterface::ShowMessage
         ("BeginFiniteBurn::ValidateThrusters() entered, checking Spacecraft "
          "<%p>'%s' for Thrusters\n", *current, (*current)->GetName().c_str());
      #endif
      
      StringArray thrusterNames = (*current)->GetStringArrayParameter(
                                  (*current)->GetParameterID("Thrusters"));
      StringArray engines = (maneuver)->GetStringArrayParameter(
                            (maneuver)->GetParameterID("Thrusters"));
      
      #ifdef DEBUG_BFB_THRUSTER
      MessageInterface::ShowMessage
         ("   Spacecraft has %d Thrusters and FiniteBurn has %d thrusters\n",
          thrusterNames.size(), engines.size());
      #endif
      
      for (StringArray::iterator i = engines.begin(); i != engines.end(); ++i)
      {
         if (find(thrusterNames.begin(), thrusterNames.end(), *i) == 
             thrusterNames.end())
         {
            thrusters.clear();
            throw CommandException("BeginFiniteBurn: Spacecraft " +
                  (*current)->GetName() + " does not have a thruster named \"" +
                  (*i) + "\"");
         }
         
         Thruster* th = 
            (Thruster*)((*current)->GetRefObject(Gmat::THRUSTER, *i));
         
         if (th)
         {
            #ifdef DEBUG_BFB_THRUSTER
            MessageInterface::ShowMessage
               ("BeginFiniteBurn::ValidateThrusters() addding the Thruster "
                "<%p>'%s' to thrusters\n", th, th->GetName().c_str());
            #endif
            thrusters.push_back(th);
         }
         else
         {
            thrusters.clear();
            throw CommandException("Thruster object \"" + (*i) +
                                   "\" was not set on Spacecraft \"" 
                                   + (*current)->GetName() + "\"");
         }
      }
   }

   #ifdef DEBUG_BFB_THRUSTER
   MessageInterface::ShowMessage
      ("BeginFiniteBurn::ValidateThrusters() leaving\n");
   #endif
}
