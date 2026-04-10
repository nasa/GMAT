//$Id$
//------------------------------------------------------------------------------
//                            File: HardwareParameters
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
 * Implements Hardware related parameter classes.
 *    FuelTank: FuelMass,
 *              FuelCenterOfMassX, FuelCenterOfMassY, FuelCenterOfMassZ,
 *              FuelMomentOfInertiaXX, FuelMomentOfInertiaXY, FuelMomentOfInertiaXZ,
 *              FuelMomentOfInertiaYY, FuelMomentOfInertiaYZ, FuelMomentOfInertiaZZ,
 *              FuelCenterOfMassX_BCS, FuelCenterOfMassY_BCS, FuelCenterOfMassZ_BCS,
 *              FuelMomentOfInertiaXX_BCS, FuelMomentOfInertiaXY_BCS, FuelMomentOfInertiaXZ_BCS,
 *              FuelMomentOfInertiaYY_BCS, FuelMomentOfInertiaYZ_BCS, FuelMomentOfInertiaZZ_BCS,
 *              Pressure, Temperature, RefTemperature, Volume,FuelDensity
 *    Thruster: DutyCycle, ThrustScaleFactor, GravitationalAccel, C1-C16,
 *              K1-K16, ThrustDirections, ThrustMagnitude, Isp, MassFlowRate
 *    PowerSystem: TotalPowerAvailable, RequiredBusPower, ThrustPowerAvailable
 */
//------------------------------------------------------------------------------

#include "HardwareParameters.hpp"
#include "ColorTypes.hpp"
#include "MessageInterface.hpp"
#include "InterpreterException.hpp"

// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

//==============================================================================
//                              FuelMass
//==============================================================================
/**
 * Implements FuelMass class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMass(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMass::FuelMass(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "FuelMass", Gmat::SPACECRAFT, Gmat::FUEL_TANK, obj,
                  "Fuel Mass", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMass(const FuelMass &copy)
//------------------------------------------------------------------------------
FuelMass::FuelMass(const FuelMass &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMass& operator=(const FuelMass &right)
//------------------------------------------------------------------------------
FuelMass& FuelMass::operator=(const FuelMass &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMass()
//------------------------------------------------------------------------------
FuelMass::~FuelMass()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMass::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MASS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMass::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MASS, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMass::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMass::Clone(void) const
{
   return new FuelMass(*this);
}


//==============================================================================
//                              FuelCenterOfMassX
//==============================================================================
/**
 * Implements FuelCenterOfMassX class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelCenterOfMassX(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelCenterOfMassX::FuelCenterOfMassX(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "FuelCenterOfMassX", Gmat::SPACECRAFT, Gmat::FUEL_TANK,
                   obj, "Fuel Center Of Mass X", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelCenterOfMassX(const FuelCenterOfMassX &copy)
//------------------------------------------------------------------------------
FuelCenterOfMassX::FuelCenterOfMassX(const FuelCenterOfMassX &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelCenterOfMassX& operator=(const FuelCenterOfMassX &right)
//------------------------------------------------------------------------------
FuelCenterOfMassX& FuelCenterOfMassX::operator=(const FuelCenterOfMassX &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelCenterOfMassX()
//------------------------------------------------------------------------------
FuelCenterOfMassX::~FuelCenterOfMassX()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelCenterOfMassX::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_CM_X);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelCenterOfMassX::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_CM_X, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelCenterOfMassX::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelCenterOfMassX::Clone(void) const
{
   return new FuelCenterOfMassX(*this);
}


//==============================================================================
//                              FuelCenterOfMassY
//==============================================================================
/**
 * Implements FuelCenterOfMassY class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelCenterOfMassY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelCenterOfMassY::FuelCenterOfMassY(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "FuelCenterOfMassY", Gmat::SPACECRAFT, Gmat::FUEL_TANK,
                  obj, "Fuel Center Of Mass Y", "")
                  
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelCenterOfMassY(const FuelCenterOfMassY &copy)
//------------------------------------------------------------------------------
FuelCenterOfMassY::FuelCenterOfMassY(const FuelCenterOfMassY &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelCenterOfMassY& operator=(const FuelCenterOfMassY &right)
//------------------------------------------------------------------------------
FuelCenterOfMassY& FuelCenterOfMassY::operator=(const FuelCenterOfMassY &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelCenterOfMassY()
//------------------------------------------------------------------------------
FuelCenterOfMassY::~FuelCenterOfMassY()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelCenterOfMassY::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_CM_Y);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelCenterOfMassY::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_CM_Y, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelCenterOfMassY::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelCenterOfMassY::Clone(void) const
{
   return new FuelCenterOfMassY(*this);
}


//==============================================================================
//                              FuelCenterOfMassZ
//==============================================================================
/**
 * Implements FuelCenterOfMassZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelCenterOfMassZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelCenterOfMassZ::FuelCenterOfMassZ(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "FuelCenterOfMassZ", Gmat::SPACECRAFT, Gmat::FUEL_TANK,
                  obj, "Fuel Center Of Mass Z", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelCenterOfMassZ(const FuelCenterOfMassZ &copy)
//------------------------------------------------------------------------------
FuelCenterOfMassZ::FuelCenterOfMassZ(const FuelCenterOfMassZ &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelCenterOfMassZ& operator=(const FuelCenterOfMassZ &right)
//------------------------------------------------------------------------------
FuelCenterOfMassZ& FuelCenterOfMassZ::operator=(const FuelCenterOfMassZ &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelCenterOfMassZ()
//------------------------------------------------------------------------------
FuelCenterOfMassZ::~FuelCenterOfMassZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelCenterOfMassZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_CM_Z);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelCenterOfMassZ::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_CM_Z, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelCenterOfMassZ::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelCenterOfMassZ::Clone(void) const
{
   return new FuelCenterOfMassZ(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaXX
//==============================================================================
/**
 * Implements FuelMomentOfInertiaXX class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaXX(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXX::FuelMomentOfInertiaXX(const std::string &name,
                                             GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaXX", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia XX", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXX(const FuelMomentOfInertiaXX &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXX::FuelMomentOfInertiaXX(const FuelMomentOfInertiaXX &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXX& operator=(const FuelMomentOfInertiaXX &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXX& FuelMomentOfInertiaXX::operator=(const FuelMomentOfInertiaXX &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaXX()
//------------------------------------------------------------------------------
FuelMomentOfInertiaXX::~FuelMomentOfInertiaXX()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaXX::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_XX);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaXX::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_XX, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaXX::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaXX::Clone(void) const
{
   return new FuelMomentOfInertiaXX(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaXY
//==============================================================================
/**
 * Implements FuelMomentOfInertiaXY class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaXY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXY::FuelMomentOfInertiaXY(const std::string &name,
                                             GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaXY", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia XY", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXY(const FuelMomentOfInertiaXY &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXY::FuelMomentOfInertiaXY(const FuelMomentOfInertiaXY &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXY& operator=(const FuelMomentOfInertiaXY &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXY& FuelMomentOfInertiaXY::operator=(const FuelMomentOfInertiaXY &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaXY()
//------------------------------------------------------------------------------
FuelMomentOfInertiaXY::~FuelMomentOfInertiaXY()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaXY::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_XY);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaXY::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_XY, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaXY::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaXY::Clone(void) const
{
   return new FuelMomentOfInertiaXY(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaXZ
//==============================================================================
/**
 * Implements FuelMomentOfInertiaXZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaXZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXZ::FuelMomentOfInertiaXZ(const std::string &name,
                                             GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaXZ", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia XZ", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXZ(const FuelMomentOfInertiaXZ &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXZ::FuelMomentOfInertiaXZ(const FuelMomentOfInertiaXZ &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXZ& operator=(const FuelMomentOfInertiaXZ &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXZ& FuelMomentOfInertiaXZ::operator=(const FuelMomentOfInertiaXZ &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaXZ()
//------------------------------------------------------------------------------
FuelMomentOfInertiaXZ::~FuelMomentOfInertiaXZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaXZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_XZ);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaXZ::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_XZ, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaXZ::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaXZ::Clone(void) const
{
   return new FuelMomentOfInertiaXZ(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaYY
//==============================================================================
/**
 * Implements FuelMomentOfInertiaYY class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaYY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYY::FuelMomentOfInertiaYY(const std::string &name,
                                             GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaYY", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia YY", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaYY(const FuelMomentOfInertiaYY &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYY::FuelMomentOfInertiaYY(const FuelMomentOfInertiaYY &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaYY& operator=(const FuelMomentOfInertiaYY &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYY& FuelMomentOfInertiaYY::operator=(const FuelMomentOfInertiaYY &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaYY()
//------------------------------------------------------------------------------
FuelMomentOfInertiaYY::~FuelMomentOfInertiaYY()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaYY::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_YY);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaYY::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_YY, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaYY::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaYY::Clone(void) const
{
   return new FuelMomentOfInertiaYY(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaYZ
//==============================================================================
/**
 * Implements FuelMomentOfInertiaYZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaYZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYZ::FuelMomentOfInertiaYZ(const std::string &name,
                                             GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaYZ", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia YZ", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaYZ(const FuelMomentOfInertiaYZ &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYZ::FuelMomentOfInertiaYZ(const FuelMomentOfInertiaYZ &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaYZ& operator=(const FuelMomentOfInertiaYZ &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYZ& FuelMomentOfInertiaYZ::operator=(const FuelMomentOfInertiaYZ &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaYZ()
//------------------------------------------------------------------------------
FuelMomentOfInertiaYZ::~FuelMomentOfInertiaYZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaYZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_YZ);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaYZ::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_YZ, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaYZ::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaYZ::Clone(void) const
{
   return new FuelMomentOfInertiaYZ(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaZZ
//==============================================================================
/**
 * Implements FuelMomentOfInertiaZZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaZZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaZZ::FuelMomentOfInertiaZZ(const std::string &name,
                                             GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaZZ", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia ZZ", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaZZ(const FuelMomentOfInertiaZZ &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaZZ::FuelMomentOfInertiaZZ(const FuelMomentOfInertiaZZ &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaZZ& operator=(const FuelMomentOfInertiaZZ &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaZZ& FuelMomentOfInertiaZZ::operator=(const FuelMomentOfInertiaZZ &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaZZ()
//------------------------------------------------------------------------------
FuelMomentOfInertiaZZ::~FuelMomentOfInertiaZZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaZZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_ZZ);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaZZ::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_ZZ, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaZZ::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaZZ::Clone(void) const
{
   return new FuelMomentOfInertiaZZ(*this);
}


//==============================================================================
//                              FuelCenterOfMassX_BCS
//==============================================================================
/**
 * Implements FuelCenterOfMassX_BCS class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelCenterOfMassX_BCS(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelCenterOfMassX_BCS::FuelCenterOfMassX_BCS(const std::string &name,
                                             GmatBase *obj)
   : HardwareReal(name, "FuelCenterOfMassX_BCS", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Center Of Mass X_BCS", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelCenterOfMassX_BCS(const FuelCenterOfMassX_BCS &copy)
//------------------------------------------------------------------------------
FuelCenterOfMassX_BCS::FuelCenterOfMassX_BCS(const FuelCenterOfMassX_BCS &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelCenterOfMassX_BCS& operator=(const FuelCenterOfMassX_BCS &right)
//------------------------------------------------------------------------------
FuelCenterOfMassX_BCS& FuelCenterOfMassX_BCS::operator=(const FuelCenterOfMassX_BCS &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelCenterOfMassX_BCS()
//------------------------------------------------------------------------------
FuelCenterOfMassX_BCS::~FuelCenterOfMassX_BCS()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelCenterOfMassX_BCS::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_CM_X_BCS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelCenterOfMassX_BCS::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_CM_X_BCS, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelCenterOfMassX_BCS::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelCenterOfMassX_BCS::Clone(void) const
{
   return new FuelCenterOfMassX_BCS(*this);
}


//==============================================================================
//                              FuelCenterOfMassY_BCS
//==============================================================================
/**
 * Implements FuelCenterOfMassY_BCS class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelCenterOfMassY_BCS(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelCenterOfMassY_BCS::FuelCenterOfMassY_BCS(const std::string &name,
                                             GmatBase *obj)
   : HardwareReal(name, "FuelCenterOfMassY_BCS", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Center Of Mass Y_BCS", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelCenterOfMassY_BCS(const FuelCenterOfMassY_BCS &copy)
//------------------------------------------------------------------------------
FuelCenterOfMassY_BCS::FuelCenterOfMassY_BCS(const FuelCenterOfMassY_BCS &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelCenterOfMassY_BCS& operator=(const FuelCenterOfMassY_BCS &right)
//------------------------------------------------------------------------------
FuelCenterOfMassY_BCS& FuelCenterOfMassY_BCS::operator=(const FuelCenterOfMassY_BCS &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelCenterOfMassY_BCS()
//------------------------------------------------------------------------------
FuelCenterOfMassY_BCS::~FuelCenterOfMassY_BCS()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelCenterOfMassY_BCS::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_CM_Y_BCS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelCenterOfMassY_BCS::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_CM_Y_BCS, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelCenterOfMassY_BCS::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelCenterOfMassY_BCS::Clone(void) const
{
   return new FuelCenterOfMassY_BCS(*this);
}


//==============================================================================
//                              FuelCenterOfMassZ_BCS
//==============================================================================
/**
 * Implements FuelCenterOfMassZ_BCS class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelCenterOfMassZ_BCS(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelCenterOfMassZ_BCS::FuelCenterOfMassZ_BCS(const std::string &name,
                                             GmatBase *obj)
   : HardwareReal(name, "FuelCenterOfMassZ_BCS", Gmat::SPACECRAFT, 
                  Gmat::FUEL_TANK, obj, "Fuel Center Of Mass Z", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelCenterOfMassZ_BCS(const FuelCenterOfMassZ_BCS &copy)
//------------------------------------------------------------------------------
FuelCenterOfMassZ_BCS::FuelCenterOfMassZ_BCS(const FuelCenterOfMassZ_BCS &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelCenterOfMassZ_BCS& operator=(const FuelCenterOfMassZ_BCS &right)
//------------------------------------------------------------------------------
FuelCenterOfMassZ_BCS& FuelCenterOfMassZ_BCS::operator=(const FuelCenterOfMassZ_BCS &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelCenterOfMassZ_BCS()
//------------------------------------------------------------------------------
FuelCenterOfMassZ_BCS::~FuelCenterOfMassZ_BCS()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelCenterOfMassZ_BCS::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_CM_Z_BCS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelCenterOfMassZ_BCS::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_CM_Z_BCS, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelCenterOfMassZ_BCS::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelCenterOfMassZ_BCS::Clone(void) const
{
   return new FuelCenterOfMassZ_BCS(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaXX_BCS
//==============================================================================
/**
 * Implements FuelMomentOfInertiaXX_BCS class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaXX_BCS(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXX_BCS::FuelMomentOfInertiaXX_BCS(const std::string &name,
                                                     GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaXX_BCS", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia XX_BCS", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXX_BCS(const FuelMomentOfInertiaXX_BCS &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXX_BCS::FuelMomentOfInertiaXX_BCS(const FuelMomentOfInertiaXX_BCS &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXX_BCS& operator=(const FuelMomentOfInertiaXX_BCS &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXX_BCS& FuelMomentOfInertiaXX_BCS::operator=(const FuelMomentOfInertiaXX_BCS &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaXX_BCS()
//------------------------------------------------------------------------------
FuelMomentOfInertiaXX_BCS::~FuelMomentOfInertiaXX_BCS()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaXX_BCS::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_XX_BCS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaXX_BCS::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_XX_BCS, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaXX_BCS::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaXX_BCS::Clone(void) const
{
   return new FuelMomentOfInertiaXX_BCS(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaXY_BCS
//==============================================================================
/**
 * Implements FuelMomentOfInertiaXY_BCS class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaXY_BCS(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXY_BCS::FuelMomentOfInertiaXY_BCS(const std::string &name,
                                                     GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaXY_BCS", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia XY_BCS", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXY_BCS(const FuelMomentOfInertiaXY_BCS &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXY_BCS::FuelMomentOfInertiaXY_BCS(const FuelMomentOfInertiaXY_BCS &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXY_BCS& operator=(const FuelMomentOfInertiaXY_BCS &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXY_BCS& FuelMomentOfInertiaXY_BCS::operator=(const FuelMomentOfInertiaXY_BCS &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaXY_BCS()
//------------------------------------------------------------------------------
FuelMomentOfInertiaXY_BCS::~FuelMomentOfInertiaXY_BCS()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaXY_BCS::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_XY_BCS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaXY_BCS::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_XY_BCS, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaXY_BCS::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaXY_BCS::Clone(void) const
{
   return new FuelMomentOfInertiaXY_BCS(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaXZ_BCS
//==============================================================================
/**
 * Implements FuelMomentOfInertiaXZ_BCS class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaXZ_BCS(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXZ_BCS::FuelMomentOfInertiaXZ_BCS(const std::string &name,
                                                     GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaXZ_BCS", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia XZ_BCS", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXZ_BCS(const FuelMomentOfInertiaXZ_BCS &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXZ_BCS::FuelMomentOfInertiaXZ_BCS(const FuelMomentOfInertiaXZ_BCS &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaXZ_BCS& operator=(const FuelMomentOfInertiaXZ_BCS &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaXZ_BCS& FuelMomentOfInertiaXZ_BCS::operator=(const FuelMomentOfInertiaXZ_BCS &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaXZ_BCS()
//------------------------------------------------------------------------------
FuelMomentOfInertiaXZ_BCS::~FuelMomentOfInertiaXZ_BCS()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaXZ_BCS::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_XZ_BCS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaXZ_BCS::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_XZ_BCS, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaXZ_BCS::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaXZ_BCS::Clone(void) const
{
   return new FuelMomentOfInertiaXZ_BCS(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaYY_BCS
//==============================================================================
/**
 * Implements FuelMomentOfInertiaYY_BCS class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaYY_BCS(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYY_BCS::FuelMomentOfInertiaYY_BCS(const std::string &name,
                                                     GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaYY_BCS", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia YY_BCS", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaYY_BCS(const FuelMomentOfInertiaYY_BCS &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYY_BCS::FuelMomentOfInertiaYY_BCS(const FuelMomentOfInertiaYY_BCS &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaYY_BCS& operator=(const FuelMomentOfInertiaYY_BCS &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYY_BCS& FuelMomentOfInertiaYY_BCS::operator=(const FuelMomentOfInertiaYY_BCS &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaYY_BCS()
//------------------------------------------------------------------------------
FuelMomentOfInertiaYY_BCS::~FuelMomentOfInertiaYY_BCS()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaYY_BCS::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_YY_BCS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaYY_BCS::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_YY_BCS, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaYY_BCS::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaYY_BCS::Clone(void) const
{
   return new FuelMomentOfInertiaYY_BCS(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaYZ_BCS
//==============================================================================
/**
 * Implements FuelMomentOfInertiaYZ_BCS class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaYZ_BCS(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYZ_BCS::FuelMomentOfInertiaYZ_BCS(const std::string &name,
                                                     GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaYZ_BCS", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia YZ_BCS", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaYZ_BCS(const FuelMomentOfInertiaYZ_BCS &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYZ_BCS::FuelMomentOfInertiaYZ_BCS(const FuelMomentOfInertiaYZ_BCS &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaYZ_BCS& operator=(const FuelMomentOfInertiaYZ_BCS &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaYZ_BCS& FuelMomentOfInertiaYZ_BCS::operator=(const FuelMomentOfInertiaYZ_BCS &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaYZ_BCS()
//------------------------------------------------------------------------------
FuelMomentOfInertiaYZ_BCS::~FuelMomentOfInertiaYZ_BCS()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaYZ_BCS::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_YZ_BCS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaYZ_BCS::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_YZ_BCS, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaYZ_BCS::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaYZ_BCS::Clone(void) const
{
   return new FuelMomentOfInertiaYZ_BCS(*this);
}


//==============================================================================
//                              FuelMomentOfInertiaZZ_BCS
//==============================================================================
/**
 * Implements FuelMomentOfInertiaZZ_BCS class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelMomentOfInertiaZZ_BCS(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelMomentOfInertiaZZ_BCS::FuelMomentOfInertiaZZ_BCS(const std::string &name,
                                                     GmatBase *obj)
   : HardwareReal(name, "FuelMomentOfInertiaZZ_BCS", Gmat::SPACECRAFT,
                  Gmat::FUEL_TANK, obj, "Fuel Moment Of Inertia ZZ_BCS", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaZZ_BCS(const FuelMomentOfInertiaZZ_BCS &copy)
//------------------------------------------------------------------------------
FuelMomentOfInertiaZZ_BCS::FuelMomentOfInertiaZZ_BCS(const FuelMomentOfInertiaZZ_BCS &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelMomentOfInertiaZZ_BCS& operator=(const FuelMomentOfInertiaZZ_BCS &right)
//------------------------------------------------------------------------------
FuelMomentOfInertiaZZ_BCS& FuelMomentOfInertiaZZ_BCS::operator=(const FuelMomentOfInertiaZZ_BCS &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~FuelMomentOfInertiaZZ_BCS()
//------------------------------------------------------------------------------
FuelMomentOfInertiaZZ_BCS::~FuelMomentOfInertiaZZ_BCS()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelMomentOfInertiaZZ_BCS::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_MOI_ZZ_BCS);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelMomentOfInertiaZZ_BCS::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_MOI_ZZ_BCS, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelMomentOfInertiaZZ_BCS::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelMomentOfInertiaZZ_BCS::Clone(void) const
{
   return new FuelMomentOfInertiaZZ_BCS(*this);
}


//==============================================================================
//                              Pressure
//==============================================================================
/**
 * Implements Pressure class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Pressure(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
Pressure::Pressure(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "Pressure", Gmat::SPACECRAFT, Gmat::CHEMICAL_FUEL_TANK, obj,
                  "Pressure", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW32;
   #endif
}


//------------------------------------------------------------------------------
// Pressure(const Pressure &copy)
//------------------------------------------------------------------------------
Pressure::Pressure(const Pressure &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// Pressure& operator=(const Pressure &right)
//------------------------------------------------------------------------------
Pressure& Pressure::operator=(const Pressure &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~Pressure()
//------------------------------------------------------------------------------
Pressure::~Pressure()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Pressure::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(PRESSURE);
    
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void Pressure::SetReal(Real val)
{
   SpacecraftData::SetReal(PRESSURE, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* Pressure::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Pressure::Clone(void) const
{
   return new Pressure(*this);
}


//==============================================================================
//                              Temperature
//==============================================================================
/**
 * Implements Temperature class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Temperature(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
Temperature::Temperature(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "Temperature", Gmat::SPACECRAFT, Gmat::CHEMICAL_FUEL_TANK, obj,
                  "Temperature", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}


//------------------------------------------------------------------------------
// Temperature(const Temperature &copy)
//------------------------------------------------------------------------------
Temperature::Temperature(const Temperature &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// Temperature& operator=(const Temperature &right)
//------------------------------------------------------------------------------
Temperature& Temperature::operator=(const Temperature &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~Temperature()
//------------------------------------------------------------------------------
Temperature::~Temperature()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Temperature::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(TEMPERATURE);
    
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void Temperature::SetReal(Real val)
{
   SpacecraftData::SetReal(TEMPERATURE, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* Temperature::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Temperature::Clone(void) const
{
   return new Temperature(*this);
}


//==============================================================================
//                              RefTemperature
//==============================================================================
/**
 * Implements RefTemperature class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RefTemperature(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
RefTemperature::RefTemperature(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "RefTemperature", Gmat::SPACECRAFT, Gmat::CHEMICAL_FUEL_TANK, obj,
                  "Ref. Temperature", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}


//------------------------------------------------------------------------------
// RefTemperature(const RefTemperature &copy)
//------------------------------------------------------------------------------
RefTemperature::RefTemperature(const RefTemperature &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// RefTemperature& operator=(const RefTemperature &right)
//------------------------------------------------------------------------------
RefTemperature& RefTemperature::operator=(const RefTemperature &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~RefTemperature()
//------------------------------------------------------------------------------
RefTemperature::~RefTemperature()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool RefTemperature::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(REF_TEMPERATURE);
    
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void RefTemperature::SetReal(Real val)
{
   SpacecraftData::SetReal(REF_TEMPERATURE, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* RefTemperature::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* RefTemperature::Clone(void) const
{
   return new RefTemperature(*this);
}


//==============================================================================
//                              Volume
//==============================================================================
/**
 * Implements Volume class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Volume(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
Volume::Volume(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "Volume", Gmat::SPACECRAFT, Gmat::CHEMICAL_FUEL_TANK, obj,
                  "Fuel Volume", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}


//------------------------------------------------------------------------------
// Volume(const Volume &copy)
//------------------------------------------------------------------------------
Volume::Volume(const Volume &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// Volume& operator=(const Volume &right)
//------------------------------------------------------------------------------
Volume& Volume::operator=(const Volume &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~Volume()
//------------------------------------------------------------------------------
Volume::~Volume()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Volume::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(VOLUME);
    
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void Volume::SetReal(Real val)
{
   SpacecraftData::SetReal(VOLUME, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* Volume::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Volume::Clone(void) const
{
   return new Volume(*this);
}


//==============================================================================
//                              FuelDensity
//==============================================================================
/**
 * Implements FuelDensity class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FuelDensity(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
FuelDensity::FuelDensity(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "FuelDensity", Gmat::SPACECRAFT, Gmat::CHEMICAL_FUEL_TANK, obj,
                  "Fuel Density", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::ORANGE32;
   #endif
}


//------------------------------------------------------------------------------
// FuelDensity(const FuelDensity &copy)
//------------------------------------------------------------------------------
FuelDensity::FuelDensity(const FuelDensity &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// FuelDensity& operator=(const FuelDensity &right)
//------------------------------------------------------------------------------
FuelDensity& FuelDensity::operator=(const FuelDensity &right)
{
   if (this != &right)
      HardwareReal::operator=(right);
   
   return *this;
}


//------------------------------------------------------------------------------
// ~FuelDensity()
//------------------------------------------------------------------------------
FuelDensity::~FuelDensity()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool FuelDensity::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(FUEL_DENSITY);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void FuelDensity::SetReal(Real val)
{
   SpacecraftData::SetReal(FUEL_DENSITY, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* FuelDensity::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* FuelDensity::Clone(void) const
{
   return new FuelDensity(*this);
}

//==============================================================================
//                              R_SB11
//==============================================================================
/**
 * Implements R_SB11 class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// R_SB11(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
R_SB11::R_SB11(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "R_SB11", Gmat::SPACECRAFT, Gmat::FUEL_TANK, obj,
                  "R_SB11", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// R_SB11(const R_SB11 &copy)
//------------------------------------------------------------------------------
R_SB11::R_SB11(const R_SB11 &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// R_SB11& operator=(const R_SB11 &right)
//------------------------------------------------------------------------------
R_SB11& R_SB11::operator=(const R_SB11 &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~R_SB11()
//------------------------------------------------------------------------------
R_SB11::~R_SB11()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool R_SB11::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(HW_R_SB_11);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void R_SB11::SetReal(Real val)
{
   SpacecraftData::SetReal(HW_R_SB_11, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* R_SB11::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* R_SB11::Clone(void) const
{
   return new R_SB11(*this);
}

//==============================================================================
//                              R_SB12
//==============================================================================
/**
 * Implements R_SB12 class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// R_SB12(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
R_SB12::R_SB12(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "R_SB12", Gmat::SPACECRAFT, Gmat::FUEL_TANK, obj,
                  "R_SB12", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// R_SB12(const R_SB12 &copy)
//------------------------------------------------------------------------------
R_SB12::R_SB12(const R_SB12 &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// R_SB12& operator=(const R_SB12 &right)
//------------------------------------------------------------------------------
R_SB12& R_SB12::operator=(const R_SB12 &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~R_SB12()
//------------------------------------------------------------------------------
R_SB12::~R_SB12()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool R_SB12::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(HW_R_SB_12);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void R_SB12::SetReal(Real val)
{
   SpacecraftData::SetReal(HW_R_SB_12, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* R_SB12::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* R_SB12::Clone(void) const
{
   return new R_SB12(*this);
}


//==============================================================================
//                              R_SB13
//==============================================================================
/**
 * Implements R_SB13 class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// R_SB13(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
R_SB13::R_SB13(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "R_SB13", Gmat::SPACECRAFT, Gmat::FUEL_TANK, obj,
                  "R_SB13", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// R_SB13(const R_SB13 &copy)
//------------------------------------------------------------------------------
R_SB13::R_SB13(const R_SB13 &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// R_SB13& operator=(const R_SB13 &right)
//------------------------------------------------------------------------------
R_SB13& R_SB13::operator=(const R_SB13 &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~R_SB13()
//------------------------------------------------------------------------------
R_SB13::~R_SB13()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool R_SB13::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(HW_R_SB_13);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void R_SB13::SetReal(Real val)
{
   SpacecraftData::SetReal(HW_R_SB_13, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* R_SB13::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* R_SB13::Clone(void) const
{
   return new R_SB13(*this);
}


//==============================================================================
//                              R_SB21
//==============================================================================
/**
 * Implements R_SB21 class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// R_SB21(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
R_SB21::R_SB21(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "R_SB21", Gmat::SPACECRAFT, Gmat::FUEL_TANK, obj,
                  "R_SB21", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// R_SB21(const R_SB21 &copy)
//------------------------------------------------------------------------------
R_SB21::R_SB21(const R_SB21 &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// R_SB21& operator=(const R_SB21 &right)
//------------------------------------------------------------------------------
R_SB21& R_SB21::operator=(const R_SB21 &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~R_SB21()
//------------------------------------------------------------------------------
R_SB21::~R_SB21()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool R_SB21::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(HW_R_SB_21);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void R_SB21::SetReal(Real val)
{
   SpacecraftData::SetReal(HW_R_SB_21, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* R_SB21::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* R_SB21::Clone(void) const
{
   return new R_SB21(*this);
}

//==============================================================================
//                              R_SB22
//==============================================================================
/**
 * Implements R_SB22 class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// R_SB22(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
R_SB22::R_SB22(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "R_SB22", Gmat::SPACECRAFT, Gmat::FUEL_TANK, obj,
                  "R_SB22", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// R_SB22(const R_SB22 &copy)
//------------------------------------------------------------------------------
R_SB22::R_SB22(const R_SB22 &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// R_SB22& operator=(const R_SB22 &right)
//------------------------------------------------------------------------------
R_SB22& R_SB22::operator=(const R_SB22 &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~R_SB22()
//------------------------------------------------------------------------------
R_SB22::~R_SB22()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool R_SB22::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(HW_R_SB_22);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void R_SB22::SetReal(Real val)
{
   SpacecraftData::SetReal(HW_R_SB_22, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* R_SB22::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* R_SB22::Clone(void) const
{
   return new R_SB22(*this);
}


//==============================================================================
//                              R_SB23
//==============================================================================
/**
 * Implements R_SB23 class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// R_SB23(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
R_SB23::R_SB23(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "R_SB23", Gmat::SPACECRAFT, Gmat::FUEL_TANK, obj,
                  "R_SB23", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// R_SB23(const R_SB23 &copy)
//------------------------------------------------------------------------------
R_SB23::R_SB23(const R_SB23 &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// R_SB23& operator=(const R_SB23 &right)
//------------------------------------------------------------------------------
R_SB23& R_SB23::operator=(const R_SB23 &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~R_SB23()
//------------------------------------------------------------------------------
R_SB23::~R_SB23()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool R_SB23::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(HW_R_SB_23);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void R_SB23::SetReal(Real val)
{
   SpacecraftData::SetReal(HW_R_SB_23, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* R_SB23::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* R_SB23::Clone(void) const
{
   return new R_SB23(*this);
}


//==============================================================================
//                              R_SB31
//==============================================================================
/**
 * Implements R_SB31 class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// R_SB31(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
R_SB31::R_SB31(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "R_SB31", Gmat::SPACECRAFT, Gmat::FUEL_TANK, obj,
                  "R_SB31", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// R_SB31(const R_SB31 &copy)
//------------------------------------------------------------------------------
R_SB31::R_SB31(const R_SB31 &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// R_SB31& operator=(const R_SB31 &right)
//------------------------------------------------------------------------------
R_SB31& R_SB31::operator=(const R_SB31 &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~R_SB31()
//------------------------------------------------------------------------------
R_SB31::~R_SB31()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool R_SB31::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(HW_R_SB_31);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void R_SB31::SetReal(Real val)
{
   SpacecraftData::SetReal(HW_R_SB_31, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* R_SB31::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* R_SB31::Clone(void) const
{
   return new R_SB31(*this);
}

//==============================================================================
//                              R_SB32
//==============================================================================
/**
 * Implements R_SB32 class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// R_SB32(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
R_SB32::R_SB32(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "R_SB32", Gmat::SPACECRAFT, Gmat::FUEL_TANK, obj,
                  "R_SB32", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// R_SB32(const R_SB32 &copy)
//------------------------------------------------------------------------------
R_SB32::R_SB32(const R_SB32 &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// R_SB32& operator=(const R_SB32 &right)
//------------------------------------------------------------------------------
R_SB32& R_SB32::operator=(const R_SB32 &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~R_SB32()
//------------------------------------------------------------------------------
R_SB32::~R_SB32()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool R_SB32::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(HW_R_SB_32);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void R_SB32::SetReal(Real val)
{
   SpacecraftData::SetReal(HW_R_SB_32, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* R_SB32::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* R_SB32::Clone(void) const
{
   return new R_SB32(*this);
}


//==============================================================================
//                              R_SB33
//==============================================================================
/**
 * Implements R_SB33 class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// R_SB33(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
R_SB33::R_SB33(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "R_SB33", Gmat::SPACECRAFT, Gmat::FUEL_TANK, obj,
                  "R_SB33", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// R_SB33(const R_SB33 &copy)
//------------------------------------------------------------------------------
R_SB33::R_SB33(const R_SB33 &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// R_SB33& operator=(const R_SB33 &right)
//------------------------------------------------------------------------------
R_SB33& R_SB33::operator=(const R_SB33 &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~R_SB33()
//------------------------------------------------------------------------------
R_SB33::~R_SB33()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool R_SB33::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(HW_R_SB_33);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void R_SB33::SetReal(Real val)
{
   SpacecraftData::SetReal(HW_R_SB_33, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* R_SB33::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* R_SB33::Clone(void) const
{
   return new R_SB33(*this);
}


//==============================================================================
//                              DutyCycle
//==============================================================================
/**
 * Implements DutyCycle class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DutyCycle(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DutyCycle::DutyCycle(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "DutyCycle", Gmat::SPACECRAFT, Gmat::THRUSTER, obj,
                  "Duty Cycle", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
}


//------------------------------------------------------------------------------
// DutyCycle(const DutyCycle &copy)
//------------------------------------------------------------------------------
DutyCycle::DutyCycle(const DutyCycle &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// DutyCycle& operator=(const DutyCycle &right)
//------------------------------------------------------------------------------
DutyCycle& DutyCycle::operator=(const DutyCycle &right)
{
   if (this != &right)
      HardwareReal::operator=(right);
   
   return *this;
}


//------------------------------------------------------------------------------
// ~DutyCycle()
//------------------------------------------------------------------------------
DutyCycle::~DutyCycle()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DutyCycle::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DUTY_CYCLE);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void DutyCycle::SetReal(Real val)
{
   SpacecraftData::SetReal(DUTY_CYCLE, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* DutyCycle::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DutyCycle::Clone(void) const
{
   return new DutyCycle(*this);
}


//==============================================================================
//                              ThrustScaleFactor
//==============================================================================
/**
 * Implements ThrustScaleFactor class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ThrustScaleFactor(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ThrustScaleFactor::ThrustScaleFactor(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "ThrustScaleFactor", Gmat::SPACECRAFT, Gmat::THRUSTER, obj,
                  "Thrust Scale Factor", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
}


//------------------------------------------------------------------------------
// ThrustScaleFactor(const ThrustScaleFactor &copy)
//------------------------------------------------------------------------------
ThrustScaleFactor::ThrustScaleFactor(const ThrustScaleFactor &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// ThrustScaleFactor& operator=(const ThrustScaleFactor &right)
//------------------------------------------------------------------------------
ThrustScaleFactor& ThrustScaleFactor::operator=(const ThrustScaleFactor &right)
{
   if (this != &right)
      HardwareReal::operator=(right);
   
   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustScaleFactor()
//------------------------------------------------------------------------------
ThrustScaleFactor::~ThrustScaleFactor()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustScaleFactor::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(THRUSTER_SCALE_FACTOR);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void ThrustScaleFactor::SetReal(Real val)
{
   SpacecraftData::SetReal(THRUSTER_SCALE_FACTOR, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* ThrustScaleFactor::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ThrustScaleFactor::Clone(void) const
{
   return new ThrustScaleFactor(*this);
}


//==============================================================================
//                              GravitationalAccel
//==============================================================================
/**
 * Implements GravitationalAccel class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GravitationalAccel(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
GravitationalAccel::GravitationalAccel(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "GravitationalAccel", Gmat::SPACECRAFT, Gmat::THRUSTER, obj,
                  "Gravitational Acceleration", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
}


//------------------------------------------------------------------------------
// GravitationalAccel(const GravitationalAccel &copy)
//------------------------------------------------------------------------------
GravitationalAccel::GravitationalAccel(const GravitationalAccel &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// GravitationalAccel& operator=(const GravitationalAccel &right)
//------------------------------------------------------------------------------
GravitationalAccel& GravitationalAccel::operator=(const GravitationalAccel &right)
{
   if (this != &right)
      HardwareReal::operator=(right);
   
   return *this;
}


//------------------------------------------------------------------------------
// ~GravitationalAccel()
//------------------------------------------------------------------------------
GravitationalAccel::~GravitationalAccel()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool GravitationalAccel::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(GRAVITATIONAL_ACCEL);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void GravitationalAccel::SetReal(Real val)
{
   SpacecraftData::SetReal(GRAVITATIONAL_ACCEL, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* GravitationalAccel::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* GravitationalAccel::Clone(void) const
{
   return new GravitationalAccel(*this);
}


//==============================================================================
//                              ThrustCoefficients
//==============================================================================
/**
 * Implements ThrustCoefficients class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ThrustCoefficients(const std::string &type, const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ThrustCoefficients::ThrustCoefficients(const std::string &type,
                                       const std::string &name, GmatBase *obj)
   : HardwareReal(name, type, Gmat::SPACECRAFT, Gmat::CHEMICAL_THRUSTER, obj,
                  "Thrust Coefficient " + type, "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
         
   if      (type == "C1" )  mThrustCoeffId = C1;
   else if (type == "C2" )  mThrustCoeffId = C2;
   else if (type == "C3" )  mThrustCoeffId = C3;
   else if (type == "C4" )  mThrustCoeffId = C4;
   else if (type == "C5" )  mThrustCoeffId = C5;
   else if (type == "C6" )  mThrustCoeffId = C6;
   else if (type == "C7" )  mThrustCoeffId = C7;
   else if (type == "C8" )  mThrustCoeffId = C8;
   else if (type == "C9" )  mThrustCoeffId = C9;
   else if (type == "C10")  mThrustCoeffId = C10;
   else if (type == "C11")  mThrustCoeffId = C11;
   else if (type == "C12")  mThrustCoeffId = C12;
   else if (type == "C13")  mThrustCoeffId = C13;
   else if (type == "C14")  mThrustCoeffId = C14;
   else if (type == "C15")  mThrustCoeffId = C15;
   else if (type == "C16")  mThrustCoeffId = C16;
   else
      mThrustCoeffId = -1;
   
   #ifdef DEBUG_THRUST_COEFF
   MessageInterface::ShowMessage
      ("ThrustCoefficients::ThrustCoefficients() type='%s', name='%s', "
       "mThrustCoeffId=%d, mDesc='%s'\n", type.c_str(), name.c_str(),
       mThrustCoeffId, mDesc.c_str());
   #endif
}


//------------------------------------------------------------------------------
// ThrustCoefficients(const ThrustCoefficients &copy)
//------------------------------------------------------------------------------
ThrustCoefficients::ThrustCoefficients(const ThrustCoefficients &copy)
   : HardwareReal(copy)
{
   mThrustCoeffId = copy.mThrustCoeffId;
}


//------------------------------------------------------------------------------
// ThrustCoefficients& operator=(const ThrustCoefficients &right)
//------------------------------------------------------------------------------
ThrustCoefficients& ThrustCoefficients::operator=(const ThrustCoefficients &right)
{
   if (this != &right)
   {
      HardwareReal::operator=(right);
      mThrustCoeffId = right.mThrustCoeffId;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustCoefficients()
//------------------------------------------------------------------------------
ThrustCoefficients::~ThrustCoefficients()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustCoefficients::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(mThrustCoeffId);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void ThrustCoefficients::SetReal(Real val)
{
   SpacecraftData::SetReal(mThrustCoeffId, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* ThrustCoefficients::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ThrustCoefficients::Clone(void) const
{
   return new ThrustCoefficients(*this);
}


//==============================================================================
//                              ImpulseCoefficients
//==============================================================================
/**
 * Implements ImpulseCoefficients class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ImpulseCoefficients(const std::string &type, const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ImpulseCoefficients::ImpulseCoefficients(const std::string &type,
                                         const std::string &name, GmatBase *obj)
   : HardwareReal(name, type, Gmat::SPACECRAFT, Gmat::CHEMICAL_THRUSTER, obj,
                  "Impulse Coefficient " + type, "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
   
   if      (type == "K1" )  mImpulseCoeffId = K1;
   else if (type == "K2" )  mImpulseCoeffId = K2;
   else if (type == "K3" )  mImpulseCoeffId = K3;
   else if (type == "K4" )  mImpulseCoeffId = K4;
   else if (type == "K5" )  mImpulseCoeffId = K5;
   else if (type == "K6" )  mImpulseCoeffId = K6;
   else if (type == "K7" )  mImpulseCoeffId = K7;
   else if (type == "K8" )  mImpulseCoeffId = K8;
   else if (type == "K9" )  mImpulseCoeffId = K9;
   else if (type == "K10")  mImpulseCoeffId = K10;
   else if (type == "K11")  mImpulseCoeffId = K11;
   else if (type == "K12")  mImpulseCoeffId = K12;
   else if (type == "K13")  mImpulseCoeffId = K13;
   else if (type == "K14")  mImpulseCoeffId = K14;
   else if (type == "K15")  mImpulseCoeffId = K15;
   else if (type == "K16")  mImpulseCoeffId = K16;
   else
      mImpulseCoeffId = -1;
   
   #ifdef DEBUG_IMPULSE_COEFF
   MessageInterface::ShowMessage
      ("ImpulseCoefficients::ImpulseCoefficients() type='%s', name='%s', "
       "mImpulseCoeffId=%d\n", type.c_str(), name.c_str(), mImpulseCoeffId);
   #endif
}


//------------------------------------------------------------------------------
// ImpulseCoefficients(const ImpulseCoefficients &copy)
//------------------------------------------------------------------------------
ImpulseCoefficients::ImpulseCoefficients(const ImpulseCoefficients &copy)
   : HardwareReal(copy)
{
   mImpulseCoeffId = copy.mImpulseCoeffId;
}


//------------------------------------------------------------------------------
// ImpulseCoefficients& operator=(const ImpulseCoefficients &right)
//------------------------------------------------------------------------------
ImpulseCoefficients& ImpulseCoefficients::operator=(const ImpulseCoefficients &right)
{
   if (this != &right)
   {
      HardwareReal::operator=(right);
      mImpulseCoeffId = right.mImpulseCoeffId;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~ImpulseCoefficients()
//------------------------------------------------------------------------------
ImpulseCoefficients::~ImpulseCoefficients()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ImpulseCoefficients::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(mImpulseCoeffId);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void ImpulseCoefficients::SetReal(Real val)
{
   SpacecraftData::SetReal(mImpulseCoeffId, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* ImpulseCoefficients::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ImpulseCoefficients::Clone(void) const
{
   return new ImpulseCoefficients(*this);
}


//==============================================================================
//                              ThrustDirections
//==============================================================================
/**
 * Implements ThrustDirections class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ThrustDirections(const std::string &type, const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ThrustDirections::ThrustDirections(const std::string &type,
                                   const std::string &name, GmatBase *obj)
   : HardwareReal(name, type, Gmat::SPACECRAFT, Gmat::THRUSTER, obj, type, "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
   
   if (type == "ThrustDirection1" )
      mThrustDirectionId = THRUST_DIRECTION1;
   else if (type == "ThrustDirection2" )
      mThrustDirectionId = THRUST_DIRECTION2;
   else if (type == "ThrustDirection3" )
      mThrustDirectionId = THRUST_DIRECTION3;
   else
      mThrustDirectionId = -1;
   
   #ifdef DEBUG_IMPULSE_COEFF
   MessageInterface::ShowMessage
      ("ThrustDirections::ThrustDirections() type='%s', name='%s', "
       "mThrustDirectionId=%d\n", type.c_str(), name.c_str(), mThrustDirectionId);
   #endif
}


//------------------------------------------------------------------------------
// ThrustDirections(const ThrustDirections &copy)
//------------------------------------------------------------------------------
ThrustDirections::ThrustDirections(const ThrustDirections &copy)
   : HardwareReal(copy)
{
   mThrustDirectionId = copy.mThrustDirectionId;
}


//------------------------------------------------------------------------------
// ThrustDirections& operator=(const ThrustDirections &right)
//------------------------------------------------------------------------------
ThrustDirections& ThrustDirections::operator=(const ThrustDirections &right)
{
   if (this != &right)
   {
      HardwareReal::operator=(right);
      mThrustDirectionId = right.mThrustDirectionId;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustDirections()
//------------------------------------------------------------------------------
ThrustDirections::~ThrustDirections()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustDirections::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(mThrustDirectionId);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void ThrustDirections::SetReal(Real val)
{
   SpacecraftData::SetReal(mThrustDirectionId, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* ThrustDirections::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ThrustDirections::Clone(void) const
{
   return new ThrustDirections(*this);
}


//==============================================================================
//                                ThrustMagnitude
//==============================================================================
/**
 * Implements ThrustMagnitude class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ThrustMagnitude(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ThrustMagnitude::ThrustMagnitude(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "ThrustMagnitude", Gmat::SPACECRAFT, Gmat::THRUSTER, obj,
                  "ThrustMagnitude", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
}

//------------------------------------------------------------------------------
// ThrustMagnitude(const ThrustMagnitude &copy)
//------------------------------------------------------------------------------
ThrustMagnitude::ThrustMagnitude(const ThrustMagnitude &copy)
   : HardwareReal(copy)
{
}

//------------------------------------------------------------------------------
// ThrustMagnitude& operator=(const ThrustMagnitude &right)
//------------------------------------------------------------------------------
ThrustMagnitude& ThrustMagnitude::operator=(const ThrustMagnitude &right)
{
   if (this != &right)
      HardwareReal::operator=(right);
   
   return *this;
}

//------------------------------------------------------------------------------
// ~ThrustMagnitude()
//------------------------------------------------------------------------------
ThrustMagnitude::~ThrustMagnitude()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustMagnitude::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(THRUST);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void ThrustMagnitude::SetReal(Real val)
{
   SpacecraftData::SetReal(THRUST, val);
   RealVar::SetReal(val);
}

//------------------------------------------------------------------------------
// GmatBase* ThrustMagnitude::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ThrustMagnitude::Clone(void) const
{
   return new ThrustMagnitude(*this);
}


//==============================================================================
//                                     Isp
//==============================================================================
/**
 * Implements Isp class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Isp(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
Isp::Isp(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "Isp", Gmat::SPACECRAFT, Gmat::THRUSTER, obj,
                  "Isp", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
}

//------------------------------------------------------------------------------
// Isp(const Isp &copy)
//------------------------------------------------------------------------------
Isp::Isp(const Isp &copy)
   : HardwareReal(copy)
{
}

//------------------------------------------------------------------------------
// Isp& operator=(const Isp &right)
//------------------------------------------------------------------------------
Isp& Isp::operator=(const Isp &right)
{
   if (this != &right)
      HardwareReal::operator=(right);
   
   return *this;
}

//------------------------------------------------------------------------------
// ~Isp()
//------------------------------------------------------------------------------
Isp::~Isp()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Isp::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(ISP);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void Isp::SetReal(Real val)
{
   SpacecraftData::SetReal(ISP, val);
   RealVar::SetReal(val);
}

//------------------------------------------------------------------------------
// GmatBase* Isp::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Isp::Clone(void) const
{
   return new Isp(*this);
}


//==============================================================================
//                                  MassFlowRate
//==============================================================================
/**
 * Implements MassFlowRate class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// MassFlowRate(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
MassFlowRate::MassFlowRate(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "MassFlowRate", Gmat::SPACECRAFT, Gmat::THRUSTER, obj,
                  "MassFlowRate", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
}

//------------------------------------------------------------------------------
// MassFlowRate(const MassFlowRate &copy)
//------------------------------------------------------------------------------
MassFlowRate::MassFlowRate(const MassFlowRate &copy)
   : HardwareReal(copy)
{
}

//------------------------------------------------------------------------------
// MassFlowRate& operator=(const MassFlowRate &right)
//------------------------------------------------------------------------------
MassFlowRate& MassFlowRate::operator=(const MassFlowRate &right)
{
   if (this != &right)
      HardwareReal::operator=(right);
   
   return *this;
}

//------------------------------------------------------------------------------
// ~MassFlowRate()
//------------------------------------------------------------------------------
MassFlowRate::~MassFlowRate()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool MassFlowRate::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(MASS_FLOW_RATE);
   
   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}

//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void MassFlowRate::SetReal(Real val)
{
   SpacecraftData::SetReal(ISP, val);
   RealVar::SetReal(val);
}

//------------------------------------------------------------------------------
// GmatBase* MassFlowRate::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* MassFlowRate::Clone(void) const
{
   return new MassFlowRate(*this);
}


//==============================================================================
//                              TotalPowerAvailable
//==============================================================================
/**
 * Implements TotalPowerAvailable class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TotalPowerAvailable(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
TotalPowerAvailable::TotalPowerAvailable(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "TotalPowerAvailable", Gmat::SPACECRAFT,
                  Gmat::POWER_SYSTEM, obj, "Total Power Available", "", false)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// TotalPowerAvailable(const TotalPowerAvailable &copy)
//------------------------------------------------------------------------------
TotalPowerAvailable::TotalPowerAvailable(const TotalPowerAvailable &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// TotalPowerAvailable& operator=(const TotalPowerAvailable &right)
//------------------------------------------------------------------------------
TotalPowerAvailable& TotalPowerAvailable::operator=(const TotalPowerAvailable &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~TotalPowerAvailable()
//------------------------------------------------------------------------------
TotalPowerAvailable::~TotalPowerAvailable()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool TotalPowerAvailable::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(TOTAL_POWER_AVAILABLE);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void TotalPowerAvailable::SetReal(Real val)
{
   SpacecraftData::SetReal(TOTAL_POWER_AVAILABLE, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* TotalPowerAvailable::Clone() const
//------------------------------------------------------------------------------
GmatBase* TotalPowerAvailable::Clone() const
{
   return new TotalPowerAvailable(*this);
}

//==============================================================================
//                              RequiredBusPower
//==============================================================================
/**
 * Implements RequiredBusPower class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RequiredBusPower(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
RequiredBusPower::RequiredBusPower(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "RequiredBusPower", Gmat::SPACECRAFT,
                  Gmat::POWER_SYSTEM, obj, "Required Bus Power", "", false)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// RequiredBusPower(const RequiredBusPower &copy)
//------------------------------------------------------------------------------
RequiredBusPower::RequiredBusPower(const RequiredBusPower &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// RequiredBusPower& operator=(const RequiredBusPower &right)
//------------------------------------------------------------------------------
RequiredBusPower& RequiredBusPower::operator=(const RequiredBusPower &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~RequiredBusPower()
//------------------------------------------------------------------------------
RequiredBusPower::~RequiredBusPower()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool RequiredBusPower::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(REQUIRED_BUS_POWER);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void RequiredBusPower::SetReal(Real val)
{
   SpacecraftData::SetReal(REQUIRED_BUS_POWER, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* RequiredBusPower::Clone() const
//------------------------------------------------------------------------------
GmatBase* RequiredBusPower::Clone() const
{
   return new RequiredBusPower(*this);
}


//==============================================================================
//                              ThrustPowerAvailable
//==============================================================================
/**
 * Implements ThrustPowerAvailable class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ThrustPowerAvailable(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ThrustPowerAvailable::ThrustPowerAvailable(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "ThrustPowerAvailable", Gmat::SPACECRAFT,
                  Gmat::POWER_SYSTEM, obj, "Thrust Power Available", "", false)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// ThrustPowerAvailable(const ThrustPowerAvailable &copy)
//------------------------------------------------------------------------------
ThrustPowerAvailable::ThrustPowerAvailable(const ThrustPowerAvailable &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// ThrustPowerAvailable& operator=(const ThrustPowerAvailable &right)
//------------------------------------------------------------------------------
ThrustPowerAvailable& ThrustPowerAvailable::operator=(const ThrustPowerAvailable &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustPowerAvailable()
//------------------------------------------------------------------------------
ThrustPowerAvailable::~ThrustPowerAvailable()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustPowerAvailable::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(THRUST_POWER_AVAILABLE);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void ThrustPowerAvailable::SetReal(Real val)
{
   SpacecraftData::SetReal(THRUST_POWER_AVAILABLE, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* ThrustPowerAvailable::Clone() const
//------------------------------------------------------------------------------
GmatBase* ThrustPowerAvailable::Clone() const
{
   return new ThrustPowerAvailable(*this);
}



//==============================================================================
//                              AreaCoefficient
//==============================================================================
/**
 * Implements AreaCoefficient class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // AreaCoefficient(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
AreaCoefficient::AreaCoefficient(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "AreaCoefficient", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Area Coefficient", "", true)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// AreaCoefficient(const AreaCoefficient &copy)
//------------------------------------------------------------------------------
AreaCoefficient::AreaCoefficient(const AreaCoefficient &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// AreaCoefficient& operator=(const AreaCoefficient &right)
//------------------------------------------------------------------------------
AreaCoefficient& AreaCoefficient::operator=(const AreaCoefficient &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~AreaCoefficient()
//------------------------------------------------------------------------------
AreaCoefficient::~AreaCoefficient()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool AreaCoefficient::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(AREA_COEFFICIENT_ID);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void AreaCoefficient::SetReal(Real val)
{
   SpacecraftData::SetReal(AREA_COEFFICIENT_ID, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* AreaCoefficient::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AreaCoefficient::Clone(void) const
{
   return new AreaCoefficient(*this);
}


//==============================================================================
//                              SpecularFraction
//==============================================================================
/**
 * Implements SpecularFraction class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // SpecularFraction(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
SpecularFraction::SpecularFraction(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "SpecularFraction", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Specular Fraction", "", true)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// SpecularFraction(const SpecularFraction &copy)
//------------------------------------------------------------------------------
SpecularFraction::SpecularFraction(const SpecularFraction &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// SpecularFraction& operator=(const SpecularFraction &right)
//------------------------------------------------------------------------------
SpecularFraction& SpecularFraction::operator=(const SpecularFraction &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SpecularFraction()
//------------------------------------------------------------------------------
SpecularFraction::~SpecularFraction()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SpecularFraction::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SPECULAR_FRACTION_ID);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void SpecularFraction::SetReal(Real val)
{
   SpacecraftData::SetReal(SPECULAR_FRACTION_ID, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* SpecularFraction::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* SpecularFraction::Clone(void) const
{
   return new SpecularFraction(*this);
}


//==============================================================================
//                              DiffuseFraction
//==============================================================================
/**
 * Implements DiffuseFraction class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // DiffuseFraction(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
DiffuseFraction::DiffuseFraction(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "DiffuseFraction", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Diffuse Fraction", "", true)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// DiffuseFraction(const DiffuseFraction &copy)
//------------------------------------------------------------------------------
DiffuseFraction::DiffuseFraction(const DiffuseFraction &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// DiffuseFraction& operator=(const DiffuseFraction &right)
//------------------------------------------------------------------------------
DiffuseFraction& DiffuseFraction::operator=(const DiffuseFraction &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DiffuseFraction()
//------------------------------------------------------------------------------
DiffuseFraction::~DiffuseFraction()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DiffuseFraction::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DIFFUSE_FRACTION_ID);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void DiffuseFraction::SetReal(Real val)
{
   SpacecraftData::SetReal(DIFFUSE_FRACTION_ID, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* DiffuseFraction::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DiffuseFraction::Clone(void) const
{
   return new DiffuseFraction(*this);
}


//==============================================================================
//                              Area
//==============================================================================
/**
 * Implements Area class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // Area(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
Area::Area(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "Area", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Area", "", false)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// Area(const Area &copy)
//------------------------------------------------------------------------------
Area::Area(const Area &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// Area& operator=(const Area &right)
//------------------------------------------------------------------------------
Area& Area::operator=(const Area &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~Area()
//------------------------------------------------------------------------------
Area::~Area()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Area::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(AREA_ID);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void Area::SetReal(Real val)
{
   SpacecraftData::SetReal(AREA_ID, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* Area::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Area::Clone(void) const
{
   return new Area(*this);
}


//==============================================================================
//                              PlateNormal
//==============================================================================
/**
 * Implements PlateNormal class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // PlateNormal(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
PlateNormal::PlateNormal(const std::string &name, GmatBase *obj)
   : HardwareRvector(name, "PlateNormal", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Plate Normal", "", false, 3)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// PlateNormal(const PlateNormal &copy)
//------------------------------------------------------------------------------
PlateNormal::PlateNormal(const PlateNormal &copy)
   : HardwareRvector(copy)
{
}


//------------------------------------------------------------------------------
// PlateNormal& operator=(const PlateNormal &right)
//------------------------------------------------------------------------------
PlateNormal& PlateNormal::operator=(const PlateNormal &right)
{
   if (this != &right)
      HardwareRvector::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~PlateNormal()
//------------------------------------------------------------------------------
PlateNormal::~PlateNormal()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PlateNormal::Evaluate()
{
   Rvector arr = SpacecraftData::GetRvector(PLATE_NORMAL_ID);
   
   for (int i = 0; i < 3; i++)
   {
      mRvectorValue[i] = arr[i];
   }

   //Find out how to correctly copy Rvector over...
   if (mRvectorValue[2] == BALLISTIC_REAL_UNDEFINED)
      return false; 
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void PlateNormal::SetRvector(Rvector& val)
{
   SpacecraftData::SetRvector(PLATE_NORMAL_ID, val);
   RvectorVar::SetRvector(val);
}


//------------------------------------------------------------------------------
// GmatBase* PlateNormal::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PlateNormal::Clone(void) const
{
   return new PlateNormal(*this);
}


//==============================================================================
//                              LitFraction
//==============================================================================
/**
 * Implements LitFraction class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // LitFraction(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
LitFraction::LitFraction(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "LitFraction", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Lit Fraction", "", false)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// LitFraction(const LitFraction &copy)
//------------------------------------------------------------------------------
LitFraction::LitFraction(const LitFraction &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// LitFraction& operator=(const LitFraction &right)
//------------------------------------------------------------------------------
LitFraction& LitFraction::operator=(const LitFraction &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~LitFraction()
//------------------------------------------------------------------------------
LitFraction::~LitFraction()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool LitFraction::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(LIT_FRACTION_ID);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void LitFraction::SetReal(Real val)
{
   SpacecraftData::SetReal(LIT_FRACTION_ID, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* LitFraction::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* LitFraction::Clone(void) const
{
   return new LitFraction(*this);
}

//==============================================================================
//                              Type
//==============================================================================
/**
 * Implements Type class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // Type(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
Type::Type(const std::string &name, GmatBase *obj)
   : HardwareString(name, "Type", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Type", "", false)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// Type(const Type &copy)
//------------------------------------------------------------------------------
Type::Type(const Type &copy)
   : HardwareString(copy)
{
}


//------------------------------------------------------------------------------
// Type& operator=(const Type &right)
//------------------------------------------------------------------------------
Type& Type::operator=(const Type &right)
{
   if (this != &right)
      HardwareString::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~Type()
//------------------------------------------------------------------------------
Type::~Type()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Type::Evaluate()
{
   mStringValue = SpacecraftData::GetString(TYPE_ID);

   if (mStringValue == GmatBase::STRING_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void Type::SetString(std::string &val)
{
   //throw InterpreterException(instanceName + " is not allowed to be set in the Mission Sequence.\n");
   SpacecraftData::SetString(TYPE_ID, val);
   StringVar::SetString(val);
}


//------------------------------------------------------------------------------
// GmatBase* Type::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Type::Clone(void) const
{
   return new Type(*this);
}










//==============================================================================
//                              AreaCoefficientSigma
//==============================================================================
/**
 * Implements AreaCoefficientSigma class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // AreaCoefficientSigma(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
AreaCoefficientSigma::AreaCoefficientSigma(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "AreaCoefficientSigma", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Area Coefficient Sigma", "", false)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// AreaCoefficientSigma(const AreaCoefficientSigma &copy)
//------------------------------------------------------------------------------
AreaCoefficientSigma::AreaCoefficientSigma(const AreaCoefficientSigma &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// AreaCoefficientSigma& operator=(const AreaCoefficientSigma &right)
//------------------------------------------------------------------------------
AreaCoefficientSigma& AreaCoefficientSigma::operator=(const AreaCoefficientSigma &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~AreaCoefficientSigma()
//------------------------------------------------------------------------------
AreaCoefficientSigma::~AreaCoefficientSigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool AreaCoefficientSigma::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(AREA_COEFFICIENT_SIGMA_ID);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void AreaCoefficientSigma::SetReal(Real val)
{
   SpacecraftData::SetReal(AREA_COEFFICIENT_SIGMA_ID, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* AreaCoefficientSigma::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AreaCoefficientSigma::Clone(void) const
{
   return new AreaCoefficientSigma(*this);
}


//==============================================================================
//                              SpecularFractionSigma
//==============================================================================
/**
 * Implements SpecularFractionSigma class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // SpecularFractionSigma(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
SpecularFractionSigma::SpecularFractionSigma(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "SpecularFractionSigma", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Specular Fraction Sigma", "", false)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// SpecularFractionSigma(const SpecularFractionSigma &copy)
//------------------------------------------------------------------------------
SpecularFractionSigma::SpecularFractionSigma(const SpecularFractionSigma &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// SpecularFractionSigma& operator=(const SpecularFractionSigma &right)
//------------------------------------------------------------------------------
SpecularFractionSigma& SpecularFractionSigma::operator=(const SpecularFractionSigma &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SpecularFractionSigma()
//------------------------------------------------------------------------------
SpecularFractionSigma::~SpecularFractionSigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SpecularFractionSigma::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SPECULAR_FRACTION_SIGMA_ID);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void SpecularFractionSigma::SetReal(Real val)
{
   SpacecraftData::SetReal(SPECULAR_FRACTION_SIGMA_ID, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* SpecularFractionSigma::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* SpecularFractionSigma::Clone(void) const
{
   return new SpecularFractionSigma(*this);
}


//==============================================================================
//                              DiffuseFractionSigma
//==============================================================================
/**
 * Implements DiffuseFractionSigma class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // DiffuseFractionSigma(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
DiffuseFractionSigma::DiffuseFractionSigma(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "DiffuseFractionSigma", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Diffuse Fraction Sigma", "", false)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// DiffuseFractionSigma(const DiffuseFractionSigma &copy)
//------------------------------------------------------------------------------
DiffuseFractionSigma::DiffuseFractionSigma(const DiffuseFractionSigma &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// DiffuseFractionSigma& operator=(const DiffuseFractionSigma &right)
//------------------------------------------------------------------------------
DiffuseFractionSigma& DiffuseFractionSigma::operator=(const DiffuseFractionSigma &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DiffuseFractionSigma()
//------------------------------------------------------------------------------
DiffuseFractionSigma::~DiffuseFractionSigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DiffuseFractionSigma::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DIFFUSE_FRACTION_SIGMA_ID);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void DiffuseFractionSigma::SetReal(Real val)
{
   SpacecraftData::SetReal(DIFFUSE_FRACTION_SIGMA_ID, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* DiffuseFractionSigma::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DiffuseFractionSigma::Clone(void) const
{
   return new DiffuseFractionSigma(*this);
}

//==============================================================================
//                             PlateNormalHistoryFile
//==============================================================================
/**
 * Implements PlateNormalHistoryFile class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // PlateNormalHistoryFile(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
PlateNormalHistoryFile::PlateNormalHistoryFile(const std::string &name, GmatBase *obj)
   : HardwareString(name, "PlateNormalHistoryFile", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Plate Normal History File", "", false)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// PlateNormalHistoryFile(const Type &copy)
//------------------------------------------------------------------------------
PlateNormalHistoryFile::PlateNormalHistoryFile(const PlateNormalHistoryFile &copy)
   : HardwareString(copy)
{
}


//------------------------------------------------------------------------------
// PlateNormalHistoryFile& operator=(const PlateNormalHistoryFile &right)
//------------------------------------------------------------------------------
PlateNormalHistoryFile& PlateNormalHistoryFile::operator=(const PlateNormalHistoryFile &right)
{
   if (this != &right)
      HardwareString::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~PlateNormalHistoryFile()
//------------------------------------------------------------------------------
PlateNormalHistoryFile::~PlateNormalHistoryFile()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PlateNormalHistoryFile::Evaluate()
{
   mStringValue = SpacecraftData::GetString(PLATE_NORMAL_HISTORY_FILE_ID);

   if (mStringValue == GmatBase::STRING_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void PlateNormalHistoryFile::SetString(std::string &val)
{
   //throw InterpreterException(instanceName + " is not allowed to be set in the Mission Sequence.\n");
   SpacecraftData::SetString(PLATE_NORMAL_HISTORY_FILE_ID, val);
   StringVar::SetString(val);
}


//------------------------------------------------------------------------------
// GmatBase* PlateNormalHistoryFile::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PlateNormalHistoryFile::Clone(void) const
{
   return new PlateNormalHistoryFile(*this);
}


//==============================================================================
//                              PlateX
//==============================================================================
/**
 * Implements PlateX class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // PlateX(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
PlateX::PlateX(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "PlateX", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "PlateX", "", false)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// PlateX(const PlateX &copy)
//------------------------------------------------------------------------------
PlateX::PlateX(const PlateX &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// PlateX& operator=(const PlateX &right)
//------------------------------------------------------------------------------
PlateX& PlateX::operator=(const PlateX &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~PlateX()
//------------------------------------------------------------------------------
PlateX::~PlateX()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PlateX::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(PLATEX);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void PlateX::SetReal(Real val)
{
   SpacecraftData::SetReal(PLATEX, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* PlateX::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PlateX::Clone(void) const
{
   return new PlateX(*this);
}


//==============================================================================
//                              PlateY
//==============================================================================
/**
 * Implements PlateY class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // PlateY(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
PlateY::PlateY(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "PlateY", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Specular Fraction Sigma", "", false)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// PlateY(const PlateY &copy)
//------------------------------------------------------------------------------
PlateY::PlateY(const PlateY &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// PlateY& operator=(const PlateY &right)
//------------------------------------------------------------------------------
PlateY& PlateY::operator=(const PlateY &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~PlateY()
//------------------------------------------------------------------------------
PlateY::~PlateY()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PlateY::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(PLATEY);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void PlateY::SetReal(Real val)
{
   SpacecraftData::SetReal(PLATEY, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* PlateY::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PlateY::Clone(void) const
{
   return new PlateY(*this);
}


//==============================================================================
//                              PlateZ
//==============================================================================
/**
 * Implements PlateZ class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // PlateZ(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
PlateZ::PlateZ(const std::string &name, GmatBase *obj)
   : HardwareReal(name, "PlateZ", Gmat::SPACECRAFT, Gmat::PLATE, obj,
      "Diffuse Fraction Sigma", "", false)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::RED32;
#endif
}


//------------------------------------------------------------------------------
// PlateZ(const PlateZ &copy)
//------------------------------------------------------------------------------
PlateZ::PlateZ(const PlateZ &copy)
   : HardwareReal(copy)
{
}


//------------------------------------------------------------------------------
// PlateZ& operator=(const PlateZ &right)
//------------------------------------------------------------------------------
PlateZ& PlateZ::operator=(const PlateZ &right)
{
   if (this != &right)
      HardwareReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~PlateZ()
//------------------------------------------------------------------------------
PlateZ::~PlateZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool PlateZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(PLATEZ);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void PlateZ::SetReal(Real val)
{
   SpacecraftData::SetReal(PLATEZ, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* PlateZ::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* PlateZ::Clone(void) const
{
   return new PlateZ(*this);
}
