//------------------------------------------------------------------------------
//                           SinglePointSignal
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 9, 2014
/**
 * The intermediate class used for SignalBase objects that include light time
 * solutions.
 */
//------------------------------------------------------------------------------

#include "SinglePointSignal.hpp"


//------------------------------------------------------------------------------
// SinglePointSignal(const std::string &typeStr, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param typeStr The object type
 * @param name Name of the new object
 */
//------------------------------------------------------------------------------
SinglePointSignal::SinglePointSignal(const std::string &typeStr, const std::string &name):
   SignalBase              (typeStr, name)
{
}


//------------------------------------------------------------------------------
// ~SinglePointSignal()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SinglePointSignal::~SinglePointSignal()
{
}


//------------------------------------------------------------------------------
// SinglePointSignal(const SinglePointSignal& ps) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ps The SinglePointSignal copied to make a new one
 */
//------------------------------------------------------------------------------
SinglePointSignal::SinglePointSignal(const SinglePointSignal& ps) :
   SignalBase              (ps)
{
}


//------------------------------------------------------------------------------
// SinglePointSignal& operator=(const SinglePointSignal& ps)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ps The SinglePointSignal copied into this one
 *
 * @return This SinglePointSignal, reconfigured to match ps
 */
//------------------------------------------------------------------------------
SinglePointSignal& SinglePointSignal::operator=(const SinglePointSignal& ps)
{
   if (this != &ps)
   {
      GmatBase::operator=(ps);
   }

   return *this;
}
