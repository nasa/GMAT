//$Id: EventException.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             EventException
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
// Author: Darrel J. Conway
// Created: 2009/08/11
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Exception class used by the Event hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef EventException_hpp
#define EventException_hpp

#include "estimation_defs.hpp"
#include "BaseException.hpp" // inheriting class's header file


class ESTIMATION_API EventException : public BaseException
{
public:
   // class constructor
   EventException(const std::string &details = "");
   // class destructor
   ~EventException();
   // Copy constructor
   EventException(const EventException &ce);
};

#endif // EventException_hpp

