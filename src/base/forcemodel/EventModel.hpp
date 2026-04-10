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
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#ifndef EventModel_hpp
#define EventModel_hpp

#include "PhysicalModel.hpp"

class EventLocator;

/**
 * This PhysicalModel connects EventFunctions to Integrators.
 *
 * The EventModel class is a container class that takes a collection of
 * EventLocator objects, accesses the contained EventFunctions, and generates
 * derivative data for each EventFunction for use by GMAT's propagation
 * subsystem.  The EventLocator objects are collected into a vector in the
 * Sandbox and passed into the ODEModel that manages the EventModel.  The 
 * ODEModel passes that vector into the EventModel during initialization.
 */
class EventModel: public PhysicalModel
{
public:
   EventModel(const std::string &nomme = "");
   virtual ~EventModel();
   EventModel(const EventModel& em);
   EventModel& operator=(const EventModel& em);
   virtual GmatBase* Clone() const;

   virtual void SetEventLocators(std::vector<EventLocator*> *els);
   virtual bool Initialize();

//   // Support for extra derivative calcs - the EventFunction contributions here
//   virtual const IntegerArray& GetSupportedDerivativeIds();
//   virtual const StringArray&  GetSupportedDerivativeNames();

   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index,
                         Integer quantity, Integer sizeOfType);

   virtual const StringArray& GetRefObjectNameArray(
         const UnsignedInt type);
   virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
         const std::string &name = "");
   virtual bool RenameRefObject(const UnsignedInt type,
         const std::string &oldName, const std::string &newName);


   virtual bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1,
         const Integer id = -1);

   DEFAULT_TO_NO_CLONES

private:
   /// The list of active EventLocators
   std::vector<EventLocator*> *events;
   /// Event functions counts for the locators
   std::vector<Integer> functionCounts;
   /// Start indices for event functions embedded in the event locator vector
   std::vector<Integer> eventStarts;
   /// Associated state data for event calculations
   std::vector<Integer> eventAssociates;
   /// Event start index in the state vector
   Integer eventStartIndex;
};

#endif /* EventModel_hpp */
