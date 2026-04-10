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
 * Implements color database class.
 */
//------------------------------------------------------------------------------

#include "ColorDatabase.hpp"
#include "ColorTypes.hpp"
#include "UtilityException.hpp"
#include "RgbColor.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_COLORDB_ADD

//---------------------------------
// static data
//---------------------------------
ColorDatabase* ColorDatabase::theInstance = NULL;

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// ColorDatabase* Instance()
//------------------------------------------------------------------------------
ColorDatabase* ColorDatabase::Instance()
{
   if (theInstance == NULL)
      theInstance = new ColorDatabase;
    
   return theInstance;
}

//---------------------------------
// Get methods
//---------------------------------

//------------------------------------------------------------------------------
// Integer GetNumColors() const
//------------------------------------------------------------------------------
/**
 * Retrieves number of colors in the database.
 *
 * @return number of colors
 */
//------------------------------------------------------------------------------
Integer ColorDatabase::GetNumColors() const
{
   return mNumColors;
}


//------------------------------------------------------------------------------
// const StringArray& GetAllColorNameArray() const
//------------------------------------------------------------------------------
/**
 * @return names of colors
 */
//------------------------------------------------------------------------------
const StringArray& ColorDatabase::GetAllColorNameArray()
{
   mColorNames.clear();
   ColorMap::iterator pos;
   
   #ifdef DEBUG_COLORDB
   MessageInterface::ShowMessage
      ("ColorDatabase::GetAllColorNameArray() mStringIntColorMap.size()=%d, "
       "mNumColors=%d\n",  mStringIntColorMap.size(), mNumColors);
   #endif
   
   for (pos = mStringIntColorMap.begin(); pos != mStringIntColorMap.end(); ++pos)
      mColorNames.push_back(pos->first);
   
   return mColorNames;
}


//------------------------------------------------------------------------------
// std::string GetAllColorNames()
//------------------------------------------------------------------------------
std::string ColorDatabase::GetAllColorNames()
{
   StringArray colorArray = GetAllColorNameArray();
   int numColors = colorArray.size();
   std::string colorNames;
   for (int i = 0; i < numColors - 1; i++)
      colorNames = colorNames + colorArray[i] + " ";
   
   colorNames = colorNames + colorArray[numColors-1];
   return colorNames;
}


//------------------------------------------------------------------------------
// UnsignedIntArray GetAllIntColors() const
//------------------------------------------------------------------------------
/**
 * @return array of colors in unsigned int format
 */
//------------------------------------------------------------------------------
UnsignedIntArray ColorDatabase::GetAllIntColors()
{
   UnsignedIntArray colors;
   ColorMap::iterator pos;
   
   for (pos = mStringIntColorMap.begin(); pos != mStringIntColorMap.end(); ++pos)
      colors.push_back(pos->second);
   
   return colors;
}


//------------------------------------------------------------------------------
// bool HasColor(const std::string &colorName)
//------------------------------------------------------------------------------
/**
 * @return true if database has the color name, false otherwise
 */
//------------------------------------------------------------------------------
bool ColorDatabase::HasColor(const std::string &colorName)
{
   bool found = false;
   
   if (mStringIntColorMap.find(colorName) != mStringIntColorMap.end())
      found = true;
   
   return found;
}


//------------------------------------------------------------------------------
// bool HasColor(UnsignedInt intColor))
//------------------------------------------------------------------------------
/**
 * @return true if database has the ginven unsigned int color, false otherwise
 */
//------------------------------------------------------------------------------
bool ColorDatabase::HasColor(UnsignedInt intColor)
{
   bool found = false;
   ColorMap::iterator pos;
   for (pos = mStringIntColorMap.begin(); pos != mStringIntColorMap.end(); ++pos)
   {
      if (pos->second == intColor)
      {
         found = true;
         break;
      }
   }
   
   return found;
}


//------------------------------------------------------------------------------
// UnsignedInt GetIntColor(const std::string &name)
//------------------------------------------------------------------------------
/**
 * @return unsigned int value of the given color name
 */
//------------------------------------------------------------------------------
UnsignedInt ColorDatabase::GetIntColor(const std::string &name)
{
   ColorMap::iterator pos = mStringIntColorMap.find(name);
   if (pos == mStringIntColorMap.end())
      throw UtilityException
         ("ColorDatabase::GetIntColor() Cannot find Color name \"" + name +
          "\" in the Database.  Available colors are \"" + GetAllColorNames() + "\"");
   else
      return pos->second;
}

//------------------------------------------------------------------------------
// RgbColor GetRgbColor(const std::string &name)
//------------------------------------------------------------------------------
/**
 * @return RgbColor of given color name
 */
//------------------------------------------------------------------------------
RgbColor ColorDatabase::GetRgbColor(const std::string &name)
{
   UnsignedInt intColor = GetIntColor(name);
   RgbColor rgbColor(intColor);
   return rgbColor;
}


//------------------------------------------------------------------------------
// std::string GetColorName(UnsignedInt intColor)
//------------------------------------------------------------------------------
/**
 * @return Predefined color name of given unsigned int color
 */
//------------------------------------------------------------------------------
std::string ColorDatabase::GetColorName(UnsignedInt intColor)
{
   ColorMap::iterator pos;
   for (pos = mStringIntColorMap.begin(); pos != mStringIntColorMap.end(); pos++)
   {
      if (pos->second == intColor)
         return pos->first;
   }
   
   // Should I throw an empty string instead of throwing an exception?
   #if 1
   return "";
   #else
   // Color not found, so throw an exception
   GmatBaseException be;
   be.SetDetails("ColorDatabase::GetColorName() Cannot find predefined color name "
                 "for the value %u(%06X)", intColor, intColor);
   throw be;
   #endif
}


//*********************************
// For Add, Remove 
//*********************************

//------------------------------------------------------------------------------
// void Add(const std::string &name, UnsignedInt intColor)
//------------------------------------------------------------------------------
void ColorDatabase::Add(const std::string &name, UnsignedInt intColor)
{
   #ifdef DEBUG_COLORDB_ADD
   MessageInterface::ShowMessage
      ("ColorDatabase::Add() <%p> entered, name='%p', intColor=%u\n",
       this, name.c_str(), intColor);
   #endif
   
   ColorMap::iterator pos = mStringIntColorMap.find(name);
   
   //if name already in the database, just ignore
   if (pos == mStringIntColorMap.end())
   {
      mStringIntColorMap.insert(std::pair<std::string, UnsignedInt>(name, intColor));
      mNumColors = mStringIntColorMap.size();
      #ifdef DEBUG_COLORDB_ADD
      MessageInterface::ShowMessage("   '%s' added to the map\n");
      #endif
   }
   else
   {
      #ifdef DEBUG_COLORDB_ADD
      MessageInterface::ShowMessage("   '%s' already in the map, so ignored\n");
      #endif
   }
}


//------------------------------------------------------------------------------
// void Add(const std::string &name, const RgbColor &rgbColor)
//------------------------------------------------------------------------------
void ColorDatabase::Add(const std::string &name, const RgbColor &rgbColor)
{
   UnsignedInt intColor = rgbColor.GetIntColor();
   Add(name, intColor);
}


//------------------------------------------------------------------------------
// void Remove(const std::string &name)
//------------------------------------------------------------------------------
void ColorDatabase::Remove(const std::string &name)
{
   //if name found in the database, remove
   if (mStringIntColorMap.find(name) != mStringIntColorMap.end())
   {   
      mStringIntColorMap.erase(name);
      mNumColors = mStringIntColorMap.size();  
      #ifdef DEBUG_COLORDB_ADD
      MessageInterface::ShowMessage("   '%s' removed from the map\n");
      #endif
   }
   else
   {
      #ifdef DEBUG_COLORDB_ADD
      MessageInterface::ShowMessage("   '%s' not in the map, so ignored\n");
      #endif
   }
}

//---------------------------------
// private
//---------------------------------

//------------------------------------------------------------------------------
// ColorDatabase()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
ColorDatabase::ColorDatabase()
{
   // Add predefined colors defined in ColorTypes.hpp
   Add("Aqua",              GmatColor::AQUA);
   Add("AquaMarine",        GmatColor::AQUA_MARINE);
   Add("Beige",             GmatColor::BEIGE);
   Add("Black",             GmatColor::BLACK);
   Add("Blue",              GmatColor::BLUE);
   Add("BlueViolet",        GmatColor::BLUE_VIOLET);
   Add("Brown",             GmatColor::BROWN);
   Add("CadetBlue",         GmatColor::CADET_BLUE);
   Add("Coral",             GmatColor::CORAL);
   Add("CornflowerBlue",    GmatColor::CORNFLOWER_BLUE);
   Add("Cyan",              GmatColor::CYAN);
   Add("DarkBlue",          GmatColor::DARK_BLUE);
   Add("DarkGoldenRod",     GmatColor::DARK_GOLDEN_ROD);
   Add("DarkGray",          GmatColor::DARK_GRAY);
   Add("DarkGreen",         GmatColor::DARK_GREEN);
   Add("DarkOliveGreen",    GmatColor::DARK_OLIVE_GREEN);
   Add("DarkOrchid",        GmatColor::DARK_ORCHID);
   Add("DarkSlateBlue",     GmatColor::DARK_SLATE_BLUE);
   Add("DarkSlateGray",     GmatColor::DARK_SLATE_GRAY);
   Add("DarkTurquoise",     GmatColor::DARK_TURQUOISE);
   Add("DimGray",           GmatColor::DIM_GRAY);
   Add("FireBrick",         GmatColor::FIRE_BRICK);
   Add("ForestGreen",       GmatColor::FOREST_GREEN);
   Add("Fuchsia",           GmatColor::FUCHSIA);
   Add("Gold",              GmatColor::GOLD);
   Add("GoldenRod",         GmatColor::GOLDEN_ROD);
   Add("Gray",              GmatColor::GRAY);
   Add("Green",             GmatColor::GREEN);
   Add("GreenYellow",       GmatColor::GREEN_YELLOW);
   Add("IndianRed",         GmatColor::INDIAN_RED);
   Add("Khaki",             GmatColor::KHAKI);
   Add("LightBlue",         GmatColor::LIGHT_BLUE);
   Add("LightGray",         GmatColor::LIGHT_GRAY);
   Add("LithTSteelBlue",    GmatColor::LIGHT_STEEL_BLUE);
   Add("Lime",              GmatColor::LIME);
   Add("LimeGreen",         GmatColor::LIME_GREEN);
   Add("Magenta",           GmatColor::MAGENTA);
   Add("Maroon",            GmatColor::MAROON);
   Add("MediumAquaMarine",  GmatColor::MEDIUM_AQUA_MARINE);
   Add("MediumBlue",        GmatColor::MEDIUM_BLUE);
   Add("MediumOrchid",      GmatColor::MEDIUM_ORCHID);
   Add("MediumSeaGreen",    GmatColor::MEDIUM_SEA_GREEN);
   Add("MediumSpringGreen", GmatColor::MEDIUM_SPRING_GREEN);
   Add("MediumTurquoise",   GmatColor::MEDIUM_TURQUOISE);
   Add("MediumVioletRed",   GmatColor::MEDIUM_VIOLET_RED);
   Add("MidnightBlue",      GmatColor::MIDNIGHT_BLUE);
   Add("Navy",              GmatColor::NAVY);
   Add("Olive",             GmatColor::OLIVE);
   Add("Orange",            GmatColor::ORANGE);
   Add("OrangeRed",         GmatColor::ORANGE_RED);
   Add("Orchid",            GmatColor::ORCHID);
   Add("PaleGreen",         GmatColor::PALE_GREEN);
   Add("Peru",              GmatColor::PERU);
   Add("Pink",              GmatColor::PINK);
   Add("Plum",              GmatColor::PLUM);
   Add("Purple",            GmatColor::PURPLE);
   Add("Red",               GmatColor::RED);
   Add("SaddleBrown",       GmatColor::SADDLE_BROWN);
   Add("Salmon",            GmatColor::SALMON);
   Add("SeaGreen",          GmatColor::SEA_GREEN);
   Add("Sienna",            GmatColor::SIENNA);
   Add("Silver",            GmatColor::SILVER);
   Add("SkyBlue",           GmatColor::SKY_BLUE);
   Add("SlateBlue",         GmatColor::SLATE_BLUE);
   Add("SpringGreen",       GmatColor::SPRING_GREEN);
   Add("SteelBlue",         GmatColor::STEEL_BLUE);
   Add("Tan",               GmatColor::TAN);
   Add("Teal",              GmatColor::TEAL);
   Add("Thistle",           GmatColor::THISTLE);
   Add("Turquoise",         GmatColor::TURQUOISE);
   Add("Violet",            GmatColor::VIOLET);
   Add("Wheat",             GmatColor::WHEAT);
   Add("White",             GmatColor::WHITE);
   Add("Yellow",            GmatColor::YELLOW);
   Add("YellowGreen",       GmatColor::YELLOW_GREEN);
   
   #ifdef DEBUG_COLORDB
   MessageInterface::ShowMessage
      ("ColorDatabase() mStringIntColorMap.size()=%d, "
       "mNumColors=%d\n",  mStringIntColorMap.size(), mNumColors);
   #endif
}


//------------------------------------------------------------------------------
// ~ColorDatabase()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ColorDatabase::~ColorDatabase()
{
}

