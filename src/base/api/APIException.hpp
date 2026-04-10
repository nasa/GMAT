//$Id$
//------------------------------------------------------------------------------
//                              GmatException
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
// Author: John McGreevy
// Created: 2019/5/9
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class to be wrapped by SWIG for the GMAT API.
 */
//------------------------------------------------------------------------------


#ifndef APIException_hpp
#define APIException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp"

class GMAT_API APIException : public BaseException
{
public:

   APIException(const std::string &details = "");
   virtual ~APIException();
   APIException(const APIException &ae);
};

#endif // APIException_hpp
