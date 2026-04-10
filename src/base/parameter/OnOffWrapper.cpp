//$Id$
//------------------------------------------------------------------------------
//                                  OnOffWrapper
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
 * Declares OnOffWrapper class.
 */
//------------------------------------------------------------------------------

#include "OnOffWrapper.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"         // for ToOnOff()
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
//  OnOffWrapper()
//------------------------------------------------------------------------------
/**
 * Constructs OnOffWrapper structures
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
OnOffWrapper::OnOffWrapper() :
   ElementWrapper()
{
   value       = "";
   wrapperType = Gmat::ON_OFF_WT;
}


//------------------------------------------------------------------------------
//  OnOffWrapper(const OnOffWrapper &copy);
//------------------------------------------------------------------------------
/**
 * Constructs base OnOffWrapper structures used in derived classes, by 
 * copying the input instance (copy constructor).
 *
 * @param <copy>  OnOffWrapper instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
OnOffWrapper::OnOffWrapper(const OnOffWrapper &copy) :
   ElementWrapper(copy),
   value         (copy.value)
{
}


//------------------------------------------------------------------------------
//  OnOffWrapper& operator=(const OnOffWrapper &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator for OnOffWrapper structures.
 *
 * @param <right> The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const OnOffWrapper& OnOffWrapper::operator=(const OnOffWrapper &right)
{
   if (&right == this)
      return *this;
   
   ElementWrapper::operator=(right);
   value = right.value;

   return *this;
}


//------------------------------------------------------------------------------
//  ~OnOffWrapper()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
OnOffWrapper::~OnOffWrapper()
{
}


//------------------------------------------------------------------------------
// ElementWrapper* OnOffWrapper::Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this wrapper
 *
 * @return The wrapper copy
 */
//------------------------------------------------------------------------------
ElementWrapper* OnOffWrapper::Clone() const
{
   return new OnOffWrapper(*this);
}



//------------------------------------------------------------------------------
//  Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the OnOffWrapper object.
 *
 * @return data type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType OnOffWrapper::GetDataType() const
{
   return Gmat::ON_OFF_TYPE;
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
Real OnOffWrapper::EvaluateReal() const
{
   throw ParameterException(
      "EvaluateReal() method not valid for wrapper of OnOff type.\n");
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
bool OnOffWrapper::SetReal(const Real toValue)
{
   throw ParameterException(
      "SetReal() method not valid for wrapper of OnOff type.\n");
}


//------------------------------------------------------------------------------
//  std::string EvaluateOnOff() const
//------------------------------------------------------------------------------
/**
 * Method to return the string value of the OnOffWrapper object.
 *
 * @return value of the OnOffWrapper object.
 * 
 */
//------------------------------------------------------------------------------
std::string OnOffWrapper::EvaluateOnOff() const
{
   return value;
}


//------------------------------------------------------------------------------
//  bool SetOnOff(const std::string &val)
//------------------------------------------------------------------------------
/**
 * Method to set the string value of the wrapped object.
 *
 * @param <val> The bool value to set
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool OnOffWrapper::SetOnOff(const std::string &val)
{
   if (val == "On" || val == "Off")
      value = val;
   else
      throw ParameterException("Input value is not \"On\" or \"Off\"");
   
   return true;
}


//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
//  void SetupWrapper()
//------------------------------------------------------------------------------
/**
 * Method to set up the OnOff Wrapper.
 *
 */
//------------------------------------------------------------------------------
void OnOffWrapper::SetupWrapper()
{
   if (description != "On" && description != "Off")
   {
      std::string errmsg = "For OnOff wrapper \"";
      errmsg += description;
      errmsg += "\", the description string is not \"On\" or \"Off\"\n"; 
      throw ParameterException(errmsg);
   }
   
   value = description;
}
