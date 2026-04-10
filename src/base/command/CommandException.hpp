//$Id$
//------------------------------------------------------------------------------
//                                  CommandException
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
// Created: 2003/10/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Exception class used by the Command hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef CommandException_hpp
#define CommandException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown from the command subsystem
 */
class GMAT_API CommandException : public BaseException
{
public:
   // class constructor
   CommandException(const std::string &details = "",
         // Change to this when the repeated strings are fixed:
//         Gmat::MessageType mt = Gmat::ERROR_);
         // Change to this if it's problematic:
         Gmat::MessageType mt = Gmat::GENERAL_);
   // class destructor
   ~CommandException();
   // Copy constructor
   CommandException(const CommandException &ce);
};

#endif // CommandException_hpp

