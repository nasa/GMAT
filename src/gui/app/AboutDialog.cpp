//$Id$
//------------------------------------------------------------------------------
//                              AboutDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2007/08/22
//
/**
 * Implements AboutDialog class.
 */
//------------------------------------------------------------------------------

#include "AboutDialog.hpp"
#include "GmatAppData.hpp"         // for SetIcon()
#include "GmatBaseException.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"
#include "ViewTextDialog.hpp"
#include "FileUtil.hpp"            // for GmatFileUtil::DoesFileExist()
#include "GmatGlobal.hpp"          // for GetGmatVersion()
#include <wx/hyperlink.h>
#include <wx/statline.h>
#include <wx/gdicmn.h>


//#define DEBUG_ICONFILE

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(AboutDialog, wxDialog)
   EVT_BUTTON(ID_BUTTON_OK, AboutDialog::OnOK)
   EVT_HYPERLINK(ID_HYPERLINK, AboutDialog::OnHyperLinkClick)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// AboutDialog(wxWindow *parent, wxWindowID id, const wxString& title,
//            GmatBase *obj = NULL, const wxPoint& pos = wxDefaultPosition,
//            const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE);
//------------------------------------------------------------------------------
/**
 * Constructs AboutDialog object.
 *
 * @param  parent  parent window
 * @param  id      window id
 * @param  title   window title
 *
 */
//------------------------------------------------------------------------------
AboutDialog::AboutDialog(wxWindow *parent, wxWindowID id, const wxString& title,
                         const wxPoint& pos, const wxSize& size, long style)
   : wxDialog(parent, id, title, pos, size, style, title)
{
   #ifdef DEBUG_ICONFILE
   MessageInterface::ShowMessage("AboutDialog::AboutDialog() entered\n");
   #endif
   
   // Write info message once per GMAT session
   static bool writeInfo = true;
   wxBitmap bitmap;
   wxBitmapButton *aboutButton;
   
   // Use FileManager::FindPath() for GMATAboutIcon file (LOJ: 2014.07.02)
   
   // if icon file available, use it
   FileManager *fm = FileManager::Instance();
   //std::string iconFile = (fm->GetFullPathname("ICON_PATH") + "GMATAboutIcon.png");
   std::string iconFile = fm->FindPath("GMATAboutIcon.png", "ICON_PATH", true, false, writeInfo);
   writeInfo = false;
   
   #ifdef DEBUG_ICONFILE
   MessageInterface::ShowMessage("   About iconFile='%s'\n", iconFile.c_str());
   #endif
   if (iconFile != "")
   {
      //bitmap.LoadFile(iconFile.c_str(), wxBITMAP_TYPE_JPEG);
      bitmap.LoadFile(iconFile.c_str(), wxBITMAP_TYPE_PNG);
      #ifdef DEBUG_ICONFILE
         MessageInterface::ShowMessage("   Scaling and creating bitmap button\n");
      #endif
      wxBitmap::Rescale(bitmap, wxSize(80, 80));
      aboutButton = new wxBitmapButton(this, -1, bitmap, wxDefaultPosition,
                                       wxSize(100, 100), wxBORDER_NONE);
   }
   else
   {
      MessageInterface::ShowMessage
         ("About GMAT icon file '%s' does not exist.\n", iconFile.c_str());
      wxBitmap nullBitmap;
      aboutButton = new wxBitmapButton(this, -1, nullBitmap, wxDefaultPosition,
                                       wxSize(100, 100));
   }
   
   #ifdef DEBUG_ICONFILE
   MessageInterface::ShowMessage("   Bitmap button created\n");
   #endif
   
   wxColourDatabase cdb;
   wxColour gmatColor = cdb.Find("NAVY");
      
   wxStaticLine *line1 = new wxStaticLine(this);
   wxStaticLine *line2 = new wxStaticLine(this);
   
   // title, release number, build date   
   wxStaticText *gmatText =
      new wxStaticText(this, -1, "General Mission Analysis Tool");
   wxFont font1 = wxFont();
   
   #ifdef __WXMAC__
   font1.SetPointSize(20);
   #else
   font1.SetPointSize(11);
   #endif
   
   font1.SetWeight(wxFONTWEIGHT_BOLD);
   gmatText->SetOwnFont(font1);
   gmatText->SetOwnForegroundColour(gmatColor);
   
   // Release number
   std::string version = GmatGlobal::Instance()->GetGmatVersion();
   wxString releaseNumber = STD_TO_WX_STRING(version);
   bool is64bit = GmatGlobal::Instance()->IsGmatCompiledIn64Bit();
   wxString gmatCompiledBitSize = " (64-bit)";
   if (!is64bit)
      gmatCompiledBitSize = " (32-bit)";
   releaseNumber = releaseNumber + gmatCompiledBitSize;
   wxStaticText *releaseText = new wxStaticText(this, -1, releaseNumber);

   // Build date
   wxString buildDate;

   // Fix bug GMT-7399
   // buildDate.Printf("Build Date: %s %s\n", __DATE__, __TIME__);
   buildDate.Printf("Build Date: %s %s\n",
      GmatGlobal::Instance()->GetGMATBuildDate(),
      GmatGlobal::Instance()->GetGMATBuildTime());

   wxStaticText *buildText = new wxStaticText(this, -1, buildDate);

   #ifdef __WXMAC__
   font1.SetPointSize(12);
   #else
   font1.SetPointSize(8);
   #endif
   
   // Set font
   //font1.SetWeight(wxFONTWEIGHT_LIGHT);
   font1.SetWeight(wxFONTWEIGHT_NORMAL);
   releaseText->SetFont(font1);
   buildText->SetFont(font1);
   
   // website, contact email, and link to license
   wxStaticText *webText = new wxStaticText(this, -1, "Website: ");
   wxString gmatUrl = "https://sourceforge.net/projects/gmat/";
   wxHyperlinkCtrl *webLink = new wxHyperlinkCtrl(this, -1, gmatUrl, gmatUrl);
   wxStaticText *contactText = new wxStaticText(this, -1, "Contact: ");
   wxStaticText *emailText = new wxStaticText(this, -1, "gmat@gsfc.nasa.gov");
   wxStaticText *licenseText = new wxStaticText(this, -1, "License: ");
   wxString licenseUrl = "https://www.apache.org/licenses/LICENSE-2.0";
   wxHyperlinkCtrl *licenseLink = new wxHyperlinkCtrl(this, -1, licenseUrl, licenseUrl);
   
   wxFlexGridSizer *contactSizer = new wxFlexGridSizer(2);
   contactSizer->Add(webText, 0, wxALIGN_RIGHT|wxALL, 2);
   contactSizer->Add(webLink, 0, wxALIGN_LEFT|wxALL, 2);
   contactSizer->Add(contactText, 0, wxALIGN_RIGHT|wxALL, 2);
   contactSizer->Add(emailText, 0, wxALIGN_LEFT|wxALL, 2);
   contactSizer->Add(licenseText, 0, wxALIGN_RIGHT | wxALL, 2);
   contactSizer->Add(licenseLink, 0, wxALIGN_LEFT | wxALL, 2);
   
   wxBoxSizer *gmatSizer = new wxBoxSizer(wxVERTICAL);
   gmatSizer->Add(gmatText, 0, wxALIGN_CENTRE|wxALL, 4);
   gmatSizer->Add(releaseText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 4);
   gmatSizer->Add(buildText, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 4);
   gmatSizer->Add(3, 3);
   gmatSizer->Add(contactSizer, 0, wxALIGN_CENTRE|wxLEFT|wxRIGHT, 4);
   
   wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
   topSizer->Add(aboutButton, 0, wxALIGN_CENTRE|wxALL, 4);
   topSizer->Add(gmatSizer, 0, wxALIGN_CENTRE|wxALL, 4);
   
   wxString use;
   use = use + "GMAT uses the following third party content:\n";
   use = use + " - " + wxVERSION_STRING + "\n";
   use = use + " - TSPlot\n";
   //use = use + " - Uses Perl Compatible Regular Expressions\n";
   use = use + " - JPL SPICE Library\n";
   use = use + " - IAU SOFA Library\n";
   use = use + " - Apache Xerces 3.2.2\n";
   use = use + " - Planetary images courtesy of JPL/Caltech/USGS, Celestia \n";
   use = use + "   Motherlode, Bjorn Jonsson, and NASA World Wind \n";
   use = use + " - f2c \n";
   use = use + " - MSISE 1990 Density Model \n";
   use = use + " - NRL MSISE 2000 Density Model \n";
   use = use + " - IRI 2007 Ionosphere Model \n";
   use = use + " - OpenFramesInterface \n";
   use = use + " - Boost";
   wxStaticText *useText = new wxStaticText(this, -1, use);
   
   wxBoxSizer *useSizer = new wxBoxSizer(wxVERTICAL);
   useSizer->Add(useText, 0, wxALIGN_CENTRE|wxALL, 4);
   
   // OK button
   theOkButton = new wxButton(this, ID_BUTTON_OK, "OK");
   
   // Add to page sizer and set sizer to this dialog
   wxBoxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
   pageSizer->Add(topSizer, 0, wxALIGN_CENTRE|wxALL, 4);
   pageSizer->Add(10, 10);
   pageSizer->Add(line1, 0, wxGROW|wxALIGN_CENTRE|wxLEFT|wxRIGHT, 20);
   pageSizer->Add(useSizer, 0, wxALIGN_CENTRE|wxALL, 4);
   pageSizer->Add(line2, 0, wxGROW|wxALIGN_CENTRE|wxLEFT|wxRIGHT, 20);
   pageSizer->Add(10, 10);
   pageSizer->Add(theOkButton, 0, wxALIGN_CENTRE|wxALL, 5);
   
   SetAutoLayout(TRUE);
   SetSizer(pageSizer);
   pageSizer->Fit(this);
   pageSizer->SetSizeHints(this);
   
   // Set GMAT main icon
   GmatAppData::Instance()->SetIcon(this, "AboutDialog");
   
   CenterOnScreen(wxBOTH);
   
   #ifdef DEBUG_ICONFILE
   MessageInterface::ShowMessage("AboutDialog::AboutDialog() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnOK(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Closes the dialog
 */
//------------------------------------------------------------------------------
void AboutDialog::OnOK(wxCommandEvent &event)
{
   Close();
}


//------------------------------------------------------------------------------
// void OnHyperLinkClick(wxHyperlinkEvent &even)
//------------------------------------------------------------------------------
/**
 * Opens up hyper link text.
 */
//------------------------------------------------------------------------------
void AboutDialog::OnHyperLinkClick(wxHyperlinkEvent &event)
{
   if (event.GetEventObject() == theLicenseLink)
   {
      ViewTextDialog *dlg =
         //new ViewTextDialog(this, _T("NASA Open Source Agreement"), false,
         //                   wxDefaultPosition, wxSize(450, 300));
         new ViewTextDialog(this, _T("Apache License, Version 2.0"), false,
                            wxDefaultPosition, wxSize(400, 300));
      
      wxString rootPath = FileManager::Instance()->GetRootPath().c_str();
      wxString fileName = rootPath + "License.txt";
      if (!GmatFileUtil::DoesFileExist(fileName.c_str()))
         fileName = "../License.txt";
      
      wxTextCtrl *text = dlg->GetTextCtrl();
      text->LoadFile(fileName);
      dlg->ShowModal();
   }
}

