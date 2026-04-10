//$Id$
//------------------------------------------------------------------------------
//                                 CSFixed
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Wendy C. Shoan/GSFC
// Created: 2006.03.24
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class definition for the CSFixed (Coordinate System Fixed) attitude class.
 * 
 * @note The time passed in for the epoch or passed into the methods 
 *       should be an A1Mjd (though its type is currently Real).
 * @note Methods called to set/get parameter values will require/return angle
 *       values in degrees and rate values in degrees/sec.  All other methods 
 *       assume/expect radians and radians/sec.
 */
//------------------------------------------------------------------------------


#ifndef CSFixed_hpp
#define CSFixed_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"
#include "Rmatrix33.hpp"

class GMAT_API CSFixed : public Kinematic 
{
public:
   /// Constructor
   CSFixed(const std::string &itsName = "");
   /// copy constructor
   CSFixed(const CSFixed& att);
   /// operator =
   CSFixed& operator=(const CSFixed& att);
   /// destructor
   virtual ~CSFixed();
   
   /// Initialize the CSFixed attitude
   virtual bool Initialize();

   /// inherited from GmatBase
   virtual GmatBase* Clone() const;


protected:
   enum 
   {
       CSFixedParamCount = KinematicParamCount,
   };
   
   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);
   virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime);

private:
   // Default constructor - not implemented
   //CSFixed(); // MSVC compiler gives warning: multiple default constructors specified

};
#endif /*CSFixed_hpp*/
