//$Id$
//------------------------------------------------------------------------------
//                            ProcessNoiseFactory
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
// Created: 2018/09/19
//
/**
 *  This class is the factory class for Process Noise.  
 */
//------------------------------------------------------------------------------
#ifndef ProcessNoiseFactory_hpp
#define ProcessNoiseFactory_hpp

#include "kalman_defs.hpp"
#include "Factory.hpp"
#include "ProcessNoiseBase.hpp"
#include "LinearProcessNoise.hpp"

class KALMAN_API ProcessNoiseFactory : public Factory
{
public:
   GmatBase*    CreateObject(const std::string &ofType,
                             const std::string &withName = "");
   ProcessNoiseBase*  CreateProcessNoise(const std::string &ofType,
                             const std::string &withName = "");

   // default constructor
   ProcessNoiseFactory();
   // constructor
   ProcessNoiseFactory(StringArray createList);
   // copy constructor
   ProcessNoiseFactory(const ProcessNoiseFactory &fact);
   // assignment operator
   ProcessNoiseFactory& operator= (const ProcessNoiseFactory &fact);

   // destructor
   ~ProcessNoiseFactory();

protected:
   // protected data

private:
   // private data


};

#endif // ProcessNoiseFactory_hpp

