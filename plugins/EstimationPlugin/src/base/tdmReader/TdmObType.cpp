//------------------------------------------------------------------------------
//                            TdmObType
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
// Created: 2014/8/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS.
//
/**
 * TdmObType class implementation
 */
//------------------------------------------------------------------------------

#include "TdmObType.hpp"
// Classes needed to configure the test cases
#include "MessageInterface.hpp"

#include "FileManager.hpp"
#include "MeasurementException.hpp"
#include "StringUtil.hpp"
#include <sstream>

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TdmObType()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
TdmObType::TdmObType(const std::string withName) :
   ObType         ("TDM", withName)
{
   theReadWriter  = new TdmReadWriter();
   theTemplate    = NULL;
   isFirstRead    = true;
   tdmPassedValidation = false;
   typeIdentifier = TFSMagicNumbers::Instance();
}


//------------------------------------------------------------------------------
// TdmObType(const TdmObType &tot)
//------------------------------------------------------------------------------
/**
 * Copy Constructor
 */
//------------------------------------------------------------------------------
TdmObType::TdmObType(const TdmObType &tot) :
   ObType         (tot)
{
   theReadWriter  = new TdmReadWriter();
   theTemplate    = NULL;
   isFirstRead    = true;
   tdmPassedValidation = false;
   typeIdentifier = tot.typeIdentifier;
}


//------------------------------------------------------------------------------
// ~TdmObType()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TdmObType::~TdmObType()
{
   delete theReadWriter;
   theReadWriter = NULL;
}


//------------------------------------------------------------------------------
// TdmObType& TdmObType::operator =(const TdmObType& tot)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param tot The obtype being copied here
 *
 * @return This ObType, set to look like tot
 */
//------------------------------------------------------------------------------
TdmObType& TdmObType::operator =(const TdmObType& tot)
{
   if (this != &tot)
   {
      ObType::operator =(tot);

      theReadWriter  = new TdmReadWriter();
      theTemplate    = NULL;
      isFirstRead    = true;
      tdmPassedValidation = false;
      typeIdentifier = tot.typeIdentifier;
   }

   return *this;
}


//-----------------------------------------------------------------------------
// GmatBase* Clone() const
//-----------------------------------------------------------------------------
/**
 * Cloning method used to create a TdmObType from a GmatBase pointer
 *
 * @return A new TdmObType object matching this one
 */
//-----------------------------------------------------------------------------
GmatBase *TdmObType::Clone() const
{
   return new TdmObType(*this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes TdmReadWriter object.
 *
 * This method initializes the TdmReadWriter and verifies all attributes are set
 * 
 *
 * @param none
 *
 * @return boolean
 */
//------------------------------------------------------------------------------
bool TdmObType::Initialize()
{
   return (theReadWriter->Initialize());
}


//------------------------------------------------------------------------------
// bool Open()
//------------------------------------------------------------------------------
/**
 * Opens XML file and validates it.
 *
 * This method opens the TDM XML file and validates against the Schema.
 * 
 * @param forRead True to open for reading, false otherwise
 * @param forWrite True to open for writing, false otherwise
 * @param append True if data being written should be appended, false if not
 *
 * @return true if the the stream was opened, false if not
 */
//------------------------------------------------------------------------------
bool TdmObType::Open(bool forRead, bool forWrite, bool append)
{
   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("GmatObType::Open(%s, %s, %s) Executing\n",
            (forRead ? "true" : "false"),
            (forWrite ? "true" : "false"),
            (append ? "true" : "false") );
   #endif
   bool retval = false;

   // temporary
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
      size_t dotLoc = fullPath.find_last_of('.');               // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
      size_t slashLoc = fullPath.find_last_of('/');             // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
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

      if (theReadWriter->Validate(fullPath))
      {
         tdmPassedValidation = true;
         retval = true;
      }
   }

   return retval;
}

//-----------------------------------------------------------------------------
// bool IsOpen()
//-----------------------------------------------------------------------------
/**
 * Returns the status of the stream
 *
 * @return true if the stream is open, false if not
 */
//-----------------------------------------------------------------------------
bool TdmObType::IsOpen()
{
   return tdmPassedValidation;
}

//------------------------------------------------------------------------------
// bool Close()
//------------------------------------------------------------------------------
/**
 * Closes the read operation by receiving "end of file".
 *
 * This method performs "end of file" activities.
 * 
 * @param none
 *
 * @return true if done properly
 */
//------------------------------------------------------------------------------
bool TdmObType::Close()
{
   isFirstRead = true;
   return true;
}


//------------------------------------------------------------------------------
// bool Finalize()
//------------------------------------------------------------------------------
/**
 * Finalizes a read operation
 *
 * This method calls TdmReadWriter Finalize() method, and performs other 
 * clean up as needed.
 * 
 * @param none
 *
 * @return true if done properly
 */
//------------------------------------------------------------------------------
bool TdmObType::Finalize()
{
   isFirstRead = true;
   return (theReadWriter->Finalize());
}


//------------------------------------------------------------------------------
// ObservationData * ReadObservation()
//------------------------------------------------------------------------------
/**
 * Retrieves an observation record
 *
 * This method reads an observation data set from a TDM XML file and
 * returns the data to the caller.
 *
 * @param none.
 *
 * @return The observation data from the XML content in memory.  
 * If there is no more data, a NULL pointer is returned.
 */
//------------------------------------------------------------------------------
//ObsData *TdmObType::ReadObservation()
ObservationData *TdmObType::ReadObservation()
{
   // Is it the first time XML open?
   if (isFirstRead)
   {
      isFirstRead = false;

      // open the XML file to validate and set the Body element.
      Open();
      theReadWriter->SetBody();
      // Read in the Metadata from the first Segment in Body element
      theTemplate = theReadWriter->ProcessMetadata();

      if (typeIdentifier == NULL)
         typeIdentifier = TFSMagicNumbers::Instance();
   }
    
//   ObsData *newData = new ObsData(*theTemplate);
   ObservationData *newData = new ObservationData(*theTemplate);
   theTemplate = theReadWriter->LoadRecord(newData);

   // we are at the end of file
   if (!theTemplate)
   {
      delete newData;
      newData = NULL;
   }
   else
      typeIdentifier->FillMagicNumber(newData);
      
   return newData;
}


//-----------------------------------------------------------------------------
// bool AddMeasurement(MeasurementData *md)
//-----------------------------------------------------------------------------
/**
 * Adds a new measurement to the TdmObType data file
 *
 * This method takes the raw observation data passed in and formats it into a
 * string compatible with XML Schema data files, and then writes that
 * to the XML file.
 *
 * @param md The measurement data containing the observation.
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool TdmObType::AddMeasurement(MeasurementData *md)
{
   return true;

}


//-----------------------------------------------------------------------------
// RampTableData * ReadRampTableData()
//-----------------------------------------------------------------------------
/**
 *
 *
 * @param none
 *
 * @return RampTableData *
 */
//-----------------------------------------------------------------------------
RampTableData *TdmObType::ReadRampTableData()
{
   return NULL;
}
