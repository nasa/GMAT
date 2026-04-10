//------------------------------------------------------------------------------
//                         GmatData
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen. NASA/GSFC
// Created: 2013/07/02
//
/**
 * Class that contains GMAT data record
 *
 * This class is based class for all data record structure used by GMAT.
 */
//------------------------------------------------------------------------------


#include "GmatData.hpp"
#include "EstimationDefs.hpp"


//-----------------------------------------------------------------------------
// ObservationData()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
GmatData::GmatData(const std::string &format):
   dataFormat   (format)
{
}


//-----------------------------------------------------------------------------
// ~ObservationData()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
GmatData::~GmatData()
{
}


//-----------------------------------------------------------------------------
// GmatData(const GmatData& gd):
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param gd The GmatData object that sets the data for the new one
 */
//-----------------------------------------------------------------------------
GmatData::GmatData(const GmatData& gd):
   dataFormat      (gd.dataFormat)
{
}


//-----------------------------------------------------------------------------
// GmatData& operator=(const GmatData& gd)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param gd The GmatData object that sets the data for the this one
 *
 * @return This object, configured to match gd
 */
//-----------------------------------------------------------------------------
GmatData& GmatData::operator=(const GmatData& gd)
{
   if (&gd != this)
   {
      dataFormat              = gd.dataFormat;
   }

   return *this;
}

