//$Id$
//------------------------------------------------------------------------------
//                           PropagationStateManager
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/12/15
//
/**
 * Implementation of the PropagationStateManager base class.  This is the class
 * for state managers used in GMAT's propagation subsystem.
 */
//------------------------------------------------------------------------------

#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "PropagationStateManager.hpp"
#include "GmatBase.hpp"
#include "Spacecraft.hpp"
#include "MessageInterface.hpp"
#include "PropagatorException.hpp"

#include "Array.hpp"
#include "RealUtilities.hpp"       // for IsNaN() and IsInf()
#include <sstream>                 // for <<
#include "FormationInterface.hpp"           // To allow formation member state updates

#include "MatrixFactorization.hpp"       // needed for SNC updates
#include "CholeskyFactorization.hpp"     // needed for SNC updates
#include "QRFactorization.hpp"           // needed for SNC updates
#include "Spacecraft.hpp"                // needed for covariance propagation

//#define DEBUG_STATE_CONSTRUCTION
//#define DUMP_STATE
//#define DEBUG_OBJECT_UPDATES
//#define DEBUG_PROPAGATE_SNC
//#define DEBUG_PROPAGATE_COVARIANCE

// When Spacecraft epochs are this close, the PSM will call them identical:
#define IDENTICAL_TIME_TOLERANCE  5.0e-11


//------------------------------------------------------------------------------
// PropagationStateManager(Integer size)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param size The size of the (initial) propagation state vector
 */
//------------------------------------------------------------------------------
PropagationStateManager::PropagationStateManager(Integer size) :
   StateManager                  (size),
   hasPostSuperpositionMember    (false)
{
}


//------------------------------------------------------------------------------
// ~PropagationStateManager()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PropagationStateManager::~PropagationStateManager()
{
}


//------------------------------------------------------------------------------
// PropagationStateManager(const PropagationStateManager& psm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param psm The state manager that is copied to the new one
 */
//------------------------------------------------------------------------------
PropagationStateManager::
         PropagationStateManager(const PropagationStateManager& psm) :
   StateManager                  (psm),
   hasPostSuperpositionMember    (psm.hasPostSuperpositionMember),
   timeBeforeLastUpdate (psm.timeBeforeLastUpdate),
   timeBeforeLastUpdateGT (psm.timeBeforeLastUpdateGT),
   covPropStorageMap (psm.covPropStorageMap)
{
}


//------------------------------------------------------------------------------
// PropagationStateManager& operator=(const PropagationStateManager& psm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param psm The state manager that is copied to this one
 *
 * @return This PSM configured to match psm
 */
//------------------------------------------------------------------------------
PropagationStateManager& 
         PropagationStateManager::operator=(const PropagationStateManager& psm)
{
   if (this != &psm)
   {
      StateManager::operator=(psm);

      hasPostSuperpositionMember = psm.hasPostSuperpositionMember;
      covPropStorageMap = psm.covPropStorageMap;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// Integer GetCount(Integer elementType)
//------------------------------------------------------------------------------
/**
 * Returns the number of objects that support the specified type
 * 
 * This default version just returns the total number of unique objects managed
 * by the StateManager
 * 
 * @param elementType ID for the type of state element that is being queried.  
 * 
 * @return The count of the number of objects supporting the type specified
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::GetCount(Gmat::StateElementId elementType)
{
   if (elementType == Gmat::UNKNOWN_STATE)
      return StateManager::GetCount(elementType);
   
   Integer count = 0;
   GmatBase *obj = NULL;
   
   for (Integer index = 0; index < stateSize; ++index)
   {
      if (stateMap[index]->elementID == elementType)
      {
         if (stateMap[index]->object != obj)
         {
            obj = stateMap[index]->object;
            ++count;
         }
      }
   }
   
   #ifdef DEBUG_STATE_ACCESS
      MessageInterface::ShowMessage("PropagationStateManager::GetCount found "
            "%d objects supporting type %d\n", count, elementType);
   #endif
      
   return count;
}


//------------------------------------------------------------------------------
// bool SetObject(GmatBase* theObject)
//------------------------------------------------------------------------------
/**
 * Adds an object to the prop state manager
 *
 * @param theObject The reference to the object
 *
 * @return true on success, false on failure or if the object is already in the
 *         list
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::SetObject(GmatBase* theObject)
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Setting object %s\n", 
            theObject->GetName().c_str());
   #endif
      
   // Be sure object is not already in the list 
   if (find(objects.begin(), objects.end(), theObject) != objects.end())
      return false;     // Could throw here, but that would stop everything

   // todo: validate that theObject can be propagated 

   objects.push_back(theObject);
   if (theObject->IsOfType(Gmat::FORMATION))
   {
      Integer id = theObject->GetParameterID("A1Epoch");
      epochIDs.push_back(id);
   }
   else
   {
      Integer id = theObject->GetParameterID("Epoch");
      if (theObject->GetParameterType(id) != Gmat::REAL_TYPE)
         id = theObject->GetParameterID("A1Epoch");
      epochIDs.push_back(id);
   }
   
   current = theObject;
   StringArray *objectProps = new StringArray;
   elements[current] = objectProps;
   *objectProps = current->GetDefaultPropItems();
   
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Object set; current points to %s\n", 
         current->GetName().c_str());
      MessageInterface::ShowMessage("Managing %d objects:\n", objects.size());
      for (UnsignedInt i = 0; i < objects.size(); ++i)
         MessageInterface::ShowMessage("   %2d: %s with %d prop items\n", i,
               objects[i]->GetName().c_str(),
               objects[i]->GetDefaultPropItems().size());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool RemoveObject(GmatBase* theObject)
//------------------------------------------------------------------------------
/**
 * Removes an object from the PSM
 *
 * @param theObject The object to remove
 *
 * @return true if an object was removed, false if not
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::RemoveObject(GmatBase* theObject)
{
   bool retval = false;

   std::vector<Integer>::iterator epid = epochIDs.begin();
   for (ObjectArray::iterator i = objects.begin(); i != objects.end(); ++i, ++epid)
   {
      if (*i == theObject)
      {
         objects.erase(i);
         elements.erase(theObject);
         if (epid != epochIDs.end())
            epochIDs.erase(epid);
         if (objects.size() > 0)
            current = objects[0];
         else
            current = NULL;

         retval = true;

         break;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string propName)
//------------------------------------------------------------------------------
/**
 * Identifies a propagation property for the current object
 *
 * @param propName The name of the property
 *
 * @return true if the property was saved for the current object; false if not
 *         (or if there is no current object)
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::SetProperty(std::string propName)
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Entered SetProperty(%s); current = %ld\n",
            propName.c_str(), current);
   #endif
   if (current) 
   {
      // Validate that the property can be propagated
      if (current->SetPropItem(propName) == Gmat::UNKNOWN_STATE)
      {
         if (propName == "MassFlow")
         {
            throw PropagatorException("No tank is present on " + current->GetName() + " for modeling of propulsion system mass flow.");
         }
         else
         {
            throw PropagatorException(propName
               + " is not a known propagation parameter on "
               + current->GetName());
         }
      }
      
      // Only add it if it is not yet there
      if (find(elements[current]->begin(), elements[current]->end(),
            propName) == elements[current]->end())
         elements[current]->push_back(propName);

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("Current property List:\n");
            for (StringArray::iterator i = elements[current]->begin(); 
                  i != elements[current]->end(); ++i)
               MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif
            
      return true;
   }
   
   return false;   
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string propName, Integer index)
//------------------------------------------------------------------------------
/**
 * Identifies a propagation property for an object referenced by index
 *
 * @param propName The name of the property
 * @param index The index of the object that has the property
 *
 * @return true if the property was saved for the current object; false if not
 *         (or if there is no current object)
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::SetProperty(std::string propName, Integer index)
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Entered SetProperty(%s, %d)\n",
            propName.c_str(), index);
   #endif

   if ((index < 0) || (index >= (Integer)objects.size()))
      throw PropagatorException("Index out of bounds specifying a prop object "
            "in a propagation state manager\n");

   GmatBase *obj = objects[index];

   if (obj)
   {
      // Validate that the property can be propagated
      if (obj->SetPropItem(propName) == Gmat::UNKNOWN_STATE)
         throw PropagatorException(propName
               + " is not a known propagation parameter on "
               + obj->GetName());
      if (find(elements[obj]->begin(), elements[obj]->end(), propName) ==
            elements[obj]->end())
         elements[obj]->push_back(propName);

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("Current property List:\n");
            for (StringArray::iterator i = elements[obj]->begin();
                  i != elements[obj]->end(); ++i)
               MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif

      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string propName, GmatBase *forObject)
//------------------------------------------------------------------------------
/**
 * Adds a propagation parameter associated with an object to the state
 * definition
 *
 * @param propName The name of the parameter
 * @param forObject The associated object
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::SetProperty(std::string propName,
      GmatBase *forObject)
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Entered SetProperty(%s, %s)\n",
            propName.c_str(), forObject->GetName().c_str());
   #endif

   if (find(objects.begin(), objects.end(), forObject) == objects.end())
      throw PropagatorException("Prop object " + forObject->GetName() +
            " not found in a propagation state manager\n");

   if (forObject)
   {
      // Validate that the property can be propagated
      if (forObject->SetPropItem(propName) == Gmat::UNKNOWN_STATE)
         throw PropagatorException(propName
               + " is not a known propagation parameter on "
               + forObject->GetName());
      if (find(elements[forObject]->begin(), elements[forObject]->end(),
            propName) == elements[forObject]->end())
         elements[forObject]->push_back(propName);

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("Current property List:\n");
            for (StringArray::iterator i = elements[forObject]->begin();
                  i != elements[forObject]->end(); ++i)
               MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif

      return true;
   }

   return false;
}



//------------------------------------------------------------------------------
// bool BuildState()
//------------------------------------------------------------------------------
/**
 * Collects the data needed and fills in state data
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::BuildState()
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Entered BuildState()\n");

      MessageInterface::ShowMessage("StateMap:\n");
      for (Integer index = 0; index < stateSize; ++index)
      {
         MessageInterface::ShowMessage("   %s.%s",
               stateMap[index]->objectName.c_str(),
               stateMap[index]->elementName.c_str());
      }
   #endif
   
   // Determine the size of the propagation state vector
   stateSize = SortVector();
   
   std::map<std::string,Integer> associateMap;
   // Build the associate map
   std::string name;
   for (Integer index = 0; index < stateSize; ++index)
   {
      // name = stateMap[index]->objectName;
		name = stateMap[index]->objectFullName;
		if (associateMap.find(name) == associateMap.end())
			associateMap[name] = index;
   }   
   
   state.SetSize(stateSize);

   for (Integer index = 0; index < stateSize; ++index)
   {
      // name = stateMap[index]->objectName;
		name = stateMap[index]->objectFullName;
      std::stringstream sel("");
      sel << stateMap[index]->subelement;
      state.SetElementProperties(index, stateMap[index]->elementID, 
            name + "." + stateMap[index]->elementName + "." + sel.str(), 
            associateMap[stateMap[index]->associateName]);
   }
   
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage(
            "Propagation state vector has %d elements:\n", stateSize);
      StringArray props = state.GetElementDescriptions();
      for (Integer index = 0; index < stateSize; ++index)
         MessageInterface::ShowMessage("   %d:  %s  --> associate: %d\n", index,
               props[index].c_str(), state.GetAssociateIndex(index));
   #endif
   
   #ifdef DUMP_STATE
      MapObjectsToVector();
      for (Integer i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage("State[%02d] = %.12lf, %s %d\n", i, state[i], 
               "RefState start =", state.GetAssociateIndex(i));
   #endif   
      for (Integer index = 0; index < objects.size(); index++)
      {
         if (objects[index]->GetType() == Gmat::SPACECRAFT)
         {
            timeBeforeLastUpdate = objects[index]->GetRealParameter("A1Epoch");
            timeAtLastUpdate = timeBeforeLastUpdate;
            timeBeforeLastUpdateGT = objects[index]->GetGmatTimeParameter("A1Epoch");
            timeAtLastUpdateGT = timeBeforeLastUpdateGT;
         }
      }
   return true;
}


//------------------------------------------------------------------------------
// bool MapObjectsToVector()
//------------------------------------------------------------------------------
/**
 * Retrieves data from the objects that are to be propagated, and sets those
 * data in the propagation state vector
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::MapObjectsToVector()
{
   #ifdef DEBUG_OBJECT_UPDATES
		MessageInterface::ShowMessage("Mapping objects to vector\n");

		// Look at state data before
		MessageInterface::ShowMessage("O -> V: Real object data before: [");
		for (Integer index = 0; index < stateSize; ++index)
			if (stateMap[index]->parameterType == Gmat::REAL_TYPE)
				MessageInterface::ShowMessage(" %.12le ",
				(stateMap[index]->object)->GetRealParameter(
					stateMap[index]->parameterID));
		MessageInterface::ShowMessage("]\n");
   #endif

   Real value;

   // Refresh formation data
   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      if (objects[i]->IsOfType(Gmat::FORMATION))
      {
         // Refresh the formation state data from the formation members
         ((FormationInterface*)(objects[i]))->UpdateState();
      }
   }

   for (Integer index = 0; index < stateSize; ++index)
   {
      switch (stateMap[index]->parameterType)
      {
         case Gmat::REAL_TYPE:
         {
            value = stateMap[index]->object->GetRealParameter(
               stateMap[index]->parameterID);

            if (GmatMathUtil::IsNaN(value))
               throw PropagatorException("Value for parameter " +
                  stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                  stateMap[index]->object->GetName() +
                  " is not a number");

            if (GmatMathUtil::IsInf(value))
               throw PropagatorException("Value for parameter " +
                  stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                  stateMap[index]->object->GetName() +
                  " is infinite");

            state[index] = value;

            // Update stateDot
            //state.GetStateDot()[index] = 0.0; 

            if (stateMap[index]->object->IsOfType(Gmat::SPACECRAFT))
            {
               Integer idx = stateMap[index]->parameterID - cartesianStateID;
               if (idx == 5)
               {
                  // Update rDot (velocity)
                  for (Integer k = 0; k < 3; ++k)
                     (state.GetStateDot())[index - 5 + k] = state[index - 2 + k];

                  // Update vDot (acceleration)
                  Rvector3 accel = ((Spacecraft*)(stateMap[index]->object))->GetAcceleration();
                  for (Integer k = 3; k < 6; ++k)
                     (state.GetStateDot())[index - 5 + k] = accel[k - 3];

                  #ifdef DEBUG_OBJECT_UPDATES
                     for (Integer k = 0; k < 6; ++k)
                        MessageInterface::ShowMessage("  stateDot[%d] = %.15le\n", 
                           index - 5 + k, (state.GetStateDot())[index - 5 + k]);
                  #endif

               }
            }
            break;
         }
         case Gmat::RVECTOR_TYPE:
            value = stateMap[index]->object->GetRealParameter(
                  stateMap[index]->parameterID, stateMap[index]->rowIndex);

            if (GmatMathUtil::IsNaN(value))
               throw PropagatorException("Value for array parameter " +
                     stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                     stateMap[index]->object->GetName() +
                     " is not a number");

            if (GmatMathUtil::IsInf(value))
               throw PropagatorException("Value for array parameter " +
                     stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                     stateMap[index]->object->GetName() +
                     " is infinite");

            state[index] = value;
            break;

         case Gmat::RMATRIX_TYPE:
            value = stateMap[index]->object->GetRealParameter(
                  stateMap[index]->parameterID, stateMap[index]->rowIndex,
                  stateMap[index]->colIndex);

            if (GmatMathUtil::IsNaN(value))
               throw PropagatorException("Value for array parameter " +
                     stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                     stateMap[index]->object->GetName() +
                     " is not a number");

            if (GmatMathUtil::IsInf(value))
               throw PropagatorException("Value for array parameter " +
                     stateMap[index]->object->GetParameterText(
                     stateMap[index]->parameterID) + " on object " +
                     stateMap[index]->object->GetName() +
                     " is infinite");

            state[index] = value;
            break;

         default:
            std::stringstream sel("");
            sel << stateMap[index]->subelement;
            std::string label = stateMap[index]->objectName + "." + 
                  stateMap[index]->elementName + "." + sel.str();
            MessageInterface::ShowMessage(
                  "%s not set; Element type not handled\n",label.c_str());
      }
   }
   
   // Manage epoch
   if (ObjectEpochsMatch() == false)
      MessageInterface::ShowMessage("Epochs do not match\n");

   if (objects.size() > 0)
   {
      state.SetEpoch(objects[0]->GetRealParameter(epochIDs[0]));
      state.SetEpochGT(objects[0]->GetGmatTimeParameter(epochIDs[0]));
   }
   
   #ifdef DEBUG_OBJECT_UPDATES
		MessageInterface::ShowMessage(
			"After mapping %d objects to vector, contents are\n"
			"   Epoch = %.12le\n", objects.size(), state.GetEpoch());
		for (Integer index = 0; index < stateSize; ++index)
		{
			std::stringstream msg("");
			msg << stateMap[index]->subelement;
			std::string lbl = stateMap[index]->objectName + "." +
				stateMap[index]->elementName + "." + msg.str() + " = ";
			MessageInterface::ShowMessage("   %d: %s%.12le. Its dot is %.12le\n", index, lbl.c_str(), 
				state[index], state.GetStateDot()[index]);
		}

		// Look at state data after
		MessageInterface::ShowMessage("Real object data after: [");
		for (Integer index = 0; index < stateSize; ++index)
			if (stateMap[index]->parameterType == Gmat::REAL_TYPE)
				MessageInterface::ShowMessage(" %.12le ",
				(stateMap[index]->object)->GetRealParameter(
					stateMap[index]->parameterID));
		MessageInterface::ShowMessage("]\n");
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool PropagationStateManager::MapVectorToObjects()
//------------------------------------------------------------------------------
/**
 * Sets data from the propagation state vector into the objects that manage
 * those data
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::MapVectorToObjects()
{
   #ifdef DEBUG_OBJECT_UPDATES
		if (state.HasPrecisionTime())
			MessageInterface::ShowMessage("Mapping vector to objects\n"
				"   EpochGT = %s\n", state.GetEpochGT().ToString().c_str());
		else
			MessageInterface::ShowMessage("Mapping vector to objects\n"
				"   Epoch = %.12lf\n", state.GetEpoch());

		// Look at state data before
		MessageInterface::ShowMessage("V -> O: Real object data before: [");
		for (Integer index = 0; index < stateSize; ++index)
        {
			if (stateMap[index]->parameterType == Gmat::REAL_TYPE)
				MessageInterface::ShowMessage(" %.12le ",
				(stateMap[index]->object)->GetRealParameter(
					stateMap[index]->parameterID));
            else if (stateMap[index]->parameterType == Gmat::RVECTOR_TYPE)
               MessageInterface::ShowMessage(" %.12le ",
               (stateMap[index]->object)->GetRealParameter(stateMap[index]->parameterID, stateMap[index]->rowIndex));
            else if (stateMap[index]->parameterType == Gmat::RMATRIX_TYPE)
               MessageInterface::ShowMessage(" %.12le ",
               (stateMap[index]->object)->GetRealParameter(
                  stateMap[index]->parameterID, stateMap[index]->rowIndex, stateMap[index]->colIndex));
        }
		MessageInterface::ShowMessage("]\n");
   #endif
   std::string stmObjectName = "";
   std::string covObjectName = "";
   Rvector3 acceleration;

   bool propCovarianceWithNoise = false;
   for (Integer index = 0; index < stateSize; ++index)
   {
      #ifdef DEBUG_OBJECT_UPDATES
			std::stringstream msg("");
			msg << stateMap[index]->subelement;
			std::string lbl = stateMap[index]->objectName + "." +
				stateMap[index]->elementName + "." + msg.str() + " = ";
			MessageInterface::ShowMessage("   %d: %s%.12le. Its dot = %.15le\n", index, lbl.c_str(), 
				state[index], state.GetStateDot()[index]);
      #endif

      // Log the oldSTM for use in Process noise calculation
      if (stateMap[index]->elementName == "STM" && stmObjectName != stateMap[index]->objectName)
      {
         stmObjectName = stateMap[index]->objectName;
         covPropStorageMap[stmObjectName + "OldSTM"] = (stateMap[index]->object)->GetRmatrixParameter("FullSTM");
      }

      // If Covariance, propagate before passing to object
      if (stateMap[index]->elementName == "Covariance" && covObjectName != stateMap[index]->objectName)
      {
         covObjectName = stateMap[index]->objectName;
         PropagateCovarianceMatrix(index, covPropStorageMap[covObjectName + "OldSTM"], propCovarianceWithNoise);
      }

      switch (stateMap[index]->parameterType)
      {
         case Gmat::REAL_TYPE:
         {
            (stateMap[index]->object)->SetRealParameter(
               stateMap[index]->parameterID, state[index]);
            
            if (stateMap[index]->object->IsOfType(Gmat::SPACECRAFT))
            {
               Integer accelIndex = stateMap[index]->parameterID - (cartesianStateID + 3);
               #ifdef DEBUG_OBJECT_UPDATES
                  MessageInterface::ShowMessage("accelIndex = %d - %d = %d\n", 
                     stateMap[index]->parameterID, (cartesianStateID + 3), accelIndex);
               #endif
               if (accelIndex == 0)
               {
                  acceleration[0] = (state.GetStateDot())[index];
                  acceleration[1] = (state.GetStateDot())[index + 1];
                  acceleration[2] = (state.GetStateDot())[index + 2];

                  ((Spacecraft*)(stateMap[index]->object))->SetAcceleration(acceleration);

                  #ifdef DEBUG_OBJECT_UPDATES
                     MessageInterface::ShowMessage("  acceleration = [%.15le   %.15le   %.15le]\n", 
                        acceleration[0], acceleration[1], acceleration[2]);
                  #endif
                  
                  // clear its value for the next acceleration value setting
                  acceleration.Set(0.0, 0.0, 0.0);
               }
            }
            break;
         }
         case Gmat::RVECTOR_TYPE:
            stateMap[index]->object->SetRealParameter(
                     stateMap[index]->parameterID, state[index],
                     stateMap[index]->rowIndex);
            break;

         case Gmat::RMATRIX_TYPE:
            stateMap[index]->object->SetRealParameter(
                     stateMap[index]->parameterID, state[index], 
                     stateMap[index]->rowIndex, stateMap[index]->colIndex);
            break;

         default:
            std::stringstream sel("");
            sel << stateMap[index]->subelement;
            std::string label = stateMap[index]->objectName + "." + 
                  stateMap[index]->elementName + "." + sel.str();
            MessageInterface::ShowMessage(
                  "%s not set; Element type not handled\n",label.c_str());
      }
   }
   


   GmatTime  theEpochGT = state.GetEpochGT();
   GmatEpoch theEpoch = state.GetEpoch();
   // Store logged time for next propagation
   Real timeSinceLast = GmatMathUtil::Abs(state.GetEpoch() - timeAtLastUpdate);
   Real timeSinceLastGT = GmatMathUtil::Abs((state.GetEpochGT() - timeAtLastUpdateGT).GetTimeInSec());
   // Check if reverting for TakeAFinalStep(), but don't revert in cases where PSM is initializing
   Real lastStepSize = GmatMathUtil::Abs(timeAtLastUpdate- timeBeforeLastUpdate);
   bool stepTooBig = lastStepSize != 0 && lastStepSize < 1;
   bool revertFromLast =  ((state.HasPrecisionTime() && state.GetEpochGT() == timeBeforeLastUpdateGT) || (!state.HasPrecisionTime() && state.GetEpoch() == timeBeforeLastUpdate)) && stepTooBig;
   if (revertFromLast)
   {
      timeAtLastUpdate = timeBeforeLastUpdate;
      timeAtLastUpdateGT = timeBeforeLastUpdateGT;
   }
   else
   {
      timeBeforeLastUpdate = timeAtLastUpdate;
      timeBeforeLastUpdateGT = timeAtLastUpdateGT;
      timeAtLastUpdate = theEpoch;
      timeAtLastUpdateGT = theEpochGT;
   }
   if (state.HasPrecisionTime())
      theEpoch = theEpochGT.GetMjd();

   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      objects[i]->SetRealParameter(epochIDs[i], theEpoch);
      if (state.HasPrecisionTime())
         objects[i]->SetGmatTimeParameter(epochIDs[i], theEpochGT);
      else
         objects[i]->SetGmatTimeParameter(epochIDs[i], GmatTime(theEpoch));
   }

   #ifdef DEBUG_OBJECT_UPDATES
		// Look at object data after
		MessageInterface::ShowMessage("Real object data after: [");
		for (Integer index = 0; index < stateSize; ++index)
        {
		   if (stateMap[index]->parameterType == Gmat::REAL_TYPE)
             MessageInterface::ShowMessage(" %.12le ",
				(stateMap[index]->object)->GetRealParameter(
					stateMap[index]->parameterID));
             else if (stateMap[index]->parameterType == Gmat::RVECTOR_TYPE)
                 MessageInterface::ShowMessage(" %.12le ",
                    (stateMap[index]->object)->GetRealParameter(stateMap[index]->parameterID, stateMap[index]->rowIndex));
             else if (stateMap[index]->parameterType == Gmat::RMATRIX_TYPE)
                 MessageInterface::ShowMessage(" %.12le ",
                 (stateMap[index]->object)->GetRealParameter(
                    stateMap[index]->parameterID, stateMap[index]->rowIndex, stateMap[index]->colIndex));
        }
		MessageInterface::ShowMessage("]\n");
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool RequiresCompletion()
//------------------------------------------------------------------------------
/**
 * Flags if additional steps are needed for derivatives after superposition
 *
 * Returns true if there is a post-superposition step required in the
 * propagation state vector.  This case occurs, for example, for the orbit STM
 * and A-Matrix in order to fill in the upper half of the matrices, and (for the
 * STM) to apply \Phi\dot = A \Phi.
 *
 * @return true if a final "completion" step is needed, false if not.
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::RequiresCompletion()
{
   return hasPostSuperpositionMember;
}

//------------------------------------------------------------------------------
// bool ObjectEpochsMatch()
//------------------------------------------------------------------------------
/**
 * Tests to see if the object epochs match
 *
 * @return true is the eopchs match, false if not
 */
//------------------------------------------------------------------------------
bool PropagationStateManager::ObjectEpochsMatch()
{
   bool retval = true;

   if (objects.size() > 1)
   {
      if (((SpacePoint*)objects[0])->HasPrecisionTime())
      {
         GmatTime theEpochGT = objects[0]->GetGmatTimeParameter(epochIDs[0]);
         Real diff = 0.0, dt;
         for (UnsignedInt i = 1; i < objects.size(); ++i)
         {
#ifdef DEBUG_OBJECT_UPDATES
            MessageInterface::ShowMessage("   Epochs are %s for %s and "
               "%s for %s\n", theEpochGT.ToString().c_str(), objects[0]->GetName().c_str(),
               objects[i]->GetGmatTimeParameter(epochIDs[i]).ToString().c_str(),
               objects[i]->GetName().c_str());
#endif

            dt = fabs((theEpochGT - objects[i]->GetGmatTimeParameter(epochIDs[i])).GetTimeInSec());
            if (dt > IDENTICAL_TIME_TOLERANCE)
            {
               retval = false;
            }
            diff = (diff > dt ? diff : dt);
         }
      }
      else
      {
         GmatEpoch theEpoch = objects[0]->GetRealParameter(epochIDs[0]);
         Real diff = 0.0, dt;
         for (UnsignedInt i = 1; i < objects.size(); ++i)
         {
#ifdef DEBUG_OBJECT_UPDATES
            MessageInterface::ShowMessage("   Epochs are %.12lf for %s and "
               "%.12lf for %s\n", theEpoch, objects[0]->GetName().c_str(),
               objects[i]->GetRealParameter(epochIDs[i]), 
               objects[i]->GetName().c_str());
#endif

            dt = fabs(theEpoch - objects[i]->GetRealParameter(epochIDs[i]));
            if (dt > IDENTICAL_TIME_TOLERANCE)
            {
               retval = false;
            }
            diff = (diff > dt ? diff : dt);
         }
      }
      // Here's how we'll warn if needed:
//      if (retval && (diff != 0.0))
//         MessageInterface::ShowMessage("Spacecraft epochs do not match in a "
//               "Propagator used in the Propagate command, but are within "
//               "tolerance (%le sec) acceptable for propagation\n",
//               IDENTICAL_TIME_TOLERANCE * 86400.0);
   }
   return retval;
}


//------------------------------------------------------------------------------
// Integer GetCompletionCount()
//------------------------------------------------------------------------------
/**
 * Obtains the number of objects that need to complete updates
 *
 * @return The number of completion indices registered
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::GetCompletionCount()
{
   return completionIndexList.size();
}


//------------------------------------------------------------------------------
// Integer GetCompletionIndex(Integer start)
//------------------------------------------------------------------------------
/**
 * Describe the method here
 *
 * @param which Index of the beginning of the element that needs completion
 *
 * @return
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::GetCompletionIndex(const Integer which)
{
   return completionIndexList[which];
}


//------------------------------------------------------------------------------
// Integer GetCompletionSize(Integer start)
//------------------------------------------------------------------------------
/**
 * Describe the method here
 *
 * @param which Index of the beginning of the element that needs completion
 *
 * @return
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::GetCompletionSize(const Integer which)
{
   return completionSizeList[which];
}


//------------------------------------------------------------------------------
// Integer GetSTMIndex(Integer forParameterID)
//------------------------------------------------------------------------------
/**
 * Finds the STM row/column index for the ID'd parameter of a given object
 *
 * @param forParameterID    The ID of the parameter
 * @param obj               the object having the parameter ID
 *
 * @return The STM row, or -1 if not in the STM
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::GetSTMIndex(Integer forParameterID, GmatBase* obj)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < stmRowMapID.size(); ++i)
   {
      #ifdef DEBUG_STM
         MessageInterface::ShowMessage("PSM %p, STM Row map %d, val = <id = %d, obj = %p> parm %d, " 
               "prop state size %d\n", this, i, stmRowMapID[i], stmRowMapObj[i], forParameterID,
               stateSize);
      #endif

      if ((stmRowMapID[i] == forParameterID)&& ((stmRowMapObj[i] == obj)||(obj == NULL)))
      {
         retval = i;
         break;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer PropagationStateManager::SortVector()
//------------------------------------------------------------------------------
/**
 * Arranges the propagation state vector for use, and determines the size of the
 * vector
 *
 * @return The size of the state vector
 */
//------------------------------------------------------------------------------
Integer PropagationStateManager::SortVector()
{
   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage(
            "Entered PropagationStateManager::SortVector()\n");
   #endif

   StringArray *propList;
   std::vector<Integer> order;
   std::vector<Gmat::StateElementId> idList;
   ObjectArray owners;
   StringArray property;
   std::vector<Integer>::iterator oLoc;

   Gmat::StateElementId id;
   Integer size, loc = 0, val;
   stateSize = 0;
   // Initially assume there is no post superposition member
   hasPostSuperpositionMember = false;


   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Element list:\n");
      Integer k = 0;
      for (std::map<GmatBase*, StringArray*>::iterator i = elements.begin();
            i != elements.end(); ++i)
      {
         current  = i->first;
         propList = i->second;
         MessageInterface::ShowMessage("   %d: %s ->\n", ++k,
               current->GetName().c_str());
         for (UnsignedInt j = 0; j < propList->size(); ++j)
         {
            MessageInterface::ShowMessage("      %s\n", (*propList)[j].c_str());
         }
      }
   #endif


   // First build a list of the property IDs and objects, measuring state size 
   // at the same time
   for (UnsignedInt q = 0; q < objects.size(); ++q)
   {
      current  = objects[q];
      propList = elements[current];
      
      for (StringArray::iterator j = propList->begin(); 
            j != propList->end(); ++j)
      {
         id = (Gmat::StateElementId)current->SetPropItem(*j);
         if (id == Gmat::UNKNOWN_STATE)
            throw PropagatorException("Unknown state element: " + (*j) +
                  " on object " + current->GetName() + ", a " +
                  current->GetTypeName());
         size = current->GetPropItemSize(id);

         #ifdef DEBUG_STATE_CONSTRUCTION
            MessageInterface::ShowMessage("%s has size %d;  ", j->c_str(), size);
         #endif

         if (size <= 0)
            throw PropagatorException("State element " + (*j) +
                  " has size set less than or equal to 0; unable to continue.");
         stateSize += size;
         for (Integer k = 0; k < size; ++k)
         {
            idList.push_back(id);
            if (current->PropItemNeedsFinalUpdate(id))
               hasPostSuperpositionMember = true;
            owners.push_back(current);
            property.push_back(*j);

            // Put this item in the ordering list
            bool found = false;
            oLoc = order.begin();
            while (oLoc != order.end())
            {
               val = idList[*oLoc];
               if (id < val)
               {
                  #ifdef DEBUG_STATE_CONSTRUCTION
                     MessageInterface::ShowMessage("Inserting; id = %d, z = %d,"
                           "  loc = %d\n", id, (*oLoc), loc);
                  #endif
                     
                  order.insert(oLoc, loc);
                  found = true;
                  break;
               }
               ++oLoc;
            }
            if (!found)
                order.push_back(loc);
            
            ++loc;
         }
      }
   }
   
   ListItem *newItem;
   val = 0;
   completionIndexList.clear();
   completionSizeList.clear();
   stmRowMapID.clear();
	stmRowMapObj.clear();

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage(
            "State size is %d\n", stateSize);
   #endif
   
   // Next build the state
   stateMap.clear();
   for (Integer i = 0; i < stateSize; ++i)
   {
      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("%d <- %d: %d %s.%s gives ", i, order[i], 
               idList[order[i]], owners[order[i]]->GetName().c_str(), 
               property[order[i]].c_str());
      #endif

      newItem = new ListItem;
      newItem->objectName  = owners[order[i]]->GetName();
		newItem->objectFullName = owners[order[i]]->GetFullName();
      newItem->elementName = property[order[i]];
      if (owners[order[i]]->HasAssociatedStateObjects())
         newItem->associateName = owners[order[i]]->GetAssociateName(val);
      else
         newItem->associateName = owners[order[i]]->GetName();
      newItem->object      = owners[order[i]];
      newItem->elementID   = idList[order[i]];
      newItem->subelement  = ++val;
      newItem->parameterID = 
            owners[order[i]]->GetParameterID(property[order[i]]);
      newItem->parameterType = 
            owners[order[i]]->GetParameterType(newItem->parameterID);
      
      newItem->dynamicObjectProperty =
            newItem->object->ParameterAffectsDynamics(newItem->parameterID);

      if (newItem->parameterType == Gmat::REAL_TYPE)
         newItem->parameterID += val - 1;

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("[%s, %s, %s, %d, %d, %d, %d, %s]\n",
               newItem->objectName.c_str(),
               newItem->elementName.c_str(),
               newItem->associateName.c_str(),
               newItem->elementID,
               newItem->subelement,
               newItem->parameterID,
               newItem->parameterType,
               (newItem->dynamicObjectProperty ? "dynamic" : "static"));
      #endif

      if (newItem->parameterType == Gmat::RVECTOR_TYPE)
      {
         const Rvector vec =
            owners[order[i]]->GetRvectorParameter(property[order[i]]);
         newItem->rowLength = vec.GetSize();
         newItem->rowIndex = val - 1;
      }

      if (newItem->parameterType == Gmat::RMATRIX_TYPE)
      {
         const Rmatrix mat = 
            owners[order[i]]->GetRmatrixParameter(property[order[i]]);
         newItem->rowLength = mat.GetNumColumns();
         newItem->colIndex = (val-1) % newItem->rowLength;
         newItem->rowIndex = (Integer)((val - 1) / newItem->rowLength);
         
         #ifdef DEBUG_STATE_CONSTRUCTION
            MessageInterface::ShowMessage(
                  "RowLen = %d, %d -> row %2d  col %2d\n", newItem->rowLength, 
                  val, newItem->rowIndex, newItem->colIndex); 
         #endif
         // While we're here, grab the STM mapping if this is STM
         if ((newItem->elementID == Gmat::ORBIT_STATE_TRANSITION_MATRIX) &&
             (newItem->rowIndex == 0))
         {
            stmRowMapID.push_back(owners[order[i]]->GetStmRowId(newItem->colIndex));
				stmRowMapObj.push_back(owners[order[i]]);
            #ifdef DEBUG_STATE_CONSTRUCTION
               MessageInterface::ShowMessage("   STM column for %d\n",
                     stmRowMapID[newItem->colIndex]);
            #endif
         }
      }
      
      newItem->nonzeroInit = owners[order[i]]->
            ParameterDvInitializesNonzero(newItem->parameterID,
                  newItem->rowIndex, newItem->colIndex);
      if (newItem->nonzeroInit)
      {
         newItem->initialValue = owners[order[i]]->
                ParameterDvInitialValue(newItem->parameterID,
                      newItem->rowIndex, newItem->colIndex);
      }
      if (newItem->object->PropItemNeedsFinalUpdate(newItem->elementID))
      {
         completionIndexList.push_back(newItem->elementID);
         completionSizeList.push_back(1);       // Or count sizes?
      }

      newItem->postDerivativeUpdate = owners[order[i]]->
            ParameterUpdatesAfterSuperposition(newItem->parameterID);


      newItem->length      = owners[order[i]]->GetPropItemSize(idList[order[i]]);
      
      if (val == newItem->length)
         val = 0;
      
      stateMap.push_back(newItem);
   }

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("State map contents:\n");
      for (std::vector<ListItem*>::iterator i = stateMap.begin(); 
            i != stateMap.end(); ++i)
         MessageInterface::ShowMessage("   %s %s %d %d of %d, id = %d\n", 
               (*i)->objectName.c_str(), (*i)->elementName.c_str(),
               (*i)->elementID, (*i)->subelement, (*i)->length, 
               (*i)->parameterID); 
      MessageInterface::ShowMessage(
            "Finished PropagationStateManager::SortVector()\n");
   #endif

   // It needs to speicify ID of spacecraft's CartesianState pramater in order to map acceleration values
   for (Integer i = 0; i < objects.size(); ++i)
   {
      if (objects[i]->IsOfType(Gmat::SPACECRAFT))
      {
         cartesianStateID = ((Spacecraft*)(objects[i]))->GetParameterID("CartesianState");
         break;
      }
   }

   return stateSize;
}



//------------------------------------------------------------------------------
// bool PropagateCovarianceMatrix()
//------------------------------------------------------------------------------
/**
 * Retrieves data from the objects that are to be propagated, and sets those
 * data in the propagation state vector
 * @param indexNum the index of the stateMap where covariance begins
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
bool PropagationStateManager::PropagateCovarianceMatrix(Integer indexNum, Rmatrix oldSTMMat, bool &propagatedCovarianceWithNoise)
{
   bool retval = false;

   // Get the info of the object the covariance is associated with, then assemble to the STM
   std::string covarianceObjectName = stateMap[indexNum]->objectName;

   // Move STM epoch from t_o to t_n-1, using equation STM(t_n,t_n-1) = STM(t_n, t_0)*STM(t_n-1,t_0)^-1
   Rmatrix stm = (stateMap[indexNum]->object)->GetRmatrixParameter("FullSTM");
   stm = stm * oldSTMMat.Inverse();  


   Rmatrix cov = (stateMap[indexNum]->object)->GetRmatrixParameter("Covariance");
   // Ensure that STM is same size as Cov (Revisit when propagating non-Cartesian covariance)
   stm.ChangeSize(cov.GetNumRows(), cov.GetNumColumns(), false);

   Rmatrix newCov;
   Real timeSinceLast = GmatMathUtil::Abs(state.GetEpoch() - timeAtLastUpdate);
   Real timeSinceLastGT = GmatMathUtil::Abs((state.GetEpochGT() - timeAtLastUpdateGT).GetTimeInSec());
   // Check if time has passed since the last update
   if ((state.HasPrecisionTime() && timeSinceLastGT > 0) || (!state.HasPrecisionTime() && timeSinceLast > 0))
   {
      // Check if reverting for TakeAFinalStep()
      bool revertFromLast = (state.HasPrecisionTime() && state.GetEpochGT() == timeBeforeLastUpdateGT) || (!state.HasPrecisionTime() && state.GetEpoch() == timeBeforeLastUpdate);
      if (revertFromLast)
      {
         newCov = covPropStorageMap[stateMap[indexNum]->objectName + "LastCov"];
         covPropStorageMap.erase(stateMap[indexNum]->objectName + "sqrtP");                  // erasing by key
      }
      else
      {
         GmatBase* processnoise = (stateMap[indexNum]->object)->GetRefObject(GmatType::GetTypeId("ProcessNoiseModel"), "ProcessNoiseModel");

         if (processnoise) // Propagation with Noise
         {
            if (processnoise->GetStringParameter("Type") == "StateNoiseCompensation")
            {
               UpdateProcessNoiseCholesky(stm, cov, indexNum, newCov);
               propagatedCovarianceWithNoise = true;
            }
            else
            {
               throw PropagatorException("The process noise must be of type StateNoiseCompensation for use with covariance propagation");
            }
         }
         else  // Basic Propagation, no Noise
         {
            // (Revisit when propagating non-Cartesian covariance)
            stm.ChangeSize(6, 6, false);
            newCov = stm * cov*stm.Transpose();
         }
         covPropStorageMap[stateMap[indexNum]->objectName + "LastCov"] = cov;
      }
   }
   else // If no time has passed, the covariance is unchanged.
   {
      newCov = cov;
   }
   #ifdef DEBUG_PROPAGATE_COVARIANCE
      MessageInterface::ShowMessage("PropagationStateManager::PropagateCovarianceMatrix: Start Covariance propagation \n");
   MessageInterface::ShowMessage("Covariance of %s at epoch %.12f\n", stateMap[indexNum]->objectName.c_str(), state.GetEpoch());
      MessageInterface::ShowMessage("oldCov = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", cov(i, j));
         }
         MessageInterface::ShowMessage("\n");
      }

      MessageInterface::ShowMessage("stm = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", stm(i, j));
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("pBar with noise = \n");
#endif
      for (Integer i = indexNum; i < indexNum + stateMap[indexNum]->length; i++)
      {

         state[i] = newCov(stateMap[i]->rowIndex, stateMap[i]->colIndex);

#ifdef DEBUG_PROPAGATE_COVARIANCE
         MessageInterface::ShowMessage(" %.12le ", newCov(stateMap[i]->rowIndex, stateMap[i]->colIndex));
         if (stateMap[i]->colIndex == 5)
         {
            MessageInterface::ShowMessage(" \n");
         }
#endif
      }

#ifdef DEBUG_PROPAGATE_COVARIANCE
      MessageInterface::ShowMessage("PropagationStateManager::PropagateCovarianceMatrix: End Covariance propagation \n");
#endif
      // Track previous time for process noise calculation
      return retval;
}


//------------------------------------------------------------------------------
// void UpdateProcessNoiseQMAtrix(Integer indexNum, Rmatrix &Q)
//------------------------------------------------------------------------------
/**
 * This updates the process noise matrix, Q
 *
 * @param indexNum the index of the stateMap where covariance begins
 * @param &Q The output noise matrix (passthrough)
 */
 //------------------------------------------------------------------------------
void PropagationStateManager::UpdateProcessNoiseQMAtrix(Integer indexNum, Rmatrix &Q)
{
   #ifdef DEBUG_PROPAGATE_SNC
      MessageInterface::ShowMessage("Performing process noise update\n");
   #endif

   // Zero out last process noise
   GmatBase* processnoise = (stateMap[indexNum]->object)->GetRefObject( GmatType::GetTypeId("ProcessNoiseModel"), "ProcessNoiseModel")->GetRefObject(GmatType::GetTypeId("ProcessNoiseModel"), "ProcessNoiseModel");


//   Rmatrix = CreateNoiseMatrix(indexNum);
   Rvector accelSigmaVec = processnoise->GetRvectorParameter("AccelNoiseSigma");
   // When using Process noise, must use time since last update (Revisit when propagating non-Cartesian covariance)
   Rmatrix result(6,6);
   Real elapsedTime = 0;
   
   if (timeBeforeLastUpdate == timeAtLastUpdate) // Check if this is the t_0 update
   {
      elapsedTime = (state.GetEpoch() - timeBeforeLastUpdate)*GmatTimeConstants::SECS_PER_DAY;
      if (state.HasPrecisionTime())
      {
         elapsedTime = (state.GetEpochGT() - timeBeforeLastUpdateGT).GetTimeInSec();
         processnoise->SetGmatTimeParameter("CurrentEpochGT", state.GetEpochGT());
      }
   }
   else
   {
      elapsedTime = (state.GetEpoch() - timeAtLastUpdate)*GmatTimeConstants::SECS_PER_DAY;
      if (state.HasPrecisionTime())
      {
         elapsedTime = (state.GetEpochGT() - timeAtLastUpdateGT).GetTimeInSec();
         processnoise->SetGmatTimeParameter("CurrentEpochGT", state.GetEpochGT());
      }
   }
   elapsedTime = GmatMathUtil::Abs(elapsedTime);
   processnoise->SetRealParameter("CurrentEpoch", state.GetEpoch());
   processnoise->SetRealParameter("ElapsedTime", elapsedTime);
   Q = processnoise->GetRmatrixParameter("CurrentNoiseMatrix");

#ifdef DEBUG_PROPAGATE_SNC
   MessageInterface::ShowMessage("ElapsedTime = %6.13f\n", elapsedTime);
   MessageInterface::ShowMessage("CurrentTime = %6.13f\n", state.GetEpoch());
   MessageInterface::ShowMessage("Q = \n");
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      for (UnsignedInt j = 0; j < 6; ++j)
         MessageInterface::ShowMessage("   %.12le", Q(i, j));
      MessageInterface::ShowMessage("\n");
}
#endif
}


//------------------------------------------------------------------------------
// void UpdateProcessNoiseStandard(Rmatrix &stm,Rmatrix &cov, Integer indexNum, Rmatrix &pBar)
//------------------------------------------------------------------------------
/**
 * Performs the time update of the state error covariance
 *
 * This method applies equation 4.7.1(b), and then symmetrizes the resulting
 * time updated covariance, pBar.
 * @param &stm The state transition matrix
 * @param &cov The unmodified covariance matrix
 * @param indexNum the index of the stateMap where covariance begins
 * @param &pBar The output covariance (passthrough)
 */
 //------------------------------------------------------------------------------
void PropagationStateManager::UpdateProcessNoiseStandard(Rmatrix &stm, Rmatrix &cov, Integer indexNum, Rmatrix &pBar)
{
   #ifdef DEBUG_PROPAGATE_SNC
      MessageInterface::ShowMessage("Performing time update\n");
   #endif
      
   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   // see esm.CartToSolveForStateConversionDerivativeMatrix() for cart2SolvMatrix when going beyond cartesian
   // With just cartesian, may set to just Identity, but left in to make easier in the future
   Rmatrix cart2SolvMatrix = Rmatrix::Identity(6); 
   Rmatrix dX_dS = cart2SolvMatrix; 
   Rmatrix dS_dX = cart2SolvMatrix.Inverse();
   Rmatrix Q(6, 6);
   UpdateProcessNoiseQMAtrix(indexNum, Q);

   Rmatrix Q_S = dS_dX * Q * dS_dX.Transpose();

   // When using Process noise, must covariance and stm from t_n-1 to t_n, rather than t_0 to t_n
   Rmatrix stm_S = dS_dX * (stm)* dX_dS;


   pBar = stm_S * (cov)* stm_S.Transpose() + Q_S;

   // make it symmetric!
   Symmetrize(pBar);

   #ifdef DEBUG_PROPAGATE_SNC
   MessageInterface::ShowMessage("Q = \n");
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      for (UnsignedInt j = 0; j < 6; ++j)
         MessageInterface::ShowMessage("   %.12le", Q(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("stm = \n");
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      for (UnsignedInt j = 0; j < 6; ++j)
         MessageInterface::ShowMessage("   %.12le", stm(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("pBar = \n");
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      for (UnsignedInt j = 0; j < 6; ++j)
      {
         MessageInterface::ShowMessage("  %.12le", pBar(i, j));
      }
      MessageInterface::ShowMessage("\n");
   }
   #endif
}

//------------------------------------------------------------------------------
// void UpdateProcessNoiseCholesky(Rmatrix &stm,Rmatrix &cov, Integer indexNum, Rmatrix &pBar)
//------------------------------------------------------------------------------
/**
 * Performs the update of the state error covariance
 *
 * This method uses Cholesky factorization for covariance
 * This is based on section 5.7 of Brown and Hwang 4e
 * @param &stm The state transition matrix
 * @param &cov The unmodified covariance matrix
 * @param indexNum the index of the stateMap where covariance begins
 * @param &pBar The output covariance (passthrough)
 */
 //------------------------------------------------------------------------------
void PropagationStateManager::UpdateProcessNoiseCholesky(Rmatrix &stm,Rmatrix &cov, Integer indexNum, Rmatrix &pBar)
{
   #ifdef DEBUG_PROPAGATE_SNC
      MessageInterface::ShowMessage("Performing time update\n");
   #endif


   // Calculate conversion derivative matrix [dX/dS] from Cartesian to Solve-for state
   // see esm.CartToSolveForStateConversionDerivativeMatrix() for cart2SolvMatrix when going beyond cartesian
   // With just cartesian, may set to just Identity, but left in to make easier in the future
   Rmatrix cart2SolvMatrix = Rmatrix::Identity(6); 
   Rmatrix dX_dS = cart2SolvMatrix; 
   Rmatrix dS_dX = cart2SolvMatrix.Inverse();
   Rmatrix Q(6, 6); 

   UpdateProcessNoiseQMAtrix(indexNum,Q);

   Rmatrix Q_S = dS_dX * Q * dS_dX.Transpose();
   Rmatrix stm_S = dS_dX * (stm)* dX_dS;


#ifdef DEBUG_PROPAGATE_SNC
   MessageInterface::ShowMessage("COV = \n");
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      for (UnsignedInt j = 0; j < 6; ++j)
         MessageInterface::ShowMessage("   %.12le", cov(i, j));
      MessageInterface::ShowMessage("\n");
   }
#endif
   // Form perform thinQR decomposition to calculate pBar

   Rmatrix sqrtQ_T(6, 6);

   CholeskyFactorization cf;
   bool hasZeroDiag = false;
   for (UnsignedInt ii = 0U; ii < 6; ii++)
   {
      if (Q_S(ii, ii) == 0U)
      {
         hasZeroDiag = true;
         break;
      }
   }

   if (!hasZeroDiag)
   {
      try
      {
         cf.Factor(Q_S, sqrtQ_T);
      }
      catch (PropagatorException e)
      {
         throw PropagatorException("The process noise matrix is not positive definite!");
      }
   }
   else
   {
      // Remove all zero rows/columns first
      IntegerArray removedIndexes;
      IntegerArray auxVector;
      Integer numRemoved;
      Rmatrix reducedQ_S = MatrixFactorization::CompressNormalMatrix(Q_S,
         removedIndexes, auxVector, numRemoved);
      // Expand from 6 to statenum when doing more than Cartesian
      Rmatrix reducedSqrtQ_T(6 - numRemoved, 6 - numRemoved);
      try
      {
         cf.Factor(reducedQ_S, reducedSqrtQ_T);
      }
      catch (PropagatorException e)
      {
         throw PropagatorException("The process noise matrix is not positive definite!");
      }

      sqrtQ_T = MatrixFactorization::ExpandNormalMatrixInverse(reducedSqrtQ_T,
         auxVector, numRemoved);
   }
   
   Rmatrix sqrtP = Rmatrix::Identity(6);

   // If sqrtP does not exist, create and add it to map 
   if (covPropStorageMap.find(stateMap[indexNum]->objectName + "sqrtP") == covPropStorageMap.end() || (GmatMathUtil::Abs(state.GetEpoch() - timeAtLastUpdate) < GmatMathUtil::Abs(timeBeforeLastUpdate- timeAtLastUpdate)))
   {
   cf.Factor(cov, sqrtP);
      covPropStorageMap[stateMap[indexNum]->objectName + "sqrtP"] = sqrtP.Transpose();
   }
   sqrtP = covPropStorageMap.at(stateMap[indexNum]->objectName + "sqrtP");

   Rmatrix stmP = stm_S * sqrtP;
   Rmatrix sqrtQ = sqrtQ_T.Transpose();

   thinQR(stmP, sqrtQ,sqrtP);
   covPropStorageMap[ stateMap[indexNum]->objectName + "sqrtP"] = sqrtP ;


   // Warn if covariance is not positive definite
   for (UnsignedInt ii = 0U; ii < 6; ii++)
   {
      if (GmatMathUtil::Abs(sqrtP(ii, ii)) < 1e-16)
      {
         MessageInterface::ShowMessage("WARNING The covariance is no longer positive definite! Epoch = %.12f\n", state.GetEpochGT().GetMjd());
         break;
      }
   }

   pBar = sqrtP * sqrtP.Transpose();

   // make it symmetric!
   Symmetrize(pBar);
#ifdef DEBUG_PROPAGATE_SNC
   MessageInterface::ShowMessage("stm = \n");
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      for (UnsignedInt j = 0; j < 6; ++j)
         MessageInterface::ShowMessage("   %.12le", stm(i, j));
      MessageInterface::ShowMessage("\n");
   }

   MessageInterface::ShowMessage("pBar = \n");
   for (UnsignedInt i = 0; i < 6; ++i)
   {
      for (UnsignedInt j = 0; j < 6; ++j)
         MessageInterface::ShowMessage("   %.12le", pBar(i, j));
      MessageInterface::ShowMessage("\n");
   }
      MessageInterface::ShowMessage("Q = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
            MessageInterface::ShowMessage("   %.12le", Q(i, j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("dS_dX = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
            MessageInterface::ShowMessage("   %.12le", dS_dX(i, j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("dX_dS = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
            MessageInterface::ShowMessage("   %.12le", dX_dS(i, j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("stm = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
            MessageInterface::ShowMessage("   %.12le", stm(i, j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("Q_s = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
            MessageInterface::ShowMessage("   %.12le", Q_S(i, j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("stmP = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", stmP(i, j));
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("sqrtP = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", sqrtP(i, j));
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("sqrtQ = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", sqrtQ(i, j));
         }
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("pBar = \n");
      for (UnsignedInt i = 0; i < 6; ++i)
      {
         for (UnsignedInt j = 0; j < 6; ++j)
         {
            MessageInterface::ShowMessage("  %.12le", pBar(i, j));
         }
         MessageInterface::ShowMessage("\n");
      }
   #endif
}
   

//------------------------------------------------------------------------------
// void Symmetrize(Rmatrix& mat)
//------------------------------------------------------------------------------
/**
 * Symmetrizes a square Rmatrix
 *
 * @param mat The matrix that is symmetrized
 */
 //------------------------------------------------------------------------------
void PropagationStateManager::Symmetrize(Rmatrix& mat)
{
   Integer size = mat.GetNumRows();

   if (size != mat.GetNumColumns())
   {
      throw PropagatorException("Cannot symmetrize non-square matrices");
   }

   for (Integer i = 0; i < size; ++i)
   {
      for (Integer j = i + 1; j < size; ++j)
      {
         mat(i, j) = 0.5 * (mat(i, j) + mat(j, i));
         mat(j, i) = mat(i, j);
      }
   }
}

//------------------------------------------------------------------------------
// void thinQR(const Rmatrix &mat1, const Rmatrix &mat2, Rmatrix &QR)
//------------------------------------------------------------------------------
/**
 * Performs the "thin" QR decomposition of the compound matrix.
 *
 * @param mat1 The left side of the matrix to concatenate before perorming the "thin" QR decomposition.
 * @param mat2 The right side of the matrix to concatenate before perorming the "thin" QR decomposition.
 * @param &QR The output matrix (passthrough)
 */
 //------------------------------------------------------------------------------
void PropagationStateManager::thinQR(const Rmatrix &mat1, const Rmatrix &mat2, Rmatrix &QR)
{
   QRFactorization qr(false);
   Rmatrix mat(mat1.GetNumRows(), mat1.GetNumColumns() + mat2.GetNumColumns());

   for (UnsignedInt ii = 0U; ii < mat.GetNumRows(); ii++)
   {
      for (UnsignedInt jj = 0U; jj < mat1.GetNumColumns(); jj++)
         mat(ii, jj) = mat1(ii, jj);

      for (UnsignedInt jj = 0U; jj < mat2.GetNumColumns(); jj++)
         mat(ii, jj + mat1.GetNumColumns()) = mat2(ii, jj);
   }

   Rmatrix Q(mat.GetNumColumns(), mat.GetNumRows());
   Rmatrix R(mat.GetNumColumns(), mat.GetNumRows());
   qr.Factor(mat.Transpose(), R, Q);

   Rmatrix N(mat.GetNumRows(), mat.GetNumRows());

   for (UnsignedInt ii = 0U; ii < mat.GetNumRows(); ii++)
      for (UnsignedInt jj = 0U; jj <= ii; jj++)
         N(ii, jj) = R(jj, ii);

   QR = N;
}
//------------------------------------------------------------------------------
// bool SatHasCovariance()
//------------------------------------------------------------------------------
/**
 * Returns true if Covariance is propagated for the input object
 *
 * @param satName The name of the object to check for elements of
 */
 //------------------------------------------------------------------------------
bool PropagationStateManager::SatHasCovariance()
{
   for (Integer i = 0; i < stateMap.size(); i++)
   {
      if (stateMap[i]->elementName == "Covariance" )
      {
         return true;
      }
   }
   return false;
}

//------------------------------------------------------------------------------
// StringArray ElementMapForSat()
//------------------------------------------------------------------------------
/**
 * Returns an element list with STM and Covariance labels for the publisher
 *
 * @param satName The name of the object to check for elements of
 */
 //------------------------------------------------------------------------------
StringArray PropagationStateManager::ElementMapForSat()
{
   StringArray elements;
   for (Integer i = 0; i < stateMap.size(); i++)
   {
      if ((stateMap[i]->elementName != "CartesianState"))
      {
         std::string elementOne;
         std::string elementTwo;
         switch (stateMap[i]->rowIndex)
         {
            case 0: elementOne = "X";
               break;
            case 1: elementOne = "Y";
               break;
            case 2: elementOne = "Z";
               break;
            case 3: elementOne = "Vx";
               break;
            case 4: elementOne = "Vy";
               break;
            case 5: elementOne = "Vz";
               break;
         }
         switch (stateMap[i]->colIndex)
         {
            case 0: elementTwo = "X";
               break;
            case 1: elementTwo = "Y";
               break;
            case 2: elementTwo = "Z";
               break;
            case 3: elementTwo = "Vx";
               break;
            case 4: elementTwo = "Vy";
               break;
            case 5: elementTwo = "Vz";
               break;
         }
         if (stateMap[i]->elementName == "Covariance")
         {
            elements.push_back(stateMap[i]->objectName + ".C" + elementOne + elementTwo);
         }
         else if (stateMap[i]->elementName == "STM")
         {
            elements.push_back(stateMap[i]->objectName + ".STM" + elementOne + elementTwo);
         }
         else
         {
            // Add more options here if more Propagate options are added
            elements.push_back("");
         }
      }
   }
   return elements;
}

//------------------------------------------------------------------------------
// std::string PropagationStateManager::GetSTMParameterName(Integer index)
//------------------------------------------------------------------------------
/**
 * Returns a string with the parameter name of the parameter in the index index
 * 
 *
 * @param Integer index, the index in stmRowMapID of the paramter requested (works with rowIndex/colIndex)
 */
 //------------------------------------------------------------------------------
std::string PropagationStateManager::GetSTMParameterName(Integer index)
{
   std::string retval = stmRowMapObj[index]->GetParameterText(stmRowMapID[index]);
   return retval;
}
