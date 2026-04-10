//$Id$
//------------------------------------------------------------------------------
//                                  StringWrapper
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun/GSFC
// Created: 2007/07/24
//
/**
 * Declares StringWrapper class.
 */
//------------------------------------------------------------------------------

#include "StringWrapper.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
//  StringWrapper()
//------------------------------------------------------------------------------
/**
 * Constructs StringWrapper structures
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
StringWrapper::StringWrapper() :
   ElementWrapper()
{
   value       = "";
   wrapperType = Gmat::STRING_WT;
}


//------------------------------------------------------------------------------
//  StringWrapper(const StringWrapper &copy);
//------------------------------------------------------------------------------
/**
 * Constructs base StringWrapper structures used in derived classes, by 
 * copying the input instance (copy constructor).
 *
 * @param <copy>  StringWrapper instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
StringWrapper::StringWrapper(const StringWrapper &copy) :
   ElementWrapper(copy),
   value         (copy.value)
{
}


//------------------------------------------------------------------------------
//  StringWrapper& operator=(const StringWrapper &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator for StringWrapper structures.
 *
 * @param <right> The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const StringWrapper& StringWrapper::operator=(const StringWrapper &right)
{
   if (&right == this)
      return *this;
   
   ElementWrapper::operator=(right);
   value = right.value;
   
   return *this;
}


//------------------------------------------------------------------------------
//  ~StringWrapper()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
StringWrapper::~StringWrapper()
{
}


//------------------------------------------------------------------------------
// ElementWrapper* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this wrapper
 *
 * @return The wrapper copy
 */
//------------------------------------------------------------------------------
ElementWrapper* StringWrapper::Clone() const
{
   return new StringWrapper(*this);
}


//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return StringWrapper value converted to std::string.
 */
//------------------------------------------------------------------------------
std::string StringWrapper::ToString()
{
   return value;
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the StringWrapper object.
 *
 * @return data type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType StringWrapper::GetDataType() const
{
   return Gmat::STRING_TYPE;
}


//---------------------------------------------------------------------------
//  Real EvaluateReal() const
//---------------------------------------------------------------------------
/**
 * Method to return the Real value of the wrapped object.
 *
 * @return Real value of the wrapped number object.
 * 
 * @note This method is not value for Strings.
 * 
 */
//---------------------------------------------------------------------------
Real StringWrapper::EvaluateReal() const
{
   throw ParameterException(
      "EvaluateReal() method not valid for wrapper of String type.\n");
}


//---------------------------------------------------------------------------
//  bool SetReal(const Real toValue)
//---------------------------------------------------------------------------
/**
 * Method to set the Real value of the wrapped object.
 *
 * @return true if successful; false otherwise.
 * 
 * @note This method is not value for Strings.
 */
//---------------------------------------------------------------------------
bool StringWrapper::SetReal(const Real toValue)
{
   throw ParameterException(
      "SetReal() method not valid for wrapper of String type.\n");
}


//------------------------------------------------------------------------------
//  String EvaluateString() const
//------------------------------------------------------------------------------
/**
 * Method to return the String value of the StringWrapper object.
 *
 * @return value of the StringWrapper object.
 * 
 */
//------------------------------------------------------------------------------
std::string StringWrapper::EvaluateString() const
{
   // Remove single quotes before returning
   std::string newValue = GmatStringUtil::RemoveEnclosingString(value, "'");
   //return value;
   return newValue;
}


//------------------------------------------------------------------------------
//  bool SetString(const std::string &val)
//------------------------------------------------------------------------------
/**
 * Method to set the std::string value of the wrapped object.
 *
 * @param <val> The string value to set
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool StringWrapper::SetString(const std::string &val)
{
   value = val;
   return true;
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
//  void SetupWrapper()
//------------------------------------------------------------------------------
/**
 * Method to set up the String Wrapper.
 *
 */
//------------------------------------------------------------------------------
void StringWrapper::SetupWrapper()
{
   value = description;
}
