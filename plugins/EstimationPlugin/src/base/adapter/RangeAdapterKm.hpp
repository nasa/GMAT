//------------------------------------------------------------------------------
//                           RangeAdapterKm
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 12, 2014
/**
 * A measurement adapter for ranges in Km
 */
//------------------------------------------------------------------------------

#ifndef RangeAdapterKm_hpp
#define RangeAdapterKm_hpp

#include "TrackingDataAdapter.hpp"


/**
 * A measurement adapter for ranges in Km
 */
class ESTIMATION_API RangeAdapterKm: public TrackingDataAdapter
{
public:
   RangeAdapterKm(const std::string& name);
   virtual ~RangeAdapterKm();
   RangeAdapterKm(const RangeAdapterKm& rak);
   RangeAdapterKm&      operator=(const RangeAdapterKm& rak);

   virtual GmatBase*    Clone() const;

   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         SetMeasurement(MeasureModel* meas);

   virtual bool         Initialize();

   // Preserve interfaces in the older measurement model code
   virtual const MeasurementData&
                        CalculateMeasurement(bool withEvents = false,
                              ObservationData* forObservation = NULL,
                              std::vector<RampTableData>* rampTB = NULL,
                              bool forSimulation = false);
   
   virtual bool         ReCalculateFrequencyAndMediaCorrection(UnsignedInt pathIndex, 
                              Real uplinkFrequency, 
                              std::vector<RampTableData>* rampTB);

   virtual const MeasurementData&
                        CalculateMeasurementAtOffset(bool withEvents = false,
                              Real dt = 0.0, ObservationData* forObservation = NULL,
                              std::vector<RampTableData>* rampTB = NULL, 
                              bool forSimulation = false);
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

   virtual Real         ApplyMultiplier(const std::string& useMeasType,
         const Real factor, const GmatBase* obj);

   Real                 GetIonoCorrection();
   Real                 GetTropoCorrection();

   void                 SetForTdrs();

   DEFAULT_TO_NO_CLONES
protected:
   /// Parameter IDs for the RangeAdapterKm
   enum
   {
      RangeAdapterKmParamCount = AdapterParamCount,
   };

   static const bool USE_TAYLOR_SERIES;
   static const bool USE_CHEBYSHEV_DIFFERENCE;
   Real PathMagnitudeDelta(Rvector3 pathVec, Rvector3 delta);
   
   const Real CalculatePilotFrequency(const Real effFreq, const Integer node4FreqBand, const std::string serviceAccess, const std::string tdrsId, const Integer smarId, const Integer dataFlag);
   const Real CalculatePilotFrequencyDerivativeWRTEffectiveFrequency(const Real effFreq, const Integer node4FreqBand, const std::string serviceAccess, const std::string tdrsId, const Integer smarId, const Integer datFlag);

   
   std::string TDRSId; ///Id of a tdrs spacecraft. Used for all TDRS adapters
   bool warnTdrsId = true;
   bool GetTdrsIdFromSpacecraftId(std::string scId);
};

#endif /* RangeAdapterKm_hpp */
