//$Id$
//------------------------------------------------------------------------------
//                                ParameterInfo
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
// Created: 2005/01/24
//
/**
 * Implements parameter info class.
 */
//------------------------------------------------------------------------------

#include "ParameterInfo.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PARAM_INFO

//---------------------------------
// static data
//---------------------------------
ParameterInfo* ParameterInfo::theInstance = NULL;

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// ParameterInfo* Instance()
//------------------------------------------------------------------------------
ParameterInfo* ParameterInfo::Instance()
{
   if (theInstance == NULL)
      theInstance = new ParameterInfo;
    
   return theInstance;
}

//------------------------------------------------------------------------------
// ParameterInfo()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
ParameterInfo::ParameterInfo()
{
   mNumParams = 0;
}

//------------------------------------------------------------------------------
// ~ParameterInfo()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ParameterInfo::~ParameterInfo()
{
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
Integer ParameterInfo::GetNumParameters() const
{
   return mNumParams;
}


//------------------------------------------------------------------------------
// const StringArray& GetTypesOfParameters()
//------------------------------------------------------------------------------
/**
 * @return names of parameters
 */
//------------------------------------------------------------------------------
const StringArray& ParameterInfo::GetTypesOfParameters()
{
   mParamTypes.clear();
   std::map<std::string, UnsignedInt>::iterator pos;
   
   for (pos = mOwnerTypeMap.begin(); pos != mOwnerTypeMap.end(); ++pos)
   {
      mParamTypes.push_back(pos->first);
   }
   
   return mParamTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetNamesOfParameters()
//------------------------------------------------------------------------------
/**
 * @return names of parameters
 */
//------------------------------------------------------------------------------
const StringArray& ParameterInfo::GetNamesOfParameters()
{
   mParamNames.clear();
   std::map<std::string, GmatParam::DepObject>::iterator pos;

   for (pos = mDepObjMap.begin(); pos != mDepObjMap.end(); ++pos)
   {
      mParamNames.push_back(pos->first);
   }
   
   return mParamNames;
}


//------------------------------------------------------------------------------
// UnsignedInt GetObjectType(const std::string &name)
//------------------------------------------------------------------------------
UnsignedInt ParameterInfo::GetObjectType(const std::string &name)
{
   if (mOwnerTypeMap.find(name) != mOwnerTypeMap.end())
      return mOwnerTypeMap[name];
   else
      return Gmat::UNKNOWN_OBJECT;
}


//------------------------------------------------------------------------------
// UnsignedInt GetOwnedObjectType(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Returns owned or attached object type.
 */
//------------------------------------------------------------------------------
UnsignedInt ParameterInfo::GetOwnedObjectType(const std::string &name)
{
   if (mOwnedObjTypeMap.find(name) != mOwnedObjTypeMap.end())
   {
      #ifdef DEBUG_PARAM_INFO
      MessageInterface::ShowMessage
         ("GetOwnedObjectType() Returning %d\n", mOwnedObjTypeMap[name]);
      #endif
      return mOwnedObjTypeMap[name];
   }
   else
      return Gmat::UNKNOWN_OBJECT;
}


//------------------------------------------------------------------------------
// GmatParam::DepObject GetDepObjectType(const std::string &name)
//------------------------------------------------------------------------------
GmatParam::DepObject ParameterInfo::GetDepObjectType(const std::string &name)
{
   if (mDepObjMap.find(name) != mDepObjMap.end())
      return mDepObjMap[name];
   else
      return GmatParam::NO_DEP;
}


//------------------------------------------------------------------------------
// bool IsPlottable(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::IsPlottable(const std::string &name)
{
   if (mIsPlottableMap.find(name) != mIsPlottableMap.end())
      return mIsPlottableMap[name];
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsReportable(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::IsReportable(const std::string &name)
{
   if (mIsReportableMap.find(name) != mIsReportableMap.end())
      return mIsReportableMap[name];
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsSettable(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::IsSettable(const std::string &name)
{
   if (mIsSettableMap.find(name) != mIsSettableMap.end())
      return mIsSettableMap[name];
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsTimeParameter(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::IsTimeParameter(const std::string &name)
{
   if (mIsTimeParamMap.find(name) != mIsTimeParamMap.end())
      return mIsTimeParamMap[name];
   else
      return false;
}


//------------------------------------------------------------------------------
// bool RequiresBodyFixedCS(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::RequiresBodyFixedCS(const std::string &name)
{
   if (mRequiresBodyFixedCSMap.find(name) != mRequiresBodyFixedCSMap.end())
      return mRequiresBodyFixedCSMap[name];
   else
      return false;
}

//------------------------------------------------------------------------------
// bool RequiresCelestialBodyCSOrigin(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::RequiresCelestialBodyCSOrigin(const std::string &name)
{
   if (mRequiresCelestialBodyCSOriginMap.find(name) !=
       mRequiresCelestialBodyCSOriginMap.end())
      return mRequiresCelestialBodyCSOriginMap[name];
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsForOwnedObject(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::IsForOwnedObject(const std::string &name)
{
   if (mIsForOwnedObjMap.find(name) != mIsForOwnedObjMap.end())
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsForAttachedObject(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::IsForAttachedObject(const std::string &name)
{
   if (mIsForAttachedObjMap.find(name) != mIsForAttachedObjMap.end())
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// void Add(const std::string &type, UnsignedInt objectType, ...)
//------------------------------------------------------------------------------
void ParameterInfo::Add(const std::string &type, UnsignedInt objectType,
                        UnsignedInt ownedObjType, const std::string &name,
                        GmatParam::DepObject depType, bool isPlottable,
                        bool isReportable, bool isSettable, bool isTimeParam,
                        const std::string &desc)
{
   #ifdef DEBUG_PARAM_INFO
   MessageInterface::ShowMessage
      ("ParameterInfo::Add() entered, type='%s', objectType=%d, ownedObjType=%d"
       "\n   name='%s', depType=%d, isPlottable=%d, isReportable=%d, isSettable=%d, "
       "isTimeParam=%d, desc='%s'\n", type.c_str(), objectType, ownedObjType,
       name.c_str(), depType, isPlottable, isReportable, isSettable, isTimeParam,
       desc.c_str());
   #endif
   
   // Check for dot first
   std::string::size_type pos = name.find_last_of(".");
   if (pos == name.npos)
   {
      #ifdef DEBUG_PARAM_INFO
      MessageInterface::ShowMessage
         ("ParameterInfo::Add() leaving, '%s' was not added, it's not a System Parameter\n",
          type.c_str());
      #endif
      return;
   }
   
   // Check for the same property
   if (mOwnerTypeMap.find(type) != mOwnerTypeMap.end())
   {
      #ifdef DEBUG_PARAM_INFO
      MessageInterface::ShowMessage
         ("ParameterInfo::Add() leaving, '%s' was not added, it's been already added\n",
          type.c_str());
      #endif
      return;
   }
   
   std::string depTypeStr = Parameter::GetDependentTypeString(depType);
   if (GmatGlobal::Instance()->IsWritingParameterInfo())
   {
      std::string objTypeStr = GmatBase::GetObjectTypeString(objectType);
      std::string ownedObjTypeStr = "          ";
      std::string plottableStr = "N";
      std::string reportableStr = "N";
      std::string settableStr = "N";
      if (isPlottable)  plottableStr  = "Y";
      if (isReportable) reportableStr = "Y";
      if (isSettable)   settableStr   = "Y";
      if (depTypeStr == "OwnedObject" || depTypeStr == "AttachedObject")
         ownedObjTypeStr = "(" + GmatBase::GetObjectTypeString(ownedObjType) + ")";
      if (mNumParams == 0)
      {
         MessageInterface::ShowMessage
            ("\n================================================================================================\n"
             "====================================== GMAT Parameter List =====================================\n"
             "================================================================================================\n"
             "(R = Reportable, P = Plottable, S = Settable)\n"
             "No   ParameterType              ObjectType     DependencyType                        R  P  S  Description\n"
             "---  ----------------           -------------  --------------------------------      -  -  -  -----------\n");
      }
      MessageInterface::ShowMessage
         ("%3d  %-25s  %-13s  %-16s%20s  %s  %s  %s  %s\n", mNumParams+1, type.c_str(),
          objTypeStr.c_str(), depTypeStr.c_str(), ownedObjTypeStr.c_str(), reportableStr.c_str(),
          plottableStr.c_str(), settableStr.c_str(), desc.c_str());
   }
   
   // Add property objectType
   mOwnerTypeMap[type] = objectType;
   
   // Add property name
   std::string propertyName = name.substr(pos+1, name.npos-pos);
   
   mDepObjMap[propertyName] = depType;
   
   // Add plottable Parameters
   if (isPlottable)
      mIsPlottableMap[type] = isPlottable;
   
   // Add reportable Parameters
   if (isReportable)
      mIsReportableMap[type] = isReportable;
   
   // Add settable Parameters
   if (isSettable)
      mIsSettableMap[type] = isSettable;
   
   // Add time Parameters
   if (isTimeParam)
      mIsTimeParamMap[type] = isTimeParam;

   // Set Requires BodyFixed CS flag to false
   mRequiresBodyFixedCSMap[type] = false;
   
   // Set Requires CelestialBody CS Origin flag to false
   mRequiresCelestialBodyCSOriginMap[type] = false;

   // Add owned object dep paramters
   if (depType == GmatParam::OWNED_OBJ)
   {
      mIsForOwnedObjMap[type] = true;
      mOwnedObjTypeMap[type] = ownedObjType;
   }
   
   // Add attached object dep paramters
   if (depType == GmatParam::ATTACHED_OBJ)
   {
      mIsForAttachedObjMap[type] = true;
      mOwnedObjTypeMap[type] = ownedObjType;
   }
   
   mNumParams = mDepObjMap.size();
   
   #ifdef DEBUG_PARAM_INFO
   MessageInterface::ShowMessage
      ("ParameterInfo::Add() leaving, propertyName:'%s' was added. There are "
       "total %d Parameters\n", propertyName.c_str(), mNumParams);
   #endif
}


//------------------------------------------------------------------------------
// void Remove(const std::string &name)
//------------------------------------------------------------------------------
void ParameterInfo::Remove(const std::string &name)
{
   mDepObjMap.erase(name);
   mNumParams = mDepObjMap.size();
}

//------------------------------------------------------------------------------
// void SetRequiresBodyFixedCS(const std::string &type, bool flag)
//------------------------------------------------------------------------------
void ParameterInfo::SetRequiresBodyFixedCS(const std::string &type, bool flag)
{
   mRequiresBodyFixedCSMap[type] = flag;
}

//------------------------------------------------------------------------------
// void SetRequiresCelestialBodyCSOrigin(const std::string &type, bool flag)
//------------------------------------------------------------------------------
void ParameterInfo::SetRequiresCelestialBodyCSOrigin(const std::string &type,
                                                     bool flag)
{
   mRequiresCelestialBodyCSOriginMap[type] = flag;
}

