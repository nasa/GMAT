//$Id: ObjectInitializer.cpp 9457 2011-04-21 18:40:34Z lindajun $
//------------------------------------------------------------------------------
//                                  ObjectInitializer
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
// number NNG06CCA54C
//
// Author: Wendy Shoan
// Created: 2008.06.11
//
// Original code from the Sandbox.
// Author: Darrel J. Conway
//
/**
 * Implementation for the ObjectInitializer class.
 * This class initializes objects of the specified LocalObjectStore and,
 * on option, GlobalObjectStore.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "GmatGlobal.hpp"
#include "Moderator.hpp"
#include "ObjectInitializer.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "GmatBaseException.hpp"
#include "SubscriberException.hpp"
#include "Publisher.hpp"
#include "RHSEquation.hpp"
#include "MathTree.hpp"
#include "Validator.hpp"            // For equation wrapper setup
#include "RealUtilities.hpp"

#include <algorithm>                // for find()

//#define DEBUG_OBJECT_INITIALIZER
//#define DEBUG_OBJECT_INITIALIZER_DETAILED
//#define DEBUG_SUBSCRIBER
//#define DEBUG_INITIALIZE_OBJ
//#define DEBUG_INITIALIZE_CS
//#define DEBUG_BUILD_ASSOCIATIONS
//#define DEBUG_FIND_OBJ
//#define DEBUG_OBJECT_MAP
//#define DEBUG_Z_ORDER
//#define DEBUG_BASE_EPOCH

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif
//#ifndef DEBUG_TRACE
//#define DEBUG_TRACE
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif
#ifdef DEBUG_TRACE
#include <ctime>                 // for clock()
#endif

//#define API_DEBUG

#ifdef API_DEBUG
   #include <iostream>
   using namespace std;
#endif

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
ObjectInitializer::ObjectInitializer(SolarSystem *solSys, ObjectMap *objMap,
                                     ObjectMap *globalObjMap, CoordinateSystem *intCS, 
                                     bool useGOS, bool fromFunction) :
   ss                   (solSys),
   LOS                  (objMap),
   GOS                  (globalObjMap),
   mod                  (NULL),
   internalCS           (intCS),
   includeGOS           (useGOS),
   registerSubscribers  (false),
   inFunction           (fromFunction),
   pCreateWidget        (NULL)
{
   mod = Moderator::Instance();
   publisher = Publisher::Instance();
}

//------------------------------------------------------------------------------
// ObjectInitializer(const ObjectInitializer &objInit)
//------------------------------------------------------------------------------
ObjectInitializer::ObjectInitializer(const ObjectInitializer &objInit) :
   ss                   (objInit.ss),
   LOS                  (objInit.LOS),
   GOS                  (objInit.GOS),
   mod                  (NULL),
   internalCS           (objInit.internalCS),
   includeGOS           (objInit.includeGOS),
   registerSubscribers  (objInit.registerSubscribers),
   inFunction           (objInit.inFunction),
   pCreateWidget        (NULL)
{
   mod = Moderator::Instance();
   publisher = Publisher::Instance();
}

//------------------------------------------------------------------------------
// ObjectInitializer& operator= (const ObjectInitializer &objInit)
//------------------------------------------------------------------------------
ObjectInitializer& ObjectInitializer::operator= (const ObjectInitializer &objInit)
{
   if (&objInit != this)
   {
      ss                  = objInit.ss;
      LOS                 = objInit.LOS;
      GOS                 = objInit.GOS;
      mod                 = objInit.mod;
      internalCS          = objInit.internalCS;
      publisher           = objInit.publisher;
      includeGOS          = objInit.includeGOS;
      registerSubscribers = objInit.registerSubscribers;
      inFunction          = objInit.inFunction;
      pCreateWidget       = NULL;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
ObjectInitializer::~ObjectInitializer()
{
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *solSys)
//------------------------------------------------------------------------------
void ObjectInitializer::SetSolarSystem(SolarSystem *solSys)
{
   ss = solSys;
}

//------------------------------------------------------------------------------
// void SetObjectMap(ObjectMap *objMap)
//------------------------------------------------------------------------------
void ObjectInitializer::SetObjectMap(ObjectMap *objMap)
{
   LOS = objMap;
}

//------------------------------------------------------------------------------
// void SetInternalCoordinateSystem(CoordinateSystem* intCS)
//------------------------------------------------------------------------------
void ObjectInitializer::SetInternalCoordinateSystem(CoordinateSystem* intCS)
{
   #ifdef DEBUG_OI_SET
   MessageInterface::ShowMessage
      ("in OI:SetInternalCoordinateSystem(), intCS=<%p>\n", intCS);
   #endif
   if (intCS != NULL)
      internalCS = intCS;
}

//------------------------------------------------------------------------------
// bool InitializeObjects(bool registerSubs, UnsignedInt objType,
//                        StringArray *unusedGOL)
//------------------------------------------------------------------------------
/*
 * Initializes objects
 *
 * @param  registerSubs  Registers subscribers if set to true (false)
 * @param  objType       Initializes only this object type, if objType is
 *                       UNKNOWN_OBJECT it initializes all objects (UNKNOWN_OBJECT)
 * @param  unusedGOL     Unused global object list which tells to ignore object
 *                       checking (NULL)
 */
//------------------------------------------------------------------------------
bool ObjectInitializer::InitializeObjects(bool registerSubs,
                                          UnsignedInt objType,
                                          StringArray *unusedGOL)
{
   #ifdef DEBUG_TRACE
   static Integer callCount = 0;
   callCount++;
   clock_t t1 = clock();
   MessageInterface::ShowMessage
      (">>>>> CALL TRACE: ObjectInitializer::InitializeObjects() entered, Count = %d, objType = %d\n",
       callCount, objType);
   #endif
   
   #ifdef DEBUG_INITIALIZE_OBJ
   MessageInterface::ShowMessage
      ("ObjectInitializer::InitializeObjects() entered, registerSubs = %s, "
       "objType = %d, objTypeStr = '%s', inFunction = %d\n", registerSubs ? "true" : "false",
       objType, GmatBase::GetObjectTypeString(objType).c_str(), inFunction);
   #endif
   
   #ifdef DEBUG_OBJECT_MAP
   ShowObjectMaps("In ObjectInitializer::InitializeObjects()");
   #endif
   
#ifdef API_DEBUG
   cout << "Entered ObjectInitializer::InitializeObjects() with "
        << LOS->size() << " objects to init\n";
#endif

   // First check for NULL object pointer in the map to avoid crash down the road
   std::map<std::string, GmatBase *>::iterator iter;

   if (LOS)
   {
      for (iter = LOS->begin(); iter != LOS->end(); ++iter)
      {
#ifdef API_DEBUG
   cout << "   Checking pointer for "
        << iter->first << "\n";
#endif
            if (iter->second == NULL)
               throw GmatBaseException
                  ("ObjectInitializer::InitializeObjects() cannot continue "
                   "due to \"" + iter->first + "\" has NULL object pointer in LOS");
      }
   }

   if (GOS)
   {
      for (iter = GOS->begin(); iter != GOS->end(); ++iter)
      {
         if (iter->second == NULL)
            throw GmatBaseException
               ("ObjectInitializer::InitializeObjects() cannot continue "
                "due to \"" + iter->first + "\" has NULL object pointer in GOS");
      }
   }
   
   registerSubscribers =  registerSubs;
   std::map<std::string, GmatBase *>::iterator omi;
   std::string oName;
   std::string j2kName;

#ifdef API_DEBUG
   cout << "Beginning object initialization" << endl;
#endif
   
   if (objType == Gmat::UNKNOWN_OBJECT)
   {
      #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("About to Initialize Internal Objects ...\n");
      #endif
      
#ifdef API_DEBUG
   cout << "   Internal object initialization\n";
#endif
      InitializeInternalObjects();
      
      #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("Internal Objects Initialized ...\n");
      #endif
   }
   
   // Set J2000 Body for all SpacePoint derivatives before anything else
   // NOTE - at this point, everything should be in the SandboxObjectMap,
   // and the GlobalObjectMap should be empty
   #ifdef DEBUG_OBJECT_INITIALIZER
      std::map<std::string, GmatBase *>::iterator omIter;
      MessageInterface::ShowMessage("--- Right before setting the J2000 body ---\n");
      MessageInterface::ShowMessage("The Object Map contains:\n");
      for (omIter = LOS->begin(); omIter != LOS->end(); ++omIter)
         MessageInterface::ShowMessage("   %s of type %s\n",
               (omIter->first).c_str(), ((omIter->second)->GetTypeName()).c_str());
      MessageInterface::ShowMessage("The Global Object Map contains:\n");
      for (omIter = GOS->begin(); omIter != GOS->end(); ++omIter)
         MessageInterface::ShowMessage("   %s of type %s\n",
               (omIter->first).c_str(), ((omIter->second)->GetTypeName()).c_str());
   #endif
      
#ifdef API_DEBUG
   cout << "   Setting J2K body\n";
#endif
   SetObjectJ2000Body(LOS);
   
   if (includeGOS)
      SetObjectJ2000Body(GOS);
   
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("J2000 Body set ...\n");
   #endif
   // This list may need to be updated more thoroughly
   // The initialization order is:
   //
   //  1. CoordinateSystems
   //  2. Hardware
   //  3. Spacecraft and Ground Stations
   //  NOTE: Measurement participant pointers must initialize before models.
   //        In the current code, that means spacecraft and ground stations, but
   //        the list might grow
   //  4. Error Models
   //  5. Data Filters
   //  6. Measurement Models
   //  7. System Parameters
   //  8. Parameters
   //  9. Subscribers
   //  10. Remaining Objects
      
#ifdef API_DEBUG
   cout << "   CoordinateSystem Init\n";
#endif
   // Coordinate Systems
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::COORDINATE_SYSTEM)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize CoordinateSystems in LOS\n");
      #endif
      InitializeObjectsInTheMap(LOS, Gmat::COORDINATE_SYSTEM);
      
      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize CoordinateSystems in GOS\n");
         #endif
         InitializeObjectsInTheMap(GOS, Gmat::COORDINATE_SYSTEM, true, unusedGOL);
      }
   }
   
#ifdef API_DEBUG
   cout << "   Calculated point Init\n";
#endif
   // Calculated Points
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::CALCULATED_POINT)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize CalculatedPoints in LOS\n");
      #endif
      InitializeObjectsInTheMap(LOS, Gmat::CALCULATED_POINT);
      
      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize CalculatedPoints in GOS\n");
         #endif
         InitializeObjectsInTheMap(GOS, Gmat::CALCULATED_POINT, true, unusedGOL);
      }
   }
   
   
   // Plate
   // Handle Plate objects
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::PLATE)
   {
#ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize Plate in LOS\n");
#endif
      InitializeObjectsInTheMap(LOS, Gmat::PLATE);

      if (includeGOS)
      {
#ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize Plate in GOS\n");
#endif
         InitializeObjectsInTheMap(GOS, Gmat::PLATE);
      }
   }

#ifdef API_DEBUG
   cout << "   Burn Init\n";
#endif
   // Burns
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::BURN)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize Burns in LOS\n");
      #endif
      InitializeObjectsInTheMap(LOS, Gmat::BURN);
      
      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize Burns in GOS\n");
         #endif
         InitializeObjectsInTheMap(GOS, Gmat::BURN, true, unusedGOL);
      }
   }

   // Hardware - moved earlier so FOVs are set
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::HARDWARE)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize Hardware in LOS\n");
      #endif
      InitializeObjectsInTheMap(LOS, Gmat::HARDWARE);

      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize Hardware in GOS\n");
         #endif
         InitializeObjectsInTheMap(GOS, Gmat::HARDWARE, true, unusedGOL);
      }
   }

#ifdef API_DEBUG
   cout << "   Spacecraft Init\n";
#endif
   // Spacecraft
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::SPACECRAFT ||
       objType == Gmat::GROUND_STATION || objType == Gmat::REGION)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize Spacecraft in LOS\n");
      #endif
      InitializeObjectsInTheMap(LOS, Gmat::SPACECRAFT);
      InitializeObjectsInTheMap(LOS, Gmat::GROUND_STATION);
      InitializeObjectsInTheMap(LOS, Gmat::REGION);
      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize Spacecraft in GOS\n");
         #endif
         InitializeObjectsInTheMap(GOS, Gmat::SPACECRAFT);
         InitializeObjectsInTheMap(GOS, Gmat::GROUND_STATION);
         InitializeObjectsInTheMap(GOS, Gmat::REGION);
      }
   }

   // Don't set base epoch inside of function calls
   if (!inFunction)
   {
      // Find and set the baseline epoch data
      GmatEpoch earliest    = 1000000.0;   // Start too large
      GmatEpoch baseDefault = 21545.000000397937;
      Real      epochTol    = 0.0000004;
      for (omi = LOS->begin(); omi != LOS->end(); ++omi)
      {
         GmatBase *obj = omi->second;
         if (obj->IsOfType(Gmat::SPACECRAFT))
         {
            GmatEpoch ep = ((Spacecraft*)(obj))->GetEpoch();
            #ifdef DEBUG_BASE_EPOCH
               MessageInterface::ShowMessage("--- (LOS) earliest is %.12lf\n",
                                             earliest);
               MessageInterface::ShowMessage("--- (LOS) ep is %.12lf\n", ep);
            #endif
            // Find the earliest Spacecraft time but omit the standard default epoch
            if ((ep < earliest) && !(GmatMathUtil::IsEqual(ep, baseDefault, epochTol)))
               earliest = ep;
         }
      }
      if (includeGOS)
      {
         for (omi = GOS->begin(); omi != GOS->end(); ++omi)
         {
            GmatBase *obj = omi->second;
            if (obj->IsOfType(Gmat::SPACECRAFT))
            {
               GmatEpoch ep = ((Spacecraft*)(obj))->GetEpoch();
               #ifdef DEBUG_BASE_EPOCH
                  MessageInterface::ShowMessage("--- (GOS) earliest is %.12lf\n",
                                                earliest);
                  MessageInterface::ShowMessage("--- (GOS) ep is %.12lf\n", ep);
               #endif
               // Find the earliest Spacecraft time but omit the standard default epoch
               if ((ep < earliest) && !(GmatMathUtil::IsEqual(ep, baseDefault, epochTol)))
                  earliest = ep;
            }
         }
      }

      if (earliest != 1000000.0)
      {
         #ifdef DEBUG_BASE_EPOCH
            MessageInterface::ShowMessage(
                            "------ Setting Base epoch to earliest = %.12lf\n",
                            earliest);
         #endif
         GmatGlobal::Instance()->SetBaseEpoch(earliest);
      }
      else
      {
         #ifdef DEBUG_BASE_EPOCH
            MessageInterface::ShowMessage(
                            "------ Setting Base epoch to baseDefault (%12.10f)\n",
                                          baseDefault);
         #endif
         GmatGlobal::Instance()->SetBaseEpoch(baseDefault);
      }
      #ifdef DEBUG_BASE_EPOCH
         MessageInterface::ShowMessage("Earliest is %.12lf\n", earliest);
         MessageInterface::ShowMessage("Base epoch is %.12lf\n",
               GmatGlobal::Instance()->GetBaseEpoch());
      #endif
   }

#ifdef API_DEBUG
   cout << "   Nav Init: Error Models\n";
#endif
   // ErrorModel
   // Handle ErrorModel objects
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::ERROR_MODEL)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize ErrorModel in LOS\n");
      #endif
      InitializeObjectsInTheMap(LOS, Gmat::ERROR_MODEL);

      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize ErrorModel in GOS\n");
         #endif
         InitializeObjectsInTheMap(GOS, Gmat::ERROR_MODEL);
      }
   }

#ifdef API_DEBUG
   cout << "   Nav Init: Data Filters\n";
#endif
   // DataFilter
   // Handle DataFilter objects
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::DATA_FILTER)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize DataFilter in LOS\n");
      #endif
      InitializeObjectsInTheMap(LOS, Gmat::DATA_FILTER);

      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize DataFilter in GOS\n");
         #endif
         InitializeObjectsInTheMap(GOS, Gmat::DATA_FILTER);
      }
   }

#ifdef API_DEBUG
   cout << "   Nav Init: Measurement Models\n";
#endif
   // MeasurementModel
   // Measurement Models must init before the Estimators/Simulator, so do next
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::MEASUREMENT_MODEL)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize MeasurementModels in LOS\n");
      #endif
      InitializeObjectsInTheMap(LOS, Gmat::MEASUREMENT_MODEL);
      
      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize MeasurementModels in GOS\n");
         #endif
         InitializeObjectsInTheMap(GOS, Gmat::MEASUREMENT_MODEL);
      }
   }
   
   //// Like Measurement Models, TrackingData objects must init before the
   //// Estimators/Simulator, so do next
   //if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::TRACKING_DATA)
   //{
   //   #ifdef DEBUG_INITIALIZE_OBJ
   //   MessageInterface::ShowMessage("--- Initialize TrackingData in LOS\n");
   //   #endif
   //   InitializeObjectsInTheMap(LOS, Gmat::TRACKING_DATA);

   //   if (includeGOS)
   //   {
   //      #ifdef DEBUG_INITIALIZE_OBJ
   //      MessageInterface::ShowMessage("--- Initialize TrackingData in GOS\n");
   //      #endif
   //      InitializeObjectsInTheMap(GOS, Gmat::TRACKING_DATA);
   //   }
   //}

   //// Handle TrackingSystem objects before the Estimators/Simulator as well
   //if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::TRACKING_SYSTEM)
   //{
   //   #ifdef DEBUG_INITIALIZE_OBJ
   //   MessageInterface::ShowMessage("--- Initialize TrackingSystem in LOS\n");
   //   #endif
   //   InitializeObjectsInTheMap(LOS, Gmat::TRACKING_SYSTEM);

   //   if (includeGOS)
   //   {
   //      #ifdef DEBUG_INITIALIZE_OBJ
   //      MessageInterface::ShowMessage("--- Initialize TrackingSystem in GOS\n");
   //      #endif
   //      InitializeObjectsInTheMap(GOS, Gmat::TRACKING_SYSTEM);
   //   }
   //}

#ifdef API_DEBUG
   cout << "   System Parameters\n";
#endif
   // System Parameters, such as sat.X
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::PARAMETER)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize Sytem Parameters in LOS\n");
      #endif
      InitializeSystemParameters(LOS);
      
      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize Sytem Parameters in LOS\n");
         #endif
         InitializeSystemParameters(GOS);
      }
   }
   
#ifdef API_DEBUG
   cout << "   Variables\n";
#endif
   // Variables
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::VARIABLE )
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize Variables in LOS\n");
      #endif
      InitializeObjectsInTheMap(LOS, Gmat::VARIABLE);
      
      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize Variables in GOS\n");
         #endif
         InitializeObjectsInTheMap(GOS, Gmat::VARIABLE);
      }
   }
   
#ifdef API_DEBUG
   cout << "   Strings\n";
#endif
   // Strings
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::STRING)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize Strings in LOS\n");
      #endif
      InitializeObjectsInTheMap(LOS, Gmat::STRING);
      
      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize String in GOS\n");
         #endif
         InitializeObjectsInTheMap(GOS, Gmat::STRING);
      }
   }
   
#ifdef API_DEBUG
   cout << "   Subscribers\n";
#endif
   // Subscribers
   if (objType == Gmat::UNKNOWN_OBJECT || objType == Gmat::SUBSCRIBER)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize Subscriber in LOS\n");
      #endif
      InitializeSubscribers(LOS);
      
      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize Subscriber in GOS\n");
         #endif
         InitializeSubscribers(GOS);
      }
   }
   
#ifdef API_DEBUG
   cout << "   Everything else\n";
#endif
   // All other objects
   if (objType == Gmat::UNKNOWN_OBJECT)
   {
      #ifdef DEBUG_INITIALIZE_OBJ
      MessageInterface::ShowMessage("--- Initialize All other objects in LOS\n");
      #endif
      InitializeAllOtherObjects(LOS);
      
      if (includeGOS)
      {
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage("--- Initialize All other objects in GOS\n");
         #endif
         InitializeAllOtherObjects(GOS);
      }
   }
   
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("ObjectInitializer: Objects Initialized ...\n");
   #endif
      
   #ifdef DEBUG_TRACE
   clock_t t2 = clock();
   MessageInterface::ShowMessage
      (">>>>> CALL TRACE: ObjectInitializer::InitializeObjects() leaving, Count = %d, "
       "Run Time: %f seconds\n", callCount, (Real)(t2-t1)/CLOCKS_PER_SEC);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void SetObjectJ2000Body(ObjectMap *objMap)
//------------------------------------------------------------------------------
/*
 * Sets J2000Body pointer to SpacePoint objects in the map
 */
//------------------------------------------------------------------------------
void ObjectInitializer::SetObjectJ2000Body(ObjectMap *objMap)
{
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = objMap->begin(); omi != objMap->end(); ++omi)
   {
      GmatBase *obj = omi->second;
      if (obj->IsOfType(Gmat::SPACE_POINT))
      {
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage(
               "Setting J2000 Body for %s in the ObjectInitializer\n",
               obj->GetName().c_str());
         #endif
         SpacePoint *spObj = (SpacePoint *)obj;
         SpacePoint* j2k = FindSpacePoint(spObj->GetJ2000BodyName());
         if (j2k)
         {
            spObj->SetSolarSystem(ss);
            spObj->SetJ2000Body(j2k);
         }
         else
            throw GmatBaseException("ObjectInitializer did not find the Spacepoint \"" +
               spObj->GetJ2000BodyName() + "\"");
      }
   }
}


//------------------------------------------------------------------------------
// void InitializeObjectsInTheMap(ObjectMap *objMap, UnsignedInt objType, ...)
//------------------------------------------------------------------------------
/*
 * Initializes specific types of objects in the map. If objType is UNDEFINED_OBJECT,
 * it will check for objTypeStr to retrieve objects.
 *
 * @param objMap the object map to be used for retrieving objects
 * @param objType the object type to be used for retrieving objects
 */
//------------------------------------------------------------------------------
void ObjectInitializer::InitializeObjectsInTheMap(ObjectMap *objMap,
                                                  UnsignedInt objType,
                                                  bool usingGOS,
                                                  StringArray *unusedGOL)
{
   std::string objTypeStr = GmatBase::GetObjectTypeString(objType).c_str();
   #ifdef DEBUG_INITIALIZE_OBJ
   MessageInterface::ShowMessage
      ("InitializeObjectsInTheMap() entered, objMap=<%p>, objType=%d, "
       "objTypeStr='%s', inFunction=%d\n", objMap, objType, objTypeStr.c_str(),
       inFunction);
   #endif
   
   std::string objName;
   
   try
   {
      std::map<std::string, GmatBase *>::iterator omi;
      for (omi = objMap->begin(); omi != objMap->end(); ++omi)
      {
         GmatBase *obj = omi->second;
         
         #ifdef DEBUG_INITIALIZE_OBJ
         MessageInterface::ShowMessage
            ("   obj = <%p>[%s]'%s'\n", obj, obj ? obj->GetTypeName().c_str() : "NULL",
             obj ? obj->GetName().c_str() : "NULL");
         #endif
         
         if (obj == NULL)
            throw GmatBaseException
               ("Cannot initialize NULL pointer of \"" + omi->first + "\" object");
         
         objName = obj->GetName();
         
         if (objType != Gmat::UNKNOWN_OBJECT)
         {
            if (obj->IsOfType(objType))
            {
               #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage
                  ("Initializing <%p>[%s]'%s'\n", obj, obj->GetTypeName().c_str(),
                   obj->GetName().c_str());
               #endif
               if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
               {
                  BuildReferences(obj);
                  InitializeCoordinateSystem((CoordinateSystem *)obj);
                  obj->Initialize();
               }
               else if (obj->IsOfType(Gmat::SPACECRAFT))
               {
                  BuildReferences(obj);
                  
                  // Put this in try/catch block for function (LOJ: 2015.02.25)
                  // When a spacecraft which already has all the hardware
                  // assocation is passed to function, associated hardware may
                  // not be in the function map, so it can be ignored during
                  // function object initialization
                  try
                  {
                     // Setup spacecraft hardware
                     BuildAssociations(obj);
                     obj->Initialize();
                  }
                  catch (BaseException &be)
                  {
                     if (inFunction)
                     {
                        #ifdef DEBUG_INITIALIZE_OBJ
                        // ignore error
                        MessageInterface::ShowMessage
                           ("===> %s\n===> Ignoring error for function\n", be.GetFullMessage().c_str());
                        #endif
                     }
                     else
                     {
                        throw;
                     }
                  }
               }
               else
               {
                  BuildReferencesAndInitialize(obj);    
               }
               
               // Check if we need to register subscribers to the publisher
               if (objType == Gmat::SUBSCRIBER && registerSubscribers)
               {
                  #ifdef DEBUG_SUBSCRIBER
                  MessageInterface::ShowMessage
                     ("ObjectInitializer registering global subscriber '%s' of type '%s' "
                      "to publisher\n", obj->GetName().c_str(), obj->GetTypeName().c_str());
                  #endif
                  publisher->Subscribe((Subscriber*)obj);
               }
            }
            else if (obj->IsOfType(objTypeStr))
            {
               #ifdef DEBUG_INITIALIZE_OBJ
               // ignore error
               MessageInterface::ShowMessage
                  ("Calling BuildReferencesAndInitialize() for <%p>[%s]'%s'\n",
                   obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
               #endif
               BuildReferencesAndInitialize(obj);
            }
         }
      }
   }
   catch (BaseException &be)
   {
      #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage
         ("==> InitializeObjectsInTheMap() caught an exception '%s'\n", be.GetFullMessage().c_str());
      #endif
      // Check if undefined ref objects can be ignored
      if (usingGOS && unusedGOL != NULL)
      {
         if (find(unusedGOL->begin(), unusedGOL->end(), objName) !=
             unusedGOL->end())
         {
            #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage
               ("==> Ignoring undefined ref obj '%s' in GOS\n",
                objName.c_str());
            #endif
         }
         else
         {
            be.SetFatal(true);
            throw;
         }
      }
      else
      {
         throw;
      }
   }
   
   #ifdef DEBUG_INITIALIZE_OBJ
   MessageInterface::ShowMessage
      ("InitializeObjectsInTheMap() leaving, objMap=<%p>, objType=%d, "
       "objTypeStr='%s', inFunction=%d\n", objMap, objType, objTypeStr.c_str(),
       inFunction);
   #endif
}


//------------------------------------------------------------------------------
// void InitializeSystemParameters(ObjectMap *objMap)
//------------------------------------------------------------------------------
void ObjectInitializer::InitializeSystemParameters(ObjectMap *objMap)
{
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = objMap->begin(); omi != objMap->end(); ++omi)
   {
      GmatBase *obj = omi->second;
      
      if (obj == NULL)
         throw GmatBaseException
            ("Cannot initialize NULL pointer of \"" + omi->first + "\" object");
      
      // Treat parameters as a special case -- because system parameters must be
      // initialized before other parameters.
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         Parameter *param = (Parameter *)obj;
         // Make sure system parameters are configured before others
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage(
                  "ObjectInitializer::Initialize objTypeName = %s, objName = %s\n",
                  obj->GetTypeName().c_str(), obj->GetName().c_str());
            #endif
            
            BuildReferencesAndInitialize(obj);
         }
      }
   }
}


//------------------------------------------------------------------------------
// void InitializeSubscribers(ObjectMap *objMap)
//------------------------------------------------------------------------------
void ObjectInitializer::InitializeSubscribers(ObjectMap *objMap)
{
   #ifdef DEBUG_SUBSCRIBER
   MessageInterface::ShowMessage
      ("ObjectInitializer::InitializeSubscribers() entered, registerSubscribers = %d\n",
       registerSubscribers);
   #endif
   
   std::list<Subscriber*>::iterator subiter;
   std::list<Subscriber*> subList = publisher->GetSubscriberList();
   /// Initialize in z-order here, if relative z-order values have been saved from a previous run
   std::list<Subscriber*> orderedList = GetSubscribersInZOrder(subList);
   std::map<std::string, GmatBase *>::iterator omi;
   
   #ifdef DEBUG_SUBSCRIBER
   MessageInterface::ShowMessage("   Number of configured substribers is %d\n", subList.size());
   #endif
   
   for (subiter = orderedList.begin(); subiter != orderedList.end(); ++subiter)
   {
      std::string subName = (*subiter)->GetName();      
      for (omi = objMap->begin(); omi != objMap->end(); ++omi)
      {
         GmatBase *obj = omi->second;
         
         if (obj == NULL)
            throw GmatBaseException
               ("Cannot initialize NULL pointer of \"" + omi->first + "\" Object");
         
         // Treat subscribers as a special case -- because they need to by initialized
         // in the order they are created. So use initializing order from the Publisher.
         if (obj->IsOfType(Gmat::SUBSCRIBER))
         {
            if (subName == obj->GetName())
            {
               #if defined(DEBUG_OBJECT_INITIALIZER) || defined(DEBUG_SUBSCRIBER)
               MessageInterface::ShowMessage
                  ("   Calling BuildReferencesAndInitialize() for <%p>[%s]'%s'\n",
                   obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
               #endif
               
               BuildReferencesAndInitialize(obj);
            }
         }
      }
   }
   
   #ifdef DEBUG_SUBSCRIBER
   MessageInterface::ShowMessage
      ("   Checking if function subscribers needs to be initialized and registered\n");
   #endif
   // Check if we need to initialize and register function subscribers to the publisher
   // (Fix for GMT-1552 LOJ: 2015.06.24)
   for (omi = objMap->begin(); omi != objMap->end(); ++omi)
   {
      GmatBase *obj = omi->second;
      std::string objName = obj->GetName(); // obj pointer is already checked above
      
      // Current status on plots inside a function:
      // OrbieView, GroundTrackPlot, XyPlot: Draws plots but they get deleted when function run finishes
      // (Plot behavior inside a function has not been specified as of 2015.07.09)
      
      // If we only allow local ReportFile or EphemerisFile inside a function
      // uncomment this line(LOJ: 2015.07.09)
      //if (obj->IsOfType(Gmat::SUBSCRIBER) && obj->IsOfType("FileOutput"))
      if (obj->IsOfType(Gmat::SUBSCRIBER))
      {
         Subscriber *sub = (Subscriber*)obj;
         
         // If name not found from the ordered list, then assumed it is a function subscriber
         if (find(orderedList.begin(), orderedList.end(), sub) == orderedList.end())
         {
            #if defined(DEBUG_OBJECT_INITIALIZER) || defined(DEBUG_SUBSCRIBER)
            MessageInterface::ShowMessage
               ("   Calling BuildReferencesAndInitialize() for subscriber <%p>[%s]'%s'\n",
                obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
            #endif
            
            BuildReferencesAndInitialize(obj);
            
            if (registerSubscribers)
            {
               #ifdef DEBUG_SUBSCRIBER
               MessageInterface::ShowMessage
                  ("   About to register a function subscriber <%p>[%s]'%s'\n", obj,
                   obj->GetTypeName().c_str(), obj->GetName().c_str());
               #endif
               
               publisher->Subscribe((Subscriber*)obj);
            }
         }
         else
         {
            #ifdef DEBUG_SUBSCRIBER
            MessageInterface::ShowMessage
               ("Skipping configured subscriber '%s', building and initialization "
                "is already handled\n", objName.c_str());
            #endif
         }
      }
   }
   
   #ifdef DEBUG_SUBSCRIBER
   MessageInterface::ShowMessage("ObjectInitializer::InitializeSubscribers() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void InitializeAllOtherObjects(ObjectMap *objMap)
//------------------------------------------------------------------------------
/*
 * Initializes the rest of objects in the map.
 */
//------------------------------------------------------------------------------
void ObjectInitializer::InitializeAllOtherObjects(ObjectMap *objMap)
{
   #ifdef DEBUG_INITIALIZE_OBJ
   MessageInterface::ShowMessage
      ("ObjectInitializer::InitializeAllOtherObjects() entered\n");
   #endif
   
   std::map<std::string, GmatBase *>::iterator omi;
   for (omi = objMap->begin(); omi != objMap->end(); ++omi)
   {
      GmatBase *obj = omi->second;
      
      if (obj == NULL)
         throw GmatBaseException
            ("Cannot initialize NULL pointer of \"" + omi->first + "\" object");
      
      if ((obj->GetType() != Gmat::COORDINATE_SYSTEM) &&
          (obj->GetType() != Gmat::SPACECRAFT) &&
          (obj->GetType() != Gmat::GROUND_STATION) &&
          (obj->GetType() != Gmat::MEASUREMENT_MODEL) &&
          (obj->GetType() != Gmat::PARAMETER) &&
          (obj->GetType() != Gmat::SUBSCRIBER))
      {
         #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage
            ("ObjectInitializer::Initialize objTypeName = %s, objName = %s\n",
             obj->GetTypeName().c_str(), obj->GetName().c_str());
         #endif
         if (obj->GetType() == Gmat::ODE_MODEL)
         {
            // ODEModel needs coordinate system settings
            BuildReferences(obj);
         }
         else if (obj->GetType() == Gmat::PROP_SETUP)
         {
            // PropSetup initialization is handled by the commands, since the
            // state that is propagated may change as spacecraft are added or
            // removed.
            BuildReferences(obj);
         }
         else
         {
            if (obj->IsOfType(Gmat::SPACE_POINT)       ||
                obj->IsOfType(Gmat::BURN)              ||
                obj->IsOfType(Gmat::HARDWARE)          ||
                obj->IsOfType("Estimator")             ||
                obj->IsOfType("Simulator")             ||
                obj->IsOfType(Gmat::EVENT_LOCATOR)     ||
                obj->IsOfType(Gmat::INTERFACE)         ||
                obj->GetType() >= Gmat::USER_DEFINED_OBJECT)
            {
               BuildReferencesAndInitialize(obj);
            }
         }
      }
   }
}


//------------------------------------------------------------------------------
// void InitializeInternalObjects()
//------------------------------------------------------------------------------
/**
 *  Initializes internal objects in the sandbox.
 */
//------------------------------------------------------------------------------
void ObjectInitializer::InitializeInternalObjects()
{
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("Entering InitializeInternalObjects ...\n");
   #endif
   SpacePoint *sp, *j2kBod;
   std::string j2kName, oName;
   
   if (ss == NULL)
      throw GmatBaseException("ObjectInitializer::InitializeInternalObjects() "
                              "The Solar System pointer is NULL");
   
   if (internalCS == NULL)
      throw GmatBaseException("ObjectInitializer::InitializeInternalObjects() "
                              "The Internal Coordinate System pointer is NULL");
   
   ss->Initialize();
   #ifdef DEBUG_INITIALIZE_OBJ
   MessageInterface::ShowMessage
      ("--- The object <%p>[%s]'%s' initialized\n",  ss, ss->GetTypeName().c_str(),
       ss->GetName().c_str());
   #endif
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage(" ... and solar system is initialized  ...\n");
   #endif
   
   // Set J2000 bodies for solar system objects -- should this happen here?
   const StringArray biu = ss->GetBodiesInUse();
   for (StringArray::const_iterator i = biu.begin(); i != biu.end(); ++i)
   {
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage(" ... initializing body %s ...\n", (*i).c_str());
      #endif
      sp = ss->GetBody(*i);
      j2kName = sp->GetStringParameter("J2000BodyName");
      j2kBod = FindSpacePoint(j2kName);
      sp->SetJ2000Body(j2kBod);
   }

   // set ref object for internal coordinate system
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage
         (" ... solar system about to be set on internalCS <%p> ...\n", internalCS);
   #endif
   
   internalCS->SetSolarSystem(ss);
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage(" ... and solar system is set on internalCS  ...\n");
      MessageInterface::ShowMessage(" ... about to call BuildReferences  ...\n");
   #endif

   BuildReferences(internalCS);

   // Set reference origin for internal coordinate system.
   oName = internalCS->GetStringParameter("Origin");
   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
         oName + "\" used for the internal coordinate system origin");
   internalCS->SetRefObject(sp, Gmat::SPACE_POINT, oName);


   // Set J2000 body for internal coordinate system
   oName = internalCS->GetStringParameter("J2000Body");
   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
         oName + "\" used for the internal coordinate system J2000 body");
   internalCS->SetRefObject(sp, Gmat::SPACE_POINT, oName);

   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage(" ... about to call Initialize on internalCS  ...\n");
   #endif
      
   internalCS->Initialize();
   #ifdef DEBUG_INITIALIZE_OBJ
   MessageInterface::ShowMessage
      ("--- The object <%p>[%s]'%s' initialized\n",  internalCS,
       internalCS->GetTypeName().c_str(), internalCS->GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// void InitializeCoordinateSystem(GmatBase *obj)
//------------------------------------------------------------------------------
/*
 * Sets reference objects of CoordinateSystem object and owned objects.
 */
//------------------------------------------------------------------------------
void ObjectInitializer::InitializeCoordinateSystem(GmatBase *obj)
{
   #ifdef DEBUG_INITIALIZE_CS
   MessageInterface::ShowMessage
      ("Entering ObjectInitializer::InitializeCoordinateSystem(), obj=<%p>[%s]'%s'\n",
       obj, obj->GetTypeName().c_str(), obj->GetName().c_str() );
   #endif
   
   if (!obj->IsOfType(Gmat::COORDINATE_SYSTEM))
   {
      #ifdef DEBUG_INITIALIZE_CS
      MessageInterface::ShowMessage
         ("The object '%s' is not of CoordinateSystem\n", obj->GetName().c_str());
      #endif
      return;
   }
   
   SpacePoint *sp;
   CoordinateSystem *cs = (CoordinateSystem*)obj;
   std::string oName;   
   
   // Set the reference objects for the coordinate system
   BuildReferences(cs);
   
   // Initialize axis of the coordinate system
   GmatBase *axes = cs->GetOwnedObject(0);
   
   if (axes == NULL)
      throw GmatBaseException
         ("Cannot initialize NULL axes of CoordinateSystem \"" + cs->GetName() + "\"");
   
   BuildReferences(axes);
   
   // Initialize origin of the coordinate system
   oName = cs->GetStringParameter("Origin");
   
   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
         oName + "\" used for the coordinate system " +
         cs->GetName() + " origin");
   
   cs->SetRefObject(sp, Gmat::SPACE_POINT, oName);
   
   oName = cs->GetStringParameter("J2000Body");
   
   sp = FindSpacePoint(oName);
   if (sp == NULL)
      throw GmatBaseException("Cannot find SpacePoint named \"" +
         oName + "\" used for the coordinate system " +
         cs->GetName() + " J2000 body");
   
   cs->SetRefObject(sp, Gmat::SPACE_POINT, oName);
   #ifdef DEBUG_INITIALIZE_CS
   MessageInterface::ShowMessage
      ("Exiting ObjectInitializer::InitializeCoordinateSystem()\n");
   #endif
}


//------------------------------------------------------------------------------
// void BuildReferencesAndInitialize(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 *  Sets all reference objects for the input object and initialize
 */
//------------------------------------------------------------------------------
void ObjectInitializer::BuildReferencesAndInitialize(GmatBase *obj)
{   
   #ifdef DEBUG_INITIALIZE_OBJ
   MessageInterface::ShowMessage
		("BuildReferencesAndInitialize() entered, obj = <%p>[%s]'%s'\n",
       obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
   MessageInterface::ShowMessage("--- Calling BuildReferences()\n");
	#endif
	
   BuildReferences(obj);
   
   // If this object has a run time GUI component, set it up now
   if (obj->HasGuiPlugin() && pCreateWidget)
   {
      StringArray panels = obj->GetGuiPanelNames("Execution");
      if (panels.size() > 0)
      {
         GmatWidget *widget = pCreateWidget("Execution", obj, NULL);
         obj->SetWidget(widget);
      }
   }

   /// @todo: Create separate EquationInitializer?
   // Setup equations if the object has any
   if (obj->HasEquation())
      SetupEquation(obj);

   #ifdef DEBUG_INITIALIZE_OBJ
   MessageInterface::ShowMessage("--- Calling '%s'->Initialize()\n", obj->GetName().c_str());
	#endif
	
   /// @todo Fix so that failed initialization (return == false) is handled
   try
   {
       obj->Initialize();
   }
   catch (BaseException &e)
   {
       std::string errormsg = e.GetDetails();
       errormsg = obj->AddFileInfoToErrorMessage(errormsg);
       e.SetDetails(errormsg);
       throw;
   }
   
   #ifdef DEBUG_INITIALIZE_OBJ
   MessageInterface::ShowMessage
      ("--- The object <%p>[%s]'%s' initialized\n",  obj,
       obj->GetTypeName().c_str(), obj->GetName().c_str());
   MessageInterface::ShowMessage
		("BuildReferencesAndInitialize() leaving, obj = <%p>[%s]'%s'\n",
       obj, obj->GetTypeName().c_str(), obj->GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// void BuildReferences()
//------------------------------------------------------------------------------
/**
 *  Sets all reference objects for the input object.
 */
//------------------------------------------------------------------------------
void ObjectInitializer::BuildReferences(GmatBase *obj)
{
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage
         ("Entering ObjectInitializer::BuildReferences, object type = '%s', name = '%s'\n",
          obj->GetTypeName().c_str(), obj->GetName().c_str());
   #endif
   std::string oName;
   
   obj->SetSolarSystem(ss);
   obj->SetInternalCoordSystem(internalCS);
   
   // Handle dependency objects for parameters
   if (obj->IsOfType(Gmat::PARAMETER))
   {
      std::string objDepName = obj->GetStringParameter("DepObject");
      if (objDepName != "")
      {
         GmatBase *depObj = FindObject(objDepName);
         ((Parameter*)obj)->AddRefObject(depObj);
      }
   }

   // PropSetup probably should do this...
   if ((obj->IsOfType(Gmat::PROP_SETUP)) ||
       (obj->IsOfType(Gmat::ODE_MODEL)))
   {
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage("--- it is a PropSetup or ODEModel ...\n");
      #endif

      bool hasODEModel = true;
      
      if (obj->IsOfType(Gmat::PROP_SETUP))
      {
         Propagator *prop = ((PropSetup*)(obj))->GetPropagator();
         if (prop->UsesODEModel() == false)
            hasODEModel = false;
      }

      if (hasODEModel)
      {
         ODEModel *fm = NULL;
         if (obj->IsOfType(Gmat::PROP_SETUP))
            fm = ((PropSetup *)obj)->GetODEModel();
         else
            fm = ((ODEModel *)obj);

         fm->SetSolarSystem(ss);

         // Handle the coordinate systems
         StringArray csList = fm->GetStringArrayParameter("CoordinateSystemList");

         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage("Coordinate system list for '%s' <%p>:\n",
               fm->GetName().c_str(), fm);
            for (StringArray::iterator i = csList.begin(); i != csList.end(); ++i)
               MessageInterface::ShowMessage("   %s\n", i->c_str());
         #endif
         // Set CS's on the objects
         for (StringArray::iterator i = csList.begin(); i != csList.end(); ++i)
         {
            CoordinateSystem *theCS = NULL;

            if (LOS)
            {
               if (LOS->find(*i) != LOS->end())
               {
                  GmatBase *ref = (*LOS)[*i];
                  if (ref->IsOfType("CoordinateSystem") == false)
                     throw GmatBaseException("Object named " + (*i) +
                        " was expected to be a Coordinate System, but it has type " +
                        ref->GetTypeName());
                  theCS = (CoordinateSystem*)ref;
                  fm->SetRefObject(theCS, theCS->GetType(), *i);
               }
            }

            if (GOS && (theCS == NULL))
            {
               if(GOS->find(*i) != GOS->end())
               {
                  GmatBase *ref = (*GOS)[*i];
                  if (ref->IsOfType("CoordinateSystem") == false)
                     throw GmatBaseException("Object named " + (*i) +
                        " was expected to be a Coordinate System, but it has type " +
                        ref->GetTypeName());
                  theCS = (CoordinateSystem*)ref;
                  fm->SetRefObject(theCS, theCS->GetType(), *i);
               }
            }

            if (theCS == NULL)
            {
               if (i->find("Fixed") != std::string::npos)
               {
                  std::string csOrigin = i->substr(0, i->find("Fixed"));

                  theCS = mod->CreateCoordinateSystem("", false);
                  AxisSystem *axes = mod->CreateAxisSystem("BodyFixed", "");
                  theCS->SetName(*i);
                  theCS->SetRefObject(axes, Gmat::AXIS_SYSTEM, "");
                  theCS->SetOriginName(csOrigin);

                  // Since CoordinateSystem clones AxisSystem, delete it from here
                  #ifdef DEBUG_MEMORY
                  MemoryTracker::Instance()->Remove
                     (axes, "localAxes", "ObjectInitializer::BuildReferences()",
                      "deleting localAxes");
                  #endif
                  delete axes;

                  fm->SetRefObject(theCS, theCS->GetType(), *i);

                  theCS->SetSolarSystem(ss);
                  BuildReferences(theCS);
                  InitializeCoordinateSystem(theCS);
                  theCS->Initialize();
                  #ifdef DEBUG_INITIALIZE_OBJ
                  MessageInterface::ShowMessage
                     ("--- The object <%p>[%s]'%s' initialized\n",  internalCS,
                      internalCS->GetTypeName().c_str(), internalCS->GetName().c_str());
                  #endif
                  Integer gosSize = 0;
                  if (GOS)
                     gosSize = GOS->size();
                  // if things have already been moved to the globalObjectStore, put it there
                  if ((gosSize > 0) && (theCS->IsGlobal()))
                     (*GOS)[*i] = theCS;
                  // otherwise, put it in the Sandbox object map - it will be moved to the GOS later
                  else
                     if (LOS)
                        (*LOS)[*i] = theCS;

                  #ifdef DEBUG_OBJECT_INITIALIZER
                     MessageInterface::ShowMessage(
                        "Coordinate system %s has body %s\n",
                        theCS->GetName().c_str(), theCS->GetOriginName().c_str());
                  #endif
               }
               if (i->find("MJ2000Eq") != std::string::npos)
               {
                  //MessageInterface::ShowMessage("===> create BodyFixed here\n");
                  theCS = mod->CreateCoordinateSystem("", false);
                  AxisSystem *axes = mod->CreateAxisSystem("MJ2000Eq", "");
                  theCS->SetName(*i);
                  theCS->SetRefObject(axes, Gmat::AXIS_SYSTEM, "");
                  theCS->SetOriginName(fm->GetStringParameter("CentralBody"));

                  // Since CoordinateSystem clones AxisSystem, delete it from here
                  #ifdef DEBUG_MEMORY
                  MemoryTracker::Instance()->Remove
                     (axes, "localAxes", "ObjectInitializer::BuildReferences()",
                      "deleting localAxes");
                  #endif
                  delete axes;

                  fm->SetRefObject(theCS, theCS->GetType(), *i);

                  theCS->SetSolarSystem(ss);
                  BuildReferences(theCS);
                  InitializeCoordinateSystem(theCS);
                  theCS->Initialize();
                  #ifdef DEBUG_INITIALIZE_OBJ
                  MessageInterface::ShowMessage
                     ("--- The object <%p>[%s]'%s' initialized\n",  internalCS,
                      internalCS->GetTypeName().c_str(), internalCS->GetName().c_str());
                  #endif
                  Integer gosSize = 0;
                  if (GOS)
                     gosSize = GOS->size();
                  // if things have already been moved to the globalObjectStore, put it there
                  if ((gosSize > 0) && (theCS->IsGlobal()))
                     (*GOS)[*i] = theCS;
                  // otherwise, put it in the Sandbox object map - it will be moved to the GOS later
                  else
                     if (LOS)
                        (*LOS)[*i] = theCS;

                  #ifdef DEBUG_OBJECT_INITIALIZER
                     MessageInterface::ShowMessage(
                        "Coordinate system %s has body %s\n",
                        theCS->GetName().c_str(), theCS->GetOriginName().c_str());
                  #endif
               }
            }
         }

         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage(
               "Initializing force model references for '%s'\n",
               (fm->GetName() == "" ? obj->GetName().c_str() :
                                      fm->GetName().c_str()) );
         #endif

         try
         {
            StringArray fmRefs = fm->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
            #ifdef DEBUG_OBJECT_INITIALIZER
               MessageInterface::ShowMessage("fmRefs are:\n");
               for (unsigned int ii=0; ii < fmRefs.size(); ii++)
                  MessageInterface::ShowMessage(" --- %s\n", (fmRefs.at(ii)).c_str());
            #endif
            for (StringArray::iterator i = fmRefs.begin(); i != fmRefs.end(); ++i)
            {
               oName = *i;
               try
               {
                  SetRefFromName(fm, oName);
               }
               catch (GmatBaseException &ex)  // *******
               {
                  // Use exception to remove Visual C++ warning
                  ex.GetMessageType();
                  // Handle SandboxExceptions first.
                  #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
                     MessageInterface::ShowMessage(
                        "RefObjectName " + oName + " not found; ignoring " +
                        ex.GetFullMessage() + "\n");
                  #endif
                  //throw ex;
               }
               catch (BaseException &ex)
               {
                  // Use exception to remove Visual C++ warning
                  ex.GetMessageType();
                  // Post a message and -- otherwise -- ignore the exceptions
                  // Handle SandboxExceptions first.
                  #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
                     MessageInterface::ShowMessage(
                        "RefObjectName not found; ignoring " +
                        ex.GetFullMessage() + "\n");
                  #endif
               }
            }
         }
         catch (GmatBaseException &ex) // **********
         {
            // Use exception to remove Visual C++ warning
            ex.GetMessageType();
            // Handle SandboxExceptions first.
            #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
               MessageInterface::ShowMessage(
                  "RefObjectNameArray not found; ignoring " +
                  ex.GetFullMessage() + "\n");
            #endif
            //throw ex;
         }
         catch (BaseException &ex) // Handles no refObject array
         {
            // Use exception to remove Visual C++ warning
            ex.GetMessageType();
            // Post a message and -- otherwise -- ignore the exceptions
            #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
               MessageInterface::ShowMessage(
                  "RefObjectNameArray not found; ignoring " +
                  ex.GetFullMessage() + "\n");
            #endif
         }

         if (obj->IsOfType(Gmat::ODE_MODEL))
            return;
      }
   }


   try
   {
      // First set the individual reference objects
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage(
               "Attempting to set individual reference objects ...\n");
      #endif
      oName = obj->GetRefObjectName(Gmat::UNKNOWN_OBJECT);
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage(
               "Attempting to set individual reference objects with name = %s...\n",
               oName.c_str());
      #endif
      SetRefFromName(obj, oName);
   }
   catch (SubscriberException &ex)
   {
      #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
      MessageInterface::ShowMessage
         ("Caught SubscriberException while setting individual reference "
          "objects and rethrowing %s\n", ex.GetFullMessage().c_str());
      #endif
      throw ex;
   }
   catch (GmatBaseException &ex) // ************
   {
      // Use exception to remove Visual C++ warning
      ex.GetMessageType();
      // Handle SandboxExceptions first.
      // For now, post a message and -- otherwise -- ignore exceptions
      #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
         MessageInterface::ShowMessage("RefObjectName not found; ignoring " +
            ex.GetFullMessage() + "\n");
      #endif
      //throw ex;
   }
   catch (BaseException &ex)
   {
      // Use exception to remove Visual C++ warning
      ex.GetMessageType();
      // Post a message and -- otherwise -- ignore the exceptions
      #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
         MessageInterface::ShowMessage("RefObjectName not found; ignoring " +
            ex.GetFullMessage() + "\n");
      #endif
   }
   
   
   // Next handle the array version
   try
   {
      #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("Attempting to set reference object arrays...\n");
      #endif
      StringArray oNameArray = obj->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
      for (StringArray::iterator i = oNameArray.begin();
           i != oNameArray.end(); ++i)
      {
         oName = *i;
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage(
                  "Attempting to set ARRAY object with name = %s...\n",
                  oName.c_str());
         #endif
         try
         {
            SetRefFromName(obj, oName);
         }
         catch (GmatBaseException &ex) // ***********
         {
            // Use exception to remove Visual C++ warning
            ex.GetMessageType();
            // Handle SandboxExceptions first.
            #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
               MessageInterface::ShowMessage(
                  "RefObjectName " + oName + " not found; ignoring " +
                  ex.GetFullMessage() + "\n");
            #endif
            
            if (ex.IsFatal())
               throw ex;
         }
         catch (SubscriberException &ex)
         {
            #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
            MessageInterface::ShowMessage
               ("Caught SubscriberException while setting reference "
                "object arrays and rethrowing %s\n", ex.GetFullMessage().c_str());
            #endif
            throw ex;
         }
         catch (BaseException &ex)
         {
            // Use exception to remove Visual C++ warning
            ex.GetMessageType();
            // Post a message and -- otherwise -- ignore the exceptions
            // Handle SandboxExceptions first.
            #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
               MessageInterface::ShowMessage(
                  "RefObjectName not found; ignoring " +
                  ex.GetFullMessage() + "\n");
            #endif
         }
      }
   }
   catch (GmatBaseException &ex)  // ***********
   {
      // Use exception to remove Visual C++ warning
      ex.GetMessageType();
      // Handle SandboxExceptions first.
      #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
         MessageInterface::ShowMessage(
            "RefObjectNameArray not found; ignoring " +
            ex.GetFullMessage() + "\n");
      #endif
      
      if (ex.IsFatal())
         throw ex;
   }
   catch (SubscriberException &ex)
   {
      throw ex;
   }
   catch (BaseException &ex) // Handles no refObject array
   {
      // Use exception to remove Visual C++ warning
      ex.GetMessageType();
      // Post a message and -- otherwise -- ignore the exceptions
      #ifdef DEBUG_OBJECT_INITIALIZER_DETAILED
         MessageInterface::ShowMessage(
            "RefObjectNameArray not found; ignoring " +
            ex.GetFullMessage() + "\n");
      #endif
   }
   #ifdef DEBUG_OBJECT_INITIALIZER
	MessageInterface::ShowMessage
		("Exiting BuildReferences for object type '%s'\n", obj->GetTypeName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// bool SetupEquation(GmatBase* obj)
//------------------------------------------------------------------------------
/**
 * Sets the objects on an object's equation fields
 *
 * @param obj The object that is setup
 *
 * @return true is no errors were encountered
 */
//------------------------------------------------------------------------------
bool ObjectInitializer::SetupEquation(GmatBase* obj)
{
   bool retval = true;

   std::vector<RHSEquation*> equations;
   obj->GetEquations(equations);
   for (UnsignedInt j = 0; j < equations.size(); ++j)
   {
      std::string equationString;
      RHSEquation *equation = equations[j];

      #ifdef DEBUG_EQUATION_PARAMETERS
         MessageInterface::ShowMessage("There is an equation on %s "
               "at %p\n", obj->GetName().c_str(), equation);
      #endif

      ObjectMap theObjectMap = (*LOS);
      // Append the GOS
      for (ObjectMap::iterator it = LOS->begin(); it != LOS->end(); ++it)
         theObjectMap[it->first] = it->second;

      retval &= equation->BuildExpression(equationString, &theObjectMap);
      if (retval)
      {
         retval = Validator::Instance()->ValidateEquation(equation);
         equation->SetMathWrappers();
         equation->GetMathTree(false)->SetObjectMap(LOS);
         equation->GetMathTree(false)->SetGlobalObjectMap(GOS);
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void SetWidgetCreator(GuiWidgetCreatorCallback creatorFun)
//------------------------------------------------------------------------------
/**
 * Sets the callback function used to create plugin GUI widgets
 *
 * @param creatorFun The function pointer that is called to create the widgets
 */
//------------------------------------------------------------------------------
void ObjectInitializer::SetWidgetCreator(GuiWidgetCreatorCallback creatorFun)
{
   pCreateWidget = creatorFun;
}


//------------------------------------------------------------------------------
// void SetRefFromName(GmatBase *obj, const std::string &oName)
//------------------------------------------------------------------------------
/**
 *  Sets a reference object on an object.
 *
 *  @param <obj>   The object that needs to set the reference.
 *  @param <oName> The name of the reference object.
 */
//------------------------------------------------------------------------------
void ObjectInitializer::SetRefFromName(GmatBase *obj, const std::string &oName)
{
   #ifdef DEBUG_OBJECT_INITIALIZER
      MessageInterface::ShowMessage("Setting reference '%s' on '%s', inFunction=%d\n", 
         oName.c_str(), obj->GetName().c_str(), inFunction);
   #endif
      
   GmatBase *refObj = NULL;
   
   if ((refObj = FindObject(oName)) != NULL)
   {
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage("FindObject found %s\n", oName.c_str());
      #endif
      // Do not set if object and its associated hardware in function
      if (refObj->IsOfType(Gmat::HARDWARE) && obj->IsLocal() && refObj->IsLocal())
      {
         #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage
            ("   '%s' is associated hardware of '%s' in function, so skip\n",
             refObj->GetName().c_str(), obj->GetName().c_str());
         #endif
      }
      else
      {
         #ifdef DEBUG_OBJECT_INITIALIZER
            MessageInterface::ShowMessage("Calling SetRefObject .... \n");
         #endif
         obj->SetRefObject(refObj, refObj->GetType(), refObj->GetName());
      }
   }
   else
   {
      #ifdef DEBUG_OBJECT_INITIALIZER
         MessageInterface::ShowMessage("FindObject did NOT find %s\n", oName.c_str());
      #endif
      // look in the SolarSystem
      refObj = FindSpacePoint(oName);
      
      if (refObj == NULL)
         throw GmatBaseException("Unknown object " + oName + " requested by " +
                                  obj->GetName());

      obj->SetRefObject(refObj, refObj->GetType(), refObj->GetName());
   }
}


//------------------------------------------------------------------------------
// void BuildAssociations(GmatBase * obj)
//------------------------------------------------------------------------------
/**
 *  Assigns clones of objects to their owners.
 *
 *  This method finds referenced objects that need to be associated with the
 *  input object through cloning, creates the clones, and hands the cloned
 *  object to the owner.
 *
 *  An example of the associations that are made here are hardware elements that
 *  get associated with spacecraft.  Users configure a single element -- for
 *  example, a tank, and then can assign that element to many different
 *  spacecraft.  In order to avoid multiple objects using the same instance of
 *  the tank, clones are made for each spacecraft that has the tank associated
 *  with it.
 *
 *  @param <obj> The owner of the clones.
 */
//------------------------------------------------------------------------------
void ObjectInitializer::BuildAssociations(GmatBase * obj)
{
   std::string objName = obj->GetName();
   std::string objType = obj->GetTypeName();
   
   #ifdef DEBUG_BUILD_ASSOCIATIONS
   MessageInterface::ShowMessage
      ("ObjectInitializer::BuildAssociations() entered, obj=<%p>[%s]'%s', "
       "inFunction=%d\n", obj, objType.c_str(), objName.c_str(), inFunction);
   #endif
   
   // Spacecraft clones associated hardware objects
   if (obj->IsOfType(Gmat::SPACECRAFT))
   {
      StringArray hw = obj->GetRefObjectNameArray(Gmat::HARDWARE);
      for (StringArray::iterator i = hw.begin(); i < hw.end(); ++i)
      {
         #ifdef DEBUG_BUILD_ASSOCIATIONS
            MessageInterface::ShowMessage
               ("ObjectInitializer::BuildAssociations() setting \"%s\" on \"%s\"\n",
                i->c_str(), objName.c_str());
         #endif
         
         GmatBase *elem = NULL;
         if ((elem = FindObject(*i)) == NULL)
            throw GmatBaseException("ObjectInitializer::BuildAssociations: Cannot find "
                                    "hardware element \"" + (*i) + "\"");
         
         // To handle Spacecraft hardware setting inside the function,
         // all hardware are cloned in the Spacecraft::SetRefObject() method. (LOJ: 2009.07.24)
         GmatBase *newElem = elem;
         
         // Now a function is parsed in two-modes (resource and command), so we don't
         // need to check this (LOJ: 2015.02.06)
         #if 0
         // If hardware is local object inside a function then skip
         if (inFunction && obj->IsLocal() && newElem->IsLocal())
         {
            #ifdef DEBUG_BUILD_ASSOCIATIONS
            MessageInterface::ShowMessage
               ("   ---> '%s' and '%s' are local objects inside a function, so skip\n",
                obj->GetName().c_str(), newElem->GetName().c_str());
            #endif
            continue;
         }
         #endif
         
         // now set Hardware to Spacecraft
         if (!obj->SetRefObject(newElem, newElem->GetType(), newElem->GetName()))
         {
            MessageInterface::ShowMessage
               ("ObjectInitializer::BuildAssociations() failed to set %s\n",
                newElem->GetName().c_str());
            // Should we throw an exception here?
         }
         else
         {
            #ifdef DEBUG_BUILD_ASSOCIATIONS
            MessageInterface::ShowMessage
               ("ObjectInitializer::BuildAssociations() successfully set <%p>'%s' to '%s'\n",
                newElem, newElem->GetName().c_str(), objName.c_str());
            #endif
            
            // Set SolarSystem and Spacecraft to Thruster since it needs coordinate
            // conversion during Thruster initialization (LOJ: 2009.03.05)
            // Set CoordinateSystem (LOJ: 2009.05.05)
            if (newElem->IsOfType(Gmat::THRUSTER))
            {
               newElem->SetSolarSystem(ss);
               newElem->SetRefObject(obj, Gmat::SPACECRAFT, objName);
               std::string csName = newElem->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
               if (csName != "")
                  newElem->SetRefObject(FindObject(csName), Gmat::COORDINATE_SYSTEM, csName);
               newElem->Initialize();
               
               #ifdef DEBUG_BUILD_ASSOCIATIONS
               MessageInterface::ShowMessage
                  ("ObjectInitializer::BuildAssociations() adding Thruster's CS to "
                   "'%s' so that it can set it to cloned Thruster\n", obj->GetName().c_str());
               #endif
               // Now add Thruster's CoordinateSystem to Spacecraft so that it can set it
               // to cloned Thruster
               StringArray objRefs = obj->GetRefObjectNameArray(Gmat::COORDINATE_SYSTEM);
               for (StringArray::iterator i = objRefs.begin(); i != objRefs.end(); ++i)
               {
                  csName = *i;
                  GmatBase *refObj = FindObject(csName);
                  if (refObj == NULL)
                     throw GmatBaseException("Unknown object " + csName + " requested by " +
                                             obj->GetName());
                  obj->TakeAction("ThrusterSettingMode", "On");
                  obj->SetRefObject(refObj, refObj->GetType(), refObj->GetName());                  
                  obj->TakeAction("ThrusterSettingMode", "Off");
               }
            }
            
            #ifdef DEBUG_BUILD_ASSOCIATIONS
            MessageInterface::ShowMessage
               ("ObjectInitializer::BuildAssociations() Calling <%p>'%s'->"
                "TakeAction(SetupHardware)\n", obj, objName.c_str());
            #endif
            obj->TakeAction("SetupHardware");
            
         }
      }
   }
   
   #ifdef DEBUG_BUILD_ASSOCIATIONS
   MessageInterface::ShowMessage
      ("ObjectInitializer::BuildAssociations() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// SpacePoint* FindSpacePoint(const std::string &spname)
//------------------------------------------------------------------------------
/**
 *  Finds a SpacePoint by name.
 *
 *  @param <spname> The name of the SpacePoint.
 *
 *  @return A pointer to the SpacePoint, or NULL if it does not exist in the
 *          stores.
 */
//------------------------------------------------------------------------------
SpacePoint * ObjectInitializer::FindSpacePoint(const std::string &spName)
{
   SpacePoint *sp = ss->GetBody(spName);


   if (sp == NULL)
   {
      GmatBase *spObj;
      if ((spObj = FindObject(spName)) != NULL)
      {
         if (spObj->IsOfType(Gmat::SPACE_POINT))
            sp = (SpacePoint*)(spObj);
      }
   }

   return sp;
}

//------------------------------------------------------------------------------
// GmatBase* ObjectInitializer::FindObject(const std::string &name)
//------------------------------------------------------------------------------
/**
 *  Finds an object by name, searching through the LOS first,
 *  then the GOS
 *
 *  @param <name> The name of the object.
 *
 *  @return A pointer to the object, or NULL if it does not exist in the
 *          object stores.
 */
//------------------------------------------------------------------------------
GmatBase* ObjectInitializer::FindObject(const std::string &name)
{
   #ifdef DEBUG_FIND_OBJ
   MessageInterface::ShowMessage
      ("ObjectInitializer::FindObject() entered, name='%s'\n", name.c_str());
   #endif
   
   // Check for special object not in LOS or GOS first (for Bug 2358 fix)
   if (name == "InternalEarthMJ2000Eq")
   {
      #ifdef DEBUG_FIND_OBJ
      MessageInterface::ShowMessage
         ("ObjectInitializer::FindObject() returning <%p>'%s', it is InternalEarthMJ2000Eq\n",
          internalCS, internalCS ? internalCS->GetName().c_str() : "NULL");
      #endif
      return internalCS;
   }
   
   GmatBase *objFound = NULL;
   
   if (LOS->find(name) == LOS->end())
   {
     // If not found in the LOS, check the Global Object Store (GOS)
      if (includeGOS)
      {
         if (GOS->find(name) == GOS->end())
            objFound = NULL;
         else
            objFound = (*GOS)[name];
      }
      else
         objFound = NULL;
   }
   else
      objFound = (*LOS)[name];
   
   #ifdef DEBUG_FIND_OBJ
   MessageInterface::ShowMessage
      ("ObjectInitializer::FindObject() returning <%p>'%s'\n",
       objFound, objFound ? objFound->GetName().c_str() : "NULL");
   #endif
   return objFound;
}


//------------------------------------------------------------------------------
// virtual void ShowObjectMaps(const std::string &str)
//------------------------------------------------------------------------------
void ObjectInitializer::ShowObjectMaps(const std::string &str)
{
   MessageInterface::ShowMessage
      ("%s\n======================================================================\n",
       str.c_str());
   MessageInterface::ShowMessage
      ("ObjectInitializer::ShowObjectMaps() LOS=<%p>, GOS=<%p>\n", LOS, GOS);
   
   if (LOS)
   {
      MessageInterface::ShowMessage("Here is the local object map \n");
      MessageInterface::ShowMessage("There are %d objects\n");
      for (std::map<std::string, GmatBase *>::iterator i = LOS->begin();
           i != LOS->end(); ++i)
         MessageInterface::ShowMessage
            ("   %40s  <%p> [%s]\n", i->first.c_str(), i->second,
             i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
   }
   if (GOS)
   {
      MessageInterface::ShowMessage("Here is the global object map\n");
      MessageInterface::ShowMessage("There are %d objects\n");
      for (std::map<std::string, GmatBase *>::iterator i = GOS->begin();
           i != GOS->end(); ++i)
         MessageInterface::ShowMessage
            ("   %40s  <%p> [%s]\n", i->first.c_str(), i->second,
             i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
   }
   MessageInterface::ShowMessage
      ("======================================================================\n");
}

//------------------------------------------------------------------------------
// std::list<Subscriber*> GetSubscribersInZOrder(std::list<Subscriber*> subList)
//------------------------------------------------------------------------------
std::list<Subscriber*> ObjectInitializer::GetSubscribersInZOrder(std::list<Subscriber*> subList)
{
   if (subList.empty()) return subList;

   #ifdef DEBUG_Z_ORDER
      MessageInterface::ShowMessage("Now checking for a non-zero z-order\n");
   #endif
   bool noZOrder    = true;
   Integer zOrderID = (subList.front())->GetParameterID("RelativeZOrder");

   // if we find a relative z-order for any of the subscribers that is non-zero, we
   // know we need to order them all
   std::list<Subscriber*>::iterator si;
   for (si = subList.begin(); si != subList.end(); si++)
   {
      #ifdef DEBUG_Z_ORDER
         MessageInterface::ShowMessage("Relative z-order for %s is %d\n", ((*si)->GetName()).c_str(), ((*si)->GetIntegerParameter(zOrderID)));
      #endif
      if ((*si)->GetIntegerParameter(zOrderID) != 0)
      {
         noZOrder = false;
         break;
      }

   }
   // if we didn't find any non-zero z-order values, just return the input list
   if (noZOrder) return subList;

   // put the subscribers in z-order, from lowest to highest
   std::list<Subscriber*> tmpList = subList;
   std::list<Subscriber*> orderedList;
   Integer currentZOrder        = GmatRealConstants::INTEGER_MAX;
   Subscriber *hasLowestZOrder  = NULL;
   unsigned int origSize        = tmpList.size();
   for (unsigned int jj = 0; jj < origSize; jj++)
   {
      currentZOrder   = GmatRealConstants::INTEGER_MAX;
      hasLowestZOrder = NULL;
      for (si = tmpList.begin(); si != tmpList.end(); si++)
      {
         Integer zOrder   = (*si)->GetIntegerParameter(zOrderID);
         #ifdef DEBUG_Z_ORDER
            MessageInterface::ShowMessage("ORDERING: Relative z-order for %s is %d\n", ((*si)->GetName()).c_str(), zOrder);
         #endif
         if (zOrder < currentZOrder)
         {
            currentZOrder   = zOrder;
            hasLowestZOrder = (*si);
         }
      }
      if (hasLowestZOrder == NULL)
      {
         throw GmatBaseException("ERROR ordering subscriber list!\n");
      }
      #ifdef DEBUG_Z_ORDER
         MessageInterface::ShowMessage("------> ADDING %s to orderedList\n", (hasLowestZOrder->GetName()).c_str());
      #endif
      orderedList.push_back(hasLowestZOrder);
      tmpList.remove(hasLowestZOrder);
   }
   return orderedList;
}


//------------------------------------------------------------------------------
// ObjectInitializer::ObjectInitializer()
//------------------------------------------------------------------------------
ObjectInitializer::ObjectInitializer() :
   ss         (NULL),
   LOS        (NULL),
   GOS        (NULL),
   mod        (NULL),
   internalCS (NULL),
   includeGOS (false),
   inFunction (false)
{
}

