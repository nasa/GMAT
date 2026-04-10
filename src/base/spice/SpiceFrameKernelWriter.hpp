//$Id$
//------------------------------------------------------------------------------
//                              SpiceFrameKernelWriter
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
// FDSS III.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: September 02, 2025
//
/**
 * Declaration of the SpiceFrameKernelWriter, which writes FK files.
 */
//------------------------------------------------------------------------------

#ifndef SpiceFrameKernelWriter_hpp
#define SpiceFrameKernelWriter_hpp

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

class GMAT_API SpiceFrameKernelWriter :public SpiceKernelWriter
{
public:
   SpiceFrameKernelWriter();
   SpiceFrameKernelWriter(const SpiceFrameKernelWriter& copy);
   SpiceFrameKernelWriter& operator=(const SpiceFrameKernelWriter& copy);
   ~SpiceFrameKernelWriter();

   void WriteCKBasedFrame(const std::string& fileName,
      const std::string& frameName, const std::string& frameId,
      const std::string& centerId);
   void WriteFixedOffsetFrame(const std::string& fileName, const std::string& frameName,
      const std::string& frameId, const std::string& centerId,
      const std::string& relativeName, const std::string& rotMatrix);
};

#endif