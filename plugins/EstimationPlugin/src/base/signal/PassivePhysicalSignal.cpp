//------------------------------------------------------------------------------
//                           PassivePhysicalSignal
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author:
// Created: 
/**
 * Class used to model physical signals that do not have hardware delay
 */
 //------------------------------------------------------------------------------

#include "PassivePhysicalSignal.hpp"

//#define DEBUG_CONSTRUCTION
//#define DEBUG_RANGE_CALCULATION

//------------------------------------------------------------------------------
// PassivePhysicalSignal(const std::string &typeStr, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param typeStr The object type
 * @param name Name of the new object
 */
 //------------------------------------------------------------------------------
PassivePhysicalSignal::PassivePhysicalSignal(const std::string &typeStr,
   const std::string &name) :
   PhysicalSignal(typeStr, name)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PassivePhysicalSignal:: default construction\n");
#endif
}

//------------------------------------------------------------------------------
// ~PassivePhysicalSignal()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
 //------------------------------------------------------------------------------
PassivePhysicalSignal::~PassivePhysicalSignal()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PassivePhysicalSignal default destructor  <%p>\n", this);
#endif
}

//------------------------------------------------------------------------------
// PassivePhysicalSignal(const PassivePhysicalSignal& )
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param pps The signal copied to make this one
 */
 //------------------------------------------------------------------------------
PassivePhysicalSignal::PassivePhysicalSignal(const PassivePhysicalSignal & pps)
   : PhysicalSignal(pps)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PassivePhysicalSignal copy constructor   from <%p> to <%p>\n", &pps, this);
#endif
}

//------------------------------------------------------------------------------
// PassivePhysicalSignal& operator=(const PassivePhysicalSignal& pps)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param pps The signal copied to make this one match it
 *
 * @return This signal made to look like pps
 */
 //------------------------------------------------------------------------------
PassivePhysicalSignal & PassivePhysicalSignal::operator=(const PassivePhysicalSignal& pps)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PassivePhysicalSignal operator =   set <%p> = <%p>\n", this, &pps);
#endif

   if (this != &pps)
   {
      PassivePhysicalSignal::operator=(pps);
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new signal that matches this one
 *
 * @return A new signal set to match this one
 */
 //------------------------------------------------------------------------------
GmatBase * PassivePhysicalSignal::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PassivePhysicalSignal::Clone() clone this <%p>\n", this);
#endif

   return new PassivePhysicalSignal(*this);
}

//------------------------------------------------------------------------------
// HardwareDelayCalculation()
//------------------------------------------------------------------------------
/**
 * This function is used to remove any configured hardware delay for a spacecraft transponder
 */
 //------------------------------------------------------------------------------
bool PassivePhysicalSignal::HardwareDelayCalculation()
{
   // handle regular delay calculation
   PhysicalSignal::HardwareDelayCalculation();

   // if the transmitter is the spacecraft set delay to 0
   if (theData.tNode->IsOfType(Gmat::SPACECRAFT))
   {
      theData.tDelay = 0.0;
   }

   // if the receiver is the spacecraft set delay to 0
   if (theData.rNode->IsOfType(Gmat::SPACECRAFT))
   {
      theData.rDelay = 0.0;
   }

#ifdef DEBUG_RANGE_CALCULATION 
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("   ++++    Passive Hardware delay calculation for leg from %s to %s :\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("     . %s's %s hardware delay   : %.12le sec\n", theData.tNode->GetName().c_str(), ((previous == NULL) ? "" : "half of"), theData.tDelay);
   MessageInterface::ShowMessage("     . %s's %s hardware delay   : %.12le sec\n", theData.rNode->GetName().c_str(), ((next == NULL) ? "" : "half of"), theData.rDelay);
   MessageInterface::ShowMessage("     . Total hardware delay for this signal leg: %.12le sec\n\n", theData.tDelay + theData.rDelay);
#endif
   return true;
}
