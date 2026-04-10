//------------------------------------------------------------------------------
//                           TDRSDOWDAdapter
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
// Created: Jan 4, 2021
/**
 * A measurement adapter for TDRS DOWD (unit: Hz)
 */
//------------------------------------------------------------------------------

#ifndef TDRSDOWDAdapter_hpp
#define TDRSDOWDAdapter_hpp

#include "TDRS3LReturnDopplerAdapter.hpp"


/**
 * A measurement adapter for TDRS DOWD
 */
class ESTIMATION_API TDRSDOWDAdapter: public TrackingDataAdapter
{
public:
   TDRSDOWDAdapter(const std::string& name);
   virtual ~TDRSDOWDAdapter();
   TDRSDOWDAdapter(const TDRSDOWDAdapter& da);
   TDRSDOWDAdapter& operator=(const TDRSDOWDAdapter& da);

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

   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string& value);
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value, 
                                           const Integer index);
   virtual std::string  GetStringParameter(const Integer id, 
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value, 
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label, 
                                           const Integer index) const;
   
   virtual Integer      SetIntegerParameter(const Integer id, 
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const std::string &label, 
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;

   virtual bool         SetBooleanParameter(const Integer id, 
                                            const bool value);
   virtual bool         SetBooleanParameter(const std::string &label, 
                                            const bool value);

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

   TDRS3LReturnDopplerAdapter* adapterTDRSComp;
   TDRS3LReturnDopplerAdapter* adapterTDRSRef;

   DEFAULT_TO_NO_CLONES

protected:
   /// Doppler count interval
   Real                 dopplerCountIntervalTDRSComp;      // unit: second
   Real                 dopplerCountIntervalTDRSRef;      // unit: second

   /// Service access list. It contains a list of all posible services which
   /// provide for the measurement.
   /// This parameter is used for simulation only.
   StringArray          serviceAccessListTDRSComp;
   StringArray          serviceAccessListTDRSRef;  // a list containing string 
                                                   // "SA1", "SA2", or "MA"
   /// TDRS node 4 frequency
   Real                 node4FreqTDRSComp;        // unit: MHz
   Real                 node4FreqTDRSRef;        // unit: MHz

   /// TDRS node 4 frequency band
   Integer              node4FreqBandTDRSComp;
   Integer              node4FreqBandTDRSRef;
                                          // 0: unspecified, 1: S-band, 
                                          // 2: X-band, 3: K-band
   /// SMAR Id
   Integer              smarIdTDRSComp;   // value of SMAR id should be 0 to 31
   Integer              smarIdTDRSRef;    // value of SMAR id should be 0 to 31

   /// TDRS generation
   Integer              dataFlagTDRSComp; // value of TDRS data flag should be 0 or 1
   Integer              dataFlagTDRSRef;  // value of TDRS data flag should be 0 or 1

   /// Parameter IDs for the DopplerAdapter
   enum
   {
      TDRS_COMP_DOPPLER_COUNT_INTERVAL = AdapterParamCount,
      TDRS_COMP_SERVICE_ACCESS,
      TDRS_COMP_NODE4_FREQUENCY,
      TDRS_COMP_NODE4_BAND,
      TDRS_COMP_SMAR_ID,
      TDRS_COMP_DATA_FLAG,
      TDRS_REF_DOPPLER_COUNT_INTERVAL,
      TDRS_REF_SERVICE_ACCESS,
      TDRS_REF_NODE4_FREQUENCY,
      TDRS_REF_NODE4_BAND,
      TDRS_REF_SMAR_ID,
      TDRS_REF_DATA_FLAG,
      TDRSDOWDAdapterParamCount,
   };

   /// Strings describing the TDRSDOWDAdapter parameters
   static const 
      std::string PARAMETER_TEXT[TDRSDOWDAdapterParamCount -
                                           AdapterParamCount];
   /// Types of the TDRSDOWDAdapter parameters
   static const 
      Gmat::ParameterType PARAMETER_TYPE[TDRSDOWDAdapterParamCount -
                                           AdapterParamCount];

   Real               GetTurnAroundRatio(Integer freqBand);

private:
   /// MeasurementData for comparison TDRS and reference TDRS
   MeasurementData measDataTDRSComp;
   MeasurementData measDataTDRSRef;

   /// service access index
   UnsignedInt    serviceAccessIndexTDRSComp;
   // serviceAccessListTDRSComp[serviceAccessIndexTDRSComp] 
   // specifies the current service access 
   // used for TDRS3LReturnDoppler measurement for comparison TDRS
   UnsignedInt    serviceAccessIndexTDRSRef;
   // serviceAccessListTDRSRef[serviceAccessIndexTDRSRef] 
   // specifies the current service access 
   // used for TDRS3LReturnDoppler measurement for reference TDRS

      // Multiplier for comparison TDRS and reference TDRS
   Real           multiplierTDRSComp;
   Real           multiplierTDRSRef;
};

#endif /* TDRSDOWDAdapter_hpp */
