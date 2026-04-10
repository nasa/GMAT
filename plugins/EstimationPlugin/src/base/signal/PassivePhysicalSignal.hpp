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

#ifndef PassivePhysicalSignal_hpp
#define PassivePhysicalSignal_hpp

#include "PhysicalSignal.hpp"

class ESTIMATION_API PassivePhysicalSignal : public PhysicalSignal
{
public:
   PassivePhysicalSignal(const std::string &typeStr, const std::string &name = "");
   virtual ~PassivePhysicalSignal();

   PassivePhysicalSignal(const PassivePhysicalSignal& pps);
   PassivePhysicalSignal& operator=(const PassivePhysicalSignal& pps);

   virtual GmatBase* Clone() const;

protected:

   /// This function is used to calculate total hardware delay
   virtual bool   HardwareDelayCalculation();

};

#endif /* PassivePhysicalSignal_hpp */

