//$Id$
//------------------------------------------------------------------------------
//                             GmatMenuBar
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
// Author: Allison Greene
// Created: 2006/05/09
/**
 * This class provides the menu bar for the main frame.
 */
//------------------------------------------------------------------------------
#ifndef GmatMenuBar_hpp
#define GmatMenuBar_hpp

#include "gmatwxdefs.hpp"
#include "GmatTreeItemData.hpp"  // for GmatTree::

class GmatMenuBar : public wxMenuBar
{
public:
   // constructors
   GmatMenuBar(GmatTree::ItemType itemType, wxMenu *windowMenu, long style = 0);
   void UpdateRecentMenu(wxArrayString files);
   
protected:
private:
   void CreateMenu(GmatTree::ItemType itemType, wxMenu *windowMenu);
   
   DECLARE_EVENT_TABLE();

};


namespace GmatMenu
{
   // IDs for the controls and the menu commands
   enum
   {
      MENU_FILE_NEW = 10000,
      MENU_FILE_NEW_SCRIPT,
      MENU_FILE_OPEN_SCRIPT,
      MENU_FILE_OPEN_RECENT_SCRIPT1,
      MENU_FILE_OPEN_RECENT_SCRIPT2,
      MENU_FILE_OPEN_RECENT_SCRIPT3,
      MENU_FILE_OPEN_RECENT_SCRIPT4,
      MENU_FILE_OPEN_RECENT_SCRIPT5,
      MENU_FILE_SAVE_SCRIPT,
      MENU_FILE_SAVE_SCRIPT_AS,
      MENU_LOAD_DEFAULT_MISSION,
      MENU_EMPTY_PROJECT,
      MENU_FILE_PRINT_SETUP,
      MENU_FILE_PRINT,
      MENU_PROJECT_EXIT,
      
      MENU_PREFERENCES_FONT,
      MENU_SET_PATH_AND_LOG,
      
      MENU_EDIT_UNDO = 11000,
      MENU_EDIT_REDO,
      MENU_EDIT_CUT,
      MENU_EDIT_COPY,
      MENU_EDIT_PASTE,
      MENU_EDIT_COMMENT,
      MENU_EDIT_UNCOMMENT,
      MENU_EDIT_SELECT_ALL,
      
      MENU_EDIT_FIND,
      MENU_EDIT_FIND_NEXT,
      MENU_EDIT_REPLACE,
      MENU_EDIT_REPLACE_NEXT,
      MENU_EDIT_LINE_NUMBER,
      MENU_EDIT_GOTO_LINE,
      MENU_EDIT_INDENT_MORE,
      MENU_EDIT_INDENT_LESS,
      
      MENU_SCRIPT_OPEN_EDITOR = 12000,
      MENU_SCRIPT_BUILD_OBJECT,
      MENU_SCRIPT_RUN,
      MENU_SCRIPT_BUILD_AND_RUN,
      
      MENU_TOOLS_FILE_COMPARE = 13000,
      MENU_TOOLS_GEN_TEXT_EPHEM_FILE,
      
      TOOL_RUN = 14000,
      TOOL_PAUSE,
      TOOL_RESUME,
      TOOL_STOP,
      TOOL_BUILD,
      
      TOOL_CLOSE_CHILDREN,
      TOOL_CLOSE_CURRENT,
      TOOL_SCRIPT,
      
      TOOL_SCREENSHOT,
      
      TOOL_ANIMATION_PLAY = 15000,
      TOOL_ANIMATION_STOP,
      TOOL_ANIMATION_FAST,
      TOOL_ANIMATION_SLOW,
      TOOL_ANIMATION_OPTIONS,

	  TOOL_NEXT_TAB,
	  TOOL_PREV_TAB,

      TOOL_DO_NOTHING_1 = 15500,
      TOOL_DO_NOTHING_2,
      TOOL_DO_NOTHING_3,
      
      MENU_MATLAB_OPEN = 16000,
      MENU_MATLAB_CLOSE,
      MENU_MATLAB_SERVER_START,
      MENU_MATLAB_SERVER_STOP,

      MENU_HELP_TOPICS = 17000,
      MENU_HELP_WELCOME,
      MENU_HELP_HELP,
      MENU_HELP_USING_GMAT,
      MENU_HELP_TUTORIAL,
      MENU_HELP_ISSUE,
      MENU_HELP_FEEDBACK,
      MENU_HELP_UPDATE,
      
      // it is important for the id corresponding to the "About" command to have
      // this standard value as otherwise it won't be handled properly under Mac
      // (where it is special and put into the "Apple" menu)
      MENU_HELP_ABOUT = wxID_ABOUT,
   };
   
};
#endif // GmatMenuBar_hpp
