//$Id$
//------------------------------------------------------------------------------
//                             PlotReceiver
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway, based on code written by Linda Jun
// Created: 2008/06/16
//
/**
 * Declares PlotReceiver class.
 */
//------------------------------------------------------------------------------


#include "PlotReceiver.hpp"

//------------------------------------------------------------------------------
// void SetViewType(GmatPlot::ViewType view)
//------------------------------------------------------------------------------
void PlotReceiver::SetViewType(GmatPlot::ViewType view)
{
   currentView = view;
}


//------------------------------------------------------------------------------
// ViewType GetViewType()
//------------------------------------------------------------------------------
GmatPlot::ViewType PlotReceiver::GetViewType()
{
   return currentView;
}


//------------------------------------------------------------------------------
// PlotReceiver()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
PlotReceiver::PlotReceiver()
{
   currentView = GmatPlot::ENHANCED_3D_VIEW;
}


//------------------------------------------------------------------------------
// ~PlotReceiver()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
PlotReceiver::~PlotReceiver()
{
}

