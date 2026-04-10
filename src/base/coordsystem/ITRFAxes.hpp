//$Id: ITRFAxes.hpp 9513 2012-02-29 21:23:06Z tuandangnguyen $
//------------------------------------------------------------------------------
//                                  ITRFAxes
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
// Author: Tuan Dang Nguyen (NASA/GSFC)
// Created: 2012/02/29
//
/**
 * Definition of the ITRFAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef ITRFAxes_hpp
#define ITRFAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "DynamicAxes.hpp"
#include "IAUFile.hpp"
#include "Rmatrix33.hpp"

class GMAT_API ITRFAxes : public DynamicAxes
{
public:

   // default constructor
   ITRFAxes(const std::string &itsName = "");
   // copy constructor
   ITRFAxes(const ITRFAxes &itrfAxes);
   // operator = for assignment
   const ITRFAxes& operator=(const ITRFAxes &itrfAxes);
   // destructor
   virtual ~ITRFAxes();
   
   // methods to set files for reading
   // 3 are needed:
   //    leap second file (NOTE - this should be done in the utilities!!)
   //    EOP file containing polar motion (x,y) and UT1-UTC offset
   //    coefficient file containing nutation and planetary coefficients
   
   virtual GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "ICRF") const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;
   
   // method to initialize the data
   virtual bool Initialize();
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;
   // We need to override this method from CoordinateBase
   // to check for a CelestialBody origin only
   virtual bool            SetRefObject(GmatBase *obj, const UnsignedInt type,
                                        const std::string &name = "");

   Rmatrix33  GetRotationMatrix(const A1Mjd &atEpoch, bool forceComputation = false);

protected:

   Rmatrix33 R1(Real angle);
   Rmatrix33 R2(Real angle);
   Rmatrix33 R3(Real angle);
   Rmatrix33 Skew(Rvector3 vec);

   enum
   {
      ITRFAxesParamCount = DynamicAxesParamCount,
   };

   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

   IAUFile*					    iauFile;
};
#endif // ITRFAxes_hpp
