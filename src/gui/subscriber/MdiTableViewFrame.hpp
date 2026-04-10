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
 * Declares MdiChildViewFrame for 3D visualization.
 */
//------------------------------------------------------------------------------
#ifndef MdiChildViewFrame_hpp
#define MdiChildViewFrame_hpp

#include "GmatMdiChildFrame.hpp"
#include "IChangeListener.hpp"
#include "ISolverListener.hpp"
#include "gmatdefs.hpp"
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

class MdiTableViewFrame: public GmatMdiChildFrame, ISolverListener
{
public:
   enum ConvergenceType
   {
      ITERATING, CONVERGENCE, NO_CONVERGENCE
   };
   MdiTableViewFrame(wxMDIParentFrame *parent, const wxString& plotName,
                     const wxString& title, const wxPoint& pos,
                     const wxSize& size, const long style);
   virtual ~MdiTableViewFrame();
   virtual void ConstraintChanged(std::string name, Real desiredValue, 
      Real value, Integer condition = 0);
   virtual void Convergence(bool value, std::string info = "");
   virtual void ObjectiveChanged(std::string name, Real value);
   virtual void VariabledChanged(std::string name, Real value);
   virtual void VariabledChanged(std::string name, std::string &value);
   
   // setters
   virtual void SetConvergence(ConvergenceType value, std::string info = "");

   /// AWB Extension - unused in GMAT
   virtual void SetIterationNumber(std::string name, Integer value) {}
   
   //virtual void SetIsNewFrame(bool flag);
   
   // actions
   
   // menu actions
   virtual void OnChangeTitle(wxCommandEvent& event);
   //virtual void OnShowDefaultView(wxCommandEvent& event);
   
   virtual void OnQuit(wxCommandEvent& event);
   
   virtual void OnActivate(wxActivateEvent& event);
   virtual void OnMove(wxMoveEvent& event);
   virtual void OnClose(wxCloseEvent &event);
   virtual void OnSize(wxSizeEvent& event);
   
   virtual void TakeAction(const std::string &action);

      
protected:
   wxGrid *variableGrid;
   wxGrid *constraintGrid;
   wxGrid *objectiveGrid;
   wxBoxSizer *gridSizer;
   wxBoxSizer *pageSizer;
   wxScrolledWindow *scrollWindow;
   wxBannerWindow *convergenceText;
   wxString mPlotTitle;
   ConvergenceType convergence;
   bool mInFunction;
   
   void CheckFrame();
   virtual void Create();
   
   DECLARE_EVENT_TABLE()
};
#endif
