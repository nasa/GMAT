//$Id$
//------------------------------------------------------------------------------
//                              UniversePanel
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
// Author: Monisha Butler
// Created: 2003/09/10
// Modified: 2004/01/13 by Allison Greene for action handling
/**
 * This class allows user to specify where Universe information is 
 * coming from
 */
//------------------------------------------------------------------------------
#include "UniversePanel.hpp"
#include "MessageInterface.hpp"
#include "ShowScriptDialog.hpp"
#include "StringUtil.hpp"
#include <fstream>
#include "bitmaps/OpenFolder.xpm"
#include <wx/config.h>
#include "gmatwxdefs.hpp"

//#define DEBUG_UNIVERSEPANEL_CREATE
//#define DEBUG_UNIVERSEPANEL_LOAD
//#define DEBUG_UNIVERSEPANEL_SAVE

//------------------------------------------------------------------------------
// event tables for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(UniversePanel, GmatPanel)
   EVT_BUTTON(ID_BUTTON_OK, GmatPanel::OnOK)
   EVT_BUTTON(ID_BUTTON_APPLY, GmatPanel::OnApply)
   EVT_BUTTON(ID_BUTTON_CANCEL, GmatPanel::OnCancel)
   EVT_BUTTON(ID_BUTTON_SCRIPT, GmatPanel::OnScript)

   EVT_BUTTON(ID_BUTTON_BROWSE, UniversePanel::OnBrowseButton)
   EVT_BUTTON(ID_SPK_BUTTON_BROWSE, UniversePanel::OnSPKBrowseButton)
   EVT_BUTTON(ID_PCK_BUTTON_BROWSE, UniversePanel::OnPCKBrowseButton)
   EVT_COMBOBOX(ID_COMBOBOX, UniversePanel::OnComboBoxChange)
   EVT_CHECKBOX(ID_CHECKBOX, UniversePanel::OnCheckBoxChange)
   EVT_TEXT(ID_TEXT_CTRL, UniversePanel::OnTextCtrlChange)

END_EVENT_TABLE()
   
   
//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// UniversePanel(wxWindow *parent)
//------------------------------------------------------------------------------
/**
 * Constructs UniversePanel object.
 *
 * @param <parent> input parent.
 */
//------------------------------------------------------------------------------
UniversePanel::UniversePanel(wxWindow *parent):GmatPanel(parent)
{
   mHasFileTypesInUseChanged = false;
   mHasFileNameChanged = false;
   mHasSPKFileNameChanged = false;
   mHasPCKFileNameChanged = false;
   mHasTextModified = false;
   theSolarSystem = NULL;
   
   // get solar system in use
   theSolarSystem = theGuiInterpreter->GetSolarSystemInUse();
   if (theSolarSystem == NULL)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, "The Solar System is NULL");
   }
   else
   {
      #ifdef DEBUG_UNIVERSEPANEL_CREATE
      MessageInterface::ShowMessage
         ("UniversePanel::UniversePanel() theSolarSystem=<%p>'%s'\n", theSolarSystem,
          theSolarSystem->GetName().c_str());
      #endif
      
      Create();
      Show();
   }
}


//------------------------------------------------------------------------------
// ~UniversePanel()
//------------------------------------------------------------------------------
/**
 * Destroys UniversePanel object (destructor)
 *
 */
//------------------------------------------------------------------------------
UniversePanel::~UniversePanel()
{
}


//------------------------------------------------------------------------------
// void OnScript()
//------------------------------------------------------------------------------
/**
 * Event handler - shows scripts
 */
//------------------------------------------------------------------------------
void UniversePanel::OnScript(wxCommandEvent &event)
{
   wxString title = "Object Script";
   // open separate window to show scripts?
   if (mObject != NULL)
   {
      title = "Scripting for ";
      title += mObject->GetName().c_str();
   }
   ShowScriptDialog ssd(this, -1, title, mObject, false);
   ssd.ShowModal();
}



//------------------------------------------------------------------------------
// methods inherited from GmatPanel
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the GUI panel to manage Solar System
 */
//------------------------------------------------------------------------------
void UniversePanel::Create()
{
   #ifdef DEBUG_UNIVERSEPANEL_CREATE
   MessageInterface::ShowMessage("UniversePanel::Create() entered\n");
   #endif
   
   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif

   Integer bsize = 2; // border size
   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Solar System"));

   wxArrayString emptyArray;
   
   //-------------------------------------------------------
   // ephemeris update interval
   //-------------------------------------------------------
   wxStaticText *intervalStaticText =
      new wxStaticText(this, ID_TEXT, wxString("Ephemeris Update " GUI_ACCEL_KEY "Interval"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   mIntervalTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""),
                                 wxDefaultPosition, wxSize(50,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   mIntervalTextCtrl->SetToolTip(pConfig->Read(_T("EphemerisUpdateIntervalHint")));
   wxStaticText *intervalUnitsStaticText =
      new wxStaticText(this, ID_TEXT, wxT("seconds"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   //-------------------------------------------------------
   // ephemeris source
   //-------------------------------------------------------
   wxStaticText *fileTypeLabel =
      new wxStaticText(this, ID_TEXT, wxString("Ephemeris " GUI_ACCEL_KEY "Source"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mFileTypeComboBox = 
      new wxComboBox(this, ID_COMBOBOX, wxT(""), wxDefaultPosition, wxDefaultSize,
                     emptyArray, wxCB_READONLY);
   mFileTypeComboBox->SetToolTip(pConfig->Read(_T("EphemerisSourceHint")));
   
   //-------------------------------------------------------
   // ephemeris file
   //-------------------------------------------------------
   deFileNameLabel =
      new wxStaticText(this, ID_TEXT, wxString("Ephemeris " GUI_ACCEL_KEY "Filename"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   mDEFileNameTextCtrl =
      new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""),
                     wxDefaultPosition, wxSize(300, -1),  0);
   mDEFileNameTextCtrl->SetToolTip(pConfig->Read(_T("EphemerisFilenameHint")));
   
   
   mBrowseButton =
      new wxBitmapButton(this, ID_BUTTON_BROWSE, openBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   mBrowseButton->SetToolTip(pConfig->Read(_T("BrowseEphemerisFilenameHint")));
   
   
   //-------------------------------------------------------
   // SPICE SPK Kernel (SPK)
   //-------------------------------------------------------
   spkNameLabel =
      new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"SPK Kernel"),
                       wxDefaultPosition, wxSize(-1,-1), 0);

   mSPKFileNameTextCtrl =
      new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""),
                     wxDefaultPosition, wxSize(300, -1),  0);
   mSPKFileNameTextCtrl->SetToolTip(pConfig->Read(_T("SPKFilenameHint")));


   mSPKBrowseButton =
      new wxBitmapButton(this, ID_SPK_BUTTON_BROWSE, openBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   mSPKBrowseButton->SetToolTip(pConfig->Read(_T("BrowseSPKFilenameHint")));


   //-------------------------------------------------------
   // SPICE Planetary Constants Kernel (PSK)
   //-------------------------------------------------------
   pckNameLabel =
      new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"Planetary Constants Kernel"),
                       wxDefaultPosition, wxSize(-1,-1), 0);

   mPCKFileNameTextCtrl =
      new wxTextCtrl(this, ID_TEXT_CTRL, wxT(""),
                     wxDefaultPosition, wxSize(300, -1),  0);
   mPCKFileNameTextCtrl->SetToolTip(pConfig->Read(_T("PlanetaryFilenameHint")));


   mPCKBrowseButton =
      new wxBitmapButton(this, ID_PCK_BUTTON_BROWSE, openBitmap, wxDefaultPosition,
                         wxSize(buttonWidth, -1));
   mPCKBrowseButton->SetToolTip(pConfig->Read(_T("BrowsePCKFilenameHint")));


   //-------------------------------------------------------
   // use TT for ephemeris
   //-------------------------------------------------------
   mOverrideCheckBox =
      new wxCheckBox(this, ID_CHECKBOX, wxString("Use " GUI_ACCEL_KEY "TT for Ephemeris"),
                     wxDefaultPosition, wxSize(-1, -1), 0);
   mOverrideCheckBox->SetToolTip(pConfig->Read(_T("UseTTForEphemerisHint")));
   
   
   //-------------------------------------------------------
   // Add to bottom grid sizer
   //-------------------------------------------------------    
   wxFlexGridSizer *bottomGridSizer = new wxFlexGridSizer(3, 0, 0);
   wxBoxSizer *intervalBoxSizer = new wxBoxSizer(wxHORIZONTAL);

   intervalBoxSizer->Add(mIntervalTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   intervalBoxSizer->Add(intervalUnitsStaticText, 0, wxALIGN_LEFT|wxALL, bsize);

   bottomGridSizer->Add(intervalStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(intervalBoxSizer);
   bottomGridSizer->Add(20,20,0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(fileTypeLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mFileTypeComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(20,20,0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(deFileNameLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mDEFileNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   bottomGridSizer->Add(spkNameLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mSPKFileNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mSPKBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   bottomGridSizer->Add(pckNameLabel, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mPCKFileNameTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(mPCKBrowseButton, 0, wxALIGN_CENTRE|wxALL, bsize);
   bottomGridSizer->Add(mOverrideCheckBox, 0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(20,20,0, wxALIGN_LEFT|wxALL, bsize);
   bottomGridSizer->Add(20,20,0, wxALIGN_LEFT|wxALL, bsize);

   //-------------------------------------------------------
   // Add to pageSizer
   //------------------------------------------------------- 
   mPageSizer = new GmatStaticBoxSizer (wxVERTICAL, this, "Options");
   mPageSizer->Add(bottomGridSizer, 0, wxALIGN_CENTRE|wxALL, bsize);
   
   theMiddleSizer->Add(mPageSizer, 1, wxALIGN_CENTER|wxGROW|wxALL, bsize);
   
   #ifdef DEBUG_UNIVERSEPANEL_CREATE
   MessageInterface::ShowMessage("UniversePanel::Create() leaving\n");
   #endif

}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data from the Solar System object into the GUI widgets
 */
//------------------------------------------------------------------------------
void UniversePanel::LoadData()
{
   #ifdef DEBUG_UNIVERSEPANEL_LOAD
   MessageInterface::ShowMessage("UniversePanel::LoadData() entered\n");
   #endif
   
   try
   {
      mAllFileTypes = theGuiInterpreter->GetPlanetarySourceTypes();
      //mAnalyticModels = theGuiInterpreter->GetAnalyticModelNames();
      StringArray fileTypesInUse = theGuiInterpreter->GetPlanetarySourceTypesInUse();
      
      #ifdef DEBUG_UNIVERSEPANEL_LOAD
      MessageInterface::ShowMessage
         ("   There are %d available file type(s)\n", mAllFileTypes.size());
      for (unsigned int i=0; i<mAllFileTypes.size(); i++)
         MessageInterface::ShowMessage
            ("      '%s'\n", mAllFileTypes[i].c_str());
      MessageInterface::ShowMessage
         ("   There are %d file type(s) in use\n", fileTypesInUse.size());
      for (unsigned int i=0; i<fileTypesInUse.size(); i++)
         MessageInterface::ShowMessage
            ("      '%s'\n", fileTypesInUse[i].c_str());
      #endif
      
      // load  ephemeris update interval
      Real interval = theSolarSystem->GetEphemUpdateInterval();
      mIntervalTextCtrl->SetValue(theGuiManager->ToWxString(interval));
      
      #ifdef DEBUG_UNIVERSEPANEL_LOAD
      MessageInterface::ShowMessage("   Interval set to %f\n", interval);
      #endif
      
      // available source
      for (unsigned int i=0; i<mAllFileTypes.size(); i++)
      {
         wxString type                  = mAllFileTypes[i].c_str();
         wxString typeName              = theGuiInterpreter->GetPlanetarySourceName(mAllFileTypes[i]).c_str();
         mFileTypeNameMap[type]         = typeName;
         mPreviousFileTypeNameMap[type] = typeName;
         mFileTypeComboBox->Append(type);
      }
      
      // Get the list of kernel names (indexed by file type)
      spiceKernelNames = theSolarSystem->GetSpiceKernelNames();

      #ifdef DEBUG_UNIVERSEPANEL_LOAD
      MessageInterface::ShowMessage("   Here is the mapping of file types\n");
      for (std::map<wxString, wxString>::iterator i = mFileTypeNameMap.begin();
           i != mFileTypeNameMap.end(); ++i)
         MessageInterface::ShowMessage
            ("      <%-20s>   '%-30s'\n", (i->first).c_str(), (i->second).c_str());
      #endif

      std::string currentSource =
         theSolarSystem->GetStringParameter(theSolarSystem->GetParameterID("EphemerisSource"));
      mFileTypeComboBox->SetStringSelection(currentSource.c_str());
      
      #ifdef DEBUG_UNIVERSEPANEL_LOAD
      MessageInterface::ShowMessage
         ("   Ephemeris source set to '%s'\n", currentSource.c_str());
      #endif
      
      if (mFileTypeComboBox->GetStringSelection() == "TwoBodyPropagation")
      {
         deFileNameLabel->SetLabel(wxString("Ephemeris " GUI_ACCEL_KEY "Filename"));
         mDEFileNameTextCtrl->Disable();
         mBrowseButton->Disable();
         spkNameLabel->Show(false);
         mSPKBrowseButton->Show(false);
         pckNameLabel->Show(false);
         mPCKBrowseButton->Show(false);
         wxWindow *windowWithFocus = FindFocus();
      }
      else
      {
//         mDEFileNameTextCtrl->Enable();
//         mBrowseButton->Enable();
         mSPKFileNameTextCtrl->Enable();
         spkNameLabel->Enable();
         mSPKBrowseButton->Enable();
         mPCKFileNameTextCtrl->Enable();
         pckNameLabel->Enable();
         mPCKBrowseButton->Enable();
         if (mFileTypeComboBox->GetStringSelection() == "SPICE")
         {
            mDEFileNameTextCtrl->Disable();
            mBrowseButton->Disable();
//            deFilenameLabel->SetLabel(wxString("SPK " GUI_ACCEL_KEY "Kernel"));
         }
         else
         {
            mDEFileNameTextCtrl->Enable();
            mBrowseButton->Enable();
//            deFilenameLabel->SetLabel(wxString("DE " GUI_ACCEL_KEY "Filename"));
         }
      }
      
      wxString selStr = mFileTypeComboBox->GetStringSelection();
      #ifdef DEBUG_UNIVERSEPANEL_LOAD
      MessageInterface::ShowMessage
         ("   Selected ephemeris source is '%s'\n", selStr.c_str());
      #endif
      if (selStr != "")
      {
         std::string sSel = selStr.WX_TO_STD_STRING;
         if (GmatStringUtil::StartsWith(sSel, "DE"))  // It's a DE file ...
         {
            wxString fileName = mFileTypeNameMap[selStr];
            mDEFileNameTextCtrl->SetValue(fileName);
            mPreviousFileTypeNameMap[selStr] = fileName;
         }
         else  // what to set DE filename to if it's SPICE?
         {
            wxString type                  = mAllFileTypes[0].c_str();
            mDEFileNameTextCtrl->SetValue(mFileTypeNameMap[type]); // ???????
            mPreviousFileTypeNameMap[selStr] = mFileTypeNameMap[type];
         }
         
         #ifdef DEBUG_UNIVERSEPANEL_LOAD
         MessageInterface::ShowMessage
            ("   Ephemeris file name set to '%s'\n", fileName.c_str());
         #endif
      }
      
      wxString spkFile = (theSolarSystem->GetStringParameter("SPKFilename")).c_str();
      mSPKFileNameTextCtrl->SetValue(spkFile);
      previousSpkFile  = spkFile;

      wxString pckFile = (theSolarSystem->GetStringParameter("PCKFilename")).c_str();
      mPCKFileNameTextCtrl->SetValue(pckFile);
      previousPckFile  = pckFile;

      bool useTT = theSolarSystem->GetBooleanParameter("UseTTForEphemeris");
      mOverrideCheckBox->SetValue(useTT);
      
      #ifdef DEBUG_UNIVERSEPANEL_LOAD
      MessageInterface::ShowMessage
         ("   UseTTForEphemeris set to %s\n", useTT ? "true" : "false");
      #endif
      
      mPageSizer->Layout();
      mObject = theSolarSystem;
      EnableUpdate(false);
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_UNIVERSEPANEL_LOAD
   MessageInterface::ShowMessage("UniversePanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from GUI to the Solar System object
 */
//------------------------------------------------------------------------------
void UniversePanel::SaveData()
{
   #ifdef DEBUG_UNIVERSEPANEL_SAVE
   MessageInterface::ShowMessage("UniversePanel::SaveData() entered\n");
   MessageInterface::ShowMessage(" hasTextModified = %s\n",
         (mHasTextModified? "true": "false"));
   MessageInterface::ShowMessage(" mHasFileTypesInUseChanged = %s\n",
         (mHasFileTypesInUseChanged? "true": "false"));
   MessageInterface::ShowMessage(" mHasFileNameChanged = %s\n",
         (mHasFileNameChanged? "true": "false"));
   #endif
   
   canClose = true;
   std::string str;
   Real interval;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   
   if (mHasTextModified)
   {
      str = mIntervalTextCtrl->GetValue();
      CheckReal(interval, str, "Interval", "Real Number >= 0.0");
   }
   
   if (!canClose)
      return;
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      // save ephemeris update interval, if changed
      if (mHasTextModified)
      {
         theSolarSystem->SetEphemUpdateInterval(interval);
         mHasTextModified = false;
      }
      
      // save planetary file types in use, if changed
      if (mHasFileTypesInUseChanged)
      {
         std::string srcSelection = std::string(mFileTypeComboBox->GetStringSelection().c_str());
         theSolarSystem->SetStringParameter(theSolarSystem->GetParameterID("EphemerisSource"),
               srcSelection);
         #ifdef DEBUG_UNIVERSEPANEL_SAVE
         MessageInterface::ShowMessage
            ("UniversePanel::SaveData() types=%s\n",
             mFileTypesInUse[0].c_str());
         #endif
         mHasFileTypesInUseChanged = false;
      }
         
      // save planetary file name, if changed
      if (mHasFileNameChanged)
      {
         wxString type = mFileTypeComboBox->GetStringSelection();
         str = mDEFileNameTextCtrl->GetValue();
         std::ifstream filename(str.c_str());
         
         // Check if the file doesn't exist then stop
         if (type != "TwoBodyPropagation" && !filename) 
         {
            std::string fieldName = "DEFilename";
            if (type == "SPICE")
               fieldName = "SPKFilename";
            MessageInterface::PopupMessage
               (Gmat::ERROR_, mMsgFormat.c_str(),
                str.c_str(), fieldName.c_str(), "", "File must exist");
            canClose = false;
            return;
         }
         filename.close();
         
         #ifdef DEBUG_UNIVERSEPANEL_SAVE
            MessageInterface::ShowMessage("Now setting file for %s to %s\n",
                  type.c_str(), str.c_str());
         #endif
         mFileTypeNameMap[type] = str.c_str();
         for (unsigned int i=0; i<mAllFileTypes.size(); i++)
         {
            wxString theType = mAllFileTypes[i].c_str();
            std::string name = std::string(mFileTypeNameMap[theType].c_str());
            if (theGuiInterpreter->SetPlanetarySourceName(mAllFileTypes[i], name))
            {
               #ifdef DEBUG_UNIVERSEPANEL_SAVE
                  MessageInterface::ShowMessage("Now setting PREVIOUS file for %s to %s\n",
                        theType.c_str(), name.c_str());
               #endif
               mPreviousFileTypeNameMap[theType] = name.c_str();
            }
            else
            {
               // if there was an error, set it back to what it was the last time it was saved
               mDEFileNameTextCtrl->SetValue(mPreviousFileTypeNameMap[theType]);
               #ifdef DEBUG_UNIVERSEPANEL_SAVE
                  MessageInterface::ShowMessage("Now setting file for %s back to %s\n",
                        theType.c_str(), mPreviousFileTypeNameMap[theType].c_str());
               #endif
               mFileTypeNameMap[theType] = mPreviousFileTypeNameMap[theType];
            }
            #ifdef DEBUG_UNIVERSEPANEL_SAVE
               std::string fieldName = "DEFilename";
               if (type == "SPICE")
                  fieldName = "SPKFilename";
               MessageInterface::ShowMessage("fieldName = %s\n", fieldName.c_str());
               MessageInterface::ShowMessage("str = %s\n", str.c_str());
            #endif
         }
         
         mHasFileNameChanged = false;
      }

      if (mHasSPKFileNameChanged)
      {
         wxString type = mFileTypeComboBox->GetStringSelection();
         str = mSPKFileNameTextCtrl->GetValue();
         std::ifstream filename(str.c_str());

         // Check if the file doesn't exist then stop
         if (type == "SPICE" && !filename)
         {
            std::string fieldName = "SPKFilename";
            MessageInterface::PopupMessage
               (Gmat::ERROR_, mMsgFormat.c_str(),
                str.c_str(), fieldName.c_str(), "", "File must exist");
            canClose = false;
            return;
         }
         filename.close();

         if (theSolarSystem->SetStringParameter("SPKFilename", str))
         {
            previousSpkFile = str.c_str();
         }
         else
         {
            // if there was an error, set it back to what it was the last time it was saved
            mSPKFileNameTextCtrl->SetValue(previousSpkFile);
         }

         mHasSPKFileNameChanged = false;
      }
      
      if (mHasPCKFileNameChanged)
      {
         wxString type = mFileTypeComboBox->GetStringSelection();
         str = mPCKFileNameTextCtrl->GetValue();
         std::ifstream filename(str.c_str());

         // Check if the file doesn't exist then stop
         if (type == "SPICE" && !filename)
         {
            std::string fieldName = "PCKFilename";
            MessageInterface::PopupMessage
               (Gmat::ERROR_, mMsgFormat.c_str(),
                str.c_str(), fieldName.c_str(), "", "File must exist");
            canClose = false;
            return;
         }
         filename.close();

         if (theSolarSystem->SetStringParameter("PCKFilename", str))
         {
            previousPckFile = str.c_str();
         }
         else
         {
            // if there was an error, set it back to what it was the last time it was saved
            mPCKFileNameTextCtrl->SetValue(previousPckFile);
         }

         mHasPCKFileNameChanged = false;
      }

      theSolarSystem->SetBooleanParameter("UseTTForEphemeris",
                                          mOverrideCheckBox->IsChecked());
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
   
   #ifdef DEBUG_UNIVERSEPANEL_SAVE
   MessageInterface::ShowMessage("UniversePanel::SaveData() leaving\n");
   #endif
}// end SaveData()


//------------------------------------------------------------------------------
// void OnBrowseButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler - handles event triggered when user pushes the Browse Button
 *
 * @param <event>   event being handled
 */
//------------------------------------------------------------------------------
void UniversePanel::OnBrowseButton(wxCommandEvent& event)
{
   wxString prevFilename = mDEFileNameTextCtrl->GetValue();
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;
      
      filename = dialog.GetPath().c_str();

      if (!filename.IsSameAs(prevFilename))
      {
         mDEFileNameTextCtrl->SetValue(filename);
         mFileTypeNameMap[mFileTypeComboBox->GetStringSelection()] = filename;
         mHasFileNameChanged = true;
         EnableUpdate(true);
      }
   }
}

//------------------------------------------------------------------------------
// void OnSPKBrowseButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler - handles event triggered when user pushes the SPK Browse
 * Button
 *
 * @param <event>   event being handled
 */
//------------------------------------------------------------------------------
void UniversePanel::OnSPKBrowseButton(wxCommandEvent& event)
{
   wxString oldname = mSPKFileNameTextCtrl->GetValue();
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));

   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;

      filename = dialog.GetPath().c_str();

      if (!filename.IsSameAs(oldname))
      {
         mSPKFileNameTextCtrl->SetValue(filename);
         mHasSPKFileNameChanged = true;
         EnableUpdate(true);
      }
   }

}

//------------------------------------------------------------------------------
// void OnPCKBrowseButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler - handles event triggered when user pushes the PCK Browse
 * Button
 *
 * @param <event>   event being handled
 */
//------------------------------------------------------------------------------
void UniversePanel::OnPCKBrowseButton(wxCommandEvent& event)
{
   wxString oldname = mPCKFileNameTextCtrl->GetValue();
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));

   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;

      filename = dialog.GetPath().c_str();

      if (!filename.IsSameAs(oldname))
      {
         mPCKFileNameTextCtrl->SetValue(filename);
         mHasPCKFileNameChanged = true;
         EnableUpdate(true);
      }
   }

}



//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler - handles event triggered when user modified a Combo Box
 * value
 *
 * @param <event>   event being handled
 */
//------------------------------------------------------------------------------
void UniversePanel::OnComboBoxChange(wxCommandEvent& event)
{
   mDEFileNameTextCtrl->Enable();
   mSPKFileNameTextCtrl->Enable();

   if (event.GetEventObject() == mFileTypeComboBox)
   {
      wxString type = mFileTypeComboBox->GetStringSelection();
      // Only change the DE filename if the type has changed to a new
      // 'DE' type
      std::string sType = type.WX_TO_STD_STRING;
      if (GmatStringUtil::StartsWith(sType, "DE"))
         mDEFileNameTextCtrl->SetValue(mFileTypeNameMap[type]);

      // Set SPK to the corresponding value
      Integer spiceIndex = theSolarSystem->GetPlanetarySourceId(sType);
      mSPKFileNameTextCtrl->SetValue(spiceKernelNames[spiceIndex]);


      if (type == "TwoBodyPropagation")
      {
         deFileNameLabel->SetLabel(wxString("Ephemeris " GUI_ACCEL_KEY "Filename"));
         mBrowseButton->Disable();
         mDEFileNameTextCtrl->Disable();
         // this next line is needed for the Mac - otherwise, when switching from SPICE,
         // the LSK text ctrl is still visible (even though it is disabled and hidden)
         // WCS 2016.06.30 we may be able to remove this sicne LSK has been removed
         mIntervalTextCtrl->SetFocus();
      }
      else
      {
         mBrowseButton->Enable();
         mDEFileNameTextCtrl->Enable();
         spkNameLabel->Enable();
         mSPKBrowseButton->Enable();
         pckNameLabel->Enable();
         mPCKBrowseButton->Enable();
         if (type == "SPICE")
         {
//            deFilenameLabel->SetLabel(wxString("SPK " GUI_ACCEL_KEY "Kernel"));
            mBrowseButton->Disable();
            mDEFileNameTextCtrl->Disable();
         }
         else // "DE"
         {
//            deFilenameLabel->SetLabel(wxString("DE " GUI_ACCEL_KEY "Filename"));
//            // this next line is needed for the Mac - otherwise, when switching from SPICE,
//            // the LSK text ctrl is still visible (even though it is disabled and hidden)
            mBrowseButton->Enable();
            mDEFileNameTextCtrl->Enable();
//            mDEFileNameTextCtrl->SetFocus();
         }
      }

      mPageSizer->Layout();
   }
   mHasFileTypesInUseChanged = true;
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler - handles event triggered when user changes the check box
 * selection
 *
 * @param <event>   event being handled
 */
//------------------------------------------------------------------------------
void UniversePanel::OnCheckBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnTextCtrlChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler - handles event triggered when user changes a text value
 *
 * @param <event>   event being handled
 */
//------------------------------------------------------------------------------
void UniversePanel::OnTextCtrlChange(wxCommandEvent& event)
{
   #ifdef DEBUG_UNIVERSEPANEL_SAVE
   MessageInterface::ShowMessage("UniversePanel::OnTextCtrlChange() entered\n");
   MessageInterface::ShowMessage(" hasTextModified = %s\n",
         (mHasTextModified? "true": "false"));
   MessageInterface::ShowMessage(" mHasFileTypesInUseChanged = %s\n",
         (mHasFileTypesInUseChanged? "true": "false"));
   MessageInterface::ShowMessage(" mHasFileNameChanged = %s\n",
         (mHasFileNameChanged? "true": "false"));
   #endif
   if (event.GetEventObject() == mIntervalTextCtrl)
   {
      if (mIntervalTextCtrl->IsModified())
         mHasTextModified = true;
   }
   
   if (event.GetEventObject() == mDEFileNameTextCtrl)
   {
      if (mDEFileNameTextCtrl->IsModified())
      {
         mHasFileNameChanged = true;
         mFileTypeNameMap[mFileTypeComboBox->GetStringSelection()] = mDEFileNameTextCtrl->GetValue();
      }
   }
   
   if (event.GetEventObject() == mSPKFileNameTextCtrl)
   {
      if (mSPKFileNameTextCtrl->IsModified())
      {
         mHasSPKFileNameChanged = true;
         wxString type          = mFileTypeComboBox->GetStringSelection();
         std::string sType      = type.WX_TO_STD_STRING;
         Integer spiceIndex     = theSolarSystem->GetPlanetarySourceId(sType);

         spiceKernelNames[spiceIndex] = mSPKFileNameTextCtrl->GetValue();
      }
   }

   if (event.GetEventObject() == mPCKFileNameTextCtrl)
   {
      if (mPCKFileNameTextCtrl->IsModified())
         mHasPCKFileNameChanged = true;
   }

   mPageSizer->Layout();
   EnableUpdate(true);

   #ifdef DEBUG_UNIVERSEPANEL_SAVE
   MessageInterface::ShowMessage("end of UniversePanel::OnTextCtrlChange() entered\n");
   MessageInterface::ShowMessage(" hasTextModified = %s\n",
         (mHasTextModified? "true": "false"));
   MessageInterface::ShowMessage(" mHasFileTypesInUseChanged = %s\n",
         (mHasFileTypesInUseChanged? "true": "false"));
   MessageInterface::ShowMessage(" mHasFileNameChanged = %s\n",
         (mHasFileNameChanged? "true": "false"));
   #endif
}
