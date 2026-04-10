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
// FDSS III.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: September 02, 2025
//
/**
 * Defines the SpiceSCClockKernelWriter, which writes SCLK files given clock
 * and segment inputs.
 */
//------------------------------------------------------------------------------
#include <stdio.h>
#include <sstream>

#include "SpiceSCClockKernelWriter.hpp"
#include "MessageInterface.hpp"
#include "FileUtil.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"

//------------------------------------------------------------------------------
// SpiceSCClockKernelWriter(const std::string& objName,
//    Integer objNAIFId, Real tickLength, RealArray moduliFields, RealArray offsetFields,
//    std::string stringDel, A1Mjd clockStart, const std::string& fileName,
//    bool renameExistingSCLK)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param objNAIFId     The ID of the spacecraft for which this clock 
 *                      corresponds to
 * @param tickLenght    The number of ticks per second
 * @param moduliFields  The moduli for each tick delimiter
 * @param offsetFields  The offesets for each tick delimiter
 * @param stringDel     The character to use for delimiters in string clock 
 *                      times returned from this SCLK
 * @param clockStart    The clock start time in A1 modified Julian
 * @param fileName      The name of the SCLK file being created
 *
 */
//------------------------------------------------------------------------------
SpiceSCClockKernelWriter::SpiceSCClockKernelWriter(const std::string& objName,
   Integer objNAIFId, Real tickLength, RealArray moduliFields, RealArray offsetFields,
   std::string stringDel, A1Mjd clockStart, const std::string& fileName) :
   SpiceKernelWriter(),
   tick(tickLength),
   moduli(moduliFields),
   offSets(offsetFields),
   stringDelimiter(stringDel),
   clockStartTime(clockStart),
   kernelFileName(fileName),
   dataWritten(false)
{
   // Check for the default NAIF ID
   if (objNAIFId == SpiceInterface::DEFAULT_NAIF_ID)
   {
      MessageInterface::ShowMessage(
         "*** WARNING *** NAIF ID for SCLK file is set to the default NAIF ID (%d).  "
         "Resulting SCLK file will contain that value as the object's ID.\n",
         objNAIFId);
   }

   /// set up CSPICE data
   objectNAIFId = objNAIFId;
   kernelNameSPICE = kernelFileName.c_str();

   // Get the string format of the start time in UTC Gregorian
   UtcDate startDate = clockStartTime.ToUtcDate();
   // Put into SCLK friendly format YYYY-MM-DD/HH:MM:SS.mmm
   clockStartStringUTC = startDate.ToPackedCalendarString();
   clockStartStringUTC.replace(8, 1, "/");
   clockStartStringUTC.insert(15, ".");
   clockStartStringUTC.insert(13, ":");
   clockStartStringUTC.insert(11, ":");
   clockStartStringUTC.insert(6, "-");
   clockStartStringUTC.insert(4, "-");

   // check to make sure the directory is writable
   std::string dirName = GmatFileUtil::ParsePathName(kernelFileName, true);
   if (dirName == "")
      dirName = "./";
   if (!GmatFileUtil::DoesDirectoryExist(dirName, true))
   {
      std::string errmsg =
         "Error opening SCLK kernel, directory \"";
      errmsg += dirName + "\" does not exist.\n";
      throw UtilityException(errmsg);
   }
   WriteSCLKInputVariables();
}

//------------------------------------------------------------------------------
// SpiceSCClockKernelWriter(const SpiceSCClockKernelWriter& copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param copy    object to copy
 *
 */
//------------------------------------------------------------------------------
SpiceSCClockKernelWriter::SpiceSCClockKernelWriter(const SpiceSCClockKernelWriter& copy) :
   SpiceKernelWriter(copy),
   objectNAIFId(copy.objectNAIFId),
   tick(copy.tick),
   moduli(copy.moduli),
   offSets(copy.offSets),
   stringDelimiter(copy.stringDelimiter),
   clockStartTime(copy.clockStartTime),
   kernelFileName(copy.kernelFileName),
   partitionStarts(copy.partitionStarts),
   partitionEnds(copy.partitionEnds),
   clockCoefficients(copy.clockCoefficients),
   dataWritten(copy.dataWritten)
{
   kernelNameSPICE = kernelFileName.c_str();
}

//------------------------------------------------------------------------------
// SpiceSCClockKernelWriter& operator=(const SpiceSCClockKernelWriter& copy)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param copy    object to copy
 *
 */
//------------------------------------------------------------------------------
SpiceSCClockKernelWriter& SpiceSCClockKernelWriter::operator=(const SpiceSCClockKernelWriter& copy)
{
   if (&copy != this)
   {
      objectNAIFId = copy.objectNAIFId;
      tick = copy.tick;
      moduli = copy.moduli;
      offSets = copy.offSets;
      stringDelimiter = copy.stringDelimiter;
      clockStartTime = copy.clockStartTime;
      kernelFileName = copy.kernelFileName;
      partitionStarts = copy.partitionStarts;
      partitionEnds = copy.partitionEnds;
      clockCoefficients = copy.clockCoefficients;
      dataWritten = copy.dataWritten;

      kernelNameSPICE = kernelFileName.c_str();
   }

   return *this;
}

//------------------------------------------------------------------------------
// ~SpiceSCClockKernelWriter()
//------------------------------------------------------------------------------
/**
 * Destructor
 *
 */
//------------------------------------------------------------------------------
SpiceSCClockKernelWriter::~SpiceSCClockKernelWriter()
{

}

//------------------------------------------------------------------------------
// SpiceSCClockKernelWriter* Clone(void) const
//------------------------------------------------------------------------------
/**
 * Creates a clone of this object
 * 
 * @return The object clone
 *
 */
//------------------------------------------------------------------------------
SpiceSCClockKernelWriter* SpiceSCClockKernelWriter::Clone(void) const
{
   SpiceSCClockKernelWriter* clonedSKW = new SpiceSCClockKernelWriter(*this);

   return clonedSKW;
}

Rvector3 SpiceSCClockKernelWriter::GetFinalTimeCoefficients()
{
   Rvector3 retVal;
   if (clockCoefficients.size() > 0)
      retVal = clockCoefficients.back();
   else
      retVal(0) = retVal(1) = retVal(2) = -1;

   return retVal;
}

//------------------------------------------------------------------------------
// void AddCoefficientData(std::vector<Rvector3> clockCoeffs)
//------------------------------------------------------------------------------
/**
 * Add coefficient data to the current coefficient list
 *
 * @param clockCoeffs   The new coefficients to append. Each Rvector3 contains
 *                      the data [ticks ETtime rate]
 *
 */
//------------------------------------------------------------------------------
void SpiceSCClockKernelWriter::AddCoefficientData(std::vector<Rvector3> clockCoeffs)
{
   if (clockCoeffs.empty())
      return;

   // Check that number of ticks is increasing
   if (!clockCoefficients.empty())
   {
      if (clockCoefficients.back()(0) > clockCoeffs[0](0))
      {
         std::string errMsg = "Error adding coefficient line to SCLK file " +
            kernelFileName + ". The total elapsed ticks (first element of the "
            "coefficients) must be increasing. The new set of ticks does not "
            "start after the last tick written.\n";
         throw UtilityException(errMsg);
      }
   }

   clockCoefficients.push_back(clockCoeffs[0]);
   for (Integer i = 1; i < clockCoeffs.size(); ++i)
   {
      if (clockCoeffs[i][0] < clockCoeffs[i - 1](0))
      {
         std::string errMsg = "Error adding coefficient line to SCLK file " +
            kernelFileName + ". The total elapsed ticks (first element of the "
            "coefficients) must be increasing. The tick " +
            GmatStringUtil::ToString(clockCoeffs[i][0]) + " at index " +
            GmatStringUtil::ToString(i) + " is less than the previous tick " +
            GmatStringUtil::ToString(clockCoeffs[i - 1][0]) + ".\n";
         throw UtilityException(errMsg);
      }
      clockCoefficients.push_back(clockCoeffs[i]);
   }
}

//------------------------------------------------------------------------------
// void AddPartitionSegment(const A1Mjd& segStart, const A1Mjd& segEnd)
//------------------------------------------------------------------------------
/**
 * Add a new partition segment
 *
 * @param segStart   The start of the segment in A1ModJulian
 * @param segEnd     The end of the segment in A1ModJulian
 *
 */
//------------------------------------------------------------------------------
void SpiceSCClockKernelWriter::AddPartitionSegment(const A1Mjd& segStart, const A1Mjd& segEnd)
{
   if (segStart >= segEnd)
   {
      std::string errMsg = "Error adding partition segment to SCLK kernel " +
         kernelFileName + ". The start of a segment must be before the end of "
         "the segment.\n";
      throw UtilityException(errMsg);
   }

   if (segStart < clockStartTime)
   {
      std::string errMsg = "Error adding partition segment to SCLK kernel " +
         kernelFileName + ". The start of the segment must occur at or "
         "after the start of the spacecraft clock's start time.\n";
      throw UtilityException(errMsg);
   }

   // Get time since clock start and convert to ticks
   Real startTick = (A1ToSpiceTime(segStart.GetReal()) -
      A1ToSpiceTime(clockStartTime.GetReal())) * tick;
   Real endTick = (A1ToSpiceTime(segEnd.GetReal()) -
      A1ToSpiceTime(clockStartTime.GetReal())) * tick;

   if (!partitionEnds.empty())
   {
      if (partitionEnds.back() > startTick)
      {
         std::string errMsg = "Error adding partition segment to SCLK kernel " +
            kernelFileName + ". The start of a new segment with A1 date " +
            GmatStringUtil::ToString(segStart.GetReal(), 0) + " starts before "
            "the end of a previous segment.\n";
         throw UtilityException(errMsg);
      }
   }

   partitionStarts.push_back(startTick);
   partitionEnds.push_back(endTick);
}

//------------------------------------------------------------------------------
// void WriteSCLKCoefficients()
//------------------------------------------------------------------------------
/**
 * Write the partition and coefficient data to the SCLK file
 *
 */
//------------------------------------------------------------------------------
void SpiceSCClockKernelWriter::WriteSCLKCoefficients(bool rewriteFile)
{
   if (rewriteFile)
      WriteSCLKInputVariables();
   if (dataWritten && !rewriteFile)
   {
      std::string errMsg = "Error writing coefficient data to SCLK kernel " +
         kernelFileName + ". The data has already been written.\n";
      throw UtilityException(errMsg);
   }

   // First write the provided partition pairs, then the parallel time coefficients
   std::ofstream fileStream;
   fileStream.open(kernelFileName, std::ios_base::app);
   std::string NAIFIdEndNum = GmatStringUtil::ToString(-objectNAIFId, 0);
   Integer numPartitions = partitionStarts.size();

   fileStream << "\nSCLK_PARTITION_START_" << NAIFIdEndNum << "   = ( ";
   for (Integer i = 0; i < numPartitions; ++i)
   {
      fileStream << GmatStringUtil::ToString(
         partitionStarts.at(i), false, true, true, 13, 13);
      if (i != numPartitions - 1)
         fileStream << "\n";
   }
   fileStream << " )\n\n";

   fileStream << "SCLK_PARTITION_END_" << NAIFIdEndNum << "     = ( ";
   for (Integer i = 0; i < numPartitions; ++i)
   {
      fileStream << GmatStringUtil::ToString(
         partitionEnds.at(i), false, true, true, 13, 13);
      if (i != numPartitions - 1)
         fileStream << "\n";
   }
   fileStream << " )\n\n";
   fileStream << "SCLK01_COEFFICIENTS_" << NAIFIdEndNum << "    = (\n\n";

   for (Integer i = 0; i < clockCoefficients.size() - 1; ++i)
   {
      fileStream << GmatStringUtil::ToString(
         clockCoefficients.at(i)(0), false, true, true, 13, 13);
      fileStream << "     " << GmatStringUtil::ToString(
         clockCoefficients.at(i)(1), false, true, true, 13, 13);
      fileStream << "     " << GmatStringUtil::ToString(
         clockCoefficients.at(i)(2), false, true, true, 13, 13) << std::endl;
   }
   fileStream << GmatStringUtil::ToString(
      clockCoefficients.back()(0), false, true, true, 13, 13);
   fileStream << "     " << GmatStringUtil::ToString(
      clockCoefficients.back()(1), false, true, true, 13, 13);
   fileStream << "     " << GmatStringUtil::ToString(
      clockCoefficients.back()(2), false, true, true, 13, 13) << " )" << std::endl;
   fileStream << " \n\\begintext" << std::endl;

   fileStream.close();
   dataWritten = true;
}

//------------------------------------------------------------------------------
// void WriteSCLKCoefficients()
//------------------------------------------------------------------------------
/**
 * Write the input/formatting data for the SCLK file
 *
 */
//------------------------------------------------------------------------------
void SpiceSCClockKernelWriter::WriteSCLKInputVariables()
{
   // This is written first, so wipe anything else from the file
   std::ofstream fileStream;
   fileStream.open(kernelFileName);
   std::string NAIFIdEndNum = GmatStringUtil::ToString(GmatMathUtil::Abs(objectNAIFId),0);
   Integer numFields = moduli.size();

   fileStream << "KPL/SCLK\n\n";
   fileStream << "\\begindata\n";
   fileStream << "SCLK_KERNEL_ID            = ( @" << clockStartStringUTC << " )\n\n";
   fileStream << "SCLK_DATA_TYPE_" << NAIFIdEndNum << "         = ( 1 )\n";
   fileStream << "SCLK01_TIME_SYSTEM_" << NAIFIdEndNum << "     = ( 1 )\n";
   fileStream << "SCLK01_N_FIELDS_" << NAIFIdEndNum << "        = ( " << GmatStringUtil::ToString(numFields, 0) << " )\n";
   fileStream << "SCLK01_MODULI_" << NAIFIdEndNum << "          = ( ";
   for (Integer i = 0; i < numFields; ++i)
      fileStream << GmatStringUtil::Trim(GmatStringUtil::ToString(moduli.at(i))) << " ";
   fileStream << ")\n";
   fileStream << "SCLK01_OFFSETS_" << NAIFIdEndNum << "         = ( ";
   for (Integer i = 0; i < numFields; ++i)
      fileStream << GmatStringUtil::Trim(GmatStringUtil::ToString(offSets.at(i))) << " ";
   fileStream << ")\n";
   fileStream << "SCLK01_OUTPUT_DELIM_" << NAIFIdEndNum << "    = ( " << stringDelimiter << " )" << std::endl;

   fileStream.close();
}