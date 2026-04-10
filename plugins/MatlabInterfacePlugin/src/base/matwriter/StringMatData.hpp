//$Id$
//------------------------------------------------------------------------------
//                              StringMatData
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
// number NNG04CI63P
//
// Author: Michael A. Barrucco, Thinking Systems, Inc.
// Created: 2016/6/24
//
/**
 * Defines the string data class used to define a container to hold strings in
 * a way that the MathWorks mat writer understands.
 */
//------------------------------------------------------------------------------

#ifndef StringMatData_hpp
#define StringMatData_hpp

#include "matlabinterface_defs.hpp"
#include "mat.h"
#include <string.h>
#include <vector>
#include "MatData.hpp"

/**
 * Container for string data to write to .mat files
 */
class MATLAB_API StringMatData : public MatData
{
public:
   StringMatData(const std::string &variable_name);
   virtual ~StringMatData();
   StringMatData(const StringMatData &sd);
   StringMatData& operator=(const StringMatData &sd);

   virtual bool AddData(const StringMatrix &data, bool isJagged = false);
   virtual bool AddData(const std::vector<StringMatrix> &data);
   virtual void WriteData(MATFile *matfile, const std::string &objectName,
                          mxArray *mx_struct, mwIndex index = 0U);
   virtual bool WriteData();

protected:
   /// The data array to be written
   StringMatrix stringData;
   /// The 3D data array to be written
   std::vector< StringMatrix> stringData3D;

   /// mxArray to string data
   mxArray *pa_string;
};

#endif
