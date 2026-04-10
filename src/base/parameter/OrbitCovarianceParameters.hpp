//$Id$
//------------------------------------------------------------------------------
//                             File: OrbitCovarianceParameters
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
// Author: John McGreevy
// Created: 2020.07.10
//
/**
 * Implements orbit covariance related parameter classes.
 *    OrbitErrorCovariance
 */
//------------------------------------------------------------------------------
#ifndef OrbitCovarianceParameters_hpp
#define OrbitCovarianceParameters_hpp

#include "gmatdefs.hpp"
#include "OrbitRmat66.hpp"

//==============================================================================
//                              OrbitErrorCovariance
//==============================================================================

class GMAT_API OrbitErrorCovariance : public OrbitRmat66
{
public:

   OrbitErrorCovariance(const std::string &name = "", GmatBase *obj = NULL);
   OrbitErrorCovariance(const OrbitErrorCovariance &copy);
   OrbitErrorCovariance& operator=(const OrbitErrorCovariance &right);
   virtual ~OrbitErrorCovariance();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

#endif //OrbitCovarianceParameters_hpp
