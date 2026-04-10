//------------------------------------------------------------------------------
//                           RangeRateAdapterKps
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
// Author: Michael Barrucco, Thinking Systems, Inc.
// Created: Oct 2nd, 2014
/**
 * A measurement adapter for ranges in Km
 */
//------------------------------------------------------------------------------

#ifndef RangeRateAdapterKps_hpp
#define RangeRateAdapterKps_hpp

#include "RangeAdapterKm.hpp"
#include "SpaceObject.hpp"

/**
 * A measurement adapter for ranges in Km
 */
class ESTIMATION_API RangeRateAdapterKps: public RangeAdapterKm
{
public:
   RangeRateAdapterKps(const std::string& name);
   virtual ~RangeRateAdapterKps();
   RangeRateAdapterKps(const RangeRateAdapterKps& rr);
   RangeRateAdapterKps&      operator=(const RangeRateAdapterKps& rr);

   virtual GmatBase*    Clone() const;

   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);

   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name,
                                     const Integer index);

   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   virtual bool         Initialize();

   // Preserve interfaces in the older measurement model code
   virtual const MeasurementData&
                        CalculateMeasurement(bool withEvents = false,
                              ObservationData* forObservation = NULL,
                              std::vector<RampTableData>* rampTB = NULL,
                              bool forSimulation = false);

   virtual const MeasurementData&
                        CalculateMeasurementAtOffset(bool withEvents,
                              Real dt, ObservationData* forObservation,
                              std::vector<RampTableData>* rampTB,
                              Integer forStrand,
                              bool forSimulation);

   virtual const std::vector<RealArray>&
                        CalculateMeasurementDerivatives(GmatBase *obj,
                              Integer id);
   virtual bool         WriteMeasurements();
   virtual bool         WriteMeasurement(Integer id);

   // Covariance handling code
   virtual Integer      HasParameterCovariances(Integer parameterId);

   virtual Integer      GetEventCount();
   virtual void         SetCorrection(const std::string& correctionName,
         const std::string& correctionType);

   DEFAULT_TO_NO_CLONES
protected:
   /// Doppler interval
   Real dopplerInterval;

   /// Data from current epoch pass in the strand
   MeasurementData cMeasurement1;
   /// Data from forward offset pass in the strand
   MeasurementData cMeasurement2;

   /// Parameter IDs for the RangeRateAdapterKps
   enum
   {
      DOPPLER_INTERVAL = RangeAdapterKmParamCount,
      RangeRateAdapterParamCount,
   };
      /// Strings describing the DSNRangeAdapter parameters
   static const std::string PARAMETER_TEXT[RangeRateAdapterParamCount -
                                           RangeAdapterKmParamCount];   
   /// Types of the RangeRateAdapter parameters
   static const Gmat::ParameterType PARAMETER_TYPE[RangeRateAdapterParamCount -
                                                   RangeAdapterKmParamCount]; 

};

#endif /* RangeRateAdapterKps_hpp */
