//$Id$
//------------------------------------------------------------------------------
//                           GroundstationInterface
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
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2013/01/09
/**
 * Proxy code for Groundstation objects, used to define interfaces implemented
 * in the Groundstation plugin.
 */
//------------------------------------------------------------------------------

#include "GroundstationInterface.hpp"


//------------------------------------------------------------------------------
// GroundstationInterface(const std::string &itsTypeName,
//       const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param itsTypeName The type name of the new object
 * @param itsName The name of the new object
 */
//------------------------------------------------------------------------------
GroundstationInterface::GroundstationInterface(const std::string &itsTypeName,
       const std::string &itsName) :
   BodyFixedPoint    (itsTypeName, itsName, Gmat::GROUND_STATION)
{
   objectTypes.push_back(Gmat::GROUND_STATION);
}


//------------------------------------------------------------------------------
// ~GroundstationInterface()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GroundstationInterface::~GroundstationInterface()
{
}


//------------------------------------------------------------------------------
// GroundstationInterface(const GroundstationInterface& gsi)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
GroundstationInterface::GroundstationInterface(
		const GroundstationInterface& gsi) :
   BodyFixedPoint          (gsi)
{
}



//------------------------------------------------------------------------------
// GroundstationInterface& operator=(const GroundstationInterface& gsi)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
GroundstationInterface& GroundstationInterface::operator=(
		const GroundstationInterface& gsi)
{
   if (this != &gsi)
   {
      BodyFixedPoint::operator=(gsi);
   }
   return *this;
}
