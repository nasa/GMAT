//$Id$
//------------------------------------------------------------------------------
//                              StringMatData
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
 * Defines the string data class used to define a container to hold strings in a 
 * way that the MathWorks mat writer understands
 */
//------------------------------------------------------------------------------

#include "StringMatData.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// StringMatData(const char * variable_name)
//------------------------------------------------------------------------------
/**
 * Constructs the StringMatData object (default constructor).
 *
 * @param variable_name Variable name of the data to be written to the .mat 
 *                      file structured array
 */
//------------------------------------------------------------------------------
StringMatData::StringMatData(const std::string &variable_name) :
   MatData           (variable_name),
   pa_string         (NULL)
{
   dataType = Gmat::STRING_TYPE;
}

//------------------------------------------------------------------------------
// ~StringMatData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
StringMatData::~StringMatData()
{
}

//------------------------------------------------------------------------------
// StringMatWriter(const StringMatData& sd)
//------------------------------------------------------------------------------
/**
 * Constructs a StringMatData object (copy constructor)
 *
 * @param sd StringMatData object to copy
 *
 */
//------------------------------------------------------------------------------
StringMatData::StringMatData(const StringMatData &sd) :
   MatData           (sd),
   pa_string         (NULL)
{
}

//------------------------------------------------------------------------------
// StringMatData& operator=(const StringMatData &sd)
//------------------------------------------------------------------------------
/**
 * Sets one string mat data object to match another
 *
 * @param sd The object that is copied
 *
 * @return This object, configured to match sd
 */
//------------------------------------------------------------------------------
StringMatData& StringMatData::operator=(const StringMatData &sd)
{
   if (this !=&sd)
   {
      MatData::operator=(sd);

      pa_string  = NULL;
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool AddData(StringMatrix data, bool isJaggedData)
//------------------------------------------------------------------------------
/**
 * Sets up an array of input strings in a mxArray for writing.
 *
 * @params data String data to write to the .mat file
 * @params isJaggedData Flag to indicate if the data may be a jagged array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool StringMatData::AddData(const StringMatrix &data, bool isJaggedData)
{
   bool retval = data.size() > 0;
   stringData = data;
   isJagged = isJaggedData;
   numDim = 2U;

   #ifdef DEBUG_ADD_DATA
      MessageInterface::ShowMessage("Adding string data; matrix is %d "
            "arrays big\n", data.size());
   #endif

   return retval;
}

//------------------------------------------------------------------------------
// bool AddData(std::vector< StringMatrix> data)
//------------------------------------------------------------------------------
/**
 * Sets up an array of input strings in a mxArray for writing.
 *
 * @params data String data to write to the .mat file
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool StringMatData::AddData(const std::vector< StringMatrix> &data)
{
   bool retval = data.size() > 0;
   stringData3D = data;
   numDim = 3U;

   #ifdef DEBUG_ADD_DATA
      MessageInterface::ShowMessage("Adding string data; matrix is %d "
            "arrays big\n", data.size());
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// void StringMatDatia::WriteData(MATFile *pmat, const char *obj_name,
//       mxArray *mat_struct, mwIndex index)
//------------------------------------------------------------------------------
/**
 * Writes string data to the open .mat file stream.
 *
 * @params pmat       File handle to .mat file
 * @params obj_name   Name of the structured array in which to write the data
 * @params mat_struct mxArray of structured array in which to write
 * @params index      Index of the structured array element in which to write
 *
 */
//------------------------------------------------------------------------------
void StringMatData::WriteData(MATFile *matfile, const std::string &objectName,
      mxArray *mx_struct, mwIndex index)
{
   pmat      = matfile;
   obj_name  = objectName;
   mat_struct = mx_struct;

   if (numDim == 2U)
   {
      if (isJagged)
      {
         // size of array
         mwSize n_size = stringData.size();

         // init data
         pa_string = mxCreateCellMatrix(1, n_size);
         mwSize m_string = mxGetNumberOfDimensions(pa_string);
         mwIndex *subs = (mwIndex*)mxCalloc(m_string, sizeof(mwIndex));
         mwIndex index;

         for (int i = 0; i < n_size; i++)
         {
            mxArray *tmp_str;

            mwSize m_size = stringData[i].size();

            // init data
            mxArray *pa_string2 = mxCreateCellMatrix(m_size, 1);
            mwSize m_string2 = mxGetNumberOfDimensions(pa_string2);
            mwIndex *subs2 = (mwIndex*)mxCalloc(m_string2, sizeof(mwIndex));
            mwIndex index2;

            for (int j = 0; j < m_size; j++)
            {
               tmp_str = mxCreateString(stringData[i][j].c_str());
               subs2[1] = (mwIndex)(j);
               index2 = mxCalcSingleSubscript(pa_string2, m_size, subs2);
               mxSetCell(pa_string2, subs2[1], tmp_str);
            }

            subs[0] = (mwIndex)(i);
            index = mxCalcSingleSubscript(pa_string, n_size, subs);
            mxSetCell(pa_string, index, pa_string2);
            mxFree(subs2);
         }

         mxFree(subs);
      }
      else
      {
         // size of array
         mwSize m_size = stringData.size();
         mwSize n_size = (m_size == 0) ? 0 : stringData[0].size();

         // init data
         pa_string = mxCreateCellMatrix(m_size, n_size);
         mwSize m_string = mxGetNumberOfDimensions(pa_string);
         mwIndex *subs = (mwIndex*)mxCalloc(m_string, sizeof(mwIndex));
         mwIndex index;

         for (int i = 0; i < m_size; i++)
         {
            mxArray *tmp_str;

            for (int j = 0; j < n_size; j++)
            {
               tmp_str = mxCreateString(stringData[i][j].c_str());
               subs[0] = (mwIndex)(i);
               subs[1] = (mwIndex)(j);
               index = mxCalcSingleSubscript(pa_string, m_size*n_size, subs);
               mxSetCell(pa_string, index, tmp_str);
            }
         }
         mxFree(subs);
      }
   }
   else if (numDim == 3U)
   {
      // size of array
      mwSize idx1_size = stringData3D.size();

      // init data
      pa_string = mxCreateCellMatrix(1, idx1_size);
      mwSize m_string = mxGetNumberOfDimensions(pa_string);
      mwIndex *subs = (mwIndex*)mxCalloc(m_string, sizeof(mwIndex));
      mwIndex index;

      for (int i = 0; i < idx1_size; i++)
      {
         mxArray *tmp_str;

         mwSize idx2_size = stringData3D[i].size();
         mwSize idx3_size = (idx2_size == 0) ? 0 : stringData3D[i][0].size();

         // init data
         mxArray *pa_string2 = mxCreateCellMatrix(idx2_size, idx3_size);
         mwSize m_string2 = mxGetNumberOfDimensions(pa_string2);
         mwIndex *subs2 = (mwIndex*)mxCalloc(m_string2, 2*sizeof(mwIndex));
         mwIndex index2;

         for (int j = 0; j < idx2_size; j++)
         {
            for (int k = 0; k < idx3_size; k++)
            {
               tmp_str = mxCreateString(stringData3D[i][j][k].c_str());
               subs2[0] = (mwIndex)(j);
               subs2[1] = (mwIndex)(k);
               index2 = mxCalcSingleSubscript(pa_string2, idx2_size*idx3_size, subs2);
               mxSetCell(pa_string2, index2, tmp_str);
            }
         }

         subs[1] = (mwIndex)(i);
         index = mxCalcSingleSubscript(pa_string, idx1_size, subs);
         mxSetCell(pa_string, subs[1], pa_string2);
         mxFree(subs2);
      }

      mxFree(subs);
   }

   int data_field_number = mxGetFieldNumber(mat_struct, varName.c_str());
   mxSetFieldByNumber(mat_struct, index, data_field_number, pa_string);
   matPutVariable(pmat, obj_name.c_str(), mat_struct);
}

//------------------------------------------------------------------------------
// bool StringMatData::WriteData()
//------------------------------------------------------------------------------
/**
 * Override for abstract method in the base class. Currently just returns false.
 *
 * @return false always.  MatWriters don't use this base class version.
 */
//------------------------------------------------------------------------------
bool StringMatData::WriteData()
{
   return false;
}
