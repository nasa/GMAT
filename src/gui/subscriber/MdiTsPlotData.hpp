//$Id$
//------------------------------------------------------------------------------
//                              MdiGlPlotData
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
// ** Legal **
//
// Author: Linda Jun
// Created: 2004/01/20
/**
 * Declares plot events and static data for XY plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiTsPlotData_hpp
#define MdiTsPlotData_hpp

#include "gmatwxdefs.hpp"

namespace GmatPlot
{
   enum TsEventType
   {
      MDI_TS_QUIT = 600, //Do not change. Higher number doesn't work!!
      MDI_TS_OPEN_PLOT_FILE,
      MDI_TS_CLEAR_PLOT,
      MDI_TS_CHANGE_TITLE,
      MDI_TS_CHILD_QUIT,
      MDI_TS_SHOW_DEFAULT_VIEW,
      MDI_TS_VIEW_OPTION,
      MDI_TS_DRAW_GRID,
      MDI_TS_DRAW_DOTTED_LINE,
      MDI_TS_HELP_VIEW
   };
}

struct MdiTsPlot
{
   static wxList mdiChildren;
   static int    numChildren;
   static bool   usePresetSize;
};

#endif
