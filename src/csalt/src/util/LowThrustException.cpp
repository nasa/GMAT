//------------------------------------------------------------------------------
//                               LowThrustException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Wendy Shoan
// Created: 2015.06.22
//
/**
 * Exception class used by the Low Thrust subsystem.
 */
//------------------------------------------------------------------------------

#include "LowThrustException.hpp"

//------------------------------------------------------------------------------
//  LowThrustException(const std::string &details)
//------------------------------------------------------------------------------
/**
 * Constructs LowThrustException instance (default constructor).
 *
 * @param <details> A message providing the details of the exception.
 */
//------------------------------------------------------------------------------
LowThrustException::LowThrustException(const std::string &details) :
   BaseException("LowThrust Exception Thrown: ", details)
{
}

//------------------------------------------------------------------------------
//  LowThrustException(const LowThrustException &be)
//------------------------------------------------------------------------------
/**
 * Constructs LowThrustException instance (copy constructor).
 *
 * @param be The instance that is copied.
 */
//------------------------------------------------------------------------------
LowThrustException::LowThrustException(const LowThrustException &be) :
   BaseException(be)
{
}
