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
// Author: Tuan D. Nguyen / NASA
// Created: 2019.10.11
//
/**
 * Reads a NPlate history file, and stores the data.
 * @todo Add assumptions and general file requirements here
 */
//------------------------------------------------------------------------------
#ifndef NPlateHistoryFileReader_hpp
#define NPlateHistoryFileReader_hpp

#include "Rvector3.hpp"
#include "utildefs.hpp"

class GMATUTIL_API NPlateHistoryFileReader
{
public:
   /// class methods
   NPlateHistoryFileReader();
   NPlateHistoryFileReader(const NPlateHistoryFileReader &copy);
   NPlateHistoryFileReader& operator=(const NPlateHistoryFileReader &copy);

   virtual ~NPlateHistoryFileReader();

   virtual NPlateHistoryFileReader* Clone() const;

   /// Initialize will parse the file, and validate the data
   virtual void            Initialize();

   /// Set the NPlate history file name
   virtual bool            SetFile(const std::string &theNPlateHistoryFile);

   /// Get face normal unit vector
   virtual Rvector3        GetFaceNormal(GmatTime t);
   
   /// Set the interpolator
   virtual bool            SetInterpolator(const std::string &theInterpolator);

   /// Get name of coordinate system in which face normal vector is presented
   std::string             GetCoordinateSystemName() {return csName; };

   /// Get start time
   GmatTime                GetStartTime() {return startTime; };

   /// Get interpolator method
   std::string             GetInterpolatorMoethod() { return interpolator; };

protected:
   struct NPlateDataRecord
   {
      Real     timeOffset;
      Real     elevation;
      Real     azimuth;
      Rvector3 r;

      // add to this as necessary

      NPlateDataRecord(Real t, Real az, Real el, Real x, Real y, Real z)
      {
         azimuth    = az;
         elevation  = el;
         timeOffset = t;
         r[0] = x; r[1] = y; r[2] = z;
      }
      NPlateDataRecord(const NPlateDataRecord &copy)
      {
         azimuth     = copy.azimuth;
         elevation   = copy.elevation;
         timeOffset  = copy.timeOffset;
         r[0] = copy.r[0];
         r[1] = copy.r[1];
         r[2] = copy.r[2];
      }
   };


   /// Starting time in history data file
   GmatTime startTime;

   /// Coordinate system
   std::string csName;

   ///// Name of interpolation method
   //std::string interpolateMethod;

   // The file name
   std::string nplateFile;
   /// Interpolation method
   std::string interpolator;
   /// Has the file been read and the data stored and validated?
   bool        isInitialized;
   // Store a vector of record pointers
   std::vector<NPlateDataRecord*>   nplateData;

   /// Create a new record and add it to the data store
   virtual bool     AddDataRecord(Real timeOffset, Real az, Real el, Real x, Real y, Real z);
   /// Parse the file, validate, and store the data
   virtual bool     ParseFile();

private:
   Real             AzimuthInterpolation(Real angle1, Real angle2, Real factor);
   Real             ElevationInterpolation(Real angle1, Real angle2, Real factor);
};

#endif // NPlateHistoryFileReader_hpp
