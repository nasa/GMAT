//$Id:$
//------------------------------------------------------------------------------
//                              SpiceKernelReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// FDSS Task order 28.
//
// Author: Wendy C. Shoan
// Created: 2009.02.24
//
/**
 * Implementation of the SpicKernelReader, which reads SPICE data (kernel) files.
 * This class calls the JPL-supplied CSPICE routines to read the specified
 * SPICE file and return the requested data.
 */
//------------------------------------------------------------------------------


#include "SpiceKernelReader.hpp"
#include "A1Mjd.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"
#include "UtilityException.hpp"

//#define DEBUG_SPICE_KERNEL_READER

//---------------------------------
// static data
//---------------------------------
const Integer  SpiceKernelReader::MAX_IDS_PER_KERNEL     = 200;
const Integer  SpiceKernelReader::MAX_COVERAGE_INTERVALS = 200000;


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
//  SpiceKernelReader()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpiceKernelReader class
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
SpiceKernelReader::SpiceKernelReader() :
   SpiceInterface(),
   objectNameSPICE     (NULL),
   naifIDSPICE         (-123456789),
   etSPICE             (0.0),
   referenceFrameSPICE (NULL)
{
}

//------------------------------------------------------------------------------
//  SpiceKernelReader(const SpiceKernelReader &reader)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SpiceKernelReader class, by copying
 * the input object.
 * (copy constructor).
 *
* @param <reader> SpiceKernelReader object to copy.
  */
//------------------------------------------------------------------------------
SpiceKernelReader::SpiceKernelReader(const SpiceKernelReader &reader) :
   SpiceInterface(reader),
   objectNameSPICE      (NULL),
   naifIDSPICE          (reader.naifIDSPICE),
   etSPICE              (reader.etSPICE),
   referenceFrameSPICE  (NULL)
{
}

//------------------------------------------------------------------------------
//  SpiceKernelReader& operator=(const SpiceKernelReader &reader)
//------------------------------------------------------------------------------
/**
 * This method copies the data from the input object to the object.
 *
 * @param <reader> the SpiceKernelReader object whose data to assign to "this"
 *                 SpiceKernelReader.
 *
 * @return "this" SpiceKernelReader with data of input SpiceKernelReader reader.
 */
//------------------------------------------------------------------------------
SpiceKernelReader& SpiceKernelReader::operator=(const SpiceKernelReader &reader)
{
   if (&reader == this)
      return *this;
   SpiceInterface::operator=(reader);
   objectNameSPICE     = reader.objectNameSPICE;
   naifIDSPICE         = reader.naifIDSPICE;
   etSPICE             = reader.etSPICE;
   referenceFrameSPICE = reader.referenceFrameSPICE;
   return *this;
}


//------------------------------------------------------------------------------
// ~SpiceKernelReader()
//------------------------------------------------------------------------------
/**
 * This method is the destructor for the SpiceKernelReader.
 *
 */
//------------------------------------------------------------------------------
SpiceKernelReader::~SpiceKernelReader()
{
}
