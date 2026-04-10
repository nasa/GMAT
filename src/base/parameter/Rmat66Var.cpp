//$Id$
//------------------------------------------------------------------------------
//                                  Rmat66Var
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
// Author: Linda Jun (GSFC/NASA)
// Created: 2009.03.30
//
/**
 * Defines base class of Rmatrix parameters.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Rmat66Var.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// Rmat66Var::Rmat66Var(const std::string &name, const std::string &typeStr,
//                      GmatParam::ParameterKey key, GmatBase *obj,
//                      const std::string &desc, const std::string &unit,
//                      GmatParam::DepObject depObj, UnsignedInt ownerType)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> parameter name
 * @param <typeStr> parameter type string
 * @param <key>  parameter key (SYSTEM_PARAM, USER_PARAM, etc)
 * @param <obj>  reference object pointer
 * @param <desc> parameter description
 * @param <unit> parameter unit
 * @param <depObj> object which parameter is dependent on (COORD_SYS, ORIGIN, NONE)
 * @param <ownerType> object type who owns this parameter as property
 * @param <isSettable> true if parameter is settable, false otherwise
 *
 * @exception <ParameterException> thrown if parameter name has blank spaces
 */
//------------------------------------------------------------------------------
Rmat66Var::Rmat66Var(const std::string &name, const std::string &typeStr,
                     GmatParam::ParameterKey key, GmatBase *obj,
                     const std::string &desc, const std::string &unit,
                     GmatParam::DepObject depObj, UnsignedInt ownerType,
                     bool isSettable)
   : Parameter(name, typeStr, key, obj, desc, unit, depObj, ownerType, false,
               false, false, true)
{
   mReturnType = Gmat::RMATRIX_TYPE;
}


//------------------------------------------------------------------------------
// Rmat66Var(const Rmat66Var &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the object being copied.
 */
//------------------------------------------------------------------------------
Rmat66Var::Rmat66Var(const Rmat66Var &copy)
   : Parameter(copy)
{
   mRmat66Value = copy.mRmat66Value;
}

//------------------------------------------------------------------------------
// Rmat66Var& operator= (const Rmat66Var& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
Rmat66Var& Rmat66Var::operator= (const Rmat66Var& right)
{
   if (this != &right)
   {
      Parameter::operator=(right);
      mRmat66Value = right.mRmat66Value;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// ~Rmat66Var()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Rmat66Var::~Rmat66Var()
{
}

//------------------------------------------------------------------------------
// bool operator==(const Rmat66Var &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are the same as this object.
 */
//------------------------------------------------------------------------------
bool Rmat66Var::operator==(const Rmat66Var &right) const
{
   return Parameter::operator==(right);
}

//------------------------------------------------------------------------------
// bool operator!=(const Rmat66Var &right) const
//------------------------------------------------------------------------------
/**
 * @return true if input object's type and name are not the same as this object.
 */
//------------------------------------------------------------------------------
bool Rmat66Var::operator!=(const Rmat66Var &right) const
{
   return Parameter::operator!=(right);
}

//------------------------------------------------------------------------------
// std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return parameter value converted to std::string.
 *
 * @exception <ParameterException> thrown if this method is called.
 */
//------------------------------------------------------------------------------
std::string Rmat66Var::ToString()
{
   Evaluate(); // WCS 2018.01.09 add to get latest & correct value to report(s)
   return mRmat66Value.ToString();
}

//------------------------------------------------------------------------------
// const Rmatrix& GetRmatrix() const
//------------------------------------------------------------------------------
/**
 * @return Rmatrix value of parameter without evaluating.
 */
//------------------------------------------------------------------------------
const Rmatrix& Rmat66Var::GetRmatrix() const
{
   MessageInterface::ShowMessage("Rmat66Var::GetRmatrix() entered\n");
   return mRmat66Value;
}

//------------------------------------------------------------------------------
// virtual void SetRmatrix(const Rmatrix &val)
//------------------------------------------------------------------------------
void Rmat66Var::SetRmatrix(const Rmatrix &val)
{
   //@todo check the size of the input matrix
   mRmat66Value = val;
}

//------------------------------------------------------------------------------
// const Rmatrix& EvaluateRmatrix()
//------------------------------------------------------------------------------
/**
 * Evaluates Real value of parameter. The derived class should implement this
 * method.
 */
//------------------------------------------------------------------------------
const Rmatrix& Rmat66Var::EvaluateRmatrix()
{
   MessageInterface::ShowMessage("Rmat66Var::EvaluateRmatrix() entered\n");
   if (mKey == GmatParam::SYSTEM_PARAM)
   {
      throw ParameterException("Parameter: EvaluateRmatrix() should be implemented "
                               "for Parameter Type:" + GetTypeName());
   }
   else
   {
      return mRmat66Value;
   }
}

