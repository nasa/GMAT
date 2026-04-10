//------------------------------------------------------------------------------
//                         TlePropFactory.cpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Darrel Conway
// Created: 2019/09/26
//
// Copyright (c) 2019, Thinking Systems, Inc.
// All rights reserved

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Edited by Jairus Elarbee 10/05/2021 
//   -Change type from "TLE" to "SPICESGP4"
// Edited by Jairus Elarbee 12/13/2021 
//   -Updated SPICESGP4 to reference the new v66 TLE propagator



/**
 *  Declaration code for the TlePropFactory class.
 */


#include "tleprop_defs.hpp"
#include "TlePropFactory.hpp"
#include "MessageInterface.hpp"
#include "SPICEPropagator.hpp"


//#define DEBUG_CREATION


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Propagator class. 
 *
 * @param <ofType> type of Propagator object to create and return.
 * @param <withName> the name for the newly-created Propagator object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
GmatBase* TlePropFactory::CreateObject(const std::string &ofType,
                                            const std::string &withName)
{
   return CreatePropagator(ofType, withName);
}

//------------------------------------------------------------------------------
//  Parameter* CreateParameter(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Parameter class. 
 *
 * @param <ofType> type of Propagator object to create and return.
 * @param <withName> the name for the newly-created Propagator object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Propagator* TlePropFactory::CreatePropagator(const std::string &ofType,
                                    const std::string &withName)
{
   Propagator *retval = NULL;
   #ifdef DEBUG_CREATION
      MessageInterface::ShowMessage("Attempting to create a \"%s\" "
            "Parameter...", ofType.c_str());
   #endif

   if (ofType == "SPICESGP4")
      retval = new SPICEPropagator(withName);


   #ifdef DEBUG_CREATION
      MessageInterface::ShowMessage("output pointer is <%p>\n", retval);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
//  TlePropFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class TlePropFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
TlePropFactory::TlePropFactory() :
    Factory                (Gmat::PROPAGATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("SPICESGP4");
   }
}

//------------------------------------------------------------------------------
//  TlePropFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class TlePropFactory.
 *
 * @param <createList> list of creatable propagator objects
 *
 */
//------------------------------------------------------------------------------
TlePropFactory::TlePropFactory(StringArray createList) :
   Factory                (createList, Gmat::PROPAGATOR)
{
}


//------------------------------------------------------------------------------
//  TlePropFactory(const TlePropFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class TlePropFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
TlePropFactory::TlePropFactory(const TlePropFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("SPICESGP4");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * TlePropFactory operator for the TlePropFactory base class.
 *
 * @param <fact> the TlePropFactory object that is copied.
 *
 * @return "this" TlePropFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
TlePropFactory& TlePropFactory::operator=(const TlePropFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    

//------------------------------------------------------------------------------
// ~TlePropFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the TlePropFactory base class.
 */
//------------------------------------------------------------------------------
TlePropFactory::~TlePropFactory()
{
}
