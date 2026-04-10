//$Id$
//------------------------------------------------------------------------------
//                            SubscriberFactory
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
// Author: Wendy Shoan
// Created: 2003/10/22
//
/**
 *  Implementation code for the SubscriberFactory class, responsible for
 *  creating Subscriber objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "SubscriberFactory.hpp"
#include "ReportFile.hpp"
#include "TextEphemFile.hpp"
#include "MessageWindow.hpp"
#include "OpenGlPlot.hpp"
#include "XyPlot.hpp"
#include "EphemerisFile.hpp"
#include "OrbitView.hpp"
#include "GroundTrackPlot.hpp"
#include "DynamicDataDisplay.hpp"
#include "GroundTrack.hpp"

#include "OwnedPlot.hpp"
//#include "LocatorView.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Subscriber class
 * in generic way.
 *
 * @param <ofType> the Subscriber object to create and return.
 * @param <withName> the name to give the newly-created Subscriber object.
 *
 */
//------------------------------------------------------------------------------
GmatBase* SubscriberFactory::CreateObject(const std::string &ofType,
                                          const std::string &withName)
{
   return CreateSubscriber(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateSubscriber(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Subscriber class
 *
 * @param <ofType>   the Subscriber object to create and return.
 * @param <withName> the name to give the newly-created Subscriber object.
 *
 */
//------------------------------------------------------------------------------
Subscriber* SubscriberFactory::CreateSubscriber(const std::string &ofType,
                                                const std::string &withName)
{
   if (ofType == "ReportFile")
      return new ReportFile(ofType, withName);
   else if (ofType == "TextEphemFile")
      return new TextEphemFile(ofType, withName);
   else if (ofType == "MessageWindow")
      return new MessageWindow(withName);
   else if (ofType == "XYPlot")
      return new XyPlot(withName);
   else if (ofType == "EphemerisFile")
      return new EphemerisFile(withName);
   else if (ofType == "OpenGLPlot")
      return new OrbitView(withName);
   else if (ofType == "Enhanced3DView")
      return new OrbitView(withName);
   else if (ofType == "OrbitView")
      return new OrbitView(withName);
   else if (ofType == "GroundTrackPlot")
      return new GroundTrack(withName);
      // return new GroundTrackPlot(withName);
   else if (ofType == "GroundTrack")
      return new GroundTrack(withName);
   else if (ofType == "DynamicDataDisplay")
      return new DynamicDataDisplay(withName);

   // These are "owned" Subscribers
   else if (ofType == "OwnedPlot")
      return new OwnedPlot(withName);
//   else if (ofType == "LocatorView")
//      return new LocatorView(withName);
   
   return NULL;
}


//------------------------------------------------------------------------------
//  SubscriberFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SubscriberFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
SubscriberFactory::SubscriberFactory() :
Factory(Gmat::SUBSCRIBER)
{
   if (creatables.empty())
   {
      creatables.push_back("ReportFile");
      creatables.push_back("TextEphemFile");
      creatables.push_back("MessageWindow");
      creatables.push_back("XYPlot");
      creatables.push_back("EphemerisFile");
      creatables.push_back("OpenGLPlot");
      creatables.push_back("Enhanced3DView");
      creatables.push_back("OrbitView");
      creatables.push_back("GroundTrackPlot");
      creatables.push_back("GroundTrack");
      creatables.push_back("OwnedPlot");
      creatables.push_back("DynamicDataDisplay");
//      creatables.push_back("LocatorView");
   }
   
   // Now fill in unviewable subscribers
   // We don't want to show these items in the ResourceTree menu
   if (unviewables.empty())
   {
      // These commands do nothing
      unviewables.push_back("OpenGLPlot");
      unviewables.push_back("Enhanced3DView");

      // These are managed by other objects rather than from the tree
//      unviewables.push_back("LocatorView");
      unviewables.push_back("OwnedPlot");
   }

   GmatType::RegisterType(Gmat::SUBSCRIBER, "Subscriber");
   GmatType::RegisterType(Gmat::REPORT_FILE, "ReportFile");
   GmatType::RegisterType(Gmat::XY_PLOT, "XYPlot");
   GmatType::RegisterType(Gmat::ORBIT_VIEW, "OrbitView");
   GmatType::RegisterType(Gmat::EPHEMERIS_FILE, "EphemerisFile");
}


//------------------------------------------------------------------------------
// SubscriberFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SubscriberFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
SubscriberFactory::SubscriberFactory(StringArray createList) :
Factory(createList,Gmat::SUBSCRIBER)
{
}


//------------------------------------------------------------------------------
//  SubscriberFactory(const SubscriberFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SubscriberFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
SubscriberFactory::SubscriberFactory(const SubscriberFactory& fact) :
Factory(fact)
{
}


//------------------------------------------------------------------------------
// SubscriberFactory& operator= (const SubscriberFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SubscriberFactory class.
 *
 * @param <fact> the SubscriberFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" SubscriberFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
SubscriberFactory& SubscriberFactory::operator= (const SubscriberFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}


//------------------------------------------------------------------------------
// ~SubscriberFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the SubscriberFactory base class.
 *
 */
//------------------------------------------------------------------------------
SubscriberFactory::~SubscriberFactory()
{
   // deletes handled by Factory destructor
}

