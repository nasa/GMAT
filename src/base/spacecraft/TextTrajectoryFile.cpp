//$Id$
//------------------------------------------------------------------------------
//                              TextTrajectoryFile
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
// ** Legal **
//
// Author: Linda Jun
// Created: 2003/12/03
/**
 * Implements TextTrajectoryFile class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "TextTrajectoryFile.hpp"

//------------------------------------------------------------------------------
// TextTrajectoryFile(const std::string &fileName)
//------------------------------------------------------------------------------
TextTrajectoryFile::TextTrajectoryFile(const std::string &fileName)
{
    mFileName = fileName;
    mDataArray.reserve(GmatTraj::INITIAL_NUM_POINTS);
}

//------------------------------------------------------------------------------
// ~TextTrajectoryFile()
//------------------------------------------------------------------------------
TextTrajectoryFile::~TextTrajectoryFile()
{
    if (mInStream.is_open())
        mInStream.close();
}

//------------------------------------------------------------------------------
// bool Open(const std::string &fileName)
//------------------------------------------------------------------------------
bool TextTrajectoryFile::Open(const std::string &fileName)
{
    if (mFileName == "")
        mFileName = fileName;

    if (mFileName != "")
    {
        if (mInStream.is_open())
            mInStream.close();
        
        mInStream.open(mFileName.c_str());
        if (!mInStream.is_open())
            return false; //or throw exception
    }
    else
    {
        return false; //or throw exception
    }
    
    return true;
}

//------------------------------------------------------------------------------
// void Close()
//------------------------------------------------------------------------------
void TextTrajectoryFile::Close()
{
    if (mInStream.is_open())
        mInStream.close();
}

//------------------------------------------------------------------------------
// TrajectoryArray& GetData()
//------------------------------------------------------------------------------
TrajectoryArray& TextTrajectoryFile::GetData()
{
    if (mInStream.is_open())
    {
        while(!mInStream.eof())
        {
            ReadLine();
        }
    }

    return mDataArray;
}

//------------------------------------------------------------------------------
// bool ReadLine()
//------------------------------------------------------------------------------
bool TextTrajectoryFile::ReadLine()
{
    //loj: how can I read first 4 items and skip the rest of the line
    for (int i=0; i<GmatTraj::NUM_ITEM_IN_LINE; i++)
        mInStream >> mTempData[i];

    mTrajData.time = mTempData[0];
    mTrajData.x = (float)mTempData[1];
    mTrajData.y = (float)mTempData[2];
    mTrajData.z = (float)mTempData[3];
    
    // add to mData
    mDataArray.push_back(mTrajData);
    return true;
}
