//$Id$
//------------------------------------------------------------------------------
//                            SpicePanel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under
// FDSS Task order 28.
//
// Author: Wendy C. Shoan
// Created: 2010.05.03
//
//
/**
 * This is the panel for the Spice tab on the notebook on the Spacecraft
 * Panel.
 *
 */
//------------------------------------------------------------------------------

#include "SpicePanel.hpp"
#include "SolarSystem.hpp"
#include "GmatBaseException.hpp"
#include "GmatAppData.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
//#include "bitmaps/OpenFolder.xpm"
#include <wx/config.h>
#include <fstream>

//#define DEBUG_SPICE_PANEL

// event tables for wxMac/Widgets
BEGIN_EVENT_TABLE(SpicePanel, wxPanel)
   EVT_BUTTON(ID_BROWSE_BUTTON_SPK_FILE, SpicePanel::OnSpkFileBrowseButton)
   EVT_BUTTON(ID_REMOVE_BUTTON_SPK_FILE, SpicePanel::OnSpkFileRemoveButton)
   EVT_LISTBOX(ID_LIST_BOX_SPK_FILE, SpicePanel::OnSpkFileListBoxChange)
   EVT_BUTTON(ID_BROWSE_BUTTON_CK_FILE, SpicePanel::OnCkFileBrowseButton)
   EVT_BUTTON(ID_REMOVE_BUTTON_CK_FILE, SpicePanel::OnCkFileRemoveButton)
   EVT_LISTBOX(ID_LIST_BOX_CK_FILE, SpicePanel::OnCkFileListBoxChange)
   EVT_BUTTON(ID_BROWSE_BUTTON_SCLK_FILE, SpicePanel::OnSclkFileBrowseButton)
   EVT_BUTTON(ID_REMOVE_BUTTON_SCLK_FILE, SpicePanel::OnSclkFileRemoveButton)
   EVT_LISTBOX(ID_LIST_BOX_SCLK_FILE, SpicePanel::OnSclkFileListBoxChange)
   EVT_BUTTON(ID_BROWSE_BUTTON_FK_FILE, SpicePanel::OnFkFileBrowseButton)
   EVT_BUTTON(ID_REMOVE_BUTTON_FK_FILE, SpicePanel::OnFkFileRemoveButton)
   EVT_LISTBOX(ID_LIST_BOX_FK_FILE, SpicePanel::OnFkFileListBoxChange)
   EVT_TEXT(ID_TEXT_CTRL_NAIF_ID, SpicePanel::OnNaifIdTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_NAIF_ID_REF_FRAME, SpicePanel::OnNaifIdRefTextCtrlChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SpicePanel(GmatPanel *scPanel,wxWindow *parent, Spacecraft *theSC)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param  the spacecraft panel on which this panel should appear
 * @param  the Spacecraft whose data to display/alter on this panel
 */
//------------------------------------------------------------------------------
SpicePanel::SpicePanel(GmatPanel *scPanel,
                       wxWindow *parent, Spacecraft *theSC) :
   wxPanel          (parent),
   naifId           (-1234567890),
   dataChanged      (false),
   canClose         (true),
   theSpacecraft    (theSC),
   spkNameChanged   (false),
   ckNameChanged    (false),
   sclkNameChanged  (false),
   fkNameChanged    (false),
   naifIdChanged    (false),
   naifIdRefFrameChanged (false),
   spkFilesDeleted  (false),
   ckFilesDeleted   (false),
   sclkFilesDeleted (false),
   fkFilesDeleted   (false),
   theScPanel       (scPanel)
{
   #ifdef DEBUG_SPICE_PANEL
      MessageInterface::ShowMessage("Entering SpicePanel Constructor, with theSC = %s (%s)\n",
            (theSC->GetName()).c_str(), (theSC->GetTypeName()).c_str());
   #endif
   guiManager     = GuiItemManager::GetInstance();
   guiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();

#ifdef __USE_SPICE__
   Create();
#else
   ; // throw an exception here?
#endif

}

//------------------------------------------------------------------------------
// ~SpicePanel()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SpicePanel::~SpicePanel()
{

}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves modified data to the Spacecraft object
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::SaveData()
{
   std::string strval;
//   Real        tmpval;
   Integer     tmpint;
   bool        retval;
   #ifdef DEBUG_SPICE_PANEL
   MessageInterface::ShowMessage(
         "SPICE panel:: spkNameChanged is %s, ckNameChanged is %s, sclkNameChanged is %s, fkNameChanged is %s, naifIdChanged is %s, naifIdRefFrameChanged is %s\n",
         (spkNameChanged? "true" : "false"), (ckNameChanged? "true" : "false"),
         (sclkNameChanged? "true" : "false"), (fkNameChanged? "true" : "false"),
         (naifIdChanged? "true" : "false"), (naifIdRefFrameChanged? "true" : "false"));
   MessageInterface::ShowMessage(
         "SPICE panel:: spkFilesDeleted is %s, ckFilesDeleted is %s, sclkFilesDeleted is %s,fkFilesDeleted is %s\n",
         (spkFilesDeleted? "true" : "false"), (ckFilesDeleted? "true" : "false"),
         (sclkFilesDeleted? "true" : "false"), (fkFilesDeleted? "true" : "false"));
   #endif

   // don't do anything if no data has been changed.
   // note that dataChanged will be true if the user modified any combo box or
   // text ctrl, whether or not he/she actually changed the value; we want to only
   // send the values to the object if something was really changed, to avoid
   // the hasBeenModified flag being set to true erroneously
   canClose    = true;

   try
   {
      unsigned int numKernels = 0;
      if (spkNameChanged)
      {
         #ifdef DEBUG_SPICE_PANEL
            MessageInterface::ShowMessage("spkNameChanged is true : %s\n",
                  strval.c_str());
         #endif

         numKernels = spkFileListBox->GetCount();
         for (unsigned int ii = 0; ii < numKernels; ii++)
         {
            strval = spkFileListBox->GetString(ii);
            std::ifstream filename(strval.c_str());

            if (!filename)
            {
               std::string errmsg = "File \"" + strval;
               errmsg += "\" does not exist.\n";
               MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
               canClose = false;
            }
            else
            {
               filename.close();
               theSpacecraft->SetStringParameter(theSpacecraft->GetParameterID("OrbitSpiceKernelName"),
                     strval);
            }
         }
      }
      if (ckNameChanged)
      {
         #ifdef DEBUG_SPICE_PANEL
            MessageInterface::ShowMessage("ckNameChanged is true : %s\n",
                  strval.c_str());
         #endif

         numKernels = ckFileListBox->GetCount();
         for (unsigned int ii = 0; ii < numKernels; ii++)
         {
            strval = ckFileListBox->GetString(ii);
            std::ifstream filename(strval.c_str());

            if (!filename)
            {
               std::string errmsg = "File \"" + strval;
               errmsg += "\" does not exist.\n";
               MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
               canClose = false;
            }
            else
            {
               filename.close();
               theSpacecraft->SetStringParameter(theSpacecraft->GetParameterID("AttitudeSpiceKernelName"),
                     strval);
            }
         }
      }
      if (sclkNameChanged)
      {
         #ifdef DEBUG_SPICE_PANEL
            MessageInterface::ShowMessage("sclkNameChanged is true : %s\n",
                  strval.c_str());
         #endif

         numKernels = sclkFileListBox->GetCount();
         for (unsigned int ii = 0; ii < numKernels; ii++)
         {
            strval = sclkFileListBox->GetString(ii);
            std::ifstream filename(strval.c_str());

            if (!filename)
            {
               std::string errmsg = "File \"" + strval;
               errmsg += "\" does not exist.\n";
               MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
               canClose = false;
            }
            else
            {
               filename.close();
               theSpacecraft->SetStringParameter(theSpacecraft->GetParameterID("SCClockSpiceKernelName"),
                     strval);
            }
         }
      }
      if (fkNameChanged)
      {
         #ifdef DEBUG_SPICE_PANEL
            MessageInterface::ShowMessage("fkNameChanged is true : %s\n",
                  strval.c_str());
         #endif

         numKernels = fkFileListBox->GetCount();
         for (unsigned int ii = 0; ii < numKernels; ii++)
         {
            strval = fkFileListBox->GetString(ii);
            std::ifstream filename(strval.c_str());

            if (!filename)
            {
               std::string errmsg = "File \"" + strval;
               errmsg += "\" does not exist.\n";
               MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
               canClose = false;
            }
            else
            {
               filename.close();
               theSpacecraft->SetStringParameter(theSpacecraft->GetParameterID("FrameSpiceKernelName"),
                     strval);
            }
         }
      }
      if (naifIdChanged)
      {
         strval = naifIdTextCtrl->GetValue();
         retval = theScPanel->CheckInteger(tmpint, strval, "NAIF ID", "Integer Number");
         #ifdef DEBUG_SPICE_PANEL
            MessageInterface::ShowMessage("CheckInteger returns : %s\n", (retval? "true" : "false"));
            MessageInterface::ShowMessage("canClose is set to %s\n", (canClose? "true" : "false"));
         #endif
         if (retval)
         {
            #ifdef DEBUG_SPICE_PANEL
               MessageInterface::ShowMessage("now attempting to set naifid to %d\n",tmpint);
            #endif
            theSpacecraft->SetIntegerParameter(theSpacecraft->GetParameterID("NAIFId"), tmpint);
         }
         else
         {
            canClose = false;
            return;
         }
      }
      if (naifIdRefFrameChanged)
      {
         strval = naifIdRefFrameTextCtrl->GetValue();
         retval = theScPanel->CheckInteger(tmpint, strval, " Reference Frame NAIF ID", "Integer Number");
         if (retval)
         {
            theSpacecraft->SetIntegerParameter(theSpacecraft->GetParameterID("NAIFIdReferenceFrame"), tmpint);
         }
         else
         {
            canClose = false;
            return;
         }
      }
      if (spkFilesDeleted)
      {
         for (unsigned int ii = 0; ii < spkFilesToDelete.size(); ii++)
         {
            theSpacecraft->RemoveSpiceKernelName("Orbit", spkFilesToDelete.at(ii));
         }
      }
      if (ckFilesDeleted)
      {
         for (unsigned int ii = 0; ii < ckFilesToDelete.size(); ii++)
         {
            theSpacecraft->RemoveSpiceKernelName("Attitude", ckFilesToDelete.at(ii));
         }
      }
      if (sclkFilesDeleted)
      {
         for (unsigned int ii = 0; ii < sclkFilesToDelete.size(); ii++)
         {
            theSpacecraft->RemoveSpiceKernelName("SCClock", sclkFilesToDelete.at(ii));
         }
      }
      if (fkFilesDeleted)
      {
         for (unsigned int ii = 0; ii < fkFilesToDelete.size(); ii++)
         {
            theSpacecraft->RemoveSpiceKernelName("Frame", fkFilesToDelete.at(ii));
         }
      }
   }
   catch (BaseException &ex)
   {
      canClose    = false;
      dataChanged = true;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }

   #ifdef DEBUG_SPICE_PANEL
      MessageInterface::ShowMessage("at end of SaveData, canClose = %s\n",
            (canClose? "true" : "false"));
   #endif
   if (canClose)
   {
      dataChanged = false;
      ResetChangeFlags(true);
   }
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads modified data from the Spacecraft object
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::LoadData()
{
   try
   {
      naifId         = theSpacecraft->GetIntegerParameter(theSpacecraft->GetParameterID("NAIFId"));
      naifIdStringWX = guiManager->ToWxString(naifId);
      naifIdTextCtrl->SetValue(naifIdStringWX);

      naifIdRefFrame         = theSpacecraft->GetIntegerParameter(theSpacecraft->GetParameterID("NAIFIdReferenceFrame"));
      naifIdRefFrameStringWX = guiManager->ToWxString(naifIdRefFrame);
      naifIdRefFrameTextCtrl->SetValue(naifIdRefFrameStringWX);

      spkKernelNames           = theSpacecraft->GetStringArrayParameter(
                                 theSpacecraft->GetParameterID("OrbitSpiceKernelName"));
      unsigned int spkListSz   = spkKernelNames.size();
      spkFileArrayWX           = new wxString[spkListSz];
      for (unsigned int jj = 0; jj < spkListSz; jj++)
      {
         spkFileArrayWX[jj] = wxString(spkKernelNames[jj].c_str());
      }
      spkFileListBox->InsertItems(spkListSz, spkFileArrayWX, 0);
      spkFileListBox->SetSelection(spkListSz-1); // Select the last item

      fkKernelNames           = theSpacecraft->GetStringArrayParameter(
                                theSpacecraft->GetParameterID("FrameSpiceKernelName"));
      unsigned int fkListSz   = fkKernelNames.size();
      fkFileArrayWX           = new wxString[fkListSz];
      for (unsigned int jj = 0; jj < fkListSz; jj++)
      {
         fkFileArrayWX[jj] = wxString(fkKernelNames[jj].c_str());
      }
      fkFileListBox->InsertItems(fkListSz, fkFileArrayWX, 0);
      fkFileListBox->SetSelection(fkListSz-1); // Select the last item

      ckKernelNames           = theSpacecraft->GetStringArrayParameter(
                                theSpacecraft->GetParameterID("AttitudeSpiceKernelName"));
      unsigned int ckListSz   = ckKernelNames.size();
      ckFileArrayWX           = new wxString[ckListSz];
      for (unsigned int jj = 0; jj < ckListSz; jj++)
      {
         ckFileArrayWX[jj] = wxString(ckKernelNames[jj].c_str());
      }
      ckFileListBox->InsertItems(ckListSz, ckFileArrayWX, 0);
      ckFileListBox->SetSelection(ckListSz-1); // Select the last item

      sclkKernelNames           = theSpacecraft->GetStringArrayParameter(
                                theSpacecraft->GetParameterID("SCClockSpiceKernelName"));
      unsigned int sclkListSz   = sclkKernelNames.size();
      sclkFileArrayWX           = new wxString[sclkListSz];
      for (unsigned int jj = 0; jj < sclkListSz; jj++)
      {
         sclkFileArrayWX[jj] = wxString(sclkKernelNames[jj].c_str());
      }
      sclkFileListBox->InsertItems(sclkListSz, sclkFileArrayWX, 0);
      sclkFileListBox->SetSelection(sclkListSz-1); // Select the last item

      ResetChangeFlags();

   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
}


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the widgets for the panel
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::Create()
{
   int bSize     = 2;
//   #if __WXMAC__
//   int buttonWidth = 40;
//   #else
//   int buttonWidth = 25;
//   #endif

//   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Spacecraft Spice"));

   // empty the temporary value strings
   naifIdStringWX             = "";
   naifIdRefFrameStringWX     = "";
   spkFileStringWX            = "";
   ckFileStringWX             = "";
   sclkFileStringWX           = "";
   fkFileStringWX             = "";

   wxBoxSizer *spkButtonSizer  = NULL;
   wxBoxSizer *ckButtonSizer   = NULL;
   wxBoxSizer *sclkButtonSizer = NULL;
   wxBoxSizer *fkButtonSizer   = NULL;

   // naif ID for spacecraft
   naifIdStaticText   = new wxStaticText(this, ID_TEXT,GUI_ACCEL_KEY"NAIF ID",
                        wxDefaultPosition, wxSize(-1,-1), 0);
   naifIdTextCtrl     = new wxTextCtrl(this, ID_TEXT_CTRL_NAIF_ID, wxT(""),
                        wxDefaultPosition, wxSize(80, -1), 0);
   naifIdTextCtrl->SetToolTip(pConfig->Read(_T("NAIFIDHint")));
   naifIdBlankText    = new wxStaticText(this, ID_TEXT,wxT(""),
                        wxDefaultPosition, wxSize(-1,-1), 0);
   // naif ID of the reference frame for the spacecraft
   naifIdRefFrameStaticText   = new wxStaticText(this, ID_TEXT,"F" GUI_ACCEL_KEY "rame\nNAIF ID",
                        wxDefaultPosition, wxSize(-1,-1), 0);
   naifIdRefFrameTextCtrl     = new wxTextCtrl(this, ID_TEXT_CTRL_NAIF_ID_REF_FRAME, wxT(""),
                        wxDefaultPosition, wxSize(80, -1), 0);
   naifIdRefFrameTextCtrl->SetToolTip(pConfig->Read(_T("NAIFIDRefFrameHint")));
   naifIdRefFrameBlankText    = new wxStaticText(this, ID_TEXT,wxT(""),
                        wxDefaultPosition, wxSize(-1,-1), 0);
   // SPK file(s)
   wxArrayString emptyList;
   spkFileStaticText   = new wxStaticText(this, ID_TEXT, GUI_ACCEL_KEY"SPK Files",
                         wxDefaultPosition, wxSize(-1,-1), 0);
   spkFileListBox      = new wxListBox(this, ID_LIST_BOX_SPK_FILE, wxDefaultPosition, wxSize(150, 100),
                         emptyList, wxLB_EXTENDED|wxLB_NEEDED_SB|wxLB_HSCROLL);
   spkFileListBox->SetToolTip(pConfig->Read(_T("SPKFileListHint")));
   spkFileBrowseButton = new wxButton(this, ID_BROWSE_BUTTON_SPK_FILE, wxString(GUI_ACCEL_KEY"Add"),
                         wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   spkFileBrowseButton->SetToolTip(pConfig->Read(_T("AddSPKFileHint")));
   spkFileRemoveButton = new wxButton(this, ID_REMOVE_BUTTON_SPK_FILE, wxString(GUI_ACCEL_KEY"Remove"),
                         wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   spkFileRemoveButton->SetToolTip(pConfig->Read(_T("RemoveSPKFileHint")));
   spkButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   spkButtonSizer->Add(spkFileBrowseButton,0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   spkButtonSizer->Add(spkFileRemoveButton,0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);

   // FK file(s)
   fkFileStaticText   = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"FK Files"),
                         wxDefaultPosition, wxSize(-1,-1), 0);
   fkFileListBox      = new wxListBox(this, ID_LIST_BOX_FK_FILE, wxDefaultPosition, wxSize(150, 100),
                         emptyList, wxLB_EXTENDED|wxLB_NEEDED_SB|wxLB_HSCROLL);
   fkFileListBox->SetToolTip(pConfig->Read(_T("FKFileListHint")));
   fkFileBrowseButton = new wxButton(this, ID_BROWSE_BUTTON_FK_FILE, wxString(GUI_ACCEL_KEY"Add"),
                         wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   fkFileBrowseButton->SetToolTip(pConfig->Read(_T("AddFKFileHint")));
   fkFileRemoveButton = new wxButton(this, ID_REMOVE_BUTTON_FK_FILE, wxString(GUI_ACCEL_KEY"Remove"),
                         wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   fkFileRemoveButton->SetToolTip(pConfig->Read(_T("RemoveFKFileHint")));
   fkButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   fkButtonSizer->Add(fkFileBrowseButton,0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   fkButtonSizer->Add(fkFileRemoveButton,0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);

   // CK file(s)
   ckFileStaticText   = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"CK Files"),
                         wxDefaultPosition, wxSize(-1,-1), 0);
   ckFileListBox      = new wxListBox(this, ID_LIST_BOX_CK_FILE, wxDefaultPosition, wxSize(150, 100),
                         emptyList, wxLB_EXTENDED|wxLB_NEEDED_SB|wxLB_HSCROLL);
   ckFileListBox->SetToolTip(pConfig->Read(_T("CKFileListHint")));
   ckFileBrowseButton = new wxButton(this, ID_BROWSE_BUTTON_CK_FILE, wxString(GUI_ACCEL_KEY"Add"),
                         wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   ckFileBrowseButton->SetToolTip(pConfig->Read(_T("AddCKFileHint")));
   ckFileRemoveButton = new wxButton(this, ID_REMOVE_BUTTON_CK_FILE, wxString(GUI_ACCEL_KEY"Remove"),
                         wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   ckFileRemoveButton->SetToolTip(pConfig->Read(_T("RemoveCKFileHint")));
   ckButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   ckButtonSizer->Add(ckFileBrowseButton,0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   ckButtonSizer->Add(ckFileRemoveButton,0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);

   // SCLK file(s)
   sclkFileStaticText   = new wxStaticText(this, ID_TEXT, wxString("SC" GUI_ACCEL_KEY "LK Files"),
                         wxDefaultPosition, wxSize(-1,-1), 0);
   sclkFileListBox      = new wxListBox(this, ID_LIST_BOX_SCLK_FILE, wxDefaultPosition, wxSize(150, 100),
                         emptyList, wxLB_EXTENDED|wxLB_NEEDED_SB|wxLB_HSCROLL);
   sclkFileListBox->SetToolTip(pConfig->Read(_T("SCLKFileListHint")));
   sclkFileBrowseButton = new wxButton(this, ID_BROWSE_BUTTON_SCLK_FILE, wxString(GUI_ACCEL_KEY"Add"),
                         wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   sclkFileBrowseButton->SetToolTip(pConfig->Read(_T("AddSCLKFileHint")));
   sclkFileRemoveButton = new wxButton(this, ID_REMOVE_BUTTON_SCLK_FILE, wxString(GUI_ACCEL_KEY"Remove"),
                         wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
   sclkFileRemoveButton->SetToolTip(pConfig->Read(_T("RemoveSCLKFileHint")));
   sclkButtonSizer = new wxBoxSizer(wxHORIZONTAL);
   sclkButtonSizer->Add(sclkFileBrowseButton,0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   sclkButtonSizer->Add(sclkFileRemoveButton,0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);

   // sizers

   leftFlexGridSizer = new wxFlexGridSizer(2,0,0);
   leftFlexGridSizer->Add(naifIdStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   leftFlexGridSizer->Add(naifIdTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
//   leftFlexGridSizer->Add(0, 0);

   leftFlexGridSizer->Add(spkFileStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   leftFlexGridSizer->Add(spkFileListBox,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
//   leftFlexGridSizer->Add(0, 0);

   leftFlexGridSizer->Add(0, 0);
   leftFlexGridSizer->Add(spkButtonSizer, 0, wxALIGN_CENTRE|wxALL, bSize);
//   leftFlexGridSizer->Add(0, 0);

   leftFlexGridSizer->Add(fkFileStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   leftFlexGridSizer->Add(fkFileListBox,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
//   leftFlexGridSizer->Add(0, 0);

   leftFlexGridSizer->Add(0, 0);
   leftFlexGridSizer->Add(fkButtonSizer, 0, wxALIGN_CENTRE|wxALL, bSize);
//   leftFlexGridSizer->Add(0, 0);


   rightFlexGridSizer = new wxFlexGridSizer(2,0,0);
   rightFlexGridSizer->Add(naifIdRefFrameStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   rightFlexGridSizer->Add(naifIdRefFrameTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
//   rightFlexGridSizer->Add(0, 0);

   rightFlexGridSizer->Add(ckFileStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   rightFlexGridSizer->Add(ckFileListBox,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
//   rightFlexGridSizer->Add(0, 0);

   rightFlexGridSizer->Add(0, 0);
   rightFlexGridSizer->Add(ckButtonSizer, 0, wxALIGN_CENTRE|wxALL, bSize);
//   rightFlexGridSizer->Add(0, 0);

   rightFlexGridSizer->Add(sclkFileStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   rightFlexGridSizer->Add(sclkFileListBox,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
//   rightFlexGridSizer->Add(0, 0);

   rightFlexGridSizer->Add(0, 0);
   rightFlexGridSizer->Add(sclkButtonSizer, 0, wxALIGN_CENTRE|wxALL, bSize);
//   rightFlexGridSizer->Add(0, 0);
   
   mainBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   
   GmatStaticBoxSizer  *boxSizer1 = new GmatStaticBoxSizer(wxVERTICAL, this, "" );
   boxSizer1->Add(leftFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bSize);
   
   GmatStaticBoxSizer  *boxSizer2 = new GmatStaticBoxSizer(wxVERTICAL, this, "");
   boxSizer2->Add(rightFlexGridSizer, 0, wxALIGN_CENTRE|wxALL, bSize);
   
   mainBoxSizer->Add(boxSizer1, 1, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   mainBoxSizer->Add(boxSizer2, 1, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   
   // Added #if in case if we want to add another layer of lined box for consistency.
   // I like without another layer, too many lines, so set to 0 (LOJ: 2009.11.18) - from CelestialBody
   #if 0
   
   GmatStaticBoxSizer *mainStaticBoxSizer = new GmatStaticBoxSizer(wxVERTICAL, this, "");
   mainStaticBoxSizer->Add(mainBoxSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   
   this->SetAutoLayout(true);
   this->SetSizer(mainStaticBoxSizer);
   mainStaticBoxSizer->Fit(this);
   mainStaticBoxSizer->SetSizeHints(this);
   
   #else
   
   wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
   mainSizer->Add(mainBoxSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   
   this->SetAutoLayout(true);
   this->SetSizer(mainSizer);
   mainSizer->Fit(this);
   mainSizer->SetSizeHints(this);
   
   #endif

}

//------------------------------------------------------------------------------
// void ResetChangeFlags(bool discardMods)
//------------------------------------------------------------------------------
/**
 * Resets the flags indicating whether or not data has been modified
 * by the user.
 *
 * @param discardMods   indicates whether or not to discard the mods made
 *                      by the user
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::ResetChangeFlags(bool discardMods)
{
   spkNameChanged        = false;
   ckNameChanged         = false;
   sclkNameChanged       = false;
   fkNameChanged         = false;
   naifIdChanged         = false;
   naifIdRefFrameChanged = false;
   spkFilesDeleted       = false;
   ckFilesDeleted        = false;
   sclkFilesDeleted      = false;
   fkFilesDeleted        = false;

   if (discardMods)
   {
//      spkFileListBox->DiscardEdits();  // why doesn't this work??
//      ckFileListBox->DiscardEdits();  // why doesn't this work??
//      sclkFileListBox->DiscardEdits();  // why doesn't this work??
//      fkFileListBox->DiscardEdits();  // why doesn't this work??
      naifIdTextCtrl->DiscardEdits();
      naifIdRefFrameTextCtrl->DiscardEdits();
   }
}

//Event Handling
//------------------------------------------------------------------------------
// void OnSpkFileBrowseButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user selects the SPK Add Button
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnSpkFileBrowseButton(wxCommandEvent &event)
{
   wxArrayString oldFiles = spkFileListBox->GetStrings();
   wxFileDialog dialog(this, _T("Choose a file to add"), _T(""), _T(""), _T("*.*"));
   Integer foundAt = -99;
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString fileName = (dialog.GetPath()).c_str();
      for (Integer ii = 0; ii < (Integer) oldFiles.GetCount(); ii++)
      {
         if (fileName.IsSameAs(oldFiles[ii]))
         {
            foundAt = ii;
            break;
         }
      }
      if (foundAt == -99) // not found, so it's new
      {
         // Deselect current selections first
         wxArrayInt selections;
         int numSelect = spkFileListBox->GetSelections(selections);
         for (int i = 0; i < numSelect; i++)
            spkFileListBox->Deselect(selections[i]);

         spkNameChanged   = true;
         dataChanged      = true;
         spkFileListBox->Append(fileName);
         spkFileListBox->SetStringSelection(fileName);
         theScPanel->EnableUpdate(true);
      }
   }
}

//------------------------------------------------------------------------------
// void OnSpkFileRemoveButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user selects the SPK Remove Button
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnSpkFileRemoveButton(wxCommandEvent &event)
{
   wxArrayInt selections;
   int numSelect = spkFileListBox->GetSelections(selections);
   // get the string values and delete the selected names from the list
   wxString    fileSelected;
   for (int i = numSelect-1; i >= 0; i--)
   {
      fileSelected = spkFileListBox->GetString(selections[i]);
      spkFilesToDelete.push_back(std::string(fileSelected.c_str()));
      spkFileListBox->Delete(selections[i]);
   }
   spkFilesDeleted = true;
   dataChanged     = true;
   theScPanel->EnableUpdate(true);

   // Select the last item
   unsigned int count = spkFileListBox->GetCount();
   if (count > 0)
      spkFileListBox->SetSelection(count -1);
}

//------------------------------------------------------------------------------
// void OnSpkFileListBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user modifies the SPK list box selection
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnSpkFileListBoxChange(wxCommandEvent &event)
{
   spkNameChanged = true;
   dataChanged    = true;
   theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnCkFileBrowseButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user selects the CK Add Button
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnCkFileBrowseButton(wxCommandEvent &event)
{
   wxArrayString oldFiles = ckFileListBox->GetStrings();
   wxFileDialog dialog(this, _T("Choose a file to add"), _T(""), _T(""), _T("*.*"));
   Integer foundAt = -99;
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString fileName = (dialog.GetPath()).c_str();
      for (Integer ii = 0; ii < (Integer) oldFiles.GetCount(); ii++)
      {
         if (fileName.IsSameAs(oldFiles[ii]))
         {
            foundAt = ii;
            break;
         }
      }
      if (foundAt == -99) // not found, so it's new
      {
         // Deselect current selections first
         wxArrayInt selections;
         int numSelect = ckFileListBox->GetSelections(selections);
         for (int i = 0; i < numSelect; i++)
            ckFileListBox->Deselect(selections[i]);

         ckNameChanged    = true;
         dataChanged      = true;
         ckFileListBox->Append(fileName);
         ckFileListBox->SetStringSelection(fileName);
         theScPanel->EnableUpdate(true);
      }
   }
}

//------------------------------------------------------------------------------
// void OnCkFileRemoveButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user selects the CK Remove Button
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnCkFileRemoveButton(wxCommandEvent &event)
{
   wxArrayInt selections;
   int numSelect = ckFileListBox->GetSelections(selections);
   // get the string values and delete the selected names from the list
   wxString    fileSelected;
   for (int i = numSelect-1; i >= 0; i--)
   {
      fileSelected = ckFileListBox->GetString(selections[i]);
      ckFilesToDelete.push_back(std::string(fileSelected.c_str()));
      ckFileListBox->Delete(selections[i]);
   }
   ckFilesDeleted  = true;
   dataChanged     = true;
   theScPanel->EnableUpdate(true);

   // Select the last item
   unsigned int count = ckFileListBox->GetCount();
   if (count > 0)
      ckFileListBox->SetSelection(count -1);
}

//------------------------------------------------------------------------------
// void OnCkFileListBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user modifies the CK list box selection.
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnCkFileListBoxChange(wxCommandEvent &event)
{
   ckNameChanged  = true;
   dataChanged    = true;
   theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnSclkFileBrowseButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user selects the SCLK Add Button
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnSclkFileBrowseButton(wxCommandEvent &event)
{
   wxArrayString oldFiles = sclkFileListBox->GetStrings();
   wxFileDialog dialog(this, _T("Choose a file to add"), _T(""), _T(""), _T("*.*"));
   Integer foundAt = -99;
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString fileName = (dialog.GetPath()).c_str();
      for (Integer ii = 0; ii < (Integer) oldFiles.GetCount(); ii++)
      {
         if (fileName.IsSameAs(oldFiles[ii]))
         {
            foundAt = ii;
            break;
         }
      }
      if (foundAt == -99) // not found, so it's new
      {
         // Deselect current selections first
         wxArrayInt selections;
         int numSelect = sclkFileListBox->GetSelections(selections);
         for (int i = 0; i < numSelect; i++)
            sclkFileListBox->Deselect(selections[i]);

         sclkNameChanged  = true;
         dataChanged      = true;
         sclkFileListBox->Append(fileName);
         sclkFileListBox->SetStringSelection(fileName);
         theScPanel->EnableUpdate(true);
      }
   }
}

//------------------------------------------------------------------------------
// void OnSclkFileRemoveButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user selects the SCLK Remove Button
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnSclkFileRemoveButton(wxCommandEvent &event)
{
   wxArrayInt selections;
   int numSelect = sclkFileListBox->GetSelections(selections);
   // get the string values and delete the selected names from the list
   wxString    fileSelected;
   for (int i = numSelect-1; i >= 0; i--)
   {
      fileSelected = sclkFileListBox->GetString(selections[i]);
      sclkFilesToDelete.push_back(std::string(fileSelected.c_str()));
      sclkFileListBox->Delete(selections[i]);
   }
   sclkFilesDeleted = true;
   dataChanged      = true;
   theScPanel->EnableUpdate(true);

   // Select the last item
   unsigned int count = sclkFileListBox->GetCount();
   if (count > 0)
      sclkFileListBox->SetSelection(count -1);
}

//------------------------------------------------------------------------------
// void OnSclkFileListBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user modifies the SCLK list box
 * selection.
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnSclkFileListBoxChange(wxCommandEvent &event)
{
   sclkNameChanged = true;
   dataChanged     = true;
   theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnFkFileBrowseButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user selects the FK Add Button
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnFkFileBrowseButton(wxCommandEvent &event)
{
   wxArrayString oldFiles = fkFileListBox->GetStrings();
   wxFileDialog dialog(this, _T("Choose a file to add"), _T(""), _T(""), _T("*.*"));
   Integer foundAt = -99;
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString fileName = (dialog.GetPath()).c_str();
      for (Integer ii = 0; ii < (Integer) oldFiles.GetCount(); ii++)
      {
         if (fileName.IsSameAs(oldFiles[ii]))
         {
            foundAt = ii;
            break;
         }
      }
      if (foundAt == -99) // not found, so it's new
      {
         // Deselect current selections first
         wxArrayInt selections;
         int numSelect = fkFileListBox->GetSelections(selections);
         for (int i = 0; i < numSelect; i++)
            fkFileListBox->Deselect(selections[i]);

         fkNameChanged    = true;
         dataChanged      = true;
         fkFileListBox->Append(fileName);
         fkFileListBox->SetStringSelection(fileName);
         theScPanel->EnableUpdate(true);
      }
   }
}

//------------------------------------------------------------------------------
// void OnFkFileRemoveButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user selects the FK Remove Button
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnFkFileRemoveButton(wxCommandEvent &event)
{
   wxArrayInt selections;
   int numSelect = fkFileListBox->GetSelections(selections);
   // get the string values and delete the selected names from the list
   wxString    fileSelected;
   for (int i = numSelect-1; i >= 0; i--)
   {
      fileSelected = fkFileListBox->GetString(selections[i]);
      fkFilesToDelete.push_back(std::string(fileSelected.c_str()));
      fkFileListBox->Delete(selections[i]);
   }
   fkFilesDeleted  = true;
   dataChanged     = true;
   theScPanel->EnableUpdate(true);

   // Select the last item
   unsigned int count = fkFileListBox->GetCount();
   if (count > 0)
      fkFileListBox->SetSelection(count -1);
}

//------------------------------------------------------------------------------
// void OnFkFileListBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user modifies the FK list box selection.
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnFkFileListBoxChange(wxCommandEvent &event)
{
   fkNameChanged  = true;
   dataChanged    = true;
   theScPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnNaifIdTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user modifies the NAIF ID text.
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnNaifIdTextCtrlChange(wxCommandEvent &event)
{
   if (naifIdTextCtrl->IsModified())
   {
      naifIdChanged   = true;
      dataChanged     = true;
      theScPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnNaifIdRefTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handles the event triggered when the user modifies the NAIF ID reference text.
 *
 * @param event   the event to handle
 *
 */
//------------------------------------------------------------------------------
void SpicePanel::OnNaifIdRefTextCtrlChange(wxCommandEvent &event)
{
   if (naifIdRefFrameTextCtrl->IsModified())
   {
      naifIdRefFrameChanged   = true;
      dataChanged     = true;
      theScPanel->EnableUpdate(true);
   }
}
