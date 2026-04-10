//$Id$
//------------------------------------------------------------------------------
//                                MdiChildDynamicDataFrame
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08.
//
// Created: 2017/01/24, Joshua Raymond, Thinking Systems, Inc.
//
/**
* Defines MdiChildDynamicDataFrame methods for a dynamic data display
*/
//------------------------------------------------------------------------------
#include "gmatwxrcs.hpp"
#include "MdiChildDynamicDataFrame.hpp"
#include "MdiTsPlotData.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "StringUtil.hpp"
#include <wx/bannerwindow.h>
#include <wx/colour.h>

BEGIN_EVENT_TABLE(MdiChildDynamicDataFrame, GmatMdiChildFrame)
   EVT_ACTIVATE(MdiChildDynamicDataFrame::OnActivate)
   EVT_SIZE(MdiChildDynamicDataFrame::OnSize)
   EVT_MOVE(MdiChildDynamicDataFrame::OnMove)
   EVT_CLOSE(MdiChildDynamicDataFrame::OnClose)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// MdiChildDynamicDataFrame(wxMDIParentFrame *parent, const wxString& plotName,
//                          const wxString& title, const wxPoint& pos,
//                          const wxSize& size, bool sizePreset,
//                          const long style)
//------------------------------------------------------------------------------
/**
* Constructor for the frame
*
* @param parent Window that owns this one
* @param plotName Name of the plot
* @param title Title of the plot
* @param pos Position for the frame on the screen
* @param size Size of the frame
* @param sizePreset Boolean of whether or not the frame size was set by the user
* @param style Style used for drawing the frame
*/
//------------------------------------------------------------------------------
MdiChildDynamicDataFrame::MdiChildDynamicDataFrame(wxMDIParentFrame *parent,
                                                   const wxString& plotName,
                                                   const wxString& title,
                                                   const wxPoint& pos,
                                                   const wxSize& size,
                                                   bool sizePreset,
                                                   const long style)  : 
      GmatMdiChildFrame(parent, plotName, title, GmatTree::OUTPUT_PERSISTENT,
                        -1, pos, size, style | wxNO_FULL_REPAINT_ON_RESIZE)
{
   mPlotTitle = title;
   MdiTsPlot::mdiChildren.Append(this);
   gridWidth = size.GetWidth();
   gridHeight = size.GetHeight();
   isSizePreset = sizePreset;
   Create();

   mCanSaveLocation = true;
   GmatAppData::Instance()->GetMainFrame()->theMdiChildren->Append(this);
}

//------------------------------------------------------------------------------
// ~MdiChildDynamicDataFrame()
//------------------------------------------------------------------------------
/**
* Destructor for the frame
*/
//------------------------------------------------------------------------------
MdiChildDynamicDataFrame::~MdiChildDynamicDataFrame()
{
   MdiTsPlot::mdiChildren.DeleteObject(this);
   MdiTsPlot::numChildren--;
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
* Method used to create the grid used for the dynamic data, initialized at zero
* rows and columns
*/
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::Create()
{
   dynamicDataGrid = new wxGrid(this, -1, wxDefaultPosition, wxSize(gridWidth,
      gridHeight));
   dynamicDataGrid->CreateGrid(0, 0);
   dynamicDataGrid->EnableEditing(false);
   dynamicDataGrid->EnableDragGridSize(false);
   dynamicDataGrid->EnableDragColMove(true);
   dynamicDataGrid->HideColLabels();
   dynamicDataGrid->HideRowLabels();
   dynamicDataGrid->SetGridLineColour(*wxBLACK);

   gridSizer = new wxBoxSizer(wxVERTICAL);
   gridSizer->Add(dynamicDataGrid, 0, wxGROW | wxALIGN_CENTER | wxALL);
   gridSizer->SetSizeHints(this);
}

//------------------------------------------------------------------------------
// void SetTableSize(Integer maxRowCount, Integer maxColCount)
//------------------------------------------------------------------------------
/**
* Method to set the size of the dynamic data to ensure all the requested data
* will fit
*
* @param maxRowCount The max number of rows
* @param maxColCount the max number of columns
*/
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::SetTableSize(Integer maxRowCount,
                                            Integer maxColCount)
{
   dynamicDataGrid->AppendCols(maxColCount);
   dynamicDataGrid->AppendRows(2*maxRowCount);
   wxFont *boldText = new wxFont();
   boldText->MakeBold();

   for (Integer i = 0; i < maxRowCount; ++i)
   {
      for (Integer j = 0; j < maxColCount; ++j)
      {
         dynamicDataGrid->SetCellBackgroundColour(2 * i, j, *wxLIGHT_GREY);
         dynamicDataGrid->SetCellFont(2 * i, j, *boldText);
      }
   }

   Real minWidth = maxColCount*250;
   Real minHeight = 3*dynamicDataGrid->GetRowHeight(0);
   SetMinClientSize(wxSize(minWidth, minHeight));
   if (isSizePreset)
      SetClientSize(gridWidth, gridHeight);
   else
      SetClientSize(wxSize(minWidth, maxRowCount*minHeight / 1.5));

   if (!isSizePreset)
   {
      gridWidth = minWidth;
      gridHeight = minHeight;
   }

   isSizePreset = true;
}


//------------------------------------------------------------------------------
// void UpdateDynamicData(std::vector<std::vector<DDD>> newData)
//------------------------------------------------------------------------------
/**
* Method to update the parameter data in the grid cells, called either for the
* inital parameter values or when the UpdateDynamicData command is run
*
* @param newData A struct containing the new data to update the cells with
*/
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::UpdateDynamicData(
   std::vector<std::vector<DDD>> newData)
{
   for (Integer i = 0; i < newData.size(); ++i)
   {
      for (Integer j = 0; j < newData[i].size(); ++j)
      {
         // Check if the name of the parameter has been added yet
         if (dynamicDataGrid->GetCellValue(2*i, j) != newData[i][j].paramName)
            dynamicDataGrid->SetCellValue(2*i, j, newData[i][j].paramName);
         dynamicDataGrid->SetCellValue(2*i + 1, j, newData[i][j].paramValue);
      }
   }

   SetDynamicDataCellTextColor(newData);
   SetDynamicDataCellBackgroundColor(newData);
   dynamicDataGrid->Update();
}

//------------------------------------------------------------------------------
// void DeleteDynamicDataGrid()
//------------------------------------------------------------------------------
/**
* Deletes all rows and columns of the current grid
*/
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::DeleteDynamicDataGrid()
{
   dynamicDataGrid->DeleteCols(0, dynamicDataGrid->GetNumberCols());
   dynamicDataGrid->DeleteRows(0, dynamicDataGrid->GetNumberRows());
}

//------------------------------------------------------------------------------
// void SetDynamicDataCellTextColor(std::vector<std::vector<DDD>> newColor)
//------------------------------------------------------------------------------
/**
* Sets the text color of the specified parameter's value
*
* @param paramName The name of the parameter to change the text color of
* @param textColor The new color to assign to the text
*/
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::SetDynamicDataCellTextColor(
   std::vector<std::vector<DDD>> newColor)
{
   
   RgbColor newTextColor;
   wxColour newWxTextColor;

   for (Integer i = 0; i < newColor.size(); ++i)
   {
      for (Integer j = 0; j < newColor[i].size(); ++j)
      {
         newTextColor = RgbColor(newColor[i][j].paramTextColor);
         newWxTextColor = wxColour(newTextColor.Red(), newTextColor.Green(),
            newTextColor.Blue());

         dynamicDataGrid->SetCellTextColour(2*i + 1, j, newWxTextColor);
      }
   }
}

//------------------------------------------------------------------------------
// void SetDynamicDataCellBackgroundColor(
// std::vector<std::vector<DDD>> newColor)
//------------------------------------------------------------------------------
/**
* Sets the background color of the specified parameter's cell
*
* @param newColor A struct containing the new colors to set each value to
*/
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::SetDynamicDataCellBackgroundColor(
   std::vector<std::vector<DDD>> newColor)
{

   RgbColor newTextColor;
   wxColour newWxTextColor;

   for (Integer i = 0; i < newColor.size(); ++i)
   {
      for (Integer j = 0; j < newColor[i].size(); ++j)
      {
         newTextColor = RgbColor(newColor[i][j].paramBackgroundColor);
         newWxTextColor = wxColour(newTextColor.Red(), newTextColor.Green(),
            newTextColor.Blue());

         dynamicDataGrid->SetCellBackgroundColour(2 * i + 1, j, newWxTextColor);
      }
   }
}

//------------------------------------------------------------------------------
// void OnChangeTitle(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
{
   wxString title = wxGetTextFromUser(_T("Enter the new title for MDI child"),
      _T(""),
      mPlotTitle, //s_title,
      GetParent()->GetParent());
   if (!title)
      return;

   mPlotTitle = title;
   SetTitle(title);
}


//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
   Close(TRUE);
}


//------------------------------------------------------------------------------
// void OnActivate(wxActivateEvent& event)
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::OnActivate(wxActivateEvent& event)
{
   //if ( event.GetActive() && mCanvas )
   //   mCanvas->SetFocus();

   GmatMdiChildFrame::OnActivate(event);
}


//------------------------------------------------------------------------------
// void OnMove(wxMoveEvent& event)
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::OnMove(wxMoveEvent& event)
{
   wxSize theSize = GetClientSize();
   gridWidth  = theSize.GetWidth();
   gridHeight = theSize.GetHeight();

   event.Skip();
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::OnClose(wxCloseEvent &event)
{
   #ifdef DEBUG_MDI_DYNAMIC_DATA_FRAME_CLOSE
      MessageInterface::ShowMessage
         ("\nMdiChildDynamicDataFrame::OnClose() '%s' entered, mCanClose=%d\n",
         mChildName.c_str(), mCanClose);
   #endif

   GmatMdiChildFrame::OnClose(event);
   event.Skip();

   #ifdef DEBUG_MDI_DYNAMIC_DATA_FRAME_CLOSE
      MessageInterface::ShowMessage
         ("MdiChildDynamicDataFrame::OnClose() '%s' exiting, mCanClose=%d\n",
         mChildName.c_str(), mCanClose);
   #endif
}

//------------------------------------------------------------------------------
// void OnSize(wxSizeEvent &event)
//------------------------------------------------------------------------------
void MdiChildDynamicDataFrame::OnSize(wxSizeEvent &event)
{
   int w;
   int h;
   GetClientSize(&w, &h);
   for (Integer i = 0; i < dynamicDataGrid->GetNumberRows(); ++i)
   {
      for (Integer j = 0; j < dynamicDataGrid->GetNumberCols(); ++j)
      dynamicDataGrid->SetColSize(j, w / dynamicDataGrid->GetNumberCols());
   }

   wxSize newSize = GetClientSize();

   gridWidth  = newSize.GetWidth();
   gridHeight = newSize.GetHeight();

   event.Skip();
}
