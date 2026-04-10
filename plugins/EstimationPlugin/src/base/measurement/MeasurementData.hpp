//$Id: MeasurementData.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         MeasurementData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/07/15
//
/**
 * Definition of the MeasurementData class, used to manage calculated
 * measurements
 */
//------------------------------------------------------------------------------


#ifndef MeasurementData_hpp
#define MeasurementData_hpp

#include "estimation_defs.hpp"
#include "EstimationDefs.hpp"
#include "Covariance.hpp"
#include "TimeSystemConverter.hpp"
#include "GmatTime.hpp"
#include "SpacePoint.hpp"

/**
 *  The measurement data structure
 *
 *  This class acts mostly as a structure which provides the core set of
 *  information used for calculated measurement data.  Each measurement can be
 *  captured in this structure.
 *
 *  This struct presents as a class because it includes code for the assignment
 *  operator and copy constructor.  To me, it's just bad form to call something
 *  a struct and include those pieces.
 */
class ESTIMATION_API MeasurementData
{
public:
   MeasurementData();
   virtual ~MeasurementData();
   void CleanUp();
   MeasurementData(const MeasurementData& md);
   MeasurementData operator=(const MeasurementData& md);


// Explicitly public so that this class acts like a struct
public:
   /// The type of measurement in this record
//   Gmat::MeasurementType   type;
   Integer           type;
   /// String value for type of measurement in this record
   std::string       typeName;
   /// Unique ID for associated model.  This number can change from run to run.
   Integer           uniqueID;
   /// Is measurement periodic, such as 0 to 360, requiring care to compute O - C
   bool              isPeriodic;
   /// For a periodic measurement, the minimum value (for Azimuth, this is 0, for XEast -180)
   Real              minValue;
   /// For a periodic measurement, the value wraps at minValue + period (for Azimuth and XEast this is 360)
   Real              period;
   /// Enumerated ID for the epoch time system
   TimeSystemConverter::TimeSystemTypes
                     epochSystem;
   /// The epoch of the measurement
   GmatEpoch         epoch;
   GmatTime          epochGT;

   /// Who is involved in the measurement.  First one is the "anchor" node
   StringArray       participantIDs;
   /// The id of sensor attached to participant in order to send or receive signal (use for GPS Point Solution)
   StringArray       sensorIDs;
   /// The measured value.  Array to handle more than one value, like AZ_EL
   RealArray         value;
   /// Corrections included in the measured value.  Array to handle more than one value, like AZ_EL
   RealArray         correction;
   /// The range vectors for each signal segment
   std::vector<Rvector3*> rangeVecs;
   
   /// The range rate vector for each signal segment
   std::vector<Rvector3*> rangeRateVecs;
   /// The range rate for each signal segment
   std::vector<Real> rangeRates;

   /// The central body for each transmit signal segment
   std::vector<SpacePoint*> tBodies;
   /// The central body for each receive signal segment
   std::vector<SpacePoint*> rBodies;
   /// The origin of coordinate system for each transmit signal segment
   std::vector<SpacePoint*> tCSOrigins;
   /// The origin of coordinate system for each receive signal segment
   std::vector<SpacePoint*> rCSOrigins;

   /// The transmit time for each signal segment
   std::vector<GmatTime> tPrecTimes;
   /// The receive time for each signal segment
   std::vector<GmatTime> rPrecTimes;
   /// The transmit location for each signal segment
   std::vector<Rvector3*> tLocs;
   /// The receive location for each signal segment
   std::vector<Rvector3*> rLocs;

   // This is use for DSN_TCP measurement only 
   /// The measured value present in travel time without noise and bias
   std::vector<GmatTime>  valueInTime;

   /// Flag indicating if the measurement could be made when it was attempted
   bool              isFeasible;
   
   /// Flag to indicate unfeasible reason
   std::string       unfeasibleReason;				// "N": default value, "BXY": blocked, "R": out of ramp table range, "U": unused, "IRMS", "OLSE"   

   /// Value used for root finding
   Real              feasibilityValue;
   /// Measurement error covariance matrix
   Covariance        *covariance;
   /// Number of events associated with this measurement
   Integer           eventCount;


///// TBD: Do these go here like this?  We may want a more generic container here
   // This section is added for DSN_SeqRange measurement data
   /// Uplink band
   Integer uplinkBand;
   /// Downlink band of the frequency at receiver
   Integer downlinkBand;

   /// Uplink frequency
   Real uplinkFreq;                            // unit: Hz
   /// Uplink frequency at received epoch
   Real uplinkFreqAtRecei;                     // unit: Hz
   /// Range modulo
   Real rangeModulo;

   ///Transmit delay, only used for DSN_PNRange for now
   Real transmitDelay;
   ///Receive delay, only used for DSN_PNRange for now
   Real receiveDelay;


///// TBD: Do these go here like this?  We may want a more generic container here
   // This section is added for DSNTwoWayDoppler measurement data
   // Doppler count interval
   Real dopplerCountInterval;

   // This section is added for TDRS Doppler measurement data
   Real           tdrsNode4Freq;            // the received frequency at the return-link TDRS      (unit: Hz)
   Integer        tdrsNode4Band;            // the received frequency band at the return-link TDRS
   std::string    tdrsServiceID;            // value of serviceID would be "S1", "S2", or "MA"
   Integer        tdrsSMARID;               // TDRS SMAR id
   Integer        tdrsDataFlag;             // TDRS data flag would be 0 or 1


   
   // This section is added for TDRS DOWD measurement data
   Real           dopplerCountIntervalTDRSRef;
   Real           tdrsNode4FreqTDRSRef;     // the received frequency at the return-link reference TDRS      (unit: Hz)
   Integer        tdrsNode4BandTDRSRef;     // the received frequency band at the return-link reference TDRS
   std::string    tdrsServiceIDTDRSRef;     // value of serviceID would be "S1", "S2", or "MA"
   Integer        tdrsSMARIDTDRSRef;        // reference TDRS SMAR id
   Integer        tdrsDataFlagTDRSRef;      // reference TDRS data flag would be 0 or 1

   Integer        uplinkBandTDRSRef;
   Real           uplinkFreqTDRSRef;        // unit: Hz
   Real           uplinkFreqAtReceiTDRSRef; // unit: Hz

   Real           horpHeight;               //The calculated Height of Ray Path
   Real           horpAngle;                //The calculated Central Angle of Ray Path
                                            
   // This part for media correction QA       GMT-5576
   bool isTropoCorrectWarning;
   bool isIonoCorrectWarning;
   Real tropoCorrectRawValue;                 // unit: km
   Real ionoCorrectRawValue;                  // unit: km
   Real tropoCorrectValue;
   Real ionoCorrectValue;
};

#endif /* MeasurementData_hpp */
