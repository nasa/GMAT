//------------------------------------------------------------------------------
//                           ElevationAdapter
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: 
// Created: Aug 31, 2018
/**
* A measurement adapter for elevation measurements
*/
//------------------------------------------------------------------------------

#ifndef ElevationAdapter_hpp
#define ElevationAdapter_hpp

#include "AngleAdapterDeg.hpp"


/**
* A measurement adapter for elevation measurements
*/
class ESTIMATION_API ElevationAdapter : public AngleAdapterDeg
{
public:
   ElevationAdapter(const std::string& name);
   virtual ~ElevationAdapter();
   ElevationAdapter(const ElevationAdapter& rak);
   ElevationAdapter&      operator=(const ElevationAdapter& rak);

   virtual GmatBase*    Clone() const;

   //virtual const MeasurementData&
   //   CalculateMeasurement(bool withEvents = false,
   //   ObservationData* forObservation = NULL,
   //   std::vector<RampTableData>* rampTB = NULL,
   //   bool forSimulation = false);

   virtual Real         CalcMeasValue();
   virtual Rvector6     CalcDerivValue();
};

#endif /* ElevationAdapter_hpp */
