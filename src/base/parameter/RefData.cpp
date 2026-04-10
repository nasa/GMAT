//$Id$
//------------------------------------------------------------------------------
//                                  RefData
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
// Created: 2004/01/09
//
/**
 * Implements base class of reference data.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "RefData.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"          // for ParseParameter()
#include "MessageInterface.hpp"

//#define DEBUG_REFDATA_CREATE
//#define DEBUG_REFDATA_OBJECT 2
//#define DEBUG_REF_OBJECT 2
//#define DEBUG_REFDATA_FIND 1
//#define DEBUG_REFDATA_ADD 1
//#define DEBUG_RENAME 1
//#define DEBUG_CLONE 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RefData(const std::string &name = "", const std::string &typeName = "",
//         UnsignedInt paramOwnerType = Gmat::SPACECRAFT)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
RefData::RefData(const std::string &name, const std::string &typeName,
                 const UnsignedInt paramOwnerType,
                 GmatParam::DepObject depObj, bool isSettable)
{
   #ifdef DEBUG_REFDATA_CREATE
   MessageInterface::ShowMessage
      ("RefData(%s) constructor entered, depObj=%d, isSettable=%d\n", name.c_str(),
       depObj, isSettable);
   #endif
   mParameter = NULL;
   mActualParamName = name;
   GmatStringUtil::ParseParameter(name, mParamTypeName, mParamOwnerName, mParamDepName);
   mParamOwnerType = paramOwnerType;
   mParamDepObj = depObj;
   mIsParamSettable = isSettable;
   mRefObjList.clear();
   mNumRefObjects = 0;

   if (mParamTypeName == "")
      mParamTypeName = typeName;
   
   #ifdef DEBUG_REFDATA_CREATE
   MessageInterface::ShowMessage
      ("RefData(%s) constructor leaving, mActualParamName='%s', mParamOwnerType=%d, "
       "mParamTypeName='%s', mParamOwnerName='%s', mParamDepName='%s'\n", name.c_str(),
       mActualParamName.c_str(), mParamOwnerType, mParamTypeName.c_str(),
       mParamOwnerName.c_str(), mParamDepName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// RefData(const RefData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the RefData object being copied.
 */
//------------------------------------------------------------------------------
RefData::RefData(const RefData &copy)
{
   // Copy mParameter here for GmatFunction to work (LOJ: 2015.07.28)
   //mParameter = NULL; // This will be set during initialization
   mParameter = copy.mParameter;
   mActualParamName = copy.mActualParamName;
   mParamOwnerName = copy.mParamOwnerName;
   mParamDepName = copy.mParamDepName;
   mParamTypeName = copy.mParamTypeName;
   mParamOwnerType = copy.mParamOwnerType;
   mParamDepObj = copy.mParamDepObj;
   mIsParamSettable = copy.mIsParamSettable;
   mObjectTypeNames = copy.mObjectTypeNames;
   mAllRefObjectNames = copy.mAllRefObjectNames;
   mNumRefObjects = copy.mNumRefObjects;
   mRefObjList = copy.mRefObjList;
}


//------------------------------------------------------------------------------
// RefData& operator= (const RefData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
RefData& RefData::operator= (const RefData& right)
{
   if (this == &right)
      return *this;
   
   mParameter = right.mParameter;
   mActualParamName = right.mActualParamName;
   mParamOwnerName = right.mParamOwnerName;
   mParamDepName = right.mParamDepName;
   mParamTypeName = right.mParamTypeName;
   mParamOwnerType = right.mParamOwnerType;
   mParamDepObj = right.mParamDepObj;
   mIsParamSettable = right.mIsParamSettable;
   mObjectTypeNames = right.mObjectTypeNames;
   mAllRefObjectNames = right.mAllRefObjectNames;
   mNumRefObjects = right.mNumRefObjects;
   mRefObjList = right.mRefObjList;
   return *this;
}


//------------------------------------------------------------------------------
// ~RefData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RefData::~RefData()
{
   mRefObjList.clear();
}


//------------------------------------------------------------------------------
// void SetParameter(Parameter *param)
//------------------------------------------------------------------------------
void RefData::SetParameter(Parameter *param)
{
   #ifdef DEBUG_PARAM
   MessageInterface::ShowMessage
      ("RefData::SetParameter() <%p>'%s' entered, param=<%p>'%s'\n", this,
       mActualParamName.c_str(), param, param ? param->GetName().c_str() : "NULL");
   #endif
   mParameter = param;
}


//------------------------------------------------------------------------------
// Parameter* GetParameter()
//------------------------------------------------------------------------------
Parameter* RefData::GetParameter()
{
   return mParameter;
}


//---------------------------------------------------------------------------
//  bool SetName(std::string &newName, const std::string &oldName = "")
//---------------------------------------------------------------------------
/**
 * Sets new name for any reference object.
 *
 * @note SpacecraftData uses this for owned object hardware name.
 */
//------------------------------------------------------------------------------
bool RefData::SetName(const std::string &newName, const std::string &oldName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("RefData::SetName() entered, newName='%s', oldName='%s'\n",
       newName.c_str(), oldName.c_str());
   #endif
   
   if (mActualParamName == oldName)
   {
      mActualParamName = newName;

      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("RefData::SetName() changed the name from '%s' to '%s'\n",
          oldName.c_str(), newName.c_str());
      #endif
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("RefData::SetName() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
Integer RefData::GetNumRefObjects() const
{
   return mNumRefObjects;
}


//------------------------------------------------------------------------------
// GmatBase* GetSpacecraft()
//------------------------------------------------------------------------------
/*
 * Returns first spacecraft object from the list
 */
//------------------------------------------------------------------------------
GmatBase* RefData::GetSpacecraft()
{
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == Gmat::SPACECRAFT)
         return mRefObjList[i].obj;
   }
   // if not found, look for it as a SpacePoint
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == Gmat::SPACE_POINT)
         return mRefObjList[i].obj;
   }
   
   return NULL;
}

//------------------------------------------------------------------------------
// GmatBase* GetParameterOwner()
//------------------------------------------------------------------------------
GmatBase* RefData::GetParameterOwner()
{
   return GetRefObject(mParamOwnerType, mParamOwnerName);
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
std::string RefData::GetRefObjectName(const UnsignedInt type) const
{
   #if DEBUG_REFDATA_OBJECT > 1
   MessageInterface::ShowMessage
      ("RefData::GetRefObjectName() entered, type=%d\n", type);
   MessageInterface::ShowMessage("   number of ref objects = %d\n", mNumRefObjects);
   #endif
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      #if DEBUG_REFDATA_OBJECT > 1
      MessageInterface::ShowMessage
         ("   mRefObjList[%d].objType=%d, obj=<%p>, %s\n", i, mRefObjList[i].objType,
          mRefObjList[i].obj, mRefObjList[i].objName.c_str());
      #endif
      
      if (mRefObjList[i].objType == type)
      {
         //Notes: will return first object name.
         #if DEBUG_REFDATA_OBJECT > 1
         MessageInterface::ShowMessage
            ("RefData::GetRefObjectName() type=%d returning: %s\n", type,
             mRefObjList[i].objName.c_str());
         #endif
         
         return mRefObjList[i].objName; 
      }
      else
      {
         if (mRefObjList[i].obj != NULL && mRefObjList[i].obj->IsOfType(type))
         {
            //Notes: will return first object name.
            #if DEBUG_REFDATA_OBJECT > 1
            MessageInterface::ShowMessage
               ("RefData::GetRefObjectName() type=%d returning: %s\n", type,
                mRefObjList[i].objName.c_str());
            #endif
            
            return mRefObjList[i].objName; 
         }
      }
   }
   
   #if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::GetRefObjectName() '%s', type=%d, throwing exception "
       "Unknown Ref Object Type, ref. object of this type never set.\n",
       mActualParamName.c_str(), type);
   #endif
   
   std::stringstream errmsg;
   errmsg << "*** INTERNAL ERROR: RefData::GetRefObjectName(): The type " << type <<
      " is unknown ref object type for " << mActualParamName << "' or " <<
      "ref object never set to this Parameter in the Moderator.";
   throw ParameterException(errmsg.str());
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves reference object name array for given type. It will return all
 * object names if type is Gmat::UNKNOWN_NAME.
 *
 * @param <type> object type
 * @return reference object name.
 */
//------------------------------------------------------------------------------
const StringArray& RefData::GetRefObjectNameArray(const UnsignedInt type)
{
   mAllRefObjectNames.clear();

   #if DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("RefData::GetRefObjectNameArray() '%s', type=%d\n   there are %d ref "
       "objects\n", mActualParamName.c_str(), type, mNumRefObjects);
   for (int i=0; i<mNumRefObjects; i++)
   {
      MessageInterface::ShowMessage
         ("   objType=%d, name='%s'\n", mRefObjList[i].objType,
          mRefObjList[i].objName.c_str());
   }
   #endif
   
   if (type == Gmat::UNKNOWN_OBJECT)
   {
      for (int i=0; i<mNumRefObjects; i++)
      {
         mAllRefObjectNames.push_back(mRefObjList[i].objName);
      }
   }
   else
   {
      for (int i=0; i<mNumRefObjects; i++)
      {
         if (mRefObjList[i].objType == type)
            mAllRefObjectNames.push_back(mRefObjList[i].objName);
      }
   }
   
   return mAllRefObjectNames;
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Adds type and name to reference object list.
 *
 * @param <type> reference object type
 * @param <name> reference object name
 *
 * @return true if type and name has successfully added to the list
 *
 */
//------------------------------------------------------------------------------
bool RefData::SetRefObjectName(UnsignedInt type, const std::string &name)
{
   #if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::SetRefObjectName() '%s' entered, type=%d(%s), name='%s', "
       "mParamOwnerName='%s'\n", mActualParamName.c_str(), type,
       GmatBase::OBJECT_TYPE_STRING[type - Gmat::SPACECRAFT].c_str(), name.c_str(),
       mParamOwnerName.c_str());
   #endif
   
   if (FindFirstObjectName(type) != "")
   {
      #if DEBUG_REFDATA_OBJECT
      MessageInterface::ShowMessage
         ("   First object name of type %d is %s\n", type, (FindFirstObjectName(type)).c_str());
      #endif
      for (int i=0; i<mNumRefObjects; i++)
      {
         if (mRefObjList[i].objType == type)
         {
            #if DEBUG_REFDATA_OBJECT
            MessageInterface::ShowMessage
               ("   Setting object of type %d at position %d with old name \"%s\" to new name \"%s\"\n",
                (Integer) type, (Integer) i, mRefObjList[i].objName.c_str(), name.c_str());
            #endif
            mRefObjList[i].objName = name;
            #if DEBUG_REFDATA_OBJECT
            MessageInterface::ShowMessage
               ("RefData::SetRefObjectName() '%s' returning true\n", mActualParamName.c_str());
            #endif
            return true;
         }
      }
   }
   
   if (mParamOwnerName == "")
   {
      if (type == mParamOwnerType)
         mParamOwnerName = name;
   }
   
   #if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("   mActualParamName='%s', mParamOwnerType=%d, mParamTypeName='%s', "
       "mParamOwnerName='%s', mParamDepName='%s'\n", mActualParamName.c_str(),
       mParamOwnerType, mParamTypeName.c_str(), mParamOwnerName.c_str(),
       mParamDepName.c_str());
   MessageInterface::ShowMessage
      ("RefData::SetRefObjectName() '%s' returning AddRefObject(type, name)\n",
       mActualParamName.c_str());
   #endif
   
   return AddRefObject(type, name);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const UnsignedInt type,
//                        const std::string &name = "");
//------------------------------------------------------------------------------
GmatBase* RefData::GetRefObject(const UnsignedInt type,
                                const std::string &name)
{
   #if DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("RefData::GetRefObject() <%p>'%s' entered, type=%d, name='%s', mNumRefObjects=%d\n",
       this, mActualParamName.c_str(), type, name.c_str(), mNumRefObjects);
   #endif
   
   GmatBase *refObj = NULL;
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == type)
      {
         if (name == "") //if name is "", return first object
         {
            //return mRefObjList[i].obj;
            refObj = mRefObjList[i].obj;
            break;
         }
         
         if (mRefObjList[i].objName == name)
         {
            //return mRefObjList[i].obj;
            refObj = mRefObjList[i].obj;
            break;
         }
      }
   }
   
   #if DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("RefData::GetRefObject() <%p>'%s' returning %s\n", this, mActualParamName.c_str(),
       GmatBase::WriteObjectInfo("", refObj).c_str());
   #endif
   
   //return NULL;
   return refObj;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets object which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool RefData::SetRefObject(GmatBase *obj, const UnsignedInt type,
                           const std::string &name)
{
   #ifdef DEBUG_REF_OBJECTS
      if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
      {
         MessageInterface::ShowMessage("Does %s == %s @ %p?\n", name.c_str(),
               obj->GetName().c_str(), obj);
         if (name == "EarthFixed")
            MessageInterface::ShowMessage("************\n%s\n************\n",
                  obj->GetGeneratingString(Gmat::SCRIPTING).c_str());
      }
   #endif
   
   bool status = false;
   #if DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("RefData::SetRefObject() <%p>'%s' entered\n   numRefObjects=%d, type=%d, "
       "obj=<%p>'%s'\n", this, mActualParamName.c_str(), mNumRefObjects, type, obj, name.c_str());
   #endif
   
   // Since Sandbox calls SetRefObject() with obj->GetType(), I need to
   // set type to SPACE_POINT if CELESTIAL_BODY
   
   UnsignedInt actualType = type;
   if (type == Gmat::CELESTIAL_BODY)
      actualType = Gmat::SPACE_POINT;
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      #if DEBUG_REF_OBJECT > 1
      MessageInterface::ShowMessage
         ("   type=%d, obj=<%p>, name='%s'\n", mRefObjList[i].objType,
          mRefObjList[i].obj, mRefObjList[i].objName.c_str());
      #endif
      
      // Check for the name first and use SPACE_POINT type to set object pointer
      // if input type is SPACECRAFT for GMAT function work (LOJ: 2015.03.03)
      if (mRefObjList[i].objName == name)
      {
         if (mRefObjList[i].objType == actualType)
         {
            mRefObjList[i].obj = obj;
            #if DEBUG_REF_OBJECT > 1
            MessageInterface::ShowMessage
               ("   1 The object pointer <%p> set to '%s'\n", obj, name.c_str());
            #endif
            status = true;
            break;
         }
         else if (actualType == Gmat::SPACECRAFT &&
                  mRefObjList[i].objType == Gmat::SPACE_POINT)
         {
            mRefObjList[i].obj = obj;
            #if DEBUG_REF_OBJECT > 1
            MessageInterface::ShowMessage
               ("   2 The object pointer <%p> set to '%s'\n", obj, name.c_str());
            #endif
            status = true;
            break;
         }
      }
   }
   
   if (!status)
   {
      #if DEBUG_REF_OBJECT
      MessageInterface::ShowMessage
         ("*** WARNING *** RefData::SetRefObject() Cannot find '%s' of type '%s'\n",
          name.c_str(), GmatBase::GetObjectTypeString(actualType).c_str());
      #endif
   }
   
   #if DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("RefData::SetRefObject() <%p>'%s' returning %d\n", this, mActualParamName.c_str(), status);
   #endif
   
   return status;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/*
 * This method renames Parameter object used in the Parameter, such as Sat in
 * Sat.EarthMJ2000Eq.X MyBurn in MyBurn.Element1. This also renames Parameter
 * object owned object name such as Sat.Thruster1.DutyCycle.
 */
//---------------------------------------------------------------------------
bool RefData::RenameRefObject(const UnsignedInt type,
                              const std::string &oldName,
                              const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("RefData::RenameRefObject() '%s' entered, type=%d, oldName='%s', "
       "newName='%s'\n", mActualParamName.c_str(), type, oldName.c_str(), newName.c_str());
   MessageInterface::ShowMessage("   mNumRefObjects=%d\n", mNumRefObjects);
   #endif
   
   // Check for allowed object types for rename
   if (type != Gmat::SPACECRAFT       && type != Gmat::COORDINATE_SYSTEM &&
       type != Gmat::CALCULATED_POINT && type != Gmat::BURN              &&
       type != Gmat::IMPULSIVE_BURN   && type != Gmat::FINITE_BURN       &&
       type != Gmat::HARDWARE         &&
       type != Gmat::THRUSTER         && type != Gmat::FUEL_TANK         &&
       type != Gmat::BARYCENTER       && type != Gmat::LIBRATION_POINT   &&
       type != Gmat::BODY_FIXED_POINT && type != Gmat::GROUND_STATION    &&
       type != Gmat::CELESTIAL_BODY   && type != Gmat::SPACE_POINT       &&
       type != Gmat::ODE_MODEL)
   {
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("RefData::RenameRefObject() '%s' returning true, nothing is done for the type:%d\n",
          mActualParamName.c_str(), type);
      #endif
      return true;
   }
   
   // Change instance name
   std::string ownerStr, typeStr, depStr;
   GmatStringUtil::ParseParameter(mActualParamName, typeStr, ownerStr, depStr);
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("   mActualParamName='%s', owner='%s', dep='%s', type='%s'\n",
       mActualParamName.c_str(), ownerStr.c_str(), depStr.c_str(), typeStr.c_str());
   #endif
   // Check for depStr for hardware parameter such as Sat.Thruster1.DutyCycle
   if (ownerStr == oldName || depStr == oldName)
   {
      mActualParamName = GmatStringUtil::ReplaceName(mActualParamName, oldName, newName);
      #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("   instance name changed to '%s'\n", mActualParamName.c_str());
      #endif
   }
   if (mParamOwnerName == oldName)
      mParamOwnerName = newName;
   if (mParamDepName == oldName)
      mParamDepName = newName;
   
   Integer numRenamed = 0;
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == type)
      {
         if (mRefObjList[i].objName == oldName)
         {
            mRefObjList[i].objName = newName;
            numRenamed++;
            #if DEBUG_RENAME
            MessageInterface::ShowMessage
               ("   '%s' renamed to '%s'\n", oldName.c_str(),
                mRefObjList[i].objName.c_str());
            #endif
         }
      }
   }
   
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("RefData::RenameRefObject() '%s' returning true, %d ref objects renamed!\n",
       mActualParamName.c_str(), numRenamed);
   #endif
   return true;
}


//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* RefData::GetValidObjectList() const
{
   return NULL;
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// bool AddRefObject(const UnsignedInt type, const std::string &name,
//                   GmatBase *obj = NULL, bool replaceName = false)
//------------------------------------------------------------------------------
/**
 * Adds object which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool RefData::AddRefObject(const UnsignedInt type, const std::string &name,
                           GmatBase *obj, bool replaceName)
{
   #if DEBUG_REFDATA_ADD
   MessageInterface::ShowMessage
      ("RefData::AddRefObject() '%s' entered, mNumRefObjects=%d, type=%d, "
       "name='%s', obj=<%p>, replaceName=%d\n", mActualParamName.c_str(), mNumRefObjects,
       type, name.c_str(), obj, replaceName);
   #endif
   
   UnsignedInt actualType = type;
   if (type == Gmat::CELESTIAL_BODY)
      actualType = Gmat::SPACE_POINT;
   
   if (IsValidObjectType(actualType))
   {
      if (FindFirstObjectName(actualType) == "")
      {
         RefObjType newObj(actualType, name, obj);
         mRefObjList.push_back(newObj);
         mNumRefObjects = mRefObjList.size();
         #if DEBUG_REFDATA_ADD
         MessageInterface::ShowMessage
            ("RefData::AddRefObject() '%s' returning true\n", mActualParamName.c_str());
         #endif
         return true;        
      }
      else
      {
         if (replaceName)
            SetRefObjectWithNewName(obj, actualType, name);
         else
            SetRefObject(obj, actualType, name);
         
         #if DEBUG_REFDATA_ADD
         MessageInterface::ShowMessage
            ("RefData::AddRefObject() '%s' returning true\n", mActualParamName.c_str());
         #endif
         return true;
      }
   }
   
   #if DEBUG_REFDATA_ADD
   MessageInterface::ShowMessage
      ("RefData::AddRefObject() '%s' does NOT have a valid object type so returning "
       "false\n", name.c_str());
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// bool SetRefObjectWithNewName(GmatBase *obj, const UnsignedInt type,
//                              const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets object pointer with new name which is used in evaluation.
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool RefData::SetRefObjectWithNewName(GmatBase *obj, const UnsignedInt type,
                                      const std::string &name)
{
   #if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("RefData::SetRefObjectWithNewName() numRefObjects=%d, type=%d, name=%s "
       "obj addr=%p\n", mNumRefObjects, type, name.c_str(), obj);
   #endif
   
   #if DEBUG_REFDATA_OBJECT > 1
   for (int i=0; i<mNumRefObjects; i++)
   {
      MessageInterface::ShowMessage
         ("type=%d, name=%s, obj=%p\n", mRefObjList[i].objType,
          mRefObjList[i].objName.c_str(), mRefObjList[i].obj);
   }   
   #endif
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      if (mRefObjList[i].objType == type)
      {
         mRefObjList[i].objName = name;
         mRefObjList[i].obj = obj;
         
         #if DEBUG_REFDATA_OBJECT > 1
         MessageInterface::ShowMessage
            ("RefData::SetRefObjectWithName() set %s to %p\n",
             mRefObjList[i].objName.c_str(), obj);
         #endif
         return true;
      }
   }

   #if DEBUG_REFDATA_OBJECT
   MessageInterface::ShowMessage
      ("*** Warning *** RefData::SetRefObjectWithName() Cannot find type=%s\n",
       GmatBase::GetObjectTypeString(type).c_str());
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void RefData::InitializeRefObjects()
{
   //do nothing here
}


//------------------------------------------------------------------------------
// bool HasObjectType(const std::string &type) const
//------------------------------------------------------------------------------
/**
 * @return true if it has the given object type, false otherwise
 */
//------------------------------------------------------------------------------
bool RefData::HasObjectType(const std::string &type) const
{

   for (int i=0; i<mNumRefObjects; i++)
   {
      if (GmatBase::GetObjectTypeString(mRefObjList[i].objType) == type)
         return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// GmatBase* FindFirstObject(const std::string &typeName) const
//------------------------------------------------------------------------------
/**
 * @return first object found for given object type name.
 */
//------------------------------------------------------------------------------
GmatBase* RefData::FindFirstObject(const std::string &typeName) const
{   
   return FindFirstObject(GmatBase::GetObjectType(typeName));
}


//------------------------------------------------------------------------------
// GmatBase* FindFirstObject(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * @return first object found for given object type.
 */
//------------------------------------------------------------------------------
GmatBase* RefData::FindFirstObject(const UnsignedInt type) const
{
   #if DEBUG_REFDATA_FIND
   MessageInterface::ShowMessage
      ("RefData::FindFirstObject() type=%d mNumRefObjects=%d\n",
       type, mNumRefObjects);
   #endif
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      #if DEBUG_REFDATA_FIND
      MessageInterface::ShowMessage
         ("RefData::FindFirstObject() i=%d, type=%d, name=%s, obj=%p\n", i,
          mRefObjList[i].objType, mRefObjList[i].objName.c_str(), mRefObjList[i].obj);
      #endif
      
      if (mRefObjList[i].objType == type)
      {
         #if DEBUG_REFDATA_FIND
         MessageInterface::ShowMessage
            ("RefData::FindFirstObject() returning %p\n", mRefObjList[i].obj);
         #endif
         
         return mRefObjList[i].obj;
      }
   }
   
   #if DEBUG_REFDATA_FIND
   MessageInterface::ShowMessage
      ("RefData::FindFirstObject() returning NULL\n");
   #endif
   
   return NULL;
}


//------------------------------------------------------------------------------
// std::string FindFirstObjectName(const std::string &typeName) const
//------------------------------------------------------------------------------
/**
 * @return first object name found for given object type name.
 */
//------------------------------------------------------------------------------
std::string RefData::FindFirstObjectName(const std::string &typeName) const
{   
   return FindFirstObjectName(GmatBase::GetObjectType(typeName));
}


//------------------------------------------------------------------------------
// std::string FindFirstObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * @return first object name found for given object type.
 */
//------------------------------------------------------------------------------
std::string RefData::FindFirstObjectName(const UnsignedInt type) const
{
   #if DEBUG_REFDATA_OBJECT > 1
   MessageInterface::ShowMessage
      ("RefData::FindFirstObjectName() entered, type=%d\n", type);
   #endif
   for (int i=0; i<mNumRefObjects; i++)
   {
      #if DEBUG_REFDATA_OBJECT > 1
      MessageInterface::ShowMessage
         ("   mRefObjList[%d].objType=%d, objName=%s\n", i, mRefObjList[i].objType,
          mRefObjList[i].objName.c_str());
      #endif
      
      if (mRefObjList[i].objType == type)
      {
         #if DEBUG_REFDATA_OBJECT > 1
         MessageInterface::ShowMessage
            ("RefData::FindFirstObjectName() returning '%s'\n", mRefObjList[i].objName.c_str());
         #endif
         return mRefObjList[i].objName;
      }
   }
   
   #if DEBUG_REFDATA_OBJECT > 1
   MessageInterface::ShowMessage
      ("RefData::FindFirstObjectName() returning '', no name of given type found\n");
   #endif
   return "";
}


//------------------------------------------------------------------------------
// StringArray FindObjectNames(const UnsignedInt type) const
//------------------------------------------------------------------------------
StringArray RefData::FindObjectNames(const UnsignedInt type) const
{
   StringArray refNames;
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      #if DEBUG_REFDATA_OBJECT > 1
      MessageInterface::ShowMessage
         ("RefData::FindObjectNames() mRefObjList[%d].objType=%d, objName=%s\n",
          i, mRefObjList[i].objType, mRefObjList[i].objName.c_str());
      #endif
      
      if (mRefObjList[i].objType == type)
         refNames.push_back(mRefObjList[i].objName);
   }
   
   return refNames;
}


//------------------------------------------------------------------------------
// GmatBase* FindObject(const UnsignedInt type, const std::string name) const
//------------------------------------------------------------------------------
GmatBase* RefData::FindObject(const UnsignedInt type, const std::string name) const
{
   #if DEBUG_REFDATA_FIND
   MessageInterface::ShowMessage
      ("RefData::FindObject() entered, type=%d, name='%s' mNumRefObjects=%d\n",
       type, name.c_str(), mNumRefObjects);
   #endif
   
   for (int i=0; i<mNumRefObjects; i++)
   {
      #if DEBUG_REFDATA_FIND
      MessageInterface::ShowMessage
         ("RefData::FindObject() i=%d, type=%d, name=%s, obj=%p\n", i,
          mRefObjList[i].objType, mRefObjList[i].objName.c_str(), mRefObjList[i].obj);
      #endif
      
      if (mRefObjList[i].objName == name)
      {
         if ((mRefObjList[i].obj != NULL) && mRefObjList[i].obj->IsOfType(type))
         {
            #if DEBUG_REFDATA_FIND
            MessageInterface::ShowMessage
               ("RefData::FindObject() returning <%p>\n", mRefObjList[i].obj);
            #endif
            return mRefObjList[i].obj;
         }
      }
   }
   
   #if DEBUG_REFDATA_FIND
   MessageInterface::ShowMessage
      ("RefData::FindObject() returning NULL\n");
   #endif
   
   return NULL;
}

