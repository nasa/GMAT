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

#include "TdmReadWriter.hpp"
#include "MessageInterface.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include "MeasurementException.hpp"
#include "DateUtil.hpp"


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TdmReadWriter()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
TdmReadWriter::TdmReadWriter()
{
   theErrorHandler = new TdmErrorHandler();
   theDOMParser = NULL;
  
   theBody = NULL;
   theSegment = NULL;
   theData = NULL;
   theObservation = NULL;
   xercesInitialized = false;
   observationIndex = 0;
   
   // Fill in the map
   mapTransmitBand["S"] = 1.0;
   mapTransmitBand["X"] = 2.0;
   mapTransmitBand["KA"] = 3.0;
   mapTransmitBand["KU"] = 4.0;
   mapTransmitBand["L"] = 5.0;
}


//------------------------------------------------------------------------------
// TdmReadWriter(const TdmReadWriter &trw)
//------------------------------------------------------------------------------
/**
 * Copy Constructor
 */
//------------------------------------------------------------------------------
TdmReadWriter::TdmReadWriter(const TdmReadWriter &trw)
{
   theErrorHandler = NULL;
   theDOMParser = NULL;
   mapTransmitBand.insert(trw.mapTransmitBand.begin(), trw.mapTransmitBand.end());

   *this = trw;
}


//------------------------------------------------------------------------------
// operator=(const TdmReadWriter &trw)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
TdmReadWriter& TdmReadWriter::operator=(const TdmReadWriter &trw)
{
   if (this != &trw)
   {
      TdmErrorHandler *newErrorHandler = trw.theErrorHandler; 
      if (theErrorHandler)
      {
         delete theErrorHandler;
         theErrorHandler = NULL;
      }

      theErrorHandler = newErrorHandler;
      theDOMParser = trw.theDOMParser;;
      theBody = trw.theBody;
      theSegment = trw.theSegment;
      theData = trw.theData;
      theObservation = trw.theObservation;
      xercesInitialized = trw.xercesInitialized;
      observationIndex = trw.observationIndex;
      mapTransmitBand.insert(trw.mapTransmitBand.begin(), trw.mapTransmitBand.end());
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~TdmReadWriter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TdmReadWriter::~TdmReadWriter()
{
   if(xercesInitialized)
      Finalize();
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes TdmReadWriter.
 *
 * This method will initialize the DOM Parser, and configure it for error handling
 * and Schema validation.
 *
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool TdmReadWriter::Initialize()
{
   if (!xercesInitialized)
   {
      try
      {
         XMLPlatformUtils::Initialize();
         theDOMParser = new XercesDOMParser();

         theDOMParser->setErrorHandler(theErrorHandler);
	      theDOMParser->setValidationScheme(XercesDOMParser::Val_Auto);
	      theDOMParser->setDoNamespaces(true);
	      theDOMParser->setDoSchema(true);
	      theDOMParser->setValidationConstraintFatal(true);

         xercesInitialized = true;
      }
      catch(const XMLException &xe)
      {
         std::string errMsg ("Xerces failed to initialize: ");
         errMsg.push_back(*XMLString::transcode(xe.getMessage()));
         throw MeasurementException(errMsg);
      }
   }

   return xercesInitialized;
}


//------------------------------------------------------------------------------
// ObservationData *ProcessMetadata()
//------------------------------------------------------------------------------
/**
 * Processes metadata and returns it to the caller.
 *
 * This method called each time a new segment is encountered, it loads the metadata 
 * into the ObservationData template and returns a pointer to it for use by the 
 * TdmObType that called the method.
 *
 * @param none
 *
 * @return ObsData pointer
 */
//------------------------------------------------------------------------------
//ObsData *TdmReadWriter::ProcessMetadata()
ObservationData *TdmReadWriter::ProcessMetadata()
{
   if (theSegment != NULL)
   {
      // Clear observation Data if it has been filled in with data.
      theTemplate.Clear();

      DOMElement *pMetaData = theSegment->getFirstElementChild();
      DOMNodeList *pChilds = pMetaData->getChildNodes();
					
		for (UnsignedInt i = 0; i < pChilds->getLength(); i++)
		{
			DOMElement *pChild = (DOMElement *)pChilds->item(i);
			if (pChild->getNodeType() == DOMNode::ELEMENT_NODE)
         {
            //Fill in the observation data theTemplate for each
            // attributes.
            switch(HashIt(pChild->getNodeName()))
            {
               case TIME_SYSTEM:
               {
                  std::string strT(XMLString::transcode(pChild->getTextContent()));
                  if ( strT == "UTC")
                     theTemplate.epochSystem = TimeSystemConverter::UTCMJD;
                  break;
               }
               case PARTICIPANT_1:
               case PARTICIPANT_2:
               case PARTICIPANT_3:
               case PARTICIPANT_4:
               case PARTICIPANT_5:
               {
                  theTemplate.participantIDs.push_back(XMLString::transcode(pChild->getTextContent()));
                  break;
               }
               case MODE:
                  break;
               case PATH:
               {
                  StringArray IDs;
                  char *pPath = XMLString::transcode(pChild->getTextContent());
                  char *pTok;
                  pTok = strtok(pPath, ",");
                  
                  while (pTok != NULL)
                  {
                     IDs.push_back(theTemplate.participantIDs.at(atoi(pTok)-1));
                     pTok = strtok(NULL, ","); 
                  }

                  theTemplate.strands.push_back(IDs);
                  XMLString::release(&pPath);
                  break;
               }
               case PATH_1:
                  break;
               case PATH_2:
                  break;
               case TRANSMIT_BAND:
               {
                  std::string strT(XMLString::transcode(pChild->getTextContent()));

                  std::map<std::string, Real>::iterator it;
                  it = mapTransmitBand.find(strT);
                  
                  if (it != mapTransmitBand.end())
                     theTemplate.value.push_back(it->second);
                  else
                     theTemplate.value.push_back(0.0);

                  theTemplate.dataMap.push_back(XMLString::transcode(pChild->getNodeName()));

                  break;
               }
               case RECEIVE_BAND:
                  break;
               case TIMETAG_REF:
               {
                  std::string strT(XMLString::transcode(pChild->getTextContent()));
                  if (strT.compare("RECEIVE") == 0 || strT.compare("receive") == 0)
                     theTemplate.epochAtEnd = true;
                  else if (strT.compare("TRANSMIT") || strT.compare("transmit") == 0)
                     theTemplate.epochAtEnd = false;
                  
                  break;
               }
//               case INTEGRATION_INTERVAL:
//                  {
//                     theTemplate.data.push_back(atof(XMLString::transcode(pChild->getTextContent())));
//                     theTemplate.dataMap.push_back(XMLString::transcode(pChild->getNodeName()));
//                  }
//                  break;
               case INTEGRATION_REF:
                  {
                     std::string strT(XMLString::transcode(pChild->getTextContent()));
                     if (strT.compare("END") == 0 || strT.compare("end") == 0)
                        theTemplate.epochAtIntegrationEnd = true;
                     else if (strT.compare("START") || strT.compare("start") == 0)
                        theTemplate.epochAtIntegrationEnd = false;
                  }
                  break;
               case RANGE_MODE:
                  break;
               case RANGE_MODULUS:
               case FREQ_OFFSET:
               case INTEGRATION_INTERVAL:
               {
                  theTemplate.value.push_back(atof(XMLString::transcode(pChild->getTextContent())));
                  theTemplate.dataMap.push_back(XMLString::transcode(pChild->getNodeName()));
                  break;
               }
               case RANGE_UNITS:
               {
                  theTemplate.unit = std::string(XMLString::transcode(pChild->getTextContent()));
                  break;
               }
               default:
                  break;
            }
			}
      }

      theData = theSegment->getLastElementChild();
      DOMNodeList *pObsChilds = theData->getChildNodes();
      int i = 0;
      while ( ((DOMElement *)pObsChilds->item(i))->getNodeType() != DOMNode::ELEMENT_NODE)
      {
        i++;
      }
      
      //first observation data
      theObservation = (DOMElement *)pObsChilds->item(i);

      return &theTemplate;
   }
   
   return NULL;
}


//------------------------------------------------------------------------------
// bool LoadRecord()
//------------------------------------------------------------------------------
/**
 * Loads Data section of XML file.
 *
 * This method retrieves the observation data and fills in the relevant fields in
 * the ObservationData record that is passed to it, by pushing the observation data
 * to the data member and the associated field tags to the dataMap in the input 
 * ObservationData record.
 *
 * @param ObsData *
 *
 * @return ObsData
 */
//------------------------------------------------------------------------------
//ObsData *TdmReadWriter::LoadRecord(ObsData *newData)
ObservationData *TdmReadWriter::LoadRecord(ObservationData *newData)
{
   std::string strPrevEpoch;
   std::string strCurrEpoch;
   std::string strObs;
   std::string strNodeName;

   strPrevEpoch = std::string(XMLString::transcode(theObservation->getFirstElementChild()->getTextContent()));
  
   for (UnsignedInt i = observationIndex; i < theData->getChildNodes()->getLength(); i++)
   {
      theObservation = (DOMElement *) theData->getChildNodes()->item(i);
      if (theObservation->getNodeType() == DOMNode::ELEMENT_NODE)
		{
			strCurrEpoch = std::string(XMLString::transcode(theObservation->getFirstElementChild()->getTextContent()));
         strObs = std::string(XMLString::transcode(theObservation->getLastElementChild()->getTextContent())); 
         strNodeName = std::string(XMLString::transcode(theObservation->getLastElementChild()->getNodeName()));

         if (theTemplate.typeName == "")
            theTemplate.typeName = newData->typeName = strNodeName;
         
         if (strPrevEpoch != strCurrEpoch)
         {
            //set the index for next call
            observationIndex = i;

            return &theTemplate;
         }
         else
         {
            // push data into newData     
            newData->epoch = ParseEpoch(strPrevEpoch);  
            newData->value.push_back(atof(strObs.c_str()));
            newData->dataMap.push_back(strNodeName);
         }
           
         strPrevEpoch = strCurrEpoch;
      }
   }
   
   theSegment = theSegment->getNextElementSibling();
   // reset the index back to zero
   observationIndex = 0;
   return (ProcessMetadata());
}


//------------------------------------------------------------------------------
// bool Validate()
//------------------------------------------------------------------------------
/**
 * Validates the XML file.
 *
 * This method called when a new TDM file is loaded for the first data read, it uses 
 * Xerces to validate the data file against the TDM schema.
 *
 * @param TDM XML filename
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool TdmReadWriter::Validate(const std::string &tdmFileName)
{
   LocalFileInputSource *xmlFile;

   // Load the TDM XML file
   try
   {
      xmlFile = new LocalFileInputSource(XMLString::transcode(tdmFileName.c_str()));
   }
   catch(const XMLException &xe)
   {
      std::string errMsg ("Xerces failed to load the file: ");
      errMsg.push_back(*XMLString::transcode(xe.getMessage()));
      throw MeasurementException(errMsg);
   }

   // Parse the TDM XML 
   if(theDOMParser != NULL)
   {
      theDOMParser->parse(*xmlFile);

      // Check to see if the Schema Validation passed
      if (theDOMParser->getErrorCount() == 0)
      {
         MessageInterface::ShowMessage("XML file is validated against the Schema file successfully.\n");
      }
	   else
      {
		   std::string errMsg ("Xerces failed validation: XML file does not conform to Schema: ");
         throw MeasurementException(errMsg);
      }  
   }

   return true;
}


//------------------------------------------------------------------------------
// bool Finalize()
//------------------------------------------------------------------------------
/**
 * Finalizes the TdmReadWriter object.
 *
 * This method cleans up the TDM file and Xerces interface if needed, and
 * any other artifacts still in memory.
 *
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool TdmReadWriter::Finalize()
{
   if (theDOMParser)
   {
      delete theDOMParser;
      theDOMParser = NULL;
   }
   if (theErrorHandler)
   {
      delete theErrorHandler;
      theErrorHandler = NULL;
   }
   xercesInitialized = false;

   XMLPlatformUtils::Terminate();

   return xercesInitialized;
}


//------------------------------------------------------------------------------
// bool SetBody()
//------------------------------------------------------------------------------
/**
 * Reads Header section of XML file (for checking the version number),
 * set the Body element and first Segment node.
 *
 * This method reads the header in XML file, and set the Body element.
 * 
 *
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool TdmReadWriter::SetBody()
{
   DOMDocument *pDoc = theDOMParser->getDocument();
	DOMElement *pTdm = pDoc->getDocumentElement();

	if (pTdm->hasAttributes())
	{
		DOMAttr *pAttrId = pTdm->getAttributeNode(XMLString::transcode("id"));
		if (!XMLString::equals(XMLString::transcode("CCSDS_TDM_VERS"),pAttrId->getValue()))
		{
			std::string errMsg = " CCSDS_TDM_VERS id is not correct";
         throw MeasurementException(errMsg);
		}

		DOMAttr *pAttrVer = pTdm->getAttributeNode(XMLString::transcode("version"));
		if (!XMLString::equals(XMLString::transcode("1.0"),pAttrVer->getValue()))
		{
			std::string errMsg = "The TDM VERSION is not correct.\n";
			throw MeasurementException(errMsg);
      }
	}

   //header
   /********** No NEED to parse these.*********** /
	DOMElement *pHeader = pTdm->getFirstElementChild();
	DOMElement *pComments = pHeader->getFirstElementChild();

	while (pComments != NULL && XMLString::equals(XMLString::transcode("COMMENT"), pComments->getTagName()))  //comments
	{
		pComments = pComments->getNextElementSibling();	
	}
	
	//whether or not there are comments or not, we get here.
	DOMElement * pDate = (DOMElement *)pHeader->getElementsByTagName(XMLString::transcode("CREATION_DATE"))->item(0);
	DOMElement * pOrig = (DOMElement *)pHeader->getElementsByTagName(XMLString::transcode("ORIGINATOR"))->item(0);
   **********************************************/

   theBody = pTdm->getLastElementChild();
   theSegment = theBody->getFirstElementChild();

   return true;
}


//------------------------------------------------------------------------------
// MetaData HashIt()
//------------------------------------------------------------------------------
/**
 * Hashes a string name to a corresponding enum value.
 * 
 *
 * This method hashes a string to a number.
 * 
 *
 * @param char * pointing to node name
 *
 * @return an enumeration value
 */
//------------------------------------------------------------------------------
TdmReadWriter::MetaData TdmReadWriter::HashIt(const XMLCh *xmlNodeName)
{
   std::string strN(XMLString::transcode(xmlNodeName));

   if (strN == "TIME_SYSTEM")
      return TIME_SYSTEM;
   if (strN == "PARTICIPANT_1")
      return PARTICIPANT_1;
   if (strN == "PARTICIPANT_2")
      return PARTICIPANT_2;
   if (strN == "PARTICIPANT_3")
      return PARTICIPANT_3;
   if (strN == "PARTICIPANT_4")
      return PARTICIPANT_4;
   if (strN == "PARTICIPANT_5")
      return PARTICIPANT_5;
   if (strN == "MODE")
      return MODE;
   if (strN == "PATH")
      return PATH;
   if (strN == "PATH_1")
      return PATH_1;
   if (strN == "PATH_2")
      return PATH_2;
   if (strN == "TRANSMIT_BAND")
      return TRANSMIT_BAND;
   if (strN == "RECEIVE_BAND")
      return RECEIVE_BAND;
   if (strN == "TIMETAG_REF")
      return TIMETAG_REF;
   if (strN == "INTEGRATION_INTERVAL")
      return INTEGRATION_INTERVAL;
   if (strN == "INTEGRATION_REF")
      return INTEGRATION_REF;
   if (strN == "RANGE_MODE")
      return RANGE_MODE;
   if (strN == "RANGE_MODULUS")
      return RANGE_MODULUS;
   if (strN == "RANGE_UNITS")
      return RANGE_UNITS;
   if (strN == "FREQ_OFFSET")
      return FREQ_OFFSET;

   return NONE;
}


//------------------------------------------------------------------------------
// GmatEpoch ParseEpoch()
//------------------------------------------------------------------------------
/**
 * Parse and Convert Epoch datetime string.
 * 
 *
 * This method parses and converts the Epoch datetime string
 * passed in to the GmatEpoch that will be used in ObsData structure.
 * 
 * Two datetime formats :
 * 1.) YYYY-MM-DDThh:mm:ss[d->d][Z]
 * 2.) YYYY-DDDThh:mm:ss[d->d][Z]
 * [d->d] is an optional fraction seconds; 'Z" is an optional time code terminator.
 * refer to CCSDS503.0-B-1_TDM.pdf document page 52. 
 * 
 *
 * @param const std::string
 *
 * @return GmatEpoch
 */
//------------------------------------------------------------------------------
GmatEpoch TdmReadWriter::ParseEpoch(const std::string strEpoch)
{  
   Integer year = -1, doy = -1, month = -1, day = -1, hour = -1, minute = -1 ;
   Real sec = -1;
   GmatEpoch mjd;

   // c_str() returns a const char *
   char *epoch = (char *)strEpoch.c_str();
   char *pch, *date, *time;
   Byte i = 0;

   // Start Parsing....
   // break the string into date and time parts :
   // using delimiter "T". 
   pch = strtok(epoch, "T");
   while (pch != NULL)
   {
      if (i == 0)
         date = pch;
      else
         time = pch;
      i++;
      pch = strtok(NULL, "T");
   }

   // parse the date part
   i = 0;
   pch = strtok(date, "-");
   while (pch != NULL)
   { 
      if (i == 0)
         year = atoi(pch);
      else if (i == 1)
         doy = atoi(pch);
      else if (i == 2)
      {
         month = doy;
         day = atoi(pch);
         // reset it back to -1 to distinguish between two formats
         doy = -1; 
      }
      i++;
      pch = strtok(NULL, "-");
   }

   // parse the time part
   i = 0;
   pch = strtok(time, ":");
   while (pch != NULL)
   {
      if (i == 0)
         hour = atoi(pch);
      else if (i == 1)
         minute = atoi(pch);
      else if (i == 2)
         sec = atof(pch);
      i++;
      pch = strtok(NULL, ":");
   }
   // Done Parsing ....
  
   // Start Conversion ....
   
   if (doy != -1)  // Handle the second format
      ToMonthDayFromYearDOY(year, doy, month, day);
   
   mjd = ModifiedJulianDate(year, month, day, hour, minute, sec); 
  
   return mjd;
}
