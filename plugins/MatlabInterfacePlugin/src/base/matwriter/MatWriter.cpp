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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
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
//

#include "MatWriter.hpp"
#include "RealMatData.hpp"
#include "StringMatData.hpp"

#include "UtilityException.hpp"
#include "MessageInterface.hpp"
#include <sstream>


//------------------------------------------------------------------------------
// MatWriter()
//------------------------------------------------------------------------------
/**
 * Constructs the MatWriter object (default constructor).
 */
//------------------------------------------------------------------------------
MatWriter::MatWriter() :
   DataWriter     (),
   mat_struct     (NULL),
   pmat           (NULL)
{
}

//------------------------------------------------------------------------------
// ~MatWriter()
//------------------------------------------------------------------------------
/**
 * Destroys the mat_struct mxArray (destructor). 
 */
//------------------------------------------------------------------------------
MatWriter::~MatWriter()
{
   // It looks like MATLAB manages the array; leaving this in case we need it
   // if (mat_struct != NULL)
   //    mxDestroyArray(mat_struct);
}

//------------------------------------------------------------------------------
// MatWriter(const MatWriter& mw)
//------------------------------------------------------------------------------
/**
 * Constructs the MatWriter object (copy constructor)
 *
 * @param mw MatWriter object to copy
 */
//------------------------------------------------------------------------------
MatWriter::MatWriter(const MatWriter& mw) :
   DataWriter     (mw),
   mat_struct     (NULL),
   pmat           (NULL)
{
}

//------------------------------------------------------------------------------
// MatWriter& operator=(const MatWriter &mw)
//------------------------------------------------------------------------------
/**
 * Sets one MatWriter object to match another
 *
 * @param <mw> The object that is copied
 *
 */
//------------------------------------------------------------------------------
MatWriter& MatWriter::operator=(const MatWriter &mw)
{
    if (this != &mw)
    {
       DataWriter::operator=(mw);

       allData = mw.allData;
       variable_names = mw.variable_names;
    }

    return *this;
}

//------------------------------------------------------------------------------
// WriterData* GetContainer(const Gmat::ParameterType ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
WriterData* MatWriter::GetContainer(const Gmat::ParameterType ofType,
      const std::string &withName)
{
   WriterData* retval = NULL;

   switch (ofType)
   {
   case Gmat::REAL_TYPE:
      retval = new RealMatData(withName);
      break;

   case Gmat::STRING_TYPE:
      retval = new StringMatData(withName);
      break;

   default:
      break;
   }

   return retval;
}

//------------------------------------------------------------------------------
// void Initialize(const char *filename,  const char *mat_file_rev)
//------------------------------------------------------------------------------
/**
 * Initializes the MatWriter object, opens a file handle to the .mat file
 *
 * @param <filename> string name of the file you wish to open
 * @param <mat_file_rev> optional input arg for the type of .mat file to write. 
 *                       Defaults to version 6 
 *
 */
//------------------------------------------------------------------------------
bool MatWriter::Initialize(const std::string &fname,  const std::string &mytype)
{
   // Set up to open file, ands open it
   filename = fname;
   format = mytype;

   OpenFile();

   return true;
}

//------------------------------------------------------------------------------
// MatWriter::OpenFile(const char *filename,  const char *mat_file_rev)
//------------------------------------------------------------------------------
/**
 * Opens a file handle to the .mat file
 *
 * @param filename     string name of the file you wish to open
 * @param mat_file_rev optional input arg for the type of .mat file to write. 
 *                     Defaults to version 6.
 *
 * @return File handle to .mat file
 */
//------------------------------------------------------------------------------
bool MatWriter::OpenFile()
{
    // check mat_file_rev
   if ( (format != "w4") && (format != "w6") &&
        (format != "w7") && (format != "w7.3") &&
        (format != "w") )
   {
      MessageInterface::ShowMessage("MATLAB Writer .mat version %s invalid; "
            "defaulting to w6\n", format.c_str());
      format = "w6";
   }

   // open file
   pmat = matOpen(filename.c_str(), format.c_str());

   if (pmat == NULL)
   {
       std::stringstream msg;
       msg << "Error creating file " << filename
           << "\n(Check write permissions in the target directory)";
       throw UtilityException(msg.str());
   }

   // return file handle to class
   return (pmat != NULL);
}

//------------------------------------------------------------------------------
// bool WriteData(const char *obj_name)
//------------------------------------------------------------------------------
/**
 * Writes data to .mat file, within the mat_struct structured array
 *
 * @param obj_name name of structured array you want to write to
 *
 * @return Status code. Checks to see if mxArray associated with the 
 *         object requested to write to exists.
 */
//------------------------------------------------------------------------------
bool MatWriter::WriteData(const std::string &obj_name)
{
   if (mat_struct == NULL)
      throw UtilityException("Cannot write MATLAB data: MAT Structure array "
              "not created");

   if (pmat == NULL)
      throw UtilityException("Cannot write MATLAB data: MATFile was not openned yet");

   for (unsigned int i = 0; i < allData.size(); ++i)
      for (unsigned int j = 0; j < allData[i].size(); ++j)
          ((MatData*)(allData[i][j]))->WriteData(pmat, obj_name, mat_struct, i);

   return true;
}

//------------------------------------------------------------------------------
// bool CloseFile()
//------------------------------------------------------------------------------
/**
 * Close the stream to the .mat file
 *
 * @return Status code. Checks to see if closing was successful
 */
//------------------------------------------------------------------------------
bool MatWriter::CloseFile()
{
   // close file
   if (pmat)
   {
      if (matClose(pmat) != 0)
         throw UtilityException("MATLAB Writer: Error closing .mat file");
      pmat = NULL;
   }
   return true;
}

//------------------------------------------------------------------------------
// bool DescribeData (const StringArray &variableList, UnsignedInt size)
//------------------------------------------------------------------------------
/**
 * Initialization routine used to decribe the incoming data
 *
 * @param variableList The names of the incoming data containers
 * @param size Number of elements in the structure array
 *
 * @return true
 */
//------------------------------------------------------------------------------
bool MatWriter::DescribeData(const StringArray &variableList, UnsignedInt size)
{
   SetMxArray(variableList, size);
   return true;
}

//------------------------------------------------------------------------------
// void SetMxArray(std::vector<std::string> variable_list, mwSize size)
//------------------------------------------------------------------------------
/**
 * Initializes the structured array that all the data will get written to.
 * data format will be mat_struct.variable 
 *
 * @param variable_list list of variable names to write to the structured array
 * @param size Number of elements in the structure array
 */
//------------------------------------------------------------------------------
void MatWriter::SetMxArray(const StringArray &variable_list, mwSize size)
{
   // get number of fields
   int number_of_fields = variable_list.size();

   // convert to cstrings
   const char **fields;
   fields = new const char*[number_of_fields];

   for (int i = 0; i < number_of_fields; i++)
      fields[i] = variable_list[i].c_str();

   // create structured array handle
   mat_struct = mxCreateStructMatrix(size, 1, number_of_fields, fields);

   delete [] fields;
}

//------------------------------------------------------------------------------
// void UnsetMxArray()
//------------------------------------------------------------------------------
/**
 * Cleans up the structured array that all data gets written to. This will cascade down
 * to all sub-arrays.
 *
 */
 //------------------------------------------------------------------------------
void MatWriter::UnsetMxArray() {
   if (mat_struct) {
      mxDestroyArray(mat_struct);
      mat_struct = NULL;
   }
}

//-------------------------------------------------------------------------------------
// bool ClearData()
//-------------------------------------------------------------------------------------
/**
 * Clears the vector of WriterData objects
 */
 //-------------------------------------------------------------------------------------
bool MatWriter::ClearData()
{
   bool retval = DataWriter::ClearData();
   UnsetMxArray();
   return retval;
}
