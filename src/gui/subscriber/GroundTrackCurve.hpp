//------------------------------------------------------------------------------
//                           GroundTrackCurve
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2025 United States Government as represented by the
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
// contract.
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Nov 17, 2025
/**
 * Data structure used for ground track traces.
 *
 * This implementation is ported from Astrodynamics Workbench(R).
 */
//------------------------------------------------------------------------------

#ifndef SRC_GUI_SUBSCRIBER_GROUNDTRACKCURVE_HPP_
#define SRC_GUI_SUBSCRIBER_GROUNDTRACKCURVE_HPP_

#include <wx/clrpicker.h> // for wxColorPickerCtrl

class GroundTrackCurve
{
public:
   GroundTrackCurve();
   virtual ~GroundTrackCurve();
   GroundTrackCurve(const GroundTrackCurve &other);
   GroundTrackCurve& operator=(const GroundTrackCurve &other);

   void AddData(double xValue, double yValue);
   void AddData(double xValue, double yValue, double tValue);
   void Clear(unsigned int fromIndex = 0);

   std::string GetLabel();
   void SetLabel(std::string theLabel);

   unsigned int size()
   {
      return xData.size();
   }

   unsigned int Length()
   {
      return xData.size();
   }

   double Xi(int index)
   {
      if ((index >= 0) && (index < xData.size()))
         return xData[index];

      return 0.0;
   }

   double Yi(int index)
   {
      if ((index >= 0) && (index < yData.size()))
         return yData[index];

      return 0.0;
   }

   double Ti(int index)
   {
      if ((index >= 0) && (index < tData.size()))
         return tData[index];
      return 0.0;
   }

   void SetColor(wxColour theColor, int atPoint = 0)
   {
      colors.push_back(theColor);
      colorChangeLocation.push_back(atPoint);
   }

   wxColour GetColor(int atPoint = 0)
   {
      int index = 0;
      if (index < colors.size())
         return colors[index];
      return wxColour(255,0,0);
   }

   void PenUp()
   {
      penUp.push_back(xData.size()-1);
   }

protected:
   /// Horizontal axis data
   std::vector<double> xData;
   /// Vertical axis data
   std::vector<double> yData;
   /// Third data set, for tracking time for sets that are not time based
   std::vector<double> tData;

   /// Location for a color change
   std::vector<int> colorChangeLocation;
   /// Color for the track
   std::vector<wxColour> colors;

   /// Locations for producing a gap in the curve -
   /// stop at this point, restart at the next
   std::vector<int> penUp;

   /// Label for the track
   std::string label;

};

#endif /* SRC_GUI_SUBSCRIBER_GROUNDTRACKCURVE_HPP_ */
