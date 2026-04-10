//$Id$
//------------------------------------------------------------------------------
//                                  BooleanWrapper
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
 * Declares BooleanWrapper class.
 */
//------------------------------------------------------------------------------

#include "BooleanWrapper.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"         // for ToBoolean()
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
//  BooleanWrapper()
//------------------------------------------------------------------------------
/**
 * Constructs BooleanWrapper structures
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
BooleanWrapper::BooleanWrapper() :
   ElementWrapper()
{
   value       = false;
   wrapperType = Gmat::BOOLEAN_WT;
}


//------------------------------------------------------------------------------
//  BooleanWrapper(const BooleanWrapper &copy);
//------------------------------------------------------------------------------
/**
 * Constructs base BooleanWrapper structures used in derived classes, by 
 * copying the input instance (copy constructor).
 *
 * @param <copy>  BooleanWrapper instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
BooleanWrapper::BooleanWrapper(const BooleanWrapper &copy) :
   ElementWrapper(copy),
   value         (copy.value)
{
}


//------------------------------------------------------------------------------
//  BooleanWrapper& operator=(const BooleanWrapper &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator for BooleanWrapper structures.
 *
 * @param <right> The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const BooleanWrapper& BooleanWrapper::operator=(const BooleanWrapper &right)
{
   if (&right == this)
      return *this;
   
   ElementWrapper::operator=(right);
   value = right.value;

   return *this;
}


//------------------------------------------------------------------------------
//  ~BooleanWrapper()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BooleanWrapper::~BooleanWrapper()
{
}


//------------------------------------------------------------------------------
//  ElementWrapper* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new object that l.ooks like this one.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
ElementWrapper* BooleanWrapper::Clone() const
{
   return new BooleanWrapper(*this);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the BooleanWrapper object.
 *
 * @return data type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType BooleanWrapper::GetDataType() const
{
   return Gmat::BOOLEAN_TYPE;
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
Real BooleanWrapper::EvaluateReal() const
{
   throw ParameterException(
      "EvaluateReal() method not valid for wrapper of Boolean type.\n");
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
bool BooleanWrapper::SetReal(const Real toValue)
{
   throw ParameterException(
      "SetReal() method not valid for wrapper of Boolean type.\n");
}


//------------------------------------------------------------------------------
//  bool EvaluateBoolean() const
//------------------------------------------------------------------------------
/**
 * Method to return the Boolean value of the BooleanWrapper object.
 *
 * @return value of the BooleanWrapper object.
 * 
 */
//------------------------------------------------------------------------------
bool BooleanWrapper::EvaluateBoolean() const
{
   return value;
}


//------------------------------------------------------------------------------
//  bool SetBoolean(const bool val)
//------------------------------------------------------------------------------
/**
 * Method to set the bool value of the wrapped object.
 *
 * @param <val> The bool value to set
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool BooleanWrapper::SetBoolean(const bool val)
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
 * Method to set up the Boolean Wrapper.
 *
 */
//------------------------------------------------------------------------------
void BooleanWrapper::SetupWrapper()
{
   if (GmatStringUtil::ToBoolean(description, value) == false)
   {
      std::string errmsg = "For Boolean wrapper \"";
      errmsg += description;
      errmsg += "\", the description string does not evaluate to Boolean\n"; 
      throw ParameterException(errmsg);
   }
}
