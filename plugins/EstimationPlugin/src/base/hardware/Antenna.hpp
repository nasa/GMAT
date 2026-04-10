// Antenna.h
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
// number S-67573-G
//
//
//   Created on: Mar 31, 2010
//       Author: tdnguye2
//

#ifndef Antenna_hpp
#define Antenna_hpp

#include "estimation_defs.hpp"
#include "Imager.hpp"

class ESTIMATION_API Antenna : public Imager
{
public:
   Antenna(const std::string &type, const std::string &name);
   virtual ~Antenna();
   Antenna(const Antenna& ant);
   Antenna& operator=(const Antenna& ant);
   virtual GmatBase* Clone() const;
   virtual void      Copy(const GmatBase* ant);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;


   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);

   DEFAULT_TO_NO_REFOBJECTS

protected:
   Real       antennaDelay;
   Real       phaseCenterLocation1;
   Real       phaseCenterLocation2;
   Real       phaseCenterLocation3;

   /// Published parameters for the RF hardware

   enum
   {
      ANTENNA_DELAY = ImagerParamCount,
      PHASE_CENTER_LOCATION1,
      PHASE_CENTER_LOCATION2,
      PHASE_CENTER_LOCATION3,
      AntennaParamCount,
   };

   static const std::string
      PARAMETER_TEXT[AntennaParamCount - ImagerParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[AntennaParamCount - ImagerParamCount];
};

#endif /* Antenna_hpp */
