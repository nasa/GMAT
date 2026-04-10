//------------------------------------------------------------------------------
//                           TDRSDopplerAdapter
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
// contract
//
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: August 13, 2015
/**
 * A measurement adapter for TDRS Doppler
 */
//------------------------------------------------------------------------------

#ifndef TDRSDopplerAdapter_hpp
#define TDRSDopplerAdapter_hpp

#include "RangeAdapterKm.hpp"


/**
 * A measurement adapter for TDRS Doppler
 */
class ESTIMATION_API TDRSDopplerAdapter: public RangeAdapterKm
{
public:
   TDRSDopplerAdapter(const std::string& name);
   virtual ~TDRSDopplerAdapter();
   TDRSDopplerAdapter(const TDRSDopplerAdapter& da);
   TDRSDopplerAdapter& operator=(const TDRSDopplerAdapter& da);

   virtual void SetSolarSystem(SolarSystem *ss);

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

   virtual bool         SetStringParameter(const Integer id, const std::string& value);
   virtual bool         SetStringParameter(const Integer id, const std::string &value, const Integer index);
   virtual std::string  GetStringParameter(const Integer id, const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label, const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, const std::string &value, const Integer index);
   virtual std::string  GetStringParameter(const std::string &label, const Integer index) const;
   
   virtual Integer      SetIntegerParameter(const Integer id, const Integer value);
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const std::string &label, const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;

   virtual bool         SetBooleanParameter(const Integer id, const bool value);
   virtual bool         SetBooleanParameter(const std::string &label, const bool value);

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         SetRefObject(GmatBase* obj,
                                     const UnsignedInt type,
                                     const std::string& name);
   virtual bool         SetRefObject(GmatBase* obj,
                                     const UnsignedInt type,
                                     const std::string& name, 
                                     const Integer index);

   virtual bool         SetMeasurement(MeasureModel* meas);
   virtual void         SetPropagators(std::vector<PropSetup*> *ps,
                           std::map<std::string, StringArray> *spMap);

   virtual void         SetCorrection(const std::string& correctionName,
                                      const std::string& correctionType);


   virtual bool         Initialize();
   virtual void         SetTransientForces(std::vector<PhysicalModel*> *tf);

   // Preserve interfaces in the older measurement model code
   virtual const MeasurementData&
                        CalculateMeasurement(bool withEvents = false,
                              ObservationData* forObservation = NULL,
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


   /// TDRS has 2 measurement paths: Start path and End path. Each measurement path has 2 signal strands: 
   /// long strand (GS -> TDRS -> Sat -> TDRS -> GS) and short strand (GS -> TDRS -> GS). Both long and 
   /// short strands have the same measurement time at ground station and received time at TDRS on downlink leg.
   RangeAdapterKm* adapterSL;              // Start-Long path
   RangeAdapterKm* adapterSS;              // Start-Short path
   RangeAdapterKm* adapterES;              // End-Short path
   // Note that: "this" is used for End-Long path

   DEFAULT_TO_NO_CLONES

protected:
   /// Doppler count interval
   Real                 dopplerCountInterval;              // unit: second

   /// Service access list. It contains a list of all posible services which provide for the measurement
   /// This parameter is used for simulation only.
   StringArray          serviceAccessList;    // a list containing string "SA1", "SA2", or "MA"

   /// Multiplier for Start-Long path, Start-Short path, End-Long path, and End-Short path
   Real                 multiplierSL;
   Real                 multiplierSS;
   Real                 multiplierEL;
   Real                 multiplierES;

   /// TDRS node 4 frequency
   Real                 node4Freq;            // unit: MHz
   /// TDRS node 4 frequency band
   Integer              node4FreqBand;        // 0: unspecified, 1: S-band, 2: X-band, 3: K-band
   /// SMAR Id
   Integer              smarId;               // value of SMAR id should be 0 to 31
   /// TDRS generation
   Integer              dataFlag;             // value of TDRS data flag should be 0 or 1

   /// Parameter IDs for the DopplerAdapter
   enum
   {
      DOPPLER_COUNT_INTERVAL = RangeAdapterKmParamCount,
      SERVICE_ACCESS,
      NODE4_FREQUENCY,
      NODE4_BAND,
      SMAR_ID,
      DATA_FLAG,
      TDRSDopplerAdapterParamCount,
   };

   /// Strings describing the TDRSDopplerAdapter parameters
   static const std::string PARAMETER_TEXT[TDRSDopplerAdapterParamCount -
                                           RangeAdapterKmParamCount];
   /// Types of the TDRSDopplerAdapter parameters
   static const Gmat::ParameterType PARAMETER_TYPE[TDRSDopplerAdapterParamCount -
                                                   RangeAdapterKmParamCount];

   Real               GetTurnAroundRatio(Integer freqBand);

private:
   /// MeasurementData for Start-Long path, Start-Short path, End-Long path, and End-Short path
   MeasurementData measDataSL;
   MeasurementData measDataSS;
   MeasurementData measDataEL;
   MeasurementData measDataES;

   /// service access index
   UnsignedInt    serviceAccessIndex;   // serviceAccessList[serviceAccessIndex] specifies the current service access used for TRDSDoppler measurement

};

#endif /* TDRSDopplerAdapter_hpp */
