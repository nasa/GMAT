//------------------------------------------------------------------------------
//                           GroundTrackCurve
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS III
// contract
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Nov 17, 2025
/**
 * Data structure used for ground track traces.
 *
 * This implementation is ported from Astrodynamics Workbench(R).
 */
//------------------------------------------------------------------------------

#include "GroundTrackCurve.hpp"

//------------------------------------------------------------------------------
// GroundTrackCurve()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
GroundTrackCurve::GroundTrackCurve()
{
}

//------------------------------------------------------------------------------
// ~GroundTrackCurve()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
GroundTrackCurve::~GroundTrackCurve()
{
}

//------------------------------------------------------------------------------
// GroundTrackCurve(const GroundTrackCurve &other)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param other Curve copied to this one (currently unused)
 */
GroundTrackCurve::GroundTrackCurve(const GroundTrackCurve &other)
{
}

//------------------------------------------------------------------------------
// GroundTrackCurve& operator=(const GroundTrackCurve &other)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param other Curve copied to this one (currently unused)
 */
GroundTrackCurve& GroundTrackCurve::operator=(const GroundTrackCurve &other)
{
   if (this != &other)
   {

   }

   return *this;
}

//------------------------------------------------------------------------------
// void AddData(double xValue, double yValue)
//------------------------------------------------------------------------------
/**
 * Adds a data point to the curve
 *
 * @param xValue The x coordinate
 * @param yValue The y coordinate
 */
void GroundTrackCurve::AddData(double xValue, double yValue)
{
   xData.push_back(xValue);
   yData.push_back(yValue);
}

//------------------------------------------------------------------------------
// void AddData(double xValue, double yValue, double tValue)
//------------------------------------------------------------------------------
/**
 * Adds a data point to the curve
 *
 * @param xValue The x coordinate
 * @param yValue The y coordinate
 * @param tValue The t coordinate
 */
void GroundTrackCurve::AddData(double xValue, double yValue, double tValue)
{
   xData.push_back(xValue);
   yData.push_back(yValue);
   tData.push_back(tValue);
}

//------------------------------------------------------------------------------
// void Clear(unsigned int fromIndex)
//------------------------------------------------------------------------------
/**
 * Removes data from a curve
 *
 * @param fromIndex The index of the first point to remove
 */
void GroundTrackCurve::Clear(unsigned int fromIndex)
{
   xData.resize(fromIndex);
   yData.resize(fromIndex);
   tData.resize(fromIndex);
}

//------------------------------------------------------------------------------
// std::string GetLabel()
//------------------------------------------------------------------------------
/**
 * Returns the label for the curve
 *
 * @retval The label
 */
std::string GroundTrackCurve::GetLabel()
{
   return label;
}

//------------------------------------------------------------------------------
// void SetLabel(std::string theLabel))
//------------------------------------------------------------------------------
/**
 * Sets the label for the curve
 *
 * @theLabel The label
 */
void GroundTrackCurve::SetLabel(std::string theLabel)
{
   label = theLabel;
}
