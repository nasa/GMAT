//------------------------------------------------------------------------------
//                         GmatObType
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
// Created: 2009/07/06
//
/**
 * ObType class used for the GMAT Internal observation data
 */
//------------------------------------------------------------------------------


#include "GmatObType.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "FileManager.hpp"
#include "MeasurementException.hpp"
#include "StringUtil.hpp"
#include <sstream>


//#define DEBUG_OBTYPE_CREATION_INITIALIZATION
//#define DEBUG_FILE_WRITE
//#define DEBUG_FILE_READ
//#define DEBUG_FILE_ACCESS
//#define DEBUG_SIGNAL_READ


#define  USE_OLD_GMDFILE_FORMAT

//-----------------------------------------------------------------------------
// GmatObType(const std::string withName)
//-----------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param withName The name of the new object
 */
//-----------------------------------------------------------------------------
GmatObType::GmatObType(const std::string withName) :
   ObType         ("GMATInternal", withName),
   epochPrecision (16),
   dataPrecision  (6)
{
   #ifdef DEBUG_OBTYPE_CREATION_INITIALIZATION
	  MessageInterface::ShowMessage("Creating a GMATInternal obtype  <%s,%p>\n", GetName().c_str(), this);
   #endif

   header = "% GMAT Internal Measurement Data File\n\n";
   warn2WayDepricated = true;
   //// Define deprecated type map
   //depTypeMap["DSNRange"] = "DSN_SeqRange";
   //depTypeMap["Doppler"]  = "DSN_TCP";
}

//-----------------------------------------------------------------------------
// ~GmatObType()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
GmatObType::~GmatObType()
{
}


//-----------------------------------------------------------------------------
// GmatObType(const GmatObType& ot) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ot The GmatObType that gets copied to this one
 */
//-----------------------------------------------------------------------------
GmatObType::GmatObType(const GmatObType& ot) :
   ObType         (ot),
   epochPrecision (ot.epochPrecision),
   dataPrecision  (ot.dataPrecision)
{
   #ifdef DEBUG_OBTYPE_CREATION_INITIALIZATION
	  MessageInterface::ShowMessage("GmatObType copy constructor (a GMATInternal obtype) from <%s,%p> to <%s,%p>\n", ot.GetName().c_str(), &ot, GetName().c_str(), this);
   #endif
}


//-----------------------------------------------------------------------------
// GmatObType& operator=(const GmatObType& ot)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ot The GmatObType that gets copied to this one
 *
 * @return This GmatObType, configured to match ot
 */
//-----------------------------------------------------------------------------
GmatObType& GmatObType::operator=(const GmatObType& ot)
{
   #ifdef DEBUG_OBTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("GmatObType operator = (assigning one GMATInternal obtype to another)\n");
   #endif

   if (this != &ot)
   {
      epochPrecision = ot.epochPrecision;
      dataPrecision  = ot.dataPrecision;
   }

   return *this;
}


//-----------------------------------------------------------------------------
// GmatBase* Clone() const
//-----------------------------------------------------------------------------
/**
 * Cloning method used to create a GmatObType from a GmatBase pointer
 *
 * @return A new GmatObType object matching this one
 */
//-----------------------------------------------------------------------------
GmatBase* GmatObType::Clone() const
{
   #ifdef DEBUG_OBTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Cloning a GMATInternal obtype\n");
   #endif

   return new GmatObType(*this);
}


//-----------------------------------------------------------------------------
// bool Initialize()
//-----------------------------------------------------------------------------
/**
 * Prepares this GmatObType for use
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool GmatObType::Initialize()
{
#ifdef DEBUG_OBTYPE_CREATION_INITIALIZATION
   MessageInterface::ShowMessage("GmatObType::Initialize() Executing\n");
#endif

   ObType::Initialize();

   return true;
}


//-----------------------------------------------------------------------------
// bool Open(bool forRead, bool forWrite, bool append)
//-----------------------------------------------------------------------------
/**
 * Opens a GmatObType stream for processing
 *
 * The method manages GmatObType path and file extension defaults in addition to
 * performing the basic open operations.
 *
 * @param forRead True to open for reading, false otherwise
 * @param forWrite True to open for writing, false otherwise
 * @param append True if data being written should be appended, false if not
 *
 * @return true if the the stream was opened, false if not
 */
//-----------------------------------------------------------------------------
bool GmatObType::Open(bool forRead, bool forWrite, bool append)
{
   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("GmatObType::Open(%s, %s, %s) Executing for \n",
            (forRead ? "true" : "false"),
            (forWrite ? "true" : "false"),
            (append ? "true" : "false"), this);
   #endif
   
   // when theStream open twice the content in theStream is always empty string. Therefore, it needs this verification.
   if (theStream.is_open())
      return true;

   bool retval = false;

   // temporary
   retval = true;
   std::ios_base::openmode mode = std::fstream::in;

   if (forRead && forWrite)
      mode = std::fstream::in | std::fstream::out;

   else
   {
      if (forRead)
         mode = std::fstream::in;
      if (forWrite)
         mode = std::fstream::out;
   }

   if (append)
      mode = mode | std::fstream::app;

   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("   Opening the stream %s, mode = %d\n",
            streamName.c_str(), mode);
   #endif

   if (streamName != "")
   {
      std::string fullPath = "";

      // If no path designation slash character is found, add the default path
      if ((streamName.find('/') == std::string::npos) &&
          (streamName.find('\\') == std::string::npos))
      {
         FileManager *fm = FileManager::Instance();
         fullPath = fm->GetPathname(FileManager::MEASUREMENT_PATH);
      }
      fullPath += streamName;

      // Add the .gmd extension if there is no extension in the file
      size_t dotLoc = fullPath.find_last_of('.');                    // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
      size_t slashLoc = fullPath.find_last_of('/');                  // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
      if (slashLoc == std::string::npos)
         slashLoc = fullPath.find_last_of('\\');

      if ((dotLoc == std::string::npos) ||
          (dotLoc < slashLoc))
      {
         fullPath += ".gmd";
      }

      #ifdef DEBUG_FILE_ACCESS
         MessageInterface::ShowMessage("   Full path is %s, mode = %d\n",
               fullPath.c_str(), mode);
      #endif

	  theStream.open(fullPath.c_str(), mode);
   }

   retval = theStream.is_open();
   if (retval && forWrite)
      theStream << header;

   if (retval == false)
   {
	  throw MeasurementException("GMATInternal Data File " + streamName + " could not be opened\n");
   }
   return retval;
}

//-----------------------------------------------------------------------------
// bool IsOpen()
//-----------------------------------------------------------------------------
/**
 * Tests to see if the GmatObType data file has been opened
 *
 * @return true if the file is open, false if not.
 */
//-----------------------------------------------------------------------------
bool GmatObType::IsOpen()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatObType::IsOpen() Executing\n");
   #endif
   return theStream.is_open();
}


//-----------------------------------------------------------------------------
// bool AddMeasurement(MeasurementData *md)
//-----------------------------------------------------------------------------
/**
 * Adds a new measurement to the GmatObType data file
 *
 * This method takes the raw observation data passed in and formats it into a
 * string compatible with GmatInternal data files, and then writes that string
 * to the open data stream.
 *
 * @param md The measurement data containing the observation.
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool GmatObType::AddMeasurement(MeasurementData *md)
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatObType<%p>::AddMeasurement() Executing\n", this);
   #endif
   bool retval = false;

   std::stringstream dataLine;
   char databuffer[2000];

   if (md->epochGT.GetMjd() <= 0.0)
   {
      Real taiEpoch = (md->epochSystem == TimeSystemConverter::TAIMJD ? md->epoch :
         theTimeConverter->ConvertToTaiMjd(md->epochSystem, md->epoch,
         GmatTimeConstants::JD_NOV_17_1858));

      char epochbuffer[200];
      sprintf(epochbuffer, "%18.12lf", taiEpoch);
      dataLine << epochbuffer;
   }
   else
   {
      GmatTime taiEpoch = (md->epochSystem == TimeSystemConverter::TAIMJD ? md->epochGT :
         theTimeConverter->ConvertToTaiMjd(md->epochSystem, md->epochGT,
         GmatTimeConstants::JD_NOV_17_1858));
      
      dataLine << taiEpoch.ToString();
   }
   
   dataLine << "    " << md->typeName
            << "    " << md->type << "    ";
#ifdef USE_OLD_GMDFILE_FORMAT
   if (md->type < 9000)
   {
      for (UnsignedInt j = 0; j < md->participantIDs.size(); ++j)
         dataLine << md->participantIDs[j] << "    ";
   }
   else
   {
      if (md->participantIDs.size() == 0)
         throw GmatBaseException("Error: No participant is set to measurement data with type " + md->typeName + ".\n");
      else if (md->participantIDs.size() == 1)
      {
         // for one participant (GPS Point Solution)
         dataLine << md->sensorIDs[0] << "    ";
      }

      else if (md->typeName != "DSN_TCP" && md->typeName != "DSN_PNRange" && md->typeName != "RangeRate" && (md->participantIDs[0] == md->participantIDs[md->participantIDs.size()-1])&&(md->participantIDs.size() == 3))
      {
         //Do not do this for DSN_TCP or RangeRate, probably should normalize this and remove this section for all data types...
         // if the first participant and the last participant are the same, write in a list w/o brackets containing all but the last one 
         for (UnsignedInt j = 0; j < md->participantIDs.size()-1; ++j)
            dataLine << md->participantIDs[j] << "    ";
      }
      else
      {
         // otherwise write all participants in signal path as a list inside brackets 
         dataLine << "{ ";
         for (UnsignedInt j = 0; j < md->participantIDs.size(); ++j)
         {
            dataLine << md->participantIDs[j];
            if (j < md->participantIDs.size()-1)
               dataLine << "    ";
            else
               dataLine << " }    ";
         }
      }
   }
   
#else
   for (UnsignedInt j = 0; j < md->participantIDs.size(); ++j)
      dataLine << md->participantIDs[j] << "    ";
#endif

   if ((md->typeName == "DSN_TCP")||(md->typeName == "RangeRate"))
   {
      dataLine << md->uplinkBand << "    ";
      dataLine << md->dopplerCountInterval << "    ";
   }
   else if ((md->typeName == "SN_Doppler")|| 
            (md->typeName == "SN_Doppler_Rtn") ||
            (md->typeName == "BRTS_Doppler"))
   {
      sprintf(databuffer, "    %.15le    %d    %s    %d   %d   %f   ",
         md->tdrsNode4Freq, md->tdrsNode4Band, md->tdrsServiceID.c_str(), md->tdrsDataFlag, md->tdrsSMARID, md->dopplerCountInterval);
      dataLine << databuffer;
   }
   else if (md->typeName == "SN_DOWD")
   {
      sprintf(databuffer, "    %.15le    %d    %s    %d   %d   %f   ",
         md->tdrsNode4Freq, md->tdrsNode4Band, md->tdrsServiceID.c_str(), md->tdrsDataFlag, md->tdrsSMARID, md->dopplerCountInterval);
      dataLine << databuffer;
      
      sprintf(databuffer, "%.15le    %d    %s    %d   %d   %f   ",
         md->tdrsNode4FreqTDRSRef, md->tdrsNode4BandTDRSRef, md->tdrsServiceIDTDRSRef.c_str(), md->tdrsDataFlagTDRSRef, md->tdrsSMARIDTDRSRef, md->dopplerCountIntervalTDRSRef);
      dataLine << databuffer;  
   }


   for (UnsignedInt k = 0; k < md->value.size(); ++k)
   {
      if (md->typeName == "DSN_SeqRange" || md->typeName == "DSN_PNRange")
         sprintf(databuffer, "%.16le",GmatMathUtil::Mod(md->value[k],md->rangeModulo));
      else
         sprintf(databuffer, "%.16le", md->value[k]);
      dataLine << databuffer;
      if (k < md->value.size()-1)
         dataLine << "    ";
   }

   // extended information:
   if (md->typeName == "DSN_SeqRange")
   {
      sprintf(databuffer, "    %d    %.15le    %.15le",
//         md->uplinkBand, md->uplinkFreq, md->rangeModulo);
         md->uplinkBand, md->uplinkFreqAtRecei, md->rangeModulo);
      dataLine << databuffer;
   }
   else if (md->typeName == "DSN_PNRange")
   {
       sprintf(databuffer, "    %.15le    %.15le    %d    %d    %.15le    %.15le",
           //         md->uplinkBand, md->uplinkFreq, md->rangeModulo);
           md->transmitDelay, md->receiveDelay, md->uplinkBand, md->downlinkBand, md->uplinkFreqAtRecei, md->rangeModulo);
       dataLine << databuffer;
   }

   theStream << dataLine.str() << "\n";

   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatObType::WriteMeasurement: \"%s\"\n",
            dataLine.str().c_str());
   #endif
   
   // temporary
   retval = true;

   return retval;
}


//-----------------------------------------------------------------------------
// ObservationData* ReadObservation()
//-----------------------------------------------------------------------------
/**
 * Retrieves an observation record
 *
 * This method reads an observation data set from a GmatInternal data stream and
 * returns the data to the caller.
 *
 * @return The observation data from the stream.  If there is no more data in
 * the stream, a NULL pointer is returned.
 */
//-----------------------------------------------------------------------------
ObservationData* GmatObType::ReadObservation()
{
   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage("GmatObType::ReadObservation() Executing\n");
   #endif

   std::string str;
   std::stringstream theLine;
   std::stringstream theLine1;

   Integer participantSize;
   Integer dataSize = 0;

   Real defaultNoiseCovariance = 0.1;


   // Do nothing when it is at the end of file
   if (theStream.eof())
      return NULL;

   // Read a line when it is not end of file
   std::getline(theStream, str);
   
   // Skip header and comment lines or empty lines
   while ((str[0] == '%') || (GmatStringUtil::RemoveAllBlanks(str) == "") ||
         (str.length() < 2))
   {
      std::getline(theStream, str);

      // Do nothing when it is at the end of file
      if (theStream.eof())
         return NULL;
   }


   // Processing data in the line
   theLine << str;
   theLine1 << str;
   currentObs.Clear();
   currentObs.dataFormat = "GMATInternal";

   // format: 21545.05439854615    Range    7000    GS2ID    ODSatID    2713.73185
   Real value;

   std::string taiEpochStr;
   theLine1 >> taiEpochStr;
   GmatTime  taiEpochGT(0.0);
   taiEpochGT.SetMjdString(taiEpochStr);
   currentObs.epochGT = (currentObs.epochSystem == TimeSystemConverter::TAIMJD ?
            taiEpochGT :
            theTimeConverter->ConvertFromTaiMjd(currentObs.epochSystem, taiEpochGT,
            GmatTimeConstants::JD_NOV_17_1858));

   GmatEpoch taiEpoch;
   theLine >> taiEpoch;
   currentObs.epoch = (currentObs.epochSystem == TimeSystemConverter::TAIMJD ?
         taiEpoch :
         theTimeConverter->ConvertFromTaiMjd(currentObs.epochSystem, taiEpoch,
               GmatTimeConstants::JD_NOV_17_1858));

   theLine >> currentObs.typeName;

   Integer type;
   theLine >> type;
   currentObs.type = (Gmat::MeasurementType)type;

   // Verify measurement type
   StringArray typeList = currentObs.GetAvailableMeasurementTypes();
   if (find(typeList.begin(), typeList.end(), currentObs.typeName) == typeList.end())
      throw MeasurementException("Error: GMAT cannot handle observation data with type '" + currentObs.typeName + "'.\n");


   // Signal based measurements have types that start at 9000; smaller IDs are
   // the old code.
   /// @todo Once ported to signal measurements, remove the code from here:
   if (type < 9000)
   {
      switch (currentObs.type)
      {
         case Gmat::GEOMETRIC_RANGE:
         case Gmat::GEOMETRIC_RANGE_RATE:
         case Gmat::USN_TWOWAYRANGE:
         case Gmat::USN_TWOWAYRANGERATE:
         case Gmat::DSN_TWOWAYRANGE:
         case Gmat::DSN_TWOWAYDOPPLER:
            participantSize = 2;
            dataSize = 1;
            break;

         case Gmat::TDRSS_TWOWAYRANGE:
         case Gmat::TDRSS_TWOWAYRANGERATE:
            participantSize = 3;
            dataSize = 1;
            break;

         case Gmat::GEOMETRIC_AZ_EL:
         case Gmat::GEOMETRIC_RA_DEC:
         case Gmat::OPTICAL_AZEL:
         case Gmat::OPTICAL_RADEC:
            participantSize = 2;
            dataSize = 2;
            defaultNoiseCovariance = 0.1;
            break;

         default:
            participantSize = 0;
            dataSize = 0;
            break;
      }

      for (Integer i = 0; i < participantSize; ++i)
      {
         theLine >> str;
         currentObs.participantIDs.push_back(str);
      }
   }
   else
   {
      #ifdef DEBUG_FILE_READ
         MessageInterface::ShowMessage("   Processing measurement type %d\n",
               type);
      #endif
#ifdef USE_OLD_GMDFILE_FORMAT
     participantSize = 2;           // In this version, measurement type is always 2-ways
#else
      // In this version, measurement type could be 1, 2, or multiple ways measurement
      if (!ProcessSignals(str, participantSize, dataSize))
      {
         MessageInterface::ShowMessage("Signal based measurement of type %d "
               "not processed successfully for line %s\n", type, str.c_str());
      }
#endif

      if ((currentObs.typeName == "Range") || (currentObs.typeName == "DSN_SeqRange" )
         || (currentObs.typeName == "RangeRate") || (currentObs.typeName == "DSN_TCP")
         || (currentObs.typeName == "SN_Doppler_Rtn") || (currentObs.typeName == "DSN_PNRange")
         || (currentObs.typeName == "SN_DOWD")
         || (currentObs.typeName == "SN_Doppler") || (currentObs.typeName == "SN_Range")
         || (currentObs.typeName == "BRTS_Doppler") || (currentObs.typeName == "BRTS_Range")
         || (currentObs.typeName == "Azimuth") || (currentObs.typeName == "Elevation")
         || (currentObs.typeName == "XEast") || (currentObs.typeName == "YNorth")
         || (currentObs.typeName == "XSouth") || (currentObs.typeName == "YEast")
         || (currentObs.typeName == "RightAscension") || (currentObs.typeName == "Declination")
         || (currentObs.typeName == "Range_Skin"))
      {
         dataSize = 1;
      }
      else if (currentObs.typeName == "GPS_PosVec")
      {
         participantSize = 1;
         dataSize = 3;
      }

      theLine >> str;
      if (str.substr(0,1) == "{")
      {
         if (str.size() == 1)
            theLine >> str;
         else
            str = str.substr(1);

         while(str.substr(str.length()-1) != "}")
         {
            currentObs.participantIDs.push_back(str);
            currentObs.sensorIDs.push_back("");
            theLine >> str;
         }
         str = str.substr(0,str.length()-1);
         if (str != "")
         {
            currentObs.participantIDs.push_back(str);
            currentObs.sensorIDs.push_back("");
         }
      }
      else if (currentObs.typeName == "GPS_PosVec")
      {
         currentObs.participantIDs.push_back(str);        // content of participantIDs will be updated later based on sensorIDs (for GPS Point Solution)
         currentObs.sensorIDs.push_back(str);
      }
      else
      {
          //Where old DSN_TCP goes
         currentObs.participantIDs.push_back(str);
         currentObs.sensorIDs.push_back("");
         std::string str1;
         theLine >> str1;
         currentObs.participantIDs.push_back(str1);
         currentObs.sensorIDs.push_back("");
         currentObs.participantIDs.push_back(str);
         currentObs.sensorIDs.push_back("");
         if (warn2WayDepricated)
         {
            if (currentObs.typeName == "DSN_TCP" || currentObs.typeName == "RangeRate")
            {
                std::string warn = "WARNING: Deprecated method for specifying a DSN_TCP or RangeRate observation detected in file: ";
                warn += streamName;
                warn += " \n    Found: \"StationID SpacecraftID\" expected \"{TransmitStationID SpacecraftID ReceiveStationID}\". \n";
                MessageInterface::ShowMessage(warn);
                warn2WayDepricated = false;
            }
         }
      }
   }


   if ((currentObs.typeName == "Range") || (currentObs.typeName == "SN_Range") 
      || (currentObs.typeName == "BRTS_Range")
      || (currentObs.typeName == "GPS_PosVec") || (currentObs.typeName == "Range_Skin"))
   {
      currentObs.unit = "km";
   }
   else if (currentObs.typeName == "DSN_TCP")
   {
      theLine >> currentObs.uplinkBand;
      theLine >> currentObs.dopplerCountInterval;
      currentObs.unit = "Hz";
   }
   else if (currentObs.typeName == "RangeRate")
   {
      theLine >> currentObs.uplinkBand;
      theLine >> currentObs.dopplerCountInterval;
      currentObs.unit = "km/s";
   }
   else if ((currentObs.typeName == "SN_Doppler")||
            (currentObs.typeName == "SN_Doppler_Rtn") ||
            (currentObs.typeName == "BRTS_Doppler"))
   {
      theLine >> currentObs.tdrsNode4Freq;            // this field is used to received frequency at the return-link TDRS 
      theLine >> currentObs.tdrsNode4Band;            // this field is used to received frequency band at the return-link TDRS 
      theLine >> currentObs.tdrsServiceID;            // value of serviceID would be "S1", "S2", or "MA"
      theLine >> currentObs.tdrsDataFlag;             // TDRS data flag would be 0 or 1
      theLine >> currentObs.tdrsSMARID;               // TDRS SMAR id
      theLine >> currentObs.dopplerCountInterval;
      currentObs.unit = "Hz";
   }
   else if (currentObs.typeName == "SN_DOWD")
   {

      // This secrtion is for reference TDRS one way doppler
      theLine >> currentObs.tdrsNode4FreqTDRSRef;            // this field is used to received frequency at the return-link TDRS 
      theLine >> currentObs.tdrsNode4BandTDRSRef;            // this field is used to received frequency band at the return-link TDRS 
      theLine >> currentObs.tdrsServiceIDTDRSRef;            // value of serviceID would be "S1", "S2", or "MA"
      theLine >> currentObs.tdrsDataFlagTDRSRef;             // TDRS data flag would be 0 or 1
      theLine >> currentObs.tdrsSMARIDTDRSRef;               // TDRS SMAR id
      theLine >> currentObs.dopplerCountIntervalTDRSRef;
      // This section is for comparison TDRS one way doppler
      theLine >> currentObs.tdrsNode4Freq;            // this field is used to received frequency at the return-link TDRS 
      theLine >> currentObs.tdrsNode4Band;            // this field is used to received frequency band at the return-link TDRS 
      theLine >> currentObs.tdrsServiceID;            // value of serviceID would be "S1", "S2", or "MA"
      theLine >> currentObs.tdrsDataFlag;             // TDRS data flag would be 0 or 1
      theLine >> currentObs.tdrsSMARID;               // TDRS SMAR id
      theLine >> currentObs.dopplerCountInterval;



      currentObs.unit = "Hz";
   }
   else if ((currentObs.typeName == "Azimuth") ||
            (currentObs.typeName == "Elevation") ||
            (currentObs.typeName == "XEast") || 
            (currentObs.typeName == "YNorth") || 
            (currentObs.typeName == "XSouth") || 
            (currentObs.typeName == "YEast") || 
            (currentObs.typeName == "RightAscension") || 
            (currentObs.typeName == "Declination"))
   {
      currentObs.unit = "deg";
   }

   for (Integer i = 0; i < dataSize; ++i)
   {
      theLine >> value;
      currentObs.value.push_back(value);
	  currentObs.value_orig.push_back(value);
   }

   // read extended infor from data record
   if (currentObs.typeName == "DSN_SeqRange")
   {
      theLine >> currentObs.uplinkBand;
//      theLine >> currentObs.uplinkFreq;
      theLine >> currentObs.uplinkFreqAtRecei;
      theLine >> currentObs.rangeModulo;
      currentObs.unit = "RU";
   }

   if (currentObs.typeName == "DSN_PNRange")
   {
       theLine >> currentObs.transmitDelay;
       theLine >> currentObs.receiveDelay;
       theLine >> currentObs.uplinkBand;
       theLine >> currentObs.downlinkBand;
       //      theLine >> currentObs.uplinkFreq;
       theLine >> currentObs.uplinkFreqAtRecei;
       theLine >> currentObs.rangeModulo;
       currentObs.unit = "RU";
   }

/*
   Covariance *noise = new Covariance();
   noise->SetDimension(dataSize);
   for (Integer i = 0; i < dataSize; ++i)
   {
      /// @todo Measurement noise covariance is hard-coded; this must be fixed
      (*noise)(i,i) = defaultNoiseCovariance;
   }
   currentObs.noiseCovariance = noise;
*/
   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage(" %.12lf    %s    %d    ", currentObs.epoch, currentObs.typeName.c_str(), currentObs.type);
      for (Integer i = 0; i < participantSize; ++i)
		  MessageInterface::ShowMessage("%s    ", currentObs.participantIDs.at(i).c_str());

      for (Integer i = 0; i < dataSize; ++i)
		  MessageInterface::ShowMessage("%.12lf    ", currentObs.value.at(i));

      if (currentObs.typeName == "DSN_SeqRange")
      {
         //MessageInterface::ShowMessage("   %d   %.12le   %.12le", currentObs.uplinkBand, currentObs.uplinkFreq, currentObs.rangeModulo);
         MessageInterface::ShowMessage("   %d   %.12le   %.12le", currentObs.uplinkBand, currentObs.uplinkFreqAtRecei, currentObs.rangeModulo);
      }
      else if ((currentObs.typeName == "DSN_TCP")||(currentObs.typeName == "RangeRate"))    //else if ((currentObs.typeName == "DSN_TCP")||(currentObs.typeName == "Doppler_RangeRate"))
      {
         MessageInterface::ShowMessage("   %d   %.12le", currentObs.uplinkBand, currentObs.dopplerCountInterval);
      }
	   MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("GmatObType::ReadObservation() End\n");
   #endif

   return &currentObs;
}


//-----------------------------------------------------------------------------
// bool Close()
//-----------------------------------------------------------------------------
/**
 * Closes the data stream
 *
 * This method flushes the data stream, and then closes it.
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool GmatObType::Close()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatObType::Close() Executing for %p\n", this);
   #endif
   bool retval = false;

   if (theStream.is_open())
   {
      theStream.flush();
      theStream.close();
      retval = !(theStream.is_open());
   }

   return retval;
}


//-----------------------------------------------------------------------------
// bool GmatObType::Finalize()
//-----------------------------------------------------------------------------
/**
 * Completes operations on this GmatObType.
 *
 * @return true always -- there is no GmatObType specific finalization needed.
 */
//-----------------------------------------------------------------------------
bool GmatObType::Finalize()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatObType::Finalize() Executing\n");
   #endif
   bool retval = true;

   return retval;
}

//------------------------------------------------------------------------------
// bool ProcessSignals(const std::string str, Integer& participantSize,
//       Integer& dataSize)
//------------------------------------------------------------------------------
/**
 * Parses a measurement file line and counts participants and data elements
 *
 * @param str The data line from the tracking data file
 * @param participantSize The counter for participants from the line
 * @param dataSize The counter for data entries on the line
 *
 * @return true if the line was successfully parsed, false if not
 */
//------------------------------------------------------------------------------
bool GmatObType::ProcessSignals(const std::string str, Integer& participantSize,
      Integer& dataSize)
{
   bool retval = false;

   participantSize = 0;
   dataSize = 0;

   std::stringstream temp(str);
   std::string field;

   // First 3 fields are already handled
   for (Integer i = 0; i < 3; ++i)
   {
      temp >> field;
      #ifdef DEBUG_SIGNAL_READ
         MessageInterface::ShowMessage("   +++ field:  %s\n", field.c_str());
      #endif
   }

   // Next set of fields are participants
   while (temp.eof() == false)
   {
      temp >> field;
      if (GmatStringUtil::IsValidNumber(field, true) == true)
      {
         #ifdef DEBUG_SIGNAL_READ
            MessageInterface::ShowMessage("   +++ Measurement:  %s\n",
                  field.c_str());
         #endif
         ++dataSize;    // Track first data entry!
         break;
      }

      #ifdef DEBUG_SIGNAL_READ
         MessageInterface::ShowMessage("   +++ Participant:  %s\n",
               field.c_str());
      #endif
      ++participantSize;
   }

   // Then the data
   while (temp.eof() == false)
   {
      temp >> field;

      if (GmatStringUtil::IsValidNumber(field, true) == false)
         throw MeasurementException("Data line \"" + str + "\" is improperly "
               "formatted for a GMAT measurement data (gmd) file");

      #ifdef DEBUG_SIGNAL_READ
         MessageInterface::ShowMessage("   +++ Measurement:  %s\n",
               field.c_str());
      #endif
      ++dataSize;
   }

   #ifdef DEBUG_SIGNAL_READ
      MessageInterface::ShowMessage("   ---> %d participants and %d data "
            "entries\n", participantSize, dataSize);
   #endif

   if ((participantSize > 0) && (dataSize > 0))
      retval = true;

   return retval;
}


//std::string GmatObType::CheckTypeDeprecation(const std::string datatype)
//{
//   std::string typeName = datatype;
//   for (std::map<std::string, std::string>::iterator i = depTypeMap.begin();
//      i != depTypeMap.end(); ++i)
//   {
//      if ((*i).first == datatype)
//      {
//         std::stringstream ss;
//         ss << "Warning: measurement type name '" << datatype << 
//            "' in .gmd data file is deprecated and will be removed in a future release. Use '" <<
//            (*i).second << "' instead.\n";
//         
//         if (find(mesg.begin(), mesg.end(), ss.str()) == mesg.end())
//         {
//            MessageInterface::ShowMessage(ss.str());
//            mesg.push_back(ss.str());
//         }
//         typeName = (*i).second;
//         break;
//      }
//   }
//
//   return typeName;
//}

