//------------------------------------------------------------------------------
//                          MeasurementModelBase
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/06/24
//
/**
 * MeasurementModelBase declaration used in GMAT's estimator and simulator
 * factory code
 */
//------------------------------------------------------------------------------

#include "MeasurementModelBase.hpp"


MeasurementModelBase::MeasurementModelBase(const std::string& name,
      const std::string& typeName) :
   GmatBase             (Gmat::MEASUREMENT_MODEL, typeName, name)
{
   theTimeConverter = TimeSystemConverter::Instance();
}

MeasurementModelBase::~MeasurementModelBase()
{
}

MeasurementModelBase::MeasurementModelBase(const MeasurementModelBase& mm) :
   GmatBase             (mm)
{
   theTimeConverter = TimeSystemConverter::Instance();
}

MeasurementModelBase& MeasurementModelBase::operator=(
      const MeasurementModelBase& mm)
{
   if (this != &mm)
      GmatBase::operator=(mm);
   return *this;
}

Integer MeasurementModelBase::GetParmIdFromEstID(Integer id, GmatBase* obj)
{
   return id - obj->GetType() * 250;
}
