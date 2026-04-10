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
* Declares MdiChildDynamicDataFrame methods for a dynamic data display
*/
//------------------------------------------------------------------------------
#ifndef MdiChildDynamicDataFrame_hpp
#define MdiChildDynamicDataFrame_hpp

#include "GmatMdiChildFrame.hpp"
#include "DynamicDataInterface.hpp"
#include "gmatdefs.hpp"
#include "RgbColor.hpp"
#include "DynamicDataStruct.hpp"
#include <wx/grid.h>
#include <wx/bannerwindow.h>

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

class MdiChildDynamicDataFrame : public GmatMdiChildFrame
{
public:
   MdiChildDynamicDataFrame(wxMDIParentFrame *parent, const wxString& plotName,
      const wxString& title, const wxPoint& pos,
      const wxSize& size, bool sizePreset, const long style);
   virtual ~MdiChildDynamicDataFrame();

   void SetTableSize(Integer maxRowCount, Integer maxColCount);
   void UpdateDynamicData(std::vector<std::vector<DDD>> newData);
   void DeleteDynamicDataGrid();
   void SetDynamicDataCellTextColor(std::vector<std::vector<DDD>> newColor);
   void SetDynamicDataCellBackgroundColor(std::vector<std::vector<DDD>> newColor);

   // menu actions
   virtual void OnChangeTitle(wxCommandEvent& event);

   virtual void OnQuit(wxCommandEvent& event);

   virtual void OnActivate(wxActivateEvent& event);
   virtual void OnMove(wxMoveEvent& event);
   virtual void OnClose(wxCloseEvent &event);
   virtual void OnSize(wxSizeEvent& event);

protected:
   virtual void Create();

   /// The title of the display window
   wxString mPlotTitle;
   /// The grid holding the display data and parameter names
   wxGrid *dynamicDataGrid;
   /// A sizer for the grid
   wxBoxSizer *gridSizer;
   /// The width of the frame
   Integer gridWidth;
   /// The height of the fram
   Integer gridHeight;
   /// Boolean showing whether the user has specified a frame size to use
   bool isSizePreset;

   DECLARE_EVENT_TABLE();
};
#endif