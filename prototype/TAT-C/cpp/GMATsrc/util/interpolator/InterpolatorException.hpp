//$Id$
//------------------------------------------------------------------------------
//                               InterpolatorException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number #####
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2009/09/28
//
//------------------------------------------------------------------------------

#ifndef InterpolatorException_hpp
#define InterpolatorException_hpp

#include "BaseException.hpp"

/**
 * Exceptions thrown by the Interpolators
 */
class GMATUTIL_API InterpolatorException : public BaseException
{
public:
   // class constructor
   InterpolatorException(std::string details = "");
   // class destructor
   ~InterpolatorException();
};

#endif // InterpolatorException_hpp

