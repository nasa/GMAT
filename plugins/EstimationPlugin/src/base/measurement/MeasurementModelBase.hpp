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

#ifndef MeasurementModelBase_hpp
#define MeasurementModelBase_hpp

#include "estimation_defs.hpp"
#include "EstimationDefs.hpp"
#include "GmatBase.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton

/**
 * Base class for measurement models and tracking file sets so the Factories 
 * can manage them as the same core type
 */
class ESTIMATION_API MeasurementModelBase : public GmatBase
{
public:
   MeasurementModelBase(const std::string &nomme,
         const std::string& typeName);
   virtual ~MeasurementModelBase();
   MeasurementModelBase(const MeasurementModelBase &mm);
   MeasurementModelBase& operator=(const MeasurementModelBase &mm);

protected:
   enum
   {
      MeasurementModelBaseParamCount = GmatBaseParamCount,
   };

   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;

   Integer GetParmIdFromEstID(Integer id, GmatBase *obj);
};

#endif /* MeasurementModelBase_hpp */
