//$Id$
//------------------------------------------------------------------------------
//                            UtilityException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/04/19
//
/**
 * Declares utility exception.
 */
//------------------------------------------------------------------------------
#ifndef UtilityException_hpp
#define UtilityException_hpp

#include "BaseException.hpp"

// General utility exception
class GMATUTIL_API UtilityException : public BaseException
{
public:
   UtilityException(const std::string& details = "") 
      : BaseException("Utility Exception: ", details) {};
};


// Gravity File exception
class GMATUTIL_API GravityFileException : public BaseException
{
public:
   GravityFileException(const std::string& details = "") 
      : BaseException("Gravity File Exception: ", details) {};
};


// Time utility exception
class GMATUTIL_API TimeException : public BaseException
{
public:
   TimeException(const std::string& details = "") 
      : BaseException("Time Exception: ", details) {};
};
#endif
