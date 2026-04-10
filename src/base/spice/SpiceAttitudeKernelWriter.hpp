//$Id:$
//------------------------------------------------------------------------------
//                              SpiceAttitudeKernelWriter
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
 * Definition of the SpiceAttitudeKernelWriter, which writes SPICE data (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to write the specified
 * SPICE file.  Currently, this class write CK files of Data Type 5 Subtype 1 (Lagrange
 * Interpolation of quaternions).  Currently,
 * each CK file written by GMAT contains data for one and only one object
 * (currently only spacecraft objects are handled).
 *
 * Calling code must create a SpiceAttitudeKernelWriter for each file that is to be
 * written.
 *
 * The output CK file name takes the form
 *       <objName>-<yyyymmdd>-<data-type>-<n>.bsp
 * where <objName>  is the name of the object for which the CK file is created
 *       <yyyymmdd> is the date of file creation
 *       <data-type>is the two-digit identifier for the CK data type (see
 *                  CK documentation at NAIF website)
 *       <n>        is the incremental file counter
 *       .bc        is the standard file extension for binary CK files
 *
 * This code creates a temporary text file, required in order to include META-Data
 * (commentary) in the CK file.  The file is deleted from the system after the
 * commentary is added to the CK file.  The name of this temporary text file
 * takes the form
 *       GMATtmpSPKcmmnt<objName>.txt
 * where <objName>  is the name of the object for which the CK file is created
 *
 */
 //------------------------------------------------------------------------------

#ifndef SpiceAttitudeKernelWriter_hpp
#define SpiceAttitudeKernelWriter_hpp

#include <stdio.h>
#include "gmatdefs.hpp"
#include "A1Mjd.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "FileManager.hpp"
#include "SpiceKernelWriter.hpp"
#include "SpiceSCClockKernelWriter.hpp"
// include the appropriate SPICE C header(s)
extern "C"
{
#include "SpiceZfc.h"    // for CSPICE routines to add meta data
}

class GMAT_API SpiceAttitudeKernelWriter :public SpiceKernelWriter
{
public:
   SpiceAttitudeKernelWriter(const std::string& objName,
      Integer objNAIFId, const std::string& fileName, Integer deg = 7,
      const std::string& frame = "J2000", const std::string& existingSCLK = "",
      bool makeSCLK = true, bool renameExistingSPK = false);
   SpiceAttitudeKernelWriter(const SpiceAttitudeKernelWriter& copy);
   SpiceAttitudeKernelWriter& operator=(const SpiceAttitudeKernelWriter& copy);
   ~SpiceAttitudeKernelWriter();

   virtual SpiceAttitudeKernelWriter* Clone() const;

   void        SetCKFile(const std::string &kernelFile);
   void        StoreSegment(const A1Mjd& start, const A1Mjd& end,
      const std::vector<Rvector>& quats, const EpochArray& epochs);
   void        FinalizeKernel(bool done = true, bool writeMetaData = true);
   void        WriteCKData();
   void        WriteSCLKData();
   void        AddMetaData(const std::string& line, bool done = false);
   Integer     GetMinNumberOfAttitudes();

protected:
   /// the name of the spacecraft or body for which the SPK is created
   std::string     objectName;
   /// the name of the central body
   std::string     centralBodyName;
   /// the file (kernel) name
   std::string     kernelFileName;
   // the reference frame
   std::string     frameName;

   /// Start times of segments
   std::vector<A1Mjd> segStarts;
   /// End times of segments
   std::vector<A1Mjd> segEnds;
   /// Array to store the segment times
   std::vector<std::vector<A1Mjd>>  segEpochs;
   /// Array to store the segment attitudes
   std::vector<std::vector<Rvector>> segAttitudes;

   /// Whether to rename a CK if file already exists
   /// Set to false for now as Windows does not handle
   /// renaming
   bool            renameCK;

   // data converted to SPICE types, to pass into SPICE methods
   /// the target body or spacecraft NAIF Id (SPICE)
   SpiceInt        objectNAIFId;
   /// the degree of interpolating polynomials to pass to SPICE
   SpiceInt        degree;
   /// the reference frame (SPICE)
   ConstSpiceChar* referenceFrame;
   /// handle to the SPK file to which to write the data
   SpiceInt        handle;
   /// "Basic" metadata
   StringArray     basicMetaData;
   /// Added metadata
   StringArray     addedMetaData;
   /// flag indicating whether or not a file handle has been obtained, and the file is
   /// open for writing
   bool            fileOpen;
   /// Has the file been finalized and closed?
   bool            fileFinalized;
   /// Has data been written to the file?
   bool            dataWritten;
   /// the name of the temporary text file
   std::string     tmpTxtFileName;
   /// the temporary text file
   FILE*           tmpTxtFile;
   /// flag indicating whether or not the temporary file can be written
   bool            tmpFileOK;
   /// do we want to open for first-write or for appending?
   bool            appending;
   /// an instance of the FileManager to handle renaming, etc.
   FileManager*    fm;
   /// the SCLK writer, to create the associated clock times
   SpiceSCClockKernelWriter* sclk;
   /// the file name of the associated SCLK
   std::string     sclkFileName;
   /// the start time of the sclk for which any appended data will be based on
   A1Mjd sclkStartTime;
   ///
   bool            generateSCLK;

   static const std::string TMP_TXT_FILE_NAME;
   static const Integer     MAX_FILE_RENAMES;

   void     SetBasicMetaData();
   /// method used to create the temporary text file, to use to set metadata (comments)
   /// on the SPK file
   void     WriteMetaData();
   /// Opens the file for writing - this should not be done until we are ready
   /// to write the data
   bool     OpenFileForWriting();
   /// Clear the currently stored data, typically when it has been written to file
   void     ClearStoredData();
   // Used to write SPICE related error messages
   void     GenerateSpiceErrorMessage(std::string errDesc);

private:
   // default constructor
   SpiceAttitudeKernelWriter();
};

#endif // SpiceAttitudeKernelWriter_hpp
