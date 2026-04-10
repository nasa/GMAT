//$Id$
//------------------------------------------------------------------------------
//                                  GravityFileUtil
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/08/15
//
/**
 * Definition of the GravityFileUtil class.  This is the code that reads the different
 * types of gravity files, such as .cof, .dat, .grv files.
 * NOTE: this is a static class
 *
 */
//------------------------------------------------------------------------------
#ifndef GravityFileUtil_hpp
#define GravityFileUtil_hpp

#include "utildefs.hpp"

namespace GmatGrav
{
   enum GravityFileType
   {
      GFT_COF = 1,
      GFT_DAT,
      GFT_GRV,
      GFT_UNKNOWN = -1
   };

   enum GravityModelType
   {
      // Earth Models
      GFM_EGM96,
      GFM_JGM2,
      GFM_JGM3,
      // Luna Models
      GFM_LP165P,
      // Mars Models
//      GFM_GMM1,
//      GFM_GMM2B,
      GFM_MARS50C,
      // Venus Models
//      GFM_MGN75HSAAP,
      GFM_MGNP180U,
      // Other or unknown models
      GFM_OTHER,
      GFM_NONE,
      NumGravityModels
   };

}


class GMATUTIL_API GravityFileUtil
{
public:

   /// dimension normalized harmonic coefficients
   static const Integer DEG_DIM = 361;
   /// dimension of coefficient drifts per year
   static const Integer DRF_DIM = 17;
   /// number of Earth Gravity Models
   static const Integer NUM_EARTH_MODELS = 3;
   /// number of Luna Gravity Models
   static const Integer NUM_LUNA_MODELS = 1;
   /// number of Mars Gravity Models
   static const Integer NUM_MARS_MODELS = 1;   // 3;
   /// number of Venus Gravity Models
   static const Integer NUM_VENUS_MODELS = 1;  // 2;
   /// number of Gravity Models for other bodies
   static const Integer NUM_OTHER_MODELS = 0;

   /// names of the gravity models
   static const std::string GRAVITY_MODEL_NAMES[GmatGrav::NumGravityModels];

   
   static GmatGrav::GravityFileType  GetFileType(const char *filename);
   static GmatGrav::GravityFileType  GetFileType(const std::string &filename);
   
   /// Method to return the gravity model name for a file with format COF only
   static GmatGrav::GravityModelType GetModelType(const char *filename, const char *forBody);
   static GmatGrav::GravityModelType GetModelType(const std::string &filename, const std::string &forBody);
   
   static bool GetFileInfo(const std::string &filename, Integer& degree, Integer& order,
                           Real &mu, Real &radius);
   static bool ReadFile(const std::string &filename, Integer& degree, Integer& order,
                        Real &mu, Real &radius, bool readCoeff,
                        Real cbar[][DEG_DIM] = NULL, Real sbar[][DEG_DIM] = NULL,
                        Real dcbar[][DRF_DIM] = NULL, Real dsbar[][DRF_DIM] = NULL,
                        Integer maxDegree = 360, Integer maxOrder = 360,
                        Integer maxDriftDegree = 2);
   
protected:

   static bool ReadCofFile(const std::string &filename, Integer& degree, Integer& order,
                           Real &mu, Real &radius, bool readCoeff, Real cbar[][DEG_DIM],
                           Real sbar[][DEG_DIM], Integer maxDegree, Integer maxOrder,
                           Integer maxDriftDegree = 0);
   static bool ReadDatFile(const std::string &filename, Integer& degree, Integer& order,
                           Real &mu, Real &radius, bool readCoeff, Real cbar[][DEG_DIM],
                           Real sbar[][DEG_DIM], Real dcbar[][DRF_DIM], Real dsbar[][DRF_DIM],
                           Integer maxDegree, Integer maxOrder, Integer maxDriftDegree);
   static bool ReadGrvFile(const std::string &filename, Integer& degree, Integer& order,
                           Real &mu, Real &radius, bool readCoeff, Real cbar[][DEG_DIM],
                           Real sbar[][DEG_DIM], Integer maxDegree, Integer maxOrder,
                           Integer maxDriftDegree = 0);

private:
   GravityFileUtil();
   GravityFileUtil(const GravityFileUtil &copy);
   const GravityFileUtil& operator=(const GravityFileUtil &right);
   virtual ~GravityFileUtil();

};

#endif // GravityFileUtil_hpp
