//$Id$
//------------------------------------------------------------------------------
//                              RealMatData
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
// Created: 2016/06/24
//
/**
 * Defines the real data class used to define a container to hold strings in a 
 * way that the MathWorks mat writer understands
 */
//------------------------------------------------------------------------------

#ifndef RealMatData_hpp
#define RealMatData_hpp

#include "matlabinterface_defs.hpp"
#include "mat.h"
#include <vector>
#include "gmatdefs.hpp"
#include "MatData.hpp"

/*
 * Container for real data to be written to .mat files
 */
class MATLAB_API RealMatData : public MatData
{
public:
   RealMatData(const std::string &variable_name);
   virtual ~RealMatData();
   RealMatData(const RealMatData &rd);
   RealMatData& operator=(const RealMatData &rd);

   virtual bool AddData(const Matrix &data, bool isJaggedData = false);
   virtual bool AddData(const std::vector<Matrix> &data);
   virtual void WriteData(MATFile *pmat, const std::string &obj_name,
        mxArray *mat_struct, mwIndex index = 0U);
   virtual bool WriteData();

protected:
   /// The data array to be written
   std::vector< RealArray> realData;
   /// The 3D data array to be written
   std::vector< Matrix> realData3D;

   /// The data
   double *data_tmp;
};

#endif
