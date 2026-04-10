//$Id: GmatPluginFunctions.cpp 9460 2011-04-21 22:03:28Z ravimathur $
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Ravi Mathur, Emergent Space Technologies, Inc.
// Created: December 16, 2014
//
/**
 * Implementation for library code interfaces.
 * 
 * This is prototype code.
 */
//------------------------------------------------------------------------------

#include "GmatPluginFunctions.hpp"
#include "DataCallbackFactory.hpp"
#include "DataCallback.hpp"

#include "Factory.hpp"
#include "MessageInterface.hpp"
#include "Moderator.hpp"

// Message from CInterface functions
std::string lastMsg = "";

extern "C"
{
   //------------------------------------------------------------------------------
   // Integer GetFactoryCount()
   //------------------------------------------------------------------------------
   /**
    * Returns the number of plug-in factories in this module
    *
    * @return The number of factories
    */
   //------------------------------------------------------------------------------
   Integer GetFactoryCount()
   {
     return 1;
   }
   
   //------------------------------------------------------------------------------
   // Factory* GetFactoryPointer(Integer index)
   //------------------------------------------------------------------------------
   /**
    * Retrieves a pointer to a specific factory
    *
    * @param index The index to the Factory
    *
    * @return The Factory pointer
    */
   //------------------------------------------------------------------------------
   Factory* GetFactoryPointer(Integer index)
   {
      Factory* factory = NULL;

      switch (index)
      {
         case 0:
            factory = new DataCallbackFactory;
            break;
            
         default:
            break;
      }

      return factory;
   }
   
   //------------------------------------------------------------------------------
   // void SetMessageReceiver(MessageReceiver* mr)
   //------------------------------------------------------------------------------
   /**
    * Sets the messaging interface used for GMAT messages
    *
    * @param mr The message receiver
    */
   //------------------------------------------------------------------------------
   void SetMessageReceiver(MessageReceiver* mr)
   {
      MessageInterface::SetMessageReceiver(mr);
   }

   //------------------------------------------------------------------------------
   // void SetCallback(const char* subscriberName, void (*CBFcn)(const double*, int, void*), void* userData)
   //------------------------------------------------------------------------------
   /**
    * Sets the callback function for a given DataCallback subscriber
    *
    * @param subscriberName Given name of the DataCallback subscriber
    * @param CBFcn User-provided callback function that will be called with an double array of requested data and user-specified additional data
    * @param userData User-specified additional data passed to CBFcn
    */
   //------------------------------------------------------------------------------
   int SetCallback(const char* subscriberName, void (*CBFcn)(const double*, int, void*), void* userData)
   {
     // Get GMAT moderator
     Moderator *theModerator = Moderator::Instance();
     if (theModerator == NULL)
     {
       lastMsg = "Cannot find the Moderator";
       return -1;
     }

     // Get subscriber with specified name
     Subscriber *sub = theModerator->GetSubscriber(subscriberName);
     if(sub == NULL)
     {
       lastMsg = "Subscriber not found";
       return -2;
     }

     // Make sure subscriber is a DataCallback (so callback can be set)
     DataCallback *dc = dynamic_cast<DataCallback*>(sub);
     if(dc == NULL)
     {
       lastMsg = "Subscriber is not a data callback";
       return -3;
     }

     // Set callback function
     dc->SetCallback(CBFcn, userData);

     lastMsg = "Callback successfully set";
     return 0;
   }

   //---------------------------------------------------------------------------
   // const char* getLastMessage()
   //---------------------------------------------------------------------------
   /**
    * Returns a status message.
    *
    * The getLastMessage function retrieves the most recent message generated by
    * the interface.  It lets users view the status of a command run through the
    * interface.
    *
    * @return The most recent message set by the interface.
    */
   //---------------------------------------------------------------------------
   const char* getLastMessage()
   {
     if (lastMsg == "")
       lastMsg = "getLastMessage() called; message is empty\n";
     return lastMsg.c_str();
   }
};
