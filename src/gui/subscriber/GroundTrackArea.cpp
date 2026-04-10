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
 * Panel used to draw the ground track data
 *
 * This implementation is ported from Astrodynamics Workbench(R).
 */
//------------------------------------------------------------------------------

#include "GroundTrackArea.hpp"
#include "GroundTrack.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MessageInterface.hpp"
#include "GroundTrackCurve.hpp"
#include "GmatAppData.hpp"
#include "Moderator.hpp"

//#define DEBUG_GT_ACTIONS

BEGIN_EVENT_TABLE(GroundTrackArea, wxPanel)
   EVT_PAINT         (GroundTrackArea::OnPaint)
   EVT_SIZE          (GroundTrackArea::OnSize)
//   EVT_MOUSE_EVENTS  (TsPlotCanvas::OnMouseEvent)
END_EVENT_TABLE()

//#define DEBUG_ACTIONS

//------------------------------------------------------------------------------
// GroundTrack(const std::string &name)
//------------------------------------------------------------------------------
/**
 * The default constructor
 */
//------------------------------------------------------------------------------
GroundTrackArea::GroundTrackArea(wxWindow *parent,
      const wxString &name) : wxPanel(parent),
      mapLoaded (false),
      refreshInterval (50),
      currentCount (0),
      lineWidth (2),
      isZoomed (false),
      zoomLongMin (-180.0),
      zoomLongMax (180.0),
      zoomLatMin (-90.0),
      zoomLatMax (90.0),
      useGrid (true),
      longLineCount (12),
      latLineCount (6),
      theSS (nullptr),
      penIsDown (true)
{
}

//------------------------------------------------------------------------------
// ~GroundTrackArea()
//------------------------------------------------------------------------------
/**
 * The default destructor
 */
//------------------------------------------------------------------------------
GroundTrackArea::~GroundTrackArea()
{
}


//------------------------------------------------------------------------------
// void SetOption(const std::string &optionSetting, const std::string &optionValue)
//------------------------------------------------------------------------------
/**
 * Applies options to the widget
 *
 * @param optionSetting The setting to apply
 * @param optionValue The new value for the setting
 */
//------------------------------------------------------------------------------
void GroundTrackArea::SetOption(const std::string &optionSetting, const std::string &optionValue)
{
   if (optionSetting == "TextureMap")
   {
      textureMap = optionValue;
      mapLoaded = false;
   }
}

//------------------------------------------------------------------------------
// void GroundTrackArea::LoadImageBG()
//------------------------------------------------------------------------------
/**
 * Loads background image texture for the plot.
 */
//------------------------------------------------------------------------------
void GroundTrackArea::LoadImageBG()
{
   std::string imageLink = textureMap;

   if ((imageLink != "") && !mapLoaded)
   {
      if (textureMap.find("/") == std::string::npos)
         imageLink = "../data/graphics/texture/" + textureMap;

      bgImage = wxImage(imageLink, wxBITMAP_TYPE_JPEG);

      if(!bgImage.IsOk())
      {
         MessageInterface::ShowMessage
            ("GroundTrackArea background image '%s' is invalid.\n",
            imageLink.c_str());
         return;
      }

      CreateScaledBg();
      mapLoaded = true;
   }
}

//------------------------------------------------------------------------------
// void CreateScaledBg()
//------------------------------------------------------------------------------
/**
 *  Creates a scaled wxBitmap of the background texture
 */
//------------------------------------------------------------------------------
void GroundTrackArea::CreateScaledBg()
{
   #ifdef DEBUG_BACKGROUND
      MessageInterface::ShowMessage(
            "DEBUG: Background Creation Entered\n");
   #endif

   wxSize theSize = GetSize();
   bgScaledMap = wxBitmap(bgImage.Scale(theSize.GetWidth(),
         theSize.GetHeight(), wxIMAGE_QUALITY_NORMAL));
}

//------------------------------------------------------------------------------
// void SaveData
//------------------------------------------------------------------------------
/**
 *  Nothing b=needed here
 */
void GroundTrackArea::SaveData()
{
}

//------------------------------------------------------------------------------
// void LoadData
//------------------------------------------------------------------------------
/**
 * Nothing needed here
 */
void GroundTrackArea::LoadData()
{
}

//------------------------------------------------------------------------------
// void SetColor(const int forCurve, const int red, const int green,
//       const int blue, const int atIndex)
//------------------------------------------------------------------------------
/**
 * Method used to set line colors.
 *
 * This method can be used to change colors on lines if that enhancement is
 * added to the component.
 *
 * @param forCurve The index of the line receiving the color
 * @param red The red component of the color
 * @param green The grteen component of the color
 * @param blue The blue component of the color
 * @param atIndex The starting index on the line for the color
 */
void GroundTrackArea::SetColor(const int forCurve, const int red, const int green,
      const int blue, const int atIndex)
{

   wxColor theColor(red, green, blue);

   if (atIndex == 0)
      startColors.push_back(theColor);
   // uncomment for changing colors
//   else
//      data[forCurve]->SetColor(theColor, atIndex);
}

//------------------------------------------------------------------------------
// void SetLineWidth(int width)
//------------------------------------------------------------------------------
/**
 * Method to change line width settings for the ground tracks.
 *
 * @param The new width
 */
void GroundTrackArea::SetLineWidth(int width)
{
   lineWidth = width;
}

//------------------------------------------------------------------------------
// bool TakeAction(const std::string &theAction)
//------------------------------------------------------------------------------
/**
 * General purpose message handler for ground track areas
 *
 * @param theAction A string defining the action that need to be taken
 *
 * @return true if some action was performed
 */
bool GroundTrackArea::TakeAction(const std::string &theAction)
{
   #ifdef DEBUG_ACTIONS
      MessageInterface::ShowMessage("Ground track action %s\n",
            theAction.c_str());
   #endif

   bool retval = false;
   std::string action, actiondata = "";

   size_t loc = theAction.find("=");

   if (loc != std::string::npos)
   {
      action = theAction.substr(0, loc);
      actiondata = theAction.substr(loc+1);
   }
   else
      action = theAction;

   #ifdef DEBUG_GT_ACTIONS
      MessageInterface::ShowMessage("Taking action %s with data %s\n",
            action.c_str(), actiondata.c_str());
   #endif

   if (action == "PlotUpdateFrequency")
   {
      int frequency = std::stoi(actiondata);
      if (frequency > 0)
      {
         refreshInterval = frequency;

         #ifdef DEBUG_GT_ACTIONS
            MessageInterface::ShowMessage("   Refresh is now %d\n",
                  refreshInterval);
         #endif

         retval = true;
      }
   }
   else if (action == "UseGrid")
   {
      if (actiondata == "True")
         useGrid = true;
      else
         useGrid = false;
   }
   else if (action == "LatitudeLineCount")
   {
      int count = std::stoi(actiondata);
      if (count > 0)
      {
         latLineCount = count;
      }
   }
   else if (action == "LongitudeLineCount")
   {
      int count = std::stoi(actiondata);
      if (count > 0)
      {
         longLineCount = count;
      }
   }
   else if (action == "AddStation")
   {
      #ifdef DEBUG_FIXEDPOINTS
         MessageInterface::ShowMessage("Station %s added\n", actiondata.c_str());
      #endif

      BodyFixedPoint *thePoint = (BodyFixedPoint*)Moderator::Instance()->GetSpacePoint(actiondata);
      // Method calls for epoch but does not use it, so using J2000 epoch here
      Rvector3 latlongalt =  thePoint->GetSphericalLocation(21545.0);
      Real lat = latlongalt[0] * GmatMathConstants::DEG_PER_RAD;
      Real lng = latlongalt[1] * GmatMathConstants::DEG_PER_RAD;
      if (lng > 180.0)
         lng += -360.0;
      UnsignedInt theRGB = thePoint->GetCurrentOrbitColor();

      UnsignedInt rgb = (theRGB & 0xff0000) / 65536 +
            (theRGB & 0x00ff00) +
            (theRGB & 0x0000ff) * 65536;

      MarkedPoint pt;
      pt.pointName = actiondata;
      pt.latitude = lat;
      pt.longitude = lng;
      pt.color = rgb;
      points.push_back(pt);

      #ifdef DEBUG_FIXEDPOINTS
         MessageInterface::ShowMessage("   [%d]: lat, long, alt = [%lf %lf %lf]\n",
               points.size(), pt.latitude, pt.longitude, latlongalt[2]);
      #endif
   }
   else if (action == "Refresh")
   {
      if (!IsShown())
         CreateScaledBg();
      Show();
   }
   else if (action == "ClearData")
   {
      for (UnsignedInt i = 0; i < data.size(); ++i)
         (data[i])->Clear();
      points.clear();
      startColors.clear();
   }
   else if (action == "Reinitialize")
   {
      Clear();
   }
   else if (action == "Satellites")
   {
      if (startColors.size() == 0)
      {
         theSats.clear();
         startColors.clear();

         // startColors.clear();
         std::string satName;
         loc = 0;
         UnsignedInt rgb;

         while (loc != std::string::npos)
         {
            loc = actiondata.find("|");
            satName = actiondata.substr(0, loc);
            if (satName != "")
               theSats.push_back(satName);
            actiondata = actiondata.substr(loc+1);

            SpacePoint *theObject = Moderator::Instance()->GetSpacePoint(satName);
            if (theObject)
            {
               // wxWidgets sets colors as 0xbbggrr; GMAT uses 0xrrggbb
               UnsignedInt theRGB = theObject->GetCurrentOrbitColor();

               rgb = (theRGB & 0xff0000) / 65536 +
                     (theRGB & 0x00ff00) +
                     (theRGB & 0x0000ff) * 65536;
            }
            else
               rgb = 0xff;  // Default to red
            startColors.push_back(wxColour(rgb));
         }

         #ifdef DEBUG_COLORS
            MessageInterface::ShowMessage("Spacecraft colors:\n");
            for (UnsignedInt i = 0; i < theSats.size(); ++i)
               MessageInterface::ShowMessage("   %s: [%d %d %d]\n",
                     theSats[i].c_str(), startColors[i].GetRed(),
                     startColors[i].GetGreen(), startColors[i].GetBlue());
         #endif
      }

   }
   else if (action == "PenDown")
   {
      penIsDown = true;
   }
   else if (action == "PenUp")
   {
      penIsDown = false;
   }
   else if (action == "RunComplete")
   {
      Refresh();
      currentCount = 0;
   }
   else if (action == "Reset")
   {
      currentCount = 0;
      points.clear();
      startColors.clear();
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool AddData(const double epoch, const double *dat, const int satcount)
//------------------------------------------------------------------------------
/**
 * Adds a data point to be plotted.
 *
 * @param epoch Epoch of the data point
 * @param longlat Latitude & longitude of the data point
 * @param satcount The number of satellites
 */
//------------------------------------------------------------------------------
bool GroundTrackArea::AddData(const double epoch, const double *dat,
      const int satcount)
{
   #ifdef DEBUG_INCOMING_DATA
      std::stringstream buffer;
      buffer << satcount << " sats :  ";
      for (int i = 0; i < satcount; ++i)
         buffer << "[" << dat[i*2] << ", " << dat[i*2+1] << "] ";
      buffer << "\n";
      MessageInterface::ShowMessage("%s", buffer.str().c_str());
   #endif

   /// Build the buffers
   if (data.size() == 0)
   {
      for (unsigned int i = 0; i < satcount; ++i)
      {
         GroundTrackCurve *curve = new GroundTrackCurve();
         data.push_back(curve);
      }
   }

   if (penIsDown)
   {
      for (unsigned int i = 0; i < satcount; ++i)
      {
         if (i < data.size())
            data[i]->AddData(dat[i*2], dat[i*2+1], epoch);
      }

      ++currentCount;
   }

   if (currentCount >= refreshInterval)
   {
      Refresh();
      currentCount = 0;
   }

   return true;
}

//------------------------------------------------------------------------------
// void SetDataNames(const StringArray &names)
//------------------------------------------------------------------------------
/**
 * Method used to update the names of the data elements
 *
 * @param names The new names
 */
bool GroundTrackArea::SetDataNames(const StringArray &names)
{
   dataNames.clear();

   for (unsigned int i = 0; i < names.size(); ++i)
   {
      dataNames.push_back(names[i]);
   }

   return true;
}

//------------------------------------------------------------------------------
// void Clear()
//------------------------------------------------------------------------------
/**
 * Resets the groundtrack data
 */
void GroundTrackArea::Clear()
{
   for (UnsignedInt i = 0; i < data.size(); ++i)
      delete data[i];
   data.clear();
   points.clear();
}

//------------------------------------------------------------------------------
// void OnPaint(wxPaintEvent& ev)
//------------------------------------------------------------------------------
/**
 * Message handler that draws or redraws the ground track
 *
 * @param ev The paint event that initiates the drawing
 */
void GroundTrackArea::OnPaint(wxPaintEvent& ev)
{
   wxPaintDC dc(this);

   #ifdef DEBUG_GROUNDTRACK_PAINT
      MessageInterface::ShowMessage("DEBUG: GroundTrackArea::OnPaint Entered.\n");
   #endif

   LoadImageBG();

   wxCoord w, h;
   dc.GetSize(&w, &h);

   double zoomTop = 90.0, zoomLeft = -180.0,
          zoomWidth = 360.0, zoomHeight = 180.0;

   // Zoom management
   if (isZoomed)
   {
      zoomLeft   = zoomLongMin;
      zoomTop    = zoomLatMax;
      zoomWidth  = zoomLongMax - zoomLongMin;
      zoomHeight = zoomLatMax - zoomLatMin;
   }

   double xFactor = w / zoomWidth;
   double yFactor = h / zoomHeight;
   double x0, xm, x1, dx;
   double y0, ym, y1, dy;
   int xv1, yv1, xv2, yv2;

   // Work with zoom options...
   double longmin = zoomLeft, longmax = zoomLeft + zoomWidth;
   wxPen currentPen = dc.GetPen();

   //Draw the background map
   if (GetSize() != bgScaledMap.GetSize() )
   {
       CreateScaledBg();
   }

   dc.DrawBitmap(bgScaledMap, 0, 0);
   char val[12];     // Only need 4, but ints can use up to 11 places

   // Grid is active
   if (useGrid)
   {
      wxPen pen;
      pen.SetStyle(wxPENSTYLE_DOT);
      pen.SetColour(wxColour(0x888888));
      pen.SetWidth(1);
      dc.SetTextForeground(0x888888);
      dc.SetPen(pen);

      int inc = 360 / longLineCount;
      for (Integer i = 1; i < longLineCount; ++i)
      {
         xv1 = (int)((i * 360.0 / longLineCount) * xFactor);
         yv2 = (int)(180.0 * yFactor);
         dc.DrawLine(xv1, 0, xv1, yv2);

         sprintf(val, "%d", -180 + i * inc);
         wxRect *labelRect = new wxRect(xv1 + 4, yv2 - 18, 20, 50);
         dc.DrawLabel(val, *labelRect);
      }
      inc = 180 / latLineCount;
      for (Integer i = 1; i < latLineCount; ++i)
      {
         yv1 = (int)(i * 180.0 / latLineCount * yFactor);
         xv2 = (int)(360.0 * xFactor);
         dc.DrawLine(0, yv1, xv2, yv1);

         sprintf(val, "%d", 90 - i * inc);
         wxRect labelRect(4, yv1 - 8, 20, 50);
         dc.DrawLabel(val, labelRect);
      }

   }

   // Mark the body fixed points
   if (points.size() > 0)
   {
      wxPen pen;
      pen.SetStyle(wxPENSTYLE_SOLID);
      pen.SetWidth(1);
      Integer markerSize = 4;
      for (UnsignedInt i = 0; i < points.size(); ++i)
      {
         wxColour theColor(points[i].color);
         dc.SetTextForeground(theColor);
         pen.SetColour(theColor);
         dc.SetPen(pen);

         xv1 = (int)((180.0 + points[i].longitude) * xFactor);
         yv1 = (int)(( 90.0 - points[i].latitude) * yFactor);

         // Mark with an X for now
         dc.DrawLine(xv1-markerSize, yv1-markerSize, xv1+markerSize, yv1+markerSize);
         dc.DrawLine(xv1-markerSize, yv1+markerSize, xv1+markerSize, yv1-markerSize);

         wxRect labelRect(xv1 + 4, yv1 - 8, 20, 50);
         dc.DrawLabel(points[i].pointName, labelRect);
      }
   }

   // Draw the ground tracks
   for (int i = 0; i < data.size(); ++i)
   {
      wxPen pen;
      dc.SetTextForeground(startColors[i]);
      pen.SetColour(startColors[i]);
      pen.SetWidth(lineWidth);
      dc.SetPen(pen);

      for (int j = 0; j < data[i]->size() - 1; ++j)
      {
         x0 = data[i]->Xi(j);
         y0 = data[i]->Yi(j);
         x1 = data[i]->Xi(j+1);
         y1 = data[i]->Yi(j+1);

         if (fabs(x1 - x0) < 180)
         {
            xv1 = (int)((180.0 + x0) * xFactor);
            yv1 = (int)(( 90.0 - y0) * yFactor);
            xv2 = (int)((180.0 + x1) * xFactor);
            yv2 = (int)(( 90.0 - y1) * yFactor);

            if ((xv1 != xv2) || (yv1 != yv2))
               dc.DrawLine(xv1, yv1, xv2, yv2);
         }
         // Edge wrapping
         else
         {
            if (x0 < 0.0)
            {
               // x0 side
               xm = x1 - 360.0;
               dx = (x0 - longmin)/(x0 - xm);
               ym = y0 - dx * (y0 - y1);


               xv1 = (int)((180.0 + x0) * xFactor);
               yv1 = (int)(( 90.0 - y0) * yFactor);
               xv2 = (int)((180.0 + longmin) * xFactor);
               yv2 = (int)(( 90.0 - ym) * yFactor);
               dc.DrawLine(xv1, yv1, xv2, yv2);

               // x1 side
               xv1 = (int)((180.0 + x1) * xFactor);
               yv1 = (int)(( 90.0 - y1) * yFactor);
               xv2 = (int)((180.0 + longmax) * xFactor);
               yv2 = (int)(( 90.0 - ym) * yFactor);
               dc.DrawLine(xv1, yv1, xv2, yv2);
            }
            else // (x0 > 0.0)
            {
               // x0 side
               xm = x1 + 360.0;
               dx = (longmax - x0)/(xm - x0);
               dy = (y1 - y0) * dx;
               ym = y0 + dy;

               xv1 = (int)((180.0 + x0) * xFactor);
               yv1 = (int)(( 90.0 - y0) * yFactor);
               xv2 = (int)((180.0 + longmax) * xFactor);
               yv2 = (int)(( 90.0 - ym) * yFactor);
               dc.DrawLine(xv1, yv1, xv2, yv2);

               // x1 side
               xm = x0 - 360.0;
               dx = (xm - longmin)/(xm - x1);
               dy = (y1 - y0) * dx;
               ym = y0 + dy;

               xv1 = (int)((180.0 + x1) * xFactor);
               yv1 = (int)(( 90.0 - y1) * yFactor);
               xv2 = (int)((180.0 + longmin) * xFactor);
               yv2 = (int)(( 90.0 - ym) * yFactor);
               dc.DrawLine(xv1, yv1, xv2, yv2);
            }
         }
      }

      // Draw a label for the curve
      dc.SetPen(currentPen);

      wxString label = theSats[i];
      wxRect labelRect(xv2+4, yv2 - 8, 20, 50);

      dc.DrawLabel(label, labelRect);
   }
}

//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent& ev)
//------------------------------------------------------------------------------
/**
 * Resizing event handler
 *
 * @param ev The sizing event that initialed the action
 */
void GroundTrackArea::OnSize(wxSizeEvent& ev)
{
   Refresh();
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Solar system setup method
 *
 * @param ss The solar system used in the run
 */
void GroundTrackArea::SetSolarSystem(SolarSystem *ss)
{
   theSS = ss;
}


