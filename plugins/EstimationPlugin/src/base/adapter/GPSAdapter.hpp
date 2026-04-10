//------------------------------------------------------------------------------
//                           GPSAdapter
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
// Author: Tuan Dang Nguyen, GSFC/NASA
// Created: Aug 24, 2016
/**
 * A measurement adapter for GPS position vector in Km
 */
//------------------------------------------------------------------------------

#ifndef GPSAdapter_hpp
#define GPSAdapter_hpp

#include "TrackingDataAdapter.hpp"

#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"


/**
 * A measurement adapter for position vector in Km
 */
class ESTIMATION_API GPSAdapter: public TrackingDataAdapter
{
public:
   GPSAdapter(const std::string& name);
   virtual ~GPSAdapter();
   GPSAdapter(const GPSAdapter& gps);
   GPSAdapter&      operator=(const GPSAdapter& gps);

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

   Real                 GetIonoCorrection();
   Real                 GetTropoCorrection();

   
   bool                 SetGPSReceiverName(const std::string name) { gpsReceiverName = name; return true; };
   std::string          GetGPSReceiverName() { return gpsReceiverName; };


   DEFAULT_TO_NO_CLONES
protected:
   /// Parameter IDs for the GPSAdapter
   enum
   {
      GPSAdapterParamCount = AdapterParamCount,
   };

   CoordinateSystem *ecf;
   CoordinateSystem *ej2k;
   CoordinateConverter *cv;

   std::string gpsReceiverName;


private:
   Rvector3 ConvertToEFCVector(const Rvector3 pos, const GmatEpoch epoch) const;

};

#endif /* GPSAdapter_hpp */
