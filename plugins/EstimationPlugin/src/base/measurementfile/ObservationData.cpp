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


#include "ObservationData.hpp"
#include "EstimationDefs.hpp"
#include <sstream>

//-----------------------------------------------------------------------------
// ObservationData()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
ObservationData::ObservationData() :
   fileIndex         (NULL),
   inUsed            (true),
   removedReason     ("N"),
   typeName          (""),
   type              (Gmat::UNKNOWN_MEASUREMENT),
   uniqueID          (-1),
   epochSystem       (TimeSystemConverter::A1MJD),
   epoch             (-1.0),
   epochGT           (-1.0),
   noiseCovariance   (NULL),
///// TBD: Determine if there is a more generic way to add these, and if they go here
   unit              ("km"),
   uplinkBand        (0),
   downlinkBand      (0),
//   uplinkFreq        (0.0),
   uplinkFreqAtRecei (0.0),
   rangeModulo       (1.0),
   transmitDelay     (0.0),
   receiveDelay      (0.0),
   dopplerCountInterval   (1.0e-10),
   tdrsServiceID     ("SA1"),
   tdrsNode4Freq     (0.0),
   tdrsNode4Band     (0),
   tdrsSMARID        (0),
   tdrsDataFlag      (0),

   // This section is added for TDRSS DOWD observation data
   dopplerCountIntervalTDRSRef  (1.0e-10),
   tdrsServiceIDTDRSRef         ("SA1"),
   tdrsNode4FreqTDRSRef         (0.0),
   tdrsNode4BandTDRSRef         (0),
   tdrsSMARIDTDRSRef            (0),
   tdrsDataFlagTDRSRef          (0)
{
///// TBD: This does not go here -- we'll be adding a new data type, so need to have a way to handle this
   dataFormat = "GMATInternal";
}


//-----------------------------------------------------------------------------
// ~ObservationData()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
ObservationData::~ObservationData()
{
}


//-----------------------------------------------------------------------------
// ObservationData(const ObservationData& od):
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param od The ObservationData object that sets the data for the new one
 */
//-----------------------------------------------------------------------------
ObservationData::ObservationData(const ObservationData& od):
   fileIndex               (od.fileIndex),
   inUsed                  (od.inUsed),
   removedReason           (od.removedReason),
   typeName                (od.typeName),
   type                    (od.type),
   uniqueID                (od.uniqueID),
   epochSystem             (od.epochSystem),
   epoch                   (od.epoch),
   epochGT                 (od.epochGT),
   epochAtEnd              (od.epochAtEnd),
   epochAtIntegrationEnd   (od.epochAtIntegrationEnd),
   participantIDs          (od.participantIDs),
   sensorIDs               (od.sensorIDs),
   strands                 (od.strands),
   value                   (od.value),
   dataMap                 (od.dataMap),
   value_orig              (od.value_orig),
   unit                    (od.unit),
   noiseCovariance         (od.noiseCovariance),
   extraDataDescriptions   (od.extraDataDescriptions),
   extraTypes              (od.extraTypes),
   extraData               (od.extraData),
//   dataFormat              (od.dataFormat),
   uplinkBand              (od.uplinkBand),
   downlinkBand            (od.downlinkBand),
//   uplinkFreq              (od.uplinkFreq),
   uplinkFreqAtRecei       (od.uplinkFreqAtRecei),
   rangeModulo             (od.rangeModulo),
   transmitDelay           (od.transmitDelay),
   receiveDelay            (od.receiveDelay),
   dopplerCountInterval	   (od.dopplerCountInterval),
   tdrsServiceID           (od.tdrsServiceID),
   tdrsNode4Freq           (od.tdrsNode4Freq),
   tdrsNode4Band           (od.tdrsNode4Band),
   tdrsSMARID              (od.tdrsSMARID),
   tdrsDataFlag            (od.tdrsDataFlag),

   // This section is added for TDRSS DOWD observation data
   dopplerCountIntervalTDRSRef  (od.dopplerCountIntervalTDRSRef),
   tdrsServiceIDTDRSRef         (od.tdrsServiceIDTDRSRef),
   tdrsNode4FreqTDRSRef         (od.tdrsNode4FreqTDRSRef),
   tdrsNode4BandTDRSRef         (od.tdrsNode4BandTDRSRef),
   tdrsSMARIDTDRSRef            (od.tdrsSMARIDTDRSRef),
   tdrsDataFlagTDRSRef          (od.tdrsDataFlagTDRSRef)
{
   dataFormat = od.dataFormat;
}


//-----------------------------------------------------------------------------
// ObservationData& operator=(const ObservationData& od)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param od The ObservationData object that sets the data for the this one
 *
 * @return This object, configured to match od
 */
//-----------------------------------------------------------------------------
ObservationData& ObservationData::operator=(const ObservationData& od)
{
   if (&od != this)
   {
      fileIndex               = od.fileIndex;
      inUsed                  = od.inUsed;
      removedReason           = od.removedReason;
      typeName                = od.typeName;
      type                    = od.type;
      uniqueID                = od.uniqueID;
      epochSystem             = od.epochSystem;
      epoch                   = od.epoch;
      epochGT                 = od.epochGT;
      epochAtEnd              = od.epochAtEnd;
      epochAtIntegrationEnd   = od.epochAtIntegrationEnd;
      participantIDs          = od.participantIDs;
      sensorIDs               = od.sensorIDs;
      strands                 = od.strands;
      value                   = od.value;
      dataMap                 = od.dataMap;
      value_orig              = od.value_orig;
      unit                    = od.unit;
      noiseCovariance         = od.noiseCovariance;
      extraDataDescriptions   = od.extraDataDescriptions;
      extraTypes              = od.extraTypes;
      extraData               = od.extraData;
      dataFormat              = od.dataFormat;
      uplinkBand              = od.uplinkBand;
      downlinkBand            = od.downlinkBand;
//      uplinkFreq              = od.uplinkFreq;
      uplinkFreqAtRecei       = od.uplinkFreqAtRecei;
      rangeModulo             = od.rangeModulo;
      transmitDelay           = od.transmitDelay;
      receiveDelay            = od.receiveDelay;
      dopplerCountInterval    = od.dopplerCountInterval;
      tdrsServiceID           = od.tdrsServiceID;
      tdrsNode4Freq           = od.tdrsNode4Freq;
      tdrsNode4Band           = od.tdrsNode4Band;
      tdrsSMARID              = od.tdrsSMARID;
      tdrsDataFlag            = od.tdrsDataFlag;

      // This section is added for TDRSS DOWD observation data
      dopplerCountIntervalTDRSRef = od.dopplerCountIntervalTDRSRef;
      tdrsServiceIDTDRSRef        = od.tdrsServiceIDTDRSRef;
      tdrsNode4FreqTDRSRef        = od.tdrsNode4FreqTDRSRef;
      tdrsNode4BandTDRSRef        = od.tdrsNode4BandTDRSRef;
      tdrsSMARIDTDRSRef           = od.tdrsSMARIDTDRSRef;
      tdrsDataFlagTDRSRef         = od.tdrsDataFlagTDRSRef;
   }

   return *this;
}


//-----------------------------------------------------------------------------
// void Clear()
//-----------------------------------------------------------------------------
/**
 * Clears all data from this ObservationData object.
 */
//-----------------------------------------------------------------------------
void ObservationData::Clear()
{
   fileIndex               = NULL;
   inUsed                  = true;
   removedReason           = "N";
   typeName                = "";
   type                    = Gmat::UNKNOWN_MEASUREMENT;
   uniqueID                = -1;
   epoch                   = 0.0;
   epochGT                 = 0.0;
   participantIDs.clear();
   sensorIDs.clear();
   value.clear();
   dataMap.clear();
   strands.clear();
   value_orig.clear();
   extraDataDescriptions.clear();
   extraTypes.clear();
   extraData.clear();
///// TBD: Determine if there is a more generic way to add these
   uplinkBand              = 0;
   downlinkBand            = 0;
//   uplinkFreq              = 0.0;
   uplinkFreqAtRecei       = 0.0;
   rangeModulo             = 1.0;
   transmitDelay           = 0.0;
   receiveDelay            = 0.0;
   dopplerCountInterval    = 1.0e-10;
   tdrsServiceID           = "SA1";
   tdrsNode4Freq           = 0.0;
   tdrsNode4Band           = 0;
   tdrsSMARID              = 0;
   tdrsDataFlag            = 0;

   dopplerCountIntervalTDRSRef = 1.0e-10;
   tdrsServiceIDTDRSRef = "SA1";
   tdrsNode4FreqTDRSRef = 0.0;
   tdrsNode4BandTDRSRef = 0;
   tdrsSMARIDTDRSRef = 0;
   tdrsDataFlagTDRSRef = 0;
}



std::string ObservationData::GetTrackingConfig()
{
   std::stringstream ss;
   ss << "{{";
   for (UnsignedInt i = 0; i < participantIDs.size(); ++i)
   {
      ss << participantIDs[i];
      if (sensorIDs[i] != "")
      {
         ss << "." << sensorIDs[i];
      }

      if (i < participantIDs.size()-1)
         ss << ",";
   }
   ss << "}" << typeName << "}";

   return ss.str();
}


StringArray ObservationData::GetAvailableMeasurementTypes()
{
   StringArray typeList;

   // New syntax's measurement types
   typeList.push_back("DSN_SeqRange");
   typeList.push_back("DSN_PNRange");
   typeList.push_back("DSN_TCP");
   typeList.push_back("GPS_PosVec");
   typeList.push_back("Range");
   typeList.push_back("Range_Skin");
   typeList.push_back("RangeRate");
   typeList.push_back("BRTS_Range");
   typeList.push_back("BRTS_Doppler");
   typeList.push_back("Azimuth");
   typeList.push_back("Elevation");
   typeList.push_back("XEast");
   typeList.push_back("YNorth");
   typeList.push_back("XSouth");
   typeList.push_back("YEast");
   typeList.push_back("SN_Range");
   typeList.push_back("SN_Doppler");
   typeList.push_back("SN_Doppler_Rtn");
   typeList.push_back("SN_DOWD");

   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();
   if (runmode == GmatGlobal::TESTING)
   {
      // new syntax's untested measurement types   
      typeList.push_back("RightAscension");
      typeList.push_back("Declination");
   }

   return typeList;
}


bool ObservationData::IsValidMeasurementType(const std::string typeName)
{
   StringArray typeList = GetAvailableMeasurementTypes();
   bool isValid = false;
   for (Integer i = 0; i < typeList.size(); ++i)
   {
      if (typeList[i] == typeName)
      {
         isValid = true;
         break;
      }
   }
   return isValid;
}
