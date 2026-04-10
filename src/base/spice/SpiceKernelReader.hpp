//$Id:$
//------------------------------------------------------------------------------
//                              SpiceKernelReader
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
// Created: 2008.10.22
//
/**
 * Definition of the SpiceKernelReader, which reads SPICE data (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to read the specified
 * SPICE file(s) and return the requested data.  
 * Kernels that may be loaded (and read by child classes) are:
 *  planetary ephemeris (SPK)
 *  spacecraft ephemeris (SPK)
 *  spacecraft pointing (CK)
 *  planetary constants (PcK)   < future >
 *  instrument (IK)             < future >
 *  
 * This is the base class for readers.  Classes inheriting from this one handle
 * the reading of specific types of data (orbit, attitude, ...).
 */
//------------------------------------------------------------------------------

#ifndef SpiceKernelReader_hpp
#define SpiceKernelReader_hpp

#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "SpiceInterface.hpp"

class GMAT_API SpiceKernelReader : public SpiceInterface
{
public:
   SpiceKernelReader();
   SpiceKernelReader(const SpiceKernelReader &reader);
   SpiceKernelReader& operator=(const SpiceKernelReader &reader);

   virtual ~SpiceKernelReader();
   
   //------------------------------------------------------------------------------
   // void GetCoverageStartAndEnd(const StringArray &kernels, Integer forNaifId,
   //                             Real              &start,   Real    &end)
   //------------------------------------------------------------------------------
   /**
    * This (pure virtual) method determines the coverage for the specified object
    * over the specified kernels.
    *
    * @param kernels   the array of kernels over which to check the coverage
    * @param forNaifId the NAIF ID of the object for which coverage should
    *                  be determined
    * @param start     (output) the earliest time of coverage for the object
    *                  included in the specified kernels
    * @param end       (output) the latest time of coverage for the object
    *                  included in the specified kernels
    *
    * @note An exception is thrown if any of the kernels listed are not currently
    *       loaded into the kernel pool, and an attempt to load it fails.
    * @note The interval between the returned start and end times is not
    *       necessarily continuous.  The method checks all intervals over which
    *       there is coverage for the specified object and returns the earliest
    *       time and the latest time of coverage.  There could be gaps in
    *       coverage over that span.
    * @note The method will ignore kernels of types not containing the type of
    *       data requested, e.g. if requesting orbit data, the method will
    *       ignore kernels in the list that are not of type 'spk'.
    */
   //------------------------------------------------------------------------------
//   virtual void GetCoverageStartAndEnd(StringArray       &kernels,
//                                       Integer           forNaifId,
//                                       Real              &start,
//                                       Real              &end) = 0;

protected:

   // data converted to SPICE types, to pass into SPICE methods
   /// the target body name (SPICE)
   ConstSpiceChar  *objectNameSPICE;
   /// the NAIF ID
   SpiceInt        naifIDSPICE;
   /// NAIF ID for the observing object
   SpiceInt        observerNaifIDSPICE;
   /// the observer epoch time (SPICE) in Ephemeris (TDB) Time
   SpiceDouble     etSPICE;
   /// the reference frame (SPICE)
   ConstSpiceChar  *referenceFrameSPICE;

   static const Integer  MAX_IDS_PER_KERNEL;
   static const Integer  MAX_COVERAGE_INTERVALS;

};

#endif // SpiceKernelReader_hpp
