//------------------------------------------------------------------------------
//                           GroundTrackWindow
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
// Created: Sep 24, 2025
/**
 * Window containing the ground track plotting area
 *
 * This implementation is ported from Astrodynamics Workbench(R).
 */
//------------------------------------------------------------------------------

#include "GroundTrackWindow.hpp"
#include "GroundTrackArea.hpp"
#include "MessageInterface.hpp"
#include "GroundTrackArea.hpp"
#include "GmatMainFrame.hpp"
#include "GmatAppData.hpp"
#include "MdiTsPlotData.hpp"

//#define DEBUG_MDI_DYNAMIC_DATA_FRAME_CLOSE


//------------------------------------------------------------------------------
// GroundTrackWindow::GroundTrackWindow(wxMDIParentFrame *parent,
//         const wxString& plotName,
//         const wxString& title,
//         const GmatTree::ItemType type,
//         const wxPoint& pos,
//         const wxSize& size,
//         const long style)
//------------------------------------------------------------------------------
/**
 * The default constructor.
 *
 * @param plotName The name of the plot
 * @param title The title of the plot
 * @param type GMAT type for the plot
 * @param pos Position on screen as a wxPoint
 * @param size Size on screen as a wxSize
 * @param style
 */
//------------------------------------------------------------------------------
GroundTrackWindow::GroundTrackWindow(wxMDIParentFrame *parent,
         const wxString& plotName,
         const wxString& title,
         const GmatTree::ItemType type,
         const wxPoint& pos,
         const wxSize& size,
         const long style)  :
      GmatMdiChildFrame(parent, plotName, title, GmatTree::OUTPUT_PERSISTENT,
            -1, pos, size, style | wxNO_FULL_REPAINT_ON_RESIZE)
{
   mPlotTitle = title;
   MdiTsPlot::mdiChildren.Append(this);
   MdiTsPlot::numChildren++;

   Create();

   // Place panel in window management list
   SetPlotName(title);
   mCanSaveLocation = true;
   mItemType = GmatTree::OUTPUT_GROUND_TRACK_PLOT;
   GmatAppData::Instance()->GetMainFrame()->theMdiChildren->Append(this);
}

//------------------------------------------------------------------------------
// ~GroundTrackWindow()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GroundTrackWindow::~GroundTrackWindow()
{
   //GroundTrackArea::mdiChildren.DeleteObject(this);
   //GroundTrackArea::numChildren--;

   // Remove this child window from the window manager
   MdiTsPlot::mdiChildren.DeleteObject(this);
   MdiTsPlot::numChildren--;
   delete theMap;
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
* Method used to create the grid used for the dynamic data, initialized at zero
* rows and columns
*/
//------------------------------------------------------------------------------
void GroundTrackWindow::Create(const std::string &title)
{
   theMap = new GroundTrackArea(this, title);
   wxBoxSizer* theSizer = new wxBoxSizer(wxVERTICAL);
   theMap->SetSizer(theSizer);
   Layout();
}

//------------------------------------------------------------------------------
// void OnChangeTitle(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Method used to change the window title from the GUI
 */
void GroundTrackWindow::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
{
   wxString title = wxGetTextFromUser(_T("Enter the new title for Groundtrack"),
      _T(""),
      mPlotTitle,
      GetParent()->GetParent());

   if (!title)
      return;

   mPlotTitle = title;
   SetTitle(title);
}


//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Method used to close the window
 */
void GroundTrackWindow::OnQuit(wxCommandEvent& WXUNUSED(event))
{
   Close(TRUE);
}


//------------------------------------------------------------------------------
// void OnActivate(wxActivateEvent& event)
//------------------------------------------------------------------------------
/**
 * Activation method
 */
void GroundTrackWindow::OnActivate(wxActivateEvent& event)
{
   GmatMdiChildFrame::OnActivate(event);
}


//------------------------------------------------------------------------------
// void OnMove(wxMoveEvent& event)
//------------------------------------------------------------------------------
/**
 * Move handler
 */
void GroundTrackWindow::OnMove(wxMoveEvent& event)
{
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
/**
 * Window close handler
 */
void GroundTrackWindow::OnClose(wxCloseEvent &event)
{
   #ifdef DEBUG_MDI_DYNAMIC_DATA_FRAME_CLOSE
      MessageInterface::ShowMessage
         ("GroundTrackWindow::OnClose() '%s' entered, mCanClose=%d\n",
         mChildName.c_str(), mCanClose);
   #endif

   GmatMdiChildFrame::OnClose(event);
   event.Skip();

   #ifdef DEBUG_MDI_DYNAMIC_DATA_FRAME_CLOSE
      MessageInterface::ShowMessage
         ("GroundTrackWindow::OnClose() '%s' exiting, mCanClose=%d\n",
         mChildName.c_str(), mCanClose);
   #endif
}

//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent &event)
//------------------------------------------------------------------------------
/**
 * Resize handler - currently managed in the GroundTrackArea component
 */
void GroundTrackWindow::OnSize(wxSizeEvent &event)
{
   event.Skip();
}

//------------------------------------------------------------------------------
// void GroundTrackWindow::OnPaint(wxPaintEvent& event)
//------------------------------------------------------------------------------
/**
 * Paint handler used to redraw the window
 */
void GroundTrackWindow::OnPaint(wxPaintEvent& event)
{
   #ifdef DEBUG_GROUNDTRACK_PAINT
      MessageInterface::ShowMessage("DEBUG: GroundTrackWindow::OnPaint Entered.\n");
   #endif

   theMap->UpdatePlot();
}

//------------------------------------------------------------------------------
// bool AddData(const double epoch, const double *longlat, const int satcount)
//------------------------------------------------------------------------------
/**
 * Passes a data point on to the associated GroundTrackArea
 *
 * @param epoch Epoch of the data point
 * @param longlat Latitude & longitude of the data point
 * @param satcount The number of satellites
 */
//------------------------------------------------------------------------------
bool GroundTrackWindow::AddData(const double epoch, const double *longlat,
      const int satcount)
{
   return theMap->AddData(epoch, longlat, satcount);
}


//------------------------------------------------------------------------------
// void SetOption(const std::string &optionSetting,
//                const std::string &optionValue)
//------------------------------------------------------------------------------
/**
 * Message handler for data from the base code vis the GuiPlotReceiver.
 *
 * This handler passes messages to the GroundTrackArea for processing
 *
 * @param optionSetting The option to be processed
 * @param optionValue Optional data associated with the option
 */
//------------------------------------------------------------------------------
void GroundTrackWindow::SetOption(const std::string &optionSetting,
      const std::string &optionValue)
{
   #ifdef DEBUG_GT_OPTIONS
      MessageInterface::ShowMessage("Option %s = %s\n", optionSetting.c_str(),
            optionValue.c_str());
   #endif

   if (optionSetting == "TextureMap")
   {
      theMap->SetOption(optionSetting, optionValue);
      theMap->TakeAction("Refresh");
   }
}

//------------------------------------------------------------------------------
// void SetColor(const int forCurve, const int red,
//       const int green, const int blue, const int atIndex)
//------------------------------------------------------------------------------
/**
 * Sets the track color
 *
 * @param forCurve Index of the curve
 * @param red Red saturation (up to 255)
 * @param green Green saturation (up to 255)
 * @param blue Blue saturation (up to 255)
 * @param atIndex Index where the color starts (default 0; not yet implemented
 *                to change)
 */
//------------------------------------------------------------------------------
void GroundTrackWindow::SetColor(const int forCurve, const int red,
      const int green, const int blue, const int atIndex)
{
   theMap->SetColor(forCurve, red, green, blue, atIndex);
}

//------------------------------------------------------------------------------
// void SetLineWidth(int width)
//------------------------------------------------------------------------------
/**
 * Adjusts the line width for the tracks
 *
 * @param width The new line width
 */
//------------------------------------------------------------------------------
void GroundTrackWindow::SetLineWidth(int width)
{
   theMap->SetLineWidth(width);
}

//------------------------------------------------------------------------------
// void LabelData(StringArray dataNames)
//------------------------------------------------------------------------------
/**
 * Set labels for the tracks
 *
 * @param dataNames The labels
 */
//------------------------------------------------------------------------------
void GroundTrackWindow::LabelData(StringArray dataNames)
{
   theMap->SetDataNames(dataNames);
}

//------------------------------------------------------------------------------
// void UpdatePlot()
//------------------------------------------------------------------------------
/**
 * Calls for the associated GroundTrackArea to update.
 */
//------------------------------------------------------------------------------
void GroundTrackWindow::UpdatePlot()
{
   theMap->UpdatePlot();
}

//------------------------------------------------------------------------------
// void ResetForNewRun()
//------------------------------------------------------------------------------
/**
 * Clears the data for a new run. Currently unimplemented.
 */
//------------------------------------------------------------------------------
void GroundTrackWindow::ResetForNewRun()
{
}

//------------------------------------------------------------------------------
// bool TakeAction(const std::string &theAction)
//------------------------------------------------------------------------------
/**
 * Passes a string containing an action on to the GroundTrackArea.
 *
 * @param theAction: The action to pass on.
 */
//------------------------------------------------------------------------------
bool GroundTrackWindow::TakeAction(const std::string &theAction)
{
   return theMap->TakeAction(theAction);
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Connects the solar system in use into the component
 */
void GroundTrackWindow::SetSolarSystem(SolarSystem *ss)
{
   theSS = ss;
   theMap->SetSolarSystem(ss);
}
