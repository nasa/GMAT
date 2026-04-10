//$Id: GmatPluginFunctions.cpp 1398 2011-04-21 20:39:37Z  $
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/04/03
//
/**
 * Implementation for library code interfaces.
 *
 * This is prototype code.
 */
//------------------------------------------------------------------------------

#include "GmatPluginFunctions.hpp"
#include "MessageInterface.hpp"

#include "EstimatorFactory.hpp"
#include "EstimationCommandFactory.hpp"
#include "MeasurementModelFactory.hpp"
#include "EstimatorHardwareFactory.hpp"
#include "ErrorModelFactory.hpp"
#include "EstimationDataFilterFactory.hpp"
#include "NavPropagatorFactory.hpp"

#ifndef USE_DATAFILE_PLUGIN
	#include "DataFileFactory.hpp"
	#include "ObTypeFactory.hpp"
#endif

#include "EventManager.hpp"


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
	   #ifdef USE_DATAFILE_PLUGIN
   	   return 10;
   	#else 
   		return 8;
      #endif
   }

   //------------------------------------------------------------------------------
   // Integer GetTriggerManagerCount()
   //------------------------------------------------------------------------------
   /**
    * Returns the number of plug-in factories in this module
    *
    * @return The number of factories
    */
   //------------------------------------------------------------------------------
   Integer GetTriggerManagerCount()
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
            factory = new EstimationCommandFactory;
            break;

         case 1:
            factory = new EstimatorFactory;
            break;

         case 2:
            factory = new MeasurementModelFactory;
            break;

         //case 3:
         //   factory = new EventFactory;
         //   break;

         //case 4:
         case 3:
            factory = new EstimatorHardwareFactory;
            break;

         //case 5:
         case 4:
            factory = new EstimationDataFilterFactory;
            break;

         //case 6:
         case 5:
            factory = new ErrorModelFactory;
            break;

         //case 7:
         case 6:
            factory = new NavPropagatorFactory;
            break;

         #ifndef USE_DATAFILE_PLUGIN
            //case 8:
            case 7:
               factory = new DataFileFactory;
               break;

            //case 9:
            case 8:
               factory = new ObTypeFactory;
               break;
         #endif
         
         default:
            break;
      }

      return factory;
   }


   //------------------------------------------------------------------------------
   // TriggerManger* GetTriggerManager(Integer index)
   //------------------------------------------------------------------------------
   /**
    * Retrieves a pointer to a specific trigger manager
    *
    * @param index The index to the TriggerManager
    *
    * @return The TriggerManger pointer
    */
   //------------------------------------------------------------------------------
   TriggerManager* GetTriggerManager(Integer index)
   {
      TriggerManager* tm = NULL;

      switch (index)
      {
         case 0:
            tm = new EventManager;
            break;

         default:
            break;
      }

      return tm;
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
   // Integer GetMenuEntryCount()
   //------------------------------------------------------------------------------
   /**
    * Provides the number of new resource entries for teh GUI
    *
    * @return The number of entries
    */
   //------------------------------------------------------------------------------
   Integer GetMenuEntryCount()
   {
      return 3;
   }


   //------------------------------------------------------------------------------
   // Gmat::PluginResource* GetMenuEntry(Integer index)
   //------------------------------------------------------------------------------
   /**
    * Provides data needed for a menu/tree entry
    *
    * @param index The index of the new resource.
    *
    * @return The data structure with information about the resource type
    */
   //------------------------------------------------------------------------------
   Gmat::PluginResource* GetMenuEntry(Integer index)
   {
      Gmat::PluginResource* res = NULL;

      switch (index)
      {
         case 0:
            res = new Gmat::PluginResource;
            res->nodeName = "Simulators";
            res->parentNodeName = "Solvers";
            res->type = Gmat::SOLVER;
            res->subtype = "Simulator";
            break;

         case 1:
            res = new Gmat::PluginResource;
            res->nodeName = "Estimators";
            res->parentNodeName = "Solvers";
            res->type = Gmat::SOLVER;
            res->subtype = "Estimator";
            break;

         case 2:
            res = new Gmat::PluginResource;
            res->nodeName = "Measurements";
            res->parentNodeName = "";
            res->type = Gmat::MEASUREMENT_MODEL;
            res->subtype = "";
            break;

         default:
            break;
      }

      return res;
   }
};
