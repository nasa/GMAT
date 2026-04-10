//------------------------------------------------------------------------------
//                            Nadir Pointing
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
// Author: Yeerang Lim/KAIST
// Created: 2013.05.09
//
/**
 * Class definition for the Nadir (Nadir Pointing) attitude class.
 * 
 * @note 
 */
//------------------------------------------------------------------------------

#ifndef Nadir_hpp
#define Nadir_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"


class GMAT_API NadirPointing : public Kinematic 
{
public:
   /// Constructor
   NadirPointing(const std::string &itsName = "");
   /// copy constructor
   NadirPointing(const NadirPointing& att);
   /// operator =
   NadirPointing& operator=(const NadirPointing& att);
   /// destructor
   virtual ~NadirPointing();
   
   /// Initialize the Nadir attitude
   virtual bool Initialize();

   /// inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual std::vector<Rmatrix33> 
                     GetRotationMatrixDerivative(GmatTime &epochGT, CoordinateSystem *j2kCS);

protected:
   enum 
   {
      NadirPointingParamCount = KinematicParamCount
   };
   
   static const Real DENOMINATOR_TOLERANCE;

   Rmatrix33 TRIAD(Rvector3& V1, Rvector3& V2, Rvector3& W1, Rvector3& W2);

   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);
   virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime);

private:
   // Default constructor - not implemented
   //NadirPointing(); // MSVC compiler gives warning: multiple default constructors specified

};
#endif /*NadirPointing_hpp*/
