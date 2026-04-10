//$Id: ObjectInitializer.hpp 9457 2011-04-21 18:40:34Z lindajun $
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
// Created: 2008.06.10
//
// Original code from the Sandbox.
// Author: Darrel J. Conway
//
/**
 * Definition for the ObjectInitializer class.
 * This class initializes objects of the specified LocalObjectStore and ,
 * on option, GlobalObjectStore.
 */
//------------------------------------------------------------------------------
#ifndef ObjectInitializer_hpp
#define ObjectInitializer_hpp

#include "gmatdefs.hpp"

// included definitions
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "AxisSystem.hpp"
#include "Subscriber.hpp"
#include "GmatWidget.hpp"

// forward references
class Moderator;
class Publisher;

/// Convenience type for creation callback for the GUI plugin code
typedef GmatWidget* (*GuiWidgetCreatorCallback)(const std::string&, GmatBase *, void *);


class GMAT_API ObjectInitializer
{
public:
   ObjectInitializer(SolarSystem *solSys, ObjectMap *objMap,
                     ObjectMap *globalObjMap, CoordinateSystem* intCS,
                     bool useGOS = false, bool fromFunction = false);
   ObjectInitializer(const ObjectInitializer &objInit);
   ObjectInitializer& operator= (const ObjectInitializer &objInit);
   virtual ~ObjectInitializer();
   
   void SetSolarSystem(SolarSystem *solSys);
   void SetObjectMap(ObjectMap *objMap);
   void SetInternalCoordinateSystem(CoordinateSystem* cs);
   bool InitializeObjects(bool registerSubs = false,
                          UnsignedInt objType = Gmat::UNKNOWN_OBJECT,
                          StringArray *unusedGOL = NULL);
   // Plugin GUI extension
   void SetWidgetCreator(GuiWidgetCreatorCallback creatorFun);
   
protected:
   
   SolarSystem      *ss;
   ObjectMap        *LOS;
   ObjectMap        *GOS;
   Moderator        *mod;
   CoordinateSystem *internalCS;
   Publisher        *publisher;
   
   bool includeGOS;
   bool registerSubscribers;
   bool inFunction;
   
   /// Callback used to plugin GUI widgets
   GuiWidgetCreatorCallback          pCreateWidget;

   void SetObjectJ2000Body(ObjectMap *objMap);
   void InitializeObjectsInTheMap(ObjectMap *objMap, UnsignedInt objType,
                                  bool usingGOS = false,
                                  StringArray *unusedGOL = NULL);
   void InitializeSystemParameters(ObjectMap *objMap);
   void InitializeSubscribers(ObjectMap *objMap);
   void InitializeAllOtherObjects(ObjectMap *objMap);
   void InitializeInternalObjects();
   void InitializeCoordinateSystem(GmatBase *obj);

   void InitializePlate(GmatBase *obj);
   
   void BuildReferencesAndInitialize(GmatBase *obj);
public:
   void BuildReferences(GmatBase *obj);
   bool SetupEquation(GmatBase *obj);
protected:
   void SetRefFromName(GmatBase *obj, const std::string &oName);
public:
   void BuildAssociations(GmatBase * obj);

protected:
   SpacePoint* FindSpacePoint(const std::string &spName);
   GmatBase*   FindObject(const std::string &name);
   void        ShowObjectMaps(const std::string &title = "");
   
   std::list<Subscriber*> GetSubscribersInZOrder(std::list<Subscriber*> subList);

private:
   
   ObjectInitializer();
   
};

#endif // ObjectInitializer_hpp
