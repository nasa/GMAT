//$Id$
//------------------------------------------------------------------------------
//                                  StateManager
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
 * Imnplementation of the StateManager base class.  This is the class for state 
 * managers used in GMAT's propagators and solvers.
 */
//------------------------------------------------------------------------------

#include "StateManager.hpp"
#include "MessageInterface.hpp"
#include "GmatBase.hpp"
#include "Spacecraft.hpp"
#include <sstream>

//#define DEBUG_STATE_ACCESS
//#define DEBUG_PUBLISHED_INFO
//#define DEBUG_PUBLISHED_DATA

//------------------------------------------------------------------------------
// StateManager(Integer size)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param size The size of the state vector
 */
//------------------------------------------------------------------------------
StateManager::StateManager(Integer size) :
   stateSize      (size),
   state          (size),
   current        (NULL)
{
   objects.clear();
   epochIDs.clear();
   elements.clear();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
StateManager::~StateManager()
{
   // Delete the string arrays in elements
   for (std::map<GmatBase*, StringArray*>::iterator i = elements.begin();
         i != elements.end(); ++i)
      delete i->second;

   for (UnsignedInt i = 0; i < stateMap.size(); ++i)
      delete stateMap[i];
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
StateManager::StateManager(const StateManager& sm) :
   stateSize   (sm.stateSize),
   state       (sm.state),
   objectNames (sm.objectNames),
   current     (NULL),
   stateMap    (sm.stateMap)
{
   objects.clear();
   epochIDs.clear();
   elements.clear();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
StateManager& StateManager::operator=(const StateManager& sm)
{
   if (this != &sm)
   {
      stateSize = sm.stateSize;
      state = sm.state;
      
      // For now, copies start empty.  This may change later.
      objects.clear();
      epochIDs.clear();
      elements.clear();

      current     = NULL;
      objectNames = sm.objectNames;
      stateMap    = sm.stateMap;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// Integer GetCount(Gmat::StateElementId elementType)
//------------------------------------------------------------------------------
/**
 * Returns the number of objects that support the specified type
 * 
 * This default version just returns the total number of unique objects managed
 * by the StateManager
 * 
 * @param elementType ID for the type of state element that is being queried.  
 *                    Gmat::UNKNOWN_STATE, the default, counts the total number
 *                    of objects being managed.
 * 
 * @return The count of the number of objects supporting the type specified
 */
//------------------------------------------------------------------------------
Integer StateManager::GetCount(Gmat::StateElementId elementType)
{
   ObjectArray pObjects;
 
   GetStateObjects(pObjects, Gmat::UNKNOWN_OBJECT);
   Integer count = pObjects.size();

   #ifdef DEBUG_STATE_ACCESS
      MessageInterface::ShowMessage(
            "StateManager::GetCount found %d objects supporting type %d\n",
            count, elementType);
   #endif
      
   return count;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
bool StateManager::UpdateState() 
{
   return true;
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
GmatState* StateManager::GetState()
{
   return &state;
}


Integer StateManager::GetStateSize()
{
   return state.GetSize();
}


//------------------------------------------------------------------------------
// bool GetStateObjects(ObjectArray& pObjects, UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Fills in the pointers to the objects
 *
 * @param pObjects Array that will receive the object pointers
 * @param type The type of objects requested
 *
 * @return true if at least one object pointer was set
 */
//------------------------------------------------------------------------------
bool StateManager::GetStateObjects(ObjectArray& pObjects, UnsignedInt type)
{
   bool retval = false;
   
   if (type == Gmat::UNKNOWN_OBJECT)
   {
      for (ObjectArray::iterator i = objects.begin(); i != objects.end(); ++i)
      {
         if (find(pObjects.begin(), pObjects.end(), (*i)) == pObjects.end())
         {
            pObjects.push_back(*i);
            retval = true;
         }
      }
   }
   else
   {
      for (ObjectArray::iterator i = objects.begin(); i != objects.end(); ++i)
      {
         if ((*i)->IsOfType(type))
         {
            if (find(pObjects.begin(), pObjects.end(), (*i)) == pObjects.end())
            {
               pObjects.push_back(*i);
               retval = true;
            }
         }
      }
   }
   
   return retval;
}


const StringArray& StateManager::GetObjectList(std::string ofType)
{
   return objectNames;
}


const std::vector<ListItem*>* StateManager::GetStateMap()
{
   return &stateMap;
}



Integer StateManager::PrepareStateInfoToPublish(StringArray &ownerNames, StringArray &elementNames)
{
   #ifdef DEBUG_PUBLISHED_INFO
      MessageInterface::ShowMessage("Before: elementNems.size() = %d\n", elementNames.size());
      for (Integer i = 0; i < elementNames.size(); ++i)
         MessageInterface::ShowMessage("ownerNames[%d] = <%s>   elementNames[%d] = <%s>\n",
            i, ownerNames[i].c_str(), i, elementNames[i].c_str());
   #endif

   // Prepare for publishing all elements in propagation state managers
   // Note that: each PropSetup propapagtor[i] has its own propagation state manager

   StringArray stateNames;
   stateNames.push_back("X"); stateNames.push_back("Y"); stateNames.push_back("Z");
   stateNames.push_back("Vx"); stateNames.push_back("Vy"); stateNames.push_back("Vz");
   
   #ifdef DEBUG_PUBLISHED_INFO
      MessageInterface::ShowMessage("   stateMap.size() = %d\n", stateMap.size());
   #endif

   for (Integer i = 0; i < stateMap.size(); ++i)
   {
      ListItem *item = stateMap[i];

      #ifdef DEBUG_PUBLISHED_INFO
         MessageInterface::ShowMessage("item %d: object = <%s>   element = <%s>  subelement = %d\n", 
            i, item->objectName.c_str(), item->elementName.c_str(), item->subelement);
      #endif

         // Add owner's name
         //ownerNames.push_back(item->objectName);
         // Add element name of the owner
         std::stringstream ss;
         if (item->length == 1)
            ss << item->objectName << "." << item->elementName;
         if (item->object->IsOfType(Gmat::FORMATION) && (item->elementName == "CartesianState"))
         {
            Integer componentIndex = (item->subelement - 1) / 6;
            Integer componentElementIndex = item->subelement - 1;
            ownerNames.push_back(item->object->GetStringArrayParameter("Add")[componentIndex]);
            if (componentElementIndex > 5)
               componentElementIndex = componentElementIndex % 6;
            ss << item->object->GetStringArrayParameter("Add")[componentIndex] << "." << stateNames[componentElementIndex];
            //Nothing
         }
         else
         {
            ownerNames.push_back(item->objectName);
            if ((item->object->IsOfType(Gmat::SPACECRAFT)) && (item->elementName == "CartesianState"))
               ss << item->objectName << "." << stateNames[item->subelement - 1];
            else
               ss << item->objectName << "." << item->elementName << "." << item->subelement;
         }
         elementNames.push_back(ss.str());
   }

   // This is an example content of ownerNames and elementNames at this point:
   // ownerNames        elementNames
   // ------------------------------
   // EstSat1           EstSat1.X
   // EstSat1           EstSat1.Y
   // ....              ...
   // EstSat1           EstSat1.VZ
   // EstSat1           EstSat1.CR
   // EstSat2           EstSat2.X
   // EstSat2           EstSat2.Y
   // ....              ...
   // EstSat2           EstSat2.VZ

   Integer numOfElements = elementNames.size();

   #ifdef DEBUG_PUBLISHED_INFO
      MessageInterface::ShowMessage("After: elementNems.size() = %d\n", elementNames.size());
      for (Integer i = 0; i < elementNames.size(); ++i)
         MessageInterface::ShowMessage("ownerNames[%d] = <%s>   elementNames[%d] = <%s>\n", 
            i, ownerNames[i].c_str(), i, elementNames[i].c_str());
   #endif
   
   return numOfElements;
}



Integer StateManager::PrepareCovAndAccelerationInfoToPublish(StringArray &ownerNames, StringArray &elementNames)
{
   StringArray stateElementNames;
   stateElementNames.push_back("X"); stateElementNames.push_back("Y"); stateElementNames.push_back("Z");
   stateElementNames.push_back("Vx"); stateElementNames.push_back("Vy"); stateElementNames.push_back("Vz");

   // 1. Get a list of propagation spacecrafts from propagation state manager
   // Note that: each PropSetup propapagtor[i] has its own propagation state manager
   StringArray scNames;
   for (Integer i = 0; i < stateMap.size(); ++i)
   {
      if (stateMap[i]->object->IsOfType(Gmat::SPACECRAFT))
      {
         bool found = false;
         for (Integer index = 0; index < scNames.size(); ++index)
         {
            if (stateMap[i]->objectName == scNames[index])
            {
               found = true;
               break;
            }
         }
         if (!found)
            scNames.push_back(stateMap[i]->objectName);
      }
      else if (stateMap[i]->object->IsOfType(Gmat::FORMATION))
      {
         bool found = false;
         for (Integer index = 0; index < scNames.size(); ++index)
         {
            if (ownerNames[i] == scNames[index])
            {
               found = true;
               break;
            }
         }
         if (!found)
            scNames.push_back(ownerNames[i]);
      }
   }

   // 2. Add 21 elements of lower left matrix of spacecarft's covariance for each spacecraft
   for (Integer i = 0; i < scNames.size(); ++i)
   {
      for (Integer row = 0; row < 6; ++row)
      {
         for (Integer col = 0; col < 6; ++col)
         {
            if (row < col)
               continue;

            std::stringstream ss;
            ss << scNames[i] << ".C" << stateElementNames[row] << stateElementNames[col];
            ownerNames.push_back(scNames[i]);
            elementNames.push_back(ss.str());
         }
      }
   }

   // 3. Add 3 elements of spacecraft's acceleration for each spacecraft
   for (Integer i = 0; i < scNames.size(); ++i)
   {
      ownerNames.push_back(scNames[i]);
      elementNames.push_back(scNames[i] + ".AccelerationX");
      ownerNames.push_back(scNames[i]);
      elementNames.push_back(scNames[i] + ".AccelerationY");
      ownerNames.push_back(scNames[i]);
      elementNames.push_back(scNames[i] + ".AccelerationZ");
   }

   Integer numOfElements = elementNames.size();
   
   #ifdef DEBUG_PUBLISHED_INFO
      MessageInterface::ShowMessage("\n");
      for (Integer i = 0; i < numOfElements; ++i)
         MessageInterface::ShowMessage("ownerNames[%d] = <%s>   elementNames[%d] = <%s>\n", 
            i, ownerNames[i].c_str(), i, elementNames[i].c_str());
   #endif
   
   return numOfElements;
}

//------------------------------------------------------------------------------
// Integer PrepareAttitudeInfoToPublish(StringArray& ownerNames,
//                                      StringArray& elementNames)
//------------------------------------------------------------------------------
/**
 * Method to generate the info for the attitude elements of the published data
 *
 * @param ownerNames Names of the owners of the attitude elements
 * @param elementNames The names of the attitude elements [Q1, Q2, Q3, Q4]
 * 
 * @return The number of attitude elements for which info was generated
 *
 */
//------------------------------------------------------------------------------
Integer StateManager::PrepareAttitudeInfoToPublish(StringArray& ownerNames,
                                                   StringArray& elementNames)
{
   // 1. Get a list of propagation spacecrafts from propagation state manager
   // Note that: each PropSetup propapagtor[i] has its own propagation state manager
   StringArray scNames;
   for (Integer i = 0; i < stateMap.size(); ++i)
   {
      if (stateMap[i]->object->IsOfType(Gmat::SPACECRAFT))
      {
         bool found = false;
         for (Integer index = 0; index < scNames.size(); ++index)
         {
            if (stateMap[i]->objectName == scNames[index])
            {
               found = true;
               break;
            }
         }
         if (!found)
            scNames.push_back(stateMap[i]->objectName);
      }
      else if (stateMap[i]->object->IsOfType(Gmat::FORMATION))
      {
         bool found = false;
         for (Integer index = 0; index < scNames.size(); ++index)
         {
            if (ownerNames[i] == scNames[index])
            {
               found = true;
               break;
            }
         }
         if (!found)
            scNames.push_back(ownerNames[i]);
      }
   }

   // 2. Add 4 quaternion elements of spacecraft's attitude
   for (Integer i = 0; i < scNames.size(); ++i)
   {
      ownerNames.push_back(scNames[i]);
      elementNames.push_back(scNames[i] + ".Q1");
      ownerNames.push_back(scNames[i]);
      elementNames.push_back(scNames[i] + ".Q2");
      ownerNames.push_back(scNames[i]);
      elementNames.push_back(scNames[i] + ".Q3");
      ownerNames.push_back(scNames[i]);
      elementNames.push_back(scNames[i] + ".Q4");
   }

   Integer numOfElements = elementNames.size();

   return numOfElements;
}


bool StateManager::PrepareStateDataToPublish(Real* publishData, Integer publishDataSize, StringArray& elementNames)
{
   StringArray stateNames;
   stateNames.push_back("X"); stateNames.push_back("Y"); stateNames.push_back("Z");
   stateNames.push_back("Vx"); stateNames.push_back("Vy"); stateNames.push_back("Vz");

   //Integer Index = len;

   // Publish value of each element handling by state manager sm 
   Integer size = state.GetSize();
   for (Integer i = 0; i < stateMap.size(); ++i)
   {
      ListItem *item = stateMap[i];

      #ifdef DEBUG_PUBLISHED_DATA
      MessageInterface::ShowMessage("item %d: object = <%s>   element = <%s>  subelement = %d\n",
         i, item->objectName.c_str(), item->elementName.c_str(), item->subelement);
      #endif

      std::stringstream ss;
      if (item->length == 1)
         ss << item->objectName << "." << item->elementName;
      if (item->object->IsOfType(Gmat::FORMATION) && (item->elementName == "CartesianState"))
      {
         Integer componentIndex = (item->subelement - 1) / 6;
         Integer componentElementIndex = item->subelement - 1;
         if (componentElementIndex > 5)
            componentElementIndex = componentElementIndex % 6;
         ss << item->object->GetStringArrayParameter("Add")[componentIndex] << "." << stateNames[componentElementIndex];
      }
      else
      {
         if ((item->object->IsOfType(Gmat::SPACECRAFT)) && (item->elementName == "CartesianState"))
            ss << item->objectName << "." << stateNames[item->subelement - 1];
         else
            ss << item->objectName << "." << item->elementName << "." << item->subelement;
      }
      std::string name = ss.str();
      
      bool found = false;
      Integer idx = 0;
      for (; idx < elementNames.size(); ++idx)
      {
         if (name == elementNames[idx])
         {
            found = true;
            break;
         }
      }

      if (found)
         publishData[idx] = state[i];
      // Skip filling data to publishData when element is not in publishing list.


      //if (index >= publishDataSize)
      //{
      //   std::stringstream ss;
      //   ss << "Error: Data is overflowed in StateManager::PrepareStateDataToPublish()"
      //      << " function when data are filled publishData array: publishDataSize = " 
      //      << publishDataSize << ". Fill data for stateMap[" << i 
      //      << "]: object name = <" << stateMap[i]->object->GetName()
      //      << ">     elementName = <" << stateMap[i]->elementName << ">   subelement = <" 
      //      << stateMap[i]->subelement << ">\n";
      //   throw GmatBaseException(ss.str());
      //}
      //
      //publishData[index] = state[i];
      //++index;
   }
   //len = index;

   #ifdef DEBUG_PUBLISHED_DATA
   for (Integer i = len; i < index; ++i)
      MessageInterface::ShowMessage("publishData[%d] = %.12lf\n", i, publishData[i]);
   #endif
   return true;
}



bool StateManager::PrepareCovAndAccelerationDataToPublish(Real* publishData, Integer publishDataSize, 
   StringArray& elementNames, Integer covStartIndex, bool hasPrecisionTime)
{
   Integer index = covStartIndex;

   // 1. Specify all object in state map
   ObjectArray objs;
   for (Integer j = 0; j < stateMap.size(); ++j)
   {
      GmatBase* obj = stateMap[j]->object;
      bool found = false;
      for (Integer k = 0; k < objs.size(); ++k)
      {
         if (obj == objs[k])
         {
            found = true;
            break;
         }
      }
      if (found)
         continue;
      else
         objs.push_back(obj);

      if (obj->IsOfType(Gmat::FORMATION) && !found)
      {
         if (obj);
         ObjectArray fromationComponents = obj->GetRefObjectArray("Spacecraft");
         for (Integer k = 0; k < fromationComponents.size(); k++)
         {
            objs.push_back(fromationComponents[k]);
         }
      }
   }

   // 2. For each spacecraft, publish covariance and acceleration
   for (Integer j = 0; j < objs.size(); ++j)
   {
      if (objs[j]->IsOfType(Gmat::SPACECRAFT))
      {
         Spacecraft *sc = (Spacecraft*)objs[j];

         // 2.1. Specify covariance and publish it
         // Specify object's State Transition Mamtrix STM(t0,t)
         Rmatrix phi = sc->GetRmatrixParameter("FullSTM");

         // Specify object's covariance cov(t0) at time t0
         Rmatrix covt0 = sc->GetRmatrixParameter("OrbitErrorCovariance");
         Rmatrix Pt0;
         Pt0.SetSize(phi.GetNumRows(), phi.GetNumColumns(), true);
         for (Integer row = 0; row < phi.GetNumRows(); ++row)
         {
            for (Integer col = 0; col < phi.GetNumColumns(); ++col)
            {
               if ((row < covt0.GetNumRows()) && (col < covt0.GetNumColumns()))
                  Pt0(row, col) = covt0(row, col);
               else
                  continue;
            }
         }

         // Specify object's covariance P(t) at time t in MJ2000Eq axis: 
         // P(t) = STM(t,t0) * P(t0) * STM(t,t0).Transpose
         //Rmatrix Pt = phi * Pt0 * phi.Transpose();
         // At here, matrix Pt0 is matrix P(t).
         Rmatrix Pt = Pt0;

         // @todo: if wanted P(t) in other axis, it needs to have 6x6 rotation matrix R = [Rot     0  ]
         //                                                                               [RotDot  Rot]
         //        and apply equation: Pnew(t) = R * P(t)

         // Publish P(t). Only 21 covariance elements for state (x,y,z,vx,vy,vz) in lower left triangular matrix are extracted and published. 
         for (Integer row = 0; row < 6; ++row)
         {
            for (Integer col = 0; col < 6; ++col)
            {
               if (row < col)
                  continue;

               if (index >= publishDataSize)
               {
                  std::stringstream ss;
                  ss << "Error: Data is overflowed in StateManger::PrepareCovAndAccelerationDataToPublish()"
                     << " function when data are filled publishData array: publishDataSize = "
                     << publishDataSize << "\n";
                  throw GmatBaseException(ss.str());
               }

               publishData[index] = Pt(row, col);
               ++index;
            }
         }

         // 2.2. Publish acceleration
         // Specify acceleration in spacecraft's coordinate origin MJ2000Eq coordinate system
         Rvector3 accel;
         if (hasPrecisionTime)
         {
            GmatTime epochGT = state.GetEpochGT();
            accel = sc->GetMJ2000Acceleration(epochGT);
         }
         else
         {
            GmatEpoch epoch = state.GetEpoch();
            accel = sc->GetMJ2000Acceleration(epoch);
         }

         // Publish 
         for (Integer k = 0; k < 3; ++k)
         {
            if (index >= publishDataSize)
            {
               std::stringstream ss;
               ss << "Error: Data is overflowed in StateManger::PrepareCovAndAccelerationDataToPublish()"
                  << " function when data are filled publishData array: publishDataSize = "
                  << publishDataSize << "\n";
               throw GmatBaseException(ss.str());
            }

            publishData[index] = accel[k];
            ++index;
         }
      }
   }

   #ifdef DEBUG_PUBLISHED_DATA
      for (Integer i = covStartIndex; i < index; ++i)
         MessageInterface::ShowMessage("publishData[%d] = %.12lf\n", i, publishData[i]);
   #endif

   return true;
}

//------------------------------------------------------------------------------
// bool PrepareAttitudeDataToPublish(Real* publishData,
//                                   Integer publishDataSize,
//                                   StringArray& elementNames,
//                                   Integer attStartIndex,
//                                   bool hasPrecisionTime)
//------------------------------------------------------------------------------
/**
 * Method to prepare and add attitude data to the pusblish data array
 * 
 * @param publishData      The data array that attitude data will be added to
 * @param publishDataSize  The size of the data array
 * @param elementNames     The names for each of the data elements
 * @param attStartIndex    The index in the data array for which attitude data 
 *                         will be added to
 * @param hasPrecisionTime Whether GMAT precision time is in use
 * 
 * @return Whether data was successfully added
 *
 */
//------------------------------------------------------------------------------
bool StateManager::PrepareAttitudeDataToPublish(Real* publishData,
                                                Integer publishDataSize,
                                                StringArray& elementNames,
                                                Integer attStartIndex,
                                                bool hasPrecisionTime)
{
   Integer index = attStartIndex;

   // 1. Specify all object in state map
   ObjectArray objs;
   for (Integer j = 0; j < stateMap.size(); ++j)
   {
      GmatBase* obj = stateMap[j]->object;
      bool found = false;
      for (Integer k = 0; k < objs.size(); ++k)
      {
         if (obj == objs[k])
         {
            found = true;
            break;
         }
      }
      if (found)
         continue;
      else
         objs.push_back(obj);

      if (obj->IsOfType(Gmat::FORMATION) && !found)
      {
         if (obj);
         ObjectArray fromationComponents = obj->GetRefObjectArray("Spacecraft");
         for (Integer k = 0; k < fromationComponents.size(); k++)
         {
            objs.push_back(fromationComponents[k]);
         }
      }
   }

   // 2. For each spacecraft, publish quaternions
   for (Integer j = 0; j < objs.size(); ++j)
   {
      if (objs[j]->IsOfType(Gmat::SPACECRAFT))
      {
         Spacecraft* sc = (Spacecraft*)objs[j];
         Attitude *att = (Attitude*)sc->GetRefObject(Gmat::ATTITUDE, "Attitude");
         Rvector quat;
         if (hasPrecisionTime)
         {
            GmatTime epochGT = state.GetEpochGT();
            quat = att->GetQuaternion(epochGT.GetMjd());
         }
         else
         {
            GmatEpoch epoch = state.GetEpoch();
            quat = att->GetQuaternion(epoch);
         }

         // Publish 
         for (Integer k = 0; k < 4; ++k)
         {
            if (index >= publishDataSize)
            {
               std::stringstream ss;
               ss << "Error: Data is overflowed in StateManger::PrepareCovAndAccelerationDataToPublish()"
                  << " function when data are filled publishData array: publishDataSize = "
                  << publishDataSize << "\n";
               throw GmatBaseException(ss.str());
            }

            publishData[index] = quat[k];
            ++index;
         }
      }
   }

   return true;
}

Rvector3 StateManager::GetAccelerationOfSpacecraft(GmatBase* obj)
{
   if (!obj->IsOfType(Gmat::SPACECRAFT))
      throw GmatBaseException("Error: Input of GetAccelerationOfSpacecraft is not a Spacecraft object\n");

   
   Integer cartesianStateID = ((Spacecraft*)obj)->GetParameterID("CartesianState");
   Integer i = 0;
   for (; i < stateMap.size(); ++i)
   {
      if ((stateMap[i]->object == obj) && (stateMap[i]->parameterID == (cartesianStateID + 3)))
         break;
   }

   Rvector3 acceleration;
   for (Integer k = 0; k < 3; ++k)
      acceleration[k] = state.GetStateDot()[i + k];

   return acceleration;
}

