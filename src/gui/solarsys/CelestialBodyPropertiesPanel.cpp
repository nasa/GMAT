//$Id$
//------------------------------------------------------------------------------
//                                  CelestialBodyPropertiesPanel
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
// Created: 2009.01.14
//
/**
 * This is the panel for the Properties tab on the notebook on the CelestialBody
 * Panel.
 *
 */
//------------------------------------------------------------------------------

#include "CelestialBodyPropertiesPanel.hpp"
#include "GmatColorPanel.hpp"
#include "GmatAppData.hpp"
#include "MessageInterface.hpp"
#include "bitmaps/OpenFolder.xpm"
#include <wx/config.h>
#include <fstream>

//#define DEBUG_CB_PROP_PANEL
//#define DEBUG_LOAD_DATA
//#define DEBUG_CB_PROP_SAVE


//------------------------------------------------------------------------------
// event tables for wxMac/Widgets
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CelestialBodyPropertiesPanel, wxPanel)
   EVT_TEXT(ID_TEXT_CTRL_MU, CelestialBodyPropertiesPanel::OnMuTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_EQRAD, CelestialBodyPropertiesPanel::OnEqRadTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_FLAT, CelestialBodyPropertiesPanel::OnFlatTextCtrlChange)
   EVT_BUTTON(ID_BROWSE_BUTTON_PCK_FILE, CelestialBodyPropertiesPanel::OnPckFileBrowseButton)
   EVT_BUTTON(ID_REMOVE_BUTTON_PCK_FILE, CelestialBodyPropertiesPanel::OnPckFileRemoveButton)
   EVT_LISTBOX(ID_LIST_BOX_PCK_FILE, CelestialBodyPropertiesPanel::OnPckFileListBoxChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CelestialBodyPropertiesPanel(GmatPanel *cbPanel, wxWindow *parent,
//                              CelestialBody *body)
//------------------------------------------------------------------------------
/**
 * Creates the panel (default constructor).
 *
 * @param <cbPanel>             pointer to panel on which this one resides
 * @param <parent>              pointer to the parent window
 * @param <body>                body whose data the panel should display
 */
//------------------------------------------------------------------------------
CelestialBodyPropertiesPanel::CelestialBodyPropertiesPanel(GmatPanel *cbPanel, 
                              wxWindow *parent, CelestialBody *body) :
   wxPanel                    (parent),
   userDef                    (false),
   allowSpiceForDefaultBodies (false),
   spiceAvailable             (false),
   dataChanged                (false),
   canClose                   (true),
   pckFilesDeleted            (false),
   theBody                    (body),
   mu                         (0.0),
   eqRad                      (0.0),
   flat                       (0.0),
   muChanged                  (false),
   eqRadChanged               (false),
   flatChanged                (false),
   pckChanged                 (false),
   theCBPanel                 (cbPanel)
{
   guiManager     = GuiItemManager::GetInstance();
   guiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   ss             = guiInterpreter->GetSolarSystemInUse();

#ifdef __USE_SPICE__
   spiceAvailable = true;
#else
   spiceAvailable = false;
#endif

   Create();
}

//------------------------------------------------------------------------------
// ~CelestialBodyPropertiesPanel()
//------------------------------------------------------------------------------
/**
 * Destroys the panel (destructor).
 *
 */
//------------------------------------------------------------------------------
CelestialBodyPropertiesPanel::~CelestialBodyPropertiesPanel()
{
   // nothing to do ... la la la la la ...
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the panel widgets to the body object.
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::SaveData()
{
   std::string strval;
   Real        tmpval;
   bool        realsOK   = true;
   
   // don't do anything if no data has been changed.
   // note that dataChanged will be true if the user modified any combo box or
   // text ctrl, whether or not he/she actually changed the value
   #ifdef DEBUG_CB_PROP_SAVE
      MessageInterface::ShowMessage("Entering CBPropPanel::SaveData, dataChanged = %s\n",
         (dataChanged? "true" : "false"));
      MessageInterface::ShowMessage("    muChanged    = %s\n",
         (muChanged? "true" : "false"));
      MessageInterface::ShowMessage("    eqRadChanged = %s\n",
         (eqRadChanged? "true" : "false"));
      MessageInterface::ShowMessage("    flatChanged  = %s\n",
         (flatChanged? "true" : "false"));
      MessageInterface::ShowMessage("    pckChanged   = %s\n",
         (pckChanged? "true" : "false"));
      MessageInterface::ShowMessage("    allowSpiceForDefaultBodies = %s\n",
         (allowSpiceForDefaultBodies? "true" : "false"));
      MessageInterface::ShowMessage("    userDef                    = %s\n",
         (userDef? "true" : "false"));
      MessageInterface::ShowMessage("    spiceAvailable             = %s\n",
         (spiceAvailable? "true" : "false"));
   #endif
   if (!dataChanged) return;
   
   canClose    = true;
   
   try
   {
      if (muChanged)
      {
         strval = muTextCtrl->GetValue();
         if (!theCBPanel->CheckReal(tmpval, strval, "Mu", "Real Number > 0",
               false, true, true, false))
            realsOK = false;
         else
         {
            mu = tmpval;
         }
      }
      if (eqRadChanged)
      {
         strval = eqRadTextCtrl->GetValue();
         if (!theCBPanel->CheckReal(tmpval, strval, "Equatorial Radius", "Real Number > 0",
               false, true, true, false))
            realsOK = false;
         else
         {
            eqRad = tmpval;
         }
      }
      if (flatChanged)
      {
         strval = flatTextCtrl->GetValue();
         if (!theCBPanel->CheckReal(tmpval, strval, "Flattening Coefficient", "Real Number >= 0",
               false, true, true, true))
            realsOK = false;
         else
         {
            flat = tmpval;
         }
      }

      if (!realsOK)
      {
         std::string errmsg = "Please enter valid Real values before saving data.\n";
         MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
         return;
      }

      if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable && pckChanged)
      {

         unsigned int numKernels = pckFileListBox->GetCount();

         #ifdef DEBUG_CB_PROP_SAVE
            MessageInterface::ShowMessage("numKernels = %d\n", (Integer) numKernels);
         #endif

         for (unsigned int ii = 0; ii < numKernels; ii++)
         {
            strval = pckFileListBox->GetString(ii);
            std::ifstream filename(strval.c_str());
            #ifdef DEBUG_CB_PROP_SAVE
               MessageInterface::ShowMessage("strval = %s\n",
                     strval.c_str());
               MessageInterface::ShowMessage("numKernels = %d\n", (Integer) numKernels);
            #endif

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
               theBody->SetStringParameter(theBody->GetParameterID("PlanetarySpiceKernelName"),
                     strval);
            }
         }
      }
      if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable && pckFilesDeleted)
      {
         for (unsigned int ii = 0; ii < pckFilesToDelete.size(); ii++)
         {
            theBody->RemoveSpiceKernelName("Planetary", pckFilesToDelete.at(ii));
         }
      }

      if (realsOK)
      {
         #ifdef DEBUG_CB_PROP_SAVE
            MessageInterface::ShowMessage("Reals are OK - setting them\n");
            MessageInterface::ShowMessage("mu = %12.4f, eqRad = %12.4f, flat = %12.4f\n",
                  mu, eqRad, flat);
            MessageInterface::ShowMessage("in Properties panel, body pointer is %p\n",
                  theBody);
         #endif

         if (muChanged)
            theBody->SetGravitationalConstant(mu);
         if (eqRadChanged)
            theBody->SetEquatorialRadius(eqRad);
         if (flatChanged)
            theBody->SetFlattening(flat);

         dataChanged = false;
         ResetChangeFlags(true);
      }
      else
      {
         canClose = false;
      }

   }
   catch (BaseException &ex)
   {
      canClose    = false;
      dataChanged = true;
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
   }

   #ifdef DEBUG_CB_PROP_SAVE
      MessageInterface::ShowMessage("At end of CBPropPanel::SaveData, canClose = %s\n",
         (canClose? "true" : "false"));
   #endif
   
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data from the body object to the panel widgets.
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::LoadData()
{
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage
      ("CelestialBodyPropertiesPanel::LoadData() entered, body = '%s'\n",
       theBody->GetName().c_str());
   #endif
   try
   {
      mu         = theBody->GetGravitationalConstant();
      muString   = guiManager->ToWxString(mu);
      muTextCtrl->SetValue(muString);
      
      eqRad       = theBody->GetEquatorialRadius();
      eqRadString = guiManager->ToWxString(eqRad);
      eqRadTextCtrl->SetValue(eqRadString);
      
      flat       = theBody->GetFlattening();
      flatString = guiManager->ToWxString(flat);
      flatTextCtrl->SetValue(flatString);
            
      if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable)
      {
         pckFileArray             = theBody->GetStringArrayParameter(
                                    theBody->GetParameterID("PlanetarySpiceKernelName"));
         unsigned int pckListSz   = pckFileArray.size();
         pckFileArrayWX           = new wxString[pckListSz];
         pckFiles.clear();
         for (unsigned int jj = 0; jj < pckListSz; jj++)
         {
            pckFiles.push_back(pckFileArray[jj]);
            pckFileArrayWX[jj] = wxString(pckFileArray[jj].c_str());
         }
         pckFileListBox->InsertItems(pckListSz, pckFileArrayWX, 0);
         pckFileListBox->SetSelection(pckListSz-1); // Select the last item
      }

      ResetChangeFlags();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage
      ("CelestialBodyPropertiesPanel::LoadData() leaving, body = '%s'\n",
       theBody->GetName().c_str());
   #endif
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
void CelestialBodyPropertiesPanel::Create()
{
   int bSize     = 2;
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Celestial Body Properties"));
   
   userDef                    = theBody->IsUserDefined();
   allowSpiceForDefaultBodies = ss->IsSpiceAllowedForDefaultBodies();

   // empty the temporary value strings
   // muString      = "";
   // eqRadString   = "";
   // flatString    = "";
   
   // mu
   muStaticText      = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"Mu"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   muTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_MU, wxT(""),
                       wxDefaultPosition, wxSize(150, -1),0,wxTextValidator(wxGMAT_FILTER_NUMERIC));
   muTextCtrl->SetToolTip(pConfig->Read(_T("MuHint")));
   muUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("km^3/sec^2"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   // eq. radius
   eqRadStaticText      = new wxStaticText(this, ID_TEXT, wxString("Equatorial " GUI_ACCEL_KEY "Radius"),
                          wxDefaultPosition, wxSize(-1,-1), 0);
   eqRadTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_EQRAD, wxT(""),
                          wxDefaultPosition, wxSize(150, -1),0,wxTextValidator(wxGMAT_FILTER_NUMERIC));
   eqRadTextCtrl->SetToolTip(pConfig->Read(_T("EquatorialRadiusHint")));
   eqRadUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT("km"),
                          wxDefaultPosition, wxSize(-1,-1), 0);
   // flattening
   flatStaticText      = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"Flattening"),
                         wxDefaultPosition, wxSize(-1,-1), 0);
   flatTextCtrl        = new wxTextCtrl(this, ID_TEXT_CTRL_FLAT, wxT(""),
                         wxDefaultPosition, wxSize(150, -1),0,wxTextValidator(wxGMAT_FILTER_NUMERIC));
   flatTextCtrl->SetToolTip(pConfig->Read(_T("FlatteningHint")));
   flatUnitsStaticText = new wxStaticText(this, ID_TEXT, wxT(""), // unitless
                         wxDefaultPosition, wxSize(-1,-1), 0);   
   
   wxBoxSizer *pckButtonSizer = NULL;
   if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable)
   {
      // PCK file(s)
      wxArrayString emptyList;
      pckStaticText       = new wxStaticText(this, ID_TEXT, wxString(GUI_ACCEL_KEY"PCK Files"),
                            wxDefaultPosition, wxSize(-1,-1), 0);
      pckFileListBox      = new wxListBox(this, ID_LIST_BOX_PCK_FILE, wxDefaultPosition, wxSize(80, 100),
                            emptyList, wxLB_EXTENDED|wxLB_NEEDED_SB|wxLB_HSCROLL);
      pckFileListBox->SetToolTip(pConfig->Read(_T("PCKFileListHint")));
      pckFileBrowseButton = new wxButton(this, ID_BROWSE_BUTTON_PCK_FILE, wxString(GUI_ACCEL_KEY"Add"),
                            wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
      pckFileBrowseButton->SetToolTip(pConfig->Read(_T("AddPCKFileHint")));
      pckFileRemoveButton = new wxButton(this, ID_REMOVE_BUTTON_PCK_FILE, wxString(GUI_ACCEL_KEY"Remove"),
                            wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
      pckFileRemoveButton->SetToolTip(pConfig->Read(_T("RemovePCKFileHint")));
      pckButtonSizer = new wxBoxSizer(wxHORIZONTAL);
      pckButtonSizer->Add(pckFileBrowseButton,0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
      pckButtonSizer->Add(pckFileRemoveButton,0, wxGROW|wxALIGN_CENTRE|wxALL, bSize);
   }


   // set the min width for one of the labels for each GmatStaticBoxSizer
   int minLabelSize = muStaticText->GetBestSize().x;
   minLabelSize = (minLabelSize < eqRadStaticText->GetBestSize().x) ? eqRadStaticText->GetBestSize().x : minLabelSize;
   minLabelSize = (minLabelSize < flatStaticText->GetBestSize().x) ? flatStaticText->GetBestSize().x : minLabelSize;
   
   eqRadStaticText->SetMinSize(wxSize(minLabelSize, eqRadStaticText->GetMinHeight()));
   
   wxFlexGridSizer *cbPropGridSizer = new wxFlexGridSizer(3,0,0);
   cbPropGridSizer->Add(muStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(muTextCtrl,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(muUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(eqRadStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(eqRadTextCtrl,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(eqRadUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(flatStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(flatTextCtrl,0, wxALIGN_LEFT|wxALL, bSize);
   cbPropGridSizer->Add(flatUnitsStaticText,0, wxALIGN_LEFT|wxALL, bSize);
//   cbPropGridSizer->Add(20,20,0,wxALIGN_LEFT|wxALL, bSize);
//   cbPropGridSizer->Add(20,20,0,wxALIGN_LEFT|wxALL, bSize);
//   cbPropGridSizer->Add(20,20,0,wxALIGN_LEFT|wxALL, bSize);
   
   if ((userDef || allowSpiceForDefaultBodies) && spiceAvailable)
   {
      cbPropGridSizer->Add(pckStaticText,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      cbPropGridSizer->Add(pckFileListBox,0, wxGROW|wxALIGN_LEFT|wxALL, bSize);
      cbPropGridSizer->Add(0, 0);

      cbPropGridSizer->Add(0, 0);
      cbPropGridSizer->Add(pckButtonSizer, 0, wxALIGN_CENTRE|wxALL, bSize);
      cbPropGridSizer->Add(0, 0);
   }

   GmatStaticBoxSizer *optionsSizer    = new GmatStaticBoxSizer(wxVERTICAL, this, "Options");
   optionsSizer->Add(cbPropGridSizer, 0, wxALIGN_LEFT|wxGROW, bSize); 
   
   pageSizer    = new GmatStaticBoxSizer(wxVERTICAL, this, "");
   pageSizer->Add(optionsSizer, 1, wxALIGN_LEFT|wxGROW, bSize); 
   
   this->SetAutoLayout(true);
   this->SetSizer(pageSizer);
   pageSizer->Fit(this);
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
void CelestialBodyPropertiesPanel::ResetChangeFlags(bool discardMods)
{
   muChanged      = false;
   eqRadChanged   = false;
   flatChanged    = false;
   pckChanged = false;
   if (discardMods)
   {
      muTextCtrl->DiscardEdits();
      eqRadTextCtrl->DiscardEdits();
      flatTextCtrl->DiscardEdits();
   }
   dataChanged = false;
}

//------------------------------------------------------------------------------
//Event Handling
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void OnMuTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the mu text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnMuTextCtrlChange(wxCommandEvent &event)
{
   if (muTextCtrl->IsModified())
   {
      muChanged   = true;
      dataChanged = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnEqRadTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the equatorial radius
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnEqRadTextCtrlChange(wxCommandEvent &event)
{
   if (eqRadTextCtrl->IsModified())
   {
      eqRadChanged   = true;
      dataChanged    = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnFlatTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the flattening coefficient
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnFlatTextCtrlChange(wxCommandEvent &event)
{
   if (flatTextCtrl->IsModified())
   {
      flatChanged   = true;
      dataChanged   = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnPckFileBrowseButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user selects the PCK browse
 * button.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnPckFileBrowseButton(wxCommandEvent &event)
{
   wxArrayString oldFiles = pckFileListBox->GetStrings();
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
         int numSelect = pckFileListBox->GetSelections(selections);
         for (int i = 0; i < numSelect; i++)
            pckFileListBox->Deselect(selections[i]);

         pckChanged   = true;
         dataChanged      = true;
         pckFileListBox->Append(fileName);
         pckFileListBox->SetStringSelection(fileName);
         theCBPanel->EnableUpdate(true);
      }
   }
}

//------------------------------------------------------------------------------
// void OnPckFileRemoveButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user selects the PCK remove
 * button.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnPckFileRemoveButton(wxCommandEvent &event)
{
   wxArrayInt selections;
   int numSelect = pckFileListBox->GetSelections(selections);
   // get the string values and delete the selected names from the list
   wxString    fileSelected;
   for (int i = numSelect-1; i >= 0; i--)
   {
      fileSelected = pckFileListBox->GetString(selections[i]);
      pckFilesToDelete.push_back(fileSelected.WX_TO_STD_STRING);
      pckFileListBox->Delete(selections[i]);
   }
   pckFilesDeleted = true;
   dataChanged     = true;
   theCBPanel->EnableUpdate(true);

   // Select the last item
   unsigned int count = pckFileListBox->GetCount();
   if (count > 0)
      pckFileListBox->SetSelection(count -1);
}

//------------------------------------------------------------------------------
// void OnPckFileListBoxChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the value on the PCK
 * file list box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyPropertiesPanel::OnPckFileListBoxChange(wxCommandEvent &event)
{
   pckChanged = true;
   dataChanged    = true;
   theCBPanel->EnableUpdate(true);
}


//------------------------------------------------------------------------------
// wxString ToString(Real rval)
//------------------------------------------------------------------------------
/**
 * Converts the Real input value to a wxString.
 *
 * @param <rval>    real value
 *
 * @return   the wxString representation of the input real value
 *
 */
//------------------------------------------------------------------------------
wxString CelestialBodyPropertiesPanel::ToString(Real rval)
{
   return guiManager->ToWxString(rval);
}
