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
 * Definition of the SpiceFrameKernelWriter, which writes FK files.
 */
//------------------------------------------------------------------------------
#include "SpiceFrameKernelWriter.hpp"

//------------------------------------------------------------------------------
// SpiceFrameKernelWriter()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 */
//------------------------------------------------------------------------------
SpiceFrameKernelWriter::SpiceFrameKernelWriter() :
   SpiceKernelWriter()
{

}

//------------------------------------------------------------------------------
// SpiceFrameKernelWriter(const SpiceFrameKernelWriter& copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param copy    object whose data to copy
 *
 */
//------------------------------------------------------------------------------
SpiceFrameKernelWriter::SpiceFrameKernelWriter(const SpiceFrameKernelWriter& copy) :
   SpiceKernelWriter(copy)
{

}

//------------------------------------------------------------------------------
// SpiceFrameKernelWriter& operator=(const SpiceFrameKernelWriter &copy)
//------------------------------------------------------------------------------
/**
 * Assigment operator
 *
 * @param copy    object whose data to copy
 *
 */
//------------------------------------------------------------------------------
SpiceFrameKernelWriter& SpiceFrameKernelWriter::operator=(const SpiceFrameKernelWriter& copy)
{
   if (&copy != this)
   {
      SpiceKernelWriter::operator=(copy);
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~SpiceFrameKernelWriter()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 */
//------------------------------------------------------------------------------
SpiceFrameKernelWriter::~SpiceFrameKernelWriter()
{

}

//------------------------------------------------------------------------------
// void WriteCKBasedFrame(const std::string &fileName,
//    const std::string& frameName, const std::string& frameId,
//    const std::string& centerId)
//------------------------------------------------------------------------------
/**
 * Method to write and load a CK based frame kernel
 *
 * @param fileName The name of the file this will be written to
 * @param frameName The unique name for this new frame
 * @param frameId The unique ID associated with this frame, typically the ID of
 *                the object it is based on * 1000 (i.e spacecraftID * 1000)
 * @param centerId The ID of the object that represents the center of this from
 *                 (I.e. a spacecraft ID)
 *
 */
//------------------------------------------------------------------------------
void SpiceFrameKernelWriter::WriteCKBasedFrame(const std::string &fileName,
   const std::string &frameName, const std::string &frameId,
   const std::string &centerId)
{
   std::ofstream fkStream;
   fkStream.open(fileName);

   fkStream << "\\begindata\n";
   fkStream << "FRAME_" << frameName << " = " << frameId << "\n";
   fkStream << "FRAME_" << frameId << "_NAME = '" << frameName << "'\n";
   fkStream << "FRAME_" << frameId << "_CLASS = 3\n";
   fkStream << "FRAME_" << frameId << "_CENTER = " << centerId << "\n";
   fkStream << "FRAME_" << frameId << "_CLASS_ID = " << frameId << "\n";
   fkStream << "CK_" << frameId << "SCLK = " << centerId << "\n";
   fkStream << "CK_" << frameId << "SPK = " << centerId << "\n";

   fkStream.close();
}

//------------------------------------------------------------------------------
// void WriteFixedOffsetFrame(const std::string &fileName,
//    const std::string& frameName, const std::string& frameId,
//    const std::string& centerId, const std::string& relativeName,
//    const std::string& rotMatrix)
//------------------------------------------------------------------------------
/**
 * Method to write and load a CK based frame kernel
 *
 * @param fileName The name of the file this will be written to
 * @param frameName The unique name for this new frame
 * @param frameId The unique ID number associated with this frame
 * @param centerId The ID of the object that represents the center of this frame
 *                 (i.e. a spacecraft ID)
 * @param relativeName The frame name that that frame will be offset/rotated
 *                     from
 * @param rotMatrix The rotation matrix from the relativeName frame to this
                    frame
 *
 */
//------------------------------------------------------------------------------
void SpiceFrameKernelWriter::WriteFixedOffsetFrame(const std::string &fileName,
   const std::string &frameName, const std::string &frameId,
   const std::string &centerId, const std::string &relativeName,
   const std::string &rotMatrix)
{
   std::ofstream fkStream;
   fkStream.open(fileName);

   fkStream << "\\begindata\n";
   fkStream << "FRAME_" << frameName << " = " << frameId << "\n";
   fkStream << "FRAME_" << frameId << "_NAME = '" << frameName << "'\n";
   fkStream << "FRAME_" << frameId << "_CLASS = 4\n";
   fkStream << "FRAME_" << frameId << "_CENTER = " << centerId << "\n";
   fkStream << "FRAME_" << frameId << "_CLASS_ID = " << frameId << "\n";
   fkStream << "TKFRAME_" << frameId << "_RELATIVE = '" << relativeName << "'\n";
   fkStream << "TKFRAME_" << frameId << "_SPEC = 'MATRIX'\n";
   fkStream << "TKFRAME_" << frameId << "_MATRIX = (" << rotMatrix << ")\n\n";

   fkStream.close();
}
