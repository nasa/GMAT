//$Id: GeometricRange.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         GeometricRange
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
// Created: 2009/06/29
//
/**
 * Definition of the geometric range measurement.
 */
//------------------------------------------------------------------------------


#ifndef GeometricRange_hpp
#define GeometricRange_hpp

#include "geometricmeasurement_defs.hpp"
#include "CoreMeasurement.hpp"

/**
 * A basic, simple range measurement
 *
 * The GeometricRange Measurement evaluates the instantaneous displacement
 * between two participants, taking into account a potentially obscuring body
 * between them.  This measurement does not evaluate light transit time between
 * the participants, nor does it include any corrections to the transit path.
 */
class GEOMETRICMEAS_API GeometricRange: public CoreMeasurement
{
public:
   GeometricRange(const std::string &name = "");
   virtual ~GeometricRange();
   GeometricRange(const GeometricRange &rm);
   GeometricRange& operator=(const GeometricRange &rm);

   virtual GmatBase*       Clone() const;
   virtual bool            Initialize();
   virtual const std::vector<RealArray>&
                           CalculateMeasurementDerivatives(GmatBase *obj,
                                 Integer id);

protected:
   bool                    Evaluate(bool withEvents = false);
};

#endif /* GeometricRange_hpp */
