//$Id: NRLMsise00Factory.hpp 9490 2011-08-24 16:28:40Z tdnguye2 $
//------------------------------------------------------------------------------
//                            NRLMsise00Factory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Nguyen
// Created: 2011/6/24
//
/**
 *  Declaration code for the NRLMsise00Factory class.
 */
//------------------------------------------------------------------------------
#ifndef NRLMSISE00Factory_hpp
#define NRLMSISE00Factory_hpp


#include "nrlmsise00_defs.hpp"
#include "Factory.hpp"
#include "AtmosphereModel.hpp"

class NRLMSISE00_API NRLMsise00Factory : public Factory
{
public:
   GmatBase* CreateObject(const std::string &ofType,
                          const std::string &withName = "");
   AtmosphereModel* CreateAtmosphereModel(const std::string &ofType,
                                          const std::string &withName = "");
   
   // default constructor
   NRLMsise00Factory();
   // constructor
   NRLMsise00Factory(StringArray createList);
   // copy constructor
   NRLMsise00Factory(const NRLMsise00Factory& fact);
   // assignment operator
   NRLMsise00Factory& operator=(const NRLMsise00Factory& fact);
   
   virtual ~NRLMsise00Factory();
   
   virtual StringArray GetListOfCreatableObjects(const std::string& qualifier);
};

#endif // NRLMSISE00Factory_hpp
