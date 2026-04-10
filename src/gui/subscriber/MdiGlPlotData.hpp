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
// Created: 2003/11/25
/**
 * Declares plot events and static data for opengl plot.
 */
//------------------------------------------------------------------------------
#ifndef MdiGlPlotData_hpp
#define MdiGlPlotData_hpp

#include "gmatwxdefs.hpp"
#include "ColorTypes.hpp"       // for GmatColor::
#include "RgbColor.hpp"
#include <string>
#include <map>

typedef std::map<wxString, RgbColor> wxStringColorMap;
typedef std::map<wxString, bool> wxStringBoolMap;

namespace GmatPlot
{
   // Event for OpenGlOptionDialog
   enum GlEventType
   {
      MDI_GL_QUIT = 500, //Do not change. Higher number doesn't work!!
      MDI_GL_OPEN_TRAJECTORY_FILE,
      MDI_GL_CHILD_ZOOM_IN,
      MDI_GL_CHILD_ZOOM_OUT,
      MDI_GL_CLEAR_PLOT,
      MDI_GL_CHANGE_TITLE,
      MDI_GL_CHILD_QUIT,
      MDI_GL_SHOW_DEFAULT_VIEW,
      MDI_GL_ZOOM_IN,
      MDI_GL_ZOOM_OUT,
      MDI_GL_VIEW_ANIMATION,
      
      MDI_GL_VIEW_OPTION,
      MDI_GL_SHOW_OPTION_PANEL,
      MDI_GL_SHOW_WIRE_FRAME,
      MDI_GL_SHOW_EQUATORIAL_PLANE,
      MDI_GL_HELP_VIEW
   };
};


struct MdiGlPlot
{
   static wxList mdiChildren;
   static int    numChildren;
   static bool   usePresetSize;
};

#endif
