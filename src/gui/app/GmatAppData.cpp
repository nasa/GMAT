//$Id$
//------------------------------------------------------------------------------
//                              GmatAppData
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/10/29
// Modified: 
//    2010.03.01 Thomas Grubb 
//       - Open configuration file (GMAT.ini)
//
/**
 * defines application data.
 */
//------------------------------------------------------------------------------
#include "GmatAppData.hpp"
#include "GuiInterpreter.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include <wx/confbase.h>
#include <wx/fileconf.h>

#if !defined __CONSOLE_APP__
#include "ViewTextFrame.hpp"
#include "ResourceTree.hpp"
#include "MissionTree.hpp"
#include "OutputTree.hpp"
#include "GmatMainFrame.hpp"
#include "FileUtil.hpp"            // for GmatFileUtil::DoesFileExist()
#endif

//#define DEBUG_SET_ICON
//#define DEBUG_GUI_CONFIG

GmatAppData* GmatAppData::theGmatAppData = NULL;

//------------------------------------------------------------------------------
// GmatAppData* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain the singleton.
 *
 * @return the singleton instance of the GmatAppData class.
 */
//------------------------------------------------------------------------------
GmatAppData* GmatAppData::Instance()
{
   if (!theGmatAppData)
   {
      theGmatAppData = new GmatAppData;
   }
   
   return theGmatAppData;
}


//------------------------------------------------------------------------------
// GuiInterpreter* GetGuiInterpreter()
//------------------------------------------------------------------------------
GuiInterpreter* GmatAppData::GetGuiInterpreter()
{
   return theGuiInterpreter;
}


//------------------------------------------------------------------------------
// void SetGuiInterpreter(GuiInterpreter *guiInterp)
//------------------------------------------------------------------------------
void GmatAppData::SetGuiInterpreter(GuiInterpreter *guiInterp)
{
   theGuiInterpreter = guiInterp;
}


#if !defined __CONSOLE_APP__

//------------------------------------------------------------------------------
// void SetMainFrame(GmatMainFrame *mainFrame)
//------------------------------------------------------------------------------
void GmatAppData::SetMainFrame(GmatMainFrame *mainFrame)
{
   theMainFrame = mainFrame;
}


//------------------------------------------------------------------------------
// GmatMainFrame* GetMainFrame()
//------------------------------------------------------------------------------
GmatMainFrame* GmatAppData::GetMainFrame()
{
   return theMainFrame;
}


//------------------------------------------------------------------------------
// void SetResourceTree(ResourceTree *resourceTree)
//------------------------------------------------------------------------------
void GmatAppData::SetResourceTree(ResourceTree *resourceTree)
{
   theResourceTree = resourceTree;
}


//------------------------------------------------------------------------------
// ResourceTree* GetResourceTree()
//------------------------------------------------------------------------------
ResourceTree* GmatAppData::GetResourceTree()
{
   return theResourceTree;
}


//------------------------------------------------------------------------------
// wxConfigBase* GetPersonalizationConfig()
//------------------------------------------------------------------------------
wxConfigBase* GmatAppData::GetPersonalizationConfig()
{
   #ifdef DEBUG_GUI_CONFIG
   MessageInterface::ShowMessage
      ("GmatAppData::GetPersonalizationConfig() entered, thePersonalizationConfig = <%p>\n",
       thePersonalizationConfig);
   #endif
   
   if (thePersonalizationConfig == NULL)
   {
      // Find personalization file from the search path (LOJ: 2014.07.02)
      //std::string pfile = FileManager::Instance()->GetFullPathname(FileManager::PERSONALIZATION_FILE);
//      std::string pfile = FileManager::Instance()->FindPath("", "PERSONALIZATION_FILE", true, false, false);
      std::string pfile = FileManager::Instance()->FindPath("", "PERSONALIZATION_FILE", true, true, true);
      #ifdef DEBUG_GUI_CONFIG
      MessageInterface::ShowMessage("   pfile = '%s'\n", pfile.c_str());
      #endif
      
      // fix crash for invalid filename
      //if (!FileManager::Instance()->DoesDirectoryExist(pfile))
      if (pfile == "")
      {
         // Show actual OS-specific directory in the message?
//         if (GmatGlobal::Instance()->IsWritingFilePathInfo())
//         {
            MessageInterface::ShowMessage
               ( "*** WARNING *** Invalid personalization file specified, \n"
                 "so reading/writing local configuration file in an OS-specific "
                 "location (see User's Guide for more information).\n",
                 pfile.c_str() );
//         }
         
         // Make blank pfile so that default local configureation file can be written
         // to user's home directory  (See NOTE below on location of file)
         // If it is changed to use GMAT.ini it will update GMAT.ini in bin directory
         // which we don't want it to happen. (LOJ: 2014.07.08)
         pfile = "";
         //pfile = "GMAT.ini";
      }
      
      //Excerpt from wxConfigBase (http://docs.wxwidgets.org/2.8/wx_wxconfigbase.html#wxconfigbasector)
      /*On non-VMS Unix systems, the default local configuration file is ~/.appname.  However, this path may be also used as user data directory (see wxStandardPaths::GetUserDataDir) if the application has several data files. In this case wxCONFIG_USE_SUBDIR flag, which changes the default local configuration file to ~/.appname/appname should be used. Notice that this flag is ignored on non-Unix system, including VMS, or if a non-default localFilename is provided. This function is new since wxWidgets version 2.8.2
       */
      // Currently it is created in home directory such as C:\Users\ljun\GMAT.ini
      // On Mac, if the file name is blank or does not exist, it will read/update the file
      // '/Users/<your-username-here>/Library/Preferences/GMAT Preferences'
      
      #ifdef DEBUG_GUI_CONFIG
         MessageInterface::ShowMessage("   pfile before making new wxFileConfig = '%s'\n",
         pfile.c_str());
      #endif
      thePersonalizationConfig = new wxFileConfig(wxEmptyString, wxEmptyString, pfile.c_str(),
                                 wxEmptyString, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_RELATIVE_PATH);
   }
   
   #ifdef DEBUG_GUI_CONFIG
      MessageInterface::ShowMessage
         ("GmatAppData::GetPersonalizationConfig() returning <%p>\n   path = '%s'\n",
         thePersonalizationConfig, thePersonalizationConfig->GetPath().WX_TO_C_STRING);
   #endif
   return thePersonalizationConfig;
}


//------------------------------------------------------------------------------
// void SetMissionTree(MissionTree *missionTree)
//------------------------------------------------------------------------------
void GmatAppData::SetMissionTree(MissionTree *missionTree)
{
   theMissionTree = missionTree;
}


//------------------------------------------------------------------------------
// MissionTree* GetMissionTree()
//------------------------------------------------------------------------------
MissionTree* GmatAppData::GetMissionTree()
{
   return theMissionTree;
}


//------------------------------------------------------------------------------
// void SetOutputTree(OutputTree *outputTree)
//------------------------------------------------------------------------------
void GmatAppData::SetOutputTree(OutputTree *outputTree)
{
   theOutputTree = outputTree;
}


//------------------------------------------------------------------------------
// OutputTree* GetOutputTree()
//------------------------------------------------------------------------------
OutputTree* GmatAppData::GetOutputTree()
{
   return theOutputTree;
}


//------------------------------------------------------------------------------
// void SetMessageWindow(ViewTextFrame *msgFrame)
//------------------------------------------------------------------------------
void GmatAppData::SetMessageWindow(ViewTextFrame *frame)
{
   theMessageWindow = frame;
}


//------------------------------------------------------------------------------
// ViewTextFrame* GetMessageWindow()
//------------------------------------------------------------------------------
ViewTextFrame* GmatAppData::GetMessageWindow()
{
   return theMessageWindow;
}


//------------------------------------------------------------------------------
// void SetCompareWindow(ViewTextFrame *frame)
//------------------------------------------------------------------------------
void GmatAppData::SetCompareWindow(ViewTextFrame *frame)
{
   theCompareWindow = frame;
}


//------------------------------------------------------------------------------
// ViewTextFrame* GetCompareWindow()
//------------------------------------------------------------------------------
ViewTextFrame* GmatAppData::GetCompareWindow()
{
   return theCompareWindow;
}


//------------------------------------------------------------------------------
// void SetMessageTextCtrl(wxTextCtrl *msgTextCtrl)
//------------------------------------------------------------------------------
void GmatAppData::SetMessageTextCtrl(wxTextCtrl *msgTextCtrl)
{
   theMessageTextCtrl = msgTextCtrl;
}


//------------------------------------------------------------------------------
// wxTextCtrl* GetMessageTextCtrl()
//------------------------------------------------------------------------------
wxTextCtrl* GmatAppData::GetMessageTextCtrl()
{
   return theMessageTextCtrl;
}


//------------------------------------------------------------------------------
// void SetFont(wxFont font)
//------------------------------------------------------------------------------
void GmatAppData::SetFont(wxFont font)
{
   theFont = font;
}

//------------------------------------------------------------------------------
// wxFont GetFont()
//------------------------------------------------------------------------------
wxFont GmatAppData::GetFont()
{
   return theFont;
}

//------------------------------------------------------------------------------
// void SetScriptFont(wxFont font)
//------------------------------------------------------------------------------
void GmatAppData::SetScriptFont(wxFont font)
{
   theScriptFont = font;
}

//------------------------------------------------------------------------------
// wxFont GetScriptFont()
//------------------------------------------------------------------------------
wxFont GmatAppData::GetScriptFont()
{
   return theScriptFont;
}

//------------------------------------------------------------------------------
// void SetFontSize(Integer size)
//------------------------------------------------------------------------------
void GmatAppData::SetFontSize(Integer size)
{
   theFontSize = size;
}

//------------------------------------------------------------------------------
// Integer GetFontSize()
//------------------------------------------------------------------------------
Integer GmatAppData::GetFontSize()
{
   return theFontSize;
}

//------------------------------------------------------------------------------
// void SetScriptFontSize(Integer size)
//------------------------------------------------------------------------------
void GmatAppData::SetScriptFontSize(Integer size)
{
   theScriptFontSize = size;
}

//------------------------------------------------------------------------------
// Integer GetScriptFontSize()
//------------------------------------------------------------------------------
Integer GmatAppData::GetScriptFontSize()
{
   return theScriptFontSize;
}


//------------------------------------------------------------------------------
// void SetTempScriptName(const wxString &tempName)
//------------------------------------------------------------------------------
void GmatAppData::SetTempScriptName(const wxString &tempName)
{
   theTempScriptName = tempName;
}


//------------------------------------------------------------------------------
// wxString GetTempScriptName()
//------------------------------------------------------------------------------
wxString GmatAppData::GetTempScriptName()
{
   return theTempScriptName;
}


//------------------------------------------------------------------------------
// bool SetIcon(wxTopLevelWindow *topWindow, const std::string &calledFrom)
//------------------------------------------------------------------------------
/**
 * Sets icon to Dialog, MdiChildFrame, MdiParentFrame, etc
 *
 * @param  topWindow  The top level window of icon to be set
 *
 * @return true If icon was successfully set, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatAppData::SetIcon(wxTopLevelWindow *topWindow, const std::string &calledFrom)
{
   #ifdef DEBUG_SET_ICON
   MessageInterface::ShowMessage
      ("GmatAppData::SetIcon() entered, called from '%s' named '%s'\n   "
       "theIconFile = '%s', theIconFileSet = %d\n", calledFrom.c_str(), topWindow->GetName().WX_TO_C_STRING,
       theIconFile.WX_TO_C_STRING, theIconFileSet);
   #endif
   
   // Write non-existent icon file warning per GMAT session
   // Changed to write no warning (LOJ: 2014.09.19)
   static bool writeWarning = false;
   FileManager *fm = FileManager::Instance();
   
   if (theIconFile == "")
   {
      // Set icon if icon file can be located
      //theIconFile = fm->GetFullPathname("MAIN_ICON_FILE").c_str();
      theIconFile = fm->FindMainIconFile(writeWarning || theIconFileSet).c_str();
      
      #ifdef DEBUG_SET_ICON
      if (theIconFile != "")
         MessageInterface::ShowMessage("   theIconFile = '%s'\n", theIconFile.WX_TO_C_STRING);
      #endif
      
      // Write warning message if it is still blank
      if (theIconFile == "")
      {
         if (writeWarning)
         {
            MessageInterface::ShowMessage
               ("*** WARNING *** Error setting icon for window '%s' named '%s'\n   "
                "Cannot find the icon file '%s'.  This warning message will be written only once.\n",
                calledFrom.c_str(), topWindow->GetName().WX_TO_C_STRING,
                fm->GetFilename("MAIN_ICON_FILE").c_str());
            writeWarning = false;
         }
         
         #ifdef DEBUG_SET_ICON
         MessageInterface::ShowMessage("GmatAppData::SetIcon() returning false\n");
         #endif
         return false;
      }
   }
   
   // Set icon
   #if defined __WXMSW__
      topWindow->SetIcon(wxIcon(theIconFile, wxBITMAP_TYPE_ICO));
   #elif defined __WXGTK__
      topWindow->SetIcon(wxIcon(theIconFile, wxBITMAP_TYPE_XPM));
   #elif defined __WXMAC__
//      This causes an error with wx3.0 any time a new window is brought up.
//      Removing the wxBitmap type from the call fixes the issue.
//      topWindow->SetIcon(wxIcon(theIconFile, wxBITMAP_TYPE_PICT_RESOURCE));
      topWindow->SetIcon(wxIcon(theIconFile));
   #endif
   
   #ifdef DEBUG_SET_ICON
      MessageInterface::ShowMessage("GmatAppData::SetIcon() returning true\n");
   #endif
      
   return true;
}

//------------------------------------------------------------------------------
// void ResetIconFile()
//------------------------------------------------------------------------------
/**
 * Resets icon file and set flag.  This method is usually called from the Moderator
 * when new script is read.
 */
//------------------------------------------------------------------------------
void GmatAppData::ResetIconFile()
{
   #ifdef DEBUG_SET_ICON
   MessageInterface::ShowMessage("GmatAppData::ResetIconFile() entered\n");
   #endif
   
   theIconFileSet = false;
   theIconFile = "";
   
   SetIconFile();
   
   #ifdef DEBUG_SET_ICON
   MessageInterface::ShowMessage("GmatAppData::ResetIconFile() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void SetIconFile()
//------------------------------------------------------------------------------
/**
 * Sets icon file from the search path. It will look in the GMAT working directory
 * and then path specified in the GMAT startup file. This method needs to be called
 * after startup file is read.
 */
//------------------------------------------------------------------------------
void GmatAppData::SetIconFile()
{
   #ifdef DEBUG_SET_ICON
   MessageInterface::ShowMessage("GmatAppData::SetIconFile() entered\n");
   #endif
   
   // Set icon file from the search path (LOJ: 2014.07.02)
   FileManager *fm = FileManager::Instance();
   theIconFile = fm->FindMainIconFile().c_str();
   
   if (theIconFile != "")
      theIconFileSet = true;
   
   #ifdef DEBUG_SET_ICON
   MessageInterface::ShowMessage
      ("GmatAppData::SetIconFile() leaving\n   theIconFile = '%s'\n", theIconFile.WX_TO_C_STRING);
   #endif
}

//------------------------------------------------------------------------------
// wxString GetIconFile()
//------------------------------------------------------------------------------
wxString GmatAppData::GetIconFile()
{
   return theIconFile;
}

//------------------------------------------------------------------------------
// GmatAppData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
GmatAppData::GmatAppData()
{
#if !defined __CONSOLE_APP__
   theMainFrame = NULL;
   theResourceTree = NULL;
   theMissionTree = NULL;
   theOutputTree = NULL;
   theMessageWindow = NULL;
   theCompareWindow = NULL;
   theMessageTextCtrl = NULL;
   theFontSize = 8;
   theScriptFontSize = 9;
   theTempScriptName = "$gmattempscript$.script";
   thePersonalizationConfig = NULL;
   theIconFileSet = false;
   
   #ifdef __USE_EDITOR__
   thePageSetupDialogData = NULL;
   #endif
   
   // Set font
   theFont = wxFont(theFontSize, wxMODERN, wxNORMAL, wxNORMAL);
   theScriptFont = wxFont(theScriptFontSize, wxMODERN, wxNORMAL, wxNORMAL);
   
   // Set the global wx config, read from local directory (GMAT.ini)
   wxFileConfig *pConfig = new wxFileConfig(wxEmptyString, wxEmptyString, "GMAT.ini", 
           wxEmptyString, wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_RELATIVE_PATH);
   wxConfigBase::Set(pConfig);
      
#endif
}


//------------------------------------------------------------------------------
// ~GmatAppData()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GmatAppData::~GmatAppData()
{
}

#endif

