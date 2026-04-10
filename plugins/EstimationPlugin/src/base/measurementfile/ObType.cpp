//------------------------------------------------------------------------------
//                                  ObType
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
//         Based on code by Matthew P. Wilkins, Shafer Corporation
// Created: 2009/07/06
//
/**
 * Base class used for the observation data streams.
 */
//------------------------------------------------------------------------------


#include "ObType.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZE

//------------------------------------------------------------------------------
// ObType(const std::string &obType, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param obType String describing the observation type.
 * @param name Name for the obtype
 */
//------------------------------------------------------------------------------
ObType::ObType(const std::string &obType, const std::string &name) :
   GmatBase          (Gmat::OBTYPE, obType, name),
   streamName        (""),
   header            (""),
   openForRead       (true),
   openForWrite      (false)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ObType default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::OBTYPE);
   objectTypeNames.push_back("ObType");
   objectTypeNames.push_back(obType);

   theTimeConverter = TimeSystemConverter::Instance();
}


//------------------------------------------------------------------------------
// ~ObType
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ObType::~ObType()
{
}


//------------------------------------------------------------------------------
// ObType(const ObType& ot)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ot The instance that is copied to this new instance
 */
//------------------------------------------------------------------------------
ObType::ObType(const ObType& ot) :
   GmatBase          (ot),
   streamName        (ot.streamName),
   header            (ot.header),
   openForRead       (ot.openForRead),
   openForWrite      (ot.openForWrite)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ObType cop constructor from <%s,%p> to <%s,%p>\n", ot.GetName().c_str(), &ot, GetName().c_str(), this);
#endif

	theTimeConverter = TimeSystemConverter::Instance();
}


//------------------------------------------------------------------------------
// ObType& operator=(const ObType& ot)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ot The instance that is copied to this new instance
 *
 * @return This object, configured to match ot.
 */
//------------------------------------------------------------------------------
ObType& ObType::operator=(const ObType& ot)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ObType operator =\n");
#endif

   if (this != &ot)
   {
      streamName   = ot.streamName;
      header       = ot.header;
      openForRead  = ot.openForRead;
      openForWrite = ot.openForWrite;
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetStreamName(std::string name)
//------------------------------------------------------------------------------
/**
 * Sets the stream name
 *
 * @param name The stream name
 */
//------------------------------------------------------------------------------
void ObType::SetStreamName(std::string name)
{
   streamName = name;
}


//------------------------------------------------------------------------------
// std::string GetStreamName()
//------------------------------------------------------------------------------
/**
 * Gets the stream name
 *
 * @return	name of the stream
 */
//------------------------------------------------------------------------------
std::string ObType::GetStreamName()
{
   return streamName;
}


//-----------------------------------------------------------------------------
// bool Initialize()
//-----------------------------------------------------------------------------
/**
 * Prepares the ObType for use in estimation or simulation.
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool ObType::Initialize()
{
#ifdef DEBUG_INITIALIZE
	MessageInterface::ShowMessage("ObType::Initialize()\n");
#endif

   return false;
}


//-----------------------------------------------------------------------------
// bool Open(bool forRead, bool forWrite, bool append)
//-----------------------------------------------------------------------------
/**
 * Opens an ObType stream for processing
 *
 * @param forRead True to open for reading, false otherwise
 * @param forWrite True to open for writing, false otherwise
 * @param append True if data being written should be appended, false if not
 *
 * @return true if the the stream was opened, false if not
 */
//-----------------------------------------------------------------------------
bool ObType::Open(bool forRead, bool forWrite, bool append)
{
   return false;
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
bool ObType::IsOpen()
{
   return false;
}


//-----------------------------------------------------------------------------
// bool Close()
//-----------------------------------------------------------------------------
/**
 * Closes the stream
 *
 * @return true if the stream was closed, false if it was not changed
 */
//-----------------------------------------------------------------------------
bool ObType::Close()
{
   return false;
}


//-----------------------------------------------------------------------------
// bool Finalize()
//-----------------------------------------------------------------------------
/**
 * Performs all actions needed to stop finish using the stream
 *
 * @return true if the stream finalization succeeded, false if not
 */
//-----------------------------------------------------------------------------
bool ObType::Finalize()
{
   return false;
}
