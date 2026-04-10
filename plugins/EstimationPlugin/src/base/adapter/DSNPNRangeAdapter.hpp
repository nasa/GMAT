//------------------------------------------------------------------------------
//                           DSNPNRangeAdapter
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
// Developed jointly by NASA/GSFC and Pearl River Technologies under the FDSS 
// contract
//
// Author: Peter Candell
// Created: Sept 23, 2024
/**
 * A measurement adapter for DSN PN ranges in Range Unit (RU)
 */
//------------------------------------------------------------------------------

#ifndef DSNPNRangeAdapter_hpp
#define DSNPNRangeAdapter_hpp

#include "RangeAdapterKm.hpp"


/**
 * A measurement adapter for DSN PN ranges in Range Unit (RU)
 */
class ESTIMATION_API DSNPNRangeAdapter: public RangeAdapterKm
{
public:
   DSNPNRangeAdapter(const std::string& name);
   virtual ~DSNPNRangeAdapter();
   DSNPNRangeAdapter(const DSNPNRangeAdapter& dsn);
   DSNPNRangeAdapter&      operator=(const DSNPNRangeAdapter& dsn);

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


   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);

   virtual bool         Initialize();

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
   virtual void         SetCorrection(const std::string& correctionName,
         const std::string& correctionType);

   DEFAULT_TO_NO_CLONES

protected:
   /// Range modulo constant
   Real            rangeModulo;                             // unit: RU

   /// Parameter IDs for the DSNPNRangeAdapter
   enum
   {
      RANGE_MODULO_CONSTANT = RangeAdapterKmParamCount,
      DSNPNRangeAdapterParamCount,
   };

   /// Strings describing the DSNPNRangeAdapter parameters
   static const std::string PARAMETER_TEXT[DSNPNRangeAdapterParamCount -
                                           RangeAdapterKmParamCount];
   /// Types of the DSNPNRangeAdapter parameters
   static const Gmat::ParameterType PARAMETER_TYPE[DSNPNRangeAdapterParamCount -
                                                   RangeAdapterKmParamCount];

   Real                 GetFrequencyFactor(Real frequency);
   virtual Real         IntegralRampedFrequency(GmatTime t1, Real delta_t, Integer& err);

};

#endif /* DSNPNRangeAdapter_hpp */
