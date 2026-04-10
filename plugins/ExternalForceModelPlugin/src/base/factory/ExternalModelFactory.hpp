//$Id: ExternalModelFactory.hpp 9461 2012-08-17 16:19:15Z acampbell $
//------------------------------------------------------------------------------
//                            ExternalModelFactory
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
// Developed jointly by NASA/GSFC and Emergent Space Technologies, Inc. 
//
// Author: Alex Campbell
// Created: 07/25/2022
//
/**
 *  Declaration code for the ExternalModelFactory class.
 */
//------------------------------------------------------------------------------
#ifndef ExternalModelFactory_hpp
#define ExternalModelFactory_hpp

#include "externalmodel_defs.hpp"
#include "Factory.hpp"
#include "PhysicalModel.hpp"
#include "ExternalModel.hpp"

class EXTERNALMODEL_API ExternalModelFactory : public Factory
{
public:
   virtual GmatBase* CreateObject(const std::string &ofType,
                                const std::string &withName);
   virtual PhysicalModel* CreatePhysicalModel(const std::string &ofType,
                                const std::string &withName);
   
   // default constructor
   ExternalModelFactory();
   // constructor
   ExternalModelFactory(StringArray createList);
   // copy constructor
   ExternalModelFactory(const ExternalModelFactory& fact);
   // assignment operator
   ExternalModelFactory& operator=(const ExternalModelFactory& fact);
   
   virtual ~ExternalModelFactory();
   
};

#endif // ExternalModelFactory_hpp
