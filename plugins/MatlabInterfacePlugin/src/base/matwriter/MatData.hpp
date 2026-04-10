//$Id$
//------------------------------------------------------------------------------
//                              MatData
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 26
//
// Author: Michael A. Barrucco, Thinking Systems, Inc.
// Created: 2016/6/24
//
/**
 * Defines the base class used for Matlab Data objects used to 
 * write .mat output files (string or numerical data) 
 */
//------------------------------------------------------------------------------

#ifndef MatData_hpp
#define MatData_hpp


#include "matlabinterface_defs.hpp"
#include <stdio.h>
#include "mat.h"
#include <iostream>
#include <string.h>
#include <vector>

#include "WriterData.hpp"

/**
 * MATLAB base data structure, used when writing .mat files.
 */
class MATLAB_API MatData : public WriterData
{
public:
   MatData(const std::string &variable_name);
   virtual ~MatData();
   MatData(const MatData &md);
   MatData& operator=(const MatData &md);

   virtual void WriteData(MATFile *pmat, const std::string &obj_name,
         mxArray *mat_struct, mwIndex index) = 0;

protected:
   // Buffered data so WriteData() can call to the parameterized version
   /// MATLAB .mat file receiving the data
   MATFile *pmat;
   /// Name of the object being written
   std::string obj_name;
   /// MATLAB data structure that contains the container receiving data
   mxArray *mat_struct;

   /// Number of dimensions in the array
   UnsignedInt numDim;
};

#endif
