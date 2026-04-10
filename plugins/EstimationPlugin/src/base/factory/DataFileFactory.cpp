//------------------------------------------------------------------------------
//                         DataFileFactory
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
// Created: 2009/06/20
//
/**
 * Factory used to create DataFile objects.
 */
//------------------------------------------------------------------------------


#include "DataFileFactory.hpp"
#include "DataFile.hpp"

//-----------------------------------------------------------------------------
// DataFileFactory()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
DataFileFactory::DataFileFactory() :
   Factory     (Gmat::DATA_FILE)
{
   if (creatables.empty())
   {
      Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
      if (runmode == GmatGlobal::TESTING)
         creatables.push_back("DataFile");
   }
   GmatType::RegisterType(Gmat::DATASTREAM, "DataStream");  // Here or elsewhere?
   GmatType::RegisterType(Gmat::DATA_FILE, "DataFile");
}


//-----------------------------------------------------------------------------
// ~DataFileFactory()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
DataFileFactory::~DataFileFactory()
{
}


//-----------------------------------------------------------------------------
// DataFileFactory(StringArray createList)
//-----------------------------------------------------------------------------
/**
 * Secondary constructor designed to override the names of the creatables
 *
 * @param createList The new list of creatable objects
 */
//-----------------------------------------------------------------------------
DataFileFactory::DataFileFactory(StringArray createList) :
   Factory        (createList, Gmat::DATA_FILE)
{
   if (creatables.empty())
   {
      Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
      if (runmode == GmatGlobal::TESTING)
         creatables.push_back("DataFile");
   }
}


//-----------------------------------------------------------------------------
// DataFileFactory(const DataFileFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fact The factory that gets copied here
 */
//-----------------------------------------------------------------------------
DataFileFactory::DataFileFactory(const DataFileFactory& fact) :
   Factory        (fact)
{
   if (creatables.empty())
   {
      Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
      if (runmode == GmatGlobal::TESTING)
         creatables.push_back("DataFile");
   }
}


//-----------------------------------------------------------------------------
// DataFileFactory& operator= (const DataFileFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fact The factory that gets copied here
 *
 * @return This factory, configured to match fact
 */
//-----------------------------------------------------------------------------
DataFileFactory& DataFileFactory::operator= (const DataFileFactory& fact)
{
   if (&fact != this)
   {
      Factory::operator=(fact);

      if (creatables.empty())
      {
         Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
         if (runmode == GmatGlobal::TESTING)
            creatables.push_back("DataFile");
      }
   }

   return *this;
}

//-----------------------------------------------------------------------------
// GmatBase* CreateDataFile(const std::string &ofType,
//       const std::string &withName)
//-----------------------------------------------------------------------------
/**
 * Creates a DataFile object with the specified name
 *
 * @param ofType The scripted type of the object
 * @param withName The name of the new object
 *
 * @return A pointer to the new object, or NULL if no new object was created
 */
//-----------------------------------------------------------------------------
GmatBase* DataFileFactory::CreateObject(const std::string &ofType,
      const std::string &withName)
{
   return CreateDataFile(ofType, withName);
}

//-----------------------------------------------------------------------------
// DataFile* CreateDataFile(const std::string &ofType,
//       const std::string &withName)
//-----------------------------------------------------------------------------
/**
 * Creates a DataFile object with the specified name
 *
 * @param ofType The scripted type of the object
 * @param withName The name of the new object
 *
 * @return A pointer to the new object, or NULL if no new object was created
 */
//-----------------------------------------------------------------------------
DataFile* DataFileFactory::CreateDataFile(const std::string &ofType,
      const std::string &withName)
{
   DataFile *retval = NULL;

   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
   if (runmode == GmatGlobal::TESTING)
   {
      if (ofType == "DataFile")
         retval = new DataFile(withName);
   }

   return retval;
}
