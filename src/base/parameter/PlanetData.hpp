//$Id$
//------------------------------------------------------------------------------
//                                  PlanetData
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
// Author: Linda Jun
// Created: 2004/12/13
//
/**
 * Declares class which provides planet related data, such as HourAngle.
 */
//------------------------------------------------------------------------------
#ifndef PlanetData_hpp
#define PlanetData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"

class GMAT_API PlanetData : public RefData
{
public:
   
   PlanetData();
   PlanetData(const PlanetData &data);
   PlanetData& operator= (const PlanetData& right);
   virtual ~PlanetData();
   
   Real GetPlanetReal(Integer item);
   Real GetPlanetReal(const std::string &str);
   void SetInternalCoordSystem(CoordinateSystem *cs);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   static const Real PLANET_REAL_UNDEFINED;// = -9876543210.1234;
   
protected:

   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(UnsignedInt type);

   std::string mCentralBodyName;
   
   Spacecraft *mSpacecraft;
   SolarSystem *mSolarSystem;
   CelestialBody *mCentralBody;
   CelestialBody *mOrigin;
   CoordinateSystem *mInternalCoordSystem;
   CoordinateSystem *mOutCoordSystem;
   
   CoordinateConverter mCoordConverter;

   enum {LATITUDE, LONGITUDE, ALTITUDE, MHA_ID, LST_ID};
   
   enum
   {
      SPACECRAFT = 0,
      SOLAR_SYSTEM,
      COORD_SYSTEM,
      SPACE_POINT,
      PlanetDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[PlanetDataObjectCount];
   static const std::string VALID_PLANET_DATA_NAMES[LST_ID - LATITUDE + 1];
};
#endif // PlanetData_hpp

   
