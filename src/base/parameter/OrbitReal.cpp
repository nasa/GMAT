//$Id$
//------------------------------------------------------------------------------
//                                OrbitReal
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
// Created: 2004/03/29
//
/**
 * Implements OrbitReal class which provides base class for orbit realated Real
 * Parameters
 */
//------------------------------------------------------------------------------

#include "OrbitReal.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_REF_OBJECT 1
//#define DEBUG_ORBITREAL 1
//#define DEBUG_ORBITREAL_SET 1

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OrbitReal(const std::string &name, const std::string &typeStr, 
//           GmatBase *obj, const std::string &desc, const std::string &unit,
//           GmatParam::DepObject depObj, Integer itemId, bool isSettable,
//           bool isPlottable, bool isReportable)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the parameter
 * @param <typeStr> type of the parameter
 * @param <obj> reference object pointer
 * @param <desc> description of the parameter
 * @param <unit> unit of the parameter
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NO_DEP)
 * @param <itemId> item id from OrbitData [-999]
 * @param <isSettable> true if parameter is settable, false otherwise [false]
 * @param <isPlottable> true if parameter is plottable, false otherwise [true]
 * @param <isReportable> true if parameter is reportable, false otherwise [true]
 */
//------------------------------------------------------------------------------
OrbitReal::OrbitReal(const std::string &name, const std::string &typeStr, 
                     GmatBase *obj, const std::string &desc, const std::string &unit,
                     GmatParam::DepObject depObj, Integer itemId, bool isSettable,
                     bool isPlottable, bool isReportable, UnsignedInt paramOwnerType)
   : RealVar(name, "", typeStr, GmatParam::SYSTEM_PARAM, obj, desc, unit,
             depObj, paramOwnerType, false, isSettable, isPlottable, isReportable),
     OrbitData(name, typeStr, paramOwnerType, depObj, isSettable)
{
   mItemId = itemId;
   mNeedCoordSystem = true;
   AddRefObject(obj);
   
   #ifdef DEBUG_ORBITREAL
   MessageInterface::ShowMessage
      ("OrbitReal() '%s' entered, mItemId = %d\n", name.c_str(), mItemId);
   #endif
}


//------------------------------------------------------------------------------
// OrbitReal(const OrbitReal &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OrbitReal::OrbitReal(const OrbitReal &copy)
   : RealVar(copy), OrbitData(copy)
{
   #if DEBUG_ORBITREAL
   MessageInterface::ShowMessage
      ("===> OrbitReal::OrbitReal() copy constructor called on %s\n",
       instanceName.c_str());
   #endif
   mItemId = copy.mItemId;
}


//------------------------------------------------------------------------------
// OrbitReal& operator=(const OrbitReal &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the parameter to make copy of
 */
//------------------------------------------------------------------------------
OrbitReal& OrbitReal::operator=(const OrbitReal &right)
{
   if (this != &right)
   {
      RealVar::operator=(right);
      OrbitData::operator=(right);
      mItemId = right.mItemId;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitReal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OrbitReal::~OrbitReal()
{
   //MessageInterface::ShowMessage("==> OrbitReal::~OrbitReal()\n");
}


//-------------------------------------
// Inherited methods from Parameter
//-------------------------------------

//------------------------------------------------------------------------------
// virtual GmatBase* GetOwner()
//------------------------------------------------------------------------------
GmatBase* OrbitReal::GetOwner()
{
   return GetParameterOwner();
}

//------------------------------------------------------------------------------
// virtual Real EvaluateReal()
//------------------------------------------------------------------------------
/**
 * @return newly evaluated value of parameter
 */
//------------------------------------------------------------------------------
Real OrbitReal::EvaluateReal()
{
   //===================================
   #ifdef DEBUG_EVAL_REAL
   //===================================
   try
   {
      Evaluate();
   }
   catch (BaseException &e)
   {
      throw ParameterException
         ("OrbitReal::EvaluateReal() for parameter " +  this->GetTypeName() + ":" +
          instanceName + "\n" + e.GetDetails());
   }
   
   //===================================
   #else
   //===================================
   
   Evaluate();
   
   //===================================
   #endif
   //===================================
   
   return mRealValue;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
void OrbitReal::SetReal(Real val)
{
   #ifdef DEBUG_ORBITREAL_SET
   MessageInterface::ShowMessage
      ("OrbitReal::SetReal() <%p>'%s' entered, val = %f, mItemId = %d\n", this,
       GetName().c_str(), val, mItemId);
   #endif
   
   OrbitData::SetReal(mItemId, val);
   RealVar::SetReal(val);
   
   #ifdef DEBUG_ORBITREAL_SET
   MessageInterface::ShowMessage
      ("OrbitReal::SetReal() <%p>'%s' leaving, val = %f, mItemId = %d\n", this,
       GetName().c_str(), val, mItemId);
   #endif
}


//------------------------------------------------------------------------------
// virtual CoordinateSystem* GetInternalCoordSystem()
//------------------------------------------------------------------------------
CoordinateSystem* OrbitReal::GetInternalCoordSystem()
{
   return OrbitData::GetInternalCoordSys();
}


//------------------------------------------------------------------------------
// virtual void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets SolarSystem pointer.
 */
//------------------------------------------------------------------------------
void OrbitReal::SetSolarSystem(SolarSystem *ss)
{
   #if DEBUG_ORBITREAL
   MessageInterface::ShowMessage
      ("OrbitReal::SetSolarSystem() ss=%s to %s\n", ss->GetTypeName().c_str(),
       this->GetName().c_str());
   #endif
   
   if (OrbitData::GetRefObject(Gmat::SOLAR_SYSTEM, ss->GetName()) == NULL)
      OrbitData::AddRefObject(ss->GetType(), ss->GetName(), ss);
   else
      OrbitData::SetRefObject(ss, Gmat::SOLAR_SYSTEM, ss->GetName());
   
}


//------------------------------------------------------------------------------
// virtual void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets internal CoordinateSystem pointer. Assumes parameter data is in
 * this internal CoordinateSystem.
 */
//------------------------------------------------------------------------------
void OrbitReal::SetInternalCoordSystem(CoordinateSystem *cs)
{
   #if DEBUG_ORBITREAL
   MessageInterface::ShowMessage
      ("OrbitReal::SetInternalCoordSystem() cs=%s to %s\n", cs->GetTypeName().c_str(),
       this->GetName().c_str());
   #endif
   
   OrbitData::SetInternalCoordSys(cs);
}


//------------------------------------------------------------------------------
// virtual Integer GetNumRefObjects() const
//------------------------------------------------------------------------------
/**
 * @return number of reference objects set.
 */
//------------------------------------------------------------------------------
Integer OrbitReal::GetNumRefObjects() const
{
   return OrbitData::GetNumRefObjects();
}


//------------------------------------------------------------------------------
// virtual bool AddRefObject(GmatBase *obj, bool replaceName = false)
//------------------------------------------------------------------------------
/**
 * Adds reference object.
 *
 * @param <obj> object pointer
 *
 * @return true if the object has been added.
 */
//------------------------------------------------------------------------------
bool OrbitReal::AddRefObject(GmatBase *obj, bool replaceName)
{
   if (obj != NULL)
   {
      #if DEBUG_ORBITREAL
      MessageInterface::ShowMessage
         ("OrbitReal::AddRefObject() obj->GetName()=%s, type=%d\n",
          obj->GetName().c_str(), obj->GetType());
      #endif
      
      if (obj->IsOfType(Gmat::CELESTIAL_BODY))
         return OrbitData::AddRefObject(Gmat::SPACE_POINT, obj->GetName(), obj,
                                        replaceName);
      else
         return OrbitData::AddRefObject(obj->GetType(), obj->GetName(), obj,
                                        replaceName);
   }
   
   return false;
}


//------------------------------------------------------------------------------
// virtual bool Validate()
//------------------------------------------------------------------------------
/**
 * Validates reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitReal::Validate()
{
   return ValidateRefObjects(this);
}


//------------------------------------------------------------------------------
// virtual bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes reference objects.
 *
 * @return true if all objects are set; false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitReal::Initialize()
{
   RealVar::Initialize();
   
   //LOJ: 2014.04.16
   SetParameter(this);
   
   try
   {
      #if DEBUG_ORBITREAL
      MessageInterface::ShowMessage
         ("===> OrbitReal::Initialize() calling InitializeRefObjects() on %s\n",
          instanceName.c_str());
      #endif
      
      InitializeRefObjects();
   }
   catch(InvalidDependencyException &e)
   {
      #if DEBUG_ORBITREAL
      MessageInterface::ShowMessage
         ("OrbitReal::Initialize() Fail to initialize Parameter:%s\n",
          this->GetName().c_str());
      #endif
      
      throw ParameterException
         ("Incorrect parameter dependency: " + GetName() + ".\n" +
          this->GetTypeName() + e.GetFullMessage() + "\n");
   }
   catch(BaseException &e)
   {
      #if DEBUG_ORBITREAL
      MessageInterface::ShowMessage
         ("OrbitReal::Initialize() Fail to initialize Parameter:%s\n",
          this->GetName().c_str());
      #endif
      
      throw ParameterException
         (e.GetFullMessage() + " in " + GetName() + "\n");
   }
   
   return true;
}


//-------------------------------------
// Methods inherited from GmatBase
//-------------------------------------

//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool OrbitReal::RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName)
{
   OrbitData::RenameRefObject(type, oldName, newName);
   RealVar::RenameRefObject(type, oldName, newName);
   
   return true;
}


//------------------------------------------------------------------------------
// virtual std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to get reference object name for given type.
 *
 * @return reference object name.
 */
//------------------------------------------------------------------------------
std::string OrbitReal::GetRefObjectName(const UnsignedInt type) const
{
   std::string objName = OrbitData::GetRefObjectName(type);
   
   if (objName == "INVALID_OBJECT_TYPE")
   {
      throw ParameterException
         ("OrbitReal::GetRefObjectName() " + GmatBase::GetObjectTypeString(type) +
          " is not valid object type of " + this->GetTypeName() + "\n");
   }
   
   return objName;
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
const StringArray& OrbitReal::GetRefObjectNameArray(const UnsignedInt type)
{
   return OrbitData::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// virtual bool SetRefObjectName(const UnsignedInt type,
//                               const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets reference object name to given object type.
 *
 * @param <type> object type
 * @param <name> object name
 *
 */
//------------------------------------------------------------------------------
bool OrbitReal::SetRefObjectName(const UnsignedInt type,
                                 const std::string &name)
{
   return OrbitData::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
// virtual GmatBase* GetRefObject(const UnsignedInt type,
//                                const std::string &name)
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to get object pointer of given type and name
 *
 * @param <type> object type
 * @param <name> object name
 *
 * @return reference object pointer for given object type and name
 */
//------------------------------------------------------------------------------
GmatBase* OrbitReal::GetRefObject(const UnsignedInt type,
                                  const std::string &name)
{
   #ifdef DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("OrbitReal::GetRefObject() <%p>'%s' entered, type=%d, name='%s'\n", this,
       this->GetName().c_str(), type, name.c_str());
   #endif
   
   GmatBase *obj = OrbitData::GetRefObject(type, name);
   
   if (obj == NULL)
   {
      throw ParameterException
         ("OrbitReal::GetRefObject() Cannot find ref. object of type:" +
          GmatBase::GetObjectTypeString(type) + ", name:" + name + " in " +
          this->GetName());
   }
   
   #ifdef DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("OrbitReal::GetRefObject() <%p>'%s' returning %s\n", this, GetName().c_str(),
       GmatBase::WriteObjectInfo("", obj).c_str());
   #endif
   
   return obj;
}


//------------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Calls OrbitData to set reference object pointer to given type and name.
 *
 * @param <obj>  reference object pointer
 * @param <type> object type
 * @param <name> object name
 *
 * @return true if object pointer is successfully set.
 *
 */
//------------------------------------------------------------------------------
bool OrbitReal::SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name)
{
   #if DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("OrbitReal::SetRefObject() '%s' entered, obj=<%p><%s>'%s', type=%d, name=%s\n",
       this->GetName().c_str(), obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   // Set owner object for Parameter here (LOJ: 2014.04.05)
   if (obj->GetName() == mParamOwnerName)
      SetOwner(obj);
   
   bool setOK = OrbitData::SetRefObject(obj, type, name);
   // Setting states for SpacePoint parameter owners, other than Spacecraft, is not allowed
   if (setOK && obj->IsOfType("SpacePoint") && !(obj->IsOfType("Spacecraft")) &&
       (obj->GetName() == mParamOwnerName))
      mIsSettable = false;
   
   #if DEBUG_REF_OBJECT
   MessageInterface::ShowMessage
      ("OrbitReal::SetRefObject() '%s' returning %d\n", GetName().c_str(), setOK);
   #endif
   
   return setOK;
//   return OrbitData::SetRefObject(obj, type, name);
}

