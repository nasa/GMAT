//$Id$
//------------------------------------------------------------------------------
//                                  SpacecraftData
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun
// Created: 2009.03.20
//
/**
 * Declares Spacecraft Spacecraft related data class.
 */
//------------------------------------------------------------------------------
#ifndef SpacecraftData_hpp
#define SpacecraftData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"

class GMAT_API SpacecraftData : public RefData
{
public:

   SpacecraftData(const std::string &name = "");
   SpacecraftData(const SpacecraftData &data);
   SpacecraftData& operator= (const SpacecraftData& right);
   virtual ~SpacecraftData();
   
   Real GetReal(Integer item);
   Real SetReal(Integer item, Real val);

   std::string GetString(Integer item);
   Real SetString(Integer item, std::string &val);

   Rvector GetRvector(Integer item);
   Rvector SetRvector(Integer item, Rvector &val);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   static const Real BALLISTIC_REAL_UNDEFINED;

   
protected:

   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(UnsignedInt type);
   
   Spacecraft *mSpacecraft;

   Rvector UNDEFINED_RETURN_RVECTOR = Rvector(3, 1, 1, BALLISTIC_REAL_UNDEFINED);
   std::string UNDEFINED_RETURN_STRING = "UNDEFINED_RETURN_STRING";
   
   enum 
   {
      DRY_MASS,
      DRY_CM_X,
      DRY_CM_Y,
      DRY_CM_Z,
      DRY_MOI_XX,
      DRY_MOI_XY,
      DRY_MOI_XZ,
      DRY_MOI_YY,
      DRY_MOI_YZ,
      DRY_MOI_ZZ,
      DRAG_COEFF,
      REFLECT_COEFF,
      DRAG_AREA,
      SRP_AREA,
      TOTAL_MASS,
      SYSTEM_CM_X,
      SYSTEM_CM_Y,
      SYSTEM_CM_Z,
      SYSTEM_MOI_XX,
      SYSTEM_MOI_XY,
      SYSTEM_MOI_XZ,
      SYSTEM_MOI_YY,
      SYSTEM_MOI_YZ,
      SYSTEM_MOI_ZZ,
      SPAD_DRAG_COEFF, 
      SPAD_REFLECT_COEFF, 
      ATMOS_DENSITY_SCALE_FACTOR,
      DRAG_COEFF_SIGMA,
      REFLECT_COEFF_SIGMA,
      SPAD_DRAG_COEFF_SIGMA,
      SPAD_REFLECT_COEFF_SIGMA,
      ATMOS_DENSITY_SCALE_FACTOR_SIGMA,
      
      // for Spacecraft owned FuelTank
      FUEL_MASS,
      FUEL_CM_X,
      FUEL_CM_Y,
      FUEL_CM_Z,
      FUEL_MOI_XX,
      FUEL_MOI_XY,
      FUEL_MOI_XZ,
      FUEL_MOI_YY,
      FUEL_MOI_YZ,
      FUEL_MOI_ZZ,
      FUEL_CM_X_BCS,
      FUEL_CM_Y_BCS,
      FUEL_CM_Z_BCS,
      FUEL_MOI_XX_BCS,
      FUEL_MOI_XY_BCS,
      FUEL_MOI_XZ_BCS,
      FUEL_MOI_YY_BCS,
      FUEL_MOI_YZ_BCS,
      FUEL_MOI_ZZ_BCS,
      PRESSURE,
      TEMPERATURE,
      VOLUME,
      FUEL_DENSITY,
      REF_TEMPERATURE,

      // for Spacecraft owned Hardware 
      HW_R_SB_11,
      HW_R_SB_12,
      HW_R_SB_13,
      HW_R_SB_21,
      HW_R_SB_22,
      HW_R_SB_23,
      HW_R_SB_31,
      HW_R_SB_32,
      HW_R_SB_33,
      
      // for Spacecraft owned Thruster
      DUTY_CYCLE,
      THRUSTER_SCALE_FACTOR,
      GRAVITATIONAL_ACCEL,
      C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16,
      K1, K2, K3, K4, K5, K6, K7, K8, K9, K10, K11, K12, K13, K14, K15, K16,
      THRUST_DIRECTION1, THRUST_DIRECTION2, THRUST_DIRECTION3, THRUST, ISP,
      MASS_FLOW_RATE,
      
      // for Spacecraft-owned PowerSystem
      TOTAL_POWER_AVAILABLE,
      REQUIRED_BUS_POWER,
      THRUST_POWER_AVAILABLE,

      // for Spacecraft-owned Plate
      TYPE_ID,
      PLATE_NORMAL_HISTORY_FILE_ID,
      PLATE_NORMAL_ID,
      AREA_ID,
      AREA_COEFFICIENT_ID,
      AREA_COEFFICIENT_SIGMA_ID,
      LIT_FRACTION_ID,
      SPECULAR_FRACTION_ID,
      SPECULAR_FRACTION_SIGMA_ID,
      DIFFUSE_FRACTION_ID,
      DIFFUSE_FRACTION_SIGMA_ID,
      PLATEX,
      PLATEY,
      PLATEZ,
   };
   
   enum
   {
      SPACECRAFT = 0,
      SpacecraftDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[SpacecraftDataObjectCount];

   // Class level member so it stays in scope when returned
   Rvector vectorResult;

private:
   
   Real GetOwnedObjectProperty(UnsignedInt objType, const std::string &name);
   Real SetOwnedObjectProperty(UnsignedInt objType, const std::string &name,
                               Real val);

   std::string GetOwnedStringObjectProperty(UnsignedInt objType, const std::string &name);
   Real SetOwnedObjectProperty(UnsignedInt objType, const std::string &name,
      std::string& val);

   Rvector GetOwnedRvectorObjectProperty(UnsignedInt objType, const std::string &name);
   Rvector SetOwnedObjectProperty(UnsignedInt objType, const std::string &name,
      Rvector& val);
   
};

#endif /*SpacecraftData_hpp*/
