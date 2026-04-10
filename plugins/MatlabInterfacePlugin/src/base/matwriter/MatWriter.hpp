//$Id$
//------------------------------------------------------------------------------
//                              MatWriter
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
 * Defines the class used for to write MatData objects Matlab Data objects to
 *  .mat files
 */
//------------------------------------------------------------------------------
//

#ifndef MatWriter_hpp
#define MatWriter_hpp

#include "matlabinterface_defs.hpp"
#include "DataWriter.hpp"
#include "WriterData.hpp"

#include <vector>
#include "mat.h"
#include "MatData.hpp"


/*
 * The DataWriter used to write .mat files.
 */
class MATLAB_API MatWriter : public DataWriter
{
public:
   MatWriter();
   virtual ~MatWriter();
   MatWriter(const MatWriter& mw);
   MatWriter& operator=(const MatWriter &mw);

   virtual WriterData* GetContainer(const Gmat::ParameterType ofType,
         const std::string &withName);

   virtual bool Initialize(const std::string &fname, const std::string &mytype = "w6");
   virtual bool OpenFile();
   virtual bool WriteData(const std::string &obj_name);
   virtual bool CloseFile();
   virtual bool DescribeData (const StringArray &variableList, UnsignedInt size = 1U);
   virtual bool ClearData();

private:
   /// handle to .mat file
   MATFile *pmat;
   /** MATLAB data array type. mat_struct defines a MATLAB structured array
    *  in which data gets written. Many structure arrays can be written to
    *  a single file, by setting a new mat_struct array. */
   mxArray *mat_struct;
   
   void SetMxArray(const StringArray &variable_list, mwSize size = 1U);

   void UnsetMxArray();

}; 

#endif
