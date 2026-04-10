//$Id$
//------------------------------------------------------------------------------
//                           DataBucket
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
// Created: Jul 20, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#include "DataBucket.hpp"
#include <algorithm>


//------------------------------------------------------------------------------
// DataBucket()
//------------------------------------------------------------------------------
/**
 * Constructs the DataBucket object (default constructor).
 */
//------------------------------------------------------------------------------
DataBucket::DataBucket() :
   fillToMatch          (true),
   initialRealValue     (-1),
   initialStringValue   ("N/A")
{
}

//------------------------------------------------------------------------------
// ~DataBucket()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DataBucket::~DataBucket()
{
}

//------------------------------------------------------------------------------
// StringMatWriter(const DataBucket& db)
//------------------------------------------------------------------------------
/**
 * Constructs a DataBucket object (copy constructor)
 *
 * @param db DataBucket object to copy
 *
 */
//------------------------------------------------------------------------------
DataBucket::DataBucket(const DataBucket& db) :
   elementStatus        (db.elementStatus),
   realNames            (db.realNames),
   realValues           (db.realValues),
   realValueSize        (db.realValueSize),
   real3DNames          (db.real3DNames),
   real3DValues         (db.real3DValues),
   real2DArrayNames     (db.real2DArrayNames),
   real2DArrayValues    (db.real2DArrayValues),
   stringNames          (db.stringNames),
   stringValues         (db.stringValues),
   stringValueSize      (db.stringValueSize),
   string3DNames        (db.string3DNames),
   string3DValues       (db.string3DValues),
   string2DArrayNames   (db.string2DArrayNames),
   string2DArrayValues  (db.string2DArrayValues),
   fillToMatch          (db.fillToMatch),
   initialRealValue     (db.initialRealValue),
   initialStringValue   (db.initialStringValue)
{
}

//------------------------------------------------------------------------------
// DataBucket& operator=(const DataBucket &db)
//------------------------------------------------------------------------------
/**
 * Sets one DataBucket object to match another
 *
 * @param db The object that is copied
 *
 * @return This object, configured to match db
 */
//------------------------------------------------------------------------------
DataBucket& DataBucket::operator =(const DataBucket& db)
{
   if (this != &db)
   {
      elementStatus       = db.elementStatus;
      realNames           = db.realNames;
      realValues          = db.realValues;
      realValueSize       = db.realValueSize;
      real3DNames         = db.real3DNames;
      real3DValues        = db.real3DValues;
      real2DArrayNames    = db.real2DArrayNames;
      real2DArrayValues   = db.real2DArrayValues;
      stringNames         = db.stringNames;
      stringValues        = db.stringValues;
      stringValueSize     = db.stringValueSize;
      string3DNames       = db.string3DNames;
      string3DValues      = db.string3DValues;
      string2DArrayNames  = db.string2DArrayNames;
      string2DArrayValues = db.string2DArrayValues;
      fillToMatch         = db.fillToMatch;
      initialRealValue    = db.initialRealValue;
      initialStringValue  = db.initialStringValue;
   }

   return *this;
}

//-------------------------------------------------------------------------------------
// void SetFillToMatch(bool matchFill)
//-------------------------------------------------------------------------------------
/**
 * Set if a container should have its values filled to match the size of existing containers
 *
 * @param val The value to set
 */
//-------------------------------------------------------------------------------------
void DataBucket::SetFillToMatch(bool matchFill)
{
   fillToMatch = matchFill;
}

//-------------------------------------------------------------------------------------
// void SetInitialRealValue(Real val)
//-------------------------------------------------------------------------------------
/**
 * Set the initial value of Reals in a Real container
 *
 * @param val The value to set
 */
//-------------------------------------------------------------------------------------
void DataBucket::SetInitialRealValue(Real val)
{
   initialRealValue = val;
}

//-------------------------------------------------------------------------------------
// void SetInitialStringValue(std::string val)
//-------------------------------------------------------------------------------------
/**
 * Set the initial value of stings in a string container
 *
 * @param val The value to set
 */
//-------------------------------------------------------------------------------------
void DataBucket::SetInitialStringValue(std::string val)
{
   initialStringValue = val;
}

//-------------------------------------------------------------------------------------
// Integer AddRealContainer(const std::string& name, UnsignedInt numElements)
//-------------------------------------------------------------------------------------
/**
 * Adds a 2D container for Real data types. The size of one dimension of the 2D array
 * is specified by the numElements parameter. The other dimension's size is equal to
 * the size of elementStatus, which is incremented by one each time
 * DataBucket::AddPoint() is called.
 *
 * @param name The name of the container
 * @param numElements Size of one dimension of the container
 *
 * @return the index of the container that was added
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::AddRealContainer(const std::string& name, UnsignedInt numElements)
{
   Integer retval = -1;

   if (find(realNames.begin(), realNames.end(), name) == realNames.end())
   {
      std::vector<RealArray> data;
      RealArray dataVec;

      if (fillToMatch)
      {
         dataVec.assign(numElements, initialRealValue);
         data.assign(elementStatus.size(), dataVec);
      }

      realNames.push_back(name);
      realValues.push_back(data);
      realValueSize.push_back(numElements);
      retval = realValues.size() - 1;
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// Integer FindRealContainer(const std::string& name)
//-------------------------------------------------------------------------------------
/**
 * Find the index of an existing Real container.
 *
 * @param name The name of the container
 *
 * @return the index of the container. Returns -1 if no container with that name is found
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::FindRealContainer(const std::string& name)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < realNames.size(); ++i)
   {
      if (realNames[i] == name)
      {
         retval = i;
         break;
      }
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// Integer GetRealContainerSize(UnsignedInt index)
//-------------------------------------------------------------------------------------
/**
 * Get the size of an existing Real container.
 *
 * @param index The index of the container
 *
 * @return the index of the container. Returns -1 if an invalid index is provided
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::GetRealContainerSize(UnsignedInt index)
{
   Integer retval = -1;

   if (index >= 0 && index < realValueSize.size())
      retval = realValueSize[index];

   return retval;
}

//-------------------------------------------------------------------------------------
// Integer AddReal3DContainer(const std::string& name)
//-------------------------------------------------------------------------------------
/**
 * Adds a 3D container for Real data types. The size of the first dimension of the 3D
 * array is equal to the size of elementStatus, which is incremented by one each time
 * DataBucket::AddPoint() is called.
 *
 * @param name The name of the container
 *
 * @return the index of the container that was added
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::AddReal3DContainer(const std::string& name)
{
   Integer retval = -1;

   if (find(real3DNames.begin(), real3DNames.end(), name) == real3DNames.end())
   {
      std::vector<std::vector<RealArray>> data;
      std::vector<RealArray> dataVec;

      if (fillToMatch)
      {
         data.assign(elementStatus.size(), dataVec);
      }

      real3DNames.push_back(name);
      real3DValues.push_back(data);
      retval = real3DValues.size() - 1;
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// Integer FindReal3DContainer(const std::string& name)
//-------------------------------------------------------------------------------------
/**
 * Find the index of an existing Real 3D container.
 *
 * @param name The name of the container
 *
 * @return the index of the container. Returns -1 if no container with that name is found
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::FindReal3DContainer(const std::string& name)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < real3DNames.size(); ++i)
   {
      if (real3DNames[i] == name)
      {
         retval = i;
         break;
      }
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// Integer AddReal2DArray(const std::string& name)
//-------------------------------------------------------------------------------------
/**
 * Adds a 2D array for Real data types. The dimensions of this array are not affected
 * when DataBucket::AddPoint() is called.
 *
 * @param name The name of the 2D array
 *
 * @return the index of the 2D array that was added
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::AddReal2DArray(const std::string& name)
{
   Integer retval = -1;

   if (find(real2DArrayNames.begin(), real2DArrayNames.end(), name) == real2DArrayNames.end())
   {
      std::vector<RealArray> data;

      real2DArrayNames.push_back(name);
      real2DArrayValues.push_back(data);
      retval = real2DArrayValues.size() - 1;
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// Integer FindReal2DArray(const std::string& name)
//-------------------------------------------------------------------------------------
/**
 * Find the index of an existing Real 2D array.
 *
 * @param name The name of the 2D array
 *
 * @return the index of the 2D array. Returns -1 if no 2D array with that name is found
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::FindReal2DArray(const std::string& name)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < real2DArrayNames.size(); ++i)
   {
      if (real2DArrayNames[i] == name)
      {
         retval = i;
         break;
      }
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// bool AddStringContainer(const std::string& name, UnsignedInt numElements)
//-------------------------------------------------------------------------------------
/**
 * Adds a 2D container for String data types. The size of one dimension of the 2D array
 * is specified by the numElements parameter. The other dimension's size is equal to
 * the size of elementStatus, which is incremented by one each time
 * DataBucket::AddPoint() is called.
 *
 * @param name The name of the container
 * @param numElements Size of one dimension of the container
 *
 * @return the index of the container that was added
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::AddStringContainer(const std::string& name, UnsignedInt numElements)
{
   Integer retval = -1;

   if (find(stringNames.begin(), stringNames.end(), name) == stringNames.end())
   {
      std::vector<StringArray> data;
      StringArray dataVec;

      if (fillToMatch)
      {
         dataVec.assign(numElements, initialStringValue);
         data.assign(elementStatus.size(), dataVec);
      }

      stringNames.push_back(name);
      stringValues.push_back(data);
      stringValueSize.push_back(numElements);
      retval = stringValues.size() - 1;
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// Integer FindStringContainer(const std::string& name)
//-------------------------------------------------------------------------------------
/**
 * Find the index of an existing String container.
 *
 * @param name The name of the container
 *
 * @return the index of the container. Returns -1 if no container with that name is found
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::FindStringContainer(const std::string& name)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < stringNames.size(); ++i)
   {
      if (stringNames[i] == name)
      {
         retval = i;
         break;
      }
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// Integer GetStringContainerSize(UnsignedInt index)
//-------------------------------------------------------------------------------------
/**
 * Get the size of an existing String container.
 *
 * @param index The index of the container
 *
 * @return the index of the container. Returns -1 if an invalid index is provided
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::GetStringContainerSize(UnsignedInt index)
{
   Integer retval = -1;

   if (index >= 0 && index < stringValueSize.size())
      retval = stringValueSize[index];

   return retval;
}

//-------------------------------------------------------------------------------------
// bool AddString3DContainer(const std::string& name)
//-------------------------------------------------------------------------------------
/**
 * Adds a 3D container for String data types. The size of the first dimension of the 3D
 * array is equal to the size of elementStatus, which is incremented by one each time
 * DataBucket::AddPoint() is called.
 *
 * @param name The name of the container
 *
 * @return the index of the container that was added
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::AddString3DContainer(const std::string& name)
{
   Integer retval = -1;

   if (find(string3DNames.begin(), string3DNames.end(), name) == string3DNames.end())
   {
      std::vector<std::vector<StringArray>> data;
      std::vector<StringArray> dataVec;

      if (fillToMatch)
      {
         data.assign(elementStatus.size(), dataVec);
      }

      string3DNames.push_back(name);
      string3DValues.push_back(data);
      retval = string3DValues.size() - 1;
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// Integer FindString3DContainer(const std::string& name)
//-------------------------------------------------------------------------------------
/**
 * Find the index of an existing String 3D container.
 *
 * @param name The name of the container
 *
 * @return the index of the container. Returns -1 if no container with that name is found
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::FindString3DContainer(const std::string& name)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < string3DNames.size(); ++i)
   {
      if (string3DNames[i] == name)
      {
         retval = i;
         break;
      }
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// bool AddString2DArray(const std::string& name, UnsignedInt numElements)
//-------------------------------------------------------------------------------------
/**
 * Adds a 2D array for String data types. The size of one dimension of the 2D array
 * is specified by the numElements parameter. The other dimension's size is equal to
 * the size of elementStatus, which is incremented by one each time
 * DataBucket::AddPoint() is called.
 *
 * @param name The name of the 2D array
 *
 * @return the index of the 2D array that was added
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::AddString2DArray(const std::string& name)
{
   Integer retval = -1;

   if (find(string2DArrayNames.begin(), string2DArrayNames.end(), name) == string2DArrayNames.end())
   {
      std::vector<StringArray> data;

      string2DArrayNames.push_back(name);
      string2DArrayValues.push_back(data);
      retval = string2DArrayValues.size() - 1;
   }

   return retval;
}

//-------------------------------------------------------------------------------------
// Integer FindString2DArray(const std::string& name)
//-------------------------------------------------------------------------------------
/**
 * Find the index of an existing String 2D array.
 *
 * @param name The name of the 2D array
 *
 * @return the index of the 2D array. Returns -1 if no 2D array with that name is found
 */
//-------------------------------------------------------------------------------------
Integer DataBucket::FindString2DArray(const std::string& name)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < string2DArrayNames.size(); ++i)
   {
      if (string2DArrayNames[i] == name)
      {
         retval = i;
         break;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer AddPoint()
//------------------------------------------------------------------------------
/**
 * Adds a data point to the member containers
 *
 * This method adds a new element to each data array in the bucket, initializing
 * Reals to REAL_INITIAL_VALUE and strings to "N/A"
 *
 * @return The index into the containers, built on the size of the elementStatus
 * container.
 */
//------------------------------------------------------------------------------
Integer DataBucket::AddPoint()
{
   elementStatus.push_back(initialRealValue);

   for (UnsignedInt i = 0; i < realValues.size(); ++i)
   {
      RealArray initialValue;
      initialValue.assign(realValueSize[i], initialRealValue);
      realValues[i].push_back(initialValue);
   }

   for (UnsignedInt i = 0; i < real3DValues.size(); ++i)
   {
      std::vector<RealArray> initialValue;
      real3DValues[i].push_back(initialValue);
   }

   for (UnsignedInt i = 0; i < stringValues.size(); ++i)
   {
      StringArray initialValue;
      initialValue.assign(stringValueSize[i], initialStringValue);
      stringValues[i].push_back(initialValue);
   }

   for (UnsignedInt i = 0; i < string3DValues.size(); ++i)
   {
      std::vector<StringArray> initialValue;
      string3DValues[i].push_back(initialValue);
   }

   return elementStatus.size() - 1;
}


//------------------------------------------------------------------------------
// Integer GetContainerSize()
//------------------------------------------------------------------------------
/**
 * Gets the size of the containers
 *
 * @return The size of the containers, built on the size of the elementStatus
 * container.
 */
//------------------------------------------------------------------------------
Integer DataBucket::GetContainerSize()
{
   return elementStatus.size();
}


//------------------------------------------------------------------------------
// void Clear()
//------------------------------------------------------------------------------
/**
 *
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
void DataBucket::Clear()
{
   elementStatus.clear();

   for (UnsignedInt i = 0; i < realValues.size(); ++i)
   {
      for (UnsignedInt j = 0; j < realValues[i].size(); ++j)
         realValues[i][j].clear();

      realValues[i].clear();
   }

   for (UnsignedInt i = 0; i < real3DValues.size(); ++i)
   {
      for (UnsignedInt j = 0; j < real3DValues[i].size(); ++j)
      {
         for (UnsignedInt k = 0; k < real3DValues[i][j].size(); ++k)
            real3DValues[i][j][k].clear();

         real3DValues[i][j].clear();
      }

      real3DValues[i].clear();
   }

   for (UnsignedInt i = 0; i < real2DArrayValues.size(); ++i)
   {
      for (UnsignedInt j = 0; j < real2DArrayValues[i].size(); ++j)
         real2DArrayValues[i][j].clear();

      real2DArrayValues[i].clear();
   }

   for (UnsignedInt i = 0; i < stringValues.size(); ++i)
   {
      for (UnsignedInt j = 0; j < stringValues[i].size(); ++j)
         stringValues[i][j].clear();

      stringValues[i].clear();
   }

   for (UnsignedInt i = 0; i < string3DValues.size(); ++i)
   {
      for (UnsignedInt j = 0; j < string3DValues[i].size(); ++j)
      {
         for (UnsignedInt k = 0; k < string3DValues[i][j].size(); ++k)
            string3DValues[i][j][k].clear();

         string3DValues[i][j].clear();
      }

      string3DValues[i].clear();
   }

   for (UnsignedInt i = 0; i < string2DArrayValues.size(); ++i)
   {
      for (UnsignedInt j = 0; j < string2DArrayValues[i].size(); ++j)
         string2DArrayValues[i][j].clear();

      string2DArrayValues[i].clear();
   }
}
