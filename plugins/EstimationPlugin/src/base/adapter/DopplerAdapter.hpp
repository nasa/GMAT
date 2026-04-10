//------------------------------------------------------------------------------
//                           DopplerAdapter
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
// Created: Sept 30, 2014
/**
 * A measurement adapter for DSN Doppler (unit: Hz)
 */
//------------------------------------------------------------------------------

#ifndef DopplerAdapter_hpp
#define DopplerAdapter_hpp

#include "RangeAdapterKm.hpp"


/**
 * A measurement adapter for DSN Doppler
 */
class ESTIMATION_API DopplerAdapter: public RangeAdapterKm
{
public:
   DopplerAdapter(const std::string& name);
   virtual ~DopplerAdapter();
   DopplerAdapter(const DopplerAdapter& da);
   DopplerAdapter&      operator=(const DopplerAdapter& da);

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
   virtual bool         SetStringParameter(const std::string &label, const std::string &value);
   virtual bool         SetStringParameter(const std::string &label, const std::string &value, const Integer index);

   virtual Integer      SetIntegerParameter(const Integer id, const Integer value);
   virtual Integer      SetIntegerParameter(const std::string &label, const Integer value);

   virtual bool         SetBooleanParameter(const Integer id, const bool value);
   virtual bool         SetBooleanParameter(const std::string &label, const bool value);

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


   /// RangeAdapterKm object for S-path
   RangeAdapterKm* adapterS;

   DEFAULT_TO_NO_CLONES

protected:
   /// Constant frequency value used in a physical measurement when needed for E path in DSNDoppler
   Real                 uplinkFreqE;                      // unit: Hz
   /// Frequency band for E path in DSNDoppler
   Integer              freqBandE;
   /// Doppler count interval
   Real                 dopplerCountInterval;             // unit: second
   /// Turn around ratio
   Real                 turnaround;
   /// Multiplier for S-path and E-path
   Real                 multiplierS;
   Real                 multiplierE;

   /// Parameter IDs for the DopplerAdapter
   enum
   {
      DOPPLER_COUNT_INTERVAL = RangeAdapterKmParamCount,
      DopplerAdapterParamCount,
   };

   /// Strings describing the DopplerAdapter parameters
   static const std::string PARAMETER_TEXT[DopplerAdapterParamCount -
                                           RangeAdapterKmParamCount];
   /// Types of the DopplerAdapter parameters
   static const Gmat::ParameterType PARAMETER_TYPE[DopplerAdapterParamCount -
                                                   RangeAdapterKmParamCount];

   Real               GetTurnAroundRatio(Integer freqBand);

private:
   /// MeasurementData for Start path
   MeasurementData measDataS;
   /// MeasurementData for End path
   MeasurementData measDataE;
};

#endif /* DopplerAdapter_hpp */
