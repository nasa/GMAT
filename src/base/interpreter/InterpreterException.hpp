//$Id$
//------------------------------------------------------------------------------
//                                  ClassName
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
// Author: Darrel J. Conway
// Created: 2003/11/04
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Insert descriptive text here.
 *
 * @note Any notes here.
 */
//------------------------------------------------------------------------------

#ifndef InterpreterException_hpp
#define InterpreterException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown by the interpreters
 */
class GMAT_API InterpreterException : public BaseException
{
public:
   // class constructor
   InterpreterException(const std::string &details = "",
         // Change to this when the repeated instances are fixed:
//         Gmat::MessageType mt = Gmat::ERROR_);
         // Change to this if it's problematic:
         Gmat::MessageType mt = Gmat::GENERAL_);

   // class destructor
   ~InterpreterException();
};

#endif // InterpreterException_hpp

