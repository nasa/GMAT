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
// Modified:
//   2010/08/24 Dunn Idle - Added MRPs
//   2014/02/10 Linda Jun - Changed Quaternion to derive from AttitudeRvector
//
/**
 * Declares Attitude related parameter classes.
 *    DCM11, DCM12, DCM13, DCM21, DCM22, DCM23, DCM31, DCM32, DCM33,
 *    DirectionCosineMatrix,
 *    Quat1, Quat2, Quat3, Quat4, Quaternion,
 *    EulerAngle1, EulerAngle2, EulerAngle3,
 *    MRP1 , MRP2 , MRP3 , AngularVelocityX, AngularVelocityY, AngularVelocityZ,
 *    EulerAngleRate1, EulerAngleRate2, EulerAngleRate3
 */
//------------------------------------------------------------------------------
#ifndef AttitudeParameters_hpp
#define AttitudeParameters_hpp

#include "gmatdefs.hpp"
#include "AttitudeReal.hpp"
#include "AttitudeRvector.hpp"
#include "AttitudeRmat33.hpp"
#include "AttitudeString.hpp"

//------------------------------------------------------------------------------
//  Direction Cosine Matrix
//------------------------------------------------------------------------------
class GMAT_API DCM11 : public AttitudeReal
{
public:

   DCM11(const std::string &name = "", GmatBase *obj = NULL);
   DCM11(const DCM11 &copy);
   DCM11& operator=(const DCM11 &right);
   virtual ~DCM11();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API DCM12 : public AttitudeReal
{
public:

   DCM12(const std::string &name = "", GmatBase *obj = NULL);
   DCM12(const DCM12 &copy);
   DCM12& operator=(const DCM12 &right);
   virtual ~DCM12();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API DCM13 : public AttitudeReal
{
public:

   DCM13(const std::string &name = "", GmatBase *obj = NULL);
   DCM13(const DCM13 &copy);
   DCM13& operator=(const DCM13 &right);
   virtual ~DCM13();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API DCM21 : public AttitudeReal
{
public:

   DCM21(const std::string &name = "", GmatBase *obj = NULL);
   DCM21(const DCM21 &copy);
   DCM21& operator=(const DCM21 &right);
   virtual ~DCM21();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API DCM22 : public AttitudeReal
{
public:

   DCM22(const std::string &name = "", GmatBase *obj = NULL);
   DCM22(const DCM22 &copy);
   DCM22& operator=(const DCM22 &right);
   virtual ~DCM22();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API DCM23 : public AttitudeReal
{
public:

   DCM23(const std::string &name = "", GmatBase *obj = NULL);
   DCM23(const DCM23 &copy);
   DCM23& operator=(const DCM23 &right);
   virtual ~DCM23();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API DCM31 : public AttitudeReal
{
public:

   DCM31(const std::string &name = "", GmatBase *obj = NULL);
   DCM31(const DCM31 &copy);
   DCM31& operator=(const DCM31 &right);
   virtual ~DCM31();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API DCM32 : public AttitudeReal
{
public:

   DCM32(const std::string &name = "", GmatBase *obj = NULL);
   DCM32(const DCM32 &copy);
   DCM32& operator=(const DCM32 &right);
   virtual ~DCM32();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API DCM33 : public AttitudeReal
{
public:

   DCM33(const std::string &name = "", GmatBase *obj = NULL);
   DCM33(const DCM33 &copy);
   DCM33& operator=(const DCM33 &right);
   virtual ~DCM33();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//------------------------------------------------------------------------------
//  Direction Cosine Matrix
//------------------------------------------------------------------------------
class GMAT_API DirectionCosineMatrix : public AttitudeRmat33
{
public:

   DirectionCosineMatrix(const std::string &name = "", GmatBase *obj = NULL);
   DirectionCosineMatrix(const DirectionCosineMatrix &copy);
   DirectionCosineMatrix& operator=(const DirectionCosineMatrix &right);
   virtual ~DirectionCosineMatrix();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//------------------------------------------------------------------------------
//  Quaternions
//------------------------------------------------------------------------------

class GMAT_API Quat1 : public AttitudeReal
{
public:

   Quat1(const std::string &name = "", GmatBase *obj = NULL);
   Quat1(const Quat1 &copy);
   Quat1& operator=(const Quat1 &right);
   virtual ~Quat1();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API Quat2 : public AttitudeReal
{
public:

   Quat2(const std::string &name = "", GmatBase *obj = NULL);
   Quat2(const Quat2 &copy);
   Quat2& operator=(const Quat2 &right);
   virtual ~Quat2();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};



class GMAT_API Quat3 : public AttitudeReal
{
public:

   Quat3(const std::string &name = "", GmatBase *obj = NULL);
   Quat3(const Quat3 &copy);
   Quat3& operator=(const Quat3 &right);
   virtual ~Quat3();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API Quat4 : public AttitudeReal
{
public:

   Quat4(const std::string &name = "", GmatBase *obj = NULL);
   Quat4(const Quat4 &copy);
   Quat4& operator=(const Quat4 &right);
   virtual ~Quat4();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


// Changed Quaternion to derive from AttitudeRvector
// (LOJ: 2014.02.10)
class GMAT_API Quaternion : public AttitudeRvector
{
public:

   Quaternion(const std::string &name = "", GmatBase *obj = NULL);
   Quaternion(const Quaternion &copy);
   Quaternion& operator=(const Quaternion &right);
   virtual ~Quaternion();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetRvector(const Rvector &val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};



//------------------------------------------------------------------------------
//  Euler Angles
//------------------------------------------------------------------------------
class GMAT_API EulerAngle1 : public AttitudeReal
{
public:

   EulerAngle1(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngle1(const EulerAngle1 &copy);
   EulerAngle1& operator=(const EulerAngle1 &right);
   virtual ~EulerAngle1();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API EulerAngle2 : public AttitudeReal
{
public:

   EulerAngle2(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngle2(const EulerAngle2 &copy);
   EulerAngle2& operator=(const EulerAngle2 &right);
   virtual ~EulerAngle2();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API EulerAngle3 : public AttitudeReal
{
public:

   EulerAngle3(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngle3(const EulerAngle3 &copy);
   EulerAngle3& operator=(const EulerAngle3 &right);
   virtual ~EulerAngle3();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//------------------------------------------------------------------------------
//  Modified Rodrigues Parameters - Added by Dunn
//------------------------------------------------------------------------------

class GMAT_API MRP1 : public AttitudeReal
{
public:

   MRP1(const std::string &name = "", GmatBase *obj = NULL);
   MRP1(const MRP1 &copy);
   MRP1& operator=(const MRP1 &right);
   virtual ~MRP1();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API MRP2 : public AttitudeReal
{
public:

   MRP2(const std::string &name = "", GmatBase *obj = NULL);
   MRP2(const MRP2 &copy);
   MRP2& operator=(const MRP2 &right);
   virtual ~MRP2();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API MRP3 : public AttitudeReal
{
public:

   MRP3(const std::string &name = "", GmatBase *obj = NULL);
   MRP3(const MRP3 &copy);
   MRP3& operator=(const MRP3 &right);
   virtual ~MRP3();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//------------------------------------------------------------------------------
//  Angular Velocity
//------------------------------------------------------------------------------

class GMAT_API AngularVelocityX : public AttitudeReal
{
public:

   AngularVelocityX(const std::string &name = "", GmatBase *obj = NULL);
   AngularVelocityX(const AngularVelocityX &copy);
   AngularVelocityX& operator=(const AngularVelocityX &right);
   virtual ~AngularVelocityX();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API AngularVelocityY : public AttitudeReal
{
public:

   AngularVelocityY(const std::string &name = "", GmatBase *obj = NULL);
   AngularVelocityY(const AngularVelocityY &copy);
   AngularVelocityY& operator=(const AngularVelocityY &right);
   virtual ~AngularVelocityY();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API AngularVelocityZ : public AttitudeReal
{
public:

   AngularVelocityZ(const std::string &name = "", GmatBase *obj = NULL);
   AngularVelocityZ(const AngularVelocityZ &copy);
   AngularVelocityZ& operator=(const AngularVelocityZ &right);
   virtual ~AngularVelocityZ();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//------------------------------------------------------------------------------
//  Euler Angle Rates
//------------------------------------------------------------------------------

class GMAT_API EulerAngleRate1 : public AttitudeReal
{
public:

   EulerAngleRate1(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngleRate1(const EulerAngleRate1 &copy);
   EulerAngleRate1& operator=(const EulerAngleRate1 &right);
   virtual ~EulerAngleRate1();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API EulerAngleRate2 : public AttitudeReal
{
public:

   EulerAngleRate2(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngleRate2(const EulerAngleRate2 &copy);
   EulerAngleRate2& operator=(const EulerAngleRate2 &right);
   virtual ~EulerAngleRate2();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API EulerAngleRate3 : public AttitudeReal
{
public:

   EulerAngleRate3(const std::string &name = "", GmatBase *obj = NULL);
   EulerAngleRate3(const EulerAngleRate3 &copy);
   EulerAngleRate3& operator=(const EulerAngleRate3 &right);
   virtual ~EulerAngleRate3();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//------------------------------------------------------------------------------
//  Body Alignment Vectors
//------------------------------------------------------------------------------
class GMAT_API BodyAlignmentVectorX : public AttitudeReal
{
public:

   BodyAlignmentVectorX(const std::string &name = "", GmatBase *obj = NULL);
   BodyAlignmentVectorX(const BodyAlignmentVectorX &copy);
   BodyAlignmentVectorX& operator=(const BodyAlignmentVectorX &right);
   virtual ~BodyAlignmentVectorX();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API BodyAlignmentVectorY : public AttitudeReal
{
public:

   BodyAlignmentVectorY(const std::string &name = "", GmatBase *obj = NULL);
   BodyAlignmentVectorY(const BodyAlignmentVectorY &copy);
   BodyAlignmentVectorY& operator=(const BodyAlignmentVectorY &right);
   virtual ~BodyAlignmentVectorY();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API BodyAlignmentVectorZ : public AttitudeReal
{
public:

   BodyAlignmentVectorZ(const std::string &name = "", GmatBase *obj = NULL);
   BodyAlignmentVectorZ(const BodyAlignmentVectorZ &copy);
   BodyAlignmentVectorZ& operator=(const BodyAlignmentVectorZ &right);
   virtual ~BodyAlignmentVectorZ();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//------------------------------------------------------------------------------
//  Body Constraint Vectors
//------------------------------------------------------------------------------
class GMAT_API BodyConstraintVectorX : public AttitudeReal
{
public:

   BodyConstraintVectorX(const std::string &name = "", GmatBase *obj = NULL);
   BodyConstraintVectorX(const BodyConstraintVectorX &copy);
   BodyConstraintVectorX& operator=(const BodyConstraintVectorX &right);
   virtual ~BodyConstraintVectorX();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API BodyConstraintVectorY : public AttitudeReal
{
public:

   BodyConstraintVectorY(const std::string &name = "", GmatBase *obj = NULL);
   BodyConstraintVectorY(const BodyConstraintVectorY &copy);
   BodyConstraintVectorY& operator=(const BodyConstraintVectorY &right);
   virtual ~BodyConstraintVectorY();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API BodyConstraintVectorZ : public AttitudeReal
{
public:

   BodyConstraintVectorZ(const std::string &name = "", GmatBase *obj = NULL);
   BodyConstraintVectorZ(const BodyConstraintVectorZ &copy);
   BodyConstraintVectorZ& operator=(const BodyConstraintVectorZ &right);
   virtual ~BodyConstraintVectorZ();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

//------------------------------------------------------------------------------
//  Attitude Strings
//------------------------------------------------------------------------------
class GMAT_API AttitudeParam : public AttitudeString
{
public:
   AttitudeParam(const std::string &name = "", GmatBase *obj = NULL);
   AttitudeParam(const AttitudeParam &copy);
   AttitudeParam& operator=(const AttitudeParam &right);
   virtual ~AttitudeParam();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetString(std::string val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API AttitudeReferenceBody : public AttitudeString
{
public:
   AttitudeReferenceBody(const std::string &name = "", GmatBase *obj = NULL);
   AttitudeReferenceBody(const AttitudeReferenceBody &copy);
   AttitudeReferenceBody& operator=(const AttitudeReferenceBody &right);
   virtual ~AttitudeReferenceBody();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetString(std::string val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

class GMAT_API AttitudeConstraintType : public AttitudeString
{
public:
   AttitudeConstraintType(const std::string &name = "", GmatBase *obj = NULL);
   AttitudeConstraintType(const AttitudeConstraintType &copy);
   AttitudeConstraintType& operator=(const AttitudeConstraintType &right);
   virtual ~AttitudeConstraintType();

   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetString(std::string val);

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

#endif /*AttitudeParameters_hpp*/
