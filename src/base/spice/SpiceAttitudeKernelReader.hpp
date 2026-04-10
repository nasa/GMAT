//$Id:$
//------------------------------------------------------------------------------
//                              SpiceAttitudeKernelReader
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
 * Definition of the SpiceAttitudeKernelReader, which reads SPICE CK (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to read the specified
 * SPICE file(s) and return the requested data.
 */
//------------------------------------------------------------------------------

#ifndef SpiceAttitudeKernelReader_hpp
#define SpiceAttitudeKernelReader_hpp

#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "SpiceKernelReader.hpp"

class GMAT_API SpiceAttitudeKernelReader : public SpiceKernelReader
{
public:

   SpiceAttitudeKernelReader();
   SpiceAttitudeKernelReader(const SpiceAttitudeKernelReader &reader);
   SpiceAttitudeKernelReader& operator=(const SpiceAttitudeKernelReader &reader);

   virtual ~SpiceAttitudeKernelReader();

   SpiceAttitudeKernelReader* Clone() const;

   /// method to return the earliest and latest times of pointing data
   /// coverage for the specified object, over the specified list of
   /// CK kernels (non-CK kernels in the list will be ignored).
   /// NOTE: This method will only check for coverage over intervals where
   /// angular velocity data is available as well.
   void   GetCoverageStartAndEnd(StringArray       &kernels,
                                 Integer           forNaifId,
                                 Real              &start,
                                 Real              &end,
                                 bool              needAngVel);

   /// method to return the orientation (attitude) of an instrument or a
   /// spacecraft at the specified time, with respect to the specified frame
   /// (support for planetary bodies' orientation may be added later)
   void   GetTargetOrientation(const std::string &objectName,
                               Integer           naifID,
                               Integer           forFrameNaifId,
                               const A1Mjd       &atTime,
//                               Real              tolerance,
                               Rmatrix33         &r33,
                               Rvector3          &angVel,
                               const std::string &referenceFrame = "J2000");


protected:

   /// the spacecraft clock time
   SpiceDouble     sclkSPICE;
   /// the output spacecraft clock time
   SpiceDouble     sclkOutputSPICE;
   /// the tolerance
   SpiceDouble     toleranceSPICE;
   /// the found flag
   SpiceBoolean    foundSPICE;
   /// NAIF ID for the object's reference frame
   Integer         frameNaifIDSPICE;

};

#endif // SpiceAttitudeKernelReader_hpp
