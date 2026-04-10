//$Id$
//------------------------------------------------------------------------------
//                           EventModel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Sep 7, 2011
//
/**
 * Implementation of the physical model used to integrate event functions
 */
//------------------------------------------------------------------------------


#include "EventModel.hpp"
#include "EventLocator.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_EXE
//#define DEBUG_INIT
//#define DEBUG_OBJECT_LIFE

//-----------------------------------------------------------------------------
// EventModel(const std::string &nomme)
//-----------------------------------------------------------------------------
/**
 *  Default constructor
 *
 *  @param nomme The name of the instance being created
 */
//-----------------------------------------------------------------------------
EventModel::EventModel(const std::string &nomme) :
   PhysicalModel        (Gmat::PHYSICAL_MODEL, "EventModel", nomme),
   events               (NULL),
   eventStartIndex      (-1)
{
   objectTypeNames.push_back("EventModel");

   #ifdef DEBUG_OBJECT_LIFE
      MessageInterface::ShowMessage("Creating EventModel in default c'tor "
            "at %p\n", this);
   #endif
}

//-----------------------------------------------------------------------------
// ~EventModel()
//-----------------------------------------------------------------------------
/**
 *  Destructor
 */
//-----------------------------------------------------------------------------
EventModel::~EventModel()
{
   #ifdef DEBUG_OBJECT_LIFE
      MessageInterface::ShowMessage("Destroying EventModel %p\n", this);
   #endif
}

//-----------------------------------------------------------------------------
// EventModel(const EventModel & em)
//-----------------------------------------------------------------------------
/**
 *  Copy constructor
 *
 *  @param em The EventModel being copied into this one.
 */
//-----------------------------------------------------------------------------
EventModel::EventModel(const EventModel & em) :
   PhysicalModel        (em),
   events               (em.events),
   functionCounts       (em.functionCounts),
   eventStarts          (em.eventStarts),
   eventAssociates      (em.eventAssociates),
   eventStartIndex      (em.eventStartIndex)
{
   #ifdef DEBUG_OBJECT_LIFE
      MessageInterface::ShowMessage("Creating EventModel in copy c'tor at %p\n",
            this);
   #endif
}

//-----------------------------------------------------------------------------
// EventModel& operator=(const EventModel & em)
//-----------------------------------------------------------------------------
/**
 *  Assignment operator
 *
 *  @param em The EventModel providing parameters for this one
 *
 *  @return This EventModel, configured to match the input model
 */
//-----------------------------------------------------------------------------
EventModel& EventModel::operator=(const EventModel & em)
{
   if (this != &em)
   {
      PhysicalModel::operator=(em);

      // Note that events is a pointer.  This should still be okay, because
      // events points to the EventLocator buffer in the Sandbox; if the 
      // pointer is set, we'll preserve it here, but clear the other buffers.
      events = em.events;
      eventStartIndex = em.eventStartIndex;

      functionCounts.clear();
      eventStarts.clear();
      eventAssociates.clear();
   }

   return *this;
}


//-----------------------------------------------------------------------------
// GmatBase* Clone() const
//-----------------------------------------------------------------------------
/**
 *  Creates a copy (via the copy constructor) of this instance
 *
 *  @return The copy of the current instance
 */
//-----------------------------------------------------------------------------
GmatBase* EventModel::Clone() const
{
   return new EventModel(*this);
}


//-----------------------------------------------------------------------------
// void SetEventLocators(std::vector<EventLocator*> *els)
//-----------------------------------------------------------------------------
/**
 *  Set the EventLocator array
 *
 *  This method is called from the Sandbox during initialization, and sets the 
 *  pointers to the EventLocator objects that are part of the current run.
 *
 *  @param els The vector of pointers to the EventLocators
 */
//-----------------------------------------------------------------------------
void EventModel::SetEventLocators(std::vector<EventLocator*> *els)
{
   events = els;
}

//-----------------------------------------------------------------------------
// bool Initialize()
//-----------------------------------------------------------------------------
/**
 *  Prepares the EventModel for use in propagation
 *
 *  @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool EventModel::Initialize()
{
   bool retval = PhysicalModel::Initialize();

   functionCounts.clear();
   eventStarts.clear();
   eventAssociates.clear();
   Integer fc = 0, es = eventStartIndex;
   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      eventStarts.push_back(es);
      fc = events->at(i)->GetFunctionCount();
      functionCounts.push_back(fc);
      if (eventStartIndex >= 0)
      {
         for (Integer j = 0; j < fc; ++j)
         {
            #ifdef DEBUG_INIT
               MessageInterface::ShowMessage("Calling SetStateIndices(%d, %d, "
                     "%d)\n", j, eventStarts[i] + j,
                     theState->GetAssociateIndex(eventStarts[i]+j));
            #endif
            events->at(i)->SetStateIndices(j, eventStarts[i] + j,
                  theState->GetAssociateIndex(eventStarts[i]+j));
         }
         es += fc;
      }
   }

   return retval;
}

//-----------------------------------------------------------------------------
// bool GetDerivatives(Real *state, Real dt, Integer order, const Integer id)
//-----------------------------------------------------------------------------
/**
 *  Retrieves the derivative data from the EventModel
 *
 *  @param state The state data for the derivative calculations
 *  @param dt The time offset for the derivative data
 *  @param order The derivative order needed (1 for first order, etc)
 *  @param id ID for the type of derivative requested; unused here.
 *
 *  @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool EventModel::GetDerivatives(Real *state, Real dt, Integer order, const Integer id)
{
   bool retval = false;

   #ifdef DEBUG_EXE
      MessageInterface::ShowMessage("EventModel::GetDerivatives called\n%d "
            "events\n%d functionCounts\n", events->size(), functionCounts.size());
      MessageInterface::ShowMessage("State = [");
      for (Integer i = 0; i < eventStarts[0]+functionCounts[0]; ++i)
         MessageInterface::ShowMessage(" %le ", state[i]);
      MessageInterface::ShowMessage("]\n");
   #endif

   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      GmatEpoch now = epoch + (elapsedTime + dt)/GmatTimeConstants::SECS_PER_DAY;
      Real *data = events->at(i)->Evaluate(now, state);
      for (Integer j = 0; j < functionCounts[i]; ++j)
      {
         deriv[eventStarts[i]+j] = data[j*3+2];
      }
      retval = true;
   }

   retval = true;

   #ifdef DEBUG_EXE
      MessageInterface::ShowMessage("Deriv = [");
      for (Integer i = 0; i < eventStarts[0]+functionCounts[0]; ++i)
         MessageInterface::ShowMessage(" %le ", deriv[i]);
      MessageInterface::ShowMessage("]\n");
   #endif

   return retval;
}

//-----------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//-----------------------------------------------------------------------------
/**
 *  Method used to determine the is a given derivative type is supported
 *
 *  @param id The (enumerated) type that is being checked
 *
 *  @return true for supported types (EVENT_FUNCTION_STATE here), false if not 
 *               supported
 */
//-----------------------------------------------------------------------------
bool EventModel::SupportsDerivative(Gmat::StateElementId id)
{
   if (id == Gmat::EVENT_FUNCTION_STATE)
      return true;
   return false;
}

//-----------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//-----------------------------------------------------------------------------
/**
 *  Sets the index of the first event function for a given event locator
 *
 * @param id State Element ID for the derivative type
 * @param index Starting index in the state vector for this type of derivative
 * @param quantity Number of objects that supply this type of data
 *
 *  @return
 */
//-----------------------------------------------------------------------------
bool EventModel::SetStart(Gmat::StateElementId id, Integer index,
      Integer quantity, Integer sizeOfType)
{
   bool retval = false;

   if (id == Gmat::EVENT_FUNCTION_STATE)
   {
      eventStarts.push_back(index);
      eventStartIndex = index;
      retval = true;
   }

   return retval;
}

//-----------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//-----------------------------------------------------------------------------
/**
 *  Retrieves the names of reference objects for the model
 *
 *  @param type The type of object requested
 *
 *  @return The list of references
 */
//-----------------------------------------------------------------------------
const StringArray& EventModel::GetRefObjectNameArray(
      const UnsignedInt type)
{
   refObjectNames.clear();
   PhysicalModel::GetRefObjectNameArray(type);

   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      StringArray temp = events->at(i)->GetRefObjectNameArray(type);
      for (UnsignedInt j = 0; j < temp.size(); ++j)
      {
         if (find(refObjectNames.begin(), refObjectNames.end(), temp[j]) ==
               refObjectNames.end())
            refObjectNames.push_back(temp[j]);
      }
   }

   return refObjectNames;
}


//-----------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//       const std::string &name)
//-----------------------------------------------------------------------------
/**
 *  Sets the reference object pointer
 *
 *  @param obj The potential reference
 *  @param The object's type
 *  @param The object's name
 *
 *  @return true if the reference was set, false if not
 */
//-----------------------------------------------------------------------------
bool EventModel::SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string &name)
{
   bool retval = false;

   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      if (events->at(i)->SetRefObject(obj, type, name))
         retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool EventModel::RenameRefObject(const UnsignedInt type,
//       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Describe the method here
 *
 * @param type The type of the object being renamed
 * @param oldName The object name before the rename happens
 * @param newName The object name before the rename happens
 *
 * @return true is an object was renamed, false if not
 *
 * @note This method may be redundant here, since the locators are not clones
 */
//------------------------------------------------------------------------------
bool EventModel::RenameRefObject(const UnsignedInt type,
      const std::string &oldName, const std::string &newName)
{
   bool retval = false;

   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      StringArray temp = events->at(i)->GetRefObjectNameArray(type);
      for (UnsignedInt j = 0; j < temp.size(); ++j)
      {
         if (temp[j] == oldName)
            retval = events->at(i)->RenameRefObject(type, oldName, newName);
      }
   }

   return retval;
}
