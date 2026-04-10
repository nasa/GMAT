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
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 11, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#include "WriterData.hpp"

WriterData::WriterData(const std::string &variable_name) :
   varName        (variable_name),
   dataType       (Gmat::UNKNOWN_PARAMETER_TYPE),
   isJagged       (false)
{
}

WriterData::~WriterData()
{
}

WriterData::WriterData(const WriterData &md) :
   varName        (md.varName),
   dataType       (md.dataType),
   isJagged       (md.isJagged)
{
}

WriterData& WriterData::operator=(const WriterData &md)
{
   if (this != &md)
   {
      // Data types should already match; no assignment across types
      if (dataType == md.dataType)
      {
         varName   = md.varName;
         isJagged  = md.isJagged;
      }
   }

   return *this;
}

bool WriterData::AddData(const StringMatrix &data, bool isJagged)
{
   return false;
}

bool WriterData::AddData(const Matrix &data, bool isJagged)
{
   return false;
}

bool WriterData::AddData(const std::vector<StringMatrix> &data)
{
   return false;
}

bool WriterData::AddData(const std::vector<Matrix> &data)
{
   return false;
}

std::string WriterData::GetName()
{
   return varName;
}

Gmat::ParameterType WriterData::GetType()
{
   return dataType;
}
