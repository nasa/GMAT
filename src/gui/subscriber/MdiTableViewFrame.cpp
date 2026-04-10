//$Id$
//------------------------------------------------------------------------------
//                              MdiTableViewFrame
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
// Author: Thomas Grubb
// Created: 2015/02/01
/**
 * Implements MdiTableViewFrame 
 */
//------------------------------------------------------------------------------
#include "gmatwxrcs.hpp"
#include "MdiTableViewFrame.hpp"
#include "Subscriber.hpp"
#include "SubscriberException.hpp"
#include "GmatAppData.hpp"
#include "StringUtil.hpp"
#include "GmatTreeItemData.hpp"
#include "GmatMainFrame.hpp"      // for namespace GmatMenu
#include "ColorTypes.hpp"         // for namespace GmatColor::
#include "MessageInterface.hpp"
#include "GuiListenerManager.hpp"
#include <wx/bannerwindow.h>
#include <wx/colour.h>


BEGIN_EVENT_TABLE(MdiTableViewFrame, GmatMdiChildFrame)
   EVT_ACTIVATE(MdiTableViewFrame::OnActivate)
   EVT_SIZE(MdiTableViewFrame::OnSize)
   EVT_MOVE(MdiTableViewFrame::OnMove)
   EVT_CLOSE(MdiTableViewFrame::OnClose) 
END_EVENT_TABLE()

//#define DEBUG_VIEW_FRAME
//#define DEBUG_MDI_TABLE_FRAME_CLOSE
//#define DEBUG_PLOT_PERSISTENCY

//------------------------------------------------------------------------------
// MdiTableViewFrame(wxMDIParentFrame *parent, const wxString& title, ...)
//------------------------------------------------------------------------------
MdiTableViewFrame::MdiTableViewFrame(wxMDIParentFrame *parent,
                                     const wxString& plotName, const wxString& title,
                                     const wxPoint& pos, const wxSize& size,
                                     const long style)
                                     : GmatMdiChildFrame(parent, plotName, title, GmatTree::OUTPUT_SOLVER_WINDOW, -1,
                                       pos, size, style | wxNO_FULL_REPAINT_ON_RESIZE )
{
   mPlotTitle = plotName;

   // Give it an icon
#ifdef __WXMSW__
   SetIcon(wxIcon(_T("chrt_icn")));
#else
   SetIcon(wxIcon( mondrian_xpm ));
#endif
   Create();
   
#ifdef DEBUG_MDI_TABLE_FRAME_CLOSE
      MessageInterface::ShowMessage
         ("MdiTableViewFrame() created with plot name %s\n",
          mPlotTitle.WX_TO_C_STRING);
#endif
}


//------------------------------------------------------------------------------
// ~MdiTableViewFrame()
//------------------------------------------------------------------------------
MdiTableViewFrame::~MdiTableViewFrame()
{   
   #ifdef DEBUG_MDI_TABLE_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("~MdiTableViewFrame() mChildName=%s\n", mChildName.WX_TO_C_STRING);
   #endif
   
   // make sure GUI Listener Manager knows that there is one less window
   GuiListenerManager::ClosingSolverListener();

   #ifdef DEBUG_MDI_TABLE_FRAME_CLOSE
//   MessageInterface::ShowMessage
//      ("~MdiTableViewFrame() exiting\n",
//       MdiGlPlot::numChildren);
   #endif
#ifdef DEBUG_MDI_TABLE_FRAME_CLOSE
   MessageInterface::ShowMessage
   ("MdiTableViewFrame() DESTRUCTED with plot name %s\n",
    mPlotTitle.WX_TO_C_STRING);
#endif
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void MdiTableViewFrame::Create()
{
   #if DEBUG_PANEL_CREATE
      MessageInterface::ShowMessage("MdiTableViewFrame::Create() entered\n");
   #endif
   
   int gridWidth = -1;
   int gridHeight = -1; //100

   wxScrolledWindow *gridWindow = new wxScrolledWindow(this);

   variableGrid = new wxGrid(gridWindow, wxID_ANY);
   variableGrid->CreateGrid(0, 4);
   variableGrid->EnableEditing(false);
   variableGrid->EnableDragColMove(true);
   // variable header
   variableGrid->SetColLabelValue(0, "Control Variable");
   variableGrid->SetColLabelValue(1, "Current Value");
   variableGrid->SetColLabelValue(2, "Last Value");
   variableGrid->SetColLabelValue(3, "Difference");
   variableGrid->HideRowLabels();
   variableGrid->SetColSize(0, 200);
   variableGrid->SetColSize(1, variableGrid->GetColSize(0));
   variableGrid->SetColSize(2, variableGrid->GetColSize(0));
   variableGrid->SetColSize(3, variableGrid->GetColSize(0));

   constraintGrid = new wxGrid(gridWindow, wxID_ANY);
   constraintGrid->CreateGrid(0, 4);
   constraintGrid->EnableEditing(false);
   constraintGrid->EnableDragColMove();
   constraintGrid->SetColSizes(variableGrid->GetColSizes());
   // constraint header
   constraintGrid->SetColLabelValue(0, "Constraints");
   constraintGrid->SetColLabelValue(1, "Desired");
   constraintGrid->SetColLabelValue(2, "Achieved");
   constraintGrid->SetColLabelValue(3, "Difference");
   constraintGrid->HideRowLabels();

   objectiveGrid = new wxGrid(gridWindow, wxID_ANY);
   objectiveGrid->CreateGrid(0, 4);
   objectiveGrid->EnableEditing(false);
   objectiveGrid->EnableDragColMove();
   objectiveGrid->SetColSizes(variableGrid->GetColSizes());
   // objective header
   objectiveGrid->SetColLabelValue(0, "Objective Function");
   objectiveGrid->SetColLabelValue(1, "Current Value");
   objectiveGrid->SetColLabelValue(2, "Last Value");
   objectiveGrid->SetColLabelValue(3, "Difference");
   objectiveGrid->HideRowLabels();
   objectiveGrid->Hide();

   convergenceText = new wxBannerWindow(this, wxBOTTOM);
   SetConvergence(ITERATING);

   //-----------------------------------------------------------------
   // Add to grid and page sizer
   //-----------------------------------------------------------------
   gridSizer = new wxBoxSizer(wxVERTICAL);
   gridSizer->Add(variableGrid, 0, wxGROW|wxALIGN_CENTER|wxALL);
   gridSizer->AddSpacer(2);
   gridSizer->Add(constraintGrid, 0, wxGROW|wxALIGN_CENTER|wxALL);
   gridSizer->AddSpacer(2);
   gridSizer->Add(objectiveGrid, 0, wxGROW|wxALIGN_CENTER|wxALL);
   gridWindow->SetSizer(gridSizer);
   gridSizer->SetSizeHints(this);

   pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(gridWindow, 1, wxGROW|wxALIGN_CENTER|wxALL);
   pageSizer->AddSpacer(2);
   pageSizer->Add(convergenceText, 0, wxGROW|wxALIGN_CENTER|wxALL);
   pageSizer->SetSizeHints(this);
   SetSizer(pageSizer);   

   //Show the scrollbars
   gridWindow->SetMinClientSize(wxSize(700, 165));
   gridWindow->SetScrollRate(0, 5);
   
   #if DEBUG_PANEL_CREATE
   MessageInterface::ShowMessage("MdiTableViewFrame::Create() exiting...\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnChangeTitle(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiTableViewFrame::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
{
   wxString title = wxGetTextFromUser(_T("Enter the new title for MDI child"),
                                      _T(""),
                                      mPlotTitle, //s_title,
                                      GetParent()->GetParent());
   if ( !title )
      return;

   mPlotTitle = title;
   SetTitle(title);
}


//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiTableViewFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
   Close(TRUE);
}


//------------------------------------------------------------------------------
// void OnActivate(wxActivateEvent& event)
//------------------------------------------------------------------------------
void MdiTableViewFrame::OnActivate(wxActivateEvent& event)
{
   //if ( event.GetActive() && mCanvas )
   //   mCanvas->SetFocus();
   
   GmatMdiChildFrame::OnActivate(event);
}


//------------------------------------------------------------------------------
// void OnMove(wxMoveEvent& event)
//------------------------------------------------------------------------------
void MdiTableViewFrame::OnMove(wxMoveEvent& event)
{
   // Refresh canvas when frame moves (LOJ: 2011.09.16)
   // Implemented here so that when user moves scroll bar the plot will be repainted
   // Without this, OrbitView or GroundTrack plot shows only white background.
   //if ( mCanvas )
   //{
   //   // Do not use Refresh, it makes flickering
   //   mCanvas->Refresh(false);
   //   mCanvas->Update();
   //}
   //
   //// VZ: here everything is totally wrong under MSW, the positions are
   ////     different and both wrong (pos2 is off by 2 pixels for me which seems
   ////     to be the width of the MDI canvas border)
   //
   ////wxPoint pos1 = event.GetPosition();
   ////wxPoint pos2 = GetPosition();
   //
   ////wxLogStatus(GmatAppData::Instance()->GetMainFrame(),
   ////            wxT("position from event: (%d, %d), from frame (%d, %d)"),
   ////            pos1.x, pos1.y, pos2.x, pos2.y);
   
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void MdiTableViewFrame::OnClose(wxCloseEvent &event)
{
   #ifdef DEBUG_MDI_TABLE_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("\nMdiTableViewFrame::OnClose() '%s' entered, mCanClose=%d\n", mChildName.WX_TO_C_STRING, mCanClose);
   #endif
   
   GmatMdiChildFrame::OnClose(event);
   event.Skip();
   
   #ifdef DEBUG_MDI_TABLE_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("MdiTableViewFrame::OnClose() '%s' exiting, mCanClose=%d\n", mChildName.WX_TO_C_STRING, mCanClose);
   #endif
}


//------------------------------------------------------------------------------
// void OnPlotSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void MdiTableViewFrame::OnSize(wxSizeEvent& event)
{
   // VZ: under MSW the size event carries the client size (quite
   //     unexpectedly) *except* for the very first one which has the full
   //     size... what should it really be? TODO: check under wxGTK
   
   //wxSize size1 = event.GetSize();
   //wxSize size2 = GetSize();
   //wxSize size3 = GetClientSize();

   //wxLogStatus(GmatAppData::Instance()->GetMainFrame(),
   //            wxT("size from event: %dx%d, from frame %dx%d, client %dx%d"),
   //            size1.x, size1.y, size2.x, size2.y, size3.x, size3.y);
   
   // resize grids
   
   int w;
   int h;
   GetClientSize(&w, &h);
   variableGrid->SetColSize(0, w/4);
   w = variableGrid->GetColSize(0);
   variableGrid->SetColSize(1, w);
   variableGrid->SetColSize(2, w);
   variableGrid->SetColSize(3, w);

   constraintGrid->SetColSize(0, w);
   constraintGrid->SetColSize(1, w);
   constraintGrid->SetColSize(2, w);
   constraintGrid->SetColSize(3, w);

   objectiveGrid->SetColSize(0, w);
   objectiveGrid->SetColSize(1, w);
   objectiveGrid->SetColSize(2, w);
   objectiveGrid->SetColSize(3, w);
   
   event.Skip();
   
}


//------------------------------------------------------------------------------
// void TakeAction(const std::string &action)
//------------------------------------------------------------------------------
void MdiTableViewFrame::TakeAction(const std::string &action)
{
   //if (mCanvas)
   //   mCanvas->TakeAction(action);
}


//------------------------------------------------------------------------------
// void ObjectiveChanged(const std::string name, Real value)
//------------------------------------------------------------------------------
void MdiTableViewFrame::ObjectiveChanged(std::string name, Real value)
{
   SetConvergence(ITERATING);
   int aRow = -1;
   // find the row
   for (int i=0;i<objectiveGrid->GetNumberRows();i++)
   {
      if (objectiveGrid->GetCellValue(i,0) == name)
      {
         aRow = i;
         break;
      }
   }
   // if row not found, add a row
   if (aRow == -1)
   {
      if (!IsShown() && (GmatGlobal::Instance()->GetGuiMode() != GmatGlobal::MINIMIZED_GUI))
         Show();
      objectiveGrid->AppendRows();
      if (objectiveGrid->GetNumberRows() == 1)
         objectiveGrid->Show();
      aRow = objectiveGrid->GetNumberRows() - 1;
      objectiveGrid->SetCellValue(aRow, 0, name);
      objectiveGrid->SetCellValue(aRow, 1, GmatStringUtil::ToString(value));
      objectiveGrid->SetCellValue(aRow, 2, "");
      objectiveGrid->SetCellValue(aRow, 3, "");
      pageSizer->SetSizeHints(this);
      pageSizer->Layout();
   }
   else
   {
      // update row
      Real oldValue;
      GmatStringUtil::ToReal( objectiveGrid->GetCellValue(aRow, 1), &oldValue );
      objectiveGrid->SetCellValue(aRow, 1, GmatStringUtil::ToString(value));
      objectiveGrid->SetCellValue(aRow, 2, GmatStringUtil::ToString(oldValue));
      objectiveGrid->SetCellValue(aRow, 3, GmatStringUtil::ToString(value-oldValue));
      objectiveGrid->Update();
   }
}


//------------------------------------------------------------------------------
// void VariabledChanged(const std::string name, Real value)
//------------------------------------------------------------------------------
void MdiTableViewFrame::VariabledChanged(std::string name, Real value)
{
   SetConvergence(ITERATING);
   int aRow = -1;
   // find the row
   for (int i=0;i<variableGrid->GetNumberRows();i++)
   {
      if (variableGrid->GetCellValue(i,0) == name)
      {
         aRow = i; 
         break;
      }
   }
   // if row not found, add a row
   if (aRow == -1)
   {
      if (!IsShown() && (GmatGlobal::Instance()->GetGuiMode() != GmatGlobal::MINIMIZED_GUI))
         Show();
      variableGrid->AppendRows();
      if (!variableGrid->IsEditable())
         variableGrid->Enable(true);
      aRow = variableGrid->GetNumberRows() - 1;
      variableGrid->SetCellValue(aRow, 0, name);
      variableGrid->SetCellValue(aRow, 1, GmatStringUtil::ToString(value));
      variableGrid->SetCellValue(aRow, 2, "");
      variableGrid->SetCellValue(aRow, 3, "");
      pageSizer->SetSizeHints(this);
      pageSizer->Layout();
   }
   else
   {
      // update row
      Real oldValue;
      GmatStringUtil::ToReal( variableGrid->GetCellValue(aRow, 1), &oldValue );
      variableGrid->SetCellValue(aRow, 1, GmatStringUtil::ToString(value));
      variableGrid->SetCellValue(aRow, 2, GmatStringUtil::ToString(oldValue));
      variableGrid->SetCellValue(aRow, 3, GmatStringUtil::ToString(value-oldValue));
      variableGrid->Update();
   }
}


//------------------------------------------------------------------------------
// void VariabledChanged(const std::string name, std::string &value)
//------------------------------------------------------------------------------
void MdiTableViewFrame::VariabledChanged(std::string name, std::string &value)
{
   SetConvergence(ITERATING);
   int aRow = -1;
   // find the row
   for (int i=0;i<variableGrid->GetNumberRows();i++)
   {
      if (variableGrid->GetCellValue(i,0) == name)
      {
         aRow = i;
         break;
      }
   }
   // if row not found, add a row
   if (aRow == -1)
   {
      if (!IsShown() && (GmatGlobal::Instance()->GetGuiMode() != GmatGlobal::MINIMIZED_GUI))
         Show();
      variableGrid->AppendRows();
      aRow = variableGrid->GetNumberRows() - 1;
      variableGrid->SetCellValue(aRow, 0, name);
      variableGrid->SetCellValue(aRow, 1, value);
      variableGrid->SetCellValue(aRow, 2, "");
      variableGrid->SetCellValue(aRow, 3, "");
      pageSizer->SetSizeHints(this);
      pageSizer->Layout();
   }
   else
   {
      // update row
      variableGrid->SetCellValue(aRow, 2, variableGrid->GetCellValue(aRow, 1));
      variableGrid->SetCellValue(aRow, 1, value);
      variableGrid->Update();
   }
}


//------------------------------------------------------------------------------
// void ConstraintChanged(std::string name, Real desiredValue, Real value)
//------------------------------------------------------------------------------
void MdiTableViewFrame::ConstraintChanged(std::string name, Real desiredValue, 
   Real value, Integer condition)
{
   SetConvergence(ITERATING);
   int aRow = -1;
   switch (condition)
   {
      case 0:
         name = "(==) " + name;
         break;
      case -1:
         name = "(<=) " + name;
         break;
      case 1:
         name = "(=>) " + name;
         break;
      default:
         break;
   }
   // find the row
   for (int i=0;i<constraintGrid->GetNumberRows();i++)
   {
      if (constraintGrid->GetCellValue(i,0) == name)
      {
         aRow = i;
         break;
      }
   }
   // if row not found, add a row
   if (aRow == -1)
   {
      if (!IsShown() && (GmatGlobal::Instance()->GetGuiMode() != GmatGlobal::MINIMIZED_GUI))
         Show();
      constraintGrid->AppendRows();
      aRow = constraintGrid->GetNumberRows() - 1;
      constraintGrid->SetCellValue(aRow, 0, name);
      pageSizer->SetSizeHints(this);
      pageSizer->Layout();
   }
   // update row
   constraintGrid->SetCellValue(aRow, 1, GmatStringUtil::ToString(desiredValue));
   constraintGrid->SetCellValue(aRow, 2, GmatStringUtil::ToString(value));
   if (condition == 1)
      constraintGrid->SetCellValue(aRow, 3, GmatStringUtil::ToString(desiredValue-value));
   else
      constraintGrid->SetCellValue(aRow, 3, GmatStringUtil::ToString(value-desiredValue));
   constraintGrid->Update();
}


//------------------------------------------------------------------------------
// void Convergence(bool value, std::string info = "")
//------------------------------------------------------------------------------
void MdiTableViewFrame::Convergence(bool value, std::string info)
{
   if (value)
   {
      SetConvergence(CONVERGENCE, info);
   }
   else
   {
      SetConvergence(NO_CONVERGENCE, info);
   }
   if (!convergenceText->IsShown())
   {
      if (!IsShown() && (GmatGlobal::Instance()->GetGuiMode() != GmatGlobal::MINIMIZED_GUI))
         Show();
      convergenceText->Show();
      gridSizer->Layout();
   }
}


void MdiTableViewFrame::SetConvergence(ConvergenceType value, std::string info)
{
   if ((value == convergence) && (info == "")) return;

   convergence = value;
   switch (value)
   {
      case ITERATING:
         convergenceText->SetText("Iterating...","");
         convergenceText->SetGradient(*wxWHITE, *wxBLUE);
         break;
      case CONVERGENCE:
         convergenceText->SetText("CONVERGED", info);
         convergenceText->SetGradient(*wxWHITE, *wxGREEN);
         break;
      case NO_CONVERGENCE:
         convergenceText->SetText("NO CONVERGENCE", info);
         convergenceText->SetGradient(*wxWHITE, *wxRED);
         break;
   }
}
