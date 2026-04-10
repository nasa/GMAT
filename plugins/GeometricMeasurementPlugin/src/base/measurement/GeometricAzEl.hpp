//$Id: GeometricAzEl.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         GeometricAzEl
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
// Author: Darrel J. Conway, Thinking Systems, Inc./ Wendy Shoan, GSFC/GSSB
// Created: 2009.08.11
//
/**
 * Definition of the geometric azimuth/elevation measurement.
 */
//------------------------------------------------------------------------------


#ifndef GeometricAzEl_hpp
#define GeometricAzEl_hpp

#include "geometricmeasurement_defs.hpp"
#include "CoreMeasurement.hpp"

/**
 * Class for instantaneous Azimuth/Elevation measurements
 */
class GEOMETRICMEAS_API GeometricAzEl: public CoreMeasurement
{
public:
   GeometricAzEl(const std::string &name = "");
   virtual ~GeometricAzEl();
   GeometricAzEl(const GeometricAzEl &aem);
   GeometricAzEl& operator=(const GeometricAzEl &aem);

   virtual GmatBase*       Clone() const;
   virtual bool            Initialize();
   virtual const std::vector<RealArray>&  
                           CalculateMeasurementDerivatives(GmatBase *obj, Integer id);

protected:
   bool                    Evaluate(bool withEvents = false);
};

#endif /* GeometricAzEl_hpp */
