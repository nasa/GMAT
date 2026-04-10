//------------------------------------------------------------------------------
//                         DataFileAdapter
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS
// Task 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 12, 2010
//
/**
 * File description here.
 */
//------------------------------------------------------------------------------


#include "DataFileAdapter.hpp"

DataFileAdapter::DataFileAdapter()
{
   // TODO Auto-generated constructor stub
}


DataFileAdapter::~DataFileAdapter()
{
   // TODO Auto-generated destructor stub
}


DataFileAdapter::DataFileAdapter(const DataFileAdapter & dfa)
{
}


DataFileAdapter& DataFileAdapter::operator =(const DataFileAdapter & dfa)
{
   if (this != &dfa)
   {

   }

   return *this;
}


ObType* DataFileAdapter::GetObTypeObject(DataFile *forFile)
{
   return NULL;
}


bool DataFileAdapter::LoadObservation(ObType & data, ObservationData & target)
{
   return false;
}


bool DataFileAdapter::LoadMeasurement(MeasurementData & data, ObType & target)
{
   return false;
}
