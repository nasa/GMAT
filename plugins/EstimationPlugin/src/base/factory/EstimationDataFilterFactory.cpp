//------------------------------------------------------------------------------
//                         EstimationDataFilterFactory
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
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2009/03/03
//
/**
 * Factory used to create EstimationDataFilter objects.
 */
//------------------------------------------------------------------------------


#include "EstimationDataFilterFactory.hpp"
//#include "StatisticAcceptFilter.hpp"
//#include "StatisticRejectFilter.hpp"
#include "AcceptFilter.hpp"
#include "RejectFilter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_CONSTRUCTOR

//-----------------------------------------------------------------------------
// EstimationDataFilterFactory()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
EstimationDataFilterFactory::EstimationDataFilterFactory() :
   Factory     (Gmat::DATA_FILTER)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory default constructor\n");
#endif

   if (creatables.empty())
   {
      /// StatisticsAcceptFilter and StatisticsRejectFilter are nolonger been used in GMAT
      //creatables.push_back("StatisticsAcceptFilter");
      //creatables.push_back("StatisticsRejectFilter");

      creatables.push_back("AcceptFilter");
      creatables.push_back("RejectFilter");
   }
   GmatType::RegisterType(Gmat::DATA_FILTER, "DataFilter");
}


//-----------------------------------------------------------------------------
// ~EstimationDataFilterFactory()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
EstimationDataFilterFactory::~EstimationDataFilterFactory()
{
}


//-----------------------------------------------------------------------------
// EstimationDataFilterFactory(StringArray createList)
//-----------------------------------------------------------------------------
/**
 * Secondary constructor designed to override the names of the creatables
 *
 * @param createList The new list of creatable objects
 */
//-----------------------------------------------------------------------------
EstimationDataFilterFactory::EstimationDataFilterFactory(StringArray createList) :
   Factory        (createList, Gmat::DATA_FILTER)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory(names) default constructor\n");
#endif

   if (creatables.empty())
   {
      /// StatisticsAcceptFilter and StatisticsRejectFilter are nolonger been used in GMAT
      //creatables.push_back("StatisticsAcceptFilter");
      //creatables.push_back("StatisticsRejectFilter");

      creatables.push_back("AcceptFilter");
      creatables.push_back("RejectFilter");
   }
}


//-----------------------------------------------------------------------------
// EstimationDataFilterFactory(const EstimationDataFilterFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fact The factory that gets copied here
 */
//-----------------------------------------------------------------------------
EstimationDataFilterFactory::EstimationDataFilterFactory(const EstimationDataFilterFactory& fact) :
   Factory        (fact)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory copy constructor\n");
#endif

   if (creatables.empty())
   {
      /// StatisticsAcceptFilter and StatisticsRejectFilter are nolonger been used in GMAT
      //creatables.push_back("StatisticsAcceptFilter");
      //creatables.push_back("StatisticsRejectFilter");

      creatables.push_back("AcceptFilter");
      creatables.push_back("RejectFilter");
   }
}


//-----------------------------------------------------------------------------
// EstimationDataFilterFactory& operator= (const EstimationDataFilterFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fact The factory that gets copied here
 *
 * @return This factory, configured to match fact
 */
//-----------------------------------------------------------------------------
EstimationDataFilterFactory& EstimationDataFilterFactory::operator= (const EstimationDataFilterFactory& fact)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory operator =\n");
#endif

   if (&fact != this)
   {
      Factory::operator=(fact);

      if (creatables.empty())
      {
         /// StatisticsAcceptFilter and StatisticsRejectFilter are nolonger been used in GMAT
         //creatables.push_back("StatisticsAcceptFilter");
         //creatables.push_back("StatisticsRejectFilter");

         creatables.push_back("AcceptFilter");
         creatables.push_back("RejectFilter");
      }
   }

   return *this;
}


//-----------------------------------------------------------------------------
// DataFilter* CreateDataFilter(const std::string &ofType,
//       const std::string &withName)
//-----------------------------------------------------------------------------
/**
 * Creates a DataFilter object with the specified name
 *
 * @param ofType The scripted type of the object
 * @param withName The name of the new object
 *
 * @return A pointer to the new object, or NULL if no new object was created
 */
//-----------------------------------------------------------------------------
DataFilter* EstimationDataFilterFactory::CreateDataFilter(const std::string &ofType,
      const std::string &withName)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory::CreateDataFilter()\n");
#endif

   DataFilter *retval = NULL;

   /// StatisticsAcceptFilter and StatisticsRejectFilter are nolonger been used in GMAT
   //if (ofType == "StatisticsAcceptFilter")
   //{
   //   MessageInterface::ShowMessage("Warning: StatisticsAcceptFilter is deprecated and will be removed in a future GMAT build. Use AcceptFilter instead.\n");
   //   retval = new StatisticAcceptFilter(ofType, withName);
   //}
   //
   //if (ofType == "StatisticsRejectFilter")
   //{
   //   MessageInterface::ShowMessage("Warning: StatisticsRejectFilter is deprecated and will be removed in a future GMAT build. Use RejectFilter instead.\n");
   //   retval = new StatisticRejectFilter(ofType, withName);
   //}

   if (ofType == "AcceptFilter")
      retval = new AcceptFilter(ofType, withName);
   
   if (ofType == "RejectFilter")
      retval = new RejectFilter(ofType, withName);

   return retval;
}


//-----------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &ofType,
//       const std::string &withName)
//-----------------------------------------------------------------------------
/**
 * Creates a DataFilter object with the specified name
 *
 * @param ofType The scripted type of the object
 * @param withName The name of the new object
 *
 * @return A pointer to the new object, or NULL if no new object was created
 */
//-----------------------------------------------------------------------------
GmatBase* EstimationDataFilterFactory::CreateObject(const std::string &ofType,
      const std::string &withName)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("EstimationDataFilterFactory::CreateObject()\n");
#endif

   return (GmatBase*) CreateDataFilter(ofType, withName);
}
