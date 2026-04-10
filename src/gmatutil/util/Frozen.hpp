//$Id$
//------------------------------------------------------------------------------
//                           Frozen      
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Evelyn Hull
// Created: 2011/07/29
//
/**
 * Definition of the Frozen Orbit Class
 */
//------------------------------------------------------------------------------

#ifndef Frozen_hpp
#define Frozen_hpp

#include "StringUtil.hpp"

class GMATUTIL_API Frozen
{
public:
   Frozen();
   ~Frozen();

   void CalculateFrozen(Real ALT, bool altVal, Real INC, bool eccVal);

   //accesor functions
   Real GetSMA();
   Real GetALT();
   Real GetECC();
   Real GetINC();
   Real GetPALT();
   Real GetAALT();
   bool IsError();
   std::string GetError();

private:
   struct orbitElements 
   {
      Real SMA;
      Real ALT;
      Real ECC;
      Real INC;
      Real AALT;
      Real PALT;
   };

   orbitElements elements;
   std::string errormsg;
   bool isError;
};

#endif
