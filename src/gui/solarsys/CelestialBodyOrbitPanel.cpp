//$Id$
//------------------------------------------------------------------------------
//                                  CelestialBodyOrbitPanel
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Wendy C. Shoan
// Created: 2009.01.26
//
/**
 * This is the panel for the Orbit tab on the notebook on the CelestialBody
 * Panel.
 *
 */
//------------------------------------------------------------------------------

#include "CelestialBodyOrbitPanel.hpp"
#include "SolarSystem.hpp"
#include "GmatBaseException.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "A1Mjd.hpp"
#include "bitmaps/OpenFolder.xpm"
#include <wx/config.h>
#include <fstream>

//#define DEBUG_CB_ORBIT_PANEL
//#define DEBUG_CB_ORBIT_SAVE

//------------------------------------------------------------------------------
// event tables for wxMac/Widgets
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CelestialBodyOrbitPanel, wxPanel)
   EVT_COMBOBOX(ID_COMBO_BOX_EPHEM_SOURCE, CelestialBodyOrbitPanel::OnEphemSourceComboBoxChange)
   EVT_TEXT(ID_TEXT_CTRL_EPHEM_FILE, CelestialBodyOrbitPanel::OnEphemFileTextCtrlChange)
   EVT_BUTTON(ID_BROWSE_BUTTON_EPHEM_FILE, CelestialBodyOrbitPanel::OnEphemFileBrowseButton)
   EVT_BUTTON(ID_BROWSE_BUTTON_SPK_FILE, CelestialBodyOrbitPanel::OnSpkFileBrowseButton)
   EVT_BUTTON(ID_REMOVE_BUTTON_SPK_FILE, CelestialBodyOrbitPanel::OnSpkFileRemoveButton)
   EVT_LISTBOX(ID_LIST_BOX_SPK_FILE, CelestialBodyOrbitPanel::OnSpkFileListBoxChange)
   EVT_TEXT(ID_TEXT_CTRL_NAIF_ID, CelestialBodyOrbitPanel::OnNaifIdTextCtrlChange)
   EVT_COMBOBOX(ID_COMBO_BOX_CENTRAL_BODY, CelestialBodyOrbitPanel::OnCentralBodyComboBoxChange)
   EVT_TEXT(ID_TEXT_CTRL_INITIAL_EPOCH, CelestialBodyOrbitPanel::OnEpochTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_SMA, CelestialBodyOrbitPanel::OnSMATextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_ECC, CelestialBodyOrbitPanel::OnECCTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_INC, CelestialBodyOrbitPanel::OnINCTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_RAAN, CelestialBodyOrbitPanel::OnRAANTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_AOP, CelestialBodyOrbitPanel::OnAOPTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_TA, CelestialBodyOrbitPanel::OnTATextCtrlChange)
END_EVENT_TABLE()


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// CelestialBodyOrbitPanel(GmatPanel *cbPanel, wxWindow *parent,
//                         CelestialBody *body)
//------------------------------------------------------------------------------
/**
 * Creates the panel (default constructor).
 *
 * @param <cbPanel>             pointer to panel on which this one resides
 * @param <parent>              pointer to the parent window
 * @param <body>                body whose data the panel should display
 */
//------------------------------------------------------------------------------
CelestialBodyOrbitPanel::CelestialBodyOrbitPanel(GmatPanel *cbPanel, 
                              wxWindow *parent, CelestialBody *body) :
   wxPanel          (parent),
   dataChanged      (false),
   canClose         (true),
   theBody          (body),
   naifID           (-99),
   initialEpoch     (0.0),
   SMA              (0.0),
   ECC              (0.0),
   INC              (0.0),
   RAAN             (0.0),
   AOP              (0.0),
   TA               (0.0),
   ephemSrcChanged  (false),
   ephemFileChanged (false),
   spkFileChanged   (false),
   naifIDChanged    (false),
   cBodyChanged     (false),
   epochChanged     (false),
   SMAChanged       (false),
   ECCChanged       (false),
   INCChanged       (false),
   RAANChanged      (false),
   AOPChanged       (false),
   TAChanged        (false),
   spkFilesDeleted  (false),
   userDef          (false),
   allowSpiceForDefaultBodies (false),
   isSun            (false),
   includeTwoBody   (false),  // ***** modify this to include InitialTwoBody epoch and elements (for non-Sun body)
   theCBPanel       (cbPanel)
{
   guiManager     = GuiItemManager::GetInstance();
   guiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   ss             = guiInterpreter->GetSolarSystemInUse();

   #if 1
   
#ifdef __USE_SPICE__
   spiceAvailable = true;
#else
   spiceAvailable = false;
#endif
   
   #else
   
   spiceAvailable = false;
   
   #endif
   Create();
}

//------------------------------------------------------------------------------
// ~CelestialBodyOrbitPanel()
//------------------------------------------------------------------------------
/**
 * Destroys the panel (destructor).
 *
 */
//------------------------------------------------------------------------------
CelestialBodyOrbitPanel::~CelestialBodyOrbitPanel()
{
   guiManager->UnregisterComboBox("CelestialBody", centralBodyComboBox);
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the panel widgets to the body object.
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::SaveData()
{
   std::string strval;
   Real        tmpval;
   Integer     tmpint;
   bool        retval;
   bool        stateChanged = SMAChanged || ECCChanged || INCChanged || RAANChanged ||
                              AOPChanged || TAChanged;
   
   // don't do anything if no data has been changed.
   // note that dataChanged will be true if the user modified any combo box or
   // text ctrl, whether or not he/she actually changed the value
   canClose    = true;
   
   try
   {
      if (ephemSrcChanged)
      {
         strval = ephemSourceComboBox->GetValue();
         theBody->SetStringParameter(theBody->GetParameterID("PosVelSource"), strval);
      }
      if (ephemFileChanged)
      {
         strval = ephemFileTextCtrl->GetValue();
         #ifdef DEBUG_CB_ORBIT_PANEL
            MessageInterface::ShowMessage("ephemFileChanged is true : %s\n",
                  strval.c_str());
         #endif
         std::ifstream filename(strval.c_str());

         if (!filename)
         {
            std::string errmsg = "File \"" + strval;
            errmsg += "\" does not exist.\n";
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
         }
         else
         {
            filename.close();
            theBody->SetStringParameter(theBody->GetParameterID("SourceFilename"), strval);
         }
      }
      if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable && spkFileChanged)
      {
         #ifdef DEBUG_CB_ORBIT_PANEL
            MessageInterface::ShowMessage("spkFileChanged is true : %s\n",
                  strval.c_str());
         #endif

         unsigned int numKernels = spkFileListBox->GetCount();
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
               theBody->SetStringParameter(theBody->GetParameterID("OrbitSpiceKernelName"),
                     strval);
            }
         }
      }
      if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable && naifIDChanged)
      {
         strval = naifIDTextCtrl->GetValue();
         retval = theCBPanel->CheckInteger(tmpint, strval, "NAIF ID", "Integer Number");
         canClose = retval;
         if (retval)
         {
            theBody->SetIntegerParameter(theBody->GetParameterID("NAIFId"), tmpint);
         }
      }
      if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable && spkFilesDeleted)
      {
         for (unsigned int ii = 0; ii < spkFilesToDelete.size(); ii++)
         {
            theBody->RemoveSpiceKernelName("Orbit", spkFilesToDelete.at(ii));
         }
      }
      if (cBodyChanged)
      {
         strval = centralBodyComboBox->GetValue();
         theBody->SetStringParameter(theBody->GetParameterID("CentralBody"), strval);
      }

      if (!isSun && includeTwoBody)
      {
         if (epochChanged)
         {
            strval = initialEpochTextCtrl->GetValue();
            retval = theCBPanel->CheckReal(tmpval, strval, "Initial Two Body Epoch", "Real Number");
            canClose = retval;
            if (retval)
            {
               A1Mjd a1Epoch(tmpval);
               theBody->SetTwoBodyEpoch(a1Epoch);
            }
         }
         if (stateChanged)
         {
            #ifdef DEBUG_CB_ORBIT_PANEL
               MessageInterface::ShowMessage("state has been changed ...\n",
                     strval.c_str());
            #endif
            Rvector6 elements;
            bool     retval[6];
            strval = SMATextCtrl->GetValue();
            retval[0] = theCBPanel->CheckReal(tmpval, strval, "Initial SMA", "Real Number");
            if (retval[0])
            {
               elements[0] = tmpval;
            }
            strval = ECCTextCtrl->GetValue();
            retval[1] = theCBPanel->CheckReal(tmpval, strval, "Initial SMA", "Real Number");
            if (retval[1])
            {
               elements[1] = tmpval;
            }
            strval = INCTextCtrl->GetValue();
            retval[2] = theCBPanel->CheckReal(tmpval, strval, "Initial INC", "Real Number");
            if (retval[2])
            {
               elements[2] = tmpval;
            }
            strval = RAANTextCtrl->GetValue();
            retval[3] = theCBPanel->CheckReal(tmpval, strval, "Initial RAAN", "Real Number");
            if (retval[3])
            {
               elements[3] = tmpval;
            }
            strval = AOPTextCtrl->GetValue();
            retval[4] = theCBPanel->CheckReal(tmpval, strval, "Initial AOP", "Real Number");
            if (retval[4])
            {
               elements[4] = tmpval;
            }
            strval = TATextCtrl->GetValue();
            retval[5] = theCBPanel->CheckReal(tmpval, strval, "Initial TA", "Real Number");
            if (retval[5])
            {
               elements[5] = tmpval;
            }
            #ifdef DEBUG_CB_ORBIT_PANEL
               MessageInterface::ShowMessage(
                     "elements = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f   ...\n",
                     elements[0],elements[1],elements[2],elements[3],elements[4],elements[5]);
            #endif
            canClose = retval[0] && retval[1] && retval[2] && retval[3] && retval[4] && retval[5];
            if (canClose) theBody->SetTwoBodyElements(elements);
       }
      }


   }
   catch (BaseException &ex)
   {
      canClose = false;
      dataChanged = true;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }
   
   #ifdef DEBUG_CB_ORBIT_SAVE
      MessageInterface::ShowMessage("at end of CBOrbitPanel::SaveData, canClose = %s\n",
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
 * Loads the data from the body object to the panel widgets.
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::LoadData()
{
   try
   {
      ephemSrc   = theBody->GetStringParameter(theBody->GetParameterID("PosVelSource"));
      ephemSourceComboBox->SetValue(ephemSrc.c_str());
      
      previousEphemSrc = ephemSrc;
      
      ephemFile  = theBody->GetSourceFileName();
      ephemFileTextCtrl->SetValue(ephemFile.c_str());

      
      centralBody = theBody->GetCentralBody();
      centralBodyComboBox->SetValue(centralBody.c_str());
      
      if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable)
      {
         naifID = theBody->GetIntegerParameter(theBody->GetParameterID("NAIFId"));
         naifIDStringWX = guiManager->ToWxString(naifID);
         naifIDTextCtrl->SetValue(naifIDStringWX);
         spkFileArray             = theBody->GetStringArrayParameter(
                                    theBody->GetParameterID("OrbitSpiceKernelName"));
         unsigned int spkListSz   = spkFileArray.size();
         spkFileArrayWX           = new wxString[spkListSz];
         spkFiles.clear();
         for (unsigned int jj = 0; jj < spkListSz; jj++)
         {
            spkFiles.push_back(spkFileArray[jj]);
            spkFileArrayWX[jj] = wxString(spkFileArray[jj].c_str());
         }
         spkFileListBox->InsertItems(spkListSz, spkFileArrayWX, 0);
         spkFileListBox->SetSelection(spkListSz-1); // Select the last item
      }
      
      if (ephemSrc != "DE405")
      {
         ephemFileStaticText->Hide();
         ephemFileTextCtrl->Hide();
         ephemFileBrowseButton->Hide();
         mainBoxSizer->Layout();  
      }
      else
      {
         ephemFileStaticText->Show();
         ephemFileTextCtrl->Show();
         ephemFileBrowseButton->Show();
         orbitDataFlexGridSizer->Layout();
      }
      if (ephemSrc == "TwoBodyPropagation")
      {
         if (!isSun && includeTwoBody)
         {
            initialEpochTextCtrl->Enable();
            SMATextCtrl->Enable();
            ECCTextCtrl->Enable();
            INCTextCtrl->Enable();
            RAANTextCtrl->Enable();
            AOPTextCtrl->Enable();
            TATextCtrl->Enable();
            mainBoxSizer->Layout();
         }
      }
      else
      {
         if (!isSun && includeTwoBody)
         {
            initialEpochTextCtrl->Disable();
            SMATextCtrl->Disable();
            ECCTextCtrl->Disable();
            INCTextCtrl->Disable();
            RAANTextCtrl->Disable();
            AOPTextCtrl->Disable();
            TATextCtrl->Disable();
            mainBoxSizer->Layout();
         }
      }
      if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable)
      {
         if (ephemSrc != "SPICE")
         {
            spkFileStaticText->Hide();
            spkFileListBox->Hide();
            spkFileBrowseButton->Hide();
            spkFileRemoveButton->Hide();
            naifIDStaticText->Hide();
            naifIDTextCtrl->Hide();
            naifIDBlankText->Hide();
            mainBoxSizer->Layout();
         }
         else // SPICE
         {
            spkFileStaticText->Show();
            spkFileListBox->Show();
            spkFileBrowseButton->Show();
            spkFileRemoveButton->Show();
            naifIDStaticText->Show();
            naifIDTextCtrl->Show();
            naifIDBlankText->Show();
            mainBoxSizer->Layout();
         }
      }
      if (!isSun && includeTwoBody)
      {
         initialEpoch = (theBody->GetTwoBodyEpoch()).Get();
         initialEpochStringWX = guiManager->ToWxString(initialEpoch);
         initialEpochTextCtrl->SetValue(initialEpochStringWX);
         
         Rvector6 elements = theBody->GetTwoBodyElements();
         SMA  = elements[0];
         ECC  = elements[1];
         INC  = elements[2];
         RAAN = elements[3];
         AOP  = elements[4];
         TA   = elements[5];
         SMAStringWX = guiManager->ToWxString(elements[0]);
         SMATextCtrl->SetValue(SMAStringWX);
         ECCStringWX = guiManager->ToWxString(elements[1]);
         ECCTextCtrl->SetValue(ECCStringWX);
         INCStringWX = guiManager->ToWxString(elements[2]);
         INCTextCtrl->SetValue(INCStringWX);
         RAANStringWX = guiManager->ToWxString(elements[3]);
         RAANTextCtrl->SetValue(RAANStringWX);
         AOPStringWX = guiManager->ToWxString(elements[4]);
         AOPTextCtrl->SetValue(AOPStringWX);
         TAStringWX = guiManager->ToWxString(elements[5]);
         TATextCtrl->SetValue(TAStringWX);
      }

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
 * Creates and arranges the widgets on the panel.
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::Create()
{
   int bSize     = 2;
   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif

   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);

   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Celestial Body Orbit"));

   if (theBody->GetName() == GmatSolarSystemDefaults::SUN_NAME)  isSun = true;
   else                                                          isSun = false;
   userDef = theBody->IsUserDefined();
   allowSpiceForDefaultBodies = ss->IsSpiceAllowedForDefaultBodies();
   
   // empty the temporary value strings
   ephemSourceStringWX    = "";
   ephemFileStringWX      = "";
   naifIDStringWX         = "";
   centralBodyStringWX    = "";
   initialEpochStringWX   = "";
   SMAStringWX            = "";
   ECCStringWX            = "";
   INCStringWX            = "";
   RAANStringWX           = "";
   AOPStringWX            = "";
   TAStringWX             = "";
   
   // ephem source combo box
   sourceArray              = theBody->GetEphemSourceList();
   unsigned int ephemListSz = sourceArray.size();
   sourceArrayWX            = new wxString[ephemListSz];
   for (unsigned int jj = 0; jj < ephemListSz; jj++)
      sourceArrayWX[jj] = wxString(sourceArray[jj].c_str());
   ephemSourceStaticText = new wxStaticText(this, ID_TEXT, wxString("Ephemeris " GUI_ACCEL_KEY "Source"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
   ephemSourceComboBox   = new wxComboBox(this, ID_COMBO_BOX_EPHEM_SOURCE, wxString(sourceArrayWX[0]),
                           wxDefaultPosition, wxDefaultSize, ephemListSz, sourceArrayWX,
                           wxCB_DROPDOWN|wxCB_READONLY);
   ephemSourceComboBox->SetToolTip(pConfig->Read(_T("EphemerisSourceHint")));
   // ephem file
   ephemFileStaticText    =  new wxStaticText(this, ID_TEXT, wxString("Ephemeris " GUI_ACCEL_KEY "File"),
                             wxDefaultPosition, wxSize(-1,-1), 0);
   ephemFileTextCtrl      = new wxTextCtrl(this, ID_TEXT_CTRL_EPHEM_FILE, wxT(""),
                            wxDefaultPosition, wxSize(150,-1), 0);
   ephemFileTextCtrl->SetToolTip(pConfig->Read(_T("EphemerisFileHint")));
   ephemFileBrowseButton  = new wxBitmapButton(this, ID_BROWSE_BUTTON_EPHEM_FILE, 
                            openBitmap, wxDefaultPosition,
                            wxSize(buttonWidth, -1));
   ephemFileBrowseButton->SetToolTip(pConfig->Read(_T("BrowseEphemerisFileHint"), "Browse for file"));
   
   wxBoxSizer *spkButtonSizer = NULL;
   
   if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable)
   {
      // naif ID for user-defined bodies
      naifIDStaticText   = new wxStaticText(this, ID_TEXT,wxString(GUI_ACCEL_KEY"NAIF ID"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      naifIDTextCtrl     = new wxTextCtrl(this, ID_TEXT_CTRL_NAIF_ID, wxT(""),
                           wxDefaultPosition, wxSize(80, -1), 0);
      naifIDTextCtrl->SetToolTip(pConfig->Read(_T("NAIFIDHint")));
      naifIDBlankText    = new wxStaticText(this, ID_TEXT,wxT(""),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      // SPK file(s)
      wxArrayString emptyList;
      spkFileStaticText   = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"SPK Files"),
                            wxDefaultPosition, wxSize(-1,-1), 0);
      spkFileListBox      = new wxListBox(this, ID_LIST_BOX_SPK_FILE, wxDefaultPosition, wxSize(80, 100),
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
   }
   
   // central body
   centralBodyStaticText = new wxStaticText(this, ID_TEXT, wxString("Central " GUI_ACCEL_KEY "Body"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
   centralBodyComboBox = guiManager->GetCelestialBodyComboBox(this, ID_COMBO_BOX_CENTRAL_BODY, 
                                                              wxSize(150,-1));
   centralBodyComboBox->SetToolTip(pConfig->Read(_T("CentralBodyHint")));
   
   if (!isSun && includeTwoBody)
   {
      // initial epoch
      initialEpochStaticText = new wxStaticText(this, ID_TEXT, wxString("Initial A1 " GUI_ACCEL_KEY "Epoch"),
                               wxDefaultPosition, wxSize(-1,-1), 0);
      initialEpochTextCtrl   = new wxTextCtrl(this, ID_TEXT_CTRL_INITIAL_EPOCH, wxT(""),
                               wxDefaultPosition, wxSize(140, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      initialEpochTextCtrl->SetToolTip(pConfig->Read(_T("InitialA1EpochHint")));
      
      // SMA
      SMAStaticText      = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"SMA"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      SMATextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_SMA, wxT(""),
                           wxDefaultPosition, wxSize(140, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      SMATextCtrl->SetToolTip(pConfig->Read(_T("SMAHint")));
      SMAUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("km"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      
      // ECC
      ECCStaticText      = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"ECC"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      ECCTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_ECC, wxT(""),
                           wxDefaultPosition, wxSize(140, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      ECCTextCtrl->SetToolTip(pConfig->Read(_T("ECCHint")));
      ECCUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT(""),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      
      // INC
      INCStaticText      = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"INC"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      INCTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_INC, wxT(""),
                           wxDefaultPosition, wxSize(140, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      INCTextCtrl->SetToolTip(pConfig->Read(_T("INCHint")));
      INCUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("deg"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      
      // RAAN
      RAANStaticText      = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"RAAN"),
                            wxDefaultPosition, wxSize(-1,-1), 0);
      RAANTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_RAAN, wxT(""),
                            wxDefaultPosition, wxSize(140, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      RAANTextCtrl->SetToolTip(pConfig->Read(_T("RAANHint")));
      RAANUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("deg"),
                            wxDefaultPosition, wxSize(-1,-1), 0);
      
      // AOP
      AOPStaticText      = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"AOP"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      AOPTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_AOP, wxT(""),
                           wxDefaultPosition, wxSize(140, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      AOPTextCtrl->SetToolTip(pConfig->Read(_T("AOPHint")));
      AOPUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("deg"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      
      // TA
      TAStaticText      = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"TA"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
      TATextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_TA, wxT(""),
                           wxDefaultPosition, wxSize(140, -1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
      TATextCtrl->SetToolTip(pConfig->Read(_T("TAHint")));
      TAUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("deg"),
                           wxDefaultPosition, wxSize(-1,-1), 0);
   }
   
   // sizers
   
   orbitDataFlexGridSizer = new wxFlexGridSizer(3,0,0);
   orbitDataFlexGridSizer->Add(centralBodyStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   orbitDataFlexGridSizer->Add(centralBodyComboBox,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   //orbitDataFlexGridSizer->Add(30,20,0,wxGROW|wxALIGN_LEFT|wxALL, bSize);
   orbitDataFlexGridSizer->Add(0, 0);
   
   orbitDataFlexGridSizer->Add(ephemSourceStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   orbitDataFlexGridSizer->Add(ephemSourceComboBox,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   //orbitDataFlexGridSizer->Add(30,20,0,wxGROW|wxALIGN_LEFT|wxALL, bSize);
   orbitDataFlexGridSizer->Add(0, 0);
   
   orbitDataFlexGridSizer->Add(ephemFileStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   orbitDataFlexGridSizer->Add(ephemFileTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
   orbitDataFlexGridSizer->Add(ephemFileBrowseButton,0, wxALIGN_CENTRE|wxALL, bSize);
   
   if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable)
   {
      orbitDataFlexGridSizer->Add(naifIDStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      orbitDataFlexGridSizer->Add(naifIDTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      orbitDataFlexGridSizer->Add(0, 0);
      
      orbitDataFlexGridSizer->Add(spkFileStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      orbitDataFlexGridSizer->Add(spkFileListBox,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      orbitDataFlexGridSizer->Add(0, 0);
      
      orbitDataFlexGridSizer->Add(0, 0);
      orbitDataFlexGridSizer->Add(spkButtonSizer, 0, wxALIGN_CENTRE|wxALL, bSize);
      orbitDataFlexGridSizer->Add(0, 0);
   }
   
   wxFlexGridSizer *initialStateFlexGridSizer = NULL;
   if (!isSun && includeTwoBody)
   {
      initialStateFlexGridSizer = new wxFlexGridSizer(3,0,0);
      initialStateFlexGridSizer->Add(initialEpochStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(initialEpochTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(30,20,0,wxGROW|wxALIGN_LEFT|wxALL, bSize);
      
      initialStateFlexGridSizer->Add(SMAStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(SMATextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(SMAUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
      
      initialStateFlexGridSizer->Add(ECCStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(ECCTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(ECCUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
      
      initialStateFlexGridSizer->Add(INCStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(INCTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(INCUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
      
      initialStateFlexGridSizer->Add(RAANStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(RAANTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(RAANUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
      
      initialStateFlexGridSizer->Add(AOPStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(AOPTextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(AOPUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
      
      initialStateFlexGridSizer->Add(TAStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(TATextCtrl,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      initialStateFlexGridSizer->Add(TAUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
      
      // Make state edit box growable
      initialStateFlexGridSizer->AddGrowableCol( 1 );
   }
   
   mainBoxSizer = new wxBoxSizer(wxHORIZONTAL);
   
   GmatStaticBoxSizer  *boxSizer1 = new GmatStaticBoxSizer(wxVERTICAL, this, "Ephemeris Data");
   boxSizer1->Add(orbitDataFlexGridSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   
   if (isSun || !includeTwoBody)
   {
      mainBoxSizer->Add(boxSizer1, 1, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
      mainBoxSizer->Add(0,0);
   }
   else
   {
      GmatStaticBoxSizer  *boxSizer2 = new GmatStaticBoxSizer(wxVERTICAL, this, "Initial Two Body State");
      boxSizer2->Add(initialStateFlexGridSizer, 0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
      mainBoxSizer->Add(boxSizer1, 0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
      mainBoxSizer->Add(boxSizer2, 1, wxGROW|wxALIGN_CENTRE|wxALL, bSize); 
   }
   
   // disable ephem source, ephem file, and central body for default bodies, since
   // those need to be set on the SolarSystem panel
   if (!userDef)
   {
      ephemSourceComboBox->Disable();
      ephemFileTextCtrl->Disable();
      ephemFileBrowseButton->Disable();
   }
   centralBodyComboBox->Disable(); // do not allow user to change central body (maybe make this
                                   // static text somewhere ...)
   
   // Added #if in case if we want to add another layer of lined box for consistency.
   // I like without another layer, too many lines, so set to 0 (LOJ: 2009.11.18)
   // Looks bad to have the extra layer on the Mac - it shifts everything over
   // to the right too much (WCS: 2013.09.30)
//   #if 1
   #ifndef __WXMAC__
   
   GmatStaticBoxSizer *mainStaticBoxSizer = new GmatStaticBoxSizer(wxHORIZONTAL, this, "");
   mainStaticBoxSizer->Add(mainBoxSizer, 1, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   
   this->SetAutoLayout(true);
   this->SetSizer(mainStaticBoxSizer);
   mainStaticBoxSizer->Fit(this);
   mainStaticBoxSizer->SetSizeHints(this);
   
   #else
   
   this->SetAutoLayout(true);
   this->SetSizer(mainBoxSizer);
   mainBoxSizer->Fit(this);
   mainBoxSizer->SetSizeHints(this);
   
   #endif
}

//------------------------------------------------------------------------------
// void ResetChangeFlags(bool discardMods)
//------------------------------------------------------------------------------
/**
 * Resets the change flags for the panel.
 *
 * @param <discardMods>    flag indicating whether or not to discard the
 *                         modifications on the widgets
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::ResetChangeFlags(bool discardMods)
{
   ephemSrcChanged  = false;
   ephemFileChanged = false;
   spkFileChanged   = false;
   naifIDChanged    = false;
   cBodyChanged     = false;
   epochChanged     = false;
   SMAChanged       = false;
   ECCChanged       = false;
   INCChanged       = false;
   RAANChanged      = false;
   AOPChanged       = false;
   TAChanged        = false;
   spkFilesDeleted  = false;
   
   if (discardMods)
   {
      ephemFileTextCtrl->DiscardEdits();
      if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable)
      {
//         spkFileListBox->DiscardEdits();  // why doesn't this work??
         naifIDTextCtrl->DiscardEdits();
      }
      if (!isSun  && includeTwoBody)
      {
         initialEpochTextCtrl->DiscardEdits();
         SMATextCtrl->DiscardEdits();
         ECCTextCtrl->DiscardEdits();
         INCTextCtrl->DiscardEdits();
         RAANTextCtrl->DiscardEdits();
         AOPTextCtrl->DiscardEdits();
         TATextCtrl->DiscardEdits();
      }
   }
}

//------------------------------------------------------------------------------
//Event Handling
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void OnEphemSourceComboBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the ephem source combo
 * box value.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnEphemSourceComboBoxChange(wxCommandEvent &event)
{
   std::string newEphemSrc = (ephemSourceComboBox->GetStringSelection()).WX_TO_STD_STRING;
   if (newEphemSrc == previousEphemSrc) return;
   ephemSrcChanged = true;
   dataChanged     = true;
   theCBPanel->EnableUpdate(true);
   if (newEphemSrc != "DE405")
   {
      ephemFileStaticText->Hide();
      ephemFileTextCtrl->Hide();
      ephemFileBrowseButton->Hide();
      orbitDataFlexGridSizer->Layout();
      mainBoxSizer->Layout();
      
   }
   else
   {
      // re-insert those items here
      ephemFileStaticText->Show();
      ephemFileTextCtrl->Show();
      ephemFileBrowseButton->Show();
      orbitDataFlexGridSizer->Layout();
      mainBoxSizer->Layout();
   }
   if (newEphemSrc == "TwoBodyPropagation")
   {
      if (!isSun  && includeTwoBody)
      {
         initialEpochTextCtrl->Enable();
         SMATextCtrl->Enable();
         ECCTextCtrl->Enable();
         INCTextCtrl->Enable();
         RAANTextCtrl->Enable();
         AOPTextCtrl->Enable();
         TATextCtrl->Enable();
         mainBoxSizer->Layout();
      }
   }
   else
   {
      if (!isSun  && includeTwoBody)
      {
         initialEpochTextCtrl->Disable();
         SMATextCtrl->Disable();
         ECCTextCtrl->Disable();
         INCTextCtrl->Disable();
         RAANTextCtrl->Disable();
         AOPTextCtrl->Disable();
         TATextCtrl->Disable();
         mainBoxSizer->Layout();
      }
   }

   if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable)
   {
      if (newEphemSrc != "SPICE")
      {
         spkFileStaticText->Hide();
         spkFileListBox->Hide();
         spkFileBrowseButton->Hide();
         spkFileRemoveButton->Hide();
         naifIDStaticText->Hide();
         naifIDTextCtrl->Hide();
         naifIDBlankText->Hide();
         naifIDTextCtrl->Disable();
         mainBoxSizer->Layout();
      }
      else // SPICE
      {
         spkFileStaticText->Show();
         spkFileListBox->Show();
         spkFileBrowseButton->Show();
         spkFileRemoveButton->Show();
         naifIDStaticText->Show();
         naifIDTextCtrl->Show();
         naifIDBlankText->Show();
         naifIDTextCtrl->Enable();
         mainBoxSizer->Layout();
      }
   }
   previousEphemSrc = newEphemSrc;
}

//------------------------------------------------------------------------------
// void OnEphemFileTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the ephem file text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnEphemFileTextCtrlChange(wxCommandEvent &event)
{
   if (ephemFileTextCtrl->IsModified())
   {
      ephemFileChanged  = true;
      dataChanged       = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnEphemFileBrowseButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user selects the ephem file browse
 * button.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnEphemFileBrowseButton(wxCommandEvent &event)
{
   wxString oldFile = ephemFileTextCtrl->GetValue();
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString fileName = (dialog.GetPath()).c_str();
      if (!fileName.IsSameAs(oldFile))
      {
         ephemFileTextCtrl->SetValue(fileName);
         ephemFileChanged = true;
         dataChanged      = true;
         theCBPanel->EnableUpdate(true);
      }
   }
}

//------------------------------------------------------------------------------
// void OnSpkFileBrowseButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user selects the SPK browse
 * button.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnSpkFileBrowseButton(wxCommandEvent &event)
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
         
         spkFileChanged   = true;
         dataChanged      = true;
         spkFileListBox->Append(fileName);
         spkFileListBox->SetStringSelection(fileName);
         theCBPanel->EnableUpdate(true);
      }
   }
}

//------------------------------------------------------------------------------
// void OnSpkFileRemoveButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user selects the SPK remove
 * button.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnSpkFileRemoveButton(wxCommandEvent &event)
{
   wxArrayInt selections;
   int numSelect = spkFileListBox->GetSelections(selections);
   // get the string values and delete the selected names from the list
   wxString    fileSelected;
   for (int i = numSelect-1; i >= 0; i--)
   {
      fileSelected = spkFileListBox->GetString(selections[i]);
      spkFilesToDelete.push_back(fileSelected.WX_TO_STD_STRING);
      spkFileListBox->Delete(selections[i]);
   }
   spkFilesDeleted = true;
   dataChanged     = true;
   theCBPanel->EnableUpdate(true);
   
   // Select the last item
   unsigned int count = spkFileListBox->GetCount();
   if (count > 0)
      spkFileListBox->SetSelection(count -1);
}

//------------------------------------------------------------------------------
// void OnSpkFileListBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the SPK
 * file list box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnSpkFileListBoxChange(wxCommandEvent &event)
{
   spkFileChanged = true;
   dataChanged    = true;
   theCBPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnNaifIdTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the NAIF ID
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnNaifIdTextCtrlChange(wxCommandEvent &event)
{
   if (naifIDTextCtrl->IsModified())
   {
      naifIDChanged   = true;
      dataChanged     = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnCentralBodyComboBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the central
 * body combo box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnCentralBodyComboBoxChange(wxCommandEvent &event)
{
   std::string newCentralBody = (centralBodyComboBox->GetStringSelection()).WX_TO_STD_STRING;
   if (newCentralBody == centralBody) return;
   cBodyChanged       = true;
   dataChanged        = true;
   centralBody        = newCentralBody;
   theCBPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnEpochTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the epoch
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnEpochTextCtrlChange(wxCommandEvent &event)
{
   if (initialEpochTextCtrl->IsModified())
   {
      epochChanged    = true;
      dataChanged     = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnSMATextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the SMA
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnSMATextCtrlChange(wxCommandEvent &event)
{
   if (SMATextCtrl->IsModified())
   {
      SMAChanged      = true;
      dataChanged     = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnECCTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the ECC
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnECCTextCtrlChange(wxCommandEvent &event)
{
   if (ECCTextCtrl->IsModified())
   {
      ECCChanged      = true;
      dataChanged     = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnINCTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the INC
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnINCTextCtrlChange(wxCommandEvent &event)
{
   if (INCTextCtrl->IsModified())
   {
      INCChanged      = true;
      dataChanged     = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnRAANTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the RAAN
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnRAANTextCtrlChange(wxCommandEvent &event)
{
   if (RAANTextCtrl->IsModified())
   {
      RAANChanged     = true;
      dataChanged     = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnAOPTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the AOP
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnAOPTextCtrlChange(wxCommandEvent &event)
{
   if (AOPTextCtrl->IsModified())
   {
      AOPChanged      = true;
      dataChanged     = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnTATextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the TA
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyOrbitPanel::OnTATextCtrlChange(wxCommandEvent &event)
{
   if (TATextCtrl->IsModified())
   {
      TAChanged       = true;
      dataChanged     = true;
      theCBPanel->EnableUpdate(true);
   }
}
