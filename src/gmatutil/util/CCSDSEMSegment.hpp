//------------------------------------------------------------------------------
//                                  CCSDSEMSegment
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
// Created: 2013.11.19
//
/**
 * Stores, validates, and manages meta data and real data read from, or
 * to be written to, a CCSDS Ephemeris Message file.
 * This is the abstract base class from which other classes must derive (e.g.
 * a class to handle attitude segments).
 * Currently, the common meta data fields handled in this base class are:
 *    COMMENT *
 *    OBJECT_NAME *
 *    OBJECT_ID *
 *    CENTER_NAME **
 *    TIME_SYSTEM
 *    START_TIME
 *    USEABLE_START_TIME
 *    USEABLE_STOP_TIME
 *    STOP_TIME
 *    INTERPOLATION_DEGREE
 *       * GMAT saves but ignores these
 *       ** GMAT saves this but validates in child classes
 *
 * In addition, the data fields handled here are:
 *    COMMENT
 *
 *    All others needed must be handled in the appropriate child class.
 *
 */
//------------------------------------------------------------------------------
#ifndef CCSDSEMSegment_hpp
#define CCSDSEMSegment_hpp

#include "utildefs.hpp"
#include "Rvector.hpp"

/// struct hold a covariance record
struct CovarianceRecord
{
   Real epoch;           // epoch at which covariance is recorded
   // Note that: the time recorded for covariance may be different
   // from the time recorded for spacecraft' state

   std::string refFrame; // reference frame in which covariance is specified
   std::string comment;  // comment about the record

   // lower-left covariance matrix (contain 21 elements)
   Rvector covar;        // lower-left covariance matrix specified at a given
                         // epoch in a given reference frame

   void SetValue(const Real ep, const std::string frame, 
      const std::string commt, const Rvector cov)
   {
      epoch = ep;
      refFrame = frame;
      comment = commt;
      covar = cov;
   };
};


class GMATUTIL_API CCSDSEMSegment
{
public:
   // static methods
   /// Parse a time string read from the EM file and convert it to
   /// a Real (A1Mjd) epoch
   static Real ParseEpoch(const std::string &epochString);
   
   /// class methods
   CCSDSEMSegment(const std::string version, Integer segNum);
   CCSDSEMSegment(const CCSDSEMSegment &copy);
   CCSDSEMSegment& operator=(const CCSDSEMSegment &copy);
   
   virtual ~CCSDSEMSegment();

   virtual CCSDSEMSegment* Clone() const = 0;

   virtual bool         Validate(bool checkData = true);

   // A GetState method must be added to child classes, returning the
   // requested state data in the required format/representation

   /// Sets a meta data field - assumes fields are all Caps
   virtual bool         SetMetaData(const std::string &fieldName,
                                    const std::string &value);

   virtual std::string  GetStringMetaData(const std::string &fieldName);
   virtual Real         GetRealMetaData(const std::string &fieldName);
   virtual Integer      GetIntegerMetaData(const std::string &fieldName);

   virtual bool         SetMetaDataForWriting(const std::string &fieldName,
                                              const std::string &value);
   
   virtual bool         AddData(Real epoch, Rvector data,
                                bool justCheckDataSize = false);
   
   virtual bool         AddMetaComment(const std::string& comment);
   virtual bool         AddDataComment(const std::string& comment);
   virtual void         ClearMetaComments();
   virtual void         ClearDataComments();
   virtual void         ClearMetaData();
   virtual void         ClearDataStore();
   
   virtual Integer      GetDataSize() const;
   virtual void         SetDataSize(const Integer size);

   virtual bool         CoversEpoch(Real theEpoch);
   virtual Real         GetStartTime() const;
   virtual Real         GetStopTime() const;
   
   virtual std::string  GetMetaDataForWriting();
   virtual std::string  GetMetaComments();
   virtual std::string  GetDataComments();
   virtual Integer      GetNumberOfDataPoints();
   virtual bool         GetEpochAndData(Integer index, Real &epoch,
                                        Rvector &data);

   virtual std::string  GetFormatVersion(){ return formatVersion;};
   virtual void         SetFormatVersion(const std::string ver) { formatVersion = ver; };

   virtual std::string  GetRefFrame() { return refFrame; };

   // Move this struct to outside this class due to struc cannot nested
   ///// struct hold a covariance record
   //struct CovarianceRecord
   //{
   //   Real epoch;           // epoch at which covariance is recorded
   //   // Note that: the time recorded for covariance may be different
   //   // from the time recorded for spacecraft' state

   //   std::string refFrame; // reference frame in which covariance is specified
   //   std::string comment;  // comment about the record

   //   // lower-left covariance matrix (contain 21 elements)
   //   Rvector covar;        // lower-left covariance matrix specified at a given
   //                         // epoch in a given reference frame

   //   void SetValue(const Real ep, const std::string frame, 
   //      const std::string commt, const Rvector cov)
   //   {
   //      epoch = ep;
   //      refFrame = frame;
   //      comment = commt;
   //      covar = cov;
   //   };
   //};

   virtual CovarianceRecord* GetCovarianceRecord(Integer recNum);
   virtual Integer           GetCovarianceRecordNumber() { return covStore.size(); };
   virtual void              ClearAllCovarianceRecords();
   virtual void              AddCovarianceRecord(CovarianceRecord *rec) { covStore.push_back(rec); };

protected:
   
   // struct to hold epochs and data
   struct EpochAndData
   {
      Real    epoch;
      // data vector will be allocated/set in the child classes according to
      // the size needed, e.g. an attitude quaternion segment would need
      // a Rvector of size 4 and OEM cartesian state would need a Rvector of size 6
      Rvector data;
   };
   
   /// Store the data in a vector
   std::vector<EpochAndData*> dataStore;
   
   /// Store the covariance records in a vector
   std::vector<CovarianceRecord*> covStore;

   /// The format version
   std::string formatVersion;

   /// The number of the segment (its position in the file)
   Integer     segmentNumber;
   /// Actual size of the data needed
   Integer     dataSize;
   /// What is the type of data for this segment?  (set in child classes)
   std::string dataType;
   /// Standard part of an error message for the segment
   std::string segError;

   // Required meta data fields
   std::string timeSystem;
   std::string startTimeStr;
   std::string stopTimeStr;
   Real        startTime;
   Real        stopTime;

   /// Optional meta data fields
   std::string usableStartTimeStr;
   std::string usableStopTimeStr;
   Real        usableStartTime;
   Real        usableStopTime;
   // The keyword associated with this item is different depending on the
   // type of Ephmeris Message file; therefore, it is handled in the
   // appropriate child class
   std::string interpolationMethod;
   std::string interpolationDegreeStr;
   Integer     interpolationDegree;
   
   /// Required meta data fields
   std::string objectName;
   std::string objectID;
   std::string centerName;
   std::string refFrame;
   
   /// Optional meta data fields
   StringArray metaComments;
   
   /// Optional data comments after META_STOP and before data block starts
   StringArray dataComments;
   
   // other data
   /// Does the segment contain usable start and stop times?
   bool        usesUsableTimes;

   bool        checkLagrangeOrder;

   Integer     firstUsable;
   Integer     lastUsable;

   // static data

   /// Tolerance to use when looking for an exact epoch match
   static const Real        EPOCH_MATCH_TOLERANCE;
   /// Initial value of a string field that GMAT does not currently use
   static const std::string UNUSED_STRING;
   /// Initial value of a string field that GMAT does use indicating
   /// that it has not yet been set
   static const std::string UNSET_STRING;
   /// Initial value of a Real field that GMAT does use indicating
   /// that it has not yet been set
   static const Real        UNSET_REAL;
   /// Initial value of an Integer field that GMAT does use indicating
   /// that it has not yet been set
   static const Integer     UNSET_INTEGER;

   // Look for an exact epoch match
   virtual Rvector      DetermineState(Real atEpoch);
   virtual bool         GetUsableIndexRange(Integer &first, Integer &last);
   /// Interpolate the data if necessary
   virtual Rvector      Interpolate(Real atEpoch) = 0;
   virtual Rvector      InterpolateLagrange(Real atEpoch);
   virtual Rvector      InterpolateSLERP(Real atEpoch);
};

#endif // CCSDSEMSegment_hpp
