//$Id$
//------------------------------------------------------------------------------
//                                ColorDatabase
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2013/11/05
//
/**
 * Declares color database class.
 */
//------------------------------------------------------------------------------
#ifndef ColorDatabase_hpp
#define ColorDatabase_hpp

#include "utildefs.hpp"
//#include "RgbColor.hpp"

class RgbColor;

class GMATUTIL_API ColorDatabase
{

public:
   
   static ColorDatabase* Instance();
   
   Integer GetNumColors() const;
   const StringArray& GetAllColorNameArray();
   std::string GetAllColorNames();
   UnsignedIntArray GetAllIntColors();
   
   bool HasColor(const std::string &colorName);
   bool HasColor(UnsignedInt intColor);
   
   UnsignedInt GetIntColor(const std::string &name);
   RgbColor GetRgbColor(const std::string &name);
   std::string GetColorName(UnsignedInt intColor);
   
   void Add(const std::string &name, UnsignedInt intColor);
   void Add(const std::string &name, const RgbColor &rgbColor);
   void Remove(const std::string &name);
   
protected:
private:
   typedef std::map<std::string, UnsignedInt> ColorMap;
   static ColorDatabase *theInstance;
   ColorMap mStringIntColorMap;
   StringArray mColorNames;
   Integer mNumColors;
   
   ColorDatabase();
   ~ColorDatabase();
};
#endif // ColorDatabase_hpp

