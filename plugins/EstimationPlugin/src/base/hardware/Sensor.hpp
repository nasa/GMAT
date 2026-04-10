//$Id: Sensor.hpp 211 2010-06-07 11:26:00  $
//------------------------------------------------------------------------------
//                             Sensor
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Tuan Nguyen
// Created: 2010/06/07 by Tuan Nguyen (GSFC-NASA)
//
/**
 * Implementation for the Sensor class
 */
//------------------------------------------------------------------------------

#ifndef Sensor_hpp
#define Sensor_hpp

/**
 * Sensor is the base class for all sensor hardware used in the estimation
 * subsystem.
 */

#include "estimation_defs.hpp"
#include "Imager.hpp"
#include "Signal.hpp"


class ESTIMATION_API Sensor : public Imager
{
public:
   Sensor(const std::string &type, const std::string &name);
   virtual ~Sensor();
   Sensor(const Sensor& sen);
   Sensor& operator=(const Sensor& sen);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   // These are the parameter accessors.  The commented out versions may be
   // needed if derived classes implement them in order to prevent compiler
   // confusion.

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id, const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label, const Integer index) const;
   virtual bool         SetStringParameter(const Integer id, const std::string &value, const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value, const Integer index);


   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);


   virtual bool         Initialize();

   virtual Real         GetDelay(Integer whichOne=0);
   virtual bool         SetDelay(Real delay, Integer whichOne=0);
   virtual bool         IsFeasible(Integer whichOne=0);
   virtual Integer      GetSignalCount();
   virtual bool         IsTransmitted(Integer whichOne=0);
   virtual Signal*      GetSignal(Integer whichOne=0);
   virtual bool         SetSignal(Signal* s,Integer whichOne=0);


protected:
   Signal*          signal1;
   Signal*          signal2;
   Real             hardwareDelay1;
   Real             hardwareDelay2;
   bool             isTransmitted1;
   bool             isTransmitted2;
   std::string      sensorID;

   /// Published parameters for the sensor
   enum
   {
      SENSOR_ID = ImagerParamCount,
      HARDWARE_DELAY,
      SensorParamCount,
   };

   static const std::string
      PARAMETER_TEXT[SensorParamCount - ImagerParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[SensorParamCount - ImagerParamCount];



};

#endif /* Sensor_hpp */
