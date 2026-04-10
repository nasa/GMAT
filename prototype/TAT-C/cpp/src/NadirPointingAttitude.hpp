//------------------------------------------------------------------------------
//                           NadirPointingAttitude
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2018.07.30
//
/**
 * Definition of the NadirPointingAttitude class.  This class models the
 * spacecraft attitude state.
 */
//------------------------------------------------------------------------------
#ifndef NadirPointingAttitude_hpp
#define NadirPointingAttitude_hpp

#include "gmatdefs.hpp"
#include "Attitude.hpp"
#include "Rmatrix33.hpp"

class NadirPointingAttitude : public Attitude
{
public:
   
   /// class construction/destruction
   NadirPointingAttitude();
   NadirPointingAttitude( const NadirPointingAttitude &copy);
   NadirPointingAttitude& operator=(const NadirPointingAttitude &copy);
   
   virtual ~NadirPointingAttitude();

   /// Clone the Attitude
   virtual Attitude* Clone() const;
   
   /// Converts the inertial-to-reference matrix @todo is this misnamed?
   virtual Rmatrix33   InertialToReference(const Rvector6& centralBodyState);
   
protected:
   
   /// Local class data for performance
   Rvector3      centralBodyFixedPos;
   Rvector3      centralBodyFixedVel;
   Rvector3      zHat;
   Rvector3      xHat;
   Rvector3      yHat;
   Rmatrix33     R_fixed_to_nadir;
   Rmatrix33     R_fixed_to_nadir_transposed;
   
};
#endif // NadirPointingAttitude_hpp
