//$Id$
//------------------------------------------------------------------------------
//                           WriterData
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 11, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef WriterData_hpp
#define WriterData_hpp

#include "utildefs.hpp"
#include <vector>


typedef std::vector<RealArray> Matrix;
typedef std::vector<StringArray> StringMatrix;


/**
 * Base class for the data structures used by DataWriters
 */
class GMATUTIL_API WriterData
{
public:
   WriterData(const std::string &variable_name);
   virtual ~WriterData();
   WriterData(const WriterData &md);
   WriterData& operator=(const WriterData &md);

   virtual bool AddData(const StringMatrix &data, bool isJaggedData = false);
   virtual bool AddData(const Matrix &data, bool isJaggedData = false);
   virtual bool AddData(const std::vector<StringMatrix> &data);
   virtual bool AddData(const std::vector<Matrix> &data);

   virtual std::string GetName();
   virtual Gmat::ParameterType GetType();

   virtual bool WriteData() = 0;

protected:
   /// Name of the data container
   std::string  varName;
   /// Type of the data
   Gmat::ParameterType dataType;
   /// Indicates if the secondary dimension(s) of the data matrix is jagged
   bool isJagged;
};

#endif /* WriterData_hpp */
