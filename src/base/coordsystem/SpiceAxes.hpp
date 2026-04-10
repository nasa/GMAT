//$Id$
//------------------------------------------------------------------------------
//                                  SpiceAxes
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
// Developed jointly by NASA/GSFC and Pearl River Technologies under
// FDSS III.
//
// Author: Peter Candell
// Created: 2024/04/18
//
/**
 * Definition of the SpiceAxes class.
 *
 * This class is an axes that class that will pull all of its rotational data from Spice. 
 * This allows users to specify any valid Spice frame and use it as long as they have
 * inputted all of the necessary Spice Kernels. 
 */
 //------------------------------------------------------------------------------

#ifndef SpiceAxes_hpp
#define SpiceAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "ItrfCoefficientsFile.hpp"
#include "CelestialBody.hpp"

class GMAT_API SpiceAxes : public AxisSystem
{
public:

   // default constructor
   SpiceAxes(const std::string &itsName = "");
   // copy constructor
   SpiceAxes(const SpiceAxes &bfAxes);
   // operator = for assignment
   const SpiceAxes& operator=(const SpiceAxes &bfAxes);
   // destructor
   virtual ~SpiceAxes();

   // methods to set files for reading
   // 3 are needed:

   virtual GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "FK5") const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;

   // method to initialize the data
   virtual bool Initialize();

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;

   //   // We need to override this method from CoordinateBase
   //   // to check for a CelestialBody or Spacecraft origin only
   virtual bool            SetRefObject(GmatBase *obj, const UnsignedInt type,
      const std::string &name = "");

   virtual void SetSpiceFrameId(std::string spiceId);

protected:

   enum
   {
      SpiceAxesParamCount = AxisSystemParamCount,
   };

   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
      bool forceComputation = false);

   virtual void CalculateRotationMatrix(const GmatTime &atEpoch,
      bool forceComputation = false);

   GmatTime                 prevEpoch;
   Real                     prevUpdateInterval;
   Real                     prevOriginUpdateInterval;
   std::string              spiceFrameId;

};
#endif // SpiceAxes_hpp
