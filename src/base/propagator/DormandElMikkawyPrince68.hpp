//$Id$
//------------------------------------------------------------------------------
//                             dormandelmikkawyprince68
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
// *** File Name : dormandelmikkawyprince68.h
// *** Created   : January 28, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 1/28/03 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//                                    Changes
//                           : 02/21/2004 - L. Ruley, Missions Applications Branch
//                                            - Updated style using GMAT cpp style guide
//                                            - All Real types to Real types
//                            - All int types to Integer types
// **************************************************************************


#ifndef DormandElMikkawyPrince68_hpp
#define DormandElMikkawyPrince68_hpp


#include "RungeKuttaNystrom.hpp"


/** \brief The 6(8) Nystrom integrator found by Dormand, El-Mikkawy and Prince

  This class implements the Runge-Kutta-Nystrom integrator from the 1987 paper
  by Dormand, El-Mikkawy and Prince, with corrections noted 1n 1991.  The 
  integrator is a 9-stage Nystrom integrator, with error control on both the 
  dependent variables and their derivatives.
 */
class GMAT_API DormandElMikkawyPrince68 :  public RungeKuttaNystrom
{
public:
//    DormandElMikkawyPrince68();
   DormandElMikkawyPrince68(const std::string &nomme = "");
   virtual ~DormandElMikkawyPrince68();

   DormandElMikkawyPrince68(const DormandElMikkawyPrince68& dpn);
   DormandElMikkawyPrince68&   operator=(const DormandElMikkawyPrince68& dpn);

   virtual GmatBase*           Clone() const;

protected:
   virtual void                SetCoefficients();
   
};


#endif
