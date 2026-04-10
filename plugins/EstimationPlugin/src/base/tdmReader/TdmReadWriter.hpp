//------------------------------------------------------------------------------
//                            TdmReadWriter
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
// Author: Farideh Farahnak
// Created: 2014/8/20
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS.
//
/**
 * TdmReadWriter class will be used by the TdmObType class.
 */
//------------------------------------------------------------------------------

#ifndef TdmReadWriter_hpp
#define TdmReadWriter_hpp

#include "TdmErrorHandler.hpp"
#include "ObservationData.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"

/**
* Class that implements the XML parsing details
* 
* This class provides the interface into the Xerces
* library, used to handle the XML parsing necessary to
* work with the TDM files.
* TdmObType class will be using this class to access the
* observation data records.
*/
class  ESTIMATION_API TdmReadWriter
{
public:
   TdmReadWriter();
   ~TdmReadWriter();

   TdmReadWriter(const TdmReadWriter &trw);
   TdmReadWriter& operator=(const TdmReadWriter &trw);

   bool Initialize();
   bool Validate(const std::string &tdmFileName);
//   ObsData *ProcessMetadata();
//   ObsData *LoadRecord(ObsData *newData);
   ObservationData *ProcessMetadata();
   ObservationData *LoadRecord(ObservationData *newData);
   bool Finalize();
   bool SetBody();

private:
   /// An ObservationData object used to capture metadata
   ObservationData theTemplate;
//   ObsData theTemplate;
   /// Error Handler that Xerces DOM parser uses to pass errors/warnings to GMAT
   TdmErrorHandler *theErrorHandler;
   /// Xerces DOM Parser
   XercesDOMParser *theDOMParser;
   /// Is the Xerces initialized
   bool xercesInitialized;
   /// XML file consists of header and body
   DOMElement *theBody;
   /// XML file has "Segment" node(s)
   DOMElement *theSegment;
   /// XML file has "Data" node
   DOMElement *theData;
   /// XML file has "Observation" node (s)
   DOMElement *theObservation;
   /// Observation node Index where we left
   int observationIndex;
   /// map Transmit Band to a real number
   std::map<std::string, Real> mapTransmitBand;

   /// enumeration type for all data in Metadata
   enum MetaData
   {
      NONE = -1,
      TIME_SYSTEM = 0,
      PARTICIPANT_1,
      PARTICIPANT_2,
      PARTICIPANT_3,
      PARTICIPANT_4,
      PARTICIPANT_5,
      MODE,
      PATH,
      PATH_1,
      PATH_2,
      TRANSMIT_BAND,
      RECEIVE_BAND,
      TIMETAG_REF,
      INTEGRATION_INTERVAL,
      INTEGRATION_REF,
      RANGE_MODE,
      RANGE_MODULUS,
      RANGE_UNITS,
      FREQ_OFFSET
   };

   /// Hash the Node name to corresponding enum value
   MetaData HashIt(const XMLCh *xmlNodeName);

   /// Convert Epoch data to date and time utility values
   GmatEpoch ParseEpoch(const std::string strEpoch);
   
};

#endif   //TdmReadWriter_hpp
