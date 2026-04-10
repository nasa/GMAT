//$Id$
//------------------------------------------------------------------------------
//                              InterfaceException
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
// Author: Wendy Shoan/GSFC
// Created: 2006.09.15
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Exception class used by the Interface classes.
 */
//------------------------------------------------------------------------------


#include "InterfaceException.hpp" // class's header file

//------------------------------------------------------------------------------
//  InterfaceException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs InterfaceException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
InterfaceException::InterfaceException(const std::string &details) :
    BaseException           ("Interface Exception Thrown: ", details)
{
}


//------------------------------------------------------------------------------
//  ~InterfaceException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
InterfaceException::~InterfaceException()
{
}


//------------------------------------------------------------------------------
//  InterfaceException(const InterfaceException &ie)
//------------------------------------------------------------------------------
/**
 * Constructs InterfaceException instance (copy constructor). 
 *
 * @param ie Instance that gets copied.
 */
//------------------------------------------------------------------------------
InterfaceException::InterfaceException(const InterfaceException &ie) :
    BaseException       (ie)
{}

