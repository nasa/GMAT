//------------------------------------------------------------------------------
//                             PrecessingSpinner
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
// Author: Yeerang Lim/KAIST
// Created: 2013.05.15
// Updated: 20130.06.26 (Jaehwan Pi/KAIST)
//
/**
 * Class definition for the PrecessingSpinner attitude class.
 * 
 * @note 
 */
//------------------------------------------------------------------------------

#ifndef PrecessingSpinner_hpp
#define PrecessingSpinner_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Kinematic.hpp"
//#include "Rmatrix33.hpp"


class GMAT_API PrecessingSpinner : public Kinematic 
{
public:
   /// Constructor
   PrecessingSpinner(const std::string &itsName = "");
   /// copy constructor
   PrecessingSpinner(const PrecessingSpinner& att);
   /// operator =
   PrecessingSpinner& operator=(const PrecessingSpinner& att);
   /// destructor
   virtual ~PrecessingSpinner();
   
   /// Initialize the Nadir attitude
   virtual bool Initialize();

   /// inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:
   enum 
   {
      PrecessingSpinnerParamCount = KinematicParamCount
   };

   ///
   Rvector3 xAxis;
   Rvector3 yAxis;
   Rvector3 bodySpinAxisNormalized;
   Rvector3 nutationReferenceVectorNormalized;

   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);   
   virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime);

private:
   // Default constructor - not implemented
   //PrecessingSpinner(); // MSVC compiler gives warning: multiple default constructors specified

};
#endif /*PrecessingSpinner_hpp*/
