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
 * Implements Spacecraft related data class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SpacecraftData.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"          // ToString()
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SPACECRAFTDATA_INIT
//#define DEBUG_SC_OWNED_OBJ
//#define DEBUG_SETREAL
//#define DEBUG_HW_GET_R_SB 
//#define DEBUG_HW_SET_R_SB 


const std::string
SpacecraftData::VALID_OBJECT_TYPE_LIST[SpacecraftDataObjectCount] =
{
   "Spacecraft"
}; 

const Real SpacecraftData::BALLISTIC_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;

//------------------------------------------------------------------------------
// SpacecraftData()
//------------------------------------------------------------------------------
SpacecraftData::SpacecraftData(const std::string &name)
   : RefData(name)
{
   mSpacecraft = NULL;
}


//------------------------------------------------------------------------------
// SpacecraftData(const SpacecraftData &copy)
//------------------------------------------------------------------------------
SpacecraftData::SpacecraftData(const SpacecraftData &copy)
   : RefData(copy)
{
   mSpacecraft = copy.mSpacecraft;
}


//------------------------------------------------------------------------------
// SpacecraftData& operator= (const SpacecraftData& right)
//------------------------------------------------------------------------------
SpacecraftData& SpacecraftData::operator= (const SpacecraftData& right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
   
   mSpacecraft = right.mSpacecraft;

   return *this;
}


//------------------------------------------------------------------------------
// ~SpacecraftData()
//------------------------------------------------------------------------------
SpacecraftData::~SpacecraftData()
{
}


//------------------------------------------------------------------------------
// Real GetReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves Spacecraft or spacecraft owned hardware element by integer id.
 */
//------------------------------------------------------------------------------
Real SpacecraftData::GetReal(Integer item)
{
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   // if there was an error initializing the ref objects, do not try to get the value
   if (mSpacecraft == NULL)
      return BALLISTIC_REAL_UNDEFINED;

   switch (item)
   {
   case DRY_MASS:
      return mSpacecraft->GetRealParameter("DryMass");
   case DRY_CM_X:
      return mSpacecraft->GetRealParameter("DryCenterOfMassX");
   case DRY_CM_Y:
      return mSpacecraft->GetRealParameter("DryCenterOfMassY");
   case DRY_CM_Z:
      return mSpacecraft->GetRealParameter("DryCenterOfMassZ");
   case DRY_MOI_XX:
      return mSpacecraft->GetRealParameter("DryMomentOfInertiaXX");
   case DRY_MOI_XY:
      return mSpacecraft->GetRealParameter("DryMomentOfInertiaXY");
   case DRY_MOI_XZ:
      return mSpacecraft->GetRealParameter("DryMomentOfInertiaXZ");
   case DRY_MOI_YY:
      return mSpacecraft->GetRealParameter("DryMomentOfInertiaYY");
   case DRY_MOI_YZ:
      return mSpacecraft->GetRealParameter("DryMomentOfInertiaYZ");
   case DRY_MOI_ZZ:
      return mSpacecraft->GetRealParameter("DryMomentOfInertiaZZ");
   case DRAG_COEFF:
      return mSpacecraft->GetRealParameter("Cd");
   case REFLECT_COEFF:
      return mSpacecraft->GetRealParameter("Cr");
   case DRAG_AREA:
      return mSpacecraft->GetRealParameter("DragArea");
   case SRP_AREA:      
      return mSpacecraft->GetRealParameter("SRPArea");
   case TOTAL_MASS:
      return mSpacecraft->GetRealParameter("TotalMass");
   case SYSTEM_CM_X:
      return mSpacecraft->GetRealParameter("SystemCenterOfMassX");
   case SYSTEM_CM_Y:
      return mSpacecraft->GetRealParameter("SystemCenterOfMassY");
   case SYSTEM_CM_Z:
      return mSpacecraft->GetRealParameter("SystemCenterOfMassZ");
   case SYSTEM_MOI_XX:
      return mSpacecraft->GetRealParameter("SystemMomentOfInertiaXX");
   case SYSTEM_MOI_XY:
      return mSpacecraft->GetRealParameter("SystemMomentOfInertiaXY");
   case SYSTEM_MOI_XZ:
      return mSpacecraft->GetRealParameter("SystemMomentOfInertiaXZ");
   case SYSTEM_MOI_YY:
      return mSpacecraft->GetRealParameter("SystemMomentOfInertiaYY");
   case SYSTEM_MOI_YZ:
      return mSpacecraft->GetRealParameter("SystemMomentOfInertiaYZ");
   case SYSTEM_MOI_ZZ:
      return mSpacecraft->GetRealParameter("SystemMomentOfInertiaZZ");
   case SPAD_DRAG_COEFF:
      return mSpacecraft->GetRealParameter("SPADDragScaleFactor");
   case SPAD_REFLECT_COEFF:
      return mSpacecraft->GetRealParameter("SPADSRPScaleFactor");
   case ATMOS_DENSITY_SCALE_FACTOR:
      return mSpacecraft->GetRealParameter("AtmosDensityScaleFactor");
   case DRAG_COEFF_SIGMA:
      return mSpacecraft->GetRealParameter("CdSigma");
   case REFLECT_COEFF_SIGMA:
      return mSpacecraft->GetRealParameter("CrSigma");
   case SPAD_DRAG_COEFF_SIGMA:
      return mSpacecraft->GetRealParameter("SPADDragScaleFactorSigma");
   case SPAD_REFLECT_COEFF_SIGMA:
      return mSpacecraft->GetRealParameter("SPADSRPScaleFactorSigma");
   case ATMOS_DENSITY_SCALE_FACTOR_SIGMA:
      return mSpacecraft->GetRealParameter("AtmosDensityScaleFactorSigma");
      
   // for Spacecraft owned FuelTank
   case FUEL_MASS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMass");
   case FUEL_CM_X:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelCenterOfMassX");
   case FUEL_CM_Y:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelCenterOfMassY");
   case FUEL_CM_Z:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelCenterOfMassZ");
   case FUEL_MOI_XX:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaXX");
   case FUEL_MOI_XY:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaXY");
   case FUEL_MOI_XZ:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaXZ");
   case FUEL_MOI_YY:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaYY");
   case FUEL_MOI_YZ:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaYZ");
   case FUEL_MOI_ZZ:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaZZ");
   case FUEL_CM_X_BCS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelCenterOfMassX_BCS");
   case FUEL_CM_Y_BCS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelCenterOfMassY_BCS");
   case FUEL_CM_Z_BCS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelCenterOfMassZ_BCS");
   case FUEL_MOI_XX_BCS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaXX_BCS");
   case FUEL_MOI_XY_BCS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaXY_BCS");
   case FUEL_MOI_XZ_BCS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaXZ_BCS");
   case FUEL_MOI_YY_BCS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaYY_BCS");
   case FUEL_MOI_YZ_BCS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaYZ_BCS");
   case FUEL_MOI_ZZ_BCS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaZZ_BCS");
   case PRESSURE:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "Pressure");
   case TEMPERATURE:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "Temperature");
   case REF_TEMPERATURE:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "RefTemperature");
   case VOLUME:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "Volume");
   case FUEL_DENSITY:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelDensity");
      
   // for Spacecraft owned Hardware 
#ifdef DEBUG_HW_GET_R_SB 
   MessageInterface::ShowMessage("\nSpacecraftData getting R_SB11..SB33\n");
#endif
   case HW_R_SB_11:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB11");
   case HW_R_SB_12:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB12");
   case HW_R_SB_13:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB13");
   case HW_R_SB_21:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB21");
   case HW_R_SB_22:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB22");
   case HW_R_SB_23:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB23");
   case HW_R_SB_31:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB31");
   case HW_R_SB_32:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB32");
   case HW_R_SB_33:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB33");

   // for Spacecraft owned Thruster
   case DUTY_CYCLE:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "DutyCycle");
   case THRUSTER_SCALE_FACTOR:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "ThrustScaleFactor");
   case GRAVITATIONAL_ACCEL:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "GravitationalAccel");
   case THRUST:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "ThrustMagnitude");
   case ISP:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "Isp");
   case MASS_FLOW_RATE:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "MassFlowRate");
      
   // for Spacecraft owned PowerSystem
   case TOTAL_POWER_AVAILABLE:
      return GetOwnedObjectProperty(Gmat::POWER_SYSTEM, "TotalPowerAvailable");
   case REQUIRED_BUS_POWER:
      return GetOwnedObjectProperty(Gmat::POWER_SYSTEM, "RequiredBusPower");
   case THRUST_POWER_AVAILABLE:
      return GetOwnedObjectProperty(Gmat::POWER_SYSTEM, "ThrustPowerAvailable");

   // Thrust Coefficients
   case C1:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C1");
   case C2:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C2");
   case C3:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C3");
   case C4:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C4");
   case C5:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C5");
   case C6:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C6");
   case C7:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C7");
   case C8:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C8");
   case C9:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C9");
   case C10:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C10");
   case C11:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C11");
   case C12:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C12");
   case C13:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C13");
   case C14:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C14");
   case C15:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C15");
   case C16:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C16");
      
   // Impulse Coefficients
   case K1:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K1");
   case K2:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K2");
   case K3:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K3");
   case K4:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K4");
   case K5:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K5");
   case K6:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K6");
   case K7:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K7");
   case K8:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K8");
   case K9:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K9");
   case K10:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K10");
   case K11:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K11");
   case K12:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K12");
   case K13:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K13");
   case K14:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K14");
   case K15:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K15");
   case K16:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K16");
      
   // Thruster ThrustDirections
   case THRUST_DIRECTION1:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection1");
   case THRUST_DIRECTION2:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection2");
   case THRUST_DIRECTION3:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection3");
      
   case AREA_COEFFICIENT_ID:
      return GetOwnedObjectProperty(Gmat::PLATE, "AreaCoefficient");
   case SPECULAR_FRACTION_ID:
      return GetOwnedObjectProperty(Gmat::PLATE, "SpecularFraction");
   case DIFFUSE_FRACTION_ID:
      return GetOwnedObjectProperty(Gmat::PLATE, "DiffuseFraction");

   case PLATE_NORMAL_ID:
      return GetOwnedObjectProperty(Gmat::PLATE, "PlateNormal");
   case AREA_ID:
      return GetOwnedObjectProperty(Gmat::PLATE, "Area");
   case LIT_FRACTION_ID:
      return GetOwnedObjectProperty(Gmat::PLATE, "LitFraction");

   case AREA_COEFFICIENT_SIGMA_ID:
      return GetOwnedObjectProperty(Gmat::PLATE, "AreaCoefficientSigma");
   case SPECULAR_FRACTION_SIGMA_ID:
      return GetOwnedObjectProperty(Gmat::PLATE, "SpecularFractionSigma");
   case DIFFUSE_FRACTION_SIGMA_ID:
      return GetOwnedObjectProperty(Gmat::PLATE, "DiffuseFractionSigma");

   case PLATEX:
      return GetOwnedObjectProperty(Gmat::PLATE, "PlateX");
   case PLATEY:
      return GetOwnedObjectProperty(Gmat::PLATE, "PlateY");
   case PLATEZ:
      return GetOwnedObjectProperty(Gmat::PLATE, "PlateZ");

   default:
      // otherwise, there is an error   
      throw ParameterException
         ("SpacecraftData::GetReal() Not readable or unknown item id: " +
          GmatStringUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real SetReal(Integer item, Real val)
//------------------------------------------------------------------------------
/**
 * Sets Spacecraft or spacecraft owned hardware element by integer id.
 */
//------------------------------------------------------------------------------
Real SpacecraftData::SetReal(Integer item, Real val)
{
   #ifdef DEBUG_SETREAL
      MessageInterface::ShowMessage("Setting %s\n", mActualParamName.c_str());
   #endif
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   // if there was an error initializing the ref objects, do not try to set the value
   if (mSpacecraft == NULL)
      return BALLISTIC_REAL_UNDEFINED;

   switch (item)
   {
   case DRY_MASS:
      return mSpacecraft->SetRealParameter("DryMass", val);
   case DRY_CM_X:
      return mSpacecraft->SetRealParameter("DryCenterOfMassX", val);
   case DRY_CM_Y:
      return mSpacecraft->SetRealParameter("DryCenterOfMassY", val);
   case DRY_CM_Z:
      return mSpacecraft->SetRealParameter("DryCenterOfMassZ", val);
   case DRY_MOI_XX:
      return mSpacecraft->SetRealParameter("DryMomentOfInertiaXX", val);
   case DRY_MOI_XY:
      return mSpacecraft->SetRealParameter("DryMomentOfInertiaXY", val);
   case DRY_MOI_XZ:
      return mSpacecraft->SetRealParameter("DryMomentOfInertiaXZ", val);
   case DRY_MOI_YY:
      return mSpacecraft->SetRealParameter("DryMomentOfInertiaYY", val);
   case DRY_MOI_YZ:
      return mSpacecraft->SetRealParameter("DryMomentOfInertiaYZ", val);
   case DRY_MOI_ZZ:
      return mSpacecraft->SetRealParameter("DryMomentOfInertiaZZ", val);
   case DRAG_COEFF:
      return mSpacecraft->SetRealParameter("Cd", val);
   case REFLECT_COEFF:
      return mSpacecraft->SetRealParameter("Cr", val);
   case DRAG_AREA:
      return mSpacecraft->SetRealParameter("DragArea", val);
   case SRP_AREA:      
      return mSpacecraft->SetRealParameter("SRPArea", val);
   case SPAD_DRAG_COEFF:
      return mSpacecraft->SetRealParameter("SPADDragScaleFactor", val);
   case SPAD_REFLECT_COEFF:
      return mSpacecraft->SetRealParameter("SPADSRPScaleFactor", val);
   case ATMOS_DENSITY_SCALE_FACTOR:
      return mSpacecraft->SetRealParameter("AtmosDensityScaleFactor", val);
   case DRAG_COEFF_SIGMA:
      return mSpacecraft->SetRealParameter("CdSigma", val);
   case REFLECT_COEFF_SIGMA:
      return mSpacecraft->SetRealParameter("CrSigma", val);
   case SPAD_DRAG_COEFF_SIGMA:
      return mSpacecraft->SetRealParameter("SPADDragScaleFactorSigma", val);
   case SPAD_REFLECT_COEFF_SIGMA:
      return mSpacecraft->SetRealParameter("SPADSRPScaleFactorSigma", val);
   case ATMOS_DENSITY_SCALE_FACTOR_SIGMA:
      return mSpacecraft->SetRealParameter("AtmosDensityScaleFactorSigma", val);
      
   // for Spacecraft owned FuelTank
   case FUEL_MASS:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMass", val);
   case FUEL_CM_X:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelCenterOfMassX", val);
   case FUEL_CM_Y:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelCenterOfMassY", val);
   case FUEL_CM_Z:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelCenterOfMassZ", val);
   case FUEL_MOI_XX:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaXX", val);
   case FUEL_MOI_XY:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaXY", val);
   case FUEL_MOI_XZ:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaXZ", val);
   case FUEL_MOI_YY:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaYY", val);
   case FUEL_MOI_YZ:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaYZ", val);
   case FUEL_MOI_ZZ:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMomentOfInertiaZZ", val);
   case PRESSURE:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "Pressure", val);
   case TEMPERATURE:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "Temperature", val);
   case REF_TEMPERATURE:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "RefTemperature", val);
   case VOLUME:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "Volume", val);
   case FUEL_DENSITY:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelDensity", val);
      
#if DEBUG_HW_SET_R_SB // @TODO: hold this SetParameter for R_SB11..SB33
   MessageInterface::ShowMessage("\nSpacecraftData setting R_SB11..SB33\n");

   // for Spacecraft owned Hardware 
   case HW_R_SB_11:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB11", val);
   case HW_R_SB_12:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB12", val);
   case HW_R_SB_13:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB13", val);
   case HW_R_SB_21:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB21", val);
   case HW_R_SB_22:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB22", val);
   case HW_R_SB_23:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB23", val);
   case HW_R_SB_31:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB31", val);
   case HW_R_SB_32:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB32", val);
   case HW_R_SB_33:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "R_SB33", val);
#endif

   // for Spacecraft owned Thruster
   case DUTY_CYCLE:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "DutyCycle", val);
   case THRUSTER_SCALE_FACTOR:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "ThrustScaleFactor", val);
   case GRAVITATIONAL_ACCEL:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "GravitationalAccel", val);
      
   // Thrust Coefficients
   case C1:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C1", val);
   case C2:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C2", val);
   case C3:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C3", val);
   case C4:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C4", val);
   case C5:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C5", val);
   case C6:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C6", val);
   case C7:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C7", val);
   case C8:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C8", val);
   case C9:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C9", val);
   case C10:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C10", val);
   case C11:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C11", val);
   case C12:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C12", val);
   case C13:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C13", val);
   case C14:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C14", val);
   case C15:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C15", val);
   case C16:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C16", val);
      
   // Impulse Coefficients
   case K1:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K1", val);
   case K2:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K2", val);
   case K3:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K3", val);
   case K4:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K4", val);
   case K5:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K5", val);
   case K6:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K6", val);
   case K7:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K7", val);
   case K8:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K8", val);
   case K9:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K9", val);
   case K10:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K10", val);
   case K11:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K11", val);
   case K12:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K12", val);
   case K13:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K13", val);
   case K14:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K14", val);
   case K15:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K15", val);
   case K16:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K16", val);
      
   // Thruster ThrustDirections
   case THRUST_DIRECTION1:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection1", val);
   case THRUST_DIRECTION2:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection2", val);
   case THRUST_DIRECTION3:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection3", val);
     
   case AREA_COEFFICIENT_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "AreaCoefficient", val);
   case SPECULAR_FRACTION_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "SpecularFraction", val);
   case DIFFUSE_FRACTION_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "DiffuseFraction", val);
   
   case PLATE_NORMAL_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "PlateNormal", val);
   case AREA_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "Area", val);
   case LIT_FRACTION_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "LitFraction", val);
   
   case AREA_COEFFICIENT_SIGMA_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "AreaCoefficientSigma", val);
   case SPECULAR_FRACTION_SIGMA_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "SpecularFractionSigma", val);
   case DIFFUSE_FRACTION_SIGMA_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "DiffuseFractionSigma", val);
   case PLATEX:
      return SetOwnedObjectProperty(Gmat::PLATE, "PlateX", val);
   case PLATEY:
      return SetOwnedObjectProperty(Gmat::PLATE, "PlateY", val);
   case PLATEZ:
      return SetOwnedObjectProperty(Gmat::PLATE, "PlateZ", val);

   default:
      // otherwise, there is an error   
      throw ParameterException
         ("SpacecraftData::SetReal() Not settable or unknown item id: " +
          GmatStringUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// std::string & SpacecraftData::GetString(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves Spacecraft or spacecraft owned hardware element by integer id.
 */
 //------------------------------------------------------------------------------
std::string SpacecraftData::GetString(Integer item)
{
   if (mSpacecraft == NULL)
      InitializeRefObjects();

   // if there was an error initializing the ref objects, do not try to set the value
   if (mSpacecraft == NULL)
      return UNDEFINED_RETURN_STRING;

   switch (item)
   {
      case TYPE_ID:
         return GetOwnedStringObjectProperty(Gmat::PLATE, "Type");
      case PLATE_NORMAL_HISTORY_FILE_ID:
         return GetOwnedStringObjectProperty(Gmat::PLATE, "PlateNormalHistoryFile");
      default:
         // otherwise, there is an error   
         throw ParameterException
         ("SpacecraftData::GetString() Not gettable or unknown item id: " +
            GmatStringUtil::ToString(item));
   }
}

//------------------------------------------------------------------------------
// Real SetString(Integer item, std::string & val)
//------------------------------------------------------------------------------
/**
 * Sets Spacecraft or spacecraft owned hardware element by integer id.
 */
 //------------------------------------------------------------------------------
Real SpacecraftData::SetString(Integer item, std::string & val)
{
   if (mSpacecraft == NULL)
      InitializeRefObjects();

   // if there was an error initializing the ref objects, do not try to set the value
   if (mSpacecraft == NULL)
      return BALLISTIC_REAL_UNDEFINED;

   switch (item)
   {
   case TYPE_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "Type", val);
   case PLATE_NORMAL_HISTORY_FILE_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "PlateNormalHistoryFile", val);
   default:
      // otherwise, there is an error   
      throw ParameterException
      ("SpacecraftData::SetString() Not settable or unknown item id: " +
         GmatStringUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Rvector & SpacecraftData::GetRvector(Integer item)
//------------------------------------------------------------------------------
/**
 * Gets Spacecraft or spacecraft owned hardware element by integer id.
 */
 //------------------------------------------------------------------------------
Rvector SpacecraftData::GetRvector(Integer item)
{
   if (mSpacecraft == NULL)
      InitializeRefObjects();

   // if there was an error initializing the ref objects, do not try to set the value
   if (mSpacecraft == NULL)
      return UNDEFINED_RETURN_RVECTOR;

   switch (item)
   {
   case PLATE_NORMAL_ID:
      return GetOwnedRvectorObjectProperty(Gmat::PLATE, "PlateNormal");
   default:
      // otherwise, there is an error   
      throw ParameterException
      ("SpacecraftData::GetRvector() Not gettable or unknown item id: " +
         GmatStringUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Rvector & SpacecraftData::SetRvector(Integer item, Rvector & val)
//------------------------------------------------------------------------------
/**
 * Gets Spacecraft or spacecraft owned hardware element by integer id.
 */
 //------------------------------------------------------------------------------
Rvector SpacecraftData::SetRvector(Integer item, Rvector & val)
{
   if (mSpacecraft == NULL)
      InitializeRefObjects();

   // if there was an error initializing the ref objects, do not try to set the value
   if (mSpacecraft == NULL)
      return UNDEFINED_RETURN_RVECTOR;

   switch (item)
   {
   case PLATE_NORMAL_ID:
      return SetOwnedObjectProperty(Gmat::PLATE, "PlateNormal", val);
   default:
      // otherwise, there is an error   
      throw ParameterException
      ("SpacecraftData::SetRvector() Not settable or unknown item id: " +
         GmatStringUtil::ToString(item));
   }
}

//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* SpacecraftData::GetValidObjectList() const
{
   return VALID_OBJECT_TYPE_LIST;
}


//------------------------------------------------------------------------------
// bool ValidateRefObjects(GmatBase *param)
//------------------------------------------------------------------------------
/**
 * Validates reference objects for given parameter
 */
//------------------------------------------------------------------------------
bool SpacecraftData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<SpacecraftDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }
   
   if (objCount == SpacecraftDataObjectCount)
      return true;
   else
      return false;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void SpacecraftData::InitializeRefObjects()
{
   #ifdef DEBUG_SPACECRAFTDATA_INIT
   MessageInterface::ShowMessage
      ("SpacecraftData::InitializeRefObjects() '%s' entered.\n", mActualParamName.c_str());
   #endif
   
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   
   if (mSpacecraft == NULL)
   {
      // Just write a message since Parameters in GmatFunction may not have ref. object
      // set until execution
      #ifdef DEBUG_SPACECRAFTDATA_INIT
      MessageInterface::ShowMessage
         ("SpacecraftData::InitializeRefObjects() Cannot find Spacecraft object.\n");
      #endif
      
      //throw ParameterException
      //   ("SpacecraftData::InitializeRefObjects() Cannot find Spacecraft object.\n");
   }
   
   #ifdef DEBUG_SPACECRAFTDATA_INIT
   MessageInterface::ShowMessage
      ("SpacecraftData::InitializeRefObjects() '%s' leaving, mSpacecraft=<%p>'%s'\n",
       mActualParamName.c_str(), mSpacecraft, mSpacecraft->GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// virtual bool IsValidObjectType(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 *
 * @return return true if object is valid object, false otherwise
 */
//------------------------------------------------------------------------------
bool SpacecraftData::IsValidObjectType(UnsignedInt type)
{
   for (int i=0; i<SpacecraftDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}


//------------------------------------------------------------------------------
// Real GetOwnedObjectProperty(UnsignedInt objType, const std::string &propName)
//------------------------------------------------------------------------------
Real SpacecraftData::GetOwnedObjectProperty(UnsignedInt objType,
                                            const std::string &propName)
{
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(mActualParamName, type, owner, dep);
   
   #ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
      ("SpacecraftData::GetOwnedObjectProperty() '%s' entered, objType=%d, propName='%s'\n   "
       "type='%s', owner='%s', dep='%s'\n", mActualParamName.c_str(), objType, propName.c_str(),
       type.c_str(), owner.c_str(), dep.c_str());
   #endif
   
   GmatBase *ownedObj = mSpacecraft->GetRefObject(objType, dep);

   #ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
      ("   ownedObj=<%p><%s>'%s'\n", ownedObj, ownedObj ? ownedObj->GetTypeName().c_str() : "NULL",
       ownedObj ? ownedObj->GetName().c_str() : "NULL");
   #endif
   
   if (ownedObj == NULL)
   {
      //Integer runMode = GmatGlobal::Instance()->GetRunMode();                       // made changes by TUAN NGUYEN
      Integer runMode = GmatGlobal::Instance()->GetRunModeStartUp();                  // made changes by TUAN NGUYEN

      std::string msg = "Cannot evaluate Parameter \"" + mActualParamName + "\"; " +
         GmatBase::GetObjectTypeString(objType) + " named \"" + dep + "\" is not "
         "attached to Spacecraft \"" + mSpacecraft->GetName() + "\"";
      
      if (runMode == GmatGlobal::TESTING || runMode == GmatGlobal::TESTING_NO_PLOTS)
         MessageInterface::ShowMessage("In SpacecraftData::GetOwnedObjectProperty() " + msg + "\n");
      
      ParameterException pe(msg);
      throw pe;
   }
   else
   {
      Real result;
      // Since MassFlowRate, Thrust, and Isp are only calculated during thruster
      // firing, call specific method
      if (propName == "MassFlowRate")
         result = ((Thruster*)ownedObj)->GetMassFlowRate();
      else if (propName == "ThrustMagnitude")
         result = ((Thruster*)ownedObj)->GetThrustMagnitude();
      else if (propName == "Isp")
          result = ((Thruster*)ownedObj)->GetIsp();
      else
         result = ownedObj->GetRealParameter(propName);
      
      #ifdef DEBUG_SC_OWNED_OBJ
      MessageInterface::ShowMessage
         ("SpacecraftData::GetOwnedObjectProperty() returning %f\n", result);
      #endif
      return result;
   }
}


//------------------------------------------------------------------------------
// Real SetOwnedObjectProperty(UnsignedInt objType, const std::string &propName,
//                             Real val)
//------------------------------------------------------------------------------
Real SpacecraftData::SetOwnedObjectProperty(UnsignedInt objType,
                                            const std::string &propName,
                                            Real val)
{
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(mActualParamName, type, owner, dep);
   
   #ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
      ("SpacecraftData::SetOwnedObjectProperty() '%s' entered, objType=%d, "
       "propName='%s', val=%f, type='%s', owner='%s', dep='%s',\n",
       mActualParamName.c_str(), objType, propName.c_str(), val,
       type.c_str(), owner.c_str(), dep.c_str());
   #endif
   
   GmatBase *ownedObj = mSpacecraft->GetRefObject(objType, dep);

   #ifdef DEBUG_SETREAL
      MessageInterface::ShowMessage("   dep == %s points to %p\n", dep.c_str(),
            ownedObj);
   #endif
   
   #ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
      ("   ownedObj=<%p><%s>'%s'\n", ownedObj, ownedObj ? ownedObj->GetTypeName().c_str() : "NULL",
       ownedObj ? ownedObj->GetName().c_str() : "NULL");
   #endif
   
   if (ownedObj == NULL)
   {
      ParameterException pe;
      pe.SetDetails("SpacecraftData::SetOwnedObjectProperty() %s \"%s\" is not "
                    "attached to Spacecraft \"%s\"",
                    GmatBase::GetObjectTypeString(objType).c_str(), dep.c_str(),
                    mSpacecraft->GetName().c_str());
      throw pe;
   }
   else
   {
      Real result = ownedObj->SetRealParameter(propName, val);
      
      #ifdef DEBUG_SC_OWNED_OBJ
      MessageInterface::ShowMessage
         ("SpacecraftData::SetOwnedObjectProperty() '%s' returning %f\n",
          mActualParamName.c_str(), result);
      #endif
      return result;
   }
}

//------------------------------------------------------------------------------
// std::string & GetOwnedStringObjectProperty(UnsignedInt objType, const std::string &name)
//------------------------------------------------------------------------------
std::string SpacecraftData::GetOwnedStringObjectProperty(UnsignedInt objType, const std::string & name)
{
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(mActualParamName, type, owner, dep);

#ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
   ("SpacecraftData::GetOwnedObjectProperty() '%s' entered, objType=%d, propName='%s'\n   "
      "type='%s', owner='%s', dep='%s'\n", mActualParamName.c_str(), objType, propName.c_str(),
      type.c_str(), owner.c_str(), dep.c_str());
#endif

   GmatBase *ownedObj = mSpacecraft->GetRefObject(objType, dep);

#ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
   ("   ownedObj=<%p><%s>'%s'\n", ownedObj, ownedObj ? ownedObj->GetTypeName().c_str() : "NULL",
      ownedObj ? ownedObj->GetName().c_str() : "NULL");
#endif

   if (ownedObj == NULL)
   {
      //Integer runMode = GmatGlobal::Instance()->GetRunMode();                      // made changes by TUAN NGUYEN
      Integer runMode = GmatGlobal::Instance()->GetRunModeStartUp();                 // made changes by TUAN NGUYEN

      std::string msg = "Cannot evaluate Parameter \"" + mActualParamName + "\"; " +
         GmatBase::GetObjectTypeString(objType) + " named \"" + dep + "\" is not "
         "attached to Spacecraft \"" + mSpacecraft->GetName() + "\"";

      if (runMode == GmatGlobal::TESTING || runMode == GmatGlobal::TESTING_NO_PLOTS)
         MessageInterface::ShowMessage("In SpacecraftData::GetOwnedObjectProperty() " + msg + "\n");

      ParameterException pe(msg);
      throw pe;
   }
   else
   {
      std::string result;
      // Since MassFlowRate, Thrust, and Isp are only calculated during thruster
      // firing, call specific method
      result = ownedObj->GetStringParameter(name);

   #ifdef DEBUG_SC_OWNED_OBJ
         MessageInterface::ShowMessage
         ("SpacecraftData::GetOwnedObjectProperty() returning %f\n", result);
   #endif
      return result;
   }
}


//------------------------------------------------------------------------------
// Real SetOwnedObjectProperty(UnsignedInt objType, const std::string &name,
//                             Real val)
//------------------------------------------------------------------------------
Real SpacecraftData::SetOwnedObjectProperty(UnsignedInt objType, const std::string & name, std::string & val)
{
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(mActualParamName, type, owner, dep);

   #ifdef DEBUG_SC_OWNED_OBJ
      MessageInterface::ShowMessage
      ("SpacecraftData::SetOwnedObjectProperty() '%s' entered, objType=%d, "
         "propName='%s', val=%f, type='%s', owner='%s', dep='%s',\n",
         mActualParamName.c_str(), objType, propName.c_str(), val,
         type.c_str(), owner.c_str(), dep.c_str());
   #endif

   GmatBase *ownedObj = mSpacecraft->GetRefObject(objType, dep);

   #ifdef DEBUG_SETREAL
      MessageInterface::ShowMessage("   dep == %s points to %p\n", dep.c_str(),
         ownedObj);
   #endif

   #ifdef DEBUG_SC_OWNED_OBJ
      MessageInterface::ShowMessage
      ("   ownedObj=<%p><%s>'%s'\n", ownedObj, ownedObj ? ownedObj->GetTypeName().c_str() : "NULL",
         ownedObj ? ownedObj->GetName().c_str() : "NULL");
   #endif

   if (ownedObj == NULL)
   {
      ParameterException pe;
      pe.SetDetails("SpacecraftData::SetOwnedObjectProperty() %s \"%s\" is not "
         "attached to Spacecraft \"%s\"",
         GmatBase::GetObjectTypeString(objType).c_str(), dep.c_str(),
         mSpacecraft->GetName().c_str());
      throw pe;
   }
   else
   {
      Real result = ownedObj->SetStringParameter(name, val);

#ifdef DEBUG_SC_OWNED_OBJ
      MessageInterface::ShowMessage
      ("SpacecraftData::SetOwnedObjectProperty() '%s' returning %f\n",
         mActualParamName.c_str(), result);
#endif
      return result;
   }
}


//------------------------------------------------------------------------------
// Rvector & SpacecraftData::GetOwnedRvectorObjectProperty(UnsignedInt objType, const std::string & name)
//                             
//------------------------------------------------------------------------------
Rvector SpacecraftData::GetOwnedRvectorObjectProperty(UnsignedInt objType, const std::string & name)
{
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(mActualParamName, type, owner, dep);

#ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
   ("SpacecraftData::GetOwnedObjectProperty() '%s' entered, objType=%d, propName='%s'\n   "
      "type='%s', owner='%s', dep='%s'\n", mActualParamName.c_str(), objType, propName.c_str(),
      type.c_str(), owner.c_str(), dep.c_str());
#endif

   GmatBase *ownedObj = mSpacecraft->GetRefObject(objType, dep);

#ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
   ("   ownedObj=<%p><%s>'%s'\n", ownedObj, ownedObj ? ownedObj->GetTypeName().c_str() : "NULL",
      ownedObj ? ownedObj->GetName().c_str() : "NULL");
#endif

   if (ownedObj == NULL)
   {
      //Integer runMode = GmatGlobal::Instance()->GetRunMode();                         // made changes by TUAN NGUYEN
      Integer runMode = GmatGlobal::Instance()->GetRunModeStartUp();                    // made changes by TUAN NGUYEN

      std::string msg = "Cannot evaluate Parameter \"" + mActualParamName + "\"; " +
         GmatBase::GetObjectTypeString(objType) + " named \"" + dep + "\" is not "
         "attached to Spacecraft \"" + mSpacecraft->GetName() + "\"";

      if (runMode == GmatGlobal::TESTING || runMode == GmatGlobal::TESTING_NO_PLOTS)
         MessageInterface::ShowMessage("In SpacecraftData::GetOwnedObjectProperty() " + msg + "\n");

      ParameterException pe(msg);
      throw pe;
   }
   else
   {
      // Since MassFlowRate, Thrust, and Isp are only calculated during thruster
      // firing, call specific method
      vectorResult = ownedObj->GetRvectorParameter(name);

#ifdef DEBUG_SC_OWNED_OBJ
      MessageInterface::ShowMessage
      ("SpacecraftData::GetOwnedObjectProperty() returning %f\n", result);
#endif
      return vectorResult;
   }
}


//------------------------------------------------------------------------------
// Rvector& SpacecraftData::SetOwnedObjectProperty(UnsignedInt objType, const std::string & name, 
//                                                 Rvector & val)
//------------------------------------------------------------------------------
Rvector SpacecraftData::SetOwnedObjectProperty(UnsignedInt objType, const std::string & name, Rvector & val)
{
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(mActualParamName, type, owner, dep);

#ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
   ("SpacecraftData::SetOwnedObjectProperty() '%s' entered, objType=%d, "
      "propName='%s', val=%f, type='%s', owner='%s', dep='%s',\n",
      mActualParamName.c_str(), objType, propName.c_str(), val,
      type.c_str(), owner.c_str(), dep.c_str());
#endif

   GmatBase *ownedObj = mSpacecraft->GetRefObject(objType, dep);

#ifdef DEBUG_SETREAL
   MessageInterface::ShowMessage("   dep == %s points to %p\n", dep.c_str(),
      ownedObj);
#endif

#ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
   ("   ownedObj=<%p><%s>'%s'\n", ownedObj, ownedObj ? ownedObj->GetTypeName().c_str() : "NULL",
      ownedObj ? ownedObj->GetName().c_str() : "NULL");
#endif

   if (ownedObj == NULL)
   {
      ParameterException pe;
      pe.SetDetails("SpacecraftData::SetOwnedObjectProperty() %s \"%s\" is not "
         "attached to Spacecraft \"%s\"",
         GmatBase::GetObjectTypeString(objType).c_str(), dep.c_str(),
         mSpacecraft->GetName().c_str());
      throw pe;
   }
   else
   {
      Rvector result = ownedObj->SetRvectorParameter(name, val);

#ifdef DEBUG_SC_OWNED_OBJ
      MessageInterface::ShowMessage
      ("SpacecraftData::SetOwnedObjectProperty() '%s' returning %f\n",
         mActualParamName.c_str(), result);
#endif


      return result;
   }
}

