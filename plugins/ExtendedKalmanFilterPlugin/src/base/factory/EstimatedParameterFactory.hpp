//$Id$
//------------------------------------------------------------------------------
//                         EstimatedParameterFactory
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. under
// contract number NNG15CR67C
//
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2020/02/04
//
/**
*  This class is the factory class for EstimatedParameter.
 */
//------------------------------------------------------------------------------
#ifndef EstimatedParameterFactory_hpp
#define EstimatedParameterFactory_hpp

#include "kalman_defs.hpp"
#include "Factory.hpp"
#include "EstimatedParameter.hpp"

class KALMAN_API EstimatedParameterFactory : public Factory
{
public:
   GmatBase*   CreateObject(const std::string &ofType,
                            const std::string &withName = "");
   EstimatedParameter*  CreateEstimatedParameter(const std::string &ofType,
                                                 const std::string &withName = "");

   // default constructor
   EstimatedParameterFactory();
   // constructor
   EstimatedParameterFactory(StringArray createList);
   // copy constructor
   EstimatedParameterFactory(const EstimatedParameterFactory &fact);
   // assignment operator
   EstimatedParameterFactory& operator= (const EstimatedParameterFactory &fact);

   // destructor
   ~EstimatedParameterFactory();

protected:
      // protected data

private:
      // private data


};

#endif // EstimatedParameterFactory_hpp

