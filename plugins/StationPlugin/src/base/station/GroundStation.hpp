//$Id$
//------------------------------------------------------------------------------
//                            GroundStation
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/08/01
// Modified:
//    2010.06.03 Tuan Nguyen
//      - Add AddHardware parameter and verify added hardware
//    2010.03.15 Thomas Grubb
//      - Changed visiblity of PARAMETER_TEXT, PARAMETER_TYPE, and enum from
//        protected to public
//      - Overrode Copy method
//
/**
 * Defines the Groundstation class used to model ground based tracking stations.
 */
//------------------------------------------------------------------------------

#ifndef GroundStation_hpp
#define GroundStation_hpp

#include "StationDefs.hpp"
#include "SpacePoint.hpp"
#include "GroundstationInterface.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "Hardware.hpp"
#include <set>
//#include "MediaCorrectionInterface.hpp"

class STATION_API GroundStation : public GroundstationInterface
{
public:
   GroundStation(const std::string &itsName);
   virtual ~GroundStation();
   GroundStation(const GroundStation& gs);
   GroundStation& operator=(const GroundStation& gs);

   // all leaf classes derived from GmatBase must supply this Clone method
   virtual GmatBase*       Clone() const;
   virtual void            Copy(const GmatBase* orig);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id, const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label, const Real value);

   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual GmatBase*    GetRefObject(const UnsignedInt type,
                                        const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                        const std::string &name = "");

   virtual ObjectArray& GetRefObjectArray(const UnsignedInt type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);


   virtual bool         HasRefObjectTypeArray();
   virtual const        ObjectTypeArray& GetRefObjectTypeArray();


   virtual bool         Initialize();

//   virtual Integer      GetEstimationParameterID(const std::string &param);
//   virtual Integer         SetEstimationParameter(const std::string &param);
   virtual bool         IsEstimationParameterValid(const Integer id);
   virtual Integer      GetEstimationParameterSize(const Integer id);
   virtual Real*        GetEstimationParameterValue(const Integer id);

   virtual bool         IsValidID(const std::string &id);


   virtual Real*        IsValidElevationAngle(const Rvector6 &state_sez);

   virtual bool         CreateErrorModelForSignalPath(std::string spacecraftName,
                                                      std::string spacecraftId);
   virtual std::map<std::string,ObjectArray>& 
                        GetErrorModelMap();

   DEFAULT_TO_NO_CLONES

protected:
   /// Ground station ID
   std::string          stationId;

   /// Added hardware of the ground station
   StringArray          hardwareNames;
   ObjectArray          hardwareList;

   /// Add ionosphere and troposphere correction modes
   std::string ionosphereModel;
   std::string troposphereModel;

   /// Parameters needed for Troposphere correction
   Real                 temperature;                     // unit: Kelvin
   Real                 pressure;                        // unit: hPa
   Real                 humidity;                        // unit: percentage
   std::string          dataSource;                      // specify data source for parameters needed for Troposphere correction: "Constant" or "FromFile"

   /// Parameters needed for verifying measurement feasibility
   Real minElevationAngle;               // unit: degree
   /// Visibility vector
   Real az_el_visible[3];

   /// Error models used for measurements in this gound station
   StringArray     errorModelNames;
   ObjectArray     errorModels;

   /// Containing all clones of ErrorModels associated with a signal path.
   // The first element containing name of spacecraft in uplink signal. 
   // The second element is an object array containing error model clones.
   std::map<std::string,ObjectArray>     errorModelMap;
   std::string  maskFileName;

public:
   /// Published parameters for ground stations
   enum
   {
      STATION_ID = BodyFixedPointParamCount,
      ADD_HARDWARE,
      IONOSPHERE_MODEL,
      TROPOSPHERE_MODEL,
      DATA_SOURCE,                  // When DataSource is 'Constant', that means temperature, pressure, and humidity are read from script, otherwise they are read from a data base 
      TEMPERATURE,                  // temperature (in K) at ground station. It is used for Troposphere correction
      PRESSURE,                     // pressure (in hPa) at ground station. It is used for Troposphere correction
      HUMIDITY,                     // humidity (in %) at ground station. It is used for Troposphere correction
      MINIMUM_ELEVATION_ANGLE,      // It is needed for verifying measurement feasibility
      ERROR_MODELS,                 // ErrorModel contains all information about noise sigma, bias for given measurement types. Therefore, the following parameters have to be removed
      MASK_FILENAME,                 // A path to a mask file. Setting this will create a imager and CustomFOV with the correct orientation
      GroundStationParamCount,
   };

   static const std::string
      PARAMETER_TEXT[GroundStationParamCount - BodyFixedPointParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[GroundStationParamCount - BodyFixedPointParamCount];

 protected:
   static const std::map<std::string, std::set<std::string>>
      DISALLOWED_ANGLE_PAIRS;


private:
   bool             VerifyAddHardware();
   bool             VerifyErrorModels(const GmatBase &errorModelToadd,
                                      std::string &message) const;
};

#endif /*GroundStation_hpp*/