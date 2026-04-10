//$Id: RFHardware.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             RFHardware
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
// Author: Darrel J. Conway
// Created: 2010/02/22 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the RFHardware class
 */
//------------------------------------------------------------------------------


#ifndef RFHardware_hpp
#define RFHardware_hpp

#include "estimation_defs.hpp"
#include "Sensor.hpp"
#include "Antenna.hpp"

/**
 * RFHardware is the base class for all RF based hardware used in the estimation
 * subsystem.
 */
class ESTIMATION_API RFHardware : public Sensor
{
public:
   RFHardware(const std::string &type, const std::string &name);
   virtual ~RFHardware();
   RFHardware(const RFHardware& rfh);
   RFHardware& operator=(const RFHardware& rfh);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual UnsignedInt GetPropertyObjectType(const Integer id) const;

   // These are the parameter accessors.  The commented out versions may be
   // needed if derived classes implement them in order to prevent compiler
   // confusion.

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id, const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                      const std::string &value);

   virtual std::string  GetStringParameter(const Integer id, const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label, const Integer index) const;
   virtual bool         SetStringParameter(const Integer id, const std::string &value, const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                      const std::string &value, const Integer index);

   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                     const std::string &name = "");
   virtual GmatBase*    GetRefObject(const UnsignedInt type, const std::string &name, const Integer index);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name, const Integer index);

   virtual std::string  GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);

   virtual const        ObjectTypeArray& GetRefObjectTypeArray();
   virtual bool         HasRefObjectTypeArray();
   virtual bool         HasLocalClone() { return true;};

   virtual bool         Initialize();

protected:
   Antenna*    primaryAntenna;
   std::string primaryAntennaName;

   /// Published parameters for the RF hardware
   enum
   {
      PRIMARY_ANTENNA = SensorParamCount,
      RFHardwareParamCount,
   };

   static const std::string
      PARAMETER_TEXT[RFHardwareParamCount - SensorParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[RFHardwareParamCount - SensorParamCount];

};

#endif /* RFHardware_hpp */
