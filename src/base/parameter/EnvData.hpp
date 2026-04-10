//$Id$
//------------------------------------------------------------------------------
//                                  EnvData
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
// Created: 2004/12/07
//
/**
 * Declares class which provides environmental data, such as atmospheric density,
 * solar flux, etc.
 */
//------------------------------------------------------------------------------
#ifndef EnvData_hpp
#define EnvData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"

class GMAT_API EnvData : public RefData
{
public:
   
   EnvData();
   EnvData(const EnvData &data);
   EnvData& operator= (const EnvData& right);
   virtual ~EnvData();
   
   Real GetEnvReal(const std::string &str);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   static const Real ENV_REAL_UNDEFINED;// = -9876543210.1234;
   
protected:
   
   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(UnsignedInt type);
   
   Spacecraft *mSpacecraft;
   SolarSystem *mSolarSystem;
   SpacePoint *mOrigin;
   
   enum
   {
      SPACECRAFT = 0,
      SOLAR_SYSTEM,
      SPACE_POINT, //loj: 4/11/05 Changed CELESTIAL_BODY to SPACE_POINT
      EnvDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[EnvDataObjectCount];
};
#endif // EnvData_hpp

   
