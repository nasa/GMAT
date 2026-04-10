//$Id: ICRFFile.hpp 9513 2012-02-24 21:23:06Z tuandangnguyen $
//------------------------------------------------------------------------------
//                                  ICRFFile.hpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Tuan Nguyen (NASA/GSFC)
// Created: 2012/05/30
//
/**
 * Definition of the ICRFFile class.  This is the code that provides
 * ICRF Euler rotation vector.
 *
 */
//------------------------------------------------------------------------------

#ifndef ICRFFile_hpp
#define ICRFFile_hpp

#include "gmatdefs.hpp"
#include "Rmatrix33.hpp"

class GMAT_API ICRFFile
{
public:
   /// Instance of the class
   static ICRFFile* Instance();
   /// initializes the ICRFFile (reads it and stores the data)
   virtual void Initialize();
   /// finalizes the ICRFFile
   void Finalize();

   // Get ICRF Euler rotation vector for a given epoch
   bool GetICRFRotationVector(Real epoch, Real* icrfRotationVector, Integer dim, Integer order);

   void RotateFromICRFtoFK5(const Real* inState, Real* outState, const Real theEpoch);

   void RotateFromFK5toICRF(const Real* inState, Real* outState, const Real theEpoch);

protected:
   static const Integer MAX_TABLE_SIZE;

   /// name of data file
   std::string          icrfFileName;
   /// full path name of icrf file (It contains the path to the file)
   std::string          icrfFileNameFullPath;
   
   /// a table containing values of independent variable
   Real*                independence;
   /// a table containing dependent vectors
   Real**               dependences;
   /// dimension of dependent vector
   Integer              dimension;
   /// the size of the table
   Integer              tableSz;
   /// number of data points
   Integer              pointsCount;

   /// specify whether the object is initialized or not
   bool isInitialized;

   void AllocateArrays();
   void CleanupArrays();

   void RotationMatrixFromICRFToFK5(const Real theEpoch);
   Rmatrix33 icrfToFK5;
   Real lastIcrfToFk5Epoch;

private:
   // default constructor
   ICRFFile(const std::string &fileName = "ICRF_Table.txt", const Integer dim = 1);
   // copy constructor - NOT IMPLEMENTED
   ICRFFile(const ICRFFile &icrfFile);
   // operator = - NOT IMPLEMENTED
   const ICRFFile& operator=(const ICRFFile &icrfFile);
   // destructor
   virtual ~ICRFFile();
   
   static ICRFFile *instance;
};
#endif // ICRFFile_hpp
