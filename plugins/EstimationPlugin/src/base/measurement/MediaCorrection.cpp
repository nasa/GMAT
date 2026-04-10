//$Id: MediaCorrection.cpp 67 2010-06-17 21:56:16Z  $
//------------------------------------------------------------------------------
//                         MediaCorrection
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Tuan Dang Nguyen GSFC-NASA
// Created: 2010/06/17
//
/**
 * Media correction model.
 */
//------------------------------------------------------------------------------


#include "MediaCorrection.hpp"


//------------------------------------------------------------------------------
// MediaCorrection(const std::string &typeStr, const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
//------------------------------------------------------------------------------
MediaCorrection::MediaCorrection(const std::string &typeStr,
											const std::string &nomme) :
	MediaCorrectionInterface (typeStr, nomme)
{
   objectTypes.push_back(Gmat::MEDIA_CORRECTION);
   objectTypeNames.push_back("MediaCorrection");
}

//------------------------------------------------------------------------------
// ~MediaCorrection()
//------------------------------------------------------------------------------
/**
 * Donstructor
 */
//------------------------------------------------------------------------------
MediaCorrection::~MediaCorrection()
{
}

//------------------------------------------------------------------------------
// MediaCorrection(const MediaCorrection& mdc)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
MediaCorrection::MediaCorrection(const MediaCorrection& mdc):
//		GmatBase  (mdc)
   MediaCorrectionInterface(mdc)
{
}


//-----------------------------------------------------------------------------
// MediaCorrection& MediaCorrection::operator=(const MediaCorrection& mc)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param mc The MediaCorrection that is provides parameters for this one
 *
 * @return This MediaCorrection, configured to match mc
 */
//-----------------------------------------------------------------------------
MediaCorrection& MediaCorrection::operator=(const MediaCorrection& mc)
{
   if (this != &mc)
   {
      GmatBase::operator=(mc);
   }

   return *this;
}
