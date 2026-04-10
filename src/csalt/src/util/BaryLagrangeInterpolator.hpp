//------------------------------------------------------------------------------
//                         BaryLagrange Interpolator  
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Youngkwang Kim
// Created: 2015/06/22
//
/**
 * Definition for the Barycentric Lagrange Interpolator class
 * (it does not follow 'interpolator' base class)
 */
//------------------------------------------------------------------------------
#ifndef BaryLagrangeInterpolator_hpp
#define BaryLagrangeInterpolator_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"

/**
 * Barycentric Lagrange class
 */
class BaryLagrangeInterpolator 
{
public:
   /// default constructor
   BaryLagrangeInterpolator(); 
   
   /// copy constructor
   BaryLagrangeInterpolator(const BaryLagrangeInterpolator &bli);

   /// assignment operator
   BaryLagrangeInterpolator& operator=(
                                 const BaryLagrangeInterpolator &bli);
   
   /// default destructor
   ~BaryLagrangeInterpolator();

   /// methods for setting indVarVec and interpPointVec
   void    SetIndVarVec(const Rvector *indVarVec);
   void    SetInterpPointVec(const Rvector *interpPointVec);

   /// methods for getting numIndVarVec and isInterpPointDefined
   Integer GetNumIndVarVec();
   Integer GetNumInterpPointVec();

   /// methods for checking indVar and InterpPointVec are defined or not
   bool    IsInterpPointVecDefined();
   bool    IsIndVarVecDefined();
   

   /// Method to perform barycentric Lagrange interpolation - interpolation
   /// data will be written at *resultVec.
   void Interpolate(const Rvector *indVarVec,
	                 const Rvector *funcValueVec,
	                 const Rvector *interpPointVec,
					     Rvector &resultVec);

   /// Method to perform barycentric Lagrange interpolation - interpolation
   /// data will be written at *resultVec.
   void Interpolate(const Rvector *funcValueVec,
	                 const Rvector *interpPointVec,
					     Rvector &resultVec);


   /// Method to perform barycentric Lagrange interpolation - interpolation
   /// data will be written at *resultVec.
   void Interpolate(const Rvector *funcValueVec,
	                 Rvector &resultVec);
protected:      
   /// Parameters
   
   /// The data points of the independent variable
   Rvector        indVar;
   /// Number of the data points
   Integer        numIndVarVec;
   Integer        numInterpPointVec;
   /// the range of the indVar used for validity check for interpPoints
   Real           indVarLB;
   Real           indVarUB;


   /// barycentric weight vector & matrix
   Rvector        weigthVec;
   Rmatrix        barycentricMatrix;


   /// Flag used to determine if indVarVec and interpPointVec are defined.
   /// if isInterpPointVecDefined == true, 
   /// barycentricMatrix exists.
   bool           isInterpPointVecDefined;
   /// if isIndVarVecDefined == true, 
   /// weigthVec exists.
   bool           isIndVarVecDefined;


   // methods

   /// internal methods for checking inputs
   bool   IsStrictlyIncreasing(const Rvector *testVec); 
   bool   ChkIndVarVecFeasibility(const Rvector *indVarVec);
   bool   ChkInterpPointVecFeasibility(const Rvector *interpPointVec);
   bool   ChkFuncValueVecFeasibility(const Rvector *funcValueVec);

   
   
   /// internal methods for calculatng internal vector and matrix
   void   CalBarycentricMatrix(const Rvector *interpPointVec);
   void   CalWeightVec();
};

#endif // BaryLagrangeInterpolator_hpp
