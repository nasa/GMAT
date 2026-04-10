//$Id$
//------------------------------------------------------------------------------
//                              RealMatData
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
 * Defines the Real data class used to define a container to hold Reals in a 
 * way that the MathWorks mat writer understands
 *
 */

#include "RealMatData.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// RealMatData(const std::string &variable_name)
//------------------------------------------------------------------------------
/**
 * Constructs the RealMatData object (default constructor).
 *
 * @param variable_name Variable name of the data to be written to the .mat 
 *                      file structured array
 */
//------------------------------------------------------------------------------
RealMatData::RealMatData(const std::string &variable_name) :
   MatData        (variable_name)
{
   dataType = Gmat::REAL_TYPE;
}

//------------------------------------------------------------------------------
// ~RealMatData()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
RealMatData::~RealMatData()
{
}

//------------------------------------------------------------------------------
// RealMatWriter(const RealMatData& rd)
//------------------------------------------------------------------------------
/**
 * Constructs the RealMatData object (copy constructor)
 *
 * @param rd RealMatData object to copy
 */
//------------------------------------------------------------------------------
RealMatData::RealMatData(const RealMatData &rd) :
   MatData        (rd)
{
}

//------------------------------------------------------------------------------
// RealMatData& operator=(const RealMatData &rd)
//------------------------------------------------------------------------------
/**
 * Sets one Real mat data object to match another
 *
 * @param rd The object that is copied
 */
//------------------------------------------------------------------------------
RealMatData& RealMatData::operator=(const RealMatData &rd)
{
   if (this != &rd)
   {
      MatData::operator=(rd);
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool AddData(std::vector< RealArray> data, bool isJaggedData)
//------------------------------------------------------------------------------
/**
 * Sets up an array of input reals for writing to an mxArray/.mat file.
 *
 * @params data Real data to write to the .mat file.
 * @params isJaggedData Flag to indicate if the data may be a jagged array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RealMatData::AddData(const Matrix &data, bool isJaggedData)
{
   bool retval = data.size() > 0;
   realData = data;
   isJagged = isJaggedData;
   numDim = 2U;

   return retval;
}

//------------------------------------------------------------------------------
// bool AddData(std::vector< Matrix> data)
//------------------------------------------------------------------------------
/**
 * Sets up an array of input reals for writing to an mxArray/.mat file.
 *
 * @params data Real data to write to the .mat file.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RealMatData::AddData(const std::vector<Matrix> &data)
{
   bool retval = data.size() > 0;
   realData3D = data;
   numDim = 3U;

   return retval;
}

//------------------------------------------------------------------------------
// void WriteData(MATFile *pmat, const std::string &obj_name, 
//       mxArray *mat_struct, mwIndex index)
//------------------------------------------------------------------------------
/**
 * Writes real data to the open .mat file stream.
 *
 * @params pmat       File handle to .mat file
 * @params obj_name   Name of the structured array in which to write the data
 * @params mat_struct mxArray of structured array in which to write
 * @params index      Index of the structured array element in which to write
 */
//------------------------------------------------------------------------------
void RealMatData::WriteData(MATFile *pmat, const std::string &obj_name,
      mxArray *mat_struct, mwIndex index)
{
   // find place to write the data within the struct
   int data_field_number = mxGetFieldNumber(mat_struct, varName.c_str());
   double *dataPtr = NULL;

   if (numDim == 2U)
   {
      if (isJagged)
      {
         // create cell matrix to house the data
         mxArray *pa1;
         pa1 = mxCreateCellMatrix(1, realData.size());

         // get matrix size
         mwSize n_size = realData.size();

         for (int i = 0; i < n_size; i++)
         {
            mwSize m_size = realData[i].size();

            // create number matrix to house the data
            mxArray *pa2;
            pa2 = mxCreateNumericMatrix(m_size, 1, mxDOUBLE_CLASS, mxREAL);
            dataPtr = (double *) mxGetData(pa2);
            
            for (int j = 0; j < m_size; j++)
            {
               dataPtr[j] = realData[i][j];
            }

            // write the data
            mxSetCell(pa1, i, pa2);
         }

         // write the data
         mxSetFieldByNumber(mat_struct, index, data_field_number, pa1);
         matPutVariable(pmat, obj_name.c_str(), mat_struct);
      }
      else
      {
         // get matrix size
         mwSize m_size = realData.size();
         mwSize n_size = (m_size == 0) ? 0 : realData[0].size();

         // create number matrix to house the data
         mxArray *pa1;
         pa1 = mxCreateNumericMatrix(m_size, n_size, mxDOUBLE_CLASS, mxREAL);
         dataPtr = (double *) mxGetData(pa1);
         
         for (int i = 0; i < m_size; i++)
         {
            for (int j = 0; j < n_size; j++)
            {
               dataPtr[i + m_size*j] = realData[i][j];
            }
         }

         // write the data
         mxSetFieldByNumber(mat_struct, index, data_field_number, pa1);
         matPutVariable(pmat, obj_name.c_str(), mat_struct);
      }
   }
   else if (numDim == 3U)
   {
      // create cell matrix to house the data
      mxArray *pa1;
      pa1 = mxCreateCellMatrix(1, realData3D.size());

      // get matrix size
      mwSize idx1_size = realData3D.size();

      for (int i = 0; i < idx1_size; i++)
      {
         mwSize idx2_size = realData3D[i].size();
         mwSize idx3_size = (idx2_size == 0) ? 0 : realData3D[i][0].size();

         // create number matrix to house the data
         mxArray *pa2;
         pa2 = mxCreateNumericMatrix(idx2_size, idx3_size, mxDOUBLE_CLASS, mxREAL);
         dataPtr = (double *) mxGetData(pa2);
        
         for (int j = 0; j < idx2_size; j++)
         {
            for (int k = 0; k < idx3_size; k++)
            {
               dataPtr[j + idx2_size*k] = realData3D[i][j][k];
            }
         }         
         mxSetCell(pa1, i, pa2);
      }

      // write the data
      mxSetFieldByNumber(mat_struct, index, data_field_number, pa1);
      matPutVariable(pmat, obj_name.c_str(), mat_struct);
   }
}


//------------------------------------------------------------------------------
// bool WriteData()
//------------------------------------------------------------------------------
/**
 * Writes real data to the open .mat file stream.
 *
 * @return false (currently does nothing)
 */
//------------------------------------------------------------------------------
bool RealMatData::WriteData()
{
   return false;
}
