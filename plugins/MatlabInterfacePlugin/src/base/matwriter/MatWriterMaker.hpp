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

#ifndef MatWriterMaker_hpp
#define MatWriterMaker_hpp

#include "matlabinterface_defs.hpp"
#include "DataWriterMaker.hpp"

/**
 * The WriterMaker used to make MatWriters
 *
 * The MatWriterMaker is the factory used to make MarWriters.  While following 
 * the factory design pattern, the DataWriter objects are not scriptable 
 * entities, so they are not derived from the GMAT Factory base class.
 */
class MATLAB_API MatWriterMaker : public DataWriterMaker
{
public:
   static MatWriterMaker* Instance();

   virtual DataWriter* CreateDataWriter();
   virtual std::string GetType();

private:
   static MatWriterMaker* instance;

   MatWriterMaker();
   virtual ~MatWriterMaker();
};

#endif /* MatWriterMaker_hpp */
