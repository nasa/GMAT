//------------------------------------------------------------------------------
//                                  NPlateHistoryFileReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Author: Wendy Shoan / NASA
// Created: 2014.01.07
//
/**
 * Reads a NPlateHistory file, and stores the data.
 * NOTE: Currently only reads azimuth, elevation, and X, Y, Z
 * @todo Add assumptions and general file requirements here
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "FileUtil.hpp"
#include "FileTypes.hpp"
#include "NPlateHistoryFileReader.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "utildefs.hpp"
#include "UtilityException.hpp"
#include "RealUtilities.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "TimeSystemConverter.hpp"

//#define DEBUG_PARSE_NPLATE_HISTORY_FILE
//#define DEBUG_ADD_DATA_RECORD
//#define DEBUG_NPLATE_DATA
//#define DEBUG_INIT_NPLATE_HISTORY_FILE
//#define DEBUG_INITIALIZE_NPLATE_HISTORY_FILE


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Default constructor
// -----------------------------------------------------------------------------
NPlateHistoryFileReader::NPlateHistoryFileReader() :
   nplateFile    (""),
   startTime     (0.0),
   csName        ("FixedToBody"),
   interpolator  ("Linear"),
   isInitialized (false)
{
}

// -----------------------------------------------------------------------------
// Copy constructor
// -----------------------------------------------------------------------------
NPlateHistoryFileReader::NPlateHistoryFileReader(const NPlateHistoryFileReader &copy) :
   nplateFile    (copy.nplateFile),
   startTime     (copy.startTime),
   csName        (copy.csName),
   interpolator  (copy.interpolator),
   isInitialized (copy.isInitialized)
{
#ifdef DEBUG_INITIALIZE_NPLATE_HISTORY_FILE
   MessageInterface::ShowMessage("NPlateHistoryFileReader::NPlateHistoryFileReader(copy = <%p>)\n", copy);
#endif
   for (unsigned int ii = 0; ii < copy.nplateData.size(); ii++)
   {
      Real timeOffset = copy.nplateData.at(ii)->timeOffset;
      Real az         = copy.nplateData.at(ii)->azimuth;
      Real el         = copy.nplateData.at(ii)->elevation;
      Rvector3 r = copy.nplateData.at(ii)->r;
      AddDataRecord(timeOffset, az, el, r[0], r[1], r[2]);
   }
#ifdef DEBUG_INITIALIZE_NPLATE_HISTORY_FILE
   MessageInterface::ShowMessage("NPlateHistoryFileReader::NPlateHistoryFileReader(copy) end\n");
#endif
}

// -----------------------------------------------------------------------------
// operator=
// -----------------------------------------------------------------------------
NPlateHistoryFileReader& NPlateHistoryFileReader::operator=(const NPlateHistoryFileReader &copy)
{
   if (&copy == this)
      return *this;

   nplateFile    = copy.nplateFile;
   startTime     = copy.startTime;
   csName        = copy.csName;
   interpolator  = copy.interpolator;
   isInitialized = copy.isInitialized;

   for (unsigned int ii = 0; ii < nplateData.size(); ii++)
      delete nplateData[ii];
   nplateData.clear();
   for (unsigned int ii = 0; ii < copy.nplateData.size(); ii++)
   {
      Real timeOffset = copy.nplateData.at(ii)->timeOffset;
      Real az      = copy.nplateData.at(ii)->azimuth;
      Real el      = copy.nplateData.at(ii)->elevation;
      Rvector3 r   = copy.nplateData.at(ii)->r;
      AddDataRecord(timeOffset, az, el, r[0], r[1], r[2]);
   }

   return *this;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
NPlateHistoryFileReader::~NPlateHistoryFileReader()
{
   for (unsigned int ii = 0; ii < nplateData.size(); ii++)
      delete nplateData[ii];
   nplateData.clear();
}

// -----------------------------------------------------------------------------
// void NPlateHistoryFileReader* Clone()
// Return a clone of the NPlateHistoryFileReader.
// -----------------------------------------------------------------------------
NPlateHistoryFileReader* NPlateHistoryFileReader::Clone() const
{
   return (new NPlateHistoryFileReader(*this));
}

//------------------------------------------------------------------------------
// void Initialize()
// Validate and initialize, parse the file, store the data.
//------------------------------------------------------------------------------
void NPlateHistoryFileReader::Initialize()
{
   if (isInitialized) return;

   #ifdef DEBUG_INIT_NPLATE_HISTORY_FILE
      MessageInterface::ShowMessage("Entering NPlateHistoryFileReader::Initialize ...\n");
   #endif
   // Check for the existence of the file
   if (!GmatFileUtil::DoesFileExist(nplateFile))
   {
      std::string errmsg = "Specified NPlate history file \" ";
      errmsg += nplateFile + "\" does not exist.\n";
      throw UtilityException(errmsg);
   }

   // Parse the file
   bool fileParsed = ParseFile();
   if (!fileParsed)
   {
      std::string errmsg = "There is an error opening or reading the ";
      errmsg += "NPlate history file \"" + nplateFile + "\".\n";
      throw UtilityException(errmsg);
   }
   #ifdef DEBUG_INIT_NPLATE_HISTORY_FILE
      MessageInterface::ShowMessage("      in NPlateHistoryFileReader::Initialize, file has been parsed.\n");
   #endif

   isInitialized = true;
}

// -----------------------------------------------------------------------------
// bool SetFile(const std::string &theNPlateHistoryFile)
// Set the NPlate history file name.
// -----------------------------------------------------------------------------
bool NPlateHistoryFileReader::SetFile(const std::string &theNPlateHistoryFile)
{
   nplateFile = theNPlateHistoryFile;
   return true;
}

// -----------------------------------------------------------------------------
// bool SetInterpolator(const std::string &theInterpolator)
// Set the NPlate interpolator.
// -----------------------------------------------------------------------------
bool NPlateHistoryFileReader::SetInterpolator(const std::string &theInterpolator)
{
   if (theInterpolator != "Linear")
      throw UtilityException("Error: Setting 'Interpolation_Method = " + theInterpolator + "' in '" + nplateFile + "' file is invalid. Only 'Linear' is allowed.\n");

   interpolator = theInterpolator;
   return true;
}


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// bool AddDataRecord(Real az, Real el, Real x, Real y, Real z)
// Create a new NPlateDataRecord, initialize it with the input data, and
// add it to the data store.
// -----------------------------------------------------------------------------
bool NPlateHistoryFileReader::AddDataRecord(Real timeOffset, Real az, Real el, Real x, Real y, Real z)
{
   NPlateDataRecord *newNPlate = new NPlateDataRecord(timeOffset, az, el, x, y, z);
   #ifdef DEBUG_ADD_DATA_RECORD
      MessageInterface::ShowMessage(
         "Adding data record:   time offset = %12.10f   azimuth = %12.10f and elevation = %12.10f  r = [%12.10f  %12.10f  %12.10f]\n",
            timeOffset, az, el, x, y, z);
   #endif
   nplateData.push_back(newNPlate);
   return true;
}

// -----------------------------------------------------------------------------
// bool ParseFile()
// Parse the file, validating where possible; store the data
// -----------------------------------------------------------------------------
bool NPlateHistoryFileReader::ParseFile()
{
   if (isInitialized) return true;

   TimeSystemConverter* theTimeConverter = TimeSystemConverter::Instance();

   // Open the file for reading
   std::string   line;
   std::ifstream theNPlateFile(nplateFile.c_str());
   if (!theNPlateFile)  return false;

   // Ignore leading white space
   theNPlateFile.setf(std::ios::skipws);
   // check for an empty file
   if (theNPlateFile.eof())
   {
      std::string errmsg = "Error reading NPlate history file \"";
      errmsg += nplateFile + "\".  ";
      errmsg += "File appears to be empty.\n";
      throw UtilityException(errmsg);
   }
   // Parse the file until end-of-file
   bool readTime = true;
   bool readCS = false;
   bool readInterpolationMethod = false;
   bool readFaceNormal = false;
   
   Rvector3 faceNormal, faceNormalUnit;
   Real timeOffset;

   while (!theNPlateFile.eof())
   {
      //getline(theNPlateFile,line);                      // fix line ending for Linux
      GmatFileUtil::GetLine(&theNPlateFile, line);        // fix line ending for Linux

      // ignore blank lines
      if (GmatStringUtil::IsBlank(line, true))
         continue;

      line = GmatStringUtil::Trim(line);

#ifdef DEBUG_PARSE_NPLATE_HISTORY_FILE
      MessageInterface::ShowMessage("In NPlateHistoryFileReader, line= %s\n",
         line.c_str());
      MessageInterface::ShowMessage("   readTime = %s\n",
         (readTime ? "true" : "false"));
      MessageInterface::ShowMessage("   readCS   = %s\n",
         (readCS ? "true" : "false"));
      MessageInterface::ShowMessage("   readInterpolationMethod   = %s\n",
         (readInterpolationMethod ? "true" : "false"));
      MessageInterface::ShowMessage("   readFaceNormal   = %s\n",
         (readFaceNormal ? "true" : "false"));
#endif

      size_t pos;
      if (readTime)
      {
         // Code to read start time here
         pos = line.find('\'');
         if (pos == line.npos)
         {
            // generate error message
         }
         else
         {
            std::string term = line.substr(0, pos);
            if (GmatStringUtil::RemoveAllBlanks(term) != "Start_Epoch=")
               throw UtilityException("Error: NPlate history file '" + nplateFile + "' has incorrect format of Start_Epoch.\n");
            else
            {
               std::string stime = line.substr(pos);
               pos = stime.find('\'');
               
               if (pos == stime.npos)
                  throw UtilityException("Error: NPlate history file '" + nplateFile + "' has incorrect format of Start_Epoch.\n");
               else
               {
                  // eleminate the first "'"
                  stime = stime.substr(pos + 1);

                  // fimd the second "'"
                  pos = stime.find('\'');
                  if (pos == stime.npos)
                     throw UtilityException("Error: NPlate history file '" + nplateFile + "' has incorrect format of Start_Epoch.\n");
                  else
                  {
                     if (GmatStringUtil::RemoveAllBlanks(stime.substr(pos + 1)) != "")
                        throw UtilityException("Error: NPlate history file '" + nplateFile + "' has incorrect format of Start_Epoch.\n");
                     else
                     {
                        stime = stime.substr(0, pos - 1);
                        startTime = GmatTime(theTimeConverter->ConvertGregorianToMjd(stime));
                     }
                  }
               }
            }
         }
         
         readTime = false;
         readCS = true;
      }
      else if (readCS)
      {
         // Code to read CS here
         std::string line1 = GmatStringUtil::RemoveAllBlanks(line);
         pos = line1.find('=');
         if ((pos == line1.npos)||(pos+1 == line1.length()))
            throw UtilityException("Error: NPlate history file '" + nplateFile + "' does not have the name of coordinate system.\n");

         if (line1.substr(0,pos) != "Coordinate_System")
            throw UtilityException("Error: NPlate history file '" + nplateFile + "' has incorrect format of Coordinate_System.\n");

         csName = line1.substr(pos + 1);

         readCS = false;
         readInterpolationMethod = true;
      }
      else if (readInterpolationMethod)
      {
         // Code to read interpolation method here
         std::string line1 = GmatStringUtil::RemoveAllBlanks(line);
         pos = line1.find('=');
         if ((pos == line1.npos) || (pos + 1 == line1.length()))
            throw UtilityException("Error: NPlate history file '" + nplateFile + "' does not have the name of interpolation method.\n");

         if (line1.substr(0, pos) != "Interpolation_Method")
            throw UtilityException("Error: NPlate history file '" + nplateFile + "' has incorrect format of Interpolation_Method.\n");

         SetInterpolator(line1.substr(pos + 1));

         readInterpolationMethod = false;
         readFaceNormal = true;
      }
      else if (readFaceNormal)
      {
         std::istringstream lineStr;
         lineStr.str(line);
         lineStr >> timeOffset >> faceNormal[0] >> faceNormal[1] >> faceNormal[2];

         if (nplateData.size() > 0)
         {
            if (timeOffset < nplateData[nplateData.size() - 1]->timeOffset)
               throw UtilityException("Error: In NPlate face normal history file '" + nplateFile + "', time offset in line '" + lineStr.str() + "' is not in ascending order.\n");
            else if (timeOffset == nplateData[nplateData.size() - 1]->timeOffset)
               throw UtilityException("Error: In NPlate face normal history file '" + nplateFile + "', it has a duplicated time offset in line '" + lineStr.str() + "'.\n");
         }

         if (faceNormal.IsZeroVector())
            throw UtilityException("Error: In NPlate face normal history file '" + nplateFile + "', zero normal vector in line '" + lineStr.str() + "' is not allowed.\n");

         faceNormalUnit = faceNormal / faceNormal.GetMagnitude();
         
         ///@todo: add code to calculate az and el here
         Real azVal = GmatMathUtil::ATan2(faceNormal[1], faceNormal[0]);
         Real elVal = GmatMathUtil::ASin(faceNormalUnit[2]);

         #ifdef DEBUG_NPLATE_DATA
            MessageInterface::ShowMessage("timeOffset = %12.10f   faceNormal = [%12.10f   %12.10f   %12.10f]   faceNormalUnit = [%12.10f   %12.10f   %12.10f]\n",
               timeOffset, faceNormal[0], faceNormal[1], faceNormal[2],
               faceNormalUnit[0], faceNormalUnit[1], faceNormalUnit[2]);
         #endif

         AddDataRecord(timeOffset, azVal, elVal, faceNormal[0], faceNormal[1], faceNormal[2]);
      }
      else
      {
         std::string errmsg = "Error reading NPlate history file ";
         errmsg            += nplateFile + ".  Internal error.\n";
         throw UtilityException(errmsg);
      }
   }

   if (theNPlateFile.is_open())  theNPlateFile.close();

   return true;
}


// -----------------------------------------------------------------------------
// Rvector3 Interpolate(Real x, Real x1, real x2, Rvector3 y1, Rvector3 y2)
// Performs bilinear interpolation of the input values.
// -----------------------------------------------------------------------------
Rvector3 NPlateHistoryFileReader::GetFaceNormal(GmatTime t)
{
   // Specify face normal unit vector at time t
   Real offset = GmatTime(t - startTime).GetTimeInSec();
   
   if ((offset < nplateData[0]->timeOffset) || (offset > nplateData[nplateData.size() - 1]->timeOffset))
   {
      GmatTime t1 = startTime.SubtractSeconds(-nplateData[0]->timeOffset);
      GmatTime t2 = startTime.SubtractSeconds(-nplateData[nplateData.size() - 1]->timeOffset);
      MessageInterface::ShowMessage("Time t = %s  is out side of time range [%s  %s]\n", t.ToString().c_str(), t1.ToString().c_str(), t2.ToString().c_str());
      std::stringstream ss;
      ss << "Error: Cannot get face normal unit vector. Time " << t.ToString() << " is out of range";
      throw UtilityException(ss.str());
   }

   Rvector3 result;
   for (Integer i = 0; i < nplateData.size()-1; ++i)
   {
      if ((nplateData[i]->timeOffset <= offset) && (offset <= nplateData[i + 1]->timeOffset))
      {
         // specify face normal unit vector 
         Real factor = (offset - nplateData[i]->timeOffset) / (nplateData[i+1]->timeOffset - nplateData[i]->timeOffset);
         //Real dAz = (nplateData[i+1]->azimuth - nplateData[i]->azimuth)*factor;
         //Real dEl = (nplateData[i+1]->elevation - nplateData[i]->elevation)*factor;
         //Real az = nplateData[i]->azimuth + dAz;
         //Real el = nplateData[i]->elevation + dEl;
         Real az = AzimuthInterpolation(nplateData[i]->azimuth, nplateData[i + 1]->azimuth, factor);
         Real el = ElevationInterpolation(nplateData[i]->elevation, nplateData[i + 1]->elevation, factor);
         result[0] = GmatMathUtil::Cos(el)*GmatMathUtil::Cos(az);     // x value
         result[1] = GmatMathUtil::Cos(el)*GmatMathUtil::Sin(az);     // y value
         result[2] = GmatMathUtil::Sin(el);     // z value
         break;
      }
   }

   return result;
}



Real NPlateHistoryFileReader::AzimuthInterpolation(Real angle1, Real angle2, Real factor)
{
   Real val = angle1 * angle2;
   if (val >= 0.0)
   {
      // case 1: both angle1 and angle2 have the same sign. That means they have no change of value due to their true values outside the range (-pi, +pi]
      return (angle1 + (angle2 - angle1)*factor);
   }
   else
   {
      // both angle1 and engle2 have oposited sign
      if (GmatMathUtil::Abs(val) < 1.0)
      {
         // both angle1 and angle2 are near zero. That means their true values have no change of value due to their true values outside the range (-pi, +pi]
         return (angle1 + (angle2 - angle1)*factor);
      }
      else
      {
         // both angle1 and angle2 have oposited sign and far away zero. That means one of them has change its value due to its true value is outside the range (-pi, +pi]
         // Adjust value of angle2 to the same sign of angle1
         if (angle1 > 0.0)
            angle2 = angle2 + GmatMathConstants::TWO_PI;
         else
            angle2 = angle2 - GmatMathConstants::TWO_PI;
         return (angle1 + (angle2 - angle1)*factor);
      }
   }
}


Real NPlateHistoryFileReader::ElevationInterpolation(Real angle1, Real angle2, Real factor)
{
   Real val = angle1 * angle2;
   if (val >= 0.0)
   {
      // case 1: both angle1 and angle2 have the same sign. That means they have no change of value due to their true values outside the range (-pi, +pi]
      return (angle1 + (angle2 - angle1)*factor);
   }
   else
   {
      // both angle1 and engle2 have oposited sign
      if (GmatMathUtil::Abs(val) < 1.0)
      {
         // both angle1 and angle2 are near zero. That means their true values have no change of value due to their true values outside the range (-pi, +pi]
         return (angle1 + (angle2 - angle1)*factor);
      }
      else
      {
         // both angle1 and angle2 have oposited sign and far away zero. That means one of them has change its value due to its true value is outside the range (-pi, +pi]
         // Adjust value of angle2 to the same sign of angle1
         if (angle1 > 0.0)
            angle2 = angle2 + GmatMathConstants::PI;
         else
            angle2 = angle2 - GmatMathConstants::PI;
         return (angle1 + (angle2 - angle1)*factor);
      }
   }
}

