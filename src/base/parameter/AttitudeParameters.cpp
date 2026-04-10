//$Id$
//------------------------------------------------------------------------------
//                                  AttitudeParameters
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
// Developed further jointly by NASA/GSFC, Thinking Systems, Inc., and 
// Schafer Corp., under AFRL NOVA Contract #FA945104D03990003
//
//
// Author: Daniel Hunter
// Created: 2006/6/26
// Modified:  Dunn Idle (added MRPs)
// Date:      2010/08/24
//
/**
 * Implements Attitude related parameter classes.
 *    DCM11, DCM12, DCM13, DCM21, DCM22, DCM23, DCM31, DCM32, DCM33,
 *    Quat1, Quat2, Quat3, Quat4, EulerAngle1, EulerAngle2, EulerAngle3,
 *    MRP1 , MRP2 , MRP3 , AngularVelocityX, AngularVelocityY, AngularVelocityZ,
 *    EulerAngleRate1, EulerAngleRate2, EulerAngleRate3
 */
//------------------------------------------------------------------------------

#include "AttitudeParameters.hpp"
#include "ColorTypes.hpp"


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Direction Cosine Matrix  ---------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  DCM11
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// DCM11(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DCM11::DCM11(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "DCM11", obj, "Direction Cosine Matrix 1,1", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED;
   #endif
}
//------------------------------------------------------------------------------
// DCM11(const DCM11 &copy)
//------------------------------------------------------------------------------
DCM11::DCM11(const DCM11 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// DCM11& operator=(const DCM11 &right)
//------------------------------------------------------------------------------
DCM11& DCM11::operator=(const DCM11 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~DCM11()
//------------------------------------------------------------------------------
DCM11::~DCM11()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DCM11::Evaluate()
{
   mRealValue = AttitudeData::GetReal(DCM_11);


   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void DCM11::SetReal(Real val)
{
   AttitudeData::SetReal(DCM_11, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* DCM11::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DCM11::Clone(void) const
{
   return new DCM11(*this);
}

//------------------------------------------------------------------------------
//  DCM12
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// DCM12(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DCM12::DCM12(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "DCM12", obj, "Direction Cosine Matrix 1,2", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW;
   #endif
}
//------------------------------------------------------------------------------
// DCM12(const DCM12 &copy)
//------------------------------------------------------------------------------
DCM12::DCM12(const DCM12 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// DCM12& operator=(const DCM12 &right)
//------------------------------------------------------------------------------
DCM12& DCM12::operator=(const DCM12 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~DCM12()
//------------------------------------------------------------------------------
DCM12::~DCM12()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DCM12::Evaluate()
{
   mRealValue = AttitudeData::GetReal(DCM_12);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void DCM12::SetReal(Real val)
{
   AttitudeData::SetReal(DCM_12, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* DCM12::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DCM12::Clone(void) const
{
   return new DCM12(*this);
}

//------------------------------------------------------------------------------
//  DCM13
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// DCM13(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DCM13::DCM13(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "DCM13", obj, "Direction Cosine Matrix 1,3", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// DCM13(const DCM13 &copy)
//------------------------------------------------------------------------------
DCM13::DCM13(const DCM13 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// DCM13& operator=(const DCM13 &right)
//------------------------------------------------------------------------------
DCM13& DCM13::operator=(const DCM13 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~DCM13()
//------------------------------------------------------------------------------
DCM13::~DCM13()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DCM13::Evaluate()
{
   mRealValue = AttitudeData::GetReal(DCM_13);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void DCM13::SetReal(Real val)
{
   AttitudeData::SetReal(DCM_13, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* DCM13::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DCM13::Clone(void) const
{
   return new DCM13(*this);
}

//------------------------------------------------------------------------------
//  DCM21
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// DCM21(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DCM21::DCM21(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "DCM21", obj, "Direction Cosine Matrix 2,1", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN;
   #endif
}
//------------------------------------------------------------------------------
// DCM21(const DCM21 &copy)
//------------------------------------------------------------------------------
DCM21::DCM21(const DCM21 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// DCM21& operator=(const DCM21 &right)
//------------------------------------------------------------------------------
DCM21& DCM21::operator=(const DCM21 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~DCM21()
//------------------------------------------------------------------------------
DCM21::~DCM21()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DCM21::Evaluate()
{
   mRealValue = AttitudeData::GetReal(DCM_21);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void DCM21::SetReal(Real val)
{
   AttitudeData::SetReal(DCM_21, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* DCM21::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DCM21::Clone(void) const
{
   return new DCM21(*this);
}

//------------------------------------------------------------------------------
//  DCM22
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// DCM22(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DCM22::DCM22(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "DCM22", obj, "Direction Cosine Matrix 2,2", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::ORANGE;
   #endif
}
//------------------------------------------------------------------------------
// DCM22(const DCM22 &copy)
//------------------------------------------------------------------------------
DCM22::DCM22(const DCM22 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// DCM22& operator=(const DCM22 &right)
//------------------------------------------------------------------------------
DCM22& DCM22::operator=(const DCM22 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~DCM22()
//------------------------------------------------------------------------------
DCM22::~DCM22()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DCM22::Evaluate()
{
   mRealValue = AttitudeData::GetReal(DCM_22);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void DCM22::SetReal(Real val)
{
   AttitudeData::SetReal(DCM_22, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* DCM22::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DCM22::Clone(void) const
{
   return new DCM22(*this);
}

//------------------------------------------------------------------------------
//  DCM23
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// DCM23(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DCM23::DCM23(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "DCM23", obj, "Direction Cosine Matrix 2,3", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
}
//------------------------------------------------------------------------------
// DCM23(const DCM23 &copy)
//------------------------------------------------------------------------------
DCM23::DCM23(const DCM23 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// DCM23& operator=(const DCM23 &right)
//------------------------------------------------------------------------------
DCM23& DCM23::operator=(const DCM23 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~DCM23()
//------------------------------------------------------------------------------
DCM23::~DCM23()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DCM23::Evaluate()
{
   mRealValue = AttitudeData::GetReal(DCM_23);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void DCM23::SetReal(Real val)
{
   AttitudeData::SetReal(DCM_23, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* DCM23::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DCM23::Clone(void) const
{
   return new DCM23(*this);
}

//------------------------------------------------------------------------------
//  DCM31
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// DCM31(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DCM31::DCM31(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "DCM31", obj, "Direction Cosine Matrix 3,1", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::PURPLE;
   #endif
}
//------------------------------------------------------------------------------
// DCM31(const DCM31 &copy)
//------------------------------------------------------------------------------
DCM31::DCM31(const DCM31 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// DCM31& operator=(const DCM31 &right)
//------------------------------------------------------------------------------
DCM31& DCM31::operator=(const DCM31 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~DCM31()
//------------------------------------------------------------------------------
DCM31::~DCM31()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DCM31::Evaluate()
{
   mRealValue = AttitudeData::GetReal(DCM_31);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void DCM31::SetReal(Real val)
{
   AttitudeData::SetReal(DCM_31, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* DCM31::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DCM31::Clone(void) const
{
   return new DCM31(*this);
}

//------------------------------------------------------------------------------
//  DCM32
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// DCM32(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DCM32::DCM32(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "DCM32", obj, "Direction Cosine Matrix 3,2", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::LIME;
   #endif
}
//------------------------------------------------------------------------------
// DCM32(const DCM32 &copy)
//------------------------------------------------------------------------------
DCM32::DCM32(const DCM32 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// DCM32& operator=(const DCM32 &right)
//------------------------------------------------------------------------------
DCM32& DCM32::operator=(const DCM32 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~DCM32()
//------------------------------------------------------------------------------
DCM32::~DCM32()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DCM32::Evaluate()
{
   mRealValue = AttitudeData::GetReal(DCM_32);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void DCM32::SetReal(Real val)
{
   AttitudeData::SetReal(DCM_32, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* DCM32::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DCM32::Clone(void) const
{
   return new DCM32(*this);
}

//------------------------------------------------------------------------------
//  DCM33
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// DCM33(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
DCM33::DCM33(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "DCM33", obj, "Direction Cosine Matrix 3,3", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::AQUA;
   #endif
}
//------------------------------------------------------------------------------
// DCM33(const DCM33 &copy)
//------------------------------------------------------------------------------
DCM33::DCM33(const DCM33 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// DCM33& operator=(const DCM33 &right)
//------------------------------------------------------------------------------
DCM33& DCM33::operator=(const DCM33 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~DCM33()
//------------------------------------------------------------------------------
DCM33::~DCM33()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DCM33::Evaluate()
{
   mRealValue = AttitudeData::GetReal(DCM_33);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void DCM33::SetReal(Real val)
{
   AttitudeData::SetReal(DCM_33, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* DCM33::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* DCM33::Clone(void) const
{
   return new DCM33(*this);
}


//------------------------------------------------------------------------------
// DirectionCosineMatrix(const std::string &name = "", GmatBase *obj = NULL)
//------------------------------------------------------------------------------
DirectionCosineMatrix::DirectionCosineMatrix(const std::string &name,
                                             GmatBase *obj)
   : AttitudeRmat33(name, "DirectionCosineMatrix", obj,
                    "Direction Cosine Matrix", "", GmatParam::NO_DEP, false)
{
}

//------------------------------------------------------------------------------
// DirectionCosineMatrix(const DirectionCosineMatrix &copy)
//------------------------------------------------------------------------------
DirectionCosineMatrix::DirectionCosineMatrix(const DirectionCosineMatrix &copy)
   : AttitudeRmat33(copy)
{
}

//------------------------------------------------------------------------------
// DirectionCosineMatrix& operator=(const DirectionCosineMatrix &right)
//------------------------------------------------------------------------------
DirectionCosineMatrix& DirectionCosineMatrix::operator=(const DirectionCosineMatrix &right)
{
   if (this != &right)
      AttitudeRmat33::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~DirectionCosineMatrix()
//------------------------------------------------------------------------------
DirectionCosineMatrix::~DirectionCosineMatrix()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool DirectionCosineMatrix::Evaluate()
{
   mRmat33Value = AttitudeData::GetRmatrix33(DIRECTION_COSINE_MATRIX);

   if (mRmat33Value == GmatBase::RMATRIX_PARAMETER_UNDEFINED)
      return false;
   else
      return true;
}

GmatBase* DirectionCosineMatrix::Clone(void) const
{
   return new DirectionCosineMatrix(*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Euler Angles  --------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  EulerAngle1
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// EulerAngle1::EulerAngle1(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EulerAngle1::EulerAngle1(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "EulerAngle1", obj, "Euler Angle 1", "deg")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED;
   #endif
   mIsAngleParam = true;
   mCycleType = GmatParam::PLUS_MINUS_180;
}
//------------------------------------------------------------------------------
// EulerAngle1(const EulerAngle1 &copy)
//------------------------------------------------------------------------------
EulerAngle1::EulerAngle1(const EulerAngle1 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// EulerAngle1& operator=(const EulerAngle1 &right)
//------------------------------------------------------------------------------
EulerAngle1& EulerAngle1::operator=(const EulerAngle1 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~EulerAngle1()
//------------------------------------------------------------------------------
EulerAngle1::~EulerAngle1()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool EulerAngle1::Evaluate()
{
   mRealValue = AttitudeData::GetReal(EULER_ANGLE_1);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void EulerAngle1::SetReal(Real val)
{
   AttitudeData::SetReal(EULER_ANGLE_1, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EulerAngle1::Clone(void) const
{
   return new EulerAngle1(*this);
}

//------------------------------------------------------------------------------
//  EulerAngle2
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// EulerAngle2::EulerAngle2(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EulerAngle2::EulerAngle2(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "EulerAngle2", obj, "Euler Angle 2", "deg")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW;
   #endif
   mIsAngleParam = true;
   mCycleType = GmatParam::PLUS_MINUS_180;
}
//------------------------------------------------------------------------------
// EulerAngle2(const EulerAngle2 &copy)
//------------------------------------------------------------------------------
EulerAngle2::EulerAngle2(const EulerAngle2 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// EulerAngle2& operator=(const EulerAngle2 &right)
//------------------------------------------------------------------------------
EulerAngle2& EulerAngle2::operator=(const EulerAngle2 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~EulerAngle2()
//------------------------------------------------------------------------------
EulerAngle2::~EulerAngle2()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool EulerAngle2::Evaluate()
{
   mRealValue = AttitudeData::GetReal(EULER_ANGLE_2);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void EulerAngle2::SetReal(Real val)
{
   AttitudeData::SetReal(EULER_ANGLE_2, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EulerAngle2::Clone(void) const
{
   return new EulerAngle2(*this);
}

//------------------------------------------------------------------------------
//  EulerAngle3
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// EulerAngle3::EulerAngle3(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EulerAngle3::EulerAngle3(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "EulerAngle3", obj, "Euler Angle 3", "deg")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
   mIsAngleParam = true;
   mCycleType = GmatParam::PLUS_MINUS_180;
}
//------------------------------------------------------------------------------
// EulerAngle3(const EulerAngle3 &copy)
//------------------------------------------------------------------------------
EulerAngle3::EulerAngle3(const EulerAngle3 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// EulerAngle3& operator=(const EulerAngle3 &right)
//------------------------------------------------------------------------------
EulerAngle3& EulerAngle3::operator=(const EulerAngle3 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~EulerAngle3()
//------------------------------------------------------------------------------
EulerAngle3::~EulerAngle3()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool EulerAngle3::Evaluate()
{
   mRealValue = AttitudeData::GetReal(EULER_ANGLE_3);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void EulerAngle3::SetReal(Real val)
{
   AttitudeData::SetReal(EULER_ANGLE_3, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EulerAngle3::Clone(void) const
{
   return new EulerAngle3(*this);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Body Alignment Vectors -----------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  BodyAlignmentVectorX
//------------------------------------------------------------------------------
BodyAlignmentVectorX::BodyAlignmentVectorX(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "BodyAlignmentVectorX", obj, "Body Alignment Vector X", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// BodyAlignmentVectorX(const BodyAlignmentVectorX &copy)
//------------------------------------------------------------------------------
BodyAlignmentVectorX::BodyAlignmentVectorX(const BodyAlignmentVectorX &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// BodyAlignmentVectorX& operator=(const BodyAlignmentVectorX &right)
//------------------------------------------------------------------------------
BodyAlignmentVectorX& BodyAlignmentVectorX::operator=(const BodyAlignmentVectorX &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~BodyAlignmentVectorX()
//------------------------------------------------------------------------------
BodyAlignmentVectorX::~BodyAlignmentVectorX()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool BodyAlignmentVectorX::Evaluate()
{

   mRealValue = AttitudeData::GetReal(BODY_ALIGNMENT_VECTOR_X);
   return true;
}
//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void BodyAlignmentVectorX::SetReal(Real val)
{
   AttitudeData::SetReal(BODY_ALIGNMENT_VECTOR_X, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BodyAlignmentVectorX::Clone(void) const
{
   return new BodyAlignmentVectorX(*this);
}

//------------------------------------------------------------------------------
//  BodyAlignmentVectorY
//------------------------------------------------------------------------------
BodyAlignmentVectorY::BodyAlignmentVectorY(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "BodyAlignmentVectorY", obj, "Body Alignment Vector Y", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// BodyAlignmentVectorY(const BodyAlignmentVectorY &copy)
//------------------------------------------------------------------------------
BodyAlignmentVectorY::BodyAlignmentVectorY(const BodyAlignmentVectorY &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// BodyAlignmentVectorY& operator=(const BodyAlignmentVectorY &right)
//------------------------------------------------------------------------------
BodyAlignmentVectorY& BodyAlignmentVectorY::operator=(const BodyAlignmentVectorY &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~BodyAlignmentVectorY()
//------------------------------------------------------------------------------
BodyAlignmentVectorY::~BodyAlignmentVectorY()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool BodyAlignmentVectorY::Evaluate()
{
   mRealValue = AttitudeData::GetReal(BODY_ALIGNMENT_VECTOR_Y);
   return true;
}
//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void BodyAlignmentVectorY::SetReal(Real val)
{
   AttitudeData::SetReal(BODY_ALIGNMENT_VECTOR_Y, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BodyAlignmentVectorY::Clone(void) const
{
   return new BodyAlignmentVectorY(*this);
}

//------------------------------------------------------------------------------
//  BodyAlignmentVectorZ
//------------------------------------------------------------------------------
BodyAlignmentVectorZ::BodyAlignmentVectorZ(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "BodyAlignmentVectorZ", obj, "Body Alignment Vector Z", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// BodyAlignmentVectorZ(const BodyAlignmentVectorZ &copy)
//------------------------------------------------------------------------------
BodyAlignmentVectorZ::BodyAlignmentVectorZ(const BodyAlignmentVectorZ &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// BodyAlignmentVectorZ& operator=(const BodyAlignmentVectorZ &right)
//------------------------------------------------------------------------------
BodyAlignmentVectorZ& BodyAlignmentVectorZ::operator=(const BodyAlignmentVectorZ &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~BodyAlignmentVectorZ()
//------------------------------------------------------------------------------
BodyAlignmentVectorZ::~BodyAlignmentVectorZ()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool BodyAlignmentVectorZ::Evaluate()
{
   mRealValue = AttitudeData::GetReal(BODY_ALIGNMENT_VECTOR_Z);
   return true;
}
//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void BodyAlignmentVectorZ::SetReal(Real val)
{
   AttitudeData::SetReal(BODY_ALIGNMENT_VECTOR_Z, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BodyAlignmentVectorZ::Clone(void) const
{
   return new BodyAlignmentVectorZ(*this);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Body Constraint Vectors ----------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  BodyConstraintVectorX
//------------------------------------------------------------------------------
BodyConstraintVectorX::BodyConstraintVectorX(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "BodyConstraintVectorX", obj, "Body Constraint Vector X", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// BodyConstraintVectorX(const BodyConstraintVectorX &copy)
//------------------------------------------------------------------------------
BodyConstraintVectorX::BodyConstraintVectorX(const BodyConstraintVectorX &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// BodyConstraintVectorX& operator=(const BodyConstraintVectorX &right)
//------------------------------------------------------------------------------
BodyConstraintVectorX& BodyConstraintVectorX::operator=(const BodyConstraintVectorX &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~BodyConstraintVectorX()
//------------------------------------------------------------------------------
BodyConstraintVectorX::~BodyConstraintVectorX()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool BodyConstraintVectorX::Evaluate()
{
   mRealValue = AttitudeData::GetReal(BODY_CONSTRAINT_VECTOR_X);
   return true;
}
//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void BodyConstraintVectorX::SetReal(Real val)
{
   AttitudeData::SetReal(BODY_CONSTRAINT_VECTOR_X, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BodyConstraintVectorX::Clone(void) const
{
   return new BodyConstraintVectorX(*this);
}

//------------------------------------------------------------------------------
//  BodyConstraintVectorY
//------------------------------------------------------------------------------
BodyConstraintVectorY::BodyConstraintVectorY(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "BodyConstraintVectorY", obj, "Body Constraint Vector Y", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// BodyConstraintVectorY(const BodyConstraintVectorY &copy)
//------------------------------------------------------------------------------
BodyConstraintVectorY::BodyConstraintVectorY(const BodyConstraintVectorY &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// BodyConstraintVectorY& operator=(const BodyConstraintVectorY &right)
//------------------------------------------------------------------------------
BodyConstraintVectorY& BodyConstraintVectorY::operator=(const BodyConstraintVectorY &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~BodyConstraintVectorY()
//------------------------------------------------------------------------------
BodyConstraintVectorY::~BodyConstraintVectorY()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool BodyConstraintVectorY::Evaluate()
{
   mRealValue = AttitudeData::GetReal(BODY_CONSTRAINT_VECTOR_Y);
   return true;
}
//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void BodyConstraintVectorY::SetReal(Real val)
{
   AttitudeData::SetReal(BODY_CONSTRAINT_VECTOR_Y, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BodyConstraintVectorY::Clone(void) const
{
   return new BodyConstraintVectorY(*this);
}

//------------------------------------------------------------------------------
//  BodyConstraintVectorZ
//------------------------------------------------------------------------------
BodyConstraintVectorZ::BodyConstraintVectorZ(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "BodyConstraintVectorZ", obj, "Body Constraint Vector Z", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// BodyConstraintVectorZ(const BodyConstraintVectorZ &copy)
//------------------------------------------------------------------------------
BodyConstraintVectorZ::BodyConstraintVectorZ(const BodyConstraintVectorZ &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// BodyConstraintVectorZ& operator=(const BodyConstraintVectorZ &right)
//------------------------------------------------------------------------------
BodyConstraintVectorZ& BodyConstraintVectorZ::operator=(const BodyConstraintVectorZ &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~BodyConstraintVectorZ()
//------------------------------------------------------------------------------
BodyConstraintVectorZ::~BodyConstraintVectorZ()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool BodyConstraintVectorZ::Evaluate()
{
   mRealValue = AttitudeData::GetReal(BODY_CONSTRAINT_VECTOR_Z);
   return true;
}
//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
//------------------------------------------------------------------------------
void BodyConstraintVectorZ::SetReal(Real val)
{
   AttitudeData::SetReal(BODY_CONSTRAINT_VECTOR_Z, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* BodyConstraintVectorZ::Clone(void) const
{
   return new BodyConstraintVectorZ(*this);
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Attitude Strings -----------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Attitude
//------------------------------------------------------------------------------
AttitudeParam::AttitudeParam(const std::string &name, GmatBase *obj)
   : AttitudeString(name, "Attitude", obj, "Attitude", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// Attitude(const Attitude &copy)
//------------------------------------------------------------------------------
AttitudeParam::AttitudeParam(const AttitudeParam &copy)
   : AttitudeString(copy)
{
}
//------------------------------------------------------------------------------
// Attitude& operator=(const Attitude &right)
//------------------------------------------------------------------------------
AttitudeParam& AttitudeParam::operator=(const AttitudeParam &right)
{
   if (this != &right)
      AttitudeString::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~Attitude()
//------------------------------------------------------------------------------
AttitudeParam::~AttitudeParam()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool AttitudeParam::Evaluate()
{
   mStringValue = AttitudeData::GetString(ATTITUDE_PARAM);
   return true;
}
//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AttitudeParam::SetString(std::string val)
{
   StringVar::SetString(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AttitudeParam::Clone(void) const
{
   return new AttitudeParam(*this);
}


//------------------------------------------------------------------------------
//  AttitudeReferenceBody
//------------------------------------------------------------------------------
AttitudeReferenceBody::AttitudeReferenceBody(const std::string &name, GmatBase *obj)
   : AttitudeString(name, "Attitude", obj, "Attitude", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// AttitudeReferenceBody(const AttitudeReferenceBody &copy)
//------------------------------------------------------------------------------
AttitudeReferenceBody::AttitudeReferenceBody(const AttitudeReferenceBody &copy)
   : AttitudeString(copy)
{
}
//------------------------------------------------------------------------------
// AttitudeReferenceBody& operator=(const AttitudeReferenceBody &right)
//------------------------------------------------------------------------------
AttitudeReferenceBody& AttitudeReferenceBody::operator=(const AttitudeReferenceBody &right)
{
   if (this != &right)
      AttitudeString::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~AttitudeReferenceBody()
//------------------------------------------------------------------------------
AttitudeReferenceBody::~AttitudeReferenceBody()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool AttitudeReferenceBody::Evaluate()
{
   mStringValue = AttitudeData::GetString(ATTITUDE_REFERENCE_BODY_ID);
   return true;
}
//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AttitudeReferenceBody::SetString(std::string val)
{
   StringVar::SetString(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AttitudeReferenceBody::Clone(void) const
{
   return new AttitudeReferenceBody(*this);
}

//------------------------------------------------------------------------------
//  AttitudeConstraintType
//------------------------------------------------------------------------------
AttitudeConstraintType::AttitudeConstraintType(const std::string &name, GmatBase *obj)
   : AttitudeString(name, "Attitude", obj, "Attitude", "")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// AttitudeConstraintType(const AttitudeConstraintType &copy)
//------------------------------------------------------------------------------
AttitudeConstraintType::AttitudeConstraintType(const AttitudeConstraintType &copy)
   : AttitudeString(copy)
{
}
//------------------------------------------------------------------------------
// AttitudeConstraintType& operator=(const AttitudeConstraintType &right)
//------------------------------------------------------------------------------
AttitudeConstraintType& AttitudeConstraintType::operator=(const AttitudeConstraintType &right)
{
   if (this != &right)
      AttitudeString::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~AttitudeConstraintType()
//------------------------------------------------------------------------------
AttitudeConstraintType::~AttitudeConstraintType()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool AttitudeConstraintType::Evaluate()
{
   mStringValue = AttitudeData::GetString(ATTITUDE_CONSTRAINT_TYPE_ID);
   return true;
}
//------------------------------------------------------------------------------
// virtual void SetReal(Real val)
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void AttitudeConstraintType::SetString(std::string val)
{
   StringVar::SetString(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AttitudeConstraintType::Clone(void) const
{
   return new AttitudeConstraintType(*this);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Modified Rodrigues Parameters - Added by Dunn ------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  MRP1
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// MRP1::MRP1(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
MRP1::MRP1(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "MRP1", obj, "Mod Rod Param 1", "")
{
   mColor = GmatColor::RED;
}
//------------------------------------------------------------------------------
// MRP1(const MRP1 &copy)
//------------------------------------------------------------------------------
MRP1::MRP1(const MRP1 &copy)
: AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// MRP1& operator=(const MRP1 &right)
//------------------------------------------------------------------------------
MRP1& MRP1::operator=(const MRP1 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~MRP1()
//------------------------------------------------------------------------------
MRP1::~MRP1()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool MRP1::Evaluate()
{
   mRealValue = AttitudeData::GetReal(MRP_1);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void MRP1::SetReal(Real val)
{
   AttitudeData::SetReal(MRP_1, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* MRP1::Clone(void) const
{
   return new MRP1(*this);
}


//------------------------------------------------------------------------------
//  MRP2 - Added by Dunn
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// MRP2::MRP2(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
MRP2::MRP2(const std::string &name, GmatBase *obj)
  : AttitudeReal(name, "MRP2", obj, "Mod Rod Param 2", "")
{
   mColor = GmatColor::GREEN;
}
//------------------------------------------------------------------------------
// MRP2(const MRP2 &copy)
//------------------------------------------------------------------------------
MRP2::MRP2(const MRP2 &copy)
: AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// MRP2& operator=(const MRP2 &right)
//------------------------------------------------------------------------------
MRP2& MRP2::operator=(const MRP2 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~MRP2()
//------------------------------------------------------------------------------
MRP2::~MRP2()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool MRP2::Evaluate()
{
   mRealValue = AttitudeData::GetReal(MRP_2);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void MRP2::SetReal(Real val)
{
   AttitudeData::SetReal(MRP_2, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* MRP2::Clone(void) const
{
   return new MRP2(*this);
}


//------------------------------------------------------------------------------
//  MRP3 - Added by Dunn
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// MRP3::MRP3(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
MRP3::MRP3(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "MRP3", obj, "Mod Rod Param 3", "")
{
   mColor = GmatColor::BLUE;
}
//------------------------------------------------------------------------------
// MRP3(const MRP3 &copy)
//------------------------------------------------------------------------------
MRP3::MRP3(const MRP3 &copy)
: AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// MRP3& operator=(const MRP3 &right)
//------------------------------------------------------------------------------
MRP3& MRP3::operator=(const MRP3 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~MRP3()
//------------------------------------------------------------------------------
MRP3::~MRP3()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool MRP3::Evaluate()
{
   mRealValue = AttitudeData::GetReal(MRP_3);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void MRP3::SetReal(Real val)
{
   AttitudeData::SetReal(MRP_3, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* MRP3::Clone(void) const
{
   return new MRP3(*this);
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Quaternions  ---------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Quat1
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Quat1(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
Quat1::Quat1(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "Q1", obj, "Quaternion 1", "", false)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED;
   #endif
}
//------------------------------------------------------------------------------
// Quat1(const Quat1 &copy)
//------------------------------------------------------------------------------
Quat1::Quat1(const Quat1 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// Quat1& operator=(const Quat1 &right)
//------------------------------------------------------------------------------
Quat1& Quat1::operator=(const Quat1 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~Quat1()
//------------------------------------------------------------------------------
Quat1::~Quat1()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Quat1::Evaluate()
{
   mRealValue = AttitudeData::GetReal(QUAT_1);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
      return false;
   else
      return true;
}
//------------------------------------------------------------------------------
// GmatBase* Quat1::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Quat1::Clone(void) const
{
   return new Quat1(*this);
}

//------------------------------------------------------------------------------
//  Quat2
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Quat2(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
Quat2::Quat2(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "Q2", obj, "Quaternion 2", "", false)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW;
   #endif
}
//------------------------------------------------------------------------------
// Quat2(const Quat2 &copy)
//------------------------------------------------------------------------------
Quat2::Quat2(const Quat2 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// Quat2& operator=(const Quat2 &right)
//------------------------------------------------------------------------------
Quat2& Quat2::operator=(const Quat2 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~Quat2()
//------------------------------------------------------------------------------
Quat2::~Quat2()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Quat2::Evaluate()
{
   mRealValue = AttitudeData::GetReal(QUAT_2);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
      return false;
   else
      return true;
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Quat2::Clone(void) const
{
   return new Quat2(*this);
}

//------------------------------------------------------------------------------
//  Quat3
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Quat3(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
Quat3::Quat3(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "Q3", obj, "Quaternion 3", "", false)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// Quat3(const Quat3 &copy)
//------------------------------------------------------------------------------
Quat3::Quat3(const Quat3 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// Quat3& operator=(const Quat3 &right)
//------------------------------------------------------------------------------
Quat3& Quat3::operator=(const Quat3 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~Quat3()
//------------------------------------------------------------------------------
Quat3::~Quat3()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Quat3::Evaluate()
{
   mRealValue = AttitudeData::GetReal(QUAT_3);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
      return false;
   else
      return true;
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Quat3::Clone(void) const
{
   return new Quat3(*this);
}

//------------------------------------------------------------------------------
//  Quat4
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Quat4::Quat4(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
Quat4::Quat4(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "Q4", obj, "Quaternion 4", "", false)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN;
   #endif
}
//------------------------------------------------------------------------------
// Quat4(const Quat4 &copy)
//------------------------------------------------------------------------------
Quat4::Quat4(const Quat4 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// Quat4& operator=(const Quat4 &right)
//------------------------------------------------------------------------------
Quat4& Quat4::operator=(const Quat4 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~Quat4()
//------------------------------------------------------------------------------
Quat4::~Quat4()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Quat4::Evaluate()
{
   mRealValue = AttitudeData::GetReal(QUAT_4);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
      return false;
   else
      return true;
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Quat4::Clone(void) const
{
   return new Quat4(*this);
}


// Changed Quaternion to be Rvector Parameter instead of String Parameter
// (LOJ: 2014.02.10)
//------------------------------------------------------------------------------
// Quaternion(const std::string &name = "", GmatBase *obj = NULL)
//------------------------------------------------------------------------------
Quaternion::Quaternion(const std::string &name, GmatBase *obj)
   : AttitudeRvector(name, "Quaternion", obj, "Quaternion", "", true, 4)
{
}

//------------------------------------------------------------------------------
// Quaternion(const Quaternion &copy)
//------------------------------------------------------------------------------
Quaternion::Quaternion(const Quaternion &copy)
   : AttitudeRvector(copy)
{
}

//------------------------------------------------------------------------------
// Quaternion& operator=(const Quaternion &right)
//------------------------------------------------------------------------------
Quaternion& Quaternion::operator=(const Quaternion &right)
{
   if (this != &right)
      AttitudeRvector::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~Quaternion()
//------------------------------------------------------------------------------
Quaternion::~Quaternion()
{
}

//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool Quaternion::Evaluate()
{
   mRvectorValue = AttitudeData::GetRvector(QUATERNION);
   return true;
}

//------------------------------------------------------------------------------
// void SetRvector(const Rvector &val)
//------------------------------------------------------------------------------
void Quaternion::SetRvector(const Rvector &val)
{
   AttitudeData::SetRvector(QUATERNION, val);
   RvectorVar::SetRvector(val);
}

//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* Quaternion::Clone(void) const
{
   return new Quaternion(*this);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Angular Velocity  ----------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  AngularVelocityX
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// AngularVelocityX::AngularVelocityX(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
AngularVelocityX::AngularVelocityX(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "AngularVelocityX", obj, "Angular Velocity X", "deg/sec")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED;
   #endif
}
//------------------------------------------------------------------------------
// AngularVelocityX(const AngularVelocityX &copy)
//------------------------------------------------------------------------------
AngularVelocityX::AngularVelocityX(const AngularVelocityX &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// AngularVelocityX& operator=(const AngularVelocityX &right)
//------------------------------------------------------------------------------
AngularVelocityX& AngularVelocityX::operator=(const AngularVelocityX &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~AngularVelocityX()
//------------------------------------------------------------------------------
AngularVelocityX::~AngularVelocityX()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
/**
* Get spacecraft's angular velocity in X-direction (in degrees/ second)
*/
//------------------------------------------------------------------------------
bool AngularVelocityX::Evaluate()
{
   mRealValue = AttitudeData::GetReal(ANGULAR_VELOCITY_X);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void AngularVelocityX::SetReal(Real val)
{
   AttitudeData::SetReal(ANGULAR_VELOCITY_X, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AngularVelocityX::Clone(void) const
{
   return new AngularVelocityX(*this);
}

//------------------------------------------------------------------------------
//  AngularVelocityY
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// AngularVelocityY::AngularVelocityY(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
AngularVelocityY::AngularVelocityY(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "AngularVelocityY", obj, "Angular Velocity Y", "deg/sec")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW;
   #endif
}
//------------------------------------------------------------------------------
// AngularVelocityY(const AngularVelocityY &copy)
//------------------------------------------------------------------------------
AngularVelocityY::AngularVelocityY(const AngularVelocityY &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// AngularVelocityY& operator=(const AngularVelocityY &right)
//------------------------------------------------------------------------------
AngularVelocityY& AngularVelocityY::operator=(const AngularVelocityY &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~AngularVelocityY()
//------------------------------------------------------------------------------
AngularVelocityY::~AngularVelocityY()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool AngularVelocityY::Evaluate()
{
   mRealValue = AttitudeData::GetReal(ANGULAR_VELOCITY_Y);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void AngularVelocityY::SetReal(Real val)
{
   AttitudeData::SetReal(ANGULAR_VELOCITY_Y, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AngularVelocityY::Clone(void) const
{
   return new AngularVelocityY(*this);
}

//------------------------------------------------------------------------------
//  AngularVelocityZ
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// AngularVelocityZ::AngularVelocityZ(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
AngularVelocityZ::AngularVelocityZ(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "AngularVelocityZ", obj, "Angular Velocity Z", "deg/sec")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// AngularVelocityZ(const AngularVelocityZ &copy)
//------------------------------------------------------------------------------
AngularVelocityZ::AngularVelocityZ(const AngularVelocityZ &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// AngularVelocityZ& operator=(const AngularVelocityZ &right)
//------------------------------------------------------------------------------
AngularVelocityZ& AngularVelocityZ::operator=(const AngularVelocityZ &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~AngularVelocityZ()
//------------------------------------------------------------------------------
AngularVelocityZ::~AngularVelocityZ()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool AngularVelocityZ::Evaluate()
{
   mRealValue = AttitudeData::GetReal(ANGULAR_VELOCITY_Z);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void AngularVelocityZ::SetReal(Real val)
{
   AttitudeData::SetReal(ANGULAR_VELOCITY_Z, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* AngularVelocityZ::Clone(void) const
{
   return new AngularVelocityZ(*this);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Euler Angle Rates  ---------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  EulerAngleRate1
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// EulerAngleRate1::EulerAngleRate1(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EulerAngleRate1::EulerAngleRate1(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "EulerAngleRate1", obj, "Euler Angle Rate 1", "deg/sec")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED;
   #endif
}
//------------------------------------------------------------------------------
// EulerAngleRate1(const EulerAngleRate1 &copy)
//------------------------------------------------------------------------------
EulerAngleRate1::EulerAngleRate1(const EulerAngleRate1 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// EulerAngleRate1& operator=(const EulerAngleRate1 &right)
//------------------------------------------------------------------------------
EulerAngleRate1& EulerAngleRate1::operator=(const EulerAngleRate1 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~EulerAngleRate1()
//------------------------------------------------------------------------------
EulerAngleRate1::~EulerAngleRate1()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool EulerAngleRate1::Evaluate()
{
   mRealValue = AttitudeData::GetReal(EULER_ANGLE_RATE_1);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void EulerAngleRate1::SetReal(Real val)
{
   AttitudeData::SetReal(EULER_ANGLE_RATE_1, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EulerAngleRate1::Clone(void) const
{
   return new EulerAngleRate1(*this);
}

//------------------------------------------------------------------------------
//  EulerAngleRate2
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// EulerAngleRate2::EulerAngleRate2(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EulerAngleRate2::EulerAngleRate2(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "EulerAngleRate2", obj, "Euler Angle Rate 2", "deg/sec")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW;
   #endif
}
//------------------------------------------------------------------------------
// EulerAngleRate2(const EulerAngleRate2 &copy)
//------------------------------------------------------------------------------
EulerAngleRate2::EulerAngleRate2(const EulerAngleRate2 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// EulerAngleRate2& operator=(const EulerAngleRate2 &right)
//------------------------------------------------------------------------------
EulerAngleRate2& EulerAngleRate2::operator=(const EulerAngleRate2 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~EulerAngleRate2()
//------------------------------------------------------------------------------
EulerAngleRate2::~EulerAngleRate2()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool EulerAngleRate2::Evaluate()
{
   mRealValue = AttitudeData::GetReal(EULER_ANGLE_RATE_2);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void EulerAngleRate2::SetReal(Real val)
{
   AttitudeData::SetReal(EULER_ANGLE_RATE_2, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EulerAngleRate2::Clone(void) const
{
   return new EulerAngleRate2(*this);
}

//------------------------------------------------------------------------------
//  EulerAngleRate3
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// EulerAngleRate3::EulerAngleRate3(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
EulerAngleRate3::EulerAngleRate3(const std::string &name, GmatBase *obj)
   : AttitudeReal(name, "EulerAngleRate3", obj, "Euler Angle Rate 3", "deg/sec")
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE;
   #endif
}
//------------------------------------------------------------------------------
// EulerAngleRate3(const EulerAngleRate3 &copy)
//------------------------------------------------------------------------------
EulerAngleRate3::EulerAngleRate3(const EulerAngleRate3 &copy)
   : AttitudeReal(copy)
{
}
//------------------------------------------------------------------------------
// EulerAngleRate3& operator=(const EulerAngleRate3 &right)
//------------------------------------------------------------------------------
EulerAngleRate3& EulerAngleRate3::operator=(const EulerAngleRate3 &right)
{
   if (this != &right)
      AttitudeReal::operator=(right);

   return *this;
}
//------------------------------------------------------------------------------
// ~EulerAngleRate3()
//------------------------------------------------------------------------------
EulerAngleRate3::~EulerAngleRate3()
{
}
//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool EulerAngleRate3::Evaluate()
{
   mRealValue = AttitudeData::GetReal(EULER_ANGLE_RATE_3);

   if (mRealValue == AttitudeData::ATTITUDE_REAL_UNDEFINED)
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
void EulerAngleRate3::SetReal(Real val)
{
   AttitudeData::SetReal(EULER_ANGLE_RATE_3, val);
   RealVar::SetReal(val);
}
//------------------------------------------------------------------------------
// GmatBase* Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EulerAngleRate3::Clone(void) const
{
   return new EulerAngleRate3(*this);
}

