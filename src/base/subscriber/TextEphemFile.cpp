//$Id$
//------------------------------------------------------------------------------
//                                  TextEphemFile
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun / NASA
// Created: 2006/03/29
//
/**
 * Writes the specified parameter value to a text ephemeris file.
 */
//------------------------------------------------------------------------------

#include "TextEphemFile.hpp"
#include "CubicSplineInterpolator.hpp"
#include "Moderator.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include <iomanip>
#include <cstdlib>

//#define DEBUG_EPHEMFILE 1
//#define DEBUG_EPHEMFILE_DATA 1
//#define DEBUG_EPHEMFILE_FIRST 1
//#define DEBUG_EPHEMFILE_LAST 1
//#define DEBUG_EPHEMFILE_WARNING 1
//#define DEBUG_RENAME 1

//---------------------------------
// static data
//---------------------------------
const std::string
TextEphemFile::PARAMETER_TEXT[TextEphemFileParamCount - ReportFileParamCount] =
{
   "EpochFormat",
   "Interval",
   "CoordinateSystem",
};

const Gmat::ParameterType
TextEphemFile::PARAMETER_TYPE[TextEphemFileParamCount - ReportFileParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
};

static const Real TIME_TOL = 1.0e-8;

//------------------------------------------------------------------------------
// TextEphemFile(const std::string &type, const std::string &name,
//               const std::string &fname)
//------------------------------------------------------------------------------
TextEphemFile::TextEphemFile(const std::string &type, const std::string &name,
                             const std::string &fname, Parameter *firstVarParam) :
   ReportFile(type, name, fname, firstVarParam)
{
   // create Interpolator
   mInterpolator = new CubicSplineInterpolator("InternalInterpolator", 6);
   
   // rename data file name
   //fileName = fname + ".tempdata$$$";
   fileName = fname + ".data";
   mHeaderFileName = fname;
   mEpochFormat = "";
   mCoordSysName = "";
   mIntervalInSec = 0.0;
   mCurrA1Mjd = 0.0;
   mOutputA1Mjd = 0.0;
   mStartA1Mjd = 0.0;
   mStopA1Mjd = 0.0;
   
   mNumValidPoints = 0;
   mBufferSize = mInterpolator->GetBufferSize();
   mEpochSysId = 0;
   mIsGregorian = false;
   
   #if DEBUG_EPHEM_FILE
   MessageInterface::ShowMessage
      ("TextEphemFile() Constructor: mBufferSize=%d\n", mBufferSize);
   #endif
}


//------------------------------------------------------------------------------
// ~TextEphemFile(void)
//------------------------------------------------------------------------------
TextEphemFile::~TextEphemFile(void)
{
   delete mInterpolator;
   dstream.flush();
   dstream.close();
}


//------------------------------------------------------------------------------
// TextEphemFile(const TextEphemFile &copy)
//------------------------------------------------------------------------------
TextEphemFile::TextEphemFile(const TextEphemFile &copy) :
   ReportFile(copy)
{
   mHeaderFileName = copy.mHeaderFileName;
   mEpochFormat = copy.mEpochFormat;
   mCoordSysName = copy.mCoordSysName;
   mIntervalInSec = copy.mIntervalInSec;
   mCurrA1Mjd = copy.mCurrA1Mjd;
   mOutputA1Mjd = copy.mOutputA1Mjd;
   mStartA1Mjd = copy.mStartA1Mjd;
   mStopA1Mjd = copy.mStopA1Mjd;
   
   // create Interpolator
   mInterpolator = (Interpolator*)(copy.mInterpolator->Clone());
   
   mNumValidPoints = 0;
   mBufferSize = mInterpolator->GetBufferSize();
   mEpochSysId = copy.mEpochSysId;
   mIsGregorian = copy.mIsGregorian;
   
   Integer i;
   for (i = 0; i < 7; i++)
      mColWidth[i] = copy.mColWidth[i];
   
   for (i = 0; i < 6; i++)
      mOutputVals[i] = copy.mOutputVals[i];
   
   for (i = 0; i < BUFFER_SIZE; i++)
   {
      mTimeBuffer[i] = copy.mTimeBuffer[i];
      mXposBuffer[i] = copy.mXposBuffer[i];
      mYposBuffer[i] = copy.mYposBuffer[i];
      mZposBuffer[i] = copy.mZposBuffer[i];
      mXvelBuffer[i] = copy.mXvelBuffer[i];
      mYvelBuffer[i] = copy.mYvelBuffer[i];
      mZvelBuffer[i] = copy.mZvelBuffer[i];
   }
   
   #if DEBUG_EPHEM_FILE
   MessageInterface::ShowMessage
      ("TextEphemFile() Copy Constructor: mBufferSize=%d\n", mBufferSize);
   #endif
}


//------------------------------------------------------------------------------
// TextEphemFile& TextEphemFile::operator=(const TextEphemFile& right)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
TextEphemFile& TextEphemFile::operator=(const TextEphemFile& right)
{
   if (this == &right)
      return *this;

   ReportFile::operator=(right);
   
   mHeaderFileName = right.mHeaderFileName;
   mEpochFormat = right.mEpochFormat;
   mCoordSysName = right.mCoordSysName;
   mIntervalInSec = right.mIntervalInSec;
   mCurrA1Mjd = right.mCurrA1Mjd;
   mOutputA1Mjd = right.mOutputA1Mjd;
   mStartA1Mjd = right.mStartA1Mjd;
   mStopA1Mjd = right.mStopA1Mjd;
   
   // create Interpolator
   mInterpolator = (Interpolator*)(right.mInterpolator->Clone());
   
   mNumValidPoints = 0;
   mBufferSize = mInterpolator->GetBufferSize();
   mEpochSysId = right.mEpochSysId;
   mIsGregorian = right.mIsGregorian;
   
   Integer i;
   for (i = 0; i < 7; i++)
      mColWidth[i] = right.mColWidth[i];
      
   for (i = 0; i < 6; i++)
      mOutputVals[i] = right.mOutputVals[i];
      
   for (i = 0; i < BUFFER_SIZE; i++)
   {
           mTimeBuffer[i] = right.mTimeBuffer[i];
      mXposBuffer[i] = right.mXposBuffer[i];
      mYposBuffer[i] = right.mYposBuffer[i];
      mZposBuffer[i] = right.mZposBuffer[i];
      mXvelBuffer[i] = right.mXvelBuffer[i];
      mYvelBuffer[i] = right.mYvelBuffer[i];
      mZvelBuffer[i] = right.mZvelBuffer[i];
   }

   #if DEBUG_EPHEM_FILE
   MessageInterface::ShowMessage
      ("TextEphemFile() = operator: mBufferSize=%d\n", mBufferSize);
   #endif
   
   return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the TextEphemFile.
 *
 * @return clone of the TextEphemFile.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TextEphemFile::Clone(void) const
{
   return (new TextEphemFile(*this));
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer TextEphemFile::GetParameterID(const std::string &str) const
{
   for (Integer i = ReportFileParamCount; i < TextEphemFileParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ReportFileParamCount])
         return i;
   }

   return ReportFile::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string TextEphemFile::GetParameterText(const Integer id) const
{
    if (id >= ReportFileParamCount && id < TextEphemFileParamCount)
        return PARAMETER_TEXT[id - ReportFileParamCount];
    else
        return ReportFile::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType TextEphemFile::GetParameterType(const Integer id) const
{
    if (id >= ReportFileParamCount && id < TextEphemFileParamCount)
        return PARAMETER_TYPE[id - ReportFileParamCount];
    else
        return ReportFile::GetParameterType(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string TextEphemFile::GetParameterTypeString(const Integer id) const
{
   if (id >= ReportFileParamCount && id < TextEphemFileParamCount)
      return ReportFile::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return ReportFile::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real TextEphemFile::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case INTERVAL:
      return mIntervalInSec;
   default:
      return ReportFile::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real TextEphemFile::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real TextEphemFile::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case INTERVAL:
      mIntervalInSec = value;
      return mIntervalInSec;
   default:
      return ReportFile::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
Real TextEphemFile::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string TextEphemFile::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case EPOCH_FORMAT:
      return mEpochFormat;
   case COORD_SYSTEM:
      return mEpochFormat;
   default:
      return ReportFile::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string TextEphemFile::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool TextEphemFile::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case EPOCH_FORMAT:
      mEpochFormat = value;
      SaveEpochType();
      return true;
   case COORD_SYSTEM:
      mCoordSysName = value;
      return true;
   default:
      return ReportFile::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool TextEphemFile::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//--------------------------------------
// protected methods
//--------------------------------------


//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool TextEphemFile::Distribute(const Real * dat, Integer len)
{
   if (!active)
      return true;

   if (isEndOfRun)
   {
      #if DEBUG_EPHEMFILE_LAST
      MessageInterface::ShowMessage
         ("==> TextEphemFile::Distribute() EndOfRun: mCurrA1Mjd=%f, mOutputA1Mjd=%f\n",
          mCurrA1Mjd, mOutputA1Mjd);
      #endif
      
      // Write final data
      if (IsTimeToWrite() || GmatMathUtil::IsEqual(mOutputA1Mjd, mTimeBuffer[9], TIME_TOL))
         WriteData();

      // Write header file
      WriteEphemHeader();
      return false;
   }
   
   if (len == 0)
   {
      return false;
   }
   
   // Skip if targeting
   if ((runstate == Gmat::TARGETING) || (runstate == Gmat::OPTIMIZING) ||
       (runstate == Gmat::SOLVING))
      return true;
   
   if (mNumParams > 0)
   {      
      if (!dstream.is_open())
         if (!OpenReportFile())
            return false;
      
      if (initial)
      {
         WriteColumnTitle();
         
         // Compute next output time, first data is time
         mCurrA1Mjd = dat[0];
         mOutputA1Mjd = dat[0] + mIntervalInSec/GmatTimeConstants::SECS_PER_DAY;
         
         // Write first data
         WriteFirstData();
         
         #if DEBUG_EPHEMFILE_FIRST
         MessageInterface::ShowMessage
         ("TextEphemFile::Distribute() first dat=%f %f %f %f %g %g %g\n", dat[0], dat[1],
          dat[2], dat[3], dat[4], dat[5], dat[6]);
         MessageInterface::ShowMessage
            ("   mNumParams=%d, mOutputA1Mjd=%f, mIntervalInSec=%f\n",
             mNumParams, mOutputA1Mjd, mIntervalInSec);
         #endif
      }
      
      if (!dstream.good())
         dstream.clear();

      mCurrA1Mjd = dat[0];
      WriteToBuffer();
      
      while(IsTimeToWrite())
      {
         WriteData();
      }
      
      #if DEBUG_EPHEMFILE_DATA > 1
      MessageInterface::ShowMessage
         ("TextEphemFile::Distribute() dat=%f %f %f %f %g %g %g\n", dat[0], dat[1],
          dat[2], dat[3], dat[4], dat[5], dat[6]);
      #endif
   }
   
   return true;
}


//------------------------------------------------------------------------------
// void WriteColumnTitle()
//------------------------------------------------------------------------------
void TextEphemFile::WriteColumnTitle()
{
   if (writeHeaders)
   {
      if (!dstream.is_open())
         return;

      // write heading for each item
      for (int i=0; i < mNumParams; i++)
      {
          if (!dstream.good())
             dstream.clear();
          
          // set longer width of param names or columnWidth
          Integer width = (Integer)mParamNames[i].length() > columnWidth ?
             mParamNames[i].length() : columnWidth;

          // parameter name has Gregorian, minimum width is 24
          if (mParamNames[i].find("Gregorian") != mParamNames[i].npos)
             if (width < 24)
                width = 24;
          
          dstream.width(width); // sets miminum field width
          
          //dstream.width(columnWidth);
          dstream.fill(' ');
          
          if (leftJustify)
             dstream.setf(std::ios::left);
          
          if (zeroFill)
             dstream.setf(std::ios::showpoint);
          
          dstream << mParamNames[i] << "   ";

          // save time width for writing data
          mColWidth[i] = width;
      }
      
      dstream << std::endl;
   }
   
   initial = false;
}


//------------------------------------------------------------------------------
// void WriteToBuffer()
//------------------------------------------------------------------------------
void TextEphemFile::WriteToBuffer()
{
   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::WriteToBuffer() mNumValidPoints=%d, mBufferSize=%d\n",
       mNumValidPoints, mBufferSize);
   #endif
   
   // buffer up
   mNumValidPoints = (mNumValidPoints < BUFFER_SIZE) ?
      mNumValidPoints + 1 : mNumValidPoints;

   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::WriteToBuffer() mNumValidPoints=%d, BUFFER_SIZE=%d\n",
       mNumValidPoints, BUFFER_SIZE);
   #endif
   
   // shift values to make room for newest value
   for (int i=0; i<BUFFER_SIZE-1; i++)
   {
      mTimeBuffer[i] = mTimeBuffer[i+1];
      mXposBuffer[i] = mXposBuffer[i+1];
      mYposBuffer[i] = mYposBuffer[i+1];
      mZposBuffer[i] = mZposBuffer[i+1];
      mXvelBuffer[i] = mXvelBuffer[i+1];
      mYvelBuffer[i] = mYvelBuffer[i+1];
      mZvelBuffer[i] = mZvelBuffer[i+1];
   }
   
   Real rval = -9999.999;
   
   mTimeBuffer[BUFFER_SIZE - 1] = mCurrA1Mjd;
   
   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::WriteToBuffer() mCurrA1Mjd=%f\n", mCurrA1Mjd);
   #endif
   
   for (int i=1; i < mNumParams; i++)
   {
      // assuming all real parameters
      
      rval = mParams[i]->EvaluateReal();
      
      #if DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("TextEphemFile::WriteToBuffer() i=%d, rval=%f\n", i, rval);
      #endif
      
      if (i == 1)
      {
         mXposBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[0] = rval;
      }
      else if (i == 2)
      {
         mYposBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[1] = rval;
      }
      else if (i == 3)
      {
         mZposBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[2] = rval;
      }
      else if (i == 4)
      {
         mXvelBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[3] = rval;
      }
      else if (i == 5)
      {
         mYvelBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[4] = rval;
      }
      else if (i == 6)
      {
         mZvelBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[5] = rval;
      }
      
   }
}


//------------------------------------------------------------------------------
// bool IsTimeToWrite()
//------------------------------------------------------------------------------
bool TextEphemFile::IsTimeToWrite()
{
   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::IsTimeToWrite() mNumValidPoints=%d, mCurrA1Mjd=%f, "
       "mOutputA1Mjd=%f\n", mNumValidPoints,  mCurrA1Mjd, mOutputA1Mjd);
   #endif

   //if (mOutputA1Mjd > mCurrA1Mjd + 1.0e-6)
   if (mOutputA1Mjd > mCurrA1Mjd)
      return false;
   
   // if output time is between the points and has BUFFER_SIZE points
   if (mNumValidPoints >= mBufferSize)
   {
      Integer startIndex = BUFFER_SIZE - mNumValidPoints;
      //MessageInterface::ShowMessage("==> startIndex=%d\n", startIndex);
      // find mid point
      Integer midIndex = -1;
      for (int i=startIndex; i<BUFFER_SIZE-1; i++)
      {
         #if DEBUG_EPHEMFILE_DATA > 1
         MessageInterface::ShowMessage
            ("==> mTimeBuffer[%d]=%f\n", i, mTimeBuffer[i]);
         #endif
         
         if (mOutputA1Mjd > mTimeBuffer[i] && mOutputA1Mjd <= mTimeBuffer[i+1])
         {
            midIndex = i;
            break;
         }
      }

      if (midIndex == -1)
      {
         #if DEBUG_EPHEMFILE_WARNING
         MessageInterface::ShowMessage
            ("*** Warning: TextEphemFile::IsTimeToWrite() Cannot find "
             "midpoint for time: %f\n   mTimeBuffer[0]=%f, mTimeBuffer[9]=%f\n",
             mOutputA1Mjd, mTimeBuffer[0], mTimeBuffer[9]);
         #endif
         
         return false;
      }
      
      if (midIndex <= startIndex)
         midIndex = startIndex+2;

      if (midIndex > 7)
         midIndex = 7;
      
      #if DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("==> midIndex=%d, mTimeBuffer[%d]=%f\n", midIndex, midIndex, mTimeBuffer[midIndex]);
      #endif
      
      Real vals[6];
      
      mInterpolator->Clear();
      for (int i=midIndex-2; i<=midIndex+2; i++)
      {
         vals[0] = mXposBuffer[i];
         vals[1] = mYposBuffer[i];
         vals[2] = mZposBuffer[i];
         vals[3] = mXvelBuffer[i];
         vals[4] = mYvelBuffer[i];
         vals[5] = mZvelBuffer[i];
         
         mInterpolator->AddPoint(mTimeBuffer[i], vals);
      }
      
      if (mInterpolator->Interpolate(mOutputA1Mjd, mOutputVals))
      {
         return true;
      }
      else
      {
         #if DEBUG_EPHEMFILE_WARNING
         MessageInterface::ShowMessage
            ("*** Warning: TextEphemFile::IsTimeToWrite() Interpolate() returned false\n");
         #endif
      }
   }

   return false;
}


//------------------------------------------------------------------------------
// void WriteTime(Real epoch)
//------------------------------------------------------------------------------
void TextEphemFile::WriteTime(Real epoch)
{
   
   bool handleLeapSecond;
   Real time = TimeSystemConverter::Instance()->Convert(epoch, TimeSystemConverter::A1MJD,
                                          mEpochSysId, GmatTimeConstants::JD_JAN_5_1941,
                                          &handleLeapSecond);

   dstream.width(mColWidth[0]);
   dstream.precision(precision);
   dstream.fill(' ');
         
   if (leftJustify)
      dstream.setf(std::ios::left);
   
   if (zeroFill)
      dstream.setf(std::ios::showpoint);
   
   if (mIsGregorian)
   {
      bool isUTC = ((mEpochSysId == TimeSystemConverter::UTCMJD) ||
                    (mEpochSysId == TimeSystemConverter::UTC));
      std::string timeStr = TimeSystemConverter::Instance()->ConvertMjdToGregorian(time, (isUTC && handleLeapSecond));
      dstream << timeStr << "   ";
   }
   else
   {
      dstream << epoch << "   ";
   }
   
}


//------------------------------------------------------------------------------
// void WriteData()
//------------------------------------------------------------------------------
void TextEphemFile::WriteData()
{
   WriteTime(mOutputA1Mjd);
   
   for (int i=1; i<mNumParams; i++)
   {
      dstream.width(mColWidth[i]);
      dstream.fill(' ');
      
      if (leftJustify)
         dstream.setf(std::ios::left);
      
      if (zeroFill)
         dstream.setf(std::ios::showpoint);
      
      dstream.precision(precision);
      dstream << mOutputVals[i-1] << "   "; 
   }
   
   dstream << std::endl;
   
   // Save ephem file stop time
   mStopA1Mjd = mOutputA1Mjd;
   
   // Compute new output time
   mOutputA1Mjd = mOutputA1Mjd + mIntervalInSec/GmatTimeConstants::SECS_PER_DAY;
   
   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::WriteData() new mOutputA1Mjd=%f, mCurrA1Mjd=%f\n\n",
       mOutputA1Mjd, mCurrA1Mjd);
   #endif
}



//------------------------------------------------------------------------------
// void WriteFirstData()
//------------------------------------------------------------------------------
void TextEphemFile::WriteFirstData()
{
   Real rval = -999.999;

   // Write time in epoch format
   WriteTime(mCurrA1Mjd);
   
   for (int i=1; i<mNumParams; i++)
   {
      rval = mParams[i]->EvaluateReal();
      
      dstream.width(mColWidth[i]);
      dstream.fill(' ');
      
      if (leftJustify)
         dstream.setf(std::ios::left);
      
      if (zeroFill)
         dstream.setf(std::ios::showpoint);
      
      dstream.precision(precision);
      dstream << rval << "   "; 
   }
   
   dstream << std::endl;

   // Save ephem file start time
   mStartA1Mjd = mCurrA1Mjd;
}


//------------------------------------------------------------------------------
// void WriteEphemHeader()
//------------------------------------------------------------------------------
void TextEphemFile::WriteEphemHeader()
{
   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::WriteEphemHeader() mHeaderFileName=%s\n",
       mHeaderFileName.c_str());
   #endif

   // Need to write MATLAB format scripts for Spacecraft and PropSetup
   // but subscriber doesn't know about these objects, so use the Moderator
   Moderator *theModerator = Moderator::Instance();
   theModerator->SaveScript(mHeaderFileName, Gmat::EPHEM_HEADER);

   // Write additional parameters
   std::ofstream headerStream(mHeaderFileName.c_str(), std::ios::app);
   headerStream.precision(precision);
   
   std::string ephemSource = theModerator->GetCurrentPlanetarySource();
   headerStream << "\n\n";
   headerStream << "PlanetaryEphemerisSource = '" << ephemSource << "';\n";
   headerStream << "\n";
   
   headerStream << "Output.EpochType = '" << mEpochFormat << "';\n";
   headerStream << "Output.StartEpoch = " << mStartA1Mjd << ";\n";
   headerStream << "Output.StopEpoch = " << mStopA1Mjd << ";\n";
   headerStream << "Output.IntervalType = 'Second';\n";
   headerStream << "Output.Interval = " << mIntervalInSec << ";\n";
   headerStream << "Output.CoordinateSystem = '" << mCoordSysName << "';\n";
   headerStream << "Output.StateType = 'Cartesian';\n";
   headerStream << "\n";
   
   headerStream << "Time.Unit = 'Day';\n";
   headerStream << "X.Unit = 'Km';\n";
   headerStream << "Y.Unit = 'Km';\n";
   headerStream << "Z.Unit = 'Km';\n";
   headerStream << "VX.Unit = 'Km/Sec';\n";
   headerStream << "VY.Unit = 'Km/Sec';\n";
   headerStream << "VZ.Unit = 'Km/Sec';\n";
   headerStream << "\n";
   headerStream << "\n";

   // Actually I want use cmd processor to append the file,
   // but, I don't know how it will work on Mac and Linux.
   
   // Open data file
   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::WriteEphemHeader() fileName=%s\n", fileName.c_str());
   #endif
   
   std::ifstream dataStream(fileName.c_str());
   if (!dataStream.is_open())
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** TextEphemFile::WriteEphemHeader() Fail to open %s\n",
          fileName.c_str());
      headerStream.close();
      return;
   }
   
   char buffer[MAX_LINE_CHAR];
   
   // Append the data
   while (!dataStream.eof())
   {
      dataStream.getline(buffer, MAX_LINE_CHAR-1);
      headerStream << buffer << "\n";
      //MessageInterface::ShowMessage("buffer=%s\n", buffer);
   }

   dataStream.close();
   headerStream.close();

//    // It doesn't work!!!
//    #ifdef __WINDOWS__
//    // Delete data file
//    if (system(("rm " + fileName).c_str()) == 0)
//       MessageInterface::ShowMessage("==> Sucessfully removed file:%s\n", fileName.c_str());
//    else
//       MessageInterface::ShowMessage("==> Removing file:%s was Unsuccessful\n", fileName.c_str());
//    #endif
}


//------------------------------------------------------------------------------
// void SaveEpochType()
//------------------------------------------------------------------------------
void TextEphemFile::SaveEpochType()
{
   Integer loc = mEpochFormat.find("ModJulian", 0);
   
   if (loc == -1)
      loc = mEpochFormat.find("Gregorian", 0);
   
   if (loc == 0)
      throw SpaceObjectException
         ("TextEphemFile::SaveEpochType() Error parsing time format '" +
          mEpochFormat + "'; could not find 'Gregorian' or 'ModJulian' substring.");
   
   std::string epochSys = mEpochFormat.substr(0, loc);
   mEpochSysId = TimeSystemConverter::Instance()->GetTimeTypeID(epochSys);
   mIsGregorian = false;
   
   if (mEpochFormat.substr(loc) == "Gregorian")
      mIsGregorian = true;

   #if TEXT_EPHEMFILE_FIRST
   MessageInterface::ShowMessage
      ("TextEphemFile::SaveEpochType() epochSys=%S, mIsGregorian=%d, mIntervalInSec=%f\n",
       epochSys.c_str(),  mIsGregorian, mIntervalInSec);
   #endif
}


