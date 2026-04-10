//$Id: NRLMsise00Atmosphere.hpp 9485 2011-07-18 00:11:14Z tdnguye2 $
//------------------------------------------------------------------------------
//                              NRLMsise00Atmosphere
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// number NNG04CC06P
//
// Author: Tuan Dang Nguyen
// Created: 2011/07/18
//
/**
 * The NRLMSISE00 atmosphere
 */
//------------------------------------------------------------------------------


#ifndef NRLMsise00Atmosphere_hpp
#define NRLMsise00Atmosphere_hpp

#include "AtmosphereModel.hpp"
#include "nrlmsise00_defs.hpp"
#include "TimeTypes.hpp"

class NRLMSISE00_API NRLMsise00Atmosphere : public AtmosphereModel
{
public:

   NRLMsise00Atmosphere(const std::string &typeStr, const std::string &name = "");
   virtual ~NRLMsise00Atmosphere();

   NRLMsise00Atmosphere(const NRLMsise00Atmosphere& msise);
   NRLMsise00Atmosphere& operator=(const NRLMsise00Atmosphere& msise);

   bool  Density(Real *position, Real *density,
                     Real epoch = GmatTimeConstants::MJD_OF_J2000,
                     Integer count = 1);

   virtual GmatBase* Clone() const; // inherited from GmatBase
   virtual void      Copy(const GmatBase* orig);
   virtual bool      Initialize();

protected:
   /// Flag to indicate if data comes from a file
   bool                    fileData;
   /// Name of the file
   std::string             fluxfilename;

   Integer                 mass;
};

#endif /* NRLMsise00Atmosphere_hpp */
