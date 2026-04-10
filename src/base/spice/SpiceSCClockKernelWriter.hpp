//$Id$
//------------------------------------------------------------------------------
//                            SpiceSCClockKernelWriter
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
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: September 02, 2025
//
/**
 * Declares of the SpiceSCClockKernelWriter, which writes SCLK files given clock
 * and segment inputs.
 */
//------------------------------------------------------------------------------

#ifndef SpiceSCClockKernelWriter_hpp
#define SpiceSCClockKernelWriter_hpp

#include <stdio.h>
#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "FileManager.hpp"
#include "SpiceKernelWriter.hpp"
// include the appropriate SPICE C header(s)
extern "C"
{
#include "SpiceZfc.h"    // for CSPICE routines to add meta data
}

class GMAT_API SpiceSCClockKernelWriter :public SpiceKernelWriter
{
public:
   SpiceSCClockKernelWriter(const std::string& objName,
      Integer objNAIFId, Real tickLength, RealArray moduliFields,
      RealArray offsetFields, std::string stringDel, A1Mjd clockStart,
      const std::string& fileName);
   SpiceSCClockKernelWriter(const SpiceSCClockKernelWriter& copy);
   SpiceSCClockKernelWriter& operator=(const SpiceSCClockKernelWriter& copy);
   ~SpiceSCClockKernelWriter();

   virtual SpiceSCClockKernelWriter* Clone() const;

   Rvector3    GetFinalTimeCoefficients();

   void        AddCoefficientData(std::vector<Rvector3>);
   void        AddPartitionSegment(const A1Mjd& segStart, const A1Mjd& segEnd);
   void        WriteSCLKCoefficients(bool rewriteFile = false);

protected:
   /// the file (kernel) name
   std::string        kernelFileName;
   /// the time system code associated with the clock time
   Integer            parallelTimeSystem;
   /// the length of one tick on the SC clock per second
   Real               tick;
   /// the delimiter type to be used in string clock inputs/outputs
   std::string        stringDelimiter;
   /// array of moduli fields representing time units for spaecraft clock
   RealArray       moduli;
   /// offsets to clock fields
   RealArray       offSets;
   /// starts of clock partitions in ticks
   RealArray partitionStarts;
   /// ends of clock partitions in ticks
   RealArray partitionEnds;
   /// clock start time in A1 modified Julian
   A1Mjd              clockStartTime;
   /// string of clock start time for file ID
   std::string        clockStartStringUTC;
   /// list of three element coefficient arrays
   std::vector<Rvector3> clockCoefficients;

   // data converted to SPICE types, to pass into SPICE methods
   /// the target body or spacecraft NAIF Id (SPICE)
   SpiceInt        objectNAIFId;
   /// Has data been written to the file?
   bool            dataWritten;

   /// Opens the file for writing - this should not be done until we are ready
   /// to write the data
   void     WriteSCLKInputVariables();

private:
   // default constructor
   SpiceSCClockKernelWriter();

};

#endif // SpiceSCClockKernelWriter_hpp
