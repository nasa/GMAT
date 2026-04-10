//$Id$
//------------------------------------------------------------------------------
//                                  ParameterWrapper
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
 * Implementation of the ParameterWrapper class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ParameterWrapper.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"           // for ReplaceName()
#include "MessageInterface.hpp"

//#define DEBUG_PW
//#define DEBUG_PW_POINTER
//#define DEBUG_PW_REFOBJ
//#define DEBUG_PARAMETER_WRAPPER
//#define DEBUG_RENAME

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  ParameterWrapper();
//---------------------------------------------------------------------------
/**
 * Constructs ParameterWrapper structures
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
ParameterWrapper::ParameterWrapper() :
   ElementWrapper(),
   param         (NULL)
{
   wrapperType = Gmat::PARAMETER_WT;
}

//---------------------------------------------------------------------------
//  ParameterWrapper(const ParameterWrapper &pw);
//---------------------------------------------------------------------------
/**
 * Constructs base ParameterWrapper structures used in derived classes, by 
 * copying the input instance (copy constructor).
 *
 * @param <pw>  ParameterWrapper instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
ParameterWrapper::ParameterWrapper(const ParameterWrapper &pw) :
   ElementWrapper(pw),
   param         (pw.param)
{
}

//---------------------------------------------------------------------------
//  ParameterWrapper& operator=(const ParameterWrapper &pw)
//---------------------------------------------------------------------------
/**
 * Assignment operator for ParameterWrapper structures.
 *
 * @param <pw> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const ParameterWrapper& ParameterWrapper::operator=(const ParameterWrapper &pw)
{
   if (&pw == this)
      return *this;
   
   ElementWrapper::operator=(pw);
   param = pw.param;
   
   return *this;
}

//---------------------------------------------------------------------------
//  ~ParameterWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
ParameterWrapper::~ParameterWrapper()
{
   #ifdef DEBUG_PW_REFOBJ
   MessageInterface::ShowMessage
      ("ParameterWrapper::~ParameterWrapper() <%p>'%s' entered, param=<%p>\n",
       this, description.c_str(), param);
   #endif
}

//------------------------------------------------------------------------------
// virtual ElementWrapper* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
ElementWrapper* ParameterWrapper::Clone() const
{
   return new ParameterWrapper(*this);
}

//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return ParameterWrapper value converted to std::string.
 */
//------------------------------------------------------------------------------
std::string ParameterWrapper::ToString()
{
   return param->ToString();
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the ParameterWrapper object.
 *
 * @return data type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ParameterWrapper::GetDataType() const
{
   // Changed to return Parameter return type (loj: 2008.06.18)
   if (param)
      return param->GetReturnType();
   
   // What type should we return here?
   return Gmat::REAL_TYPE;
}

//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * This method retrives a reference object for the wrapper name
 * 
 * @param <name> name of the wrapper
 *
 * @return reference for success; NULL if name not found
 *
 */
//------------------------------------------------------------------------------
GmatBase* ParameterWrapper::GetRefObject(const std::string &name)
{
   #ifdef DEBUG_PW_REFOBJ
   MessageInterface::ShowMessage
      ("ParameterWrapper::GetRefObject() returning <%p>'%s'\n", param,
       param ? param->GetName().c_str() : "NULL");
   #endif
   
   // We don't need to check for the name since only one Parameter
   return (GmatBase*)param;
}

//---------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj)
//---------------------------------------------------------------------------
/**
 * Method to set the reference object (Parameter) pointer on the wrapped 
 * object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ParameterWrapper::SetRefObject(GmatBase *obj)
{
   #ifdef DEBUG_PW_REFOBJ
   MessageInterface::ShowMessage
      ("ParameterWrapper::SetRefObject() obj=<%p><%s>\n", obj,
       obj ? obj->GetName().c_str() : "NULL");
   #endif
   
//   if ( (obj->GetName() != refObjectNames[0]) ||
//        (!obj->IsOfType("Parameter")) )
//   {
//      std::string errmsg = "Referenced object \"";
//      errmsg += obj->GetName();
//      errmsg += "\" was passed into the parameter wrapper \"";
//      errmsg += description;
//      errmsg += "\", but an object named \"";
//      errmsg += refObjectNames[0];
//      errmsg += "\" was expected.\n";
//      throw ParameterException(errmsg);
//   }
   if ( (obj->GetName() == refObjectNames[0]) || (obj->IsOfType("Parameter")) )
   {
      param = (Parameter*) obj;
      return true;
   }
   else return false;
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
Real ParameterWrapper::EvaluateReal() const
{
   if (param == NULL)
      throw ParameterException(
      "Cannot return value of Parameter - pointer is NULL\n");
   #ifdef DEBUG_PW
      MessageInterface::ShowMessage(
      "In ParameterWrapper::EvaluateReal, value is %.12f\n", param->EvaluateReal());
   #endif

   #ifdef DEBUG_PW_POINTER
   MessageInterface::ShowMessage
      ("ParameterWrapper: Parameter %s is at %p\n", param->GetName().c_str(), param);
   GmatBase *paramOwner = param->GetOwner();
   MessageInterface::ShowMessage
      ("ParameterWrapper::EvaluateReal() '%s', owner of param = <%p>'%s'\n",
       description.c_str(), paramOwner, paramOwner ? paramOwner->GetName().c_str() : "NULL");
   #endif
   return param->EvaluateReal();
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
bool ParameterWrapper::SetReal(const Real toValue)
{
   if (param == NULL)
      throw ParameterException(
      "Cannot set value of Parameter - pointer is NULL\n");
   param->SetReal(toValue);
   return true;
}


//---------------------------------------------------------------------------
// virtual std::string EvaluateString() const;
//---------------------------------------------------------------------------
/**
 * Method to return the String value of the wrapped object.
 *
 * @return Real value of the wrapped number object.
 * 
 */
//---------------------------------------------------------------------------
std::string ParameterWrapper::EvaluateString() const
{
   if (param == NULL)
      throw ParameterException(
      "Cannot return value of Parameter - pointer is NULL\n");
   #ifdef DEBUG_PW
      MessageInterface::ShowMessage(
      "In ParameterWrapper::EvaluateString, value is '%s'\n", (param->EvaluateString()).c_str());
   #endif
   return param->EvaluateString();
}


//---------------------------------------------------------------------------
// virtual bool SetString(const std::string &toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the String value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ParameterWrapper::SetString(const std::string &toValue)
{
   if (param == NULL)
      throw ParameterException(
      "Cannot set value of Parameter - pointer is NULL\n");
   param->SetString(toValue);
   return true;
}


//---------------------------------------------------------------------------
//  const Rmatrix& EvaluateArray() const
//---------------------------------------------------------------------------
/**
 * Method to return the Rmatrix value of the wrapped object.
 *
 * @return Rmatrix value of the wrapped number object.
 * 
 */
//---------------------------------------------------------------------------
const Rmatrix& ParameterWrapper::EvaluateArray() const
{
   if (param == NULL)
      throw ParameterException("Cannot return value of Parameter - pointer is NULL\n");
   
   #ifdef DEBUG_PW
   MessageInterface::ShowMessage
      ("In ParameterWrapper::EvaluateArray, value is %s\n",
       (param->EvaluateRmatrix()).ToString().c_str());
   #endif
   
   return param->EvaluateRmatrix();
}


//---------------------------------------------------------------------------
//  bool SetArray(const Rmatrix &toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ParameterWrapper::SetArray(const Rmatrix &toValue)
{
   if (param == NULL)
      throw ParameterException("Cannot set value of Parameter - pointer is NULL\n");
   
   param->SetRmatrix(toValue);
   return true;
}


//---------------------------------------------------------------------------
//  const Rvector& EvaluateRvector() const
//---------------------------------------------------------------------------
/**
 * Method to return the Rvector value of the wrapped object.
 *
 * @return Rvector value of the wrapped number object.
 * 
 */
//---------------------------------------------------------------------------
const Rvector& ParameterWrapper::EvaluateRvector() const
{
   if (param == NULL)
      throw ParameterException("Cannot return value of Parameter - pointer is NULL\n");
   
   #ifdef DEBUG_PW
   MessageInterface::ShowMessage
      ("In ParameterWrapper::EvaluateRvector, value is %s\n",
       (param->EvaluateRvector()).ToString().c_str());
   #endif
   
   return param->EvaluateRvector();
}


//---------------------------------------------------------------------------
//  bool SetRvector(const Rvector &toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 */
//---------------------------------------------------------------------------
bool ParameterWrapper::SetRvector(const Rvector &toValue)
{
   if (param == NULL)
      throw ParameterException("Cannot set value of Parameter - pointer is NULL\n");
   
   param->SetRvector(toValue);
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* EvaluateObject() const
//------------------------------------------------------------------------------
/**
 * Method to return the Object pointer of the ParameterWrapper object.
 *
 * @return value of the ParameterWrapper object.
 * 
 */
//------------------------------------------------------------------------------
GmatBase* ParameterWrapper::EvaluateObject() const
{
   return param;
}


//------------------------------------------------------------------------------
// bool SetObject(const GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Method to set the object of the wrapped object.
 *
 * @param <obj> The object pointer to set
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool ParameterWrapper::SetObject(const GmatBase *obj)
{
   if (obj == NULL)
   {
      if (param == NULL)
         throw ParameterException("Cannot set undefined object to undefined object");
      else
         throw ParameterException
            ("Cannot set undefined object to object of type \"" +
             param->GetTypeName() +  "\"");
   }
   
   if (param != NULL)
   {
      // Let's check the object type
      if (param->GetTypeName() == obj->GetTypeName())
      {
         #ifdef DEBUG_PARAMETER_WRAPPER
         MessageInterface::ShowMessage
            ("ParameterWrapper::SetObject() fromType=%s, toType=%s\n",
             obj->GetTypeName().c_str(), param->GetTypeName().c_str());
         #endif
         
         param->Copy(obj);
      }
      else
      {
         ParameterException pe;
         pe.SetDetails("Cannot set object of type \"%s\" to object of type \"%s\"",
                       obj->GetTypeName().c_str(), param->GetTypeName().c_str());
         throw pe;
      }
   }
   else
   {
      throw ParameterException("Cannot set Parameter \"" + obj->GetName() +
                               "\" to an undefined object");
   }
   
   return true;
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
bool ParameterWrapper::RenameObject(const std::string &oldName, 
                                    const std::string &newName)
{
   std::string paramName;
   if (param)
      paramName = param->GetName();

   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("\nParameterWrapper::RenameObject() entered, oldName='%s', newName='%s', "
       "param=<%p>'%s'\n   description='%s'\n", oldName.c_str(), newName.c_str(), param,
       paramName.c_str(), description.c_str());
   #endif
   
   ElementWrapper::RenameObject(oldName, newName);
   // description is now renamed in ElementWrapper::RenameObject()
   //description = refObjectNames[0];
   
   // @note Parameter name gets changed in the ConfigManager
   // so nothing to do for the param
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ParameterWrapper::RenameObject() leaving, oldName='%s', newName='%s', "
       "param=<%p>'%s'\n   description='%s'\n\n", oldName.c_str(), newName.c_str(),
       param, paramName.c_str(), description.c_str());
   #endif
   
   return true;
}


//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method to set up the Parameter Wrapper.
 *
 */
//---------------------------------------------------------------------------
void ParameterWrapper::SetupWrapper()
{
   refObjectNames.push_back(description);
}

