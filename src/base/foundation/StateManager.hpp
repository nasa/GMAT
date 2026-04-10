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
 * Definition of the StateManager base class.  This is the class for state 
 * managers used in GMAT's propagators and solvers.
 */
//------------------------------------------------------------------------------

#ifndef StateManager_hpp
#define StateManager_hpp

#include "GmatState.hpp"
#include "gmatdefs.hpp"
#include "Rvector3.hpp"
#include <map>
#include <vector>

// Example: information of CAN.ErrorModel1.Bias is stored in ListItem such as below
struct GMAT_API ListItem
{
   /**
    * Name of the object owning the parameter being listed
    *
    * For owned clone objects like error models, the model name
    * (e.g. "ErrorModel1") is stored in objectName.
    */
   std::string objectName;
   /// Full heirarchical name of the object (e.g. "CAN.ErrorModel1")
   std::string objectFullName;
   /// Name of the object field (e.g. "Bias") for the current item
   std::string elementName;
   /// Associate used to track cross correlations and referenced objects
   std::string associateName;
   /// The object holding the list item
   GmatBase*   object;
   // Gmat::StateElementId
   /// The parameter ID for the list item
   Integer     elementID;
   /// index of each subelement of parameter
   Integer     subelement;
   /// Parameter Id for the current (sub)element
   Integer     parameterID;
   /// Type for the parameter
   Gmat::ParameterType
               parameterType;
   /// Row index used for the element, when in an array
   Integer     rowIndex;
   /// Length of the row for row or array data
   Integer     rowLength;
   /// Column index used for the element, when in an array
   Integer     colIndex;
   /// Number of elements stored for the item; for arrays, the number of columns (CHECK THIS)
   Integer     length;
   /// true if property forces object updates
   bool        dynamicObjectProperty;
   /// Indicates initial value != 0.0
   bool        nonzeroInit;
   /// Non-zero initial value
   Real        initialValue;
   /// true = Post-superposition step needed
   bool        postDerivativeUpdate;
};


#ifdef EXPORT_TEMPLATES

    // Instantiate STL template classes used in GMAT  
    // This does not create an object. It only forces the generation of all
    // of the members of the listed classes. It exports them from the DLL 
    // and imports them into the .exe file.

    // Fix the vector of ListItem pointers:
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<ListItem*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<ListItem*>;

   // Fix the map of GmatBase* -> StringArray* pointers:
   EXPIMP_TEMPLATE template struct DECLSPECIFIER std::less<GmatBase*>;
   EXPIMP_TEMPLATE template struct DECLSPECIFIER std::pair<GmatBase*, StringArray>;
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<std::pair<GmatBase*, StringArray const>>;
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::_Tmap_traits<GmatBase*, StringArray, std::less<GmatBase*>, std::allocator<std::pair<GmatBase*, StringArray const>>, false>;
#if _MSC_VER < 1700
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::_Tree_nod<std::_Tmap_traits<GmatBase*, StringArray, std::less<GmatBase*>, std::allocator<std::pair<GmatBase*, StringArray const>>, false>>;
#endif
//   EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<std::_Tree_nod<std::_Tmap_traits<GmatBase *,StringArray,std::less<GmatBase *>,std::allocator<std::pair<GmatBase *,const StringArray>>,false>>::_Node >;
   EXPIMP_TEMPLATE template class DECLSPECIFIER std::map<GmatBase*, StringArray*>;
   
#endif



/**
 * The state manager base class.
 */
class GMAT_API StateManager
{
public:
	StateManager(Integer size = 0);
	virtual ~StateManager();
   StateManager(const StateManager& sm);
   StateManager& operator=(const StateManager& sm);
   
   // Abstract methods
   virtual bool SetObject(GmatBase* theObject) = 0;
   virtual bool SetProperty(std::string propName) = 0;
   virtual bool SetProperty(std::string propName, Integer index) = 0;
   virtual bool SetProperty(std::string propName, GmatBase *forObject) = 0;
   virtual bool BuildState() = 0;
   virtual bool MapObjectsToVector() = 0;
   virtual bool MapVectorToObjects() = 0;
   
   virtual Integer GetCount(Gmat::StateElementId elementType = 
                               Gmat::UNKNOWN_STATE);

   virtual bool UpdateState();
   
   /// Get state in J2000BodyMJ2000Eq coordinates (currently it is in EarthMJ2000Eq - GMAT internal coordinates).
   virtual GmatState* GetState();

   virtual Integer GetStateSize();
   
   virtual bool GetStateObjects(ObjectArray& pObjects, 
         UnsignedInt type = Gmat::UNKNOWN_OBJECT);
   
   virtual const StringArray& GetObjectList(std::string ofType = "");
   virtual const std::vector<ListItem*>* GetStateMap();

   virtual Integer  PrepareStateInfoToPublish(StringArray &ownerNames, 
                                              StringArray &elementNames);
   virtual Integer  PrepareCovAndAccelerationInfoToPublish(StringArray &ownerNames, 
                                                           StringArray &elementNames);
   virtual Integer  PrepareAttitudeInfoToPublish(StringArray& ownerNames,
                                                 StringArray& elementNames);
   virtual bool     PrepareStateDataToPublish(Real* publicData, 
                                              Integer publicDataSize, 
                                              StringArray &elementNames);

   virtual bool     PrepareCovAndAccelerationDataToPublish(Real* publishData, 
                                                           Integer publishDataSize, 
                                                           StringArray &elementNames, 
                                                           Integer covStartIndex, 
                                                           bool hasPrecisionTime);

   virtual bool     PrepareAttitudeDataToPublish(Real* publicData,
                                                 Integer publicDataSize,
                                                 StringArray& elementNames,
                                                 Integer attStartIndex,
                                                 bool hasPrecisionTime);

   virtual Rvector3 GetAccelerationOfSpacecraft(GmatBase* obj);

protected:
   /// Size of the managed state vector
   Integer                    stateSize;
   /// The state in J2000BodyMJ2000Eq coordinates (currently it is in EarthMJ2000Eq - GMAT internal coordinates).
   GmatState                  state;
   
   ObjectArray                objects;
   StringArray                objectNames;

   std::vector<Integer>       epochIDs;
   std::map<GmatBase*, StringArray*>  elements;
   GmatBase*                  current;

   std::vector<ListItem*>     stateMap;
};

#endif /*StateManager_hpp*/
