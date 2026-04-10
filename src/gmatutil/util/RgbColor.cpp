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
 * Implements RgbColor class and provides conversion between RGB color and
 * unsigned int color. RGBA is reversed for intel storage order.
 */
//------------------------------------------------------------------------------

#include "RgbColor.hpp"
#include "StringUtil.hpp"
#include "ColorDatabase.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"
#include <stdio.h>                 // for sprintf()

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// RgbColor()
//------------------------------------------------------------------------------
RgbColor::RgbColor()
{
   // default to black
   colorType.rgbColor.red = 0;
   colorType.rgbColor.green = 0;
   colorType.rgbColor.blue = 0;
   colorType.rgbColor.alpha = 1; // opaque
   
   colorType.bgrColor.red = 0;
   colorType.bgrColor.green = 0;
   colorType.bgrColor.blue = 0;
   colorType.bgrColor.alpha = 1; // opaque
}

//------------------------------------------------------------------------------
// RgbColor(const Byte red, const Byte green, const Byte blue, const Byte alpha = 0)
//------------------------------------------------------------------------------
RgbColor::RgbColor(const Byte red, const Byte green, const Byte blue, const Byte alpha)
{
   colorType.rgbColor.red = red;
   colorType.rgbColor.green = green;
   colorType.rgbColor.blue = blue;
   colorType.rgbColor.alpha = alpha;
   
   colorType.bgrColor.red = red;
   colorType.bgrColor.green = green;
   colorType.bgrColor.blue = blue;
   colorType.bgrColor.alpha = alpha;
}

//------------------------------------------------------------------------------
// RgbColor(const UnsignedInt intColor)
//------------------------------------------------------------------------------
RgbColor::RgbColor(const UnsignedInt intColor)
{
   colorType.intColor = intColor;
}

//------------------------------------------------------------------------------
// RgbColor(const RgbColor &rgbColor)
//------------------------------------------------------------------------------
RgbColor::RgbColor(const RgbColor &rgbColor)
{
   colorType.rgbColor.red = rgbColor.colorType.rgbColor.red;
   colorType.rgbColor.green = rgbColor.colorType.rgbColor.green;
   colorType.rgbColor.blue = rgbColor.colorType.rgbColor.blue;
   colorType.rgbColor.alpha = rgbColor.colorType.rgbColor.alpha;
   
   colorType.bgrColor.red = rgbColor.colorType.bgrColor.red;
   colorType.bgrColor.green = rgbColor.colorType.bgrColor.green;
   colorType.bgrColor.blue = rgbColor.colorType.bgrColor.blue;
   colorType.bgrColor.alpha = rgbColor.colorType.bgrColor.alpha;
   
   colorType.intColor = rgbColor.colorType.intColor;
}

//------------------------------------------------------------------------------
// RgbColor& operator=(const RgbColor &rgbColor)
//------------------------------------------------------------------------------
RgbColor& RgbColor::operator=(const RgbColor &rgbColor)
{
   if (&rgbColor != this)
   {
      colorType.rgbColor.red = rgbColor.colorType.rgbColor.red;
      colorType.rgbColor.green = rgbColor.colorType.rgbColor.green;
      colorType.rgbColor.blue = rgbColor.colorType.rgbColor.blue;
      colorType.rgbColor.alpha = rgbColor.colorType.rgbColor.alpha;
      
      colorType.bgrColor.red = rgbColor.colorType.bgrColor.red;
      colorType.bgrColor.green = rgbColor.colorType.bgrColor.green;
      colorType.bgrColor.blue = rgbColor.colorType.bgrColor.blue;
      colorType.bgrColor.alpha = rgbColor.colorType.bgrColor.alpha;
      
      colorType.intColor = rgbColor.colorType.intColor;
   }
   return *this;
}

//------------------------------------------------------------------------------
// virtual ~RgbColor()
//------------------------------------------------------------------------------
RgbColor::~RgbColor()
{
}


//------------------------------------------------------------------------------
// UnsignedInt GetIntColor() const
//------------------------------------------------------------------------------
UnsignedInt RgbColor::GetIntColor() const
{
   return colorType.intColor;
}

//------------------------------------------------------------------------------
// Byte Red()
//------------------------------------------------------------------------------
Byte RgbColor::Red()
{
   return colorType.bgrColor.red;
}

//------------------------------------------------------------------------------
// Byte Green()
//------------------------------------------------------------------------------
Byte RgbColor::Green()
{
   return colorType.bgrColor.green;
}

//------------------------------------------------------------------------------
// Byte Blue()
//------------------------------------------------------------------------------
Byte RgbColor::Blue()
{
   return colorType.bgrColor.blue;
}

//------------------------------------------------------------------------------
// Byte Alpha()
//------------------------------------------------------------------------------
Byte RgbColor::Alpha()
{
   return colorType.bgrColor.alpha;
}

//------------------------------------------------------------------------------
// void Set(const Byte red, const Byte green, const Byte blue, const Byte alpha = 0)
//------------------------------------------------------------------------------
/**
 * Sets rgb color
 *
 * @param  red    Red value (0 - 255)
 * @param  green  Green value (0 - 255)
 * @param  blue   Blue value (0 - 255)
 * @param  alpha  Alpha value (0 = Transparent, 255 = Opaque)
 */
//------------------------------------------------------------------------------
void RgbColor::Set(const Byte red, const Byte green, const Byte blue, const Byte alpha)
{
   colorType.bgrColor.red = red;
   colorType.bgrColor.green = green;
   colorType.bgrColor.blue = blue;
   colorType.bgrColor.alpha = alpha;
}

//------------------------------------------------------------------------------
// void Set(const UnsignedInt intColor)
//------------------------------------------------------------------------------
void RgbColor::Set(const UnsignedInt intColor)
{
   colorType.intColor = intColor;
}

// static methods
//------------------------------------------------------------------------------
// static UnsignedInt ToIntColor(const std::string &colorString)
//------------------------------------------------------------------------------
/**
 * Converts color in color name or rgb tuples such as [255 0 0].
 * For rgb tuples, each value must be between 0 and 255.
 *
 * @colorString  Color name or rgb tuples
 * @throws an exception if input is invalid color or invalid rgb tuples
 */
//------------------------------------------------------------------------------
UnsignedInt RgbColor::ToIntColor(const std::string &colorString)
{
   #ifdef DEBUG_INT_COLOR
   MessageInterface::ShowMessage
      ("RgbColor::ToIntColor() entered, colorString = '%s'\n", colorString.c_str());
   #endif
   
   // First figure out if input is a color name
   ColorDatabase *colorDb = ColorDatabase::Instance();
   UnsignedInt intColor;
   
   if (!GmatStringUtil::StartsWith(colorString, "[") &&
       !GmatStringUtil::EndsWith(colorString, "]"))
   {
      if (colorDb->HasColor(colorString))
      {
         intColor = colorDb->GetIntColor(colorString);
         #ifdef DEBUG_INT_COLOR
         MessageInterface::ShowMessage("RgbColor::ToIntColor() returning %06X\n", intColor);
         #endif
         return intColor;
      }
      else
      {
         UtilityException ue;
         ue.SetDetails("The color \"%s\" not found in the color database. "
                       "Available colors are \"%s\"", colorString.c_str(),
                       colorDb->GetAllColorNames().c_str());
         throw ue;
      }
   }
   
   // It is rgb tuples
   UnsignedIntArray intArray = GmatStringUtil::ToUnsignedIntArray(colorString);
   Byte rgb[3];
   bool error = false;
   if (intArray.size() != 3)
   {
      error = true;
   }
   else
   {
      for (UnsignedInt i = 0; i < intArray.size(); i++)
      {
         if (intArray[i] > 255)
         {
            error = true;
            break;
         }
         else
         {
            rgb[i] = intArray[i];
         }
      }
   }
   
   if (error)
   {
      UtilityException ue;
      ue.SetDetails("\"%s\" has invalid RGB color values. Valid color value is "
                    "triplet Integer number between 0 and 255 inside of [ ]",
                    colorString.c_str());
      throw ue;
   }
   else
   {
      RgbColor color = RgbColor(rgb[0], rgb[1], rgb[2]);
      intColor = color.GetIntColor();
      #ifdef DEBUG_INT_COLOR
      MessageInterface::ShowMessage("RgbColor::ToIntColor() returning %06X\n", intColor);
      #endif
      return intColor;
   }
}

//------------------------------------------------------------------------------
// static std::string ToRgbString(const UnsignedInt &intColor)
//------------------------------------------------------------------------------
std::string RgbColor::ToRgbString(const UnsignedInt &intColor)
{
   char buffer[20];
   RgbColor color = RgbColor(intColor);
   sprintf(buffer, "[%d %d %d]", color.Red(), color.Green(), color.Blue());
   return std::string(buffer);
}

