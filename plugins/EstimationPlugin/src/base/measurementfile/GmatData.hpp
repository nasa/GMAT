//------------------------------------------------------------------------------
//                         GmatData
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
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen. NASA/GSFC
// Created: 2013/07/02
//
/**
 * Class that contains GMAT data
 *
 * This class is essentially a struct designed to contain GMAT data
 * retrieved from a data stream.
 */
//------------------------------------------------------------------------------


#ifndef GmatData_hpp
#define GmatData_hpp

#include "estimation_defs.hpp"
#include "EstimationDefs.hpp"


/**
 * Class used to set and retrieve GMAT data record.
 */
class ESTIMATION_API GmatData
{
public:
   GmatData(const std::string &format = "");
   virtual ~GmatData();
   GmatData(const GmatData& od);
   GmatData&  operator=(const GmatData& od);

   /// declare abstract functions:
   virtual void   Clear() = 0;

public:
   /// Data format: one of "GMATInternal", "GMAT_OD", "GMAT_ODDoppler" or "GMAT_RampTable"
   std::string dataFormat;

};

#endif /* GmatData_hpp */
