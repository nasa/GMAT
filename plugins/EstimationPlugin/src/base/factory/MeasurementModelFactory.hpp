//------------------------------------------------------------------------------
//                         MeasurementModelFactory
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/06/18
//
/**
 * Factory used to create the MeasurementModel objects
 */
//------------------------------------------------------------------------------

#ifndef MeasurementModelFactory_hpp
#define MeasurementModelFactory_hpp


#include "estimation_defs.hpp"
#include "Factory.hpp"


class ESTIMATION_API MeasurementModelFactory : public Factory
{
public:
   MeasurementModelFactory();
   virtual ~MeasurementModelFactory();

   MeasurementModelFactory(StringArray createList);
   MeasurementModelFactory(const MeasurementModelFactory& fact);
   MeasurementModelFactory& operator= (const MeasurementModelFactory& fact);

   GmatBase* CreateObject(const std::string &ofType,
         const std::string &withName = "");
   MeasurementModelBase* CreateMeasurementModel(const std::string &ofType,
         const std::string &withName = "");
};

#endif /* MeasurementModelFactory_hpp */
