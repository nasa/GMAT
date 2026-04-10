//$Id:$
//------------------------------------------------------------------------------
//                              SpiceOrbitKernelReader
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
// FDSS Task order 28.
//
// Author: Wendy C. Shoan
// Created: 2010.04.05
//
/**
 * Definition of the SpiceOrbitKernelReader, which reads SPICE SPK (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to read the specified
 * SPICE file(s) and return the requested data.
 */
//------------------------------------------------------------------------------

#ifndef SpiceOrbitKernelReader_hpp
#define SpiceOrbitKernelReader_hpp

#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "Rmatrix66.hpp"
#include "SpiceKernelReader.hpp"
#include <tuple>
#include <map>

class GMAT_API SpiceOrbitKernelReader : public SpiceKernelReader
{
public:

   SpiceOrbitKernelReader();
   SpiceOrbitKernelReader(const SpiceOrbitKernelReader &reader);
   SpiceOrbitKernelReader& operator=(const SpiceOrbitKernelReader &reader);

   virtual ~SpiceOrbitKernelReader();

//   SpiceOrbitKernelReader* Clone() const;
   virtual SpiceInterface* Clone() const;

   /// method to return the earliest and latest times of orbit data
   /// coverage for the specified object, over the specified list of
   /// SPK kernels (non-SPK kernels in the list will be ignored).
   void      GetCoverageStartAndEnd(StringArray       &kernels,
                                    Integer           forNaifId,
                                    Real              &start,
                                    Real              &end);

   void      GetCoverageSpans(StringArray       &kernels,
                              Integer           forNaifId,
                              RealArray         &start,
                              RealArray         &end);

   /// method to return the state (position, velocity) of a planetary body or
   /// spacecraft at the specified time, with respect to the specified frame
   Rvector6  GetTargetState(const std::string &targetName,
                            const Integer     targetNAIFId,
                            const A1Mjd       &atTime,
                            const std::string &observingBodyName,
                            const Integer     observingBodyNAIFId,
                            const std::string &referenceFrame = "J2000",
                            const std::string &aberration = "NONE");

   Rvector6  GetTargetState(const std::string &targetName, 
      const Integer     targetNAIFId,
      const GmatTime    &atTime,
      const std::string &observingBodyName,
      const Integer     observingBodyNAIFId,
      const std::string &referenceFrame = "J2000",
      const std::string &aberration = "NONE");


   Rmatrix66 GetRotationMatrix(std::string fromFrame, std::string toFrame, const GmatTime &atTime);
   Rmatrix33 GetRotationMatrix33(std::string fromFrame, std::string toFrame, const GmatTime &atTime);

protected:

   // data converted to SPICE types, to pass into SPICE methods
   /// the observing body name (SPICE)
   ConstSpiceChar  *observingBodyNameSPICE;
   /// the aberration correction flag (SPICE)
   ConstSpiceChar  *aberrationSPICE;
   //map used to store error messages to reduce their number when it comes to gaps in ephemeris data
   std::map<std::string, std::string> spiceErrorMSG;

   std::map<Integer, std::tuple<GmatTime, GmatTime>> objectCoverageRanges;

   int gapsFound;

   void PrintErrorMessage(const std::string &targetName);
   void PrintWarningMessage(const std::string &targetName);
   void StoreErrorMessage(const std::string & targetName);

   bool      CheckIfInCoverageRange(Integer forNaifId, const A1Mjd  &atTime);
   void      StoreCoverageRange(Integer forNaifId);
};

#endif // SpiceOrbitKernelReader_hpp
