// $Id$
//------------------------------------------------------------------------------
//                           TsPlotOptionsDialog
//------------------------------------------------------------------------------
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2005/05/20
//
// This code is Copyright (c) 2005, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Declares the TsPlotOptionsDialog window.
 */
//------------------------------------------------------------------------------


#ifndef PLOTOPTIONSDIALOG_HPP_
#define PLOTOPTIONSDIALOG_HPP_

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/notebook.h>

#include <string>


class TsPlotOptionsDialog : public wxDialog
{
public:
	TsPlotOptionsDialog(const wxString &xLabel, const wxString &yLabel,
                     wxWindow* parent, wxWindowID id, 
                     const wxString& title = "TS Plot Options", 
                     const wxPoint& pos = wxDefaultPosition, 
                     const wxSize& size = wxSize(780,380), 
                     long style = wxDEFAULT_DIALOG_STYLE, 
                     const wxString& name = "TSPlotOptions");
	virtual ~TsPlotOptionsDialog();
   
   // Accessors
   wxString          GetPlotTitle();
   wxString          GetXLabel();
   wxString          GetYLabel();
   int               GetWidth();
   int               GetStyle();
   void              SetPlotTitle(const wxString &str);
   void              SetXLabel(const wxString &str);
   void              SetYLabel(const wxString &str);
   void              SetWidth(int lw);
   void              SetStyle(int ls);
   
   void              SetXPrecision(int xPrec);
   void              SetYPrecision(int yPrec);
   int               GetXPrecision();
   int               GetYPrecision();
   
   bool              GetXMinState();
   bool              GetXMaxState();
   bool              GetYMinState();
   bool              GetYMaxState();
   
   void              SetXMinState(bool st);
   void              SetXMaxState(bool st);
   void              SetYMinState(bool st);
   void              SetYMaxState(bool st);
   
   double            GetXMin();
   double            GetXMax();
   double            GetYMin();
   double            GetYMax();
   
   void              SetXMin(double st);
   void              SetXMax(double st);
   void              SetYMin(double st);
   void              SetYMax(double st);
   
   void              SetXName(const wxString &nomme);
   void              SetYName(const wxString &nomme);
   void              UpdateLabels();

   void              SetXTickCount(int count);
   int               GetXTickCount();
   void              SetYTickCount(int count);
   int               GetYTickCount();
   
//   void              SetXMin(double st);
//   void              SetXMax(double st);
   
protected:
//   wxBoxSizer           *labels;
   
//   wxNotebook           *theControls;
//   wxNotebookPage       *globalPage;
//   wxNotebookPage       *curvePage;

   // Controls that are accessed to set plot properties
   wxTextCtrl           *plotTitle;
   wxTextCtrl           *xAxisLabel;
   wxTextCtrl           *yAxisLabel;
   wxSpinCtrl           *lineWidth;
   wxTextCtrl           *lineStyle;
   
   // X axis options
   wxCheckBox           *userXMinimum;
   wxTextCtrl           *xMinimum;
   wxCheckBox           *userXMaximum;
   wxTextCtrl           *xMaximum;
   wxCheckBox           *xLogScale;
   wxSpinCtrl           *xTickCount;
   wxTextCtrl           *xMinorTickCount;
   wxCheckBox           *xMinorTickLines;
   wxSpinCtrl           *xPrecision;

   // Y axis options
   wxCheckBox           *userYMinimum;
   wxTextCtrl           *yMinimum;
   wxCheckBox           *userYMaximum;
   wxTextCtrl           *yMaximum;
   wxCheckBox           *yLogScale;
   wxSpinCtrl           *yTickCount;
   wxTextCtrl           *yMinorTickCount;
   wxCheckBox           *yMinorTickLines;
   wxSpinCtrl           *yPrecision;
   
   wxString             xName;
   wxString             yName;
   
   void OnSettableXMinimum(wxCommandEvent &event);
   void OnSettableXMaximum(wxCommandEvent &event);
   void OnSettableYMinimum(wxCommandEvent &event);
   void OnSettableYMaximum(wxCommandEvent &event);

//private:
   DECLARE_EVENT_TABLE()
   
      // IDs for the controls and the menu commands
   enum
   {     
      ID_XMIN_CHECKBOX = 44400,
      ID_XMAX_CHECKBOX,
      ID_YMIN_CHECKBOX,
      ID_YMAX_CHECKBOX
   };
   
};

#endif /*PLOTOPTIONSDIALOG_HPP_*/
