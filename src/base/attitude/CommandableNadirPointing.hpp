//------------------------------------------------------------------------------
//                            Commandable Nadir Pointing
//------------------------------------------------------------------------------
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under purchase
// order NNG16LD52P
//
//
// Author: Mike Stark// Created: 2022.03.10
//
/**
 * Class definition for the CommandableNadirPointing class.
 *
 * @note
 */
//------------------------------------------------------------------------------

#ifndef CommandableNadir_hpp
#define CommandableNadir_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"

class CommandableNadirPointing : public Kinematic
{
public:
   /// Constructor
   CommandableNadirPointing(const std::string &itsName = "");
   /// copy constructor
   CommandableNadirPointing(const CommandableNadirPointing& att);
   /// operator =
   CommandableNadirPointing& operator=(const CommandableNadirPointing& att);
   /// destructor
   virtual ~CommandableNadirPointing();
   
   /// Initialize the Nadir attitude
   virtual bool Initialize();

   /// inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   //------------------------------------------------------------------------------
   // *** Functions overridden to allow parameter setting
   //     without referencing Attitude class
   //------------------------------------------------------------------------------

   // Method to get the attitude as a Quaternion
   virtual const Rvector&     GetQuaternion(Real atTime);
   
   // Method to get the attitude as a Direction Cosine Matrix
   virtual const Rmatrix33&   GetCosineMatrix(Real atTime);
   
   // parameter setting operations to override Attitude versions for
   // nadir-pointing parameters
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
   virtual Real        SetRealParameter(const std::string &label,
                                        const Real value);
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value);
   
   // use to set quaternion without re-initializing;
   // supports the setting of quaternion during mission sequence
   virtual const Rvector&    SetRvectorParameter(const Integer id,
                                                 const Rvector &value);
   virtual const Rmatrix&    SetRmatrixParameter(const Integer id,
                                                 const Rmatrix &value);
   
   // allow the setting of the quaternion from script's mission sequence
   virtual bool IsParameterCommandModeSettable(const Integer id) const;

protected:
   enum 
   {
      CommandableNadirPointingParamCount = KinematicParamCount
   };
   
   static const Real DENOMINATOR_TOLERANCE;
   
   // flag for first call to ComputeCosineMatrixAndAngularVelocity()
   bool      hasComputedFirstDCM;

   // this will ultimately be deleted
   Rmatrix33 TRIAD(Rvector3& V1, Rvector3& V2, Rvector3& W1, Rvector3& W2);
 
   virtual void ComputeAttitudeFromCartesianVectors(Real atTime);
   virtual void ComputePositionAndVelocity (Real atTime,
      Rvector3& pos, Rvector3& vel);
   virtual void ComputeReferenceAttitude(Real atTime);
   // computes standard nadir-pointing (+z is nadir, -y is orbit normal)

   
   
   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);
   
   
   virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime);
   virtual std::vector<Rmatrix33> GetRotationMatrixDerivative
      (GmatTime &epochGT, CoordinateSystem *j2kCS);

private:
   // Default constructor - not implemented

};
#endif /*CommandableNadir_hpp*/
