//$Id$
//------------------------------------------------------------------------------
//                           DataWriter
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
 * Base class for DataWriter objects, generally coded in plugins
 */
//------------------------------------------------------------------------------

#ifndef DataWriter_hpp
#define DataWriter_hpp

#include "utildefs.hpp"
#include "WriterData.hpp"


/**
 * Base class for DataWriter objects, generally coded in plugins
 *
 * This class defines the interfaces used in DataWriter derived classes.  The
 * code was put in place to support .mat file writing.  The MatWriter class, in
 * GMAT's MATLAB interface plugin, uses this base class to define its
 * interfaces, which are then used in the Estimation plugin to write out
 * navigation data used to diagnose issues in estimation processes.
 */
class GMATUTIL_API DataWriter
{
public:
   DataWriter();
   virtual ~DataWriter();
   DataWriter(const DataWriter& dw);
   DataWriter& operator=(const DataWriter& dw);

   /**
    * Method used to construct an empty data container compatible with the writer
    *
    * @param ofType The data type the container needs to support
    * @param withName The name of the variable associated with the container
    *
    * @return A pointer to a newly created container.  The caller owns this
    *         container until it is handed back to the writer via a call to
    *         AddData.
    */
   virtual WriterData* GetContainer(const Gmat::ParameterType ofType,
         const std::string &withName) = 0;

   virtual bool Initialize(const std::string &name, const std::string &rev);
   virtual bool OpenFile();
   virtual bool WriteData(const std::string &obj_name) = 0;
   virtual bool CloseFile();
   virtual bool AddDataBlock();
   virtual bool AddData(WriterData * MatDataContainer, UnsignedInt index = 0U);
//   virtual WriterData *GetDataContainer(const std::string &dataName);
   virtual bool DescribeData (const StringArray &variableList, UnsignedInt size = 1U);
   virtual bool ClearData();

protected:
   /// vector of variable names to write to the .mat file
   std::vector<std::string> variable_names;
   /// vector of vector of WriterData objects
   std::vector<std::vector<WriterData*>> allData;

   /// Name of the output file
   std::string filename;
   /// Format of the output file, for types that need it
   std::string format;
};

#endif /* DataWriter_hpp */
