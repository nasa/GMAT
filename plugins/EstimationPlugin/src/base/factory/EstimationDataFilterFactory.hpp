//------------------------------------------------------------------------------
//                         EstimationDataFilterFactory
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
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2015/03/03
//
/**
 * Factory used to create EstimationDataFilter objects.
 */
//------------------------------------------------------------------------------


#ifndef EstimationDataFilterFactory_hpp
#define EstimationDataFilterFactory_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"


class ESTIMATION_API EstimationDataFilterFactory : public Factory
{
public:
   EstimationDataFilterFactory();
   virtual ~EstimationDataFilterFactory();

   EstimationDataFilterFactory(StringArray createList);
   EstimationDataFilterFactory(const EstimationDataFilterFactory& fact);
   EstimationDataFilterFactory& operator= (const EstimationDataFilterFactory& fact);

   virtual GmatBase* CreateObject(const std::string &ofType,
         const std::string &withName = "");

   virtual DataFilter *CreateDataFilter(const std::string &ofType,
         const std::string &withName = "");
};

#endif /* EstimationDataFilterFactory_hpp */
