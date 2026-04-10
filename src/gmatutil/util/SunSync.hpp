//$Id$
//------------------------------------------------------------------------------
//                           SunSync      
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
// Created: 2011/07/20
//
/**
 * Definition of the Sun Sync Orbit Class
 */
//------------------------------------------------------------------------------

#ifndef SunSync_hpp
#define SunSync_hpp

#include "StringUtil.hpp"

class GMATUTIL_API SunSync
{
public:
   SunSync();
   SunSync(Real a, Real alt, Real e, Real i, Real rop, Real roa, Real p);
   ~SunSync();

   void CalculateSunSync(bool aVal, Real a, bool altVal, Real alt, bool eVal, 
                         Real e, bool iVal, Real i, bool ropVal, Real RoP, 
                         bool roaVal, Real RoA, bool pVal, Real P);
   
   //accessor methods
   Real GetSMA();
   Real GetALT();
   Real GetECC();
   Real GetINC();
   Real GetROP();
   Real GetROA();
   Real GetP();
   bool IsError();
   std::string GetError();

   Real SolveA(Real e, Real i, bool flag);
   Real SolveE(Real a, Real i, bool flag, bool altFlag = false);
   Real SolveI(Real a, Real e, bool flag, bool roaFlag = false, bool ropFlag = false,
               bool eFlag = true, bool aFlag = true, bool pFlag = false, 
               bool altFlag = false);
   void SolvePAE(Real P, Real i, Real &a, Real &e);
   void SolveRoAAE(Real RoA, Real i, Real &a, Real &e);
   void SolveRoPAE(Real RoP, Real i, Real &a, Real &e);

private:
   struct orbitElements 
   {
      Real SMA;
      Real ALT;
      Real ECC;
      Real INC;
      Real ROP;
      Real ROA;
      Real P;
   };

   orbitElements elements;
   std::string errormsg;
   bool isError;
};

#endif
