//------------------------------------------------------------------------------
//                            ThreeAxisKinematic
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
// Author: Michael Stark / GSFC
// Created: 2019.05.xx
//
/**
 * Class definition for the ThreeAxisKinematic attitude class. This class
 * provides the capability to propagate quaternions over time using the angular
 * velocity vector
 * 
 * @note 
 */
//------------------------------------------------------------------------------

#ifndef ThreeAxis_hpp
#define ThreeAxis_hpp

#include "gmatdefs.hpp"
#include "Rmatrix.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"


class GMAT_API ThreeAxisKinematic : public Kinematic
{
public:
   /// Constructor
   ThreeAxisKinematic(const std::string &itsName = "");
   /// copy constructor
   ThreeAxisKinematic(const ThreeAxisKinematic& att);
   /// operator =
   ThreeAxisKinematic& operator=(const ThreeAxisKinematic& att);
   /// destructor
   virtual ~ThreeAxisKinematic();
   
   /// Initialize the Nadir attitude
   virtual bool Initialize();

   /// inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   // overrides GmatBase to allow commanding of angular velocity
   virtual bool IsParameterCommandModeSettable(const Integer id) const;

   
protected:
   enum 
   {
      ThreeAxisParamCount = KinematicParamCount
   };
   
   static const Real DENOMINATOR_TOLERANCE;
   
   /// 4x4 Identity Matrix
   Rmatrix I44;
   
   /// 4 x 4 skew symmetric matrix for propagating quaternions
   Rmatrix Omega;
   
   /// Magnitude of angular velocity vector
   Real wMag;

   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);
   virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime);          
private:
   // Default constructor - not implemented
   //ThreeAxisKinematic(); // MSVC compiler gives warning: multiple default constructors specified

};
#endif /*ThreeAxis_hpp*/
