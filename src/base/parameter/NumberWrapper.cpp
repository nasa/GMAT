//$Id$
//------------------------------------------------------------------------------
//                                  NumberWrapper
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
 * Implementation of the NumberWrapper class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "NumberWrapper.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"

#include "MessageInterface.hpp"

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  NumberWrapper(;
//---------------------------------------------------------------------------
/**
 * Constructs NumberWrapper structures
 * (default constructor).
 *
 */
//---------------------------------------------------------------------------
NumberWrapper::NumberWrapper() :
   ElementWrapper()
{
   value       = ElementWrapper::UNDEFINED_REAL;
   wrapperType = Gmat::NUMBER_WT;
}

//---------------------------------------------------------------------------
//  NumberWrapper(const NumberWrapper &nr);
//---------------------------------------------------------------------------
/**
 * Constructs base NumberWrapper structures used in derived classes, by 
 * copying the input instance (copy constructor).
 *
 * @param <nr>  NumberWrapper instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
NumberWrapper::NumberWrapper(const NumberWrapper &nr) :
   ElementWrapper(nr),
   value         (nr.value)
{
}

//---------------------------------------------------------------------------
//  NumberWrapper& operator=(const NumberWrapper &nr)
//---------------------------------------------------------------------------
/**
 * Assignment operator for NumberWrapper structures.
 *
 * @param <nr> The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const NumberWrapper& NumberWrapper::operator=(const NumberWrapper &nr)
{
   if (&nr == this)
      return *this;
   
   ElementWrapper::operator=(nr);
   value = nr.value;

   return *this;
}

//---------------------------------------------------------------------------
//  ~NumberWrapper()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
NumberWrapper::~NumberWrapper()
{
}

//------------------------------------------------------------------------------
// virtual ElementWrapper* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to create a copy of the object
 */
//------------------------------------------------------------------------------
ElementWrapper* NumberWrapper::Clone() const
{
   return new NumberWrapper(*this);
}

//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return NumberWrapper value converted to std::string.
 */
//------------------------------------------------------------------------------
std::string NumberWrapper::ToString()
{
   return GmatStringUtil::ToString(value);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType GetDataType() const
//------------------------------------------------------------------------------
/**
 * This method returns the data type for the NumberWrapper object.
 *
 * @return data type for the object.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType NumberWrapper::GetDataType() const
{
   return Gmat::REAL_TYPE;
}

//---------------------------------------------------------------------------
//  Real EvaluateReal() const
//---------------------------------------------------------------------------
/**
 * Method to return the Real value of the wrapped number object.
 *
 * @return Real value of the wrapped number object.
 * 
 */
//---------------------------------------------------------------------------
Real NumberWrapper::EvaluateReal() const
{
   return value;
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
bool NumberWrapper::SetReal(const Real toValue)
{
   value = toValue;
   return true;
}


//---------------------------------------------------------------------------
//  void SetupWrapper()
//---------------------------------------------------------------------------
/**
 * Method to set up the Number Wrapper.
 *
 */
//---------------------------------------------------------------------------
void NumberWrapper::SetupWrapper()
{
   // Changed this so that math equation such as "2+2" or "x" will work
   // as GmatFunction input value(loj:2008.08.27)
   if (GmatStringUtil::ToReal(description, value) == false)
   {
      if (!GmatStringUtil::IsMathEquation(description) &&
          !GmatStringUtil::IsValidName(description))
      {
         std::string errmsg = "For number wrapper \"";
         errmsg += description;
         errmsg += 
            "\", the description string does not evaluate to a real number\n"; 
         throw ParameterException(errmsg);
      }
   }
}
