//$Id$
//------------------------------------------------------------------------------
//                               BurnException
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
// Created: 2004/1/13
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the Burn hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef BurnException_hpp
#define BurnException_hpp

#include "BaseException.hpp"
#include "gmatdefs.hpp"          // For GMAT_API

class GMAT_API BurnException : public BaseException
{
public:
   BurnException(const std::string &details = "");
   virtual ~BurnException();
   BurnException(const BurnException &be);
};

#endif // BurnException_hpp
