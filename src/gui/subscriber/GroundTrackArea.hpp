//------------------------------------------------------------------------------
//                           GroundTrackArea
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Sep 24, 2025
/**
 * Panel used to draw the ground track data.
 *
 * This implementation is ported from Astrodynamics Workbench(R).
 */
//------------------------------------------------------------------------------

#ifndef SRC_GUI_SUBSCRIBER_GROUNDTRACKAREA_HPP_
#define SRC_GUI_SUBSCRIBER_GROUNDTRACKAREA_HPP_

#include "GmatPanel.hpp"
#include <wx/clrpicker.h> // for wxColorPickerCtrl
#include "GroundTrackCurve.hpp"

class GroundTrackArea : public wxPanel
{
public:
   GroundTrackArea(wxWindow *parent, const wxString &name);
   virtual ~GroundTrackArea();

   void Clear();

   void SetOption(const std::string &optionSetting, const std::string &optionValue);
   bool AddData(const double epoch, const double *dat, const int satcount);
   bool SetDataNames(const StringArray &names);
//   bool AddPoint(PointData *point);
   void SetColor(const int forCurve, const int red, const int green,
         const int blue, const int atIndex = 0);
   void SetColor(const std::string forStation, const int red, const int green,
         const int blue, const int atIndex = 0);
   void SetLineWidth(int width);
   void UpdatePlot()
   { };

   virtual void SetSolarSystem(SolarSystem *ss);

   bool TakeAction(const std::string &theAction);

private:

   /// Data structure used for stations and other body fixed points
   struct MarkedPoint
   {
      std::string pointName;
      Real latitude;
      Real longitude;
      UnsignedInt color;
   };

   /// Background image for the frame
   std::string textureMap;
   /// Flag used to avoid multiple map refreshes
   bool mapLoaded;
   /// The raw map data
   wxImage bgImage;
   /// Map data that scales with the window
   wxBitmap bgScaledMap;
   /// Number of points needed to cause a ground track update
   int refreshInterval;
   /// Current number of new points received
   int currentCount;
   /// Line width for the ground tracks
   int lineWidth;
   /// Nmaes for the tracks
   StringArray dataNames;

   // lat/long grid management
   /// Toggle to turn teh grid on and off
   bool useGrid;
   /// Number of longitude grid lines
   Integer longLineCount;
   /// Number of latitude grid lines
   Integer latLineCount;

   /// The trajectory data
   std::vector<GroundTrackCurve*> data;
   /// Colors for the tracks
   std::vector<wxColour> startColors;
   /// Satellite Names
   StringArray theSats;

   /// Solar system for data lookups
   SolarSystem *theSS;

   // Other data settings
   /// Flag for receiving data
   bool penIsDown;

   // Zoom settings - not currently used, avaialble for future enhancements

   /// Flag to indicate that the track is zoomed
   bool isZoomed;
   /// Minimum longitude displayed
   Real zoomLongMin;
   /// Maximum longitude displayed
   Real zoomLongMax;
   /// Maximum latitude displayed
   Real zoomLatMin;
   /// Maximum latitude displayed
   Real zoomLatMax;

   /// The marked point data
   std::vector<MarkedPoint> points;

   void OnPaint(wxPaintEvent& ev);
   void OnSize(wxSizeEvent& ev);
   void LoadImageBG();
   void CreateScaledBg();
   void SaveData();
   void LoadData();

   GuiInterpreter *theGuiInterpreter;
   wxBoxSizer *thePanelSizer;


   DECLARE_EVENT_TABLE();

};

#endif /* SRC_GUI_SUBSCRIBER_GROUNDTRACKAREA_HPP_ */
