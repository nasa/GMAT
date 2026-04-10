//$Id$
//------------------------------------------------------------------------------
//                             RgbColor
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
// Created: 2004/06/02
//
/**
 * Declares RgbColor class and provides conversion between RGB color and
 * unsigned int color.
 */
//------------------------------------------------------------------------------
#ifndef RgbColor_hpp
#define RgbColor_hpp

#include "utildefs.hpp"

class GMATUTIL_API RgbColor
{
public:

   RgbColor();
   RgbColor(const Byte red, const Byte green, const Byte blue, const Byte alpha = 0);
   RgbColor(const UnsignedInt intColor);
   RgbColor(const RgbColor &rgbColor);
   RgbColor& operator=(const RgbColor &rgbColor);
   virtual ~RgbColor();
   
   UnsignedInt GetIntColor() const;
   Byte Red();
   Byte Green();
   Byte Blue();
   Byte Alpha();
   
   void Set(const Byte red, const Byte green, const Byte blue, const Byte alpha = 0);
   void Set(const UnsignedInt rgb);
   
   // static methods
   static UnsignedInt ToIntColor(const std::string &colorString);
   static std::string ToRgbString(const UnsignedInt &intColor);
   
private:
   struct RgbType
   {
      unsigned red      : 8;
      unsigned green    : 8;
      unsigned blue     : 8;
      unsigned alpha    : 8;
   } rgbType;
   
   // Reverse for intel storage order
   struct BgrType
   {
      unsigned blue     : 8;
      unsigned green    : 8;
      unsigned red      : 8;
      unsigned alpha    : 8;
   } bgrType;
   
   union ColorType
   {
      RgbType rgbColor;
      BgrType bgrColor;
      UnsignedInt intColor;
   } colorType;
   
};

#endif
