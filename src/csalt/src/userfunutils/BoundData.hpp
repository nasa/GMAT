//------------------------------------------------------------------------------
//                              BoundData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Author: Wendy Shoan / NASA
// Created: 2016.01.05
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 *
 * NOTE: This class contains state, control, and time lower and upper bounds
 *
 */
//------------------------------------------------------------------------------

#ifndef BoundData_hpp
#define BoundData_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"

class BoundData
{
public:
   
   BoundData();
   BoundData(const BoundData &copy);
   BoundData& operator=(const BoundData &copy);
   virtual ~BoundData();
   
   virtual void              SetStateLowerBound(const Rvector &bound);
   virtual void              SetStateUpperBound(const Rvector &bound);
   virtual void              SetControlLowerBound(const Rvector &bound);
   virtual void              SetControlUpperBound(const Rvector &bound);
   virtual void              SetTimeLowerBound(const Rvector &bound);
   virtual void              SetTimeUpperBound(const Rvector &bound);
   virtual void              SetStaticLowerBound(const Rvector &bound);
   virtual void              SetStaticUpperBound(const Rvector &bound);
   
   virtual const Rvector&    GetStateLowerBound();
   virtual const Rvector&    GetStateUpperBound();
   virtual const Rvector&    GetControlLowerBound();
   virtual const Rvector&    GetControlUpperBound();
   virtual const Rvector&    GetTimeLowerBound();
   virtual const Rvector&    GetTimeUpperBound();
   virtual const Rvector&    GetStaticLowerBound();
   virtual const Rvector&    GetStaticUpperBound();
   
   
protected:
   
   /// State Lower Bound
   Rvector stateLowerBound;
   /// State Upper Bound
   Rvector stateUpperBound;
   /// Control Lower Bound
   Rvector controlLowerBound;
   /// Control Upper Bound
   Rvector controlUpperBound;
   /// Time Lower Bound
   Rvector timeLowerBound;
   /// Time Upper Bound
   Rvector timeUpperBound;
   /// Static Lower Bound
   Rvector staticLowerBound;
   /// Static Upper Bound
   Rvector staticUpperBound;
   /// Vector used in place of bounds that don't exist
   Rvector emptyBoundVector;

};
#endif // BoundData_hpp
