//$Id$
//------------------------------------------------------------------------------
//                            Publisher
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
// Created: 2003/09/21
//
/**
 *  Implementation code for the Publisher class.
 */
//------------------------------------------------------------------------------

#include "Publisher.hpp"
#include "PublisherException.hpp"
#include "MessageInterface.hpp"
#include "Moderator.hpp"
#include <string>
#include <algorithm>               // for find()


//#define DBGLVL_PUBLISHER_SUBSCRIBE 1
//#define DBGLVL_PUBLISHER_REGISTER 2
//#define DBGLVL_PUBLISHER_PUBLISH 2
//#define DBGLVL_PUBLISHER_CLEAR 1
//#define DEBUG_PUBLISHER_RUN_STATE
//#define DEBUG_PUBLISHER_BUFFERS


//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

// Initialize the singleton
Publisher* Publisher::instance = NULL;

//------------------------------------------------------------------------------
// Publisher* Instance(void)
//------------------------------------------------------------------------------
Publisher* Publisher::Instance()
{
   if (instance == NULL)
      instance = new Publisher;
   return instance;
}

//------------------------------------------------------------------------------
// Publisher(void)
//------------------------------------------------------------------------------
Publisher::Publisher() :
   providerId          (-1),
   currProviderId      (-1),
   runState            (Gmat::IDLE),
   maneuvering         (false),
   internalCoordSystem (NULL),
   dataCoordSystem     (NULL),
   dataMJ2000EqOrigin  (NULL) //,
   // Buffers used to avoid publishing repeated data
   //   dataBuffer          (NULL),
   //   countBuffer         (0)
{
}

//------------------------------------------------------------------------------
// ~Publisher(void)
//------------------------------------------------------------------------------
Publisher::~Publisher()
{
   subscriberList.clear();
   coordSysMap.clear();
   
   // Clear registered pub data
   std::map<GmatBase*, std::vector<DataType>* >::iterator iter = providerMap.begin();
   while (iter != providerMap.end())
   {
      std::vector<DataType>* dataList = iter->second;
      if (dataList != NULL)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (dataList, "dataList", "Publisher::UnregisterPublishedData()", "deleting dataList");
         #endif
         
         delete dataList;
      }
      iter++;
   }
   // Buffers used to avoid publishing repeated data
   //   if (dataBuffer)
   //      delete [] dataBuffer;
}

//------------------------------------------------------------------------------
// bool Subscribe(Subscriber *s)
//------------------------------------------------------------------------------
bool Publisher::Subscribe(Subscriber *s)
{
   #if DBGLVL_PUBLISHER_SUBSCRIBE
   MessageInterface::ShowMessage
      ("Publisher::Subscribe() sub = <%p><%s>'%s'\n", s, s->GetTypeName().c_str(),
       s->GetName().c_str());
   #endif
   
   if (!s)
      return false;
   
   if (s->GetType() != Gmat::SUBSCRIBER)
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** Publisher::Subscribe() Cannot add non-Subscriber object "
          "'%s'. It is type of '%s'\n", s->GetName().c_str(), s->GetTypeName().c_str());
      return false;
   }
   
   // Add if subscriber is already not in the list (LOJ: 2009.04.08)
   if (find(subscriberList.begin(), subscriberList.end(), s) == subscriberList.end())
   {
      subscriberList.push_back(s);
      s->SetProviderId(currProviderId);
      #if DBGLVL_PUBLISHER_SUBSCRIBE
      MessageInterface::ShowMessage
         ("   Adding <%p>'%s' to subscriber list\n", s, s->GetName().c_str());
      ShowSubscribers();
      #endif
   }
   else
   {
      #if DBGLVL_PUBLISHER_SUBSCRIBE
      MessageInterface::ShowMessage
         ("   <%p>'%s' was already added\n", s, s->GetName().c_str());
      #endif      
   }
   
   return true;
}

//------------------------------------------------------------------------------
// bool Unsubscribe(Subscriber *s)
//------------------------------------------------------------------------------
bool Publisher::Unsubscribe(Subscriber *s)
{
   if (!s)
      return false;
   
   if (subscriberList.empty())
   {
      #if DBGLVL_PUBLISHER_SUBSCRIBE
      MessageInterface::ShowMessage
         ("Publisher::Unsubscribe() sub = <%p>'%s' returning false, "
          "the subscriber list is empty\n", s, s->GetName().c_str());
      #endif
      return false;
   }
   
   #if DBGLVL_PUBLISHER_SUBSCRIBE
   MessageInterface::ShowMessage
      ("Publisher::Unsubscribe() sub = <%p>'%s'\n", s, s->GetName().c_str());
   #endif
   
   #if DBGLVL_PUBLISHER_SUBSCRIBE
   MessageInterface::ShowMessage("   About to remove <%p> from the list\n", s);
   ShowSubscribers();
   #endif
   
   subscriberList.remove(s);
   
   #if DBGLVL_PUBLISHER_SUBSCRIBE
   MessageInterface::ShowMessage("Publisher::Unsubscribe() returning true\n");
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
// bool UnsubscribeAll()
//------------------------------------------------------------------------------
bool Publisher::UnsubscribeAll()
{
   #if DBGLVL_PUBLISHER_SUBSCRIBE
   MessageInterface::ShowMessage
      ("Publisher::UnsubscribeAll() entered. Clearing %d subscribers\n",
       subscriberList.size());
   #endif
   
   subscriberList.clear();
   
   ClearPublishedData();
   
   // delete locally created coordinate systems
   std::map<std::string, CoordinateSystem*>::iterator iter;
   for (iter = coordSysMap.begin(); iter != coordSysMap.end(); ++iter)
   {
      if ((iter->first).find("Local-") != std::string::npos)
      {
         #if DBGLVL_PUBLISHER
         MessageInterface::ShowMessage
            ("Publisher::UnsubscribeAll() deleting %s\n", iter->first.c_str());
         #endif
         
         delete iter->second;
      }
   }
   
   coordSysMap.clear();
   return true;
}


//------------------------------------------------------------------------------
// bool Publish(GmatBase *provider, Integer id, Real *data, Integer count, ...)
//------------------------------------------------------------------------------
/**
 * This method is called from the propagation enabled command to publish data
 * so that subscribers can receive data.
 *
 * @param provider The command who is calling this method
 * @param id       The id given by the Publisher
 * @param data     The real type data to publish
 * @param count    Number of data points to publish
 * @param propDir  The direction of propagation (1.0 = forward, -1.0 = backward,
 *
 * Settings to only publish to ephems in step mode.  Remove these cases to
 * implement GMT-6110
 *                                               2.0 = forward, ephems only,
 *                                               -2.0 = backwards, ephems only)
 */
//------------------------------------------------------------------------------
bool Publisher::Publish(GmatBase *provider, Integer id, Real *data, Integer count,
                        Real propDir)
{
   #if DBGLVL_PUBLISHER_PUBLISH
   MessageInterface::ShowMessage
      ("Publisher::Publish(Real) entered, provider=<%p><%s>, id=%d, "
       "currProviderId=%d, count=%d, propDir=%s\n", provider, provider->GetTypeName().c_str(),
       id, currProviderId, count, propDir == 1.0 ? "forward" : "backward");
   MessageInterface::ShowMessage("   providerMap.size()=%d\n", providerMap.size());
   #endif

   //   // Trap and do not publish identical data twice in a row
   //   bool isRepeated = false;
   //
   // The commented code here ensures that repeated data is not published,  This
   // is turned off in the current implementation
   //   if (count != 0)
   //   {
   //      if (countBuffer != count)
   //      {
   //         if (dataBuffer)
   //            delete [] dataBuffer;
   //         dataBuffer = new Real[count];
   //         countBuffer = count;
   //         memcpy(dataBuffer, data, count*sizeof(Real));
   //      }
   //      else
   //      {
   //         if (memcmp(data, dataBuffer, count*sizeof(Real)) == 0)
   //            isRepeated = true;
   //         else
   //            memcpy(dataBuffer, data, count*sizeof(Real));
   //      }
   //   }
   //
   //   if (!isRepeated)
   //   {

      if ((count > 0) && (data[0] >= 6116.0) && (data[0] <= 58127.5))
         GmatGlobal::Instance()->SetCurrentEpoch(data[0]);

      // No subscribers
      if (subscriberList.empty())
      {
         #if DBGLVL_PUBLISHER_PUBLISH
         MessageInterface::ShowMessage
            ("*** WARNING *** Publisher::Publish() There are no subscribers, "
             "so just returning false\n");
         #endif
         return false;
      }

      // Check for no providers
      std::map<GmatBase*, std::vector<DataType>* >::iterator iter = providerMap.find(provider);
      if (iter == providerMap.end())
      {
         #if DBGLVL_PUBLISHER_PUBLISH
         MessageInterface::ShowMessage
            ("*** WARNING *** Publisher::Publish() There are no registered providers, "
             "so just returning false\n");
         #endif
         return false;
      }

      if (id != currProviderId)
      {
         currProviderId = id;
         UpdateProviderId(id);
      }


      // Get data labels
      std::vector<DataType>* dataList = iter->second;
   
      #if DBGLVL_PUBLISHER_PUBLISH > 1
      MessageInterface::ShowMessage("   dataList->size()=%d\n", dataList->size());
      #endif

      // Convert the data into a string for distribution
      Integer length = count*25 + 1;
      char *stream = new char[length];
   
      #ifdef DEBUG_PUBLISHER_BUFFERS
         MessageInterface::ShowMessage("Allocated %d chars at %p\n", length,
               stream);
      #endif
   
      stream[0] = '\0';    // Init to empty string

      for (Integer i = 0; i < count; ++i)
      {
         #ifdef DEBUG_PUBLISHER_BUFFERS
            MessageInterface::ShowMessage("   %d: %12lf\n", i, data[i]);
         #endif
         sprintf(stream, "%s%16le", stream, data[i]);
         if (i < count - 1)
            strcat(stream, ", ");
         else
            strcat(stream, "\n");
         #ifdef DEBUG_PUBLISHER_BUFFERS
               MessageInterface::ShowMessage("   used %d\n", strlen(stream));
         #endif
      }
   
      #ifdef DEBUG_PUBLISHER_BUFFERS
         MessageInterface::ShowMessage("   Data:  %s\n", stream);
      #endif
   
      #if DBGLVL_PUBLISHER_PUBLISH
      MessageInterface::ShowMessage
         ("Publisher::Publish() calling ReceiveData() number of subsbribers = %d\n",
          subscriberList.size());
      #endif
      
      // Code to only publish to ephems in step mode.  Remove this to
      // implement GMT-6110
      bool ephemsOnly = ((propDir == 2.0) || (propDir == -2.0));

      std::list<Subscriber*>::iterator current = subscriberList.begin();
      while (current != subscriberList.end())
      {
         #ifdef DEBUG_PUBLISHER_BUFFERS
            MessageInterface::ShowMessage("   Publishing to %s\n",
                  (*current)->GetName().c_str());
         #endif

         #if DBGLVL_PUBLISHER_PUBLISH > 1
         MessageInterface::ShowMessage
            ("Publisher::Publish() sub = <%p><%s>'%s'\n", (*current),
             (*current)->GetTypeName().c_str(), (*current)->GetName().c_str());
         #endif

         // Set labels
         #if DBGLVL_PUBLISHER_PUBLISH > 1
         MessageInterface::ShowMessage
            ("Publisher::Publish() Setting data labels:\n");
         StringArray dataLabels = (*dataList)[id].labels;
         for (unsigned int ii = 0; ii < dataLabels.size(); ii++)
            MessageInterface::ShowMessage("%s ", dataLabels[ii].c_str());
         MessageInterface::ShowMessage("\n");
         #endif

         // Code to only publish to ephems in step mode.  Remove this to
         // implement GMT-6110
         if (ephemsOnly && !((*current)->IsOfType(Gmat::EPHEMERIS_FILE)))
         {
            ++current;
            continue;
         }

         (*current)->SetDataLabels((*dataList)[id].labels);

         // Set provider
         if (count > 0)
            (*current)->SetProvider(provider, data[0]);
         else
            (*current)->SetProvider(provider);

         // Set propagation direction
         (*current)->SetPropagationDirection((propDir > 0.0 ? 1.0 : -1.0));

         if (!(*current)->ReceiveData(stream))
            return false;
         if (!(*current)->ReceiveData(data, count))
            return false;
         current++;
      }

      #ifdef DEBUG_PUBLISHER_BUFFERS
         MessageInterface::ShowMessage("   Cleaning up\n");
      #endif
      delete [] stream;
      //   }  End of the repeated data check block

   #if DBGLVL_PUBLISHER_PUBLISH
   MessageInterface::ShowMessage("Publisher::Publish() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Publish(char *data, Integer count = 0)
//------------------------------------------------------------------------------
bool Publisher::Publish(Integer id, char *data, Integer count)
{
   Integer i;
    
   // No subscribers
   if (subscriberList.empty())
      return false;

   #if DBGLVL_PUBLISHER_PUBLISH > 1
   MessageInterface::ShowMessage("Publisher::Publish(char) id = %d\n", id);
   #endif
   
   if ((id < 0) || (id > providerId))
   {
      throw PublisherException
         ("Character data provider has not registered with the Publisher.");
   }
   
   if (id != currProviderId)
   {
      currProviderId = id;
      UpdateProviderId(id);
   }
   
   // Convert the data into a string for distribution
   Integer length;
   if (count)
        length = count + 1;
   else
        length = strlen(data) + 1;
        
   char *stream = new char[length];

   if (count)
   {
      for (i = 0; i < count; ++i)
         stream[i] = data[i];
      stream[i] = '\0';
   }
   else
   {
      strcpy(stream, data);
   }
   
   strcat(stream, "\n");

   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      if (!(*current)->ReceiveData(stream))
         return false;
      current++;
   }

   delete [] stream;
   return true;
}

//------------------------------------------------------------------------------
// bool Publish(Integer id, Integer *data, Integer count = false)
//------------------------------------------------------------------------------
bool Publisher::Publish(Integer id, Integer *data, Integer count)
{
   // No subscribers
   if (subscriberList.empty())
      return false;
   
   #if DBGLVL_PUBLISHER_PUBLISH > 1
   MessageInterface::ShowMessage("Publisher::Publish(Integer) id = %d\n", id);
   #endif
   
   if ((id < 0) || (id > providerId))
      throw PublisherException
         ("Integer data provider has not registered with the Publisher.");
   
   if (id != currProviderId)
   {
      currProviderId = id;
      UpdateProviderId(id);
   }
   
   // Convert the data into a string for distribution
   char *stream = new char[count*25 + 1];
   stream[0] = '\0';
   
   for(Integer i = 0; i < count; ++i)
   {
      sprintf(stream, "%s%d", stream, data[i]);
      if (i < count - 1)
         strcat(stream, ", ");
      else
         strcat(stream, "\n");
   }
   
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      if (!(*current)->ReceiveData(stream))
         return false;
      current++;
   }

   delete [] stream;
   return true;
}


//------------------------------------------------------------------------------
// void Ping()
//------------------------------------------------------------------------------
/**
 * Pings the system so that GUI overrides can hand control off for messages.
 */
//------------------------------------------------------------------------------
void Publisher::Ping()
{
}


//------------------------------------------------------------------------------
// bool FlushBuffers(bool endOfDataBlock = true)
//------------------------------------------------------------------------------
/**
 * Indicates end of data stream.
 */
//------------------------------------------------------------------------------
bool Publisher::FlushBuffers(bool endOfDataBlock)
{
   // No subscribers
   if (subscriberList.empty())
      return false;
   
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      if (!(*current)->FlushData(endOfDataBlock))
         return false;
      current++;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool NotifyEndOfRun()
//------------------------------------------------------------------------------
bool Publisher::NotifyEndOfRun()
{
   // No subscribers
   if (subscriberList.empty())
      return false;
   
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      if (!(*current)->SetEndOfRun())
         return false;
      current++;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// const std::list<Subscriber*> GetSubscriberList()
//------------------------------------------------------------------------------
const std::list<Subscriber*> Publisher::GetSubscriberList()
{
   return subscriberList;
}


//------------------------------------------------------------------------------
// void ClearPublishedData()
//------------------------------------------------------------------------------
void Publisher::ClearPublishedData()
{   
   #if DBGLVL_PUBLISHER_CLEAR
   MessageInterface::ShowMessage
      ("Publisher::ClearPublishedData() entered, clearing %d element owner objects\n",
       objectArray.size());
   MessageInterface::ShowMessage("   providerMap.size()=%u\n", providerMap.size());
   MessageInterface::ShowMessage("   subscriberList.size()=%u\n", subscriberList.size());
   #endif
   
   objectArray.clear();
   elementArray.clear();
   providerId = -1;
   currProviderId = -1;
   
   // Clear subscriber's data
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      (*current)->ClearDataLabels();
      current++;
   }
   
   #if DBGLVL_PUBLISHER_CLEAR > 1
   MessageInterface::ShowMessage
      ("Publisher::ClearPublishedData() Using new Publisher code\n");
   #endif
   
   std::map<GmatBase*, std::vector<DataType>* >::iterator iter = providerMap.begin();
   while (iter != providerMap.end())
   {
      std::vector<DataType>* dataList = iter->second;
      if (dataList != NULL)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (dataList, "dataList", "Publisher::ClearPublishedData()", "deleting dataList");
         #endif
         
         delete dataList;
      }
      iter++;
   }
   
   providerMap.clear();
   subscriberList.clear();
   
   #if DBGLVL_PUBLISHER_CLEAR
   MessageInterface::ShowMessage
      ("Publisher::ClearPublishedData() leaving, providerMap.size()=%u\n",
       providerMap.size());
   #endif
}


//------------------------------------------------------------------------------
// Integer RegisterPublishedData(GmatBase *provider, Integer id,
//         const StringArray& owners, const StringArray& elements)
//------------------------------------------------------------------------------
/*
 * Registers provider with data elements. This method passes elements as data
 * labels to subscribers by calling SetDataLabels(elements).
 *
 * @param  provider  Provider who calls publiser
 * @param  id        Provider id, if id is -1 it will assign new id
 * @param  owners    Object names, such as spacecraft name
 * @param  elements  Element names, such as Sat1.X, Sat1.Y
 *
 * @return  new provider id or existing id
 *
 * @note Currently only Propagate command registers and publishes orbit
 *       trajectory data. The owner is not currently used since elements has owner
 *       name, such as Sat1.X which Sat1 is the owner name.
 */
//------------------------------------------------------------------------------
Integer Publisher::RegisterPublishedData(GmatBase *provider, Integer id,
                                         const StringArray& owners, 
                                         const StringArray& elements)
{
   #if DBGLVL_PUBLISHER_REGISTER
   MessageInterface::ShowMessage
      ("Publisher::RegisterPublishedData() entered, <%p><%s> passed id: %d, %d "
       "owners and %d elements\n", provider, provider->GetTypeName().c_str(), id,
       owners.size(), elements.size());
   MessageInterface::ShowMessage("   providerMap.size()=%d\n", providerMap.size());
   #endif
   
   #if DBGLVL_PUBLISHER_REGISTER > 1
   for (unsigned int i=0; i<owners.size(); i++)
      MessageInterface::ShowMessage("   owner[%d]= <%s>  elements[%d] = <%s>\n", 
         i, owners[i].c_str(), i, elements[i].c_str());
   MessageInterface::ShowMessage
      ("   providerId=%d, subscriberList.size()=%d\n", providerId, subscriberList.size());
   #endif
   
   if (subscriberList.empty())
   {
      // Let's just show warning if debug is turned on
      //throw PublisherException("There are no registered subscribers.");
      #if DBGLVL_PUBLISHER_REGISTER
      MessageInterface::ShowMessage
         ("*** WARNING *** Publisher::RegisterPublishedData() There are no "
          "subscribers to register data\n");
      MessageInterface::ShowMessage
         ("Publisher::RegisterPublishedData() returning %d\n", providerId);
      #endif
      return providerId;
   }
   
   Integer actualId = -1;
   
   #if DBGLVL_PUBLISHER_REGISTER > 1
   MessageInterface::ShowMessage
      ("Publisher::RegisterPublishedData() Using new Publisher code\n");
   #endif
   
   if (id != -1)
   {
      #if DBGLVL_PUBLISHER_REGISTER
      MessageInterface::ShowMessage
         ("Publisher::RegisterPublishedData() returning %d, provider already has "
          "registered with data\n", id);
      #endif
      
      // Just set current run state
      std::list<Subscriber*>::iterator current = subscriberList.begin();
      while (current != subscriberList.end())
      {
         #if DBGLVL_PUBLISHER_REGISTER > 1
         MessageInterface::ShowMessage
            ("   calling <%s>->SetRunState()\n", (*current)->GetName().c_str());
         #endif
         
         (*current)->SetRunState(runState);
         current++;
      }
      return id;
   }
   
   // Add publishing data object names
   std::map<GmatBase*, std::vector<DataType>* >::iterator iter =
      providerMap.find(provider);
   if (!owners.empty())
   {
      //@tbd Should it be cleared first? (LOJ: 2009.11.04)
      objectArray.clear();
      objectArray.push_back(StringArray(owners));
   }
   
   // @note
   // New Publisher code doesn't use global providerId anymore. It keeps
   // track of provider's data labels and ids and copies corresponding
   // labels to current data labels before distributing the data in Publish().
   // There was an issue with provider id being kept incrementing if data is
   // registered and published inside a GmatFunction
   if (!elements.empty())
   {
      if (iter == providerMap.end())
      {
         #if DBGLVL_PUBLISHER_REGISTER
         MessageInterface::ShowMessage("==> provider not found\n");
         #endif
         
         // create new dataList
         actualId = 0;
         DataType newData(elements, actualId);
         
         std::vector<DataType>* dataList = new std::vector<DataType>;
         
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Add
            (dataList, "dataList", "Publisher::RegisterPublishedData()", "creating dataList");
         #endif
         
         dataList->push_back(newData);
         providerMap.insert(std::make_pair(provider, dataList));
      }
      else
      {
         #if DBGLVL_PUBLISHER_REGISTER
         MessageInterface::ShowMessage("==> provider found\n");
         #endif
         
         std::vector<DataType>* oldList = iter->second;
         actualId = oldList->size();
         DataType newData(elements, actualId);
         oldList->push_back(newData);
      }
   }
   
   #if DBGLVL_PUBLISHER_REGISTER > 1
   std::map<GmatBase*, std::vector<DataType>* >::iterator iter2 = providerMap.find(provider);
   std::vector<DataType>* tempList = iter2->second;
   MessageInterface::ShowMessage
      ("   actualId=%d, tempList->size() = %d\n", actualId, tempList->size());
   #endif
   
   providerId = actualId;
   
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      #if DBGLVL_PUBLISHER_REGISTER > 1
      MessageInterface::ShowMessage
         ("   calling <%s>->SetDataLabels()\n", (*current)->GetName().c_str());
      #endif
      
      (*current)->SetDataLabels(elements);
      (*current)->SetRunState(runState);
      current++;
   }
   
   #if DBGLVL_PUBLISHER_REGISTER
   MessageInterface::ShowMessage("   providerMap.size()=%d\n", providerMap.size());
   MessageInterface::ShowMessage
      ("Publisher::RegisterPublishedData() provider=<%p> returning %d\n",
       provider, actualId);
   #endif
   
   return actualId;
}


//------------------------------------------------------------------------------
// UnregisterPublishedData(GmatBase *provider)
//------------------------------------------------------------------------------
void Publisher::UnregisterPublishedData(GmatBase *provider)
{
   #if DBGLVL_PUBLISHER_REGISTER
   MessageInterface::ShowMessage
      ("Publisher::UnregisterPublishedData() entered, <%p><%s>, providerMap.size() = %d\n",
       provider, provider->GetTypeName().c_str(), providerMap.size());
   #endif
   
   std::map<GmatBase*, std::vector<DataType>* >::iterator iter = providerMap.find(provider);
   if (iter != providerMap.end())
   {
      std::vector<DataType>* dataList = iter->second;
      if (dataList != NULL)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (dataList, "dataList", "Publisher::UnregisterPublishedData()", "deleting dataList");
         #endif
         
         delete dataList;
      }
      providerMap.erase(iter);
   }
   else
   {
      #if DBGLVL_PUBLISHER_REGISTER
      MessageInterface::ShowMessage
         ("*** WARNING *** Publisher::UnregisterPublishedData() provider <%p> "
          "not registered\n");
      #endif
   }
   
   #if DBGLVL_PUBLISHER_REGISTER
   MessageInterface::ShowMessage
      ("Publisher::UnregisterPublishedData() leaving, providerMap.size() = %d\n",
       providerMap.size());
   #endif
   
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& type)
//------------------------------------------------------------------------------
const StringArray& Publisher::GetStringArrayParameter(const std::string& type)
{
   if ((currProviderId < 0) || (currProviderId > providerId))
      throw PublisherException("Data provider id out of range.");
   
   if (type == "SpaceObjectMap")
      return objectArray[currProviderId];
   
   if (type == "PublishedDataMap")
      return elementArray[currProviderId];
   
   throw PublisherException("Unknown StringArray type requested.");
}


//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * Sets the internal coordinate system.
 *
 * @parameter  cs  Internal coordinate system (usally EarthMJ2000Eq)
 */
//------------------------------------------------------------------------------
void Publisher::SetInternalCoordSystem(CoordinateSystem *cs)
{
   if (cs != NULL)
      internalCoordSystem = cs;
}


//------------------------------------------------------------------------------
// void SetDataCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * Sets the coordinate system of data to subscribers. It also adds the coordinate
 * system to the map, if it is not found.
 *
 * @parameter  cs  Coordinate system which data represents
 */
//------------------------------------------------------------------------------
void Publisher::SetDataCoordSystem(CoordinateSystem *cs)
{
   if (cs == NULL)
      return;
   
   dataCoordSystem = cs;
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      (*current)->SetDataCoordSystem(cs);
      current++;
   }
   
   // if coordinate system not found in the map, add
   std::string csName = cs->GetName();
   if (coordSysMap.find(csName) != coordSysMap.end())
      coordSysMap[csName] = cs;
}


//------------------------------------------------------------------------------
// void SetDataMJ2000EqOrigin(CelestialBody *cb)
//------------------------------------------------------------------------------
/*
 * Sets origin of MJ2000Eq coordinate system which data represents. If matching
 * coordinate system name found from the map, it sets to data coordinate system,
 * otherwise, it creates new coordinate system.
 *
 * @parameter  cb  Origin of MJ2000Eq coordinate system which data represents
 */
//------------------------------------------------------------------------------
void Publisher::SetDataMJ2000EqOrigin(CelestialBody *cb)
{
   if (cb == NULL)
      return;
   
   #if DBGLVL_PUBLISHER_DATA_REP
   MessageInterface::ShowMessage
      ("Publisher::SetDataMJ2000EqOrigin() cb=%s<%p>\n", cb->GetName().c_str(),
       cb);
   #endif
   
   dataMJ2000EqOrigin = cb;
   std::string originName = cb->GetName();
   std::string csName = originName + "MJ2000Eq";
      
   std::map<std::string, CoordinateSystem*>::iterator csIter =
      coordSysMap.find(csName);
   
   if (coordSysMap.find(csName) != coordSysMap.end())
   {
      // set coordinate system from the map
      dataCoordSystem = coordSysMap.find(csName)->second;
   }
   else
   {
      // check as local name
      csName = "Local-" + csName;
      if (coordSysMap.find(csName) != coordSysMap.end())
         dataCoordSystem = coordSysMap.find(csName)->second;
      else
      {
         // Create coordinate system if not exist
         CoordinateSystem *newCs = (CoordinateSystem*)internalCoordSystem->Clone();
         newCs->SetName(csName);
         newCs->SetStringParameter("Origin", originName);
         newCs->SetRefObject(cb, Gmat::CELESTIAL_BODY, originName);
         newCs->Initialize();
         coordSysMap[csName] = newCs;
         dataCoordSystem = newCs;
         
         #if DBGLVL_PUBLISHER_DATA_REP
         MessageInterface::ShowMessage
            ("   ===> %s not found in the map, so created <%p>\n", csName.c_str(),
             newCs);
         #endif
      }
   }
   
   // set to subscribers
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      (*current)->SetDataMJ2000EqOrigin(cb);
      (*current)->SetDataCoordSystem(dataCoordSystem);
      current++;
   }
}


//------------------------------------------------------------------------------
// void SetRunState(const Gmat::RunState state)
//------------------------------------------------------------------------------
void Publisher::SetRunState(const Gmat::RunState state)
{
   #ifdef DEBUG_PUBLISHER_RUN_STATE
   MessageInterface::ShowMessage
      ("Publisher::SetRunState() entered, setting run state %d to "
       "%d subscribers\n", state, subscriberList.size());
   #endif
   
   runState = state;
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      #ifdef DEBUG_PUBLISHER_RUN_STATE
      MessageInterface::ShowMessage("   current=<%p>\n", *current);
      #endif
      
      (*current)->SetRunState(runState);
      current++;
   }
   
   #ifdef DEBUG_PUBLISHER_RUN_STATE
   MessageInterface::ShowMessage("Publisher::SetRunState() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetManeuvering(GmatBase *originator, bool flag, Real epoch,
//                     const std::string &satName, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * Sets single spacecraft maneuvering flag.
 *
 * @param originator  The maneuver command pointer who is maneuvering
 * @param flag  Set to true if maneuvering
 * @param epoch  Epoch of maneuver
 * @param satName  Name of the maneuvering spacecraft
 * @param desc  Description of maneuver (e.g. impulsive or finite)
 */
//------------------------------------------------------------------------------
void Publisher::SetManeuvering(GmatBase *originator, bool flag, Real epoch,
                               const std::string &satName,
                               const std::string &desc)
{
   maneuvering = flag;
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      (*current)->SetManeuvering(originator, flag, epoch, satName, desc);
      current++;
   }
}


//------------------------------------------------------------------------------
// void SetManeuvering(GmatBase *originator, bool flag, Real epoch,
//                     const StringArray &satNames, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * Sets multiple spacecrafts maneuvering flag.
 * 
 * @param originator  The maneuver command pointer who is maneuvering
 * @param flag  Set to true if maneuvering
 * @param epoch  Epoch of maneuver
 * @param satNames  Names of the maneuvering spacecraft
 * @param desc  Description of maneuver (e.g. impulsive or finite)
 */
//------------------------------------------------------------------------------
void Publisher::SetManeuvering(GmatBase *originator, bool flag, Real epoch,
                               const StringArray &satNames,
                               const std::string &desc)
{
   maneuvering = flag;
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      (*current)->SetManeuvering(originator, flag, epoch, satNames, desc);
      current++;
   }
}


//------------------------------------------------------------------------------
// bool GetManeuvering()
//------------------------------------------------------------------------------
bool Publisher::GetManeuvering()
{
   return maneuvering;
}


//------------------------------------------------------------------------------
// void SetSpacecraftPropertyChanged(GmatBase *originator, Real epoch,
//                           const std::string &satName, const std::string &desc)
//------------------------------------------------------------------------------
/**
 * Sets spacecraft property change so that subscribers can handle appropriately.
 * 
 * @param originator  The assignment command pointer who is setting
 * @param epoch  Epoch of spacecraft at property change
 * @param satName  Name of the spacecraft
 * @param desc  Description of property change
 */
//------------------------------------------------------------------------------
void Publisher::SetSpacecraftPropertyChanged(GmatBase *originator, Real epoch,
                                             const std::string &satName,
                                             const std::string &desc)
{
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      (*current)->SetSpacecraftPropertyChanged(originator, epoch, satName, desc);
      current++;
   }
}


//------------------------------------------------------------------------------
// void SetOrbitColorChanged(GmatBase *originator, const std::string &newColor, ...)
//------------------------------------------------------------------------------
/**
 * Sets object orbit color change so that subscribers can handle appropriately.
 * 
 * @param originator  The assignment command pointer who is setting
 * @param newColor  New color to be applied to the object
 * @param objName  Name of the object
 * @param desc  Description of property change
 */
//------------------------------------------------------------------------------
void Publisher::SetOrbitColorChanged(GmatBase *originator,
                                     const std::string &newColor,
                                     const std::string &objName,
                                     const std::string &desc)
{
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      (*current)->SetOrbitColorChanged(originator, newColor, objName, desc);
      current++;
   }
}


//------------------------------------------------------------------------------
// void SetTargetColorChanged(GmatBase *originator, const std::string &newColor, ...)
//------------------------------------------------------------------------------
/**
 * Sets object target color change so that subscribers can handle appropriately.
 * 
 * @param originator  The assignment command pointer who is setting
 * @param newColor  New color to be applied to the object
 * @param objName  Name of the object
 * @param desc  Description of property change
 */
//------------------------------------------------------------------------------
void Publisher::SetTargetColorChanged(GmatBase *originator,
                                      const std::string &newColor,
                                      const std::string &objName,
                                      const std::string &desc)
{
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      (*current)->SetTargetColorChanged(originator, newColor, objName, desc);
      current++;
   }
}

//------------------------------------------------------------------------------
// void SetSegmentOrbitColor(GmatBase *originator, bool overrideColor,
//                           UnsignedInt orbitColor)
//------------------------------------------------------------------------------
/**
 * Sets propagation segment orbit color so that subscribers can handle appropriately.
 * 
 * @param originator  The Propagate command pointer who is setting
 * @param overrideColor  The flag indicating whether or not to override orbit color
 * @param orbitColor  New orbit color to be applied to the space object
 */
//------------------------------------------------------------------------------
void Publisher::SetSegmentOrbitColor(GmatBase *originator, bool overrideColor,
                                     UnsignedInt orbitColor, const StringArray &objNames)
{
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      (*current)->SetSegmentOrbitColor(originator, overrideColor, orbitColor, objNames);
      current++;
   }
}

//------------------------------------------------------------------------------
// void UpdateProviderId(Integer newId)
//------------------------------------------------------------------------------
void Publisher::UpdateProviderId(Integer newId)
{
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      (*current)->SetProviderId(newId);
      current++;
   }
}


//------------------------------------------------------------------------------
// void ShowSubscribers()
//------------------------------------------------------------------------------
void Publisher::ShowSubscribers()
{
   if (subscriberList.empty())
   {
      MessageInterface::ShowMessage("   ===== There are no subscribers\n");
      return;
   }
   
   MessageInterface::ShowMessage
      ("   ===== There are %d subscriber(s)\n", subscriberList.size());
   
   std::list<Subscriber*>::iterator current = subscriberList.begin();
   while (current != subscriberList.end())
   {
      MessageInterface::ShowMessage
         ("   <%p>'%s'\n", *current, (*current)->GetName().c_str());
      current++;
   }
}

