//$Id$
//------------------------------------------------------------------------------
//                                ParameterDatabase
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
// Author: Linda Jun
// Created: 2003/09/18
//
/**
 * Implements parameter database class.
 */
//------------------------------------------------------------------------------

#include "ParameterDatabase.hpp"
#include "ParameterDatabaseException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RENAME
//#define DEBUG_PARAM_DB
//#define DEBUG_PARAMDB_ADD

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// ParameterDatabase()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
ParameterDatabase::ParameterDatabase()
{
   mNumParams = 0;
   mStringParamPtrMap = new StringParamPtrMap;
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (mStringParamPtrMap, "mStringParamPtrMap", "ParameterDatabase::ParameterDatabase()",
       "mStringParamPtrMap = new StringParamPtrMap");
   #endif
   
   #ifdef DEBUG_PARAMDB
   MessageInterface::ShowMessage
      ("ParameterDatabase(default) mStringParamPtrMap.size()=%d, "
       "mNumParams=%d\n",  mStringParamPtrMap->size(), mNumParams);
   #endif
}


//------------------------------------------------------------------------------
// ParameterDatabase(const ParameterDatabase &copy)
//------------------------------------------------------------------------------
ParameterDatabase::ParameterDatabase(const ParameterDatabase &copy)
{
   mNumParams = copy.mNumParams;
   mStringParamPtrMap = new StringParamPtrMap;
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (mStringParamPtrMap, "mStringParamPtrMap", "ParameterDatabase copy constructor",
       "mStringParamPtrMap = new StringParamPtrMap");
   #endif
   
   StringParamPtrMap::iterator pos;
   
   for (pos = copy.mStringParamPtrMap->begin();
        pos != copy.mStringParamPtrMap->end(); ++pos)
   {
      Add(pos->first, pos->second);
   }
   
   #ifdef DEBUG_PARAMDB
   MessageInterface::ShowMessage
      ("==> ParameterDatabase(copy) mStringParamPtrMap.size()=%d, "
       "mNumParams=%d\n",  mStringParamPtrMap->size(), mNumParams);
   #endif
}


//------------------------------------------------------------------------------
// ParameterDatabase& operator=(const ParameterDatabase &right)
//------------------------------------------------------------------------------
ParameterDatabase& ParameterDatabase::operator=(const ParameterDatabase &right)
{
   if (this == &right)
      return *this;
   
   mNumParams = right.mNumParams;

   if (mStringParamPtrMap)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (mStringParamPtrMap, "mStringParamPtrMap", "ParameterDatabase operator=",
          "deleting old map");
      #endif
      delete mStringParamPtrMap;
   }
   
   mStringParamPtrMap = new StringParamPtrMap;
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (mStringParamPtrMap, "mStringParamPtrMap", "ParameterDatabase operator=",
       "mStringParamPtrMap = new StringParamPtrMap");
   #endif
   
   StringParamPtrMap::iterator pos;
   
   for (pos = right.mStringParamPtrMap->begin();
        pos != right.mStringParamPtrMap->end(); ++pos)
   {
      Add(pos->first, pos->second);
   }
   
   #ifdef DEBUG_PARAMDB
   MessageInterface::ShowMessage
      ("==> ParameterDatabase(=) mStringParamPtrMap.size()=%d, "
       "mNumParams=%d\n",  mStringParamPtrMap->size(), mNumParams);
   #endif
   
   return *this;
}


//------------------------------------------------------------------------------
// virtual ~ParameterDatabase()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ParameterDatabase::~ParameterDatabase()
{
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Remove
      (mStringParamPtrMap, "mStringParamPtrMap", "ParameterDatabase destructor",
       "deleting old map");
   #endif
   delete mStringParamPtrMap;
   mStringParamPtrMap = NULL;
}

//---------------------------------
// Get methods
//---------------------------------

//------------------------------------------------------------------------------
// Integer GetNumParameters() const
//------------------------------------------------------------------------------
/**
 * Retrieves number of parameters in the database.
 *
 * @return number of parameters
 */
//------------------------------------------------------------------------------
Integer ParameterDatabase::GetNumParameters() const
{
   return mNumParams;
}


//------------------------------------------------------------------------------
// const StringArray& GetNamesOfParameters()
//------------------------------------------------------------------------------
/**
 * @return names of parameters
 */
//------------------------------------------------------------------------------
const StringArray& ParameterDatabase::GetNamesOfParameters()
{
   mParamNames.clear();
   StringParamPtrMap::iterator pos;

   if (mStringParamPtrMap == NULL)
      throw ParameterDatabaseException
         ("ParameterDatabase::GetNamesOfParameters() mStringParamPtrMap is NULL\n");

   #ifdef DEBUG_PARAMDB
   MessageInterface::ShowMessage
      ("==> ParameterDatabase::GetNamesOfParameters() mStringParamPtrMap.size()=%d, "
       "mNumParams=%d\n",  mStringParamPtrMap->size(), mNumParams);
   #endif
   
   for (pos = mStringParamPtrMap->begin(); pos != mStringParamPtrMap->end(); ++pos)
   {
      mParamNames.push_back(pos->first);
   }
   
   return mParamNames;
}


//------------------------------------------------------------------------------
// ParameterPtrArray GetParameters() const
//------------------------------------------------------------------------------
/**
 * @return array of parameters
 */
//------------------------------------------------------------------------------
ParameterPtrArray ParameterDatabase::GetParameters() const
{
   ParameterPtrArray parameters;
   StringParamPtrMap::iterator pos;
   
   for (pos = mStringParamPtrMap->begin(); pos != mStringParamPtrMap->end(); ++pos)
   {
      parameters.push_back(pos->second);
   }
   
   return parameters;
}


//------------------------------------------------------------------------------
// bool HasParameter(const std::string &name) const
//------------------------------------------------------------------------------
/**
 * @return true if database has the parameter name, false otherwise
 */
//------------------------------------------------------------------------------
bool ParameterDatabase::HasParameter(const std::string &name) const
{
   bool found = false;
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   
   if (pos != mStringParamPtrMap->end())
      found = true;
   
   return found;
}


//------------------------------------------------------------------------------
// bool RenameParameter(const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * @param <oldName> parameter name to be renamed
 * @param <newName> new prameter name
 *
 * @return true if parameter is renamed, false otherwise
 */
//------------------------------------------------------------------------------
bool ParameterDatabase::RenameParameter(const std::string &oldName,
                                        const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ParameterDatabase::RenameParameter() oldName=%s, newName=%s\n",
       oldName.c_str(), newName.c_str());
   #endif
   
   StringArray paramNames = GetNamesOfParameters();
   std::string::size_type pos;
   StringParamPtrMap::iterator pos1;
   std::string newParamName;
   
   for (UnsignedInt i=0; i<paramNames.size(); i++)
   {
      pos = paramNames[i].find(oldName);
      
      // if oldName found
      if (pos != paramNames[i].npos)
      {
         newParamName = paramNames[i];
         newParamName.replace(pos, oldName.size(), newName);
         
         pos1 = mStringParamPtrMap->find(paramNames[i]);
         
         if (pos1 != mStringParamPtrMap->end())
         {
            // add new parameter name key and delete old
            Add(newParamName, pos1->second);
            mStringParamPtrMap->erase(pos1);
         }
      }
   }
      
   return true;
}


//------------------------------------------------------------------------------
// Integer GetParameterCount(const std::string &name) const
//------------------------------------------------------------------------------
/**
 * @return number of data elements of given parameter name
 */
//------------------------------------------------------------------------------
Integer ParameterDatabase::GetParameterCount(const std::string &name) const
{
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   if (pos == mStringParamPtrMap->end())
      throw ParameterDatabaseException
         ("ParameterDatabase::GetParameterCount() Parameter name " + name +
          " not found in the database");
   else
      return pos->second->GetParameterCount();
}


//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name) const
//------------------------------------------------------------------------------
/**
 * @return parameter object of given parameter name
 */
//------------------------------------------------------------------------------
Parameter* ParameterDatabase::GetParameter(const std::string &name) const
{
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   if (pos == mStringParamPtrMap->end())
      throw ParameterDatabaseException
         ("ParameterDatabase::GetParameter() Cannot find Parameter name \"" + name +
          "\" in the Database");
   else
      return pos->second;
}


//------------------------------------------------------------------------------
// std::string GetFirstParameterName() const
//------------------------------------------------------------------------------
std::string ParameterDatabase::GetFirstParameterName() const
{
   StringParamPtrMap::iterator pos;
   pos = mStringParamPtrMap->begin();
   return pos->first;
}


//------------------------------------------------------------------------------
// bool SetParameter(const std::string &name, Parameter *param)
//------------------------------------------------------------------------------
bool ParameterDatabase::SetParameter(const std::string &name, Parameter *param)
{
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   if (pos == mStringParamPtrMap->end())
      throw ParameterDatabaseException
         ("ParameterDatabase::SetParameter() Parameter name " + name +
          " not found in the database\n");

   pos->second = param;
   return true;
}


//*********************************
// for Add, Remove 
//*********************************

//------------------------------------------------------------------------------
// void Add(Parameter *param)
//------------------------------------------------------------------------------
void ParameterDatabase::Add(Parameter *param)
{
   if (param != NULL)
   {
      std::string name = param->GetName();
      Add(name, param);
   }
   else
   {
      throw ParameterDatabaseException
         ("ParameterDatabase::Add() Cannot add NULL Parameter\n");
   }
}


//------------------------------------------------------------------------------
// void Add(const std::string &name, Parameter *param = NULL)
//------------------------------------------------------------------------------
void ParameterDatabase::Add(const std::string &name, Parameter *param)
{
   #ifdef DEBUG_PARAMDB_ADD
   MessageInterface::ShowMessage
      ("ParameterDatabase::Add() <%p> entered, name='%p', param=<%p>'%s'\n",
       this, name.c_str(), param);
   #endif
   
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   
   //if name already in the database, just ignore
   if (pos == mStringParamPtrMap->end())
   {
      mStringParamPtrMap->insert(StringParamPtrPair(name, param));
      mNumParams = mStringParamPtrMap->size();
      #ifdef DEBUG_PARAMDB_ADD
      MessageInterface::ShowMessage("   '%s' added to the map\n");
      #endif
   }
   else
   {
      #ifdef DEBUG_PARAMDB_ADD
      MessageInterface::ShowMessage("   '%s' already in the map, so ignored\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// void Remove(const Parameter *param)
//------------------------------------------------------------------------------
void ParameterDatabase::Remove(const Parameter *param)
{
   Remove(param->GetName());
}


//------------------------------------------------------------------------------
// void Remove(const std::string &name)
//------------------------------------------------------------------------------
void ParameterDatabase::Remove(const std::string &name)
{
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   if (pos == mStringParamPtrMap->end())
      throw ParameterDatabaseException
         ("ParameterDatabase::Remove() Parameter name: " + name +
          " not found in the database\n");
   
   mStringParamPtrMap->erase(name);
   mNumParams = mStringParamPtrMap->size();  
}

