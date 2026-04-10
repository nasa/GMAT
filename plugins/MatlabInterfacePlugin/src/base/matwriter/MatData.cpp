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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Michael A. Barrucco, Thinking Systems, Inc.
// Created: 2016/6/24
//
/**
 * Defines the base class used for Matlab Data objects used to 
 * write .mat output files (string or numerical data) 
 */
//------------------------------------------------------------------------------

#include "MatData.hpp"


//------------------------------------------------------------------------------
// MatData()
//------------------------------------------------------------------------------
/**
 * Constructs the MatData object (default constructor).
 */
//------------------------------------------------------------------------------
MatData::MatData(const std::string &variable_name) :
   WriterData        (variable_name),
   pmat              (NULL),
   obj_name          (""),
   mat_struct        (NULL),
   numDim            (0U)
{
}

//------------------------------------------------------------------------------
// MatData()
//------------------------------------------------------------------------------
/**
 * Destroys the mat_struct mxArray (destructor). 
 */
//------------------------------------------------------------------------------
MatData::~MatData()
{
}

//------------------------------------------------------------------------------
// MatData(const MatData& md)
//------------------------------------------------------------------------------
/**
 * Constructs the MatData object (copy constructor)
 *
 * @param mw MatData object to copy
 */
//------------------------------------------------------------------------------
MatData::MatData(const MatData &md) :
   WriterData        (md),
   pmat              (NULL),
   obj_name          (""),
   mat_struct        (NULL),
   numDim            (0U)
{
}

//------------------------------------------------------------------------------
// MatData& operator=(const MatData &md)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param md The MatData object copied here
 *
 * @return This object, matching md
 */
//------------------------------------------------------------------------------
MatData& MatData::operator=(const MatData &md)
{
   if (this == &md)
   {
      WriterData::operator=(md);

      pmat        = NULL;
      obj_name    = md.obj_name;
      mat_struct  = NULL;
      numDim      = 0U;
   }

   return *this;
}
