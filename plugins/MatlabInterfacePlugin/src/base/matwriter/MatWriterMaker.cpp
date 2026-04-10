//$Id$
//------------------------------------------------------------------------------
//                           MatWriterMaker
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
// Created: Jul 12, 2016
/**
 * Interface used to make MatWriters
 */
//------------------------------------------------------------------------------

#include "MatWriterMaker.hpp"
#include "MatWriter.hpp"

/// Initialize the singleton
MatWriterMaker* MatWriterMaker::instance = NULL;


//------------------------------------------------------------------------------
// MatWriterMaker* Instance()
//------------------------------------------------------------------------------
/**
 * Access method for the MatWriterMaker singleton
 * 
 * @return The singleton
 */
//------------------------------------------------------------------------------
MatWriterMaker* MatWriterMaker::Instance()
{
   if (instance == NULL)
      instance = new MatWriterMaker();

   return instance;
}


//------------------------------------------------------------------------------
// DataWriter* CreateDataWriter()
//------------------------------------------------------------------------------
/**
 * Method used to create a new MatWriter DataWriter.
 *
 * @return A new MatWriter.
 */
//------------------------------------------------------------------------------
DataWriter* MatWriterMaker::CreateDataWriter()
{
   return new MatWriter();
}

//------------------------------------------------------------------------------
// std::string GetType()
//------------------------------------------------------------------------------
/**
 * Method used to identify the type of DataWriter supported by this maker
 */
//------------------------------------------------------------------------------
std::string MatWriterMaker::GetType()
{
   return "MatWriter";
}

//------------------------------------------------------------------------------
// MatWriterMaker()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
MatWriterMaker::MatWriterMaker()
{
}

//------------------------------------------------------------------------------
// ~MatWriterMaker()
//------------------------------------------------------------------------------
/**
 *  Destructor
 */
//------------------------------------------------------------------------------
MatWriterMaker::~MatWriterMaker()
{
}
