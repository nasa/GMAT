//$Id$
//------------------------------------------------------------------------------
//                               NuclearPowerSystem
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
// Author: Wendy C. Shoan
// Created: 2014.04.28
//
/**
 * Base class definition for the NuclearPowerSystem.
 */
//------------------------------------------------------------------------------


#ifndef NuclearPowerSystem_hpp
#define NuclearPowerSystem_hpp

#include "PowerSystem.hpp"

/**
 * Basic nuclear power system model attached to Spacecraft.
 */
class GMAT_API NuclearPowerSystem : public PowerSystem
{
public:

   NuclearPowerSystem(const std::string &nomme);
   virtual ~NuclearPowerSystem();
   NuclearPowerSystem(const NuclearPowerSystem& copy);
   NuclearPowerSystem& operator=(const NuclearPowerSystem& copy);

   virtual bool         Initialize();
   virtual GmatBase*    Clone() const;

   virtual Real         GetPowerGenerated() const; // Total Power

   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// Published parameters for nuclear power systems
   enum
   {
      NuclearPowerSystemParamCount = PowerSystemParamCount,
   };

};

#endif // NuclearPowerSystem_hpp
