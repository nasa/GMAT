//------------------------------------------------------------------------------
//                         ObservationData
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
// Created: 2009/08/06
//
/**
 * Class that contains observation data
 *
 * This class is essentially a struct designed to contain the observation data
 * retrieved from a ObType object.
 */
//------------------------------------------------------------------------------


#ifndef ObservationData_hpp
#define ObservationData_hpp

#include "estimation_defs.hpp"
#include "EstimationDefs.hpp"
///// TBD: Determine if there is a more generic way to add these
#include "GmatData.hpp"
#include "Covariance.hpp"
#include "TimeSystemConverter.hpp"

class DataFile;


/**
 * Class used to set and retrieve observation data
 */
class ESTIMATION_API ObservationData : public GmatData
{
public:
   ObservationData();
   virtual ~ObservationData();
   ObservationData(const ObservationData& od);
   ObservationData&  operator=(const ObservationData& od);

   virtual void      Clear();
   
   std::string       GetTrackingConfig();

   StringArray       GetAvailableMeasurementTypes();
   bool              IsValidMeasurementType(const std::string typeName);

// Explicitly public so that this class acts like a struct
public:
   /// File index: contain a pointer to DataFile. This field is needed for data filter based on data file
   DataFile*         fileIndex;

   /// Flag to specify this data record is in use or not
   bool              inUsed;

   /// Flag to specify reason the record not been used. 
   /// "N": Normal, "U": unused, "IRMS": OLSEInitialRMSSigma, 
   /// "OLSE": outer-loop sigma filter, "BXY": Blocked, "R": out of ramp table range
   std::string       removedReason;

   /// The text name of the data type, if available
   std::string       typeName;
   /// The type of measurement in this record
   Integer           type;
   /// Unique ID for associated data stream.
   Integer           uniqueID;
   /// Enumerated ID for the epoch time system
   TimeSystemConverter::TimeSystemTypes
                     epochSystem;
   /// The epoch of the measurement
   GmatEpoch         epoch;
   GmatTime          epochGT;

   /// Flag indicating if epoch is at start or end of signal
   bool              epochAtEnd;
   /// Flag indicating if epoch is at start or end of integration
   bool              epochAtIntegrationEnd;
   
   /// Who is involved in the measurement.  First one is the "anchor" node
   StringArray       participantIDs;
   /// The id of sensor attached to participant in order to send or receive signal (use for GPS Point Solution)
   StringArray       sensorIDs;
   /// The signal strands for the measurement
   std::vector<StringArray> strands;
   /// The observed value with correction.  Array to handle more than one value, like AZ_EL
   RealArray         value;
   /// Associated data element names for the data in the value array
   StringArray       dataMap;

   /// The original observed value
   RealArray         value_orig;

   /// Measurement unit
   std::string		   unit;
   /// The associated noise covariance matrix
   Covariance*       noiseCovariance;


   /// Strings describing any ancillary data in the observation source
   StringArray       extraDataDescriptions;
   /// Types for any ancillary data in the observation source
   IntegerArray      extraTypes;
   /// Ancillary data from the observation source, in string format
   StringArray       extraData;

///// TBD: Determine if there is a more generic way to add these
//   std::string dataFormat;			// Flag indicating which data format is used. Its value is either "GMATInternal" or "GMAT_OD"
   /// Uplink band of the frequency at receiver
   Integer           uplinkBand;
   /// Downlink band of the frequency at receiver
   Integer           downlinkBand;
   ///// Uplink frequency at transmit epoch                                     // This variable is nolonger in use 
   //Real              uplinkFreq;                        // unit: Hz           // This variable is nolonger in use
   /// Uplink frequency at receive epoch. For ramped frequency model, this frequency is different from uplinkFreq
   Real              uplinkFreqAtRecei;                 // unit: Hz
   /// Range modulo
   Real              rangeModulo;

   ///Transmit delay, only used for DSN_PNRange for now
   Real transmitDelay;
   ///Receive delay, only used for DSN_PNRange for now
   Real receiveDelay;
   // This section is added for Gmat_ODDoppler observation data
   /// Time difference between the reception time at station for start path and end path  
   Real              dopplerCountInterval;

   // This section is added for TDRSS Doppler observation data
   /// TRDS service Id
   std::string       tdrsServiceID;   // "SA1", "SA2", or "MA"
   /// TDRS node 4 frequency
   Real              tdrsNode4Freq;   // the received frequency at the return-link TDRS  (unit: Hz)
   /// TDRS node 4 frequency band
   Integer           tdrsNode4Band;   // the received frequency band at the return-link TDRS. 0: unspecified, 1: S-band, 2: X-band, 3: K-band 
   /// TDRS SMAR Id
   Integer           tdrsSMARID;      // SMAR id
   /// TDRS generation
   Integer           tdrsDataFlag;    // TDRS data flag: 0 or 1


   // This section is added for TDRSS DOWD observation data
   /// Time difference between the reception time at station for start path and end path for reference TDRS oneway doppler 
   Real              dopplerCountIntervalTDRSRef;
   /// TRDS service Id for reference TDRS
   std::string       tdrsServiceIDTDRSRef;   // "SA1", "SA2", or "MA"
   /// TDRS node 4 frequency for reference TDRS
   Real              tdrsNode4FreqTDRSRef;   // the received frequency at the return-link TDRS  (unit: Hz)
   /// TDRS node 4 frequency band for reference TDRS
   Integer           tdrsNode4BandTDRSRef;   // the received frequency band at the return-link TDRS. 0: unspecified, 1: S-band, 2: X-band, 3: K-band 
   /// TDRS SMAR Id for reference TDRS
   Integer           tdrsSMARIDTDRSRef;      // SMAR id
   /// TDRS generation for reference TDRS
   Integer           tdrsDataFlagTDRSRef;    // TDRS data flag: 0 or 1
};

#endif /* ObservationData_hpp */
