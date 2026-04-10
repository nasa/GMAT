//$Id$
//------------------------------------------------------------------------------
//                               FunctionException
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
// Author: Allison Greene
// Created: 2004/9/22
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the Function hierarchy.
 */
//------------------------------------------------------------------------------


#include "FunctionException.hpp"


//------------------------------------------------------------------------------
//  FunctionException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs FunctionException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
FunctionException::FunctionException(std::string details) :
    BaseException           ("Function Exception Thrown: ", details)
{
}


//------------------------------------------------------------------------------
//  ~FunctionException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
FunctionException::~FunctionException()
{
}


//------------------------------------------------------------------------------
//  FunctionException(const FunctionException &ce)
//------------------------------------------------------------------------------
/**
 * Constructs FunctionException instance (copy constructor).
 */
//------------------------------------------------------------------------------
FunctionException::FunctionException(const FunctionException &be) :
    BaseException       (be)
{
}
