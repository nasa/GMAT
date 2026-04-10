//$Id$
//------------------------------------------------------------------------------
//                                  Publisher
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2003/10/21
/**
 * Definition for the Publisher class.
 */
//------------------------------------------------------------------------------

#ifndef Publisher_hpp
#define Publisher_hpp

#include "Subscriber.hpp"
#include <list>
#include <vector>
#include <map>


class GMAT_API Publisher
{
public:
   static Publisher*    Instance();
   
   // destructor
   virtual ~Publisher();
   
   bool Subscribe(Subscriber *s);
   bool Unsubscribe(Subscriber *s);
   bool UnsubscribeAll();
   
   virtual bool Publish(GmatBase *provider, Integer id, Real *data, Integer count, Real propDir = 1.0);
   virtual bool Publish(Integer id, char *data, Integer count = 0);
   virtual bool Publish(Integer id, Integer *data, Integer count);
   virtual void Ping();
   
   bool FlushBuffers(bool endOfDataBlock = true);
   bool NotifyEndOfRun();
   
   const std::list<Subscriber*> GetSubscriberList();
   
   // Interface methods used to identify the data sent to the publisher and
   // subscribers
   Integer              RegisterPublishedData(GmatBase *provider, Integer id,
                                              const StringArray& owners,
                                              const StringArray& elements);
   void                 UnregisterPublishedData(GmatBase *provider);
   void                 ClearPublishedData();
   const StringArray&   GetStringArrayParameter(const std::string& type);
   void                 SetInternalCoordSystem(CoordinateSystem *cs);
   void                 SetDataCoordSystem(CoordinateSystem *cs);
   void                 SetDataMJ2000EqOrigin(CelestialBody *cb);
   
   // Interfaces used to update the state of the running system
   void                 SetRunState(const Gmat::RunState state);
   
   void                 SetManeuvering(GmatBase *originator,
                                       bool flag, Real epoch,
                                       const std::string &satNames,
                                       const std::string &desc);
   void                 SetManeuvering(GmatBase *originator,
                                       bool flag, Real epoch,
                                       const StringArray &satNames,
                                       const std::string &desc);
   bool                 GetManeuvering();
   
   void                 SetSpacecraftPropertyChanged(GmatBase *originator, Real epoch,
                                                     const std::string &satName,
                                                     const std::string &desc);
   void                 SetOrbitColorChanged(GmatBase *originator,
                                             const std::string &newColor,
                                             const std::string &objName,
                                             const std::string &desc);
   void                 SetTargetColorChanged(GmatBase *originator,
                                              const std::string &newColor,
                                              const std::string &objName,
                                              const std::string &desc);
   void                 SetSegmentOrbitColor(GmatBase *originator,
                                             bool overrideColor,
                                             UnsignedInt orbitColor,
                                             const StringArray &objNames);
   
   CoordinateSystem* GetInternalCoordSystem() { return internalCoordSystem; }
   CoordinateSystem* GetDataCoordSystem() { return dataCoordSystem; }
   CelestialBody* GetDataMJ2000EqOrigin() { return dataMJ2000EqOrigin; }
   inline Gmat::RunState GetRunState();
   
protected:
   /// The singleton
   static Publisher         *instance;
   /// List of the subscribers
   std::list<Subscriber*>   subscriberList;
   /// Index used to identify number of registered data providers
   Integer                  providerId;
   /// ID for the current data provider
   Integer                  currProviderId;
   /// Arrays used to track objects for published data
   std::vector<StringArray> objectArray;
   /// Arrays used to track elements for published data
   std::vector<StringArray> elementArray;
   
   /// State of the system (used to track data for display or suppression)
   Gmat::RunState           runState;
   /// Flag indicating maneuvering
   bool                     maneuvering;
   /// Internal coordinate system
   CoordinateSystem         *internalCoordSystem;
   /// Coordinate system of data
   CoordinateSystem         *dataCoordSystem;
   /// Origin of MJ2000Eq of data
   CelestialBody            *dataMJ2000EqOrigin;
   /// Map of coordinate system of data
   std::map<std::string, CoordinateSystem*> coordSysMap;
   
   //   // Data buffers used to avoid repeated points
   //   /// The data last published
   //   Real *dataBuffer;
   //   /// Count for the last published data
   //   Integer countBuffer;

   /// published data info
   struct DataType
   {
      StringArray    labels;
      Integer        id;
      DataType(StringArray labs, Integer pos)
      {
         labels = labs;
         id = pos;
      };
   };
   
   /// published data map
   std::map<GmatBase*, std::vector<DataType>* > providerMap;
   
   void                 UpdateProviderId(Integer newId);
   
   // for debug
   void                 ShowSubscribers();
   
   // default constructor
   Publisher();
   // assignment operator
   Publisher& operator=(const Publisher &right);
};



inline Gmat::RunState Publisher::GetRunState()
{
   return runState; 
}

#endif // Publisher_hpp
