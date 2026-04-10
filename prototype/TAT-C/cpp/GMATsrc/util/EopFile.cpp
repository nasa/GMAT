//$Id$
//------------------------------------------------------------------------------
//                                  EopFile
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/01/26
//
/**
 * Implementation of the EopFile class.  This is the code that reads the polar
 * motion information from EOP file (type 14 C04 or type 08 C04).  NOTE: reading
 * of Finals file is no longer supported.
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "utildefs.hpp"
//#include "GmatGlobal.hpp"
#include "EopFile.hpp"
#include "TimeTypes.hpp"
#include "UtilityException.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "TimeSystemConverter.hpp"

//#define DEBUG_OFFSET
//#define DEBUG_EOP_READ
//#define DEBUG_EOP_INITIALIZE

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Integer EopFile::MAX_TABLE_SIZE = 50405;  // up to year >= 2100

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  EopFile(const std::string &fileName, 
//          GmatEop::EopFileType eop = GmatEop::EOP_C04);
//---------------------------------------------------------------------------
/**
 * Constructs base EopFile structures used in derived classes
 * (default constructor).
 *
 * @param fileNme  EOP file name.
 */
//---------------------------------------------------------------------------
EopFile::EopFile(const std::string &fileName, GmatEop::EopFileType eop) :
eopFType        (eop),
eopFileName     (fileName),
tableSz         (0),
polarMotion     (new Rmatrix(MAX_TABLE_SIZE,4)),
ut1UtcOffsets   (new Rmatrix(MAX_TABLE_SIZE,2)),
taiTime         (new Rvector(MAX_TABLE_SIZE)),
lastUtcJd       (0.0),
lastOffset      (0.0),
lastIndex       (0),
isInitialized   (false)
{
   theTimeConverter = TimeSystemConverter::Instance();
}

//---------------------------------------------------------------------------
//  EopFile(const EopFile &eopF);
//---------------------------------------------------------------------------
/**
 * Constructs base EopFile structures, by copying 
 * the input instance (copy constructor).
 *
 * @param eopF   EopFile instance to copy to create "this" 
 *               instance.
 */
//---------------------------------------------------------------------------
EopFile::EopFile(const EopFile &eopF) :
eopFType        (eopF.eopFType),
eopFileName     (eopF.eopFileName),
tableSz         (eopF.tableSz),
polarMotion     (new Rmatrix(*(eopF.polarMotion))),
ut1UtcOffsets   (new Rmatrix(*(eopF.ut1UtcOffsets))),
taiTime         (new Rvector(*(eopF.taiTime))),
lastUtcJd       (eopF.lastUtcJd),
lastOffset      (eopF.lastOffset),
lastIndex       (eopF.lastIndex),
isInitialized   (eopF.isInitialized)
{
   theTimeConverter = TimeSystemConverter::Instance();
}

//---------------------------------------------------------------------------
//  EopFile& operator=(const EopFile &eopF)
//---------------------------------------------------------------------------
/**
 * Assignment operator for EopFile structures.
 *
 * @param eopF  The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const EopFile& EopFile::operator=(const EopFile &eopF)
{
   if (&eopF == this)
      return *this;
   eopFType      = eopF.eopFType;
   eopFileName   = eopF.eopFileName;
   tableSz       = eopF.tableSz;
   delete polarMotion;
   delete ut1UtcOffsets;
   delete taiTime;

   polarMotion   = new Rmatrix(*(eopF.polarMotion));
   ut1UtcOffsets = new Rmatrix(*(eopF.ut1UtcOffsets));
   taiTime       = new Rvector(*(eopF.taiTime));

   lastUtcJd     = eopF.lastUtcJd;
   lastOffset    = eopF.lastOffset;
   lastIndex     = eopF.lastIndex;
   isInitialized = eopF.isInitialized;

   return *this;
}


//---------------------------------------------------------------------------
//  ~EopFile()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
EopFile::~EopFile()
{
   #ifdef DEBUG_EOP_INITIALIZE
      MessageInterface::ShowMessage("--- DESTRUCTING EOPFILE\n");
   #endif
   if (polarMotion)   delete polarMotion;
   if (ut1UtcOffsets) delete ut1UtcOffsets;
   if (taiTime)       delete taiTime;
}


//------------------------------------------------------------------------------
//  void  Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the EopFile class, by reading the file and 
 * storing the UT1-UTC offset and polar motion data.
 *
 */
//------------------------------------------------------------------------------
void EopFile::Initialize()
{
   #ifdef DEBUG_EOP_INITIALIZE
      MessageInterface::ShowMessage("Initializing EopFile: eopFileName = %s\n", eopFileName.c_str());
   #endif
   if (isInitialized) return;
   
   std::string   line;
   std::ifstream eopFile(eopFileName.c_str());
   if (!eopFile)
      throw UtilityException("Error opening EopFile " + 
                             eopFileName);
   eopFile.setf(std::ios::skipws);
   
   // Delete the old data if a file has already been read
   if (tableSz > 0)
   {
      #ifdef DEBUG_EOP_INITIALIZE
         MessageInterface::ShowMessage("--- deleting OLD polarMotion and ut1UtcOffsets and resetting ...\n");
      #endif
      delete polarMotion;
      delete ut1UtcOffsets;
      delete taiTime;
      
      polarMotion   = new Rmatrix(MAX_TABLE_SIZE,4);
      ut1UtcOffsets = new Rmatrix(MAX_TABLE_SIZE,2);
      taiTime       = new Rvector(MAX_TABLE_SIZE);
      tableSz       = 0;
   }
   
   if (eopFType == GmatEop::EOP_C04)
   {
      #ifdef DEBUG_EOP_INITIALIZE
         MessageInterface::ShowMessage("--- attempting to read file %s ...\n", eopFileName.c_str());
      #endif
      // read up to the first data line
      bool startNow = false;
      std::string   firstWord;
      while ((!startNow) && (!eopFile.eof()))
      {
         getline(eopFile,line);
         std::istringstream lineStr;
         lineStr.str(line);
         lineStr >> firstWord;
         if (firstWord == "1962") startNow = true;
      }
      if (startNow == false)
         throw UtilityException("Unable to read EopFile.");
      // now start reading the data
      Integer     year, day, mjd;
      std::string month;
      Real        x, y, ut1_utc, lod; // ignore dPsi, dEpsilon (or dX, dY)
      bool done = false;
      while (!done)
      {
         if (!IsBlank(line.c_str()))
         {
            std::istringstream lineS;
            lineS.str(line);
            lineS >> year >> month >> day >> mjd >> x >> y >> ut1_utc >> lod;
            #ifdef DEBUG_EOP_READ
               //MessageInterface::ShowMessage(
                  //"%d   %s   %d   %d   %.12f   %.12f   %.12f   %.12f\n",
                  //year, month.c_str(), day, mjd, x, y, ut1_utc, lod);
               MessageInterface::ShowMessage(
                  "%d   %.12f   %.12f   %.12f   %.12f\n",
                  mjd, x, y, ut1_utc, lod);
            #endif
            ut1UtcOffsets->SetElement(tableSz,0,mjd + GmatTimeConstants::JD_NOV_17_1858);
            ut1UtcOffsets->SetElement(tableSz,1,ut1_utc);

            GmatEpoch utcepoch = mjd + GmatTimeConstants::JD_NOV_17_1858 - GmatTimeConstants::JD_JAN_5_1941;
            GmatEpoch a1JD = theTimeConverter->Convert(utcepoch, TimeSystemConverter::UTCMJD, TimeSystemConverter::TAIMJD);
            taiTime->SetElement(tableSz, a1JD); // is actually TAI

            polarMotion->SetElement(tableSz,0,mjd + GmatTimeConstants::JD_NOV_17_1858);
            polarMotion->SetElement(tableSz,1,x);
            polarMotion->SetElement(tableSz,2,y);
            polarMotion->SetElement(tableSz,3,lod);
            tableSz++;
         }
         if (eopFile.eof())   done = true;
         else                 getline(eopFile, line);
      }
   }
   else if (eopFType == GmatEop::FINALS)  // No longer supported!!
   {
      char        ipFlag1, ipFlag2;
      Real        mjd, x, y, ut1_utc, lod;
      // ignore dutc, lod, dlod, I/P, dPsi ddPsi, dEpsilon, ddEpsilon, Bull. B data?
      Real        dx, dy, dut1_utc;
      bool        done = false;
      while (!done && (!eopFile.eof()))
      {
         getline(eopFile, line);
         if (!IsBlank(line.c_str()))
         {
            std::istringstream lineS;
            lineS.str(line);
            lineS.ignore(6);
            lineS >> mjd >> ipFlag1
               >> x >> dx >> y >> dy >> ipFlag2 >> ut1_utc >> dut1_utc >> lod;
            // We're done when we reach the end of the predicted values
            if ((ipFlag1 != 'I') && (ipFlag1 != 'P')) 
            {
               done = true;
            }
            else
            {
               ut1UtcOffsets->SetElement(tableSz,0,mjd + GmatTimeConstants::JD_NOV_17_1858);
               ut1UtcOffsets->SetElement(tableSz,1,ut1_utc);

               GmatEpoch utcepoch = mjd + GmatTimeConstants::JD_NOV_17_1858 - GmatTimeConstants::JD_JAN_5_1941;
               GmatEpoch a1JD = theTimeConverter->Convert(utcepoch, TimeSystemConverter::UTCMJD, TimeSystemConverter::TAIMJD);
               taiTime->SetElement(tableSz, a1JD); // is actually TAI

               polarMotion->SetElement(tableSz,0,mjd + GmatTimeConstants::JD_NOV_17_1858);
               polarMotion->SetElement(tableSz,1,x);
               polarMotion->SetElement(tableSz,2,y);
               polarMotion->SetElement(tableSz,3,lod*1.0e-03); // convert to seconds
               tableSz++;
            }
         }
      }
   }
   else
   {
      throw UtilityException("Error In EopFile - file type unknown.");
   }
   if (eopFile.is_open())  eopFile.close();
   // set the last value to the end of the file (since we search from back 
   // to front)
   lastUtcJd  = ut1UtcOffsets->GetElement((tableSz-1), 0);
   lastOffset = ut1UtcOffsets->GetElement((tableSz-1), 1);
   lastTaiMjd = taiTime->GetElement(tableSz - 1);
   lastIndex  = tableSz - 1;
   
   previousIndex = lastIndex;
   
   isInitialized = true;

//   // Set the pointer on the GmatGlobal
//   GmatGlobal::Instance()->SetEopFile(this);
}


//---------------------------------------------------------------------------
//  void ResetEopFile(const std::string &toName, 
//                    GmatEop::EopFileType toType = GmatEop::EOP_C04)
//---------------------------------------------------------------------------
void EopFile::ResetEopFile(const std::string &toName, 
                           GmatEop::EopFileType toType)
{
   if (eopFileName != toName)
   {
      eopFileName   = toName;
      eopFType      = toType;
      isInitialized = false;
   }
}

//---------------------------------------------------------------------------
//  std::string GetFileName() const
//---------------------------------------------------------------------------
/**
 * Returns the name of the EOP file.
 *
 * @return name of the EOp file.  
 */
//---------------------------------------------------------------------------
std::string EopFile::GetFileName() const
{
   return eopFileName;
}


//---------------------------------------------------------------------------
//  Real GetUt1UtcOffset(const Real taiMjd)
//---------------------------------------------------------------------------
 /**
 * Returns the UT1-UTC offset for the given tai mjd.
 *
 * @param taiMjd  The tai mjd for which to return the offset.
 *
 * @return UT1-UTC offset for the given time; values between table entries
 *         are interpolated linearly.  
 */
//---------------------------------------------------------------------------
Real EopFile::GetUt1UtcOffset(const Real taiMjd)
{
   //MessageInterface::ShowMessage("===> GetUt1UtcOffset() utcMjd=%f\n", utcMjd);
   
   if (!isInitialized)  Initialize();
   
   if (lastTaiMjd == taiMjd) return lastOffset;
   
   Integer         i = 0;
   const Real* data  = ut1UtcOffsets->GetDataVector();
   Integer col     = ut1UtcOffsets->GetNumColumns();
   Real    off     = 0.0;
   /*
   Integer row     = ut1UtcOffsets->GetNumRows();
   MessageInterface::ShowMessage
      ("===> after ut1UtcOffsets->GetData() tableSz=%d, row=%d, col=%d\n",
       tableSz, row, col);
   */
   if (taiMjd >= taiTime->GetElement(tableSz-1))
   {
      off = data[((tableSz - 1) * col) + 1];
      lastIndex = tableSz - 1;
   }
   else if (taiMjd <= taiTime->GetElement(0))
   {
      off = data[1];
      lastIndex = 0;
   }
   else
   {
      //// for case: utcJD_Min < utcJD < utcJD_Max
      if (taiMjd  < lastTaiMjd)
      {
         if (lastIndex > (tableSz-2)) i = tableSz - 2;
         else                         i = lastIndex;
         for (; i >=0; i--)
         {
            if (taiMjd >= taiTime->GetElement(i)) 
            {
               //Real diffJD  = data[(i+1)*col] - 
               //               data[i*col];
               //Real whereJD = utcJD - data[i*col];
               Real diffJD = taiTime->GetElement(i + 1) - taiTime->GetElement(i);
               Real whereJD = taiMjd - taiTime->GetElement(i);

               Real ratio = whereJD / diffJD;
               Real diffOff = data[(i + 1)*col + 1] -
                              data[i*col + 1];
               Real errorInSec = (diffJD - 1.0)*GmatTimeConstants::SECS_PER_DAY;
               if (GmatMathUtil::Abs(errorInSec) > 0.6)
                  diffOff = diffOff - GmatMathUtil::Round(errorInSec);
               off = data[i*col + 1] + ratio * diffOff;
               lastIndex = i;
               break;
            }
         }
      }
      else
      {
         for (i = lastIndex; i <= tableSz-2; i++)
         {
            if ((taiMjd >= taiTime->GetElement(i)) &&
                (taiMjd <  taiTime->GetElement(i+1)))
            {
               //Real diffJD  = data[(i+1)*col] - 
               //               data[i*col];
               //Real whereJD = utcJD - data[i*col];
               Real diffJD = taiTime->GetElement(i + 1) - taiTime->GetElement(i);
               Real whereJD = taiMjd - taiTime->GetElement(i);

               Real ratio = whereJD / diffJD;
               Real diffOff = data[(i + 1)*col + 1] -
                              data[i*col + 1];
               Real errorInSec = (diffJD - 1.0)*GmatTimeConstants::SECS_PER_DAY;
               if (GmatMathUtil::Abs(errorInSec) > 0.6)
                  diffOff = diffOff - GmatMathUtil::Round(errorInSec);
               off = data[i*col + 1] + ratio * diffOff;
               lastIndex    = i;
               break;
            }
         }
      }
   }
   lastTaiMjd = taiMjd;
   lastOffset = off;
   #ifdef DEBUG_OFFSET
      MessageInterface::ShowMessage
         ("===> after completion off=%f, lastTaiMjd=%f, lastOffset=%f\n",
          off, lastTaiMjd, lastOffset);
   #endif
   return off;
}

//---------------------------------------------------------------------------
//  Rmatrix GetPolarMotionData()
//---------------------------------------------------------------------------
/**
 * Returns the polar motion data.
 * for each row:  mjd, x, y
 *
 * @return polar motion data.
 */
//---------------------------------------------------------------------------
Rmatrix EopFile::GetPolarMotionData()
{
   if (!isInitialized)  Initialize();
   
   return Rmatrix(*polarMotion);
}

//---------------------------------------------------------------------------
//  bool GetPolarMotionAndLod(const GmatTime &forUtcMjd, Real &xval, Real  &yval,
//                            Real &lodval)
//---------------------------------------------------------------------------
/**
 * Returns the polar motion data X, Y, and LOD, for the input UTC MJD time.
 * 
 * @param forUtcMjd time for which to return the data
 * @param xval      return X value of polar motion data (arcsec)
 * @param yval      return Y value of polar motion data (arcsec)
 * @param lodval    return LOD value (seconds)
 *
 */
//---------------------------------------------------------------------------
bool EopFile::GetPolarMotionAndLod(const GmatTime &forUtcMjd, Real &xval, Real  &yval,
                                   Real &lodval)
{
   if (!isInitialized)  Initialize();
   
   Integer i = 0;
   GmatTime    utcJD = forUtcMjd + GmatTimeConstants::JD_NOV_17_1858;
   Integer col = polarMotion->GetNumColumns();
   const Real *data = polarMotion->GetDataVector();
   
   // if it's before the time on the file, return the first values
   
   //if (utcJD <= polarMotion->GetElement(0,0))
   if (utcJD <= data[0])
   {
      xval   = data[1];
      yval   = data[2];
      lodval = data[3];
//       xval   = polarMotion->GetElement(0,1);
//       yval   = polarMotion->GetElement(0,2);
//       lodval = polarMotion->GetElement(0,3);
   }
   else
   {
      // First try to use the index from the last call
      if ((previousIndex < tableSz - 1) && 
          (utcJD <= data[(previousIndex+1)*col]) &&
          (utcJD >= data[previousIndex*col]))
      {
         Integer leftIndex  = previousIndex*col, 
                 rightIndex = (previousIndex+1)*col;
         // otherwise, interpolate between values
         Real diffJD  = data[rightIndex] - data[leftIndex];
         GmatTime whereJD = utcJD - data[leftIndex];

         Real ratio = whereJD.GetMjd() / diffJD;
         Real diffX   = data[rightIndex + 1] - data[leftIndex + 1];
         Real diffY   = data[rightIndex + 2] - data[leftIndex + 2];


         //Real diffLOD = polarMotion->GetElement(i+1,3) -
         //               polarMotion->GetElement(i,3);
         xval   = data[leftIndex + 1] + ratio * diffX;
         yval   = data[leftIndex + 2] + ratio * diffY;

         // 2005.02.23 - Steve says not to interpolate lod
         //lodval = polarMotion->GetElement(i,3) + ratio * diffLOD;
         lodval = data[leftIndex + 3];
      }
      else
      {
         for (i = (tableSz - 1); i >= 0; i--)
         {
            //if (utcJD >= polarMotion->GetElement(i,0))
            if (utcJD >= data[i*col + 0])
            {
               // if it's greater than the last entry in the table, then return the 
               // last value
               if (i == (tableSz -1))
               {
                  xval   = data[i*col + 1];
                  yval   = data[i*col + 2];
                  lodval = data[i*col + 3];
               }
               else
               {
                  // otherwise, interpolate between values
                  Real diffJD  = data[(i+1)*col + 0] - 
                                 data[i*col + 0];
                  GmatTime whereJD = utcJD - data[i*col + 0];

                  Real ratio   = whereJD.GetMjd() / diffJD;
                  Real diffX   = data[(i+1)*col + 1] -
                                 data[i*col + 1];
                  Real diffY   = data[(i+1)*col + 2] -
                                 data[i*col + 2];
                  //Real diffLOD = polarMotion->GetElement(i+1,3) -
                  //               polarMotion->GetElement(i,3);
                  xval   = data[i*col + 1] + ratio * diffX;
                  yval   = data[i*col + 2] + ratio * diffY;
                  // 2005.02.23 - Steve says not to interpolate lod
                  //lodval = polarMotion->GetElement(i,3) + ratio * diffLOD;
                  lodval = data[i*col + 3];
                  
               }
               // Buffer the index for performance
               previousIndex = i;
               break;
            }
         }
      }
   }
   return true;
}

//------------------------------------------------------------------------------
//  bool IsBlank(char* aLine)
//------------------------------------------------------------------------------
/**
 * This method returns true if the string is empty or is all white space.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool EopFile::IsBlank(const char* aLine)
{
   Integer i;
   for (i=0;i<(int)strlen(aLine);i++)
   {
      //loj: 5/18/04 if (!isblank(aLine[i])) return false;
      if (!isspace(aLine[i])) return false;
   }
   return true;
}


void EopFile::GetTimeRange(Real& timeMin, Real& timeMax)
{
   static RealArray ra;
   const Real *data = polarMotion->GetDataVector();
   Integer col = polarMotion->GetNumColumns();
   Integer row = polarMotion->GetNumRows();
   Real timeUtcMjdMin = data[0] - GmatTimeConstants::JD_JAN_5_1941;                       // unit: UTCMjd w.r.t ref GmatTimeConstants::JD_JAN_5_1941
   Real timeUtcMjdMax = data[(tableSz - 1)*col] - GmatTimeConstants::JD_JAN_5_1941;       // unit: UTCMjd w.r.t ref GmatTimeConstants::JD_JAN_5_1941
   timeMin = theTimeConverter->Convert(timeUtcMjdMin, TimeSystemConverter::UTCMJD, TimeSystemConverter::A1MJD);    // unit: A1Mjd
   timeMax = theTimeConverter->Convert(timeUtcMjdMax, TimeSystemConverter::UTCMJD, TimeSystemConverter::A1MJD);    // unit: A1Mjd

   //MessageInterface::ShowMessage("timeMin = %lf A1Mjd    timeMax = %lf A1Mjd\n", timeMin, timeMax);
}
