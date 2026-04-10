//------------------------------------------------------------------------------
//                                ThrustSegmentReal
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Peter Candell
// Created: 2022/09/07
//
/**
 * Declares ThrustSegmentScaleFactor class which provides base class for environment realated
 * Real Parameters
 */
 //------------------------------------------------------------------------------

#include "ThrustFileDefs.hpp"
#include "ThrustSegmentParameters.hpp"
#include "ColorTypes.hpp"
#include "MessageInterface.hpp"
#include "InterpreterException.hpp"

//==============================================================================
//                              ThrustSegmentScaleFactor
//==============================================================================
/**
 * Implements ThrustSegmentScaleFactor class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // ThrustSegmentScaleFactor(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
ThrustSegmentScaleFactor::ThrustSegmentScaleFactor(const std::string &name, GmatBase *obj)
   : ThrustSegmentReal(name, "ThrustSegmentScaleFactor", obj,
      "Thrust Segment Scale Factor", "", true)
{
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::CHESTNUT;
   #endif
}


//------------------------------------------------------------------------------
// ThrustSegmentScaleFactor(const ThrustSegmentScaleFactor &copy)
//------------------------------------------------------------------------------
ThrustSegmentScaleFactor::ThrustSegmentScaleFactor(const ThrustSegmentScaleFactor &copy)
   : ThrustSegmentReal(copy)
{
}


//------------------------------------------------------------------------------
// ThrustSegmentScaleFactor& operator=(const ThrustSegmentScaleFactor &right)
//------------------------------------------------------------------------------
ThrustSegmentScaleFactor& ThrustSegmentScaleFactor::operator=(const ThrustSegmentScaleFactor &right)
{
   if (this != &right)
      ThrustSegmentReal::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustSegmentScaleFactor()
//------------------------------------------------------------------------------
ThrustSegmentScaleFactor::~ThrustSegmentScaleFactor()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustSegmentScaleFactor::Evaluate()
{
   mRealValue = ThrustSegmentData::GetReal(THRUSTSEGMENTSCALEFACTOR);

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
void ThrustSegmentScaleFactor::SetReal(Real val)
{
   ThrustSegmentData::SetReal(THRUSTSEGMENTSCALEFACTOR, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* ThrustSegmentScaleFactor::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ThrustSegmentScaleFactor::Clone(void) const
{
   return new ThrustSegmentScaleFactor(*this);
}



//==============================================================================
//                              TSFSigma
//==============================================================================
/**
 * Implements TSFSigma class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // TSFSigma(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
TSFSigma::TSFSigma(const std::string &name, GmatBase *obj)
   : ThrustSegmentReal(name, "TSFSigma", obj,
      "Thrust Scale Factor Sigma", "", true)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::CHESTNUT;
#endif
}


//------------------------------------------------------------------------------
// TSFSigma(const TSFSigma &copy)
//------------------------------------------------------------------------------
TSFSigma::TSFSigma(const TSFSigma &copy)
   : ThrustSegmentReal(copy)
{
}


//------------------------------------------------------------------------------
// TSFSigma& operator=(const TSFSigma &right)
//------------------------------------------------------------------------------
TSFSigma& TSFSigma::operator=(const TSFSigma &right)
{
   if (this != &right)
      TSFSigma::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~TSFSigma()
//------------------------------------------------------------------------------
TSFSigma::~TSFSigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool TSFSigma::Evaluate()
{
   mRealValue = ThrustSegmentData::GetReal(TSF_SIGMA);

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
void TSFSigma::SetReal(Real val)
{
   ThrustSegmentData::SetReal(TSF_SIGMA, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* TSFSigma::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* TSFSigma::Clone(void) const
{
   return new TSFSigma(*this);
}


//==============================================================================
//                              TSFEpsilon
//==============================================================================
/**
 * Implements TSFEpsilon class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // TSFEpsilon(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
TSFEpsilon::TSFEpsilon(const std::string &name, GmatBase *obj)
   : ThrustSegmentReal(name, "TSFEpsilon", obj,
      "Thrust Scale Factor Sigma", "", true)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::CHESTNUT;
#endif
}


//------------------------------------------------------------------------------
// TSFEpsilon(const TSFEpsilon &copy)
//------------------------------------------------------------------------------
TSFEpsilon::TSFEpsilon(const TSFEpsilon &copy)
   : ThrustSegmentReal(copy)
{
}


//------------------------------------------------------------------------------
// TSFEpsilon& operator=(const TSFEpsilon &right)
//------------------------------------------------------------------------------
TSFEpsilon& TSFEpsilon::operator=(const TSFEpsilon &right)
{
   if (this != &right)
      TSFEpsilon::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~TSFEpsilon()
//------------------------------------------------------------------------------
TSFEpsilon::~TSFEpsilon()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool TSFEpsilon::Evaluate()
{
   mRealValue = ThrustSegmentData::GetReal(TSF_EPSILON);

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
void TSFEpsilon::SetReal(Real val)
{
   ThrustSegmentData::SetReal(TSF_EPSILON, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* TSFEpsilon::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* TSFEpsilon::Clone(void) const
{
   return new TSFEpsilon(*this);
}


//==============================================================================
//                              StartEpoch
//==============================================================================
/**
 * Implements StartEpoch class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // StartEpoch(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
StartEpoch::StartEpoch(const std::string &name, GmatBase *obj)
   : ThrustSegmentReal(name, "StartEpoch", obj,
      "Thrust Scale Factor Sigma", "", true)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::CHESTNUT;
#endif
}


//------------------------------------------------------------------------------
// StartEpoch(const StartEpoch &copy)
//------------------------------------------------------------------------------
StartEpoch::StartEpoch(const StartEpoch &copy)
   : ThrustSegmentReal(copy)
{
}


//------------------------------------------------------------------------------
// StartEpoch& operator=(const StartEpoch &right)
//------------------------------------------------------------------------------
StartEpoch& StartEpoch::operator=(const StartEpoch &right)
{
   if (this != &right)
      StartEpoch::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~StartEpoch()
//------------------------------------------------------------------------------
StartEpoch::~StartEpoch()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool StartEpoch::Evaluate()
{
   mRealValue = ThrustSegmentData::GetReal(START_EPOCH);

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
void StartEpoch::SetReal(Real val)
{
   ThrustSegmentData::SetReal(START_EPOCH, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* StartEpoch::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* StartEpoch::Clone(void) const
{
   return new StartEpoch(*this);
}



//==============================================================================
//                              EndEpoch
//==============================================================================
/**
 * Implements EndEpoch class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // EndEpoch(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
EndEpoch::EndEpoch(const std::string &name, GmatBase *obj)
   : ThrustSegmentReal(name, "EndEpoch", obj,
      "Thrust Scale Factor Sigma", "", true)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::CHESTNUT;
#endif
}


//------------------------------------------------------------------------------
// EndEpoch(const EndEpoch &copy)
//------------------------------------------------------------------------------
EndEpoch::EndEpoch(const EndEpoch &copy)
   : ThrustSegmentReal(copy)
{
}


//------------------------------------------------------------------------------
// EndEpoch& operator=(const EndEpoch &right)
//------------------------------------------------------------------------------
EndEpoch& EndEpoch::operator=(const EndEpoch &right)
{
   if (this != &right)
      EndEpoch::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~EndEpoch()
//------------------------------------------------------------------------------
EndEpoch::~EndEpoch()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool EndEpoch::Evaluate()
{
   mRealValue = ThrustSegmentData::GetReal(END_EPOCH);

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
void EndEpoch::SetReal(Real val)
{
   ThrustSegmentData::SetReal(END_EPOCH, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* EndEpoch::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* EndEpoch::Clone(void) const
{
   return new EndEpoch(*this);
}


//==============================================================================
//                              MassFlowScaleFactor
//==============================================================================
/**
 * Implements MassFlowScaleFactor class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // MassFlowScaleFactor(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
MassFlowScaleFactor::MassFlowScaleFactor(const std::string &name, GmatBase *obj)
   : ThrustSegmentReal(name, "MassFlowScaleFactor", obj,
      "Thrust Scale Factor Sigma", "", true)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::CHESTNUT;
#endif
}


//------------------------------------------------------------------------------
// MassFlowScaleFactor(const MassFlowScaleFactor &copy)
//------------------------------------------------------------------------------
MassFlowScaleFactor::MassFlowScaleFactor(const MassFlowScaleFactor &copy)
   : ThrustSegmentReal(copy)
{
}


//------------------------------------------------------------------------------
// MassFlowScaleFactor& operator=(const MassFlowScaleFactor &right)
//------------------------------------------------------------------------------
MassFlowScaleFactor& MassFlowScaleFactor::operator=(const MassFlowScaleFactor &right)
{
   if (this != &right)
      MassFlowScaleFactor::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~MassFlowScaleFactor()
//------------------------------------------------------------------------------
MassFlowScaleFactor::~MassFlowScaleFactor()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool MassFlowScaleFactor::Evaluate()
{
   mRealValue = ThrustSegmentData::GetReal(MASSFLOWSCALEFACTOR);

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
void MassFlowScaleFactor::SetReal(Real val)
{
   ThrustSegmentData::SetReal(MASSFLOWSCALEFACTOR, val);
   RealVar::SetReal(val);
}


//------------------------------------------------------------------------------
// GmatBase* MassFlowScaleFactor::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* MassFlowScaleFactor::Clone(void) const
{
   return new MassFlowScaleFactor(*this);
}



//==============================================================================
//                              ThrustAngleConstraint
//==============================================================================
/**
 * Implements ThrustAngleConstraint class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // ThrustAngleConstraint(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
ThrustAngleConstraint::ThrustAngleConstraint(const std::string &name, GmatBase *obj)
   : ThrustSegmentRvector(name, "ThrustAngleConstraint", obj,
      "Thrust Angle Constraint", "", true, 3)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::CHESTNUT;
#endif
}


//------------------------------------------------------------------------------
// ThrustAngleConstraint(const ThrustAngleConstraint &copy)
//------------------------------------------------------------------------------
ThrustAngleConstraint::ThrustAngleConstraint(const ThrustAngleConstraint &copy)
   : ThrustSegmentRvector(copy)
{
}


//------------------------------------------------------------------------------
// ThrustAngleConstraint& operator=(const ThrustAngleConstraint &right)
//------------------------------------------------------------------------------
ThrustAngleConstraint& ThrustAngleConstraint::operator=(const ThrustAngleConstraint &right)
{
   if (this != &right)
      ThrustAngleConstraint::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustAngleConstraint()
//------------------------------------------------------------------------------
ThrustAngleConstraint::~ThrustAngleConstraint()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustAngleConstraint::Evaluate()
{
   const Real* arr = ThrustSegmentData::GetRvector(THRUST_ANGLE_CONSTRAINT).GetDataVector();

   for (int i = 0; i < 3; i++)
   {
      mRvectorValue[i] = arr[i];
   }

   //Find out how to correctly copy Rvector over...
   if (mRvectorValue[2] == THRUST_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetSetRvector(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void ThrustAngleConstraint::SetRvector(Rvector& val)
{
   ThrustSegmentData::SetRvector(THRUST_ANGLE_CONSTRAINT, val);
   RvectorVar::SetRvector(val);
}


//------------------------------------------------------------------------------
// GmatBase* ThrustAngleConstraint::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ThrustAngleConstraint::Clone(void) const
{
   return new ThrustAngleConstraint(*this);
}


//==============================================================================
//                              ThrustAngle1
//==============================================================================
/**
 * Implements ThrustAngle1 class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // ThrustAngle1(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
ThrustAngle1::ThrustAngle1(const std::string &name, GmatBase *obj)
   : ThrustSegmentRvector(name, "ThrustAngle1", obj,
      "Thrust Angle 1", "", true, 3)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::CHESTNUT;
#endif
}


//------------------------------------------------------------------------------
// ThrustAngle1(const ThrustAngle1 &copy)
//------------------------------------------------------------------------------
ThrustAngle1::ThrustAngle1(const ThrustAngle1 &copy)
   : ThrustSegmentRvector(copy)
{
}


//------------------------------------------------------------------------------
// ThrustAngle1& operator=(const ThrustAngle1 &right)
//------------------------------------------------------------------------------
ThrustAngle1& ThrustAngle1::operator=(const ThrustAngle1 &right)
{
   if (this != &right)
      ThrustAngle1::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustAngle1()
//------------------------------------------------------------------------------
ThrustAngle1::~ThrustAngle1()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustAngle1::Evaluate()
{
   const Real* arr = ThrustSegmentData::GetRvector(THRUST_ANGLE_1).GetDataVector();
   const UnsignedInt size = ThrustSegmentData::GetRvector(THRUST_ANGLE_1).GetSize();
   for (int i = 0; i < size; i++)
   {
      mRvectorValue[i] = arr[i];
   }

   if (mRvectorValue[2] == THRUST_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetSetRvector(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void ThrustAngle1::SetRvector(Rvector& val)
{
   ThrustSegmentData::SetRvector(THRUST_ANGLE_1, val);
   RvectorVar::SetRvector(val);
}


//------------------------------------------------------------------------------
// GmatBase* ThrustAngle1::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ThrustAngle1::Clone(void) const
{
   return new ThrustAngle1(*this);
}


//==============================================================================
//                              ThrustAngle2
//==============================================================================
/**
 * Implements ThrustAngle2 class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // ThrustAngle2(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
ThrustAngle2::ThrustAngle2(const std::string &name, GmatBase *obj)
   : ThrustSegmentRvector(name, "ThrustAngle2", obj,
      "Thrust Angle 2", "", true, 3)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::CHESTNUT;
#endif
}


//------------------------------------------------------------------------------
// ThrustAngle2(const ThrustAngle2 &copy)
//------------------------------------------------------------------------------
ThrustAngle2::ThrustAngle2(const ThrustAngle2 &copy)
   : ThrustSegmentRvector(copy)
{
}


//------------------------------------------------------------------------------
// ThrustAngle2& operator=(const ThrustAngle2 &right)
//------------------------------------------------------------------------------
ThrustAngle2& ThrustAngle2::operator=(const ThrustAngle2 &right)
{
   if (this != &right)
      ThrustAngle2::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustAngle2()
//------------------------------------------------------------------------------
ThrustAngle2::~ThrustAngle2()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustAngle2::Evaluate()
{
   const Real* arr = ThrustSegmentData::GetRvector(THRUST_ANGLE_2).GetDataVector();
   const UnsignedInt size = ThrustSegmentData::GetRvector(THRUST_ANGLE_2).GetSize();
   for (int i = 0; i < size; i++)
   {
      mRvectorValue[i] = arr[i];
   }

   //Find out how to correctly copy Rvector over...
   if (mRvectorValue[2] == THRUST_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetSetRvector(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void ThrustAngle2::SetRvector(Rvector& val)
{
   ThrustSegmentData::SetRvector(THRUST_ANGLE_2, val);
   RvectorVar::SetRvector(val);
}


//------------------------------------------------------------------------------
// GmatBase* ThrustAngle2::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ThrustAngle2::Clone(void) const
{
   return new ThrustAngle2(*this);
}


//==============================================================================
//                              ThrustAngle1Sigma
//==============================================================================
/**
 * Implements ThrustAngle1Sigma class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // ThrustAngle1Sigma(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
ThrustAngle1Sigma::ThrustAngle1Sigma(const std::string &name, GmatBase *obj)
   : ThrustSegmentRvector(name, "ThrustAngle1Sigma", obj,
      "Thrust Angle Sigma 1", "", true, 3)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::CHESTNUT;
#endif
}


//------------------------------------------------------------------------------
// ThrustAngle1Sigma(const ThrustAngle1Sigma &copy)
//------------------------------------------------------------------------------
ThrustAngle1Sigma::ThrustAngle1Sigma(const ThrustAngle1Sigma &copy)
   : ThrustSegmentRvector(copy)
{
}


//------------------------------------------------------------------------------
// ThrustAngle1Sigma& operator=(const ThrustAngle1Sigma &right)
//------------------------------------------------------------------------------
ThrustAngle1Sigma& ThrustAngle1Sigma::operator=(const ThrustAngle1Sigma &right)
{
   if (this != &right)
      ThrustAngle1Sigma::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustAngle1Sigma()
//------------------------------------------------------------------------------
ThrustAngle1Sigma::~ThrustAngle1Sigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustAngle1Sigma::Evaluate()
{
   const Real* arr = ThrustSegmentData::GetRvector(THRUST_ANGLE_SIGMA_1).GetDataVector();
   const UnsignedInt size = ThrustSegmentData::GetRvector(THRUST_ANGLE_SIGMA_1).GetSize();
    
   for (int i = 0; i < size; i++)
   {
      mRvectorValue[i] = arr[i];
   }

   //Find out how to correctly copy Rvector over...
   if (mRvectorValue[2] == THRUST_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetSetRvector(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void ThrustAngle1Sigma::SetRvector(Rvector& val)
{
   ThrustSegmentData::SetRvector(THRUST_ANGLE_SIGMA_1, val);
   RvectorVar::SetRvector(val);
}


//------------------------------------------------------------------------------
// GmatBase* ThrustAngle1Sigma::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ThrustAngle1Sigma::Clone(void) const
{
   return new ThrustAngle1Sigma(*this);
}


//==============================================================================
//                              ThrustAngle2Sigma
//==============================================================================
/**
 * Implements ThrustAngle2Sigma class.
 */
 //------------------------------------------------------------------------------

 //------------------------------------------------------------------------------
 // ThrustAngle2Sigma(const std::string &name, GmatBase *obj)
 //------------------------------------------------------------------------------
ThrustAngle2Sigma::ThrustAngle2Sigma(const std::string &name, GmatBase *obj)
   : ThrustSegmentRvector(name, "ThrustAngle2Sigma", obj,
      "Thrust Angle Sigma 2", "", true, 3)
{
#ifdef USE_PREDEFINED_COLORS
   mColor = GmatColor::CHESTNUT;
#endif
}


//------------------------------------------------------------------------------
// ThrustAngle2Sigma(const ThrustAngle2Sigma &copy)
//------------------------------------------------------------------------------
ThrustAngle2Sigma::ThrustAngle2Sigma(const ThrustAngle2Sigma &copy)
   : ThrustSegmentRvector(copy)
{
}


//------------------------------------------------------------------------------
// ThrustAngle2Sigma& operator=(const ThrustAngle2Sigma &right)
//------------------------------------------------------------------------------
ThrustAngle2Sigma& ThrustAngle2Sigma::operator=(const ThrustAngle2Sigma &right)
{
   if (this != &right)
      ThrustAngle2Sigma::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustAngle2Sigma()
//------------------------------------------------------------------------------
ThrustAngle2Sigma::~ThrustAngle2Sigma()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool ThrustAngle2Sigma::Evaluate()
{
   const Real* arr = ThrustSegmentData::GetRvector(THRUST_ANGLE_SIGMA_2).GetDataVector();
   const UnsignedInt size = ThrustSegmentData::GetRvector(THRUST_ANGLE_SIGMA_2).GetSize();
   for (int i = 0; i < size; i++)
   {
      mRvectorValue[i] = arr[i];
   }

   //Find out how to correctly copy Rvector over...
   if (mRvectorValue[2] == THRUST_REAL_UNDEFINED)
      return false;
   else
      return true;
}


//------------------------------------------------------------------------------
// virtual void SetSetRvector(Real val)
//------------------------------------------------------------------------------
/**
 * Sets value to the owner of the parameter.
 */
 //------------------------------------------------------------------------------
void ThrustAngle2Sigma::SetRvector(Rvector& val)
{
   ThrustSegmentData::SetRvector(THRUST_ANGLE_SIGMA_2, val);
   RvectorVar::SetRvector(val);
}


//------------------------------------------------------------------------------
// GmatBase* ThrustAngle2Sigma::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* ThrustAngle2Sigma::Clone(void) const
{
   return new ThrustAngle2Sigma(*this);
}