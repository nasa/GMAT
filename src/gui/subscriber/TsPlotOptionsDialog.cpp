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
 * Implements the TsPlotOptionsDialog window.
 */
//------------------------------------------------------------------------------
 

#include "TsPlotOptionsDialog.hpp"
#include <sstream>

BEGIN_EVENT_TABLE(TsPlotOptionsDialog, wxDialog)
   EVT_CHECKBOX(ID_XMIN_CHECKBOX, TsPlotOptionsDialog::OnSettableXMinimum)
   EVT_CHECKBOX(ID_XMAX_CHECKBOX, TsPlotOptionsDialog::OnSettableXMaximum)
   EVT_CHECKBOX(ID_YMIN_CHECKBOX, TsPlotOptionsDialog::OnSettableYMinimum)
   EVT_CHECKBOX(ID_YMAX_CHECKBOX, TsPlotOptionsDialog::OnSettableYMaximum)
END_EVENT_TABLE()


TsPlotOptionsDialog::TsPlotOptionsDialog(const wxString &xLabel, 
                                     const wxString &yLabel, 
                                     wxWindow* parent, wxWindowID id, 
                                     const wxString& title, const wxPoint& pos, 
                                     const wxSize& size, long style, 
                                     const wxString& name) :
   wxDialog       (parent, id, title, pos, size, style, name),
   xName          (xLabel),
   yName          (yLabel)
{
   wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
   
//   theControls = new wxNotebook();
//   globalPage  = new wxNotebookPage();
//   curvePage   = new wxNotebookPage();

//   theControls->AddPage(globalPage, "Global Settings", true);
//   theControls->AddPage(curvePage, "Curves", false);

   // Lay out the three columns that appear in the top portion of the dialog
   wxBoxSizer *sideSizer = new wxBoxSizer(wxHORIZONTAL);

   wxStaticBoxSizer *leftSizer = new wxStaticBoxSizer(wxVERTICAL, this, "");
   wxStaticBoxSizer *xAxisSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      (xName + "-Axis Properties").c_str());
   wxStaticBoxSizer *yAxisSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      (yName + "-Axis Properties").c_str());
   
   // Set up the two panels that appear on the left of the dialog
   wxStaticBoxSizer *labelSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      "Plot Labels");
   wxStaticBoxSizer *lineSizer = new wxStaticBoxSizer(wxVERTICAL, this, 
      "Line Properties");
   
   leftSizer->Add(labelSizer, 0, wxALL, 2);
   leftSizer->Add(lineSizer, 0, wxALL, 2);

   // Set up the plot labels   
   wxBoxSizer *titleBox = new wxBoxSizer(wxHORIZONTAL);

   wxString xLabelStr = xName + " Label:";
   wxString yLabelStr = yName + " Label:";
   int w, h, wid;

   wxClientDC dc(this);
   wxString str = xLabelStr.c_str();
   dc.GetTextExtent(str, &w, &h);
   wid = w;
   dc.GetTextExtent(wxString(yLabelStr.c_str()), &w, &h);
   if (w > wid)
      wid = w;
   dc.GetTextExtent(wxString("Plot Title:"), &w, &h);
   if (w > wid)
      wid = w;

   wxSize lExtent(w,h);

   titleBox->Add(new wxStaticText(this, -1, "Plot Title:", 
      wxDefaultPosition, lExtent, wxALIGN_RIGHT), 0, 
      wxALL | wxALIGN_CENTER_VERTICAL, 5);
   //titleBox->AddStretchSpacer();
   plotTitle = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(150,-1));
   titleBox->Add(plotTitle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *xlabelBox = new wxBoxSizer(wxHORIZONTAL);
   xlabelBox->Add(new wxStaticText(this, -1, xLabelStr.c_str(), 
      wxDefaultPosition, lExtent, wxALIGN_RIGHT), 0, 
      wxALL | wxALIGN_CENTER_VERTICAL, 5);
   //xlabelBox->AddStretchSpacer();
   xAxisLabel = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(150,-1));
   xlabelBox->Add(xAxisLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *ylabelBox = new wxBoxSizer(wxHORIZONTAL);
   ylabelBox->Add(new wxStaticText(this, -1, yLabelStr.c_str(), 
      wxDefaultPosition, lExtent, wxALIGN_RIGHT), 0, 
      wxALL | wxALIGN_CENTER_VERTICAL, 5);
   //ylabelBox->AddStretchSpacer();
   yAxisLabel = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxSize(150,-1));
   ylabelBox->Add(yAxisLabel, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   labelSizer->Add(titleBox, 0, wxALL, 5);
   labelSizer->Add(xlabelBox, 0, wxALL, 5);
   labelSizer->Add(ylabelBox, 0, wxALL, 5);

   // Set up the line style options
   wxBoxSizer *lineWidthBox = new wxBoxSizer(wxHORIZONTAL);
   lineWidthBox->Add(new wxStaticText(this, -1, "Line Width:"), 0, 
                                  wxALL | wxALIGN_CENTER_VERTICAL, 5);
   lineWidth = new wxSpinCtrl(this, -1, "1");
   lineWidth->SetRange(1,10);
   lineWidthBox->Add(lineWidth, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *lineStyleBox = new wxBoxSizer(wxHORIZONTAL);
   lineStyleBox->Add(new wxStaticText(this, -1, "Line Style:"), 0, 
                                  wxALL | wxALIGN_CENTER_VERTICAL, 5);
   lineStyle = new wxTextCtrl(this, -1);
   lineStyle->Enable(false);
   lineStyleBox->Add(lineStyle, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
   
   lineSizer->Add(lineWidthBox, 0, wxALL, 2); 
   lineSizer->Add(lineStyleBox, 0, wxALL, 2); 
   
   // Set up the X-axis options
   wxBoxSizer *xminBox = new wxBoxSizer(wxHORIZONTAL);
   userXMinimum = new wxCheckBox(this, ID_XMIN_CHECKBOX, 
                                 (xName + " Minimum:  ").c_str());
   xminBox->Add(userXMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xMinimum = new wxTextCtrl(this, -1, "0.0");
   xminBox->Add(xMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *xmaxBox = new wxBoxSizer(wxHORIZONTAL);
   userXMaximum = new wxCheckBox(this, ID_XMAX_CHECKBOX, 
                                 (xName + " Maximum:  ").c_str());
   xmaxBox->Add(userXMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xMaximum = new wxTextCtrl(this, -1, "10.0");
   xmaxBox->Add(xMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   xLogScale = new wxCheckBox(this, -1, 
                              ("Logarithmic " + xName + " Axis").c_str());
   xLogScale->Enable(false);

   wxBoxSizer *xTickBox = new wxBoxSizer(wxHORIZONTAL);
   xTickBox->Add(new wxStaticText(this, -1, 
                                  ("Number of " + xName + " Ticks:").c_str()), 
                                  0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xTickCount = new wxSpinCtrl(this, -1, "6");
   xTickCount->SetRange(1,20);
   xTickBox->Add(xTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *xMinorTickBox = new wxBoxSizer(wxHORIZONTAL);
   xMinorTickBox->Add(new wxStaticText(this, -1, 
                                   ("# of Minor " + xName + " Ticks:").c_str()), 
                                   0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xMinorTickCount = new wxTextCtrl(this, -1);
   xMinorTickCount->Enable(false);
   xMinorTickBox->Add(xMinorTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   xMinorTickLines = new wxCheckBox(this, -1, 
                                    ("Draw Minor " + xName + " Lines").c_str());
   xMinorTickLines->Enable(false);

   wxBoxSizer *xPrecisionBox = new wxBoxSizer(wxHORIZONTAL);
   xPrecisionBox->Add(new wxStaticText(this, -1, 
                                   (xName + " Label Precision:").c_str()), 0, 
                                   wxALL | wxALIGN_CENTER_VERTICAL, 0);
   xPrecision = new wxSpinCtrl(this, -1, "6");
   xPrecision->SetRange(2,16);
   xPrecisionBox->Add(xPrecision, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

   xAxisSizer->Add(xminBox, 0, wxALL, 5);
   xAxisSizer->Add(xmaxBox, 0, wxALL, 5);
   xAxisSizer->Add(xLogScale, 0, wxALL, 5);
   xAxisSizer->Add(xTickBox, 0, wxALL, 5);
   xAxisSizer->Add(xMinorTickBox, 0, wxALL, 5);
   xAxisSizer->Add(xMinorTickLines, 0, wxALL, 5);
   xAxisSizer->Add(xPrecisionBox, 0, wxALL, 5);

   // Set up the Y-axis options
   wxBoxSizer *yminBox = new wxBoxSizer(wxHORIZONTAL);
   userYMinimum = new wxCheckBox(this, ID_YMIN_CHECKBOX, 
                                 (yName + " Minimum:  ").c_str());
   yminBox->Add(userYMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yMinimum = new wxTextCtrl(this, -1, "0.0");
   yminBox->Add(yMinimum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *ymaxBox = new wxBoxSizer(wxHORIZONTAL);
   userYMaximum = new wxCheckBox(this, ID_YMAX_CHECKBOX, 
                                 (yName + " Maximum:  ").c_str());
   ymaxBox->Add(userYMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yMaximum = new wxTextCtrl(this, -1, "10.0");
   ymaxBox->Add(yMaximum, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   yLogScale = new wxCheckBox(this, -1, 
                              ("Logarithmic " + yName + " Axis").c_str());
   yLogScale->Enable(false);

   wxBoxSizer *yTickBox = new wxBoxSizer(wxHORIZONTAL);
   yTickBox->Add(new wxStaticText(this, -1, 
                                  ("Number of " + yName + " Ticks:").c_str()), 0, 
                                  wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yTickCount = new wxSpinCtrl(this, -1, "4");
   yTickCount->SetRange(1,25);
   yTickBox->Add(yTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   wxBoxSizer *yMinorTickBox = new wxBoxSizer(wxHORIZONTAL);
   yMinorTickBox->Add(new wxStaticText(this, -1, 
                                   ("# of Minor " + yName + " Ticks:").c_str()), 
                                   0, wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yMinorTickCount = new wxTextCtrl(this, -1);
   yMinorTickCount->Enable(false);
   yMinorTickBox->Add(yMinorTickCount, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

   yMinorTickLines = new wxCheckBox(this, -1, 
                                    ("Draw Minor " + yName + " Lines").c_str());
   yMinorTickLines->Enable(false);

   wxBoxSizer *yPrecisionBox = new wxBoxSizer(wxHORIZONTAL);
   yPrecisionBox->Add(new wxStaticText(this, -1, 
                                   (yName + " Label Precision:").c_str()), 0, 
                                   wxALL | wxALIGN_CENTER_VERTICAL, 0);
   yPrecision = new wxSpinCtrl(this, -1, "6");
   yPrecision->SetRange(2,16);
   yPrecisionBox->Add(yPrecision, 0, wxALL | wxALIGN_CENTER_VERTICAL, 0);

   yAxisSizer->Add(yminBox, 0, wxALL, 5);
   yAxisSizer->Add(ymaxBox, 0, wxALL, 5);
   yAxisSizer->Add(yLogScale, 0, wxALL, 5);
   yAxisSizer->Add(yTickBox, 0, wxALL, 5);
   yAxisSizer->Add(yMinorTickBox, 0, wxALL, 5);
   yAxisSizer->Add(yMinorTickLines, 0, wxALL, 5);
   yAxisSizer->Add(yPrecisionBox, 0, wxALL, 5);

   // Pull it all together and put buttons at the bottom   
   sideSizer->Add(leftSizer, 0, wxALL, 2);
   sideSizer->Add(xAxisSizer, 1, wxALL, 2);
   sideSizer->Add(yAxisSizer, 1, wxALL, 2);

   // Configure buttons for bottom of dialog
   wxStaticBoxSizer *buttons = new wxStaticBoxSizer(wxHORIZONTAL, this);
   buttons->AddStretchSpacer();
   buttons->Add(CreateButtonSizer(wxOK), 0, wxALL | wxALIGN_CENTER, 5);
   buttons->Add(80, 0, 0);
   buttons->Add(CreateButtonSizer(wxCANCEL), 0, wxALL | wxALIGN_CENTER, 5);
   buttons->AddStretchSpacer();

//   topSizer->Add(theControls, 1, wxALL | wxGROW, 5);
   topSizer->Add(sideSizer, 1, wxALL | wxGROW, 5);
   topSizer->Add(buttons, 0, wxALL | wxGROW, 5);

   SetSizer(topSizer);
   topSizer->Fit(this);
}

TsPlotOptionsDialog::~TsPlotOptionsDialog()
{
}

void TsPlotOptionsDialog::UpdateLabels()
{
}

wxString TsPlotOptionsDialog::GetPlotTitle()
{
   return plotTitle->GetValue();
}

wxString TsPlotOptionsDialog::GetXLabel()
{
   return xAxisLabel->GetValue();
}

wxString TsPlotOptionsDialog::GetYLabel()
{
   return yAxisLabel->GetValue();
}

void TsPlotOptionsDialog::SetPlotTitle(const wxString &str)
{
   plotTitle->SetValue(str);
}

void TsPlotOptionsDialog::SetXLabel(const wxString &str)
{
   xAxisLabel->SetValue(str);
}

void TsPlotOptionsDialog::SetYLabel(const wxString &str)
{
   yAxisLabel->SetValue(str);
}

int TsPlotOptionsDialog::GetWidth()
{
   return lineWidth->GetValue();
}

int TsPlotOptionsDialog::GetStyle()
{
   return wxSOLID;
}

void TsPlotOptionsDialog::SetWidth(int lw)
{
   lineWidth->SetValue(lw);
}

void TsPlotOptionsDialog::SetStyle(int ls)
{
}

void TsPlotOptionsDialog::SetXPrecision(int xPrec)
{
   xPrecision->SetValue(xPrec);
}

void TsPlotOptionsDialog::SetYPrecision(int yPrec)
{
   yPrecision->SetValue(yPrec);
}

int TsPlotOptionsDialog::GetXPrecision()
{
   return xPrecision->GetValue();
}

int TsPlotOptionsDialog::GetYPrecision()
{
   return yPrecision->GetValue();
}

bool TsPlotOptionsDialog::GetXMinState()
{
   return userXMinimum->IsChecked();
}

bool TsPlotOptionsDialog::GetXMaxState()
{
   return userXMaximum->IsChecked();
}

bool TsPlotOptionsDialog::GetYMinState()
{
   return userYMinimum->IsChecked();
}

bool TsPlotOptionsDialog::GetYMaxState()
{
   return userYMaximum->IsChecked();
}

void TsPlotOptionsDialog::SetXMinState(bool st)
{
   userXMinimum->SetValue(st);
   xMinimum->Enable(st);
}

void TsPlotOptionsDialog::SetXMaxState(bool st)
{
   userXMaximum->SetValue(st);
   xMaximum->Enable(st);
}

void TsPlotOptionsDialog::SetYMinState(bool st)
{
   userYMinimum->SetValue(st);
   yMinimum->Enable(st);
}

void TsPlotOptionsDialog::SetYMaxState(bool st)
{
   userYMaximum->SetValue(st);
   yMaximum->Enable(st);
}

double TsPlotOptionsDialog::GetXMin()
{
   std::stringstream str;
   str.precision(xPrecision->GetValue());
   double rv;
   str << xMinimum->GetValue().c_str();
   str >> rv;
   return rv;
}

double TsPlotOptionsDialog::GetXMax()
{
   std::stringstream str;
   str.precision(xPrecision->GetValue());
   double rv;
   str << xMaximum->GetValue().c_str();
   str >> rv;
   return rv;
}

double TsPlotOptionsDialog::GetYMin()
{
   std::stringstream str;
   str.precision(yPrecision->GetValue());
   double rv;
   str << yMinimum->GetValue().c_str();
   str >> rv;
   return rv;
}

double TsPlotOptionsDialog::GetYMax()
{
   std::stringstream str;
   str.precision(yPrecision->GetValue());
   double rv;
   str << yMaximum->GetValue().c_str();
   str >> rv;
   return rv;
}

void TsPlotOptionsDialog::SetXMin(double st)
{
   std::stringstream str;
   str.precision(xPrecision->GetValue());
   str << st;
   xMinimum->SetValue(str.str().c_str());
}

void TsPlotOptionsDialog::SetXMax(double st)
{
   std::stringstream str;
   str.precision(xPrecision->GetValue());
   str << st;
   xMaximum->SetValue(str.str().c_str());
}

void TsPlotOptionsDialog::SetYMin(double st)
{
   std::stringstream str;
   str.precision(yPrecision->GetValue());
   str << st;
   yMinimum->SetValue(str.str().c_str());
}

void TsPlotOptionsDialog::SetYMax(double st)
{
   std::stringstream str;
   str.precision(yPrecision->GetValue());
   str << st;
   yMaximum->SetValue(str.str().c_str());
}

void TsPlotOptionsDialog::SetXName(const wxString &nomme)
{
   xName = nomme;
}

void TsPlotOptionsDialog::SetYName(const wxString &nomme)
{
   yName = nomme;
}


void TsPlotOptionsDialog::SetXTickCount(int count)
{
   xTickCount->SetValue(count);
}


int TsPlotOptionsDialog::GetXTickCount()
{
   return xTickCount->GetValue();
}


void TsPlotOptionsDialog::SetYTickCount(int count)
{
   yTickCount->SetValue(count);
}


int TsPlotOptionsDialog::GetYTickCount()
{
   return yTickCount->GetValue();
}


void TsPlotOptionsDialog::OnSettableXMinimum(wxCommandEvent &event)
{
   xMinimum->Enable(userXMinimum->GetValue());
}


void TsPlotOptionsDialog::OnSettableXMaximum(wxCommandEvent &event)
{
   xMaximum->Enable(userXMaximum->GetValue());
}


void TsPlotOptionsDialog::OnSettableYMinimum(wxCommandEvent &event)
{
   yMinimum->Enable(userYMinimum->GetValue());
}


void TsPlotOptionsDialog::OnSettableYMaximum(wxCommandEvent &event)
{
   yMaximum->Enable(userYMaximum->GetValue());
}
