//$Id$
//------------------------------------------------------------------------------
//                                  ObjectPropertyWrapper
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// number NNG04CC06P
//
// Author: Wendy C. Shoan
// Created: 2007.01.18
//
/**
 * Implementation of the ObjectPropertyWrapper class.
 *
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ObjectPropertyWrapper.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"

#include "PropSetup.hpp"               // Handle owned Propagator special case

#include "MessageInterface.hpp"

//#define DEBUG_RENAME_OBJ_PROP
//#define DEBUG_OPW
//#define DEBUG_OBJ_PROP_SET_STRING

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ObjectPropertyWrapper();
//---------------------------------------------------------------------------
/**
 * Constructs ObjectPropertyWrapper structures
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
ObjectPropertyWrapper::ObjectPropertyWrapper() :
   ElementWrapper(),
   object        (NULL),
   propID        (-1)
{
   wrapperType = Gmat::OBJECT_PROPERTY_WT;
}

//---------------------------------------------------------------------------
//  ObjectPropertyWrapper(const ObjectPropertyWrapper &opw);
//---------------------------------------------------------------------------
/**
 * Constructs base ObjectPropertyWrapper structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param <opw>  ObjectPropertyWrapper instance to copy to create "this" 
 * instance.
 */
//---------------------------------------------------------------------------
ObjectPropertyWrapper::ObjectPropertyWrapper(const ObjectPropertyWrapper &opw) :
   ElementWrapper(opw),
   object        (opw.object),
   propIDNames   (opw.propIDNames),
   propID        (opw.propID)
{
}

//---------------------------------------------------------------------------
//  ObjectPropertyWrapper& operator=(const ObjectPropertyWrapper &opw)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ObjectPropertyWrapper structures.
 *
 * @param <opw> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ObjectPropertyWrapper& ObjectPropertyWrapper::operator=(
                             const ObjectPropertyWrapper &opw)
{
   if (&opw == this)
      return *this;

   ElementWrapper::operator=(opw);
   object      = opw.object;
   propIDNames = opw.propIDNames;
   propID      = opw.propID;

   return *this;
}

//---------------------------------------------------------------------------
//  ~ObjectPropertyWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ObjectPropertyWrapper::~ObjectPropertyWrapper()
{
}


//------------------------------------------------------------------------------
// ElementWrapper* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this wrapper
 *
 * @return The new instance
 */
//------------------------------------------------------------------------------
ElementWrapper* ObjectPropertyWrapper::Clone() const
{
   return new ObjectPropertyWrapper(*this);
}

//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return ObjectPropertyWrapper value converted to std::string.
 */
//------------------------------------------------------------------------------
std::string ObjectPropertyWrapper::ToString()
{
   if (object == NULL)
      throw ParameterException
         ("ObjectPropertyWrapper::ToString() The object is NULL");
   
   // Get property data type
   Gmat::ParameterType dataType = GetDataType();
   std::string retval;
   
   switch (dataType)
   {
   case Gmat::BOOLEAN_TYPE:
   {
      bool bval = EvaluateBoolean();
      retval = "true";
      if (!bval)
         retval = "false";
      break;
   }
   case Gmat::INTEGER_TYPE:
   {
      Integer ival = EvaluateInteger();
      retval = GmatStringUtil::ToString(ival);
      break;
   }
   case Gmat::REAL_TYPE:
   {
      Real rval = EvaluateReal();
      retval = GmatStringUtil::ToString(rval);
      break;
   }
   case Gmat::RMATRIX_TYPE:
   {
      Rmatrix rmat = EvaluateArray();
      retval = rmat.ToString(16);
      break;
   }
   case Gmat::RVECTOR_TYPE:
   {
      Rvector rvec = EvaluateRvector();
      retval = rvec.ToString(16);
      break;
   }
   case Gmat::EQUATION_TYPE:
   {
      retval = EquationToString();
      break;
   }
   case Gmat::STRING_TYPE:
   case Gmat::ENUMERATION_TYPE:
   case Gmat::COLOR_TYPE:
   case Gmat::FILENAME_TYPE:
   {
      std::string sval = EvaluateString();
      if (dataType == Gmat::STRING_TYPE || dataType == Gmat::FILENAME_TYPE)
      {
         if (!GmatStringUtil::IsEnclosedWith(sval, "'"))
            sval = "'" + sval + "'";
      }
      retval = sval;
      break;
   }
   case Gmat::ON_OFF_TYPE:
   {
      std::string sval = EvaluateOnOff();
      retval = sval;
      break;
   }
   case Gmat::OBJECT_TYPE:
      {
         Integer hardwareType = object->GetParameterID(propIDNames.at(0));

         // Old code assumes object has a "PowerSystem" field:
         // if (hardwareType == object->GetParameterID("PowerSystem"))

         // Reworked because most objects do not have that field
         // This should be assessed and cleaned up; what follows if a bit
         // hackish to avoid breaking code close to release
         UnsignedInt powerID = Gmat::UNKNOWN_OBJECT;

         try
         {
            powerID = object->GetParameterID("PowerSystem");
         }
         catch(BaseException &ex)
         {
            // Ignore exception; reset powerID just in case
            powerID = Gmat::UNKNOWN_OBJECT;
         };

         if (hardwareType == powerID)
            retval = object->GetRefObject(Gmat::HARDWARE,
                  propIDNames.at(0))->GetGeneratingString(Gmat::MATLAB_STRUCT);
         else
            retval = EvaluateString();
      }
      break;
   case Gmat::OBJECTARRAY_TYPE:
   {
      StringArray hardwareInfo = object->GetStringArrayParameter(propID);
      for (Integer i = 0; i < hardwareInfo.size(); i++)
      {
         if (propIDNames[0] == hardwareInfo[i])
         {
            Integer hardwareNum = i;
            retval = object->GetRefObject(Gmat::HARDWARE, hardwareInfo.at(hardwareNum))->GetGeneratingString(Gmat::MATLAB_STRUCT);
            break;
         }
      }
      break;
   }
   default:
   {
      GmatBaseException be;
      be.SetDetails("ObjectPropertyWrapper::ToString() the parameter type %d is "
                    "unknown for \"%s\"", propID, description.c_str());
      throw be;
   }
   }
   
   return retval;
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the object property.
 *
 * @return data type for the object property.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ObjectPropertyWrapper::GetDataType() const
{
   #ifdef DEBUG_OPW
   MessageInterface::ShowMessage
      ("ObjectPropertyWrapper::GetDataType() entered, object=<%p><%s>'%s', propName = '%s', "
       "propID = %d\n", object, object ? object->GetTypeName().c_str() : "NULL",
       object ? object->GetName().c_str() : "NULL", propIDNames[0].c_str(), propID);
   #endif
   
   if (object == NULL)
      throw ParameterException
         ("ObjectPropertyWrapper::GetDataType() The object is NULL, "
          "so cannot get data type");
   
   Gmat::ParameterType propType = object->GetParameterType(propID);
   
   #ifdef DEBUG_OPW
   MessageInterface::ShowMessage
      ("ObjectPropertyWrapper::GetDataType() returning %d\n", propType);
   #endif
   
   return propType;
}


//------------------------------------------------------------------------------
// virtual bool SetRefObjectName(const std::string &name, Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object name for the ElementWrapper 
 * object.
 * 
 * @param <name> name of the ref object to set
 * @param <index> index of ref object name to set (0)
 *
 * @return true for success; false for failure.
 *
 */
//------------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetRefObjectName(const std::string &name, Integer index)
{
   if (index >= (Integer)refObjectNames.size())
      return false;
   
   refObjectNames[index] = name;
   return true;
}


//------------------------------------------------------------------------------
//  const StringArray&  GetRefObjectNames() 
//------------------------------------------------------------------------------
/**
 * @see ElementWrapper
 */
//------------------------------------------------------------------------------
const StringArray& ObjectPropertyWrapper::GetRefObjectNames()
{
   #ifdef DEBUG_OPW
   MessageInterface::ShowMessage
      ("ObjectPropertyWrapper::GetRefObjectNames() entered, object=<%p><%s>'%s'\n   "
       "description='%s'\n",  object, object ? object->GetTypeName().c_str() : "NULL",
       object ? object->GetName().c_str() : "NULL", description.c_str());
   MessageInterface::ShowMessage
      ("   returning %d ref object names\n", refObjectNames.size());
   for (UnsignedInt i=0; i<refObjectNames.size(); i++)
   MessageInterface::ShowMessage("   '%s'\n", refObjectNames[i].c_str());
   #endif
   
   return refObjectNames;
}


//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * @see ElementWrapper
 */
//------------------------------------------------------------------------------
GmatBase* ObjectPropertyWrapper::GetRefObject(const std::string &name)
{
   return object;
}


//---------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj)
//---------------------------------------------------------------------------
/**
 * Method to set the reference object (ObjectProperty) pointer on the wrapped 
 * object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetRefObject(GmatBase *obj)
{
   if (obj == NULL)
   {
      #ifdef DEBUG_OPW
      MessageInterface::ShowMessage
         ("***** In ObjPropWrapper::SetRefObject, the input object is NULL, "
          "so returning false\n");
      #endif
      return false;
   }
   
   #ifdef DEBUG_OPW
   MessageInterface::ShowMessage
      ("ObjectPropertyWrapper::SetRefObject() entered, obj=<%p><%s><%s>\n   "
       "refObjectNames[0]=<%s>, propIDNames[0]=<%s>, ownedObjName=<%s>\n",
       obj, obj->GetTypeName().c_str(), obj->GetName().c_str(),
       refObjectNames[0].c_str(), propIDNames[0].c_str(), ownedObjName.c_str());
   #endif
   
//   if (obj->GetName() != refObjectNames[0])
//   {
//
//      errmsg += object->GetName();
//      errmsg += "\" was passed into the object property wrapper \"";
//      errmsg += description;
//      errmsg += "\", but an object named \"";
//      errmsg += refObjectNames[0];
//      errmsg += "\" was expected.\n";
//      throw ParameterException(errmsg);
//   }
   
   if (obj->GetName() == refObjectNames[0])
   {
      object = obj;
      
      // Handle owned Propagators as a special case
      try
      {
         propID = object->GetParameterID(propIDNames[0]);
         #ifdef DEBUG_OPW
         MessageInterface::ShowMessage
            ("   For parameter '%s', propID = %d\n", propIDNames[0].c_str(), propID);
         #endif
      }
      catch (GmatBaseException ex)
      {
         #ifdef DEBUG_OPW
         MessageInterface::ShowMessage
            ("   For parameter '%s', propID not found, so trying owned object\n", propIDNames[0].c_str());
         #endif
         // Handle the Propagator inside a PropSetup
         if (obj->IsOfType(Gmat::PROP_SETUP))
         {
            object = (GmatBase*)(((PropSetup *)obj)->GetPropagator());
            propID = object->GetParameterID(propIDNames[0]);
         }
         // Handle the AxisSystem inside CoordinateSystem (LOJ: 2012.11.16)
         else if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
         {
            object = obj->GetRefObject(Gmat::AXIS_SYSTEM, "");
            propID = object->GetParameterID(propIDNames[0]);
         }
         else
            throw;
      }
      #ifdef DEBUG_OPW
         MessageInterface::ShowMessage(
         "In ObjPropWrapper::SetRefObject, setting to object %s\n",
         obj->GetName().c_str());
         MessageInterface::ShowMessage("      and propID = %d\n", propID);
      #endif
      return true;
   }
   #ifdef DEBUG_OPW
   MessageInterface::ShowMessage
      ("***** In ObjPropWrapper::SetRefObject, object name '%s' does not match with "
       "ref object name '%s', so just returning false\n", obj->GetName().c_str(),
       refObjectNames[0].c_str());
   #endif
   
   return false;
}


//---------------------------------------------------------------------------
//  bool RenameObject(const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Method to rename a reference object for the wrapper.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::RenameObject(const std::string &oldName, 
                                         const std::string &newName)
{
   ElementWrapper::RenameObject(oldName, newName);
   // now rebuild the description string from the refObjectNames
   Integer pos = description.find(".");
   if (description.find(".") != std::string::npos)
   {
      #ifdef DEBUG_RENAME_OBJ_PROP
         MessageInterface::ShowMessage(
         "Found a dot at position %d in the description string %s\n",
         (Integer) pos, description.c_str());
      #endif
      description.replace(0,pos,refObjectNames[0]);
      #ifdef DEBUG_RENAME_OBJ_PROP
         MessageInterface::ShowMessage(
         "--- replacing with %s\n", (refObjectNames[0]).c_str());
         MessageInterface::ShowMessage(
         "--- and now description is %s\n", description.c_str());
      #endif
   }
   else // this shouldn't happen, but ...
   {
      std::string errmsg = 
         "Expecting a \".\" in the description for object property \"";
      errmsg += description + " \"";
      throw ParameterException(errmsg);
   }
   return true;
}

//---------------------------------------------------------------------------
//  Real EvaluateReal() const
//---------------------------------------------------------------------------
/**
 * Method to return the Real value of the wrapped object.
 *
 * @return Real value of the wrapped number object.
 * 
 */
//---------------------------------------------------------------------------
Real ObjectPropertyWrapper::EvaluateReal() const
{
   if (object == NULL)
      throw ParameterException(
      "Cannot return value of ObjectProperty - object pointer is NULL\n");
   
   Real itsValue;
   try
   {
      Gmat::ParameterType propType = GetDataType();
      if (propType == Gmat::INTEGER_TYPE)
      {
         itsValue = (Real)(object->GetIntegerParameter(propID));
      }
      else if (propType == Gmat::REAL_TYPE)
      {
         itsValue = object->GetRealParameter(propID);
      }
      else
      {
         throw GmatBaseException
            ("EvaluateReal() method not valid for wrapper of non-Integer or non-Real type.\n");
      }
      
      #ifdef DEBUG_OPW
         MessageInterface::ShowMessage(
         "In ObjPropWrapper::EvaluateReal, value = %.12f\n", itsValue);
      #endif
   }
   catch (BaseException &be)
   {
      //std::stringstream errmsg;
      //errmsg << be.GetFullMessage(); // << std::endl;
      //throw ParameterException(errmsg.str());
      
      // Just rethrow here since ref object is not a Parameter object which
      // is confusing when Parameter exception: is shown (LOJ: 2014.04.17)
      throw;
   }
   
   return itsValue;
}

//---------------------------------------------------------------------------
//  bool SetReal(const Real toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetReal(const Real toValue)
{
   if (object == NULL)
      throw ParameterException(
      "Cannot set value of ObjectProperty - object pointer is NULL\n");

   try
   {
      #ifdef DEBUG_OPW
         MessageInterface::ShowMessage(
         "In ObjPropWrapper::SetReal, about to set value to %.12f\n", toValue);
      #endif
      object->SetRealParameter(propID, toValue);
      #ifdef DEBUG_OPW
         MessageInterface::ShowMessage(
         "In ObjPropWrapper::SetReal, value has been set to %.12f\n", toValue);
      #endif
   }
   catch (BaseException &be)
   {
      #ifdef DEBUG_OPW
         MessageInterface::ShowMessage(
         "   exception thrown!  msg = %s\n", (be.GetFullMessage()).c_str());
      #endif
         
      //std::stringstream errmsg;
//      errmsg << "Cannot set Real value for id \"" << propID;
//      errmsg << "\" for object \"" << object->GetName();
//      errmsg << "\" - exception thrown: "<< be.GetFullMessage() << std::endl;
      //errmsg << be.GetFullMessage() << std::endl;
      //throw ParameterException(errmsg.str());
      
      // Just rethrow here since ref object is not a Parameter object which
      // is confusing  when Parameter exception: is shown (LOJ: 2014.04.17)
      throw;
   }
   
   return true;
}

//---------------------------------------------------------------------------
// bool SetArray(const Rmatrix &toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Rmatrix value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetArray(const Rmatrix &toValue)
{
   if (object == NULL)
      throw ParameterException(
      "Cannot set value of ObjectProperty - object pointer is NULL\n");
   
   try
   {
      #ifdef DEBUG_OPW
      MessageInterface::ShowMessage
         ("In ObjPropWrapper::SetArray, about to set value to %s\n", toValue.ToString().c_str());
      #endif
      object->SetRmatrixParameter(propID, toValue);
      #ifdef DEBUG_OPW
      MessageInterface::ShowMessage
         ("In ObjPropWrapper::SetRmatrix, value has been set to %s\n", toValue.ToString().c_str());
      #endif
   }
   catch (BaseException &be)
   {
      #ifdef DEBUG_OPW
      MessageInterface::ShowMessage
         ("   exception thrown!  msg = %s\n", (be.GetFullMessage()).c_str());
      #endif
      
      throw;
   }
   
   return true;
}

//---------------------------------------------------------------------------
// const Rmatrix& EvaluateArray() const
//---------------------------------------------------------------------------
/**
 * Method to retrieve the Rmatrix value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
const Rmatrix& ObjectPropertyWrapper::EvaluateArray() const
{
   if (object == NULL)
      throw ParameterException(
                               "Cannot set value of ObjectProperty - object pointer is NULL\n");
   
   try
   {
#ifdef DEBUG_OPW
      MessageInterface::ShowMessage
      ("In ObjPropWrapper::EvaluateArray, about to get value of %s\n", GetDescription().c_str());
      const Rmatrix rmat = object->GetRmatrixParameter(propID);
      MessageInterface::ShowMessage
      ("In ObjPropWrapper::EvaluateArray, value retrieve is %s\n", rmat.ToString(16).c_str());
#endif
      return object->GetRmatrixParameter(propID);
   }
   catch (BaseException &be)
   {
#ifdef DEBUG_OPW
      MessageInterface::ShowMessage
      ("   exception thrown!  msg = %s\n", (be.GetFullMessage()).c_str());
#endif
      
      throw;
   }
}
//---------------------------------------------------------------------------
// const Rmatrix& EvaluateRvector() const
//---------------------------------------------------------------------------
/**
 * Method to retrieve the Rmatrix value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
const Rvector& ObjectPropertyWrapper::EvaluateRvector() const
{
   if (object == NULL)
      throw ParameterException(
           "Cannot set value of ObjectProperty - object pointer is NULL\n");
   
   try
   {
#ifdef DEBUG_OPW
      MessageInterface::ShowMessage
      ("In ObjPropWrapper::EvaluateRvector, about to get value of %s\n", GetDescription().c_str());
      const Rvector rvec = object->GetRvectorParameter(propID);
      MessageInterface::ShowMessage
      ("In ObjPropWrapper::EvaluateRvector, value retrieve is %s\n", rvec.ToString(16).c_str());
#endif
      return object->GetRvectorParameter(propID);
   }
   catch (BaseException &be)
   {
#ifdef DEBUG_OPW
      MessageInterface::ShowMessage
      ("   exception thrown!  msg = %s\n", (be.GetFullMessage()).c_str());
#endif
      
      throw;
   }
}

//---------------------------------------------------------------------------
//  bool SetRvector(const Rvector &toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Rvector value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetRvector(const Rvector &toValue)
{
   if (object == NULL)
      throw ParameterException(
      "Cannot set value of ObjectProperty - object pointer is NULL\n");

   try
   {
      #ifdef DEBUG_OPW
      MessageInterface::ShowMessage
         ("In ObjPropWrapper::SetRvector, about to set value to %s\n", toValue.ToString().c_str());
      #endif
      object->SetRvectorParameter(propID, toValue);
      #ifdef DEBUG_OPW
      MessageInterface::ShowMessage
         ("In ObjPropWrapper::SetRvector, value has been set to %s\n", toValue.ToString().c_str());
      #endif
   }
   catch (BaseException &be)
   {
      #ifdef DEBUG_OPW
      MessageInterface::ShowMessage
         ("   exception thrown!  msg = %s\n", (be.GetFullMessage()).c_str());
      #endif
      
      //std::stringstream errmsg;
      //errmsg << be.GetFullMessage() << std::endl;
      //throw ParameterException(errmsg.str());
      
      // Just rethrow here since ref object is not a Parameter object which
      // is confusing  when Parameter exception: is shown (LOJ: 2014.04.17)
      throw;
   }
   
   return true;
}


//---------------------------------------------------------------------------
// std::string EvaluateString() const
//---------------------------------------------------------------------------
std::string ObjectPropertyWrapper::EvaluateString() const
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::STRING_TYPE || propType == Gmat::ON_OFF_TYPE ||
       propType == Gmat::ENUMERATION_TYPE ||
       propType == Gmat::FILENAME_TYPE || propType == Gmat::COLOR_TYPE ||
       propType == Gmat::OBJECT_TYPE)  // Added OBJECT_TYPE to handle "DefaultFM.CentralBody"
      return object->GetStringParameter(propID);
   else
      throw GmatBaseException
         ("ObjectPropertyWrapper::EvaluateString() method not valid for "
          "wrapper of non-String type for field " +
          object->GetParameterText(propID) + ".\n");
}

//---------------------------------------------------------------------------
// bool SetString(const std::string &toValue)
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetString(const std::string &toValue)
{
#ifdef DEBUG_OBJ_PROP_SET_STRING
   MessageInterface::ShowMessage("Entering OBWrapper::SetString with toValue = %s\n",
         toValue.c_str());
   MessageInterface::ShowMessage("   and data type = %d\n", (Integer) GetDataType());
   MessageInterface::ShowMessage
      ("   object = <%p><%s>'%s'\n", object, object->GetTypeName().c_str(),
       object->GetName().c_str());
#endif
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::STRING_TYPE ||
       propType == Gmat::ENUMERATION_TYPE ||
       propType == Gmat::COLOR_TYPE ||
       propType == Gmat::FILENAME_TYPE ||
       propType == Gmat::STRINGARRAY_TYPE ||
       propType == Gmat::OBJECT_TYPE) // Added OBJECT_TYPE to handle "DefaultFM.Drag = None;"
      return object->SetStringParameter(propID, toValue);
   else if (propType == Gmat::BOOLEANARRAY_TYPE)
   {
      BooleanArray boolArray = GmatStringUtil::ToBooleanArray(toValue);
      return object->SetBooleanArrayParameter(propID, boolArray);
   }
   else if (propType == Gmat::UNSIGNED_INTARRAY_TYPE)
      return object->SetStringParameter(propID, toValue);
   else if (propType == Gmat::RVECTOR_TYPE)  // added to handle Rvectors with brackets
      return object->SetStringParameter(propID, toValue);
   else
      throw GmatBaseException
         ("SetString() method not valid for wrapper of non-String type.\n");
}

//---------------------------------------------------------------------------
// std::string EvaluateOnOff() const
//---------------------------------------------------------------------------
std::string ObjectPropertyWrapper::EvaluateOnOff() const
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::ON_OFF_TYPE)
      return object->GetOnOffParameter(propID);
   else
      throw GmatBaseException
         ("EvaluateOnOff() method not valid for wrapper of non-OnOff type.\n");
}

//---------------------------------------------------------------------------
// bool SetOnOff(const std::string &toValue)
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetOnOff(const std::string &toValue)
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::ON_OFF_TYPE)
      return object->SetOnOffParameter(propID, toValue);
   else
      throw GmatBaseException
         ("SetOnOff() method not valid for wrapper of non-OnOff type.\n");
}

//---------------------------------------------------------------------------
// bool EvaluateBoolean() const
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::EvaluateBoolean() const
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::BOOLEAN_TYPE)
      return object->GetBooleanParameter(propID);
   else
      throw GmatBaseException
         ("EvaluateBoolean() method not valid for wrapper of non-Boolean type.\n");
}

//---------------------------------------------------------------------------
// bool SetBoolean(const bool toValue)
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetBoolean(const bool toValue)
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::BOOLEAN_TYPE)
      return object->SetBooleanParameter(propID, toValue);
   else
      throw GmatBaseException
         ("SetBoolean() method not valid for wrapper of non-Boolean type.\n");
}

//---------------------------------------------------------------------------
// Integer EvaluateInteger() const
//---------------------------------------------------------------------------
Integer ObjectPropertyWrapper::EvaluateInteger() const
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::INTEGER_TYPE)
      return object->GetIntegerParameter(propID);
   else
      throw GmatBaseException
         ("EvaluateInteger() method not valid for wrapper of non-Integer type.\n");
}

//---------------------------------------------------------------------------
// bool SetInteger(const Integer toValue)
//---------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetInteger(const Integer toValue)
{
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::INTEGER_TYPE)
   {
      Integer retval = object->SetIntegerParameter(propID, toValue);
      return (retval == 0 ? false : true);
//      return true;
   }
   else
      throw GmatBaseException
         ("SetInteger() method not valid for wrapper of non-Integer type.\n");
}


//------------------------------------------------------------------------------
// bool SetObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Method to set the object of the wrapped object.
 *
 * @param <obj> The object pointer to set
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool ObjectPropertyWrapper::SetObject(GmatBase *obj)
{
   #ifdef DEBUG_OPW
   MessageInterface::ShowMessage
      ("ObjectPropertyWrapper::SetObject() obj=<%p><%s>'%s'\n   object=<%p><%s>'%s'\n",
       obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL",
       object, object ? object->GetTypeName().c_str() : "NULL",
       object ? object->GetName().c_str() : "NULL");       
   #endif
   
   if (obj == NULL)
   {
      throw ParameterException
         ("Cannot set undefined object to object property \"" +
          GetDescription() +  "\"");         
   }
   
   if (object == NULL)
   {
      throw ParameterException
         ("The object is not set \"" + GetDescription() +  "\"");         
   }
      
   Gmat::ParameterType propType = GetDataType();
   if (propType == Gmat::OBJECT_TYPE || propType == Gmat::OBJECTARRAY_TYPE)
   {
      if (object->SetStringParameter(propID, obj->GetName()))
         return object->SetRefObject(obj, obj->GetType(), obj->GetName());
   }
   else
      throw GmatBaseException
         ("ObjectPropertyWrapper::SetObject() method not valid for wrapper of non-Object type.\n");
   
   return true;
}


//------------------------------------------------------------------------------
// std::string EquationToString() const
//------------------------------------------------------------------------------
/**
 * Fires an equation parameter and generates the resulting string representation
 *
 * @return The string results
 */
//------------------------------------------------------------------------------
std::string ObjectPropertyWrapper::EquationToString() const
{
   Real results = object->GetRealParameter(propID);
   std::string value = GmatStringUtil::ToString(results);
   return value;
}


//------------------------------------------------------------------------------
// Integer GetPropertyId()
//------------------------------------------------------------------------------
/**
 * Retrieves the object's property ID
 *
 * @return The ID
 */
//------------------------------------------------------------------------------
Integer ObjectPropertyWrapper::GetPropertyId()
{
   return propID;
}

//------------------------------------------------------------------------------
// const StringArray& GetPropertyNames()
//------------------------------------------------------------------------------
/**
 * Retrieves the object's property names
 *
 * @return The property names
 */
//------------------------------------------------------------------------------
const StringArray& ObjectPropertyWrapper::GetPropertyNames()
{
   return propIDNames;
}


bool ObjectPropertyWrapper::TakeRequiredAction() const
{
   return object->TakeRequiredAction(propID);
}


//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method to set up the Object Property Wrapper.
 *
 */
//---------------------------------------------------------------------------
void ObjectPropertyWrapper::SetupWrapper()
{
   std::string type, owner, depobj;
   GmatStringUtil::ParseParameter(description, type, owner, depobj);
   
   #ifdef DEBUG_OPW
   MessageInterface::ShowMessage
      ("In ObjPropWrapper::SetupWrapper, desc='%s'\n   owner='%s', depobj='%s', "
       "type='%s'\n", description.c_str(), owner.c_str(), depobj.c_str(), type.c_str());
   #endif
   //if (depobj != "")
   //{
   //   throw ParameterException(
   //   "Dependent objects not yet supported for the object property wrapper \"" +
   //   description + "\".\n");
   //   /// @todo Handle object properties that use dependent objects here
   //}   
   if (owner == "")
   {
      throw ParameterException(
      "Owner object name is empty for the object property wrapper \"" +
      description + "\".\n");
   }
   if (type == "")
   {
      throw ParameterException(
      "Property ID string is empty for the object property wrapper \"" +
      description + "\".\n");
   }
   refObjectNames.push_back(owner);
   propIDNames.push_back(type);   
   ownedObjName = depobj;
}
