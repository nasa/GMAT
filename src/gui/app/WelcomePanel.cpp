//$Id$
//------------------------------------------------------------------------------
//                            WelcomePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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
// number NNG04CC06P.
//
//
// Author: Thomas Grubb
// Created: 2010/09/09
//------------------------------------------------------------------------------
#include "WelcomePanel.hpp"
#include "MessageInterface.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "GuiInterpreter.hpp"
#include "FileManager.hpp"         // for GetFullPathname()
#include "FileUtil.hpp"            // for DoesDirectoryExist()
#include "StringUtil.hpp"          // for Replace()
#include "GmatAppData.hpp"         // for SetIcon(), etc
#include "GmatMainFrame.hpp"
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/config.h>
#include <wx/hyperlink.h>
#include <wx/statline.h>
#include <wx/gdicmn.h>

//#define DEBUG_FILL_GROUP
//#define DEBUG_SAMPLE_SCRIPT
//#define DEBUG_WELCOME_HELP

//------------------------------
// event tables for wxWindows
//------------------------------
BEGIN_EVENT_TABLE(WelcomePanel, wxFrame)
   EVT_HYPERLINK(ID_BUTTON_RECENT, WelcomePanel::OnOpenRecentScript)
   EVT_HYPERLINK(ID_FILE, WelcomePanel::OnOpenSampleScript)
   EVT_HYPERLINK(ID_HELP, WelcomePanel::OnOpenHelpLink)
   EVT_CHECKBOX(ID_CHECKBOX, WelcomePanel::OnShowWelcomePanelClicked)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// WelcomePanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs WelcomePanel object.
 */
//------------------------------------------------------------------------------
WelcomePanel::WelcomePanel(wxFrame *frame, const wxString& title,
      int x, int y, int w, int h)
: wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h),
          // wxRESIZE_BOX is no longer in wxWidgets-3.0
          //(wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxRESIZE_BOX | wxMAXIMIZE_BOX)) | wxFRAME_FLOAT_ON_PARENT)
          (wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxMAXIMIZE_BOX)) | wxFRAME_FLOAT_ON_PARENT)
{
   SetBackgroundColour(wxColor(240, 240, 240));
   Create();
   CenterOnScreen(wxBOTH);
   SetFocus();
}

//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void WelcomePanel::Create()
{
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("WelcomePanel::Create() entered\n");
   #endif
   
   wxStaticLine *line1;
   wxBitmap bitmap;
   wxBitmapButton *aboutButton;
   // Border size
   bsize = 4;
   
   // get the config object
   wxFileConfig *pConfig;
   
   // Load GMAT ICON
   FileManager *fm = FileManager::Instance();
   std::string iconFile = fm->GetFullPathname("ICON_PATH") + "GMATIcon.jpg";
   
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("WelcomePanel::Create() iconFile='%s'\n", iconFile.c_str());
   #endif
   
   if (GmatFileUtil::DoesFileExist(iconFile))
   {
      #ifdef DEBUG_CREATE
      MessageInterface::ShowMessage
         ("   Loading iconFile '%s'\n", iconFile.c_str());
      #endif
      bitmap = LoadBitmap( iconFile.c_str(), 200, 200 );
      aboutButton = new wxBitmapButton(this, -1, bitmap, wxDefaultPosition,
                          wxSize(200,200));
   }
   else
   {
      #ifdef DEBUG_CREATE
      MessageInterface::ShowMessage
         ("   the iconFile '%s' doesnot exist, so creating default\n", iconFile.c_str());
      #endif
      wxBitmap nullMap;
      aboutButton = new wxBitmapButton(this, -1, nullMap, wxDefaultPosition,
                           wxSize(200,200));
   }
   
   wxColourDatabase cdb;
   wxColour gmatColor = cdb.Find("NAVY");
   // title, build date
   wxStaticText *gmatText =
      new wxStaticText(this, -1, "General Mission Analysis Tool");
   wxFont font1 = wxFont();

   #ifdef __WXMAC__
   font1.SetPointSize(20);
   #else
   font1.SetPointSize(20);
   #endif

   font1.SetWeight(wxFONTWEIGHT_BOLD);
   gmatText->SetOwnFont(font1);
   gmatText->SetOwnForegroundColour(gmatColor);

   // website and contact email
   wxStaticText *webText = new wxStaticText(this, -1, "Website: ");
   wxString gmatUrl = "https://sourceforge.net/projects/gmat/";
   wxHyperlinkCtrl *webLink = new wxHyperlinkCtrl(this, -1, gmatUrl, gmatUrl);
   
   wxFlexGridSizer *contactSizer = new wxFlexGridSizer(2);
   contactSizer->Add(webText, 0, wxALIGN_RIGHT|wxALL, 2);
   contactSizer->Add(webLink, 0, wxALIGN_LEFT|wxALL, 2);
   
   wxBoxSizer *gmatTextSizer = new wxBoxSizer(wxVERTICAL);
   gmatTextSizer->Add(gmatText, 0, wxALIGN_LEFT|wxALL, bsize);
   gmatTextSizer->Add(3, 3);
   gmatTextSizer->Add(contactSizer, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, bsize);
   
   wxBoxSizer *headerSizer = new wxBoxSizer(wxHORIZONTAL);
   headerSizer->Add(aboutButton, 0, wxALIGN_LEFT|wxALL, bsize);
   headerSizer->Add(gmatTextSizer, 0, wxALIGN_LEFT|wxALL, bsize);

   //-----------------------------------------------------------------
   // now Links and Resources
   //-----------------------------------------------------------------
   wxStaticText *resourcesText =
      new wxStaticText(this, -1, "Links and Resources");
   resourcesText->SetOwnFont(font1);
   resourcesText->SetOwnForegroundColour(gmatColor);
   wxBoxSizer *resourcesSizer = new wxBoxSizer(wxVERTICAL);
   resourcesSizer->Add(resourcesText, 0, wxALIGN_LEFT|wxALL, bsize);
   resourcesSizer->AddSpacer(bsize*2);
   // now Links
   pConfig = (wxFileConfig *) wxConfigBase::Get();
   wxString configLocWelcome = "/Welcome/Links";
   #ifdef __WXMAC__
      configLocWelcome = configLocWelcome + "/Online";
   #endif
   resourcesSizer->Add(FillGroup(pConfig, configLocWelcome, "", 3, ID_HELP, false),
                       0, wxALIGN_LEFT|wxALL, bsize*2);

   //-----------------------------------------------------------------
   // now Sample Projects
   //-----------------------------------------------------------------
   resourcesSizer->Add(FillGroup(pConfig, "/Welcome/Samples", "", 3, ID_FILE, false),
                       0, wxALIGN_LEFT|wxALL, bsize*2);

   //-----------------------------------------------------------------
   // now Getting Started
   //-----------------------------------------------------------------
   wxStaticText *getStartedText =
      new wxStaticText(this, -1, "Getting Started");
   getStartedText->SetOwnFont(font1);
   getStartedText->SetOwnForegroundColour(gmatColor);
   wxBoxSizer *gettingStartedSizer = new wxBoxSizer(wxVERTICAL);
   gettingStartedSizer->Add(getStartedText, 0, wxALIGN_LEFT|wxALL, bsize);
   gettingStartedSizer->AddSpacer(bsize*2);
   wxString configLoc = "/GettingStarted/Tutorials";
   #ifdef __WXMAC__
      configLoc = configLoc + "/Online";
   #endif
   gettingStartedSizer->Add(FillGroup(pConfig, configLoc, "/GettingStarted/Tutorials/Icons",
                                      1, ID_HELP, false), 0, wxALIGN_LEFT|wxALL, bsize*2);
   //gettingStartedSizer->AddSpacer(bsize*2);

   //-----------------------------------------------------------------
   // now recent scripts
   //-----------------------------------------------------------------
   wxStaticText *recentScriptsText =
      new wxStaticText(this, -1, "Recent Scripts");
   recentScriptsText->SetOwnFont(font1);
   recentScriptsText->SetOwnForegroundColour(gmatColor);
   wxBoxSizer *recentSizer = new wxBoxSizer(wxVERTICAL);
   recentSizer->Add(recentScriptsText, 0, wxALIGN_LEFT|wxALL, bsize);
   recentSizer->AddSpacer(bsize*2);
   // add the recent scripts
   pConfig = (wxFileConfig *) GmatAppData::Instance()->GetPersonalizationConfig();
   recentSizer->Add(FillGroup(pConfig, "/RecentFiles", "", 1, ID_BUTTON_RECENT, true, true),
                    0, wxALIGN_LEFT|wxALL, bsize*2);


   //-----------------------------------------------------------------
   // Now put it all together
   //-----------------------------------------------------------------
   wxBoxSizer *contentSizer = new wxBoxSizer(wxHORIZONTAL);
   contentSizer->Add(recentSizer, 0, wxALIGN_LEFT|wxALL, bsize*2);
   line1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
   contentSizer->Add(line1, 0, wxGROW|wxALIGN_CENTRE, 20);
   contentSizer->Add(gettingStartedSizer, 1, wxGROW|wxALIGN_LEFT|wxALL, bsize*2);

   wxSizer *theMiddleSizer = (wxSizer*)(new wxBoxSizer(wxVERTICAL));
   theMiddleSizer->Add(headerSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize*2);
   line1 = new wxStaticLine(this);
   theMiddleSizer->Add(line1, 0, wxEXPAND|wxALIGN_CENTRE, bsize*2);
   theMiddleSizer->Add(contentSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL);
   line1 = new wxStaticLine(this);
   theMiddleSizer->Add(line1, 0, wxEXPAND|wxALIGN_CENTRE, bsize*2);
   theMiddleSizer->Add(resourcesSizer, 0, wxALIGN_LEFT|wxEXPAND|wxALL, bsize*2);
   line1 = new wxStaticLine(this);
   theMiddleSizer->Add(line1, 0, wxEXPAND|wxALIGN_CENTRE, bsize*2);

   wxCheckBox *cbShow = new wxCheckBox( this, ID_CHECKBOX, wxT("&Show Welcome Page On Startup"));
   wxString showWelcomePanel;
   pConfig->Read("/Main/ShowWelcomeOnStart", &showWelcomePanel, "true");
   cbShow->SetValue( showWelcomePanel.Lower() == "true" );
   theMiddleSizer->Add(cbShow, 0, wxALIGN_LEFT|wxALL, bsize*2);

   wxSizer *thePanelSizer = new wxBoxSizer(wxVERTICAL);
   thePanelSizer->Add(theMiddleSizer, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize*2);
   SetSizer(thePanelSizer);
   thePanelSizer->SetSizeHints(this);
   
   // Set GMAT main icon
   GmatAppData::Instance()->SetIcon(this, "WelcomePanel");
   
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("WelcomePanel::Create() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// ~WelcomePanel()
//------------------------------------------------------------------------------
WelcomePanel::~WelcomePanel()
{
   GmatAppData::Instance()->GetMainFrame()->CloseWelcomePanel();
}


//------------------------------------------------------------------------------
// void OnExit(wxCommandEvent& WXUNUSED(event) )
//------------------------------------------------------------------------------
void WelcomePanel::OnExit(wxCommandEvent& WXUNUSED(event) )
{
   Close(true);
}


//------------------------------------------------------------------------------
// void OnShowWelcomePanelClicked(wxCommandEvent& WXUNUSED(event) )
//------------------------------------------------------------------------------
void WelcomePanel::OnShowWelcomePanelClicked(wxCommandEvent& event)
{
   if (GmatGlobal::Instance()->GetWritePersonalizationFile())
   {
      wxFileConfig *pConfig = (wxFileConfig *) GmatAppData::Instance()->GetPersonalizationConfig();
      if (event.IsChecked())
         pConfig->Write("/Main/ShowWelcomeOnStart", "true");
      else
         pConfig->Write("/Main/ShowWelcomeOnStart", "false");
   }
}


//------------------------------------------------------------------------------
// wxFlexGridSizer *FillGroup(wxFileConfig *config, wxString INIGroup, int maxRows, ...)
//------------------------------------------------------------------------------
/**
 * Handles filling a sizer with entries from INI group.
 *
 * @param config configuration file to read
 * @param INIGroup name of group (should include any preceding slashes, e.g. "/RecentFiles")
 * @param maxCols maximum number of columns
 */
//------------------------------------------------------------------------------
wxFlexGridSizer *WelcomePanel::FillGroup(wxFileConfig *config, wxString INIGroup,
                                         wxString INIIconGroup, int maxCols,
                                         wxWindowID id, bool isFileList, bool isReversed)
{
   #ifdef DEBUG_FILL_GROUP
   MessageInterface::ShowMessage
      ("WelcomePanel::FillGroup() entered, INIGroup='%s', INIIconGroup='%s', "
       "isFileList=%d\n", INIGroup.WX_TO_C_STRING, INIIconGroup.WX_TO_C_STRING, isFileList);
   #endif
   
   wxFlexGridSizer *aSizer;
   wxFlexGridSizer *aIconNTextSizer;
   wxArrayString linkLabels;
   wxArrayString linkURLs;
   wxArrayString linkIcons;
   size_t i;
   wxBitmapButton *abitmapButton = NULL;

   wxString aKey;
   wxString aValue;
   long dummy;
   int aCol = 1;
   wxHyperlinkCtrl *aButton;
   wxStaticLine *line1;
   
   // get the links from the config
   config->SetPath(INIGroup);
   // read filenames from config object
   if (config->GetFirstEntry(aKey, dummy))
   {
      if (isFileList)
	  {
		 if (isReversed)
			linkLabels.Insert(GmatFileUtil::ParseFileName(config->Read(aKey).c_str()).c_str(), 0);
		 else
			linkLabels.Add(GmatFileUtil::ParseFileName(config->Read(aKey).c_str()).c_str());
	  }
      else
	  {
		 if (isReversed)
			linkLabels.Insert(aKey, 0);
		 else
			linkLabels.Add(aKey);
	  }
      
      aValue = config->Read(aKey);
      if (isReversed)
		linkURLs.Insert(aValue,0);
	  else
		linkURLs.Add(aValue);
      
      #ifdef DEBUG_FILL_GROUP
      MessageInterface::ShowMessage("   aKey='%s', aValue='%s'\n", aKey.WX_TO_C_STRING, aValue.WX_TO_C_STRING);
      #endif
      
      while (config->GetNextEntry(aKey, dummy))
      {
         if (isFileList)
		 {
			 if (isReversed)
				linkLabels.Insert(GmatFileUtil::ParseFileName(config->Read(aKey).c_str()).c_str(), 0);
			 else
				linkLabels.Add(GmatFileUtil::ParseFileName(config->Read(aKey).c_str()).c_str());
		 }
		 else
		 {
			 if (isReversed)
				linkLabels.Insert(aKey, 0);
			 else
				linkLabels.Add(aKey);
		 }
         
         aValue = config->Read(aKey);
         if (isReversed)
		    linkURLs.Insert(aValue,0);
		 else
			linkURLs.Add(aValue);
         
         #ifdef DEBUG_FILL_GROUP
         MessageInterface::ShowMessage("   aKey='%s', aValue='%s'\n", aKey.WX_TO_C_STRING, aValue.WX_TO_C_STRING);
         #endif
      }
   }
   // go through all urls and use INIGROUP/Online if no helpcontroller and is not web url
   if (GmatAppData::Instance()->GetMainFrame()->GetHelpController() == NULL)
   {
	   config->SetPath(INIGroup + "/Online");
	   for (i=0; i<linkLabels.size(); i++)
	   {
		   if (!(linkURLs[i].Contains("\\") || linkURLs[i].Contains("/") || linkURLs[i].Contains(":") ))
		   {
			   aValue = config->Read(linkLabels[i]);
			   if (!aValue.IsEmpty())
				   linkURLs[i] = aValue;
		   }
	   }
   }

   // get the icons if section exists
   if (INIIconGroup != "")
   {
      config->SetPath(INIIconGroup);
      for (i=0; i<linkLabels.size(); i++)
         linkIcons.Add(config->Read(linkLabels[i]));
   }

   if (maxCols > 1)
   {
      maxCols = maxCols * 2;
      aSizer = new wxFlexGridSizer(maxCols, 10, 40);
   }
   else
      aSizer = new wxFlexGridSizer(maxCols, 10, 40);
   
   #ifdef DEBUG_FILL_GROUP
   MessageInterface::ShowMessage("   There are %d linkLables\n", linkLabels.size());
   #endif
   
   if (linkLabels.size() > 0)
   {
      FileManager *fm = FileManager::Instance();
      for (i=0; i<linkLabels.size(); i++)
      {
         aButton = new wxHyperlinkCtrl
            (this, id, linkLabels[i], linkURLs[i], wxDefaultPosition, wxDefaultSize,
             wxNO_BORDER|wxHL_ALIGN_CENTRE);
         aButton->SetToolTip(linkURLs[i]);
         if (linkIcons.size() > 0)
         {
            aIconNTextSizer = new wxFlexGridSizer(2, 20, 20);
            std::string fullPath = fm->GetFullPathname("ICON_PATH") + std::string(linkIcons[i].c_str());
            #ifdef DEBUG_FILL_GROUP
            MessageInterface::ShowMessage("   fullPath='%s'\n", fullPath.c_str());
            #endif
            if (GmatFileUtil::DoesFileExist(fullPath.c_str()))
            {
               wxBitmap bitmap = LoadBitmap( fullPath.c_str(), 50, 50 );
               abitmapButton = new wxBitmapButton(this, -1, bitmap, wxDefaultPosition,
                                                  wxSize(50,50));
            }
            else
            {
               MessageInterface::ShowMessage
                  ("*** WARNING *** Can't load image from file '%s'\n", fullPath.c_str());
               abitmapButton = NULL;
            }
            
            aIconNTextSizer->Add(abitmapButton, 0, wxALIGN_LEFT, bsize);
            aIconNTextSizer->Add(aButton, 0, wxALIGN_LEFT, bsize);
            aSizer->Add(aIconNTextSizer, 0, wxALIGN_LEFT, bsize*2);
         }
         else
            aSizer->Add(aButton, 0, wxALIGN_LEFT, bsize*3);
         aCol = (aCol + 1) % maxCols;
         if ((maxCols > 1) && (aCol < (maxCols-1)))
         {
            line1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
            aSizer->Add(line1, 0, wxALIGN_CENTRE, 20);
         }
         aCol = (aCol + 1) % maxCols;
      }
   }
   
   #ifdef DEBUG_FILL_GROUP
   MessageInterface::ShowMessage
      ("WelcomePanel::FillGroup() returning aSizer<%p>\n", aSizer);
   #endif
   
   return aSizer;
}


//------------------------------------------------------------------------------
// void OnOpenRecentScript(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void WelcomePanel::OnOpenRecentScript(wxHyperlinkEvent& event)
{
   GmatAppData::Instance()->GetMainFrame()->OpenRecentScript(event.GetURL(),
                                            event, false);
   GmatAppData::Instance()->GetMainFrame()->CloseWelcomePanel();  // ****
}


//------------------------------------------------------------------------------
// void OnOpenHelpLink(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening a help link (either to help file or web link)
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void WelcomePanel::OnOpenHelpLink(wxHyperlinkEvent& event)
{
   // if link is a keyword (no slashes or periods), use help controller
   // otherwise, assume it is a URL and use the default browser
   wxString link = event.GetURL();
   #ifdef DEBUG_WELCOME_HELP
      MessageInterface::ShowMessage("WelcomePanel::OnOpenHelpLink, url = %s\n",
            link.c_str());
   #endif
      // Get the absolute path to the application
   std::string workingDir = GmatFileUtil::GetApplicationPath();
   std::string sepType = GmatFileUtil::GetPathSeparator();
   Integer end = workingDir.find_last_of(sepType);
   if (end != std::string::npos)
      workingDir = workingDir.substr(0, end);
   
   link = workingDir + wxString("/") + link;

   FileManager* fm = FileManager::Instance();

#ifdef DEBUG_MENU_HELP
      MessageInterface::ShowMessage
      ("GmatMainFrame::OnHelpOnline() base help url='%s'\n", url.c_str());
#endif

   if (!fm->DoesFileExist((std::string)link))
   {
      std::string popMess = "Help file \"" + link.ToStdString() + "\" does not "
         "exist. Please check the HELP_HTML_FILE path listed in the GMAT "
         "startup file.";
      MessageInterface::PopupMessage(Gmat::ERROR_, popMess);
      return;
   }

   wxLaunchDefaultApplication(link);
}


//------------------------------------------------------------------------------
// void OnOpenSampleScript(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
/**
 * Handles opening script file from the menu bar.
 *
 * @param <event> input event.
 */
//------------------------------------------------------------------------------
void WelcomePanel::OnOpenSampleScript(wxHyperlinkEvent& event)
{
   std::string sampleDir = event.GetURL().WX_TO_STD_STRING;
   std::string appFullPath = GmatFileUtil::GetApplicationPath();
   std::string sepType = GmatFileUtil::GetPathSeparator();
   
   Integer end = appFullPath.find_last_of(sepType);
   //Remove "/Gmat.exe"
   if (end != std::string::npos)
      appFullPath = appFullPath.substr(0, end-1);
   //Remove "bin"
   end = appFullPath.find_last_of(sepType);
   if (end != std::string::npos)
      appFullPath = appFullPath.substr(0, end);

   std::string sampleFullPath = sampleDir;
   sampleFullPath = GmatStringUtil::Replace(sampleFullPath, "\\", "/");
   if (GmatFileUtil::IsPathRelative(sampleFullPath))
   {
      Integer start = sampleFullPath.find_first_of("/");
      if (start != std::string::npos)
         sampleFullPath = sampleFullPath.substr(start);
      sampleFullPath = appFullPath + sampleFullPath;
   }
   
   #ifdef DEBUG_SAMPLE_SCRIPT
      MessageInterface::ShowMessage
         ("WelcomePanel::OnOpenSampleScript() entered\n   sampleDir='%s'\n   appDir=%s'\n"
          "   sampleFullPath='%s'\n", sampleDir.c_str(), appFullPath.c_str(), sampleFullPath.c_str());
   #endif
   
   if (GmatFileUtil::DoesDirectoryExist(sampleFullPath + "/", false))
   {
      //wxString samplePath = wxString(sampleFullPath.c_str());
      wxFileDialog dialog(this, _T("Choose a file"), sampleFullPath, _T(""), _T("*.*"));
      
      if (dialog.ShowModal() == wxID_OK)
      {
         wxString scriptfile;
         scriptfile = dialog.GetPath().c_str();
         #ifdef DEBUG_SAMPLE_SCRIPT
            MessageInterface::ShowMessage("   scriptfile='%s'\n", scriptfile.c_str());
         #endif

         // Fix bug GMT-6627
         GmatAppData::Instance()->GetMainFrame()->OpenRecentScript(scriptfile, event, false);
         GmatAppData::Instance()->GetMainFrame()->CloseWelcomePanel();
      }
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Cannot open samples directory, '%s'\n", sampleFullPath.c_str());
   }
}


//------------------------------------------------------------------------------
// wxBitmap *LoadBitmap( wxString filename )
//------------------------------------------------------------------------------
/**
 * Loads a JPG bitmap from a file
 *
 * @param filename name of the image file
 */
//------------------------------------------------------------------------------
wxBitmap WelcomePanel::LoadBitmap( wxString filename, int width, int height )
{
   wxBitmap bitmap;
   std::string ext = GmatFileUtil::ParseFileExtension(filename.c_str());
   std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
   // Check for file exist first, otherwise the system hangs for non-existent file
   if (GmatFileUtil::DoesDirectoryExist(filename.c_str(), false) && 
	   ((ext == "JPG") || (ext == "JPEG") || (ext == "PNG")))
   {
	   if ((ext == "JPG") || (ext == "JPEG"))
	   {
		  wxImage::AddHandler(new wxJPEGHandler);
		  if (bitmap.LoadFile(filename, wxBITMAP_TYPE_JPEG))
		  {
			 wxImage image = bitmap.ConvertToImage();
			 bitmap = wxBitmap(image.Scale(width, height));
		  }
	   }
	   else
	   {
		   wxImage::AddHandler(new wxPNGHandler);
		   if (bitmap.LoadFile(filename, wxBITMAP_TYPE_PNG))
		   {
			 wxImage image = bitmap.ConvertToImage();
			 bitmap = wxBitmap(image.Scale(width, height));
		   }
	   }
   }
   else
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** Can't load image from '%s'\n", filename.WX_TO_C_STRING);
   }
   
   return bitmap;
}

