//$Id$
//------------------------------------------------------------------------------
//                            File: BallisticMassParameters
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
 * Implements BallisticMass related parameter classes.
 *    DryMass, DryCenterOfMass[XYZ], DryMomentOfInertia[XX-ZZ],
 *    DragCoeff, ReflectCoeff, DragArea,
 *    SRPArea, TotalMass, SystemCenterOfMass[XYZ], SystemMomentOfInertia[XX-ZZ]
 */
//------------------------------------------------------------------------------

#include "BallisticMassParameters.hpp"
#include "ColorTypes.hpp"


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

//==============================================================================
//                              DryMass
//==============================================================================
/**
 * Implements DryMass class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryMass(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryMass::DryMass(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryMass", obj, "Dry Mass", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryMass(const DryMass &copy)
//------------------------------------------------------------------------------
DryMass::DryMass(const DryMass &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryMass& operator=(const DryMass &right)
//------------------------------------------------------------------------------
DryMass& DryMass::operator=(const DryMass &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryMass()
//------------------------------------------------------------------------------
DryMass::~DryMass()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryMass::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_MASS);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryMass::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryMass::Clone() const
{
   return new DryMass(*this);
}


//==============================================================================
//                              DryCenterOfMassX
//==============================================================================
/**
 * Implements DryCenterOfMassX class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryCenterOfMassX(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryCenterOfMassX::DryCenterOfMassX(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryCenterOfMassX", obj,
                       "Dry Center Of Mass X", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryCenterOfMassX(const DryCenterOfMassX &copy)
//------------------------------------------------------------------------------
DryCenterOfMassX::DryCenterOfMassX(const DryCenterOfMassX &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryCenterOfMassX& operator=(const DryCenterOfMassX &right)
//------------------------------------------------------------------------------
DryCenterOfMassX& DryCenterOfMassX::operator=(const DryCenterOfMassX &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryCenterOfMassX()
//------------------------------------------------------------------------------
DryCenterOfMassX::~DryCenterOfMassX()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryCenterOfMassX::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_CM_X);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryCenterOfMassX::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryCenterOfMassX::Clone() const
{
   return new DryCenterOfMassX(*this);
}


//==============================================================================
//                              DryCenterOfMassY
//==============================================================================
/**
 * Implements DryCenterOfMassY class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryCenterOfMassY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryCenterOfMassY::DryCenterOfMassY(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryCenterOfMassY", obj,
                       "Dry Center Of Mass Y", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryCenterOfMassY(const DryCenterOfMassY &copy)
//------------------------------------------------------------------------------
DryCenterOfMassY::DryCenterOfMassY(const DryCenterOfMassY &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryCenterOfMassY& operator=(const DryCenterOfMassY &right)
//------------------------------------------------------------------------------
DryCenterOfMassY& DryCenterOfMassY::operator=(const DryCenterOfMassY &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryCenterOfMassY()
//------------------------------------------------------------------------------
DryCenterOfMassY::~DryCenterOfMassY()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryCenterOfMassY::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_CM_Y);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryCenterOfMassY::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryCenterOfMassY::Clone() const
{
   return new DryCenterOfMassY(*this);
}


//==============================================================================
//                              DryCenterOfMassZ
//==============================================================================
/**
 * Implements DryCenterOfMassZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryCenterOfMassZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryCenterOfMassZ::DryCenterOfMassZ(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryCenterOfMassZ", obj,
                       "Dry Center Of Mass Z", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryCenterOfMassZ(const DryCenterOfMassZ &copy)
//------------------------------------------------------------------------------
DryCenterOfMassZ::DryCenterOfMassZ(const DryCenterOfMassZ &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryCenterOfMassZ& operator=(const DryCenterOfMassZ &right)
//------------------------------------------------------------------------------
DryCenterOfMassZ& DryCenterOfMassZ::operator=(const DryCenterOfMassZ &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryCenterOfMassZ()
//------------------------------------------------------------------------------
DryCenterOfMassZ::~DryCenterOfMassZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryCenterOfMassZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_CM_Z);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryCenterOfMassZ::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryCenterOfMassZ::Clone() const
{
   return new DryCenterOfMassZ(*this);
}


//==============================================================================
//                              DryMomentOfInertiaXX
//==============================================================================
/**
 * Implements DryMomentOfInertiaXX class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryMomentOfInertiaXX(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryMomentOfInertiaXX::DryMomentOfInertiaXX(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryMomentOfInertiaXX", obj, 
                       "Dry Moment Of Inertia XX", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaXX(const DryMomentOfInertiaXX &copy)
//------------------------------------------------------------------------------
DryMomentOfInertiaXX::DryMomentOfInertiaXX(const DryMomentOfInertiaXX &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaXX& operator=(const DryMomentOfInertiaXX &right)
//------------------------------------------------------------------------------
DryMomentOfInertiaXX& DryMomentOfInertiaXX::operator=(const DryMomentOfInertiaXX &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryMomentOfInertiaXX()
//------------------------------------------------------------------------------
DryMomentOfInertiaXX::~DryMomentOfInertiaXX()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryMomentOfInertiaXX::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_MOI_XX);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryMomentOfInertiaXX::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryMomentOfInertiaXX::Clone() const
{
   return new DryMomentOfInertiaXX(*this);
}


//==============================================================================
//                              DryMomentOfInertiaXY
//==============================================================================
/**
 * Implements DryMomentOfInertiaXY class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryMomentOfInertiaXY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryMomentOfInertiaXY::DryMomentOfInertiaXY(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryMomentOfInertiaXY", obj, 
                       "Dry Moment Of Inertia XY", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaXY(const DryMomentOfInertiaXY &copy)
//------------------------------------------------------------------------------
DryMomentOfInertiaXY::DryMomentOfInertiaXY(const DryMomentOfInertiaXY &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaXY& operator=(const DryMomentOfInertiaXY &right)
//------------------------------------------------------------------------------
DryMomentOfInertiaXY& DryMomentOfInertiaXY::operator=(const DryMomentOfInertiaXY &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryMomentOfInertiaXY()
//------------------------------------------------------------------------------
DryMomentOfInertiaXY::~DryMomentOfInertiaXY()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryMomentOfInertiaXY::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_MOI_XY);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryMomentOfInertiaXY::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryMomentOfInertiaXY::Clone() const
{
   return new DryMomentOfInertiaXY(*this);
}


//==============================================================================
//                              DryMomentOfInertiaXZ
//==============================================================================
/**
 * Implements DryMomentOfInertiaXZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryMomentOfInertiaXZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryMomentOfInertiaXZ::DryMomentOfInertiaXZ(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryMomentOfInertiaXZ", obj,
                       "Dry Moment Of Inertia XZ", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaXZ(const DryMomentOfInertiaXZ &copy)
//------------------------------------------------------------------------------
DryMomentOfInertiaXZ::DryMomentOfInertiaXZ(const DryMomentOfInertiaXZ &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaXZ& operator=(const DryMomentOfInertiaXZ &right)
//------------------------------------------------------------------------------
DryMomentOfInertiaXZ& DryMomentOfInertiaXZ::operator=(const DryMomentOfInertiaXZ &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryMomentOfInertiaXZ()
//------------------------------------------------------------------------------
DryMomentOfInertiaXZ::~DryMomentOfInertiaXZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryMomentOfInertiaXZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_MOI_XZ);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryMomentOfInertiaXZ::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryMomentOfInertiaXZ::Clone() const
{
   return new DryMomentOfInertiaXZ(*this);
}


//==============================================================================
//                              DryMomentOfInertiaYY
//==============================================================================
/**
 * Implements DryMomentOfInertiaYY class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryMomentOfInertiaYY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryMomentOfInertiaYY::DryMomentOfInertiaYY(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryMomentOfInertiaYY", obj,
                       "Dry Moment Of Inertia YY", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaYY(const DryMomentOfInertiaYY &copy)
//------------------------------------------------------------------------------
DryMomentOfInertiaYY::DryMomentOfInertiaYY(const DryMomentOfInertiaYY &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaYY& operator=(const DryMomentOfInertiaYY &right)
//------------------------------------------------------------------------------
DryMomentOfInertiaYY& DryMomentOfInertiaYY::operator=(const DryMomentOfInertiaYY &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryMomentOfInertiaYY()
//------------------------------------------------------------------------------
DryMomentOfInertiaYY::~DryMomentOfInertiaYY()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryMomentOfInertiaYY::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_MOI_YY);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryMomentOfInertiaYY::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryMomentOfInertiaYY::Clone() const
{
   return new DryMomentOfInertiaYY(*this);
}


//==============================================================================
//                              DryMomentOfInertiaYZ
//==============================================================================
/**
 * Implements DryMomentOfInertiaYZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryMomentOfInertiaYZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryMomentOfInertiaYZ::DryMomentOfInertiaYZ(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryMomentOfInertiaYZ", obj,
                      "Dry Moment Of Inertia YZ", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaYZ(const DryMomentOfInertiaYZ &copy)
//------------------------------------------------------------------------------
DryMomentOfInertiaYZ::DryMomentOfInertiaYZ(const DryMomentOfInertiaYZ &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaYZ& operator=(const DryMomentOfInertiaYZ &right)
//------------------------------------------------------------------------------
DryMomentOfInertiaYZ& DryMomentOfInertiaYZ::operator=(const DryMomentOfInertiaYZ &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryMomentOfInertiaYZ()
//------------------------------------------------------------------------------
DryMomentOfInertiaYZ::~DryMomentOfInertiaYZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryMomentOfInertiaYZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_MOI_YZ);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryMomentOfInertiaYZ::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryMomentOfInertiaYZ::Clone() const
{
   return new DryMomentOfInertiaYZ(*this);
}


//==============================================================================
//                              DryMomentOfInertiaZZ
//==============================================================================
/**
 * Implements DryMomentOfInertiaZZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DryMomentOfInertiaZZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DryMomentOfInertiaZZ::DryMomentOfInertiaZZ(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DryMomentOfInertiaZZ", obj,
                       "Dry Moment Of Inertia ZZ", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaZZ(const DryMomentOfInertiaZZ &copy)
//------------------------------------------------------------------------------
DryMomentOfInertiaZZ::DryMomentOfInertiaZZ(const DryMomentOfInertiaZZ &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DryMomentOfInertiaZZ& operator=(const DryMomentOfInertiaZZ &right)
//------------------------------------------------------------------------------
DryMomentOfInertiaZZ& DryMomentOfInertiaZZ::operator=(const DryMomentOfInertiaZZ &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DryMomentOfInertiaZZ()
//------------------------------------------------------------------------------
DryMomentOfInertiaZZ::~DryMomentOfInertiaZZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DryMomentOfInertiaZZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRY_MOI_ZZ);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DryMomentOfInertiaZZ::Clone() const
//------------------------------------------------------------------------------
GmatBase* DryMomentOfInertiaZZ::Clone() const
{
   return new DryMomentOfInertiaZZ(*this);
}


//==============================================================================
//                              DragCoeff
//==============================================================================
/**
 * Implements DragCoeff class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DragCoeff(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DragCoeff::DragCoeff(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "Cd", obj, "Drag Coefficient", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW32;
   #endif
}


//------------------------------------------------------------------------------
// DragCoeff(const DragCoeff &copy)
//------------------------------------------------------------------------------
DragCoeff::DragCoeff(const DragCoeff &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DragCoeff& operator=(const DragCoeff &right)
//------------------------------------------------------------------------------
DragCoeff& DragCoeff::operator=(const DragCoeff &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DragCoeff()
//------------------------------------------------------------------------------
DragCoeff::~DragCoeff()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DragCoeff::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRAG_COEFF);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DragCoeff::Clone() const
//------------------------------------------------------------------------------
GmatBase* DragCoeff::Clone() const
{
   return new DragCoeff(*this);
}


//==============================================================================
//                              ReflectCoeff
//==============================================================================
/**
 * Implements ReflectCoeff class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ReflectCoeff(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
ReflectCoeff::ReflectCoeff(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "Cr", obj, "Reflectivity Coefficient", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}


//------------------------------------------------------------------------------
// ReflectCoeff(const ReflectCoeff &copy)
//------------------------------------------------------------------------------
ReflectCoeff::ReflectCoeff(const ReflectCoeff &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// ReflectCoeff& operator=(const ReflectCoeff &right)
//------------------------------------------------------------------------------
ReflectCoeff& ReflectCoeff::operator=(const ReflectCoeff &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ReflectCoeff()
//------------------------------------------------------------------------------
ReflectCoeff::~ReflectCoeff()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ReflectCoeff::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(REFLECT_COEFF);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* ReflectCoeff::Clone() const
//------------------------------------------------------------------------------
GmatBase* ReflectCoeff::Clone() const
{
   return new ReflectCoeff(*this);
}


//==============================================================================
//                              DragArea
//==============================================================================
/**
 * Implements DragArea class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// DragArea(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DragArea::DragArea(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "DragArea", obj, "Drag Area", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}


//------------------------------------------------------------------------------
// DragArea(const DragArea &copy)
//------------------------------------------------------------------------------
DragArea::DragArea(const DragArea &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DragArea& operator=(const DragArea &right)
//------------------------------------------------------------------------------
DragArea& DragArea::operator=(const DragArea &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DragArea()
//------------------------------------------------------------------------------
DragArea::~DragArea()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DragArea::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRAG_AREA);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DragArea::Clone() const
//------------------------------------------------------------------------------
GmatBase* DragArea::Clone() const
{
   return new DragArea(*this);
}


//==============================================================================
//                              SRPArea
//==============================================================================
/**
 * Implements SRPArea class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SRPArea(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SRPArea::SRPArea(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SRPArea", obj, "SRP Area", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::ORANGE32;
   #endif
}


//------------------------------------------------------------------------------
// SRPArea(const SRPArea &copy)
//------------------------------------------------------------------------------
SRPArea::SRPArea(const SRPArea &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SRPArea& operator=(const SRPArea &right)
//------------------------------------------------------------------------------
SRPArea& SRPArea::operator=(const SRPArea &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SRPArea()
//------------------------------------------------------------------------------
SRPArea::~SRPArea()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SRPArea::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SRP_AREA);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SRPArea::Clone() const
//------------------------------------------------------------------------------
GmatBase* SRPArea::Clone() const
{
   return new SRPArea(*this);
}


//==============================================================================
//                              TotalMass
//==============================================================================
/**
 * Implements TotalMass class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TotalMass(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
TotalMass::TotalMass(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "TotalMass", obj, "Total Mass", "", false)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
}


//------------------------------------------------------------------------------
// TotalMass(const TotalMass &copy)
//------------------------------------------------------------------------------
TotalMass::TotalMass(const TotalMass &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// TotalMass& operator=(const TotalMass &right)
//------------------------------------------------------------------------------
TotalMass& TotalMass::operator=(const TotalMass &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~TotalMass()
//------------------------------------------------------------------------------
TotalMass::~TotalMass()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool TotalMass::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(TOTAL_MASS);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* TotalMass::Clone() const
//------------------------------------------------------------------------------
GmatBase* TotalMass::Clone() const
{
   return new TotalMass(*this);
}


//==============================================================================
//                              SystemCenterOfMassX
//==============================================================================
/**
 * Implements SystemCenterOfMassX class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SystemCenterOfMassX(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SystemCenterOfMassX::SystemCenterOfMassX(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SystemCenterOfMassX", obj,
                       "System CenterOfMass X", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// SystemCenterOfMassX(const SystemCenterOfMassX &copy)
//------------------------------------------------------------------------------
SystemCenterOfMassX::SystemCenterOfMassX(const SystemCenterOfMassX &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SystemCenterOfMassX& operator=(const SystemCenterOfMassX &right)
//------------------------------------------------------------------------------
SystemCenterOfMassX& SystemCenterOfMassX::operator=(const SystemCenterOfMassX &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SystemCenterOfMassX()
//------------------------------------------------------------------------------
SystemCenterOfMassX::~SystemCenterOfMassX()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SystemCenterOfMassX::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SYSTEM_CM_X);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SystemCenterOfMassX::Clone() const
//------------------------------------------------------------------------------
GmatBase* SystemCenterOfMassX::Clone() const
{
   return new SystemCenterOfMassX(*this);
}


//==============================================================================
//                              SystemCenterOfMassY
//==============================================================================
/**
 * Implements SystemCenterOfMassY class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SystemCenterOfMassY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SystemCenterOfMassY::SystemCenterOfMassY(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SystemCenterOfMassY", obj,
                       "System CenterOfMass Y", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// SystemCenterOfMassY(const SystemCenterOfMassY &copy)
//------------------------------------------------------------------------------
SystemCenterOfMassY::SystemCenterOfMassY(const SystemCenterOfMassY &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SystemCenterOfMassY& operator=(const SystemCenterOfMassY &right)
//------------------------------------------------------------------------------
SystemCenterOfMassY& SystemCenterOfMassY::operator=(const SystemCenterOfMassY &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SystemCenterOfMassY()
//------------------------------------------------------------------------------
SystemCenterOfMassY::~SystemCenterOfMassY()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SystemCenterOfMassY::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SYSTEM_CM_Y);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SystemCenterOfMassY::Clone() const
//------------------------------------------------------------------------------
GmatBase* SystemCenterOfMassY::Clone() const
{
   return new SystemCenterOfMassY(*this);
}


//==============================================================================
//                              SystemCenterOfMassZ
//==============================================================================
/**
 * Implements SystemCenterOfMassZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SystemCenterOfMassZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SystemCenterOfMassZ::SystemCenterOfMassZ(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SystemCenterOfMassZ", obj,
                       "System CenterOfMass Z", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// SystemCenterOfMassZ(const SystemCenterOfMassZ &copy)
//------------------------------------------------------------------------------
SystemCenterOfMassZ::SystemCenterOfMassZ(const SystemCenterOfMassZ &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SystemCenterOfMassZ& operator=(const SystemCenterOfMassZ &right)
//------------------------------------------------------------------------------
SystemCenterOfMassZ& SystemCenterOfMassZ::operator=(const SystemCenterOfMassZ &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SystemCenterOfMassZ()
//------------------------------------------------------------------------------
SystemCenterOfMassZ::~SystemCenterOfMassZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SystemCenterOfMassZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SYSTEM_CM_Z);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SystemCenterOfMassZ::Clone() const
//------------------------------------------------------------------------------
GmatBase* SystemCenterOfMassZ::Clone() const
{
   return new SystemCenterOfMassZ(*this);
}


//==============================================================================
//                              SystemMomentOfInertiaXX
//==============================================================================
/**
 * Implements SystemMomentOfInertiaXX class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SystemMomentOfInertiaXX(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SystemMomentOfInertiaXX::SystemMomentOfInertiaXX(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SystemMomentOfInertiaXX", obj,
                       "System MomentOfInertia XX", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaXX(const SystemMomentOfInertiaXX &copy)
//------------------------------------------------------------------------------
SystemMomentOfInertiaXX::SystemMomentOfInertiaXX(const SystemMomentOfInertiaXX &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaXX& operator=(const SystemMomentOfInertiaXX &right)
//------------------------------------------------------------------------------
SystemMomentOfInertiaXX& SystemMomentOfInertiaXX::operator=(const SystemMomentOfInertiaXX &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SystemMomentOfInertiaXX()
//------------------------------------------------------------------------------
SystemMomentOfInertiaXX::~SystemMomentOfInertiaXX()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SystemMomentOfInertiaXX::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SYSTEM_MOI_XX);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SystemMomentOfInertiaXX::Clone() const
//------------------------------------------------------------------------------
GmatBase* SystemMomentOfInertiaXX::Clone() const
{
   return new SystemMomentOfInertiaXX(*this);
}


//==============================================================================
//                              SystemMomentOfInertiaXY
//==============================================================================
/**
 * Implements SystemMomentOfInertiaXY class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SystemMomentOfInertiaXY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SystemMomentOfInertiaXY::SystemMomentOfInertiaXY(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SystemMomentOfInertiaXY", obj,
                       "System MomentOfInertia XY", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaXY(const SystemMomentOfInertiaXY &copy)
//------------------------------------------------------------------------------
SystemMomentOfInertiaXY::SystemMomentOfInertiaXY(const SystemMomentOfInertiaXY &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaXY& operator=(const SystemMomentOfInertiaXY &right)
//------------------------------------------------------------------------------
SystemMomentOfInertiaXY& SystemMomentOfInertiaXY::operator=(const SystemMomentOfInertiaXY &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SystemMomentOfInertiaXY()
//------------------------------------------------------------------------------
SystemMomentOfInertiaXY::~SystemMomentOfInertiaXY()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SystemMomentOfInertiaXY::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SYSTEM_MOI_XY);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SystemMomentOfInertiaXY::Clone() const
//------------------------------------------------------------------------------
GmatBase* SystemMomentOfInertiaXY::Clone() const
{
   return new SystemMomentOfInertiaXY(*this);
}


//==============================================================================
//                              SystemMomentOfInertiaXZ
//==============================================================================
/**
 * Implements SystemMomentOfInertiaXZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SystemMomentOfInertiaXZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SystemMomentOfInertiaXZ::SystemMomentOfInertiaXZ(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SystemMomentOfInertiaXZ", obj,
                       "System MomentOfInertia XZ", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaXZ(const SystemMomentOfInertiaXZ &copy)
//------------------------------------------------------------------------------
SystemMomentOfInertiaXZ::SystemMomentOfInertiaXZ(const SystemMomentOfInertiaXZ &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaXZ& operator=(const SystemMomentOfInertiaXZ &right)
//------------------------------------------------------------------------------
SystemMomentOfInertiaXZ& SystemMomentOfInertiaXZ::operator=(const SystemMomentOfInertiaXZ &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SystemMomentOfInertiaXZ()
//------------------------------------------------------------------------------
SystemMomentOfInertiaXZ::~SystemMomentOfInertiaXZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SystemMomentOfInertiaXZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SYSTEM_MOI_XZ);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SystemMomentOfInertiaXZ::Clone() const
//------------------------------------------------------------------------------
GmatBase* SystemMomentOfInertiaXZ::Clone() const
{
   return new SystemMomentOfInertiaXZ(*this);
}


//==============================================================================
//                              SystemMomentOfInertiaYY
//==============================================================================
/**
 * Implements SystemMomentOfInertiaYY class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SystemMomentOfInertiaYY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SystemMomentOfInertiaYY::SystemMomentOfInertiaYY(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SystemMomentOfInertiaYY", obj,
                       "System MomentOfInertia YY", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaYY(const SystemMomentOfInertiaYY &copy)
//------------------------------------------------------------------------------
SystemMomentOfInertiaYY::SystemMomentOfInertiaYY(const SystemMomentOfInertiaYY &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaYY& operator=(const SystemMomentOfInertiaYY &right)
//------------------------------------------------------------------------------
SystemMomentOfInertiaYY& SystemMomentOfInertiaYY::operator=(const SystemMomentOfInertiaYY &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SystemMomentOfInertiaYY()
//------------------------------------------------------------------------------
SystemMomentOfInertiaYY::~SystemMomentOfInertiaYY()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SystemMomentOfInertiaYY::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SYSTEM_MOI_YY);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SystemMomentOfInertiaYY::Clone() const
//------------------------------------------------------------------------------
GmatBase* SystemMomentOfInertiaYY::Clone() const
{
   return new SystemMomentOfInertiaYY(*this);
}


//==============================================================================
//                              SystemMomentOfInertiaYZ
//==============================================================================
/**
 * Implements SystemMomentOfInertiaYZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SystemMomentOfInertiaYZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SystemMomentOfInertiaYZ::SystemMomentOfInertiaYZ(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SystemMomentOfInertiaYZ", obj,
                       "System MomentOfInertia YZ", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaYZ(const SystemMomentOfInertiaYZ &copy)
//------------------------------------------------------------------------------
SystemMomentOfInertiaYZ::SystemMomentOfInertiaYZ(const SystemMomentOfInertiaYZ &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaYZ& operator=(const SystemMomentOfInertiaYZ &right)
//------------------------------------------------------------------------------
SystemMomentOfInertiaYZ& SystemMomentOfInertiaYZ::operator=(const SystemMomentOfInertiaYZ &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SystemMomentOfInertiaYZ()
//------------------------------------------------------------------------------
SystemMomentOfInertiaYZ::~SystemMomentOfInertiaYZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SystemMomentOfInertiaYZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SYSTEM_MOI_YZ);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SystemMomentOfInertiaYZ::Clone() const
//------------------------------------------------------------------------------
GmatBase* SystemMomentOfInertiaYZ::Clone() const
{
   return new SystemMomentOfInertiaYZ(*this);
}


//==============================================================================
//                              SystemMomentOfInertiaZZ
//==============================================================================
/**
 * Implements SystemMomentOfInertiaZZ class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SystemMomentOfInertiaZZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SystemMomentOfInertiaZZ::SystemMomentOfInertiaZZ(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SystemMomentOfInertiaZZ", obj,
                       "System MomentOfInertia ZZ", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaZZ(const SystemMomentOfInertiaZZ &copy)
//------------------------------------------------------------------------------
SystemMomentOfInertiaZZ::SystemMomentOfInertiaZZ(const SystemMomentOfInertiaZZ &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SystemMomentOfInertiaZZ& operator=(const SystemMomentOfInertiaZZ &right)
//------------------------------------------------------------------------------
SystemMomentOfInertiaZZ& SystemMomentOfInertiaZZ::operator=(const SystemMomentOfInertiaZZ &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SystemMomentOfInertiaZZ()
//------------------------------------------------------------------------------
SystemMomentOfInertiaZZ::~SystemMomentOfInertiaZZ()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SystemMomentOfInertiaZZ::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SYSTEM_MOI_ZZ);


   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SystemMomentOfInertiaZZ::Clone() const
//------------------------------------------------------------------------------
GmatBase* SystemMomentOfInertiaZZ::Clone() const
{
   return new SystemMomentOfInertiaZZ(*this);
}


//==============================================================================
//                              SpadDragScaleFactor
//==============================================================================
/**
* Implements SpadDragScaleFactor class.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SpadDragScaleFactor(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SpadDragScaleFactor::SpadDragScaleFactor(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SPADDragScaleFactor", obj, "SPAD Drag Scale Factor", "")
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::BLUE32;
#endif
}


//------------------------------------------------------------------------------
// SpadDragScaleFactor(const SpadDragScaleFactor &copy)
//------------------------------------------------------------------------------
SpadDragScaleFactor::SpadDragScaleFactor(const SpadDragScaleFactor &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SpadDragScaleFactor& operator=(const SpadDragScaleFactor &right)
//------------------------------------------------------------------------------
SpadDragScaleFactor& SpadDragScaleFactor::operator=(const SpadDragScaleFactor &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SpadDragScaleFactor()
//------------------------------------------------------------------------------
SpadDragScaleFactor::~SpadDragScaleFactor()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SpadDragScaleFactor::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SPAD_DRAG_COEFF);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SpadDragScaleFactor::Clone() const
//------------------------------------------------------------------------------
GmatBase* SpadDragScaleFactor::Clone() const
{
   return new SpadDragScaleFactor(*this);
}


//==============================================================================
//                              SpadSRPScaleFactor
//==============================================================================
/**
* Implements SpadSRPScaleFactor class.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SpadSRPScaleFactor(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SpadSRPScaleFactor::SpadSRPScaleFactor(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SPADSRPScaleFactor", obj, "SPAD SRP Scale Factor", "")
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::BLUE32;
#endif
}


//------------------------------------------------------------------------------
// SpadSRPScaleFactor(const SpadSRPScaleFactor &copy)
//------------------------------------------------------------------------------
SpadSRPScaleFactor::SpadSRPScaleFactor(const SpadSRPScaleFactor &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SpadSRPScaleFactor& operator=(const SpadSRPScaleFactor &right)
//------------------------------------------------------------------------------
SpadSRPScaleFactor& SpadSRPScaleFactor::operator=(const SpadSRPScaleFactor &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SpadSRPScaleFactor()
//------------------------------------------------------------------------------
SpadSRPScaleFactor::~SpadSRPScaleFactor()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SpadSRPScaleFactor::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SPAD_REFLECT_COEFF);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SpadSRPScaleFactor::Clone() const
//------------------------------------------------------------------------------
GmatBase* SpadSRPScaleFactor::Clone() const
{
   return new SpadSRPScaleFactor(*this);
}


//==============================================================================
//                       AtmosDensityScaleFactor
//==============================================================================
/**
 * Implements AtmosDensityScaleFactor class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // AtmosDensityScaleFactor(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
AtmosDensityScaleFactor::AtmosDensityScaleFactor(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "AtmosDensityScaleFactor", obj, "Atmospheric Density Scale Factor", "")
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::YELLOW32;
#endif
}


//------------------------------------------------------------------------------
// AtmosDensityScaleFactor(const AtmosDensityScaleFactor &copy)
//------------------------------------------------------------------------------
AtmosDensityScaleFactor::AtmosDensityScaleFactor(const AtmosDensityScaleFactor &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// AtmosDensityScaleFactor& operator=(const AtmosDensityScaleFactor &right)
//------------------------------------------------------------------------------
AtmosDensityScaleFactor& AtmosDensityScaleFactor::operator=(const AtmosDensityScaleFactor &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~AtmosDensityScaleFactor()
//------------------------------------------------------------------------------
AtmosDensityScaleFactor::~AtmosDensityScaleFactor()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool AtmosDensityScaleFactor::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(ATMOS_DENSITY_SCALE_FACTOR);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* AtmosDensityScaleFactor::Clone() const
//------------------------------------------------------------------------------
GmatBase* AtmosDensityScaleFactor::Clone() const
{
   return new AtmosDensityScaleFactor(*this);
}


//==============================================================================
//                              DragCoeffSigma
//==============================================================================
/**
 * Implements DragCoeffSigma class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // DragCoeffSigma(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
DragCoeffSigma::DragCoeffSigma(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "CdSigma", obj, "Drag Coefficient Sigma", "")
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::YELLOW32;
#endif
}


//------------------------------------------------------------------------------
// DragCoeffSigma(const DragCoeffSigma &copy)
//------------------------------------------------------------------------------
DragCoeffSigma::DragCoeffSigma(const DragCoeffSigma &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// DragCoeffSigma& operator=(const DragCoeffSigma &right)
//------------------------------------------------------------------------------
DragCoeffSigma& DragCoeffSigma::operator=(const DragCoeffSigma &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~DragCoeffSigma()
//------------------------------------------------------------------------------
DragCoeffSigma::~DragCoeffSigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DragCoeffSigma::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(DRAG_COEFF_SIGMA);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* DragCoeffSigma::Clone() const
//------------------------------------------------------------------------------
GmatBase* DragCoeffSigma::Clone() const
{
   return new DragCoeffSigma(*this);
}


//==============================================================================
//                              ReflectCoeffSigma
//==============================================================================
/**
 * Implements ReflectCoeffSigma class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // ReflectCoeffSigma(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
ReflectCoeffSigma::ReflectCoeffSigma(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "CrSigma", obj, "Reflectivity Coefficient Sigma", "")
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::BLUE32;
#endif
}


//------------------------------------------------------------------------------
// ReflectCoeffSigma(const ReflectCoeffSigma &copy)
//------------------------------------------------------------------------------
ReflectCoeffSigma::ReflectCoeffSigma(const ReflectCoeffSigma &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// ReflectCoeffSigma& operator=(const ReflectCoeffSigma &right)
//------------------------------------------------------------------------------
ReflectCoeffSigma& ReflectCoeffSigma::operator=(const ReflectCoeffSigma &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ReflectCoeffSigma()
//------------------------------------------------------------------------------
ReflectCoeffSigma::~ReflectCoeffSigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ReflectCoeffSigma::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(REFLECT_COEFF_SIGMA);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* ReflectCoeffSigma::Clone() const
//------------------------------------------------------------------------------
GmatBase* ReflectCoeffSigma::Clone() const
{
   return new ReflectCoeffSigma(*this);
}


//==============================================================================
//                              SpadDragScaleFactorSigma
//==============================================================================
/**
* Implements SpadDragScaleFactorSigma class.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SpadDragScaleFactorSigma(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SpadDragScaleFactorSigma::SpadDragScaleFactorSigma(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SPADDragScaleFactorSigma", obj, "SPAD Drag Scale Factor Sigma", "")
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::BLUE32;
#endif
}


//------------------------------------------------------------------------------
// SpadDragScaleFactorSigma(const SpadDragScaleFactorSigma &copy)
//------------------------------------------------------------------------------
SpadDragScaleFactorSigma::SpadDragScaleFactorSigma(const SpadDragScaleFactorSigma &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SpadDragScaleFactorSigma& operator=(const SpadDragScaleFactorSigma &right)
//------------------------------------------------------------------------------
SpadDragScaleFactorSigma& SpadDragScaleFactorSigma::operator=(const SpadDragScaleFactorSigma &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SpadDragScaleFactorSigma()
//------------------------------------------------------------------------------
SpadDragScaleFactorSigma::~SpadDragScaleFactorSigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SpadDragScaleFactorSigma::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SPAD_DRAG_COEFF_SIGMA);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SpadDragScaleFactorSigma::Clone() const
//------------------------------------------------------------------------------
GmatBase* SpadDragScaleFactorSigma::Clone() const
{
   return new SpadDragScaleFactorSigma(*this);
}



//==============================================================================
//                              SpadSRPScaleFactorSigma
//==============================================================================
/**
* Implements SpadSRPScaleFactorSigma class.
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SpadSRPScaleFactorSigma(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
SpadSRPScaleFactorSigma::SpadSRPScaleFactorSigma(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "SPADSRPScaleFactorSigma", obj, "SPAD SRP Scale Factor Sigma", "")
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::BLUE32;
#endif
}


//------------------------------------------------------------------------------
// SpadSRPScaleFactorSigma(const SpadSRPScaleFactorSigma &copy)
//------------------------------------------------------------------------------
SpadSRPScaleFactorSigma::SpadSRPScaleFactorSigma(const SpadSRPScaleFactorSigma &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// SpadSRPScaleFactorSigma& operator=(const SpadSRPScaleFactorSigma &right)
//------------------------------------------------------------------------------
SpadSRPScaleFactorSigma& SpadSRPScaleFactorSigma::operator=(const SpadSRPScaleFactorSigma &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~SpadSRPScaleFactorSigma()
//------------------------------------------------------------------------------
SpadSRPScaleFactorSigma::~SpadSRPScaleFactorSigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool SpadSRPScaleFactorSigma::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(SPAD_REFLECT_COEFF_SIGMA);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* SpadSRPScaleFactorSigma::Clone() const
//------------------------------------------------------------------------------
GmatBase* SpadSRPScaleFactorSigma::Clone() const
{
   return new SpadSRPScaleFactorSigma(*this);
}


//==============================================================================
//                       AtmosDensityScaleFactorSigma
//==============================================================================
/**
 * Implements AtmosDensityScaleFactorSigma class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // AtmosDensityScaleFactorSigma(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
AtmosDensityScaleFactorSigma::AtmosDensityScaleFactorSigma(const std::string &name, GmatBase *obj)
   : BallisticMassReal(name, "AtmosDensityScaleFactorSigma", obj, "Atmospheric Density Scale Factor Sigma", "")
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::YELLOW32;
#endif
}


//------------------------------------------------------------------------------
// AtmosDensityScaleFactorSigma(const AtmosDensityScaleFactorSigma &copy)
//------------------------------------------------------------------------------
AtmosDensityScaleFactorSigma::AtmosDensityScaleFactorSigma(const AtmosDensityScaleFactorSigma &copy)
   : BallisticMassReal(copy)
{
}


//------------------------------------------------------------------------------
// AtmosDensityScaleFactorSigma& operator=(const AtmosDensityScaleFactorSigma &right)
//------------------------------------------------------------------------------
AtmosDensityScaleFactorSigma& AtmosDensityScaleFactorSigma::operator=(const AtmosDensityScaleFactorSigma &right)
{
   if (this != &right)
      BallisticMassReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~AtmosDensityScaleFactorSigma()
//------------------------------------------------------------------------------
AtmosDensityScaleFactorSigma::~AtmosDensityScaleFactorSigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool AtmosDensityScaleFactorSigma::Evaluate()
{
   mRealValue = SpacecraftData::GetReal(ATMOS_DENSITY_SCALE_FACTOR_SIGMA);

   if (mRealValue == GmatBase::REAL_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// GmatBase* AtmosDensityScaleFactorSigma::Clone() const
//------------------------------------------------------------------------------
GmatBase* AtmosDensityScaleFactorSigma::Clone() const
{
   return new AtmosDensityScaleFactorSigma(*this);
}


