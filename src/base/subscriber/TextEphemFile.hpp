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
 * Writes the specified parameter value to a ephemeris file.
 */
//------------------------------------------------------------------------------
#ifndef TextEphemFile_hpp
#define TextEphemFile_hpp

#include "ReportFile.hpp"
#include "Interpolator.hpp"
#include <fstream>


class GMAT_API TextEphemFile : public ReportFile
{
public:
   TextEphemFile(const std::string &type, const std::string &name,
                 const std::string &fname = "", 
                 Parameter *firstVarParam = NULL);
   
   virtual ~TextEphemFile(void);
   
   TextEphemFile(const TextEphemFile &);
   TextEphemFile& operator=(const TextEphemFile&);
   
   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

   // parameters
   virtual Integer GetParameterID(const std::string &str) const;
   virtual std::string GetParameterText(const Integer id) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value);
   
protected:
   
   virtual bool Distribute(const Real * dat, Integer len);
   
   enum
   {
      EPOCH_FORMAT = ReportFileParamCount,
      INTERVAL,
      COORD_SYSTEM,
      TextEphemFileParamCount  ///< Count of the parameters for this class
   };
   
   static const std::string
      PARAMETER_TEXT[TextEphemFileParamCount - ReportFileParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[TextEphemFileParamCount - ReportFileParamCount];
   
private:

   static const Integer MAX_LINE_CHAR = 1024;
   static const Integer BUFFER_SIZE = 10;
   
   Interpolator *mInterpolator;

   std::string mHeaderFileName;
   std::string mEpochFormat;
   std::string mCoordSysName;
   Real mIntervalInSec;
   Real mCurrA1Mjd;
   Real mOutputA1Mjd;
   Real mStartA1Mjd;
   Real mStopA1Mjd;

   // Assuming using CubicSplineInterpolator, it needs 5 points
   Real mTimeBuffer[BUFFER_SIZE];
   Real mXposBuffer[BUFFER_SIZE];
   Real mYposBuffer[BUFFER_SIZE];
   Real mZposBuffer[BUFFER_SIZE];
   Real mXvelBuffer[BUFFER_SIZE];
   Real mYvelBuffer[BUFFER_SIZE];
   Real mZvelBuffer[BUFFER_SIZE];
   Real mOutputVals[6]; // X, Y, Z, Vx, Vy, Vz for now
   Integer mNumValidPoints;
   Integer mBufferSize;
   Integer mColWidth[7];
   Integer mEpochSysId;
   bool mIsGregorian;
   
   void WriteColumnTitle();
   void WriteToBuffer();
   bool IsTimeToWrite();
   void WriteTime(Real epoch);
   void WriteData();
   void WriteFirstData();
   void WriteEphemHeader();
   void SaveEpochType();
   
};


#endif
