//------------------------------------------------------------------------------
//                                  CoordinateConverter
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2004/12/27
//
/**
 * Definition of the CoordinateConverter class.
 *
 */
//------------------------------------------------------------------------------

#ifndef CoordinateConverter_hpp
#define CoordinateConverter_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "A1Mjd.hpp"
#include "CoordinateSystem.hpp"
#include "Rvector.hpp"
#include "SolarSystem.hpp"

class GMAT_API CoordinateConverter
{
public:

   // default constructor
   CoordinateConverter();
   // copy constructor
   CoordinateConverter(const CoordinateConverter &coordCvt);
   // operator = 
   const CoordinateConverter& operator=(const CoordinateConverter &coordCvt);
   // destructor
   virtual ~CoordinateConverter();
   
   // initializes the CoordinateConverter
   virtual void Initialize(); 
   
	bool SetToCalculateRotMatrixDeriv(bool turnOn);

   bool Convert(const A1Mjd &epoch, const Rvector &inState,
                CoordinateSystem *inCoord, Rvector &outState,
                CoordinateSystem *outCoord, 
                bool forceNutationComputation = false, bool omitTranslation = false);
   bool Convert(const A1Mjd &epoch, const Real *inState,
                CoordinateSystem *inCoord, Real *outState,
                CoordinateSystem *outCoord, 
                bool forceNutationComputation = false, bool omitTranslation = false);

   
   bool Convert(const Real epoch, const Rvector &inState,
      CoordinateSystem *inCoord, Rvector &outState,
      CoordinateSystem *outCoord,
      bool forceNutationComputation = false, bool omitTranslation = false)
   {
      return Convert(A1Mjd(epoch), inState, inCoord, outState, outCoord,
         forceNutationComputation, omitTranslation);
   };

   
   bool Convert(const Real epoch, const Real *inState,
      CoordinateSystem *inCoord, Real *outState,
      CoordinateSystem *outCoord,
      bool forceNutationComputation = false, bool omitTranslation = false)
   {
      return Convert(A1Mjd(epoch), inState, inCoord, outState, outCoord,
         forceNutationComputation, omitTranslation);
   };

   
   bool Convert(const GmatTime &epoch, const Rvector &inState,
      CoordinateSystem *inCoord, Rvector &outState,
      CoordinateSystem *outCoord,
      bool forceNutationComputation = false, bool omitTranslation = false);

   
   bool Convert(const GmatTime &epoch, const Real *inState,
      CoordinateSystem *inCoord, Real *outState,
      CoordinateSystem *outCoord,
      bool forceNutationComputation = false, bool omitTranslation = false);

   // method to return the rotation matrix used to do the last conversion
   Rmatrix33    GetLastRotationMatrix() const;
   Rmatrix33    GetLastRotationDotMatrix() const;
   Rmatrix33    GetRotationMatrixFromICRFToFK5(const A1Mjd &atEpoch);

	std::vector<Rmatrix33> 
		          GetLastRotationMatrixDerivative() const;


protected:
   Rmatrix33 lastRotMatrix;
   Rmatrix33 lastRotDotMatrix;

	bool      specifyRotMatrixDeriv;
	std::vector<Rmatrix33> lastRotMatrixDeriv; 

   bool         ConvertFromBaseToBase(const A1Mjd &epoch, SolarSystem *solarSystem,
                                      const std::string &inBase,  const std::string &outBase,
                                      const Rvector &inBaseState, Rvector &outBaseState);
   bool         ConvertFromBaseToBase(const A1Mjd &epoch, SolarSystem *solarSystem,
                                      const std::string &inBase,  const std::string &outBase,
                                      const Real *inBaseState,    Real *outBaseState);
   
   
   bool         ConvertFromBaseToBase(const GmatTime &epoch, SolarSystem *solarSystem,
                                      const std::string &inBase, const std::string &outBase,
                                      const Rvector &inBaseState, Rvector &outBaseState);
   
   bool         ConvertFromBaseToBase(const GmatTime &epoch, SolarSystem *solarSystem,
                                      const std::string &inBase, const std::string &outBase,
                                      const Real *inBaseState, Real *outBaseState);
   void         RotateFromICRFtoFK5(const Real* inState, Real* outState, const A1Mjd& atEpoch);

private:
   void         RotationMatrixFromICRFToFK5(const A1Mjd &atEpoch);
   Rmatrix33 icrfToFK5;
   Rmatrix33 icrfToFK5Dot;
   Real lastIcrfToFk5Epoch;

};
#endif // CoordinateConverter_hpp
