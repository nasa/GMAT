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
// ** Legal **
//
// Author: Allison Greene
// Created: 2006/05/09
/**
 * This class provides the menu bar for the main frame.
 */
//------------------------------------------------------------------------------
#include "GmatMenuBar.hpp"
#include "GmatTreeItemData.hpp"
#include "GmatGlobal.hpp"        // for GetRunMode()
#include "MessageInterface.hpp"
#include "GmatAppData.hpp"
#include <wx/config.h>

#define __ADD_CLOSE_TO_WINDOW_MENU__
//#define __SHOW_EPHEM_FILE__

//#define DEBUG_MENUBAR 1

using namespace GmatMenu;
//------------------------------------------------------------------------------
// EVENT_TABLE(GmatMenuBar, wxMenuBar)
//------------------------------------------------------------------------------
/**
 * Events Table for the menu and tool bar
 *
 * @note Indexes event handler functions.
 */
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(GmatMenuBar, wxMenuBar)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// GmatMenuBar(GmatTree::ItemType itemType, wxMenu *windowMenu,
//             wxMenu *newWinMenu, long style)
//------------------------------------------------------------------------------
GmatMenuBar::GmatMenuBar(GmatTree::ItemType itemType, wxMenu *windowMenu,
                         long style)
   : wxMenuBar(style)
{
   CreateMenu(itemType, windowMenu);
}


//------------------------------------------------------------------------------
// void CreateMenu(GmatTree::ItemType itemType, wxMenu *windowMenu)
//------------------------------------------------------------------------------
/**
 * Adds items to the menu.
 *
 * @param <itemType> input item type from GmatTree::ItemType
 * @param <windowMenu> input Window menu pointer. This is available on Windows only
 *
 */
//------------------------------------------------------------------------------
void GmatMenuBar::CreateMenu(GmatTree::ItemType itemType, wxMenu *windowMenu)
{
   //Integer runMode = GmatGlobal::Instance()->GetRunMode();             // made changes by TUAN NGUYEN
   Integer runMode = GmatGlobal::Instance()->GetRunModeStartUp();        // made changes by TUAN NGUYEN
   
   #ifdef DEBUG_MENUBAR
   MessageInterface::ShowMessage
      ("GmatMenuBar::CreateMenu() itemType=%d, windowMenu=%p, runMode=%d\n",
       itemType, windowMenu, runMode);
   #endif
   
   //-----------------------------------------------------------------
   // File menu
   //-----------------------------------------------------------------   
   wxMenu *fileMenu = new wxMenu;
   wxMenu *newMenu = new wxMenu;
   newMenu->Append(MENU_FILE_NEW_SCRIPT, wxT("Script\tCtrl+Shift+N"));
   newMenu->Append(MENU_LOAD_DEFAULT_MISSION, wxT("Mission\tCtrl+N"));
   fileMenu->Append(MENU_FILE_NEW, wxT("New"), newMenu, wxT(""));
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_FILE_OPEN_SCRIPT, wxT("&Open...\tCtrl+O"), wxT(""));   
   fileMenu->Append(MENU_FILE_OPEN_RECENT_SCRIPT1, wxT("Open &Recent"), wxT(""));
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_FILE_SAVE_SCRIPT, wxT("&Save...\tCtrl+S"), wxT(""));
   fileMenu->Append(MENU_FILE_SAVE_SCRIPT_AS, wxT("Save As...\tF12"), wxT(""));
   
   if (runMode == GmatGlobal::TESTING || runMode == GmatGlobal::TESTING_NO_PLOTS)
   {
      fileMenu->AppendSeparator();
      fileMenu->Append(MENU_EMPTY_PROJECT, wxT("Empty Project"), wxT(""));   
      fileMenu->AppendSeparator();
      
      fileMenu->Append(MENU_SET_PATH_AND_LOG, wxT("Set File Paths..."), wxT(""));
   }
   
   #ifdef __ENABLE_PRINT__
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_FILE_PRINT_SETUP, wxT("Print Setup..."), wxT(""));
   fileMenu->Append(MENU_FILE_PRINT, wxT("Print...\tCtrl+P"), wxT(""));
   #endif //__ENABLE_PRINT__
   
   fileMenu->AppendSeparator();
   fileMenu->Append(MENU_PROJECT_EXIT, wxT("Exit"), wxT(""));
   #ifdef __WXMAC__
       wxApp::s_macExitMenuItemId = MENU_PROJECT_EXIT;
   #endif
   
   this->Append(fileMenu, wxT("&File"));
   
   //-----------------------------------------------------------------
   // Edit menu
   //-----------------------------------------------------------------
   wxMenu *editMenu = new wxMenu;
   editMenu->Append(MENU_EDIT_UNDO, wxT("Undo\tCtrl+Z"), wxT(""));
   editMenu->Append(MENU_EDIT_REDO, wxT("Redo\tCtrl+Y"), wxT(""));
   editMenu->AppendSeparator();
   editMenu->Append(MENU_EDIT_CUT, wxT("Cut\tCtrl+X"), wxT(""));
   editMenu->Append(MENU_EDIT_COPY, wxT("Copy\tCtrl+C"), wxT(""));
   editMenu->Append(MENU_EDIT_PASTE, wxT("Paste\tCtrl+V"), wxT(""));
   editMenu->AppendSeparator();
   editMenu->Append(MENU_EDIT_COMMENT, wxT("Comment\tCtrl+R"), wxT(""));
   editMenu->Append(MENU_EDIT_UNCOMMENT, wxT("Uncomment\tCtrl+T"), wxT(""));
   editMenu->Append(MENU_EDIT_SELECT_ALL, wxT("Select All\tCtrl+A"), wxT(""));
   
   #ifdef __USE_STC_EDITOR__
   editMenu->AppendSeparator();
   editMenu->Append(MENU_EDIT_FIND, _("&Find and Replace...\tCtrl+F"));
   editMenu->Append(MENU_EDIT_FIND_NEXT, _("Find &next\tF3"));
   //editMenu->Append(MENU_EDIT_REPLACE, _("&Replace...\tCtrl+F"));
   //editMenu->Append(MENU_EDIT_REPLACE_NEXT, _("Find and &Replace\tCtrl+H"));
   editMenu->AppendSeparator();
   editMenu->AppendCheckItem(MENU_EDIT_LINE_NUMBER, _("Show line &numbers"));
   editMenu->Append(MENU_EDIT_GOTO_LINE, _("&Goto\tCtrl+G"));
   editMenu->AppendSeparator();
   editMenu->Append(MENU_EDIT_INDENT_MORE, _("&Indent more\tCtrl+I"));
   editMenu->Append(MENU_EDIT_INDENT_LESS, _("I&ndent less\tCtrl+Shift+I"));
   
   // default is to show line numbers, so put check mark
   editMenu->Check(MENU_EDIT_LINE_NUMBER, true);
   
   #endif // __USE_STC_EDITOR__
   
   this->Append(editMenu, wxT("&Edit"));
   
   //-----------------------------------------------------------------
   // Tools menu
   //-----------------------------------------------------------------
   if (runMode == GmatGlobal::TESTING || runMode == GmatGlobal::TESTING_NO_PLOTS)
   {
      wxMenu *toolsMenu = new wxMenu;
      toolsMenu->Append(MENU_TOOLS_FILE_COMPARE, wxT("Compare Files"), wxT(""));
      
      #ifdef __SHOW_EPHEM_FILE__
      toolsMenu->Append(MENU_TOOLS_GEN_TEXT_EPHEM_FILE, wxT("Generate Text Ephemeris File"), wxT(""));
      #endif
      
      this->Append(toolsMenu, wxT("Tools"));
   }
   
   //-----------------------------------------------------------------
   // Help menu
   //-----------------------------------------------------------------
   
   wxMenu *helpMenu = new wxMenu;
   helpMenu->Append(MENU_HELP_WELCOME, wxT("Welcome Page\tCtrl+F1"), wxT(""));
   helpMenu->AppendSeparator();
   helpMenu->Append(MENU_HELP_HELP, wxT("Help\tF1"), wxT(""));
   helpMenu->AppendSeparator();
   helpMenu->Append(MENU_HELP_USING_GMAT, wxT("Using GMAT"), wxT(""));
   helpMenu->Append(MENU_HELP_TUTORIAL, wxT("Tutorials"), wxT(""));
   helpMenu->AppendSeparator();
   if (runMode == GmatGlobal::TESTING)
      helpMenu->Append(MENU_HELP_UPDATE, wxT("Check for Updates..."), wxT(""));
   helpMenu->Append(MENU_HELP_ISSUE, wxT("Report an Issue"), wxT(""));
   helpMenu->Append(MENU_HELP_FEEDBACK, wxT("Provide Feedback"), wxT(""));
   helpMenu->AppendSeparator();
   helpMenu->Append(MENU_HELP_ABOUT, wxT("About GMAT"), wxT(""));
   this->Append(helpMenu, wxT("Help"));
   
   //-----------------------------------------------------------------
   // Window menu
   //-----------------------------------------------------------------
   // @note: In order for system Window menu to work, do not call
   // SetMenuBar() from GmatMdiChildFrame after theMenuBar is created.
   #ifdef __ADD_CLOSE_TO_WINDOW_MENU__
   //-------------------------------------------------------
   // If on Windows, use Window menu from MdiParenentFrame
   // otherwise, create Window menu
   //-------------------------------------------------------
   
   wxMenu *winMenu =(wxMenu*)NULL;
   bool createWindowMenu = true;
   bool prependClose = false;
   
   #ifdef __WXMSW__
   if (windowMenu != NULL)
   {
      createWindowMenu = false;
      winMenu = windowMenu;
      if (winMenu->FindItem("Close All") == wxNOT_FOUND)
         prependClose = true;
   }
   #endif
   
   #ifdef DEBUG_MENUBAR
   MessageInterface::ShowMessage
     ("GmatMenuBar::CreateMenu() All menus created. Now creating WindowMenu...\n"
       "   this=%p, winMenu=%p, createWindowMenu=%d, prependClose=%d\n", this,
       winMenu, createWindowMenu, prependClose);
   #endif
   
   if (createWindowMenu)
   {
      #ifdef DEBUG_MENUBAR
      MessageInterface::ShowMessage("===> creating Window menu\n");
      #endif
      winMenu = new wxMenu;
      winMenu->Append(TOOL_CLOSE_CHILDREN, wxT("Close All"), wxT(""));
      winMenu->Append(TOOL_CLOSE_CURRENT, wxT("Close"), wxT(""));
      
      // Insert before Help menu
      int helpPos = FindMenu("Help");
      this->Insert(helpPos, winMenu, wxT("Window"));
   }
   else if (prependClose)
   {
      #ifdef DEBUG_MENUBAR
      MessageInterface::ShowMessage("===> prepending Close menu item\n");
      #endif
      winMenu->PrependSeparator();
      winMenu->Prepend(TOOL_CLOSE_CURRENT, wxT("Close"), wxT(""));      
      winMenu->Prepend(TOOL_CLOSE_CHILDREN, wxT("Close All"), wxT(""));
   }
   #endif
   
   wxArrayString files;
   wxString aFilename;

   // get the config object
   wxConfigBase *pConfig = GmatAppData::Instance()->GetPersonalizationConfig();
   pConfig->SetPath(wxT("/RecentFiles"));

   // read filenames from config object
   size_t i=0;
   while (pConfig->Read(wxString::Format(wxT("%d"), (int)i), &aFilename)) 
   {
      files.Add(aFilename);
      i++;
   }
   UpdateRecentMenu(files);

   #ifdef DEBUG_MENUBAR
   MessageInterface::ShowMessage("GmatMenuBar::CreateMenu() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void UpdateRecentMenu(wxArrayString files)
//------------------------------------------------------------------------------
/*
 * Updates the File->Open Recent-> menu
 */
//------------------------------------------------------------------------------
void GmatMenuBar::UpdateRecentMenu(wxArrayString files)
{
#ifdef DEBUG_MENUBAR
   MessageInterface::ShowMessage("===> updating recent files menu, number of files = %d\n", files.GetCount());
#endif
   // get the Recent menu
   wxMenu *fileMenu = GetMenu(0);
   size_t recentIndex = fileMenu->FindItem(wxT("Open Recent"));
   // clear the Recent menu
   fileMenu->Destroy(recentIndex);

   // create a new recent menu
   wxMenu *recentMenu = new wxMenu;
   if (files.GetCount() > 4)
      recentMenu->Append(MENU_FILE_OPEN_RECENT_SCRIPT5, files[4]+"\tCtrl+Shift+1");
   if (files.GetCount() > 3)
      recentMenu->Append(MENU_FILE_OPEN_RECENT_SCRIPT4, files[3]+"\tCtrl+Shift+2");
   if (files.GetCount() > 2)
      recentMenu->Append(MENU_FILE_OPEN_RECENT_SCRIPT3, files[2]+"\tCtrl+Shift+3");
   if (files.GetCount() > 1)
      recentMenu->Append(MENU_FILE_OPEN_RECENT_SCRIPT2, files[1]+"\tCtrl+Shift+4");
   if (files.GetCount() > 0)
      recentMenu->Append(MENU_FILE_OPEN_RECENT_SCRIPT1, files[0]+"\tCtrl+Shift+5");

   wxMenuItem *recentMenuItem = fileMenu->Insert(3, MENU_FILE_OPEN_RECENT_SCRIPT1, wxT("Open Recent"), recentMenu, wxT(""));
   if (recentMenu->GetMenuItemCount() == 0)
      recentMenuItem->Enable(false);
}


