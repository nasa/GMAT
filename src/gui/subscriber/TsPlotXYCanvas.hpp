// $Id$
//------------------------------------------------------------------------------
//                            TsPlotXYCanvas
//------------------------------------------------------------------------------
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2007/05/26
//
// This code is Copyright (c) 2007, Thinking Systems, Inc.
// Licensed under the Academic Free License version 3.0.
/**
 * Declares the TsPlotXYCanvas window.
 */
//------------------------------------------------------------------------------


#ifndef TSPLOTXYCANVAS_HPP_
#define TSPLOTXYCANVAS_HPP_

#include "TsPlotCanvas.hpp"

class TsPlotXYCanvas : public TsPlotCanvas
{
public:
	TsPlotXYCanvas(wxWindow* parent, wxWindowID id = -1,
                  const wxPoint& pos = wxPoint(1,1),
                  const wxSize& size = wxDefaultSize,
                  long style = wxTAB_TRAVERSAL,
                  const wxString& name = "");
	virtual    ~TsPlotXYCanvas();

protected:
   virtual void DrawAxes(wxDC &dc);
   virtual void DrawLabels(wxDC &dc);
   virtual void PlotData(wxDC &dc);
   virtual void Rescale(wxDC &dc);
};

#endif /*TSPLOTXYCANVAS_HPP_*/
