//$Id$
//------------------------------------------------------------------------------
//                              AtmosphereException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2004/02/29
//
/**
 * This class provides an exception class for the AtmosphereModel classes
 */
//------------------------------------------------------------------------------
#ifndef AtmosphereException_hpp
#define AtmosphereException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp"

class GMAT_API AtmosphereException : public BaseException
{
public:
   //---------------------------------------------------------------------------
   //  AtmosphereException(const std::string &details)
   //---------------------------------------------------------------------------
   /**
    * Constructs an AtmosphereException object (default constructor).
    *
    * @param <details> Detailed message about why the exception was thrown.
    */
   //---------------------------------------------------------------------------
   AtmosphereException(const std::string &details) :
      BaseException  ("Atmosphere model exception: ", details)
   {
   }

protected:

private:
};
#endif // AtmosphereException_hpp
