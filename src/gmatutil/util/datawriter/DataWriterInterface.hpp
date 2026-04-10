//$Id$
//------------------------------------------------------------------------------
//                           DataWriterInterface
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

#ifndef DataWriterInterface_hpp
#define DataWriterInterface_hpp

#include "utildefs.hpp"
#include <map>
#include <string>

#include "DataWriterMaker.hpp"
#include "DataWriter.hpp"


/**
 * Interface singleton used to manage data writers
 */
class GMATUTIL_API DataWriterInterface
{
public:
   static DataWriterInterface* Instance();

   void RegisterWriterMaker(DataWriterMaker *newMaker);
   DataWriter* GetDataWriter(const std::string &ofType);

private:
   /// Singleton pointer to the writer creator
   static DataWriterInterface *instance;

   /// Mapping from types to writers
   std::map<std::string, DataWriterMaker*> writerMap;

   DataWriterInterface();
   virtual ~DataWriterInterface();
};

#endif /* DataWriterInterface_hpp */
