//$Id:$
//------------------------------------------------------------------------------
//                              SpiceKernelWriter
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
// Created: 2009.11.25
//
/**
 * Definition of the SpiceKernelWriter, which writes SPICE data (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to write the specified
 * SPICE file.  Currently, this class write SPK files of Data Type 13 (Hermite
 * Interpolation with Unequal time steps; geometric - no aberration).  Currently,
 * each SPK file written by GMAT contains data for one and only one object
 * (currently only spacecraft objects are handled).
 *
 * Calling code must create a SpiceKernelWriter for each file that is to be
 * written.
 *
 * The output SPK file name takes the form
 *       <objName>-<yyyymmdd>-<data-type>-<n>.bsp
 * where <objName>  is the name of the object for which the SPK file is created
 *       <yyyymmdd> is the date of file creation
 *       <data-type>is the two-digit identifier for the SPK data type (see
 *                  SPK documentation at NAIF website)
 *       <n>        is the incremental file counter
 *       .bsp       is the standard file extension for binary SPK files
 *
 * This code creates a temporary text file, required in order to include META-Data
 * (commentary) in the SPK file.  The file is deleted from the system after the
 * commentary is added to the SPK file.  The name of this temporary text file
 * takes the form
 *       GMATtmpSPKcmmnt<objName>.txt
 * where <objName>  is the name of the object for which the SPK file is created
 *
 */
//------------------------------------------------------------------------------

#ifndef SpiceKernelWriter_hpp
#define SpiceKernelWriter_hpp

#include <stdio.h>
#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "SpiceInterface.hpp"
// include the appropriate SPICE C header(s)
extern "C"
{
   #include "SpiceZfc.h"    // for CSPICE routines to add meta data
}

class GMAT_API SpiceKernelWriter :public SpiceInterface
{
public:
   SpiceKernelWriter();
   SpiceKernelWriter(const SpiceKernelWriter &copy);
   SpiceKernelWriter& operator=(const SpiceKernelWriter &copy);
   ~SpiceKernelWriter();


protected:

};

#endif // SpiceKernelWriter_hpp
