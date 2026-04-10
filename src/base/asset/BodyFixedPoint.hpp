//$Id$
//------------------------------------------------------------------------------
//                            BodyFixedPoint
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
// number NNG06CA54C
//
// Author: Wendy C. Shoan, NASA/GSFC (moved from GroundStation code,
//         original author: Darrel J. Conway, Thinking Systems, Inc.)
// Created: 2008.09.18
// Modified:
//    2010.03.15 Thomas Grubb
//      - Changed visiblity of PARAMETER_TEXT, PARAMETER_TYPE, and enum from
//        protected to public
//      - Added LOCATION_UNITS_x labels to enum
//      - Overrode Copy method
//
/**
 * Defines the BodyFixedPoint class used to model body-fixed space points.
 */
//------------------------------------------------------------------------------

#ifndef BodyFixedPoint_hpp
#define BodyFixedPoint_hpp

#include "SpacePoint.hpp"
#include "BodyFixedStateConverter.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"

#ifdef __USE_SPICE__
   #include "SpiceInterface.hpp"
#endif


class GMAT_API BodyFixedPoint : public SpacePoint
{
public:
   BodyFixedPoint(const std::string &itsType, const std::string &itsName,
         UnsignedInt objType = Gmat::BODY_FIXED_POINT);
   virtual ~BodyFixedPoint();
   BodyFixedPoint(const BodyFixedPoint& bfp);
   BodyFixedPoint& operator=(const BodyFixedPoint& bfp);

   virtual bool            Initialize();
   virtual Real            GetEpoch();
   virtual Real            SetEpoch(const Real ep);
   virtual Rvector6        GetLastState();

   // Parameter access methods - overridden from GmatBase
   virtual void            Copy(const GmatBase* orig);
   virtual std::string     GetParameterText(const Integer id) const;
   virtual Integer         GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;

   virtual bool            IsParameterReadOnly(const Integer id) const;
   virtual bool            IsParameterReadOnly(const std::string &label) const;

   virtual UnsignedInt
                           GetPropertyObjectType(const Integer id) const;
   virtual const StringArray&
                           GetPropertyEnumStrings(const Integer id) const;

   virtual std::string     GetStringParameter(const Integer id) const;
   virtual bool            SetStringParameter(const Integer id,
                                              const std::string &value);
   virtual std::string     GetStringParameter(const std::string &label) const;
   virtual bool            SetStringParameter(const std::string &label,
                                              const std::string &value);

   virtual std::string     GetStringParameter(const Integer id,
                                              const Integer index) const;
   virtual bool            SetStringParameter(const Integer id,
                                              const std::string &value,
                                              const Integer index);
   virtual std::string     GetStringParameter(const std::string &label,
                                              const Integer index) const;
   virtual bool            SetStringParameter(const std::string &label,
                                              const std::string &value,
                                              const Integer index);

   virtual Real            GetRealParameter(const Integer id) const;
   virtual Real            SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real            GetRealParameter(const std::string &label) const;
   virtual Real            SetRealParameter(const std::string &label,
                                         const Real value);

   virtual GmatBase*       GetRefObject(const UnsignedInt type,
                                        const std::string &name);
   virtual bool            SetRefObject(GmatBase *obj, const UnsignedInt type,
                                        const std::string &name = "");
   virtual GmatBase*       GetRefObject(const UnsignedInt type,
                                        const std::string &name,
                                        const Integer index);
   virtual bool            SetRefObject(GmatBase *obj, const UnsignedInt type,
                                        const std::string &name,
                                        const Integer index);

   virtual bool            HasRefObjectTypeArray();
   virtual std::string     GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray&
                           GetRefObjectNameArray(const UnsignedInt type);
   virtual const ObjectTypeArray&
                           GetRefObjectTypeArray();

   virtual const Rvector6  GetMJ2000State(const A1Mjd &atTime);
   virtual const Rvector3  GetMJ2000Position(const A1Mjd &atTime);
   virtual const Rvector3  GetMJ2000Velocity(const A1Mjd &atTime);

   virtual const Rvector6  GetMJ2000State(const GmatTime &atTime);
   virtual const Rvector3  GetMJ2000Position(const GmatTime &atTime);
   virtual const Rvector3  GetMJ2000Velocity(const GmatTime &atTime);

   virtual const Rvector3  GetBodyFixedLocation(const A1Mjd &atTime);
   virtual const Rvector3  GetSphericalLocation(const A1Mjd &atTime);
   virtual CoordinateSystem*
                           GetBodyFixedCoordinateSystem() const;

   virtual void            SetSolarSystem(SolarSystem *ss);
   virtual bool            IsValidID(const std::string &id);

   virtual bool            InitializeForContactLocation(bool deleteFiles = true);
   virtual GmatBase *      Clone() const;

   virtual SpacePoint *    GetCentralBody();

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// The point is attached to this body
   std::string       cBodyName;
   /// The point is attached to this body
   SpacePoint        *theBody;
   /// Mean Radius of the body
   Real              meanEquatorialRadius;
   /// Flattening coefficient of the body
   Real              flattening;
   /// Strings that identify the location parameters
   StringArray       locationLabels;
   /// Strings that identify the location units
   StringArray       locationUnits;
   /// Type of coordinate system
   std::string       stateType;
   /// For geographic states, the horizon type
   std::string       horizon;
   /// The location, in the stateType-horizon system
   Real              location[3];
   /// The Cartesian body-fixed location
   Real              bfLocation[3];
   /// A solar system pointer (not sure if this is needed)
   //SolarSystem       *solarSystem;
   /// name of the BodyFixed coordinate system
   std::string       bfcsName;
   /// The body-fixed coordinate system that the BFP lives on
   CoordinateSystem  *bfcs;
   /// name of the MJ2000 coordinate system
   std::string       mj2kcsName;
   /// A MJ2000 coordinate system
   CoordinateSystem  *mj2kcs;

   A1Mjd             lastStateTime;
   GmatTime          lastStateTimeGT;
   Rvector6          lastState;

   /// Keeps track of the last GmatGlobal::Instance()->GetCurrentEpoch() so that we know if it has updated
   GmatEpoch runEpoch;
   /// Base filename for the SPK and FK kernels
   std::string       kernelBaseName;
   /// The SPK kernel written for this Asset
   std::string       spkName;
   /// The FK kernel written for this Asset
   std::string       fkName;
   /// delete SPK file on destruction?
   bool              deleteSPK;
   /// delete FK file on destruction?
   bool              deleteFK;
   /// has he NaifID been determined?
   bool              naifIDDetermined;
   /// have the kernel names been figured out?
   bool              kernelNamesDetermined;
   /// Were the kernels written?
   bool              kernelsWritten;

   #ifdef __USE_SPICE__
      SpiceInterface  *spice;
   #endif

   Rvector3       j2000Pos;
   Rvector3       j2000Vel;
   Rvector6       j2000PosVel;


   static Integer gsNaifId;

   /// Converter helper
   CoordinateConverter ccvtr;

   //virtual bool HasLocalClones() const;
   //virtual void RenameRefObject(const UnsignedInt, const std::string & name, const std::string &newName);
   void UpdateBodyFixedLocation();

   bool WriteSPK(bool deleteFile = true);
   bool WriteFK(bool deleteFile = true);

   Rvector3 GetTopocentricConversion(const std::string &centralNaifId);

public:  // needs to be public for current GroundStationPanel code to use
   /// Published parameters for body-fixed points
   enum
   {
      CENTRAL_BODY = SpacePointParamCount,
      STATE_TYPE,          // Cartesian or Spherical
      HORIZON_REFERENCE,   // Sphere or Ellipsoid
      LOCATION_1,          // X or Latitude
      LOCATION_2,          // Y or Longitude
      LOCATION_3,          // Z or Height
      LOCATION_LABEL_1,
      LOCATION_LABEL_2,
      LOCATION_LABEL_3,
      LOCATION_UNITS_1,
      LOCATION_UNITS_2,
      LOCATION_UNITS_3,
      BodyFixedPointParamCount
   };

   /// burn parameter labels
   static const std::string
      PARAMETER_TEXT[BodyFixedPointParamCount - SpacePointParamCount];
   /// burn parameter types
   static const Gmat::ParameterType
      PARAMETER_TYPE[BodyFixedPointParamCount - SpacePointParamCount];

};

#endif /*BodyFixedPoint_hpp*/
