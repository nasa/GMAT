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


#ifndef CelestialBodyOrbitPanel_hpp
#define CelestialBodyOrbitPanel_hpp

#include "gmatdefs.hpp"
#include "CelestialBody.hpp"
#include "SolarSystem.hpp"
#include "GuiInterpreter.hpp"
#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"  
#include "GmatPanel.hpp"
#include "GmatStaticBoxSizer.hpp"

class CelestialBodyOrbitPanel : public wxPanel
{
public:
   CelestialBodyOrbitPanel(GmatPanel *cbPanel, wxWindow *parent, CelestialBody *body);
   ~CelestialBodyOrbitPanel();
   
   void SaveData();
   void LoadData();
   
   bool IsDataChanged() { return dataChanged;};
   bool CanClosePanel() { return canClose;};
   
private:
   
   bool           dataChanged;
   bool           canClose;
   
   CelestialBody  *theBody;
   GuiItemManager *guiManager;
   GuiInterpreter *guiInterpreter;
   SolarSystem    *ss;
   
   std::string    ephemSrc;
   std::string    previousEphemSrc;
   std::string    ephemFile;
   StringArray    spkFiles;
   Integer        naifID;
   std::string    centralBody;
   Real           initialEpoch;
   Real           SMA;
   Real           ECC;
   Real           INC;
   Real           RAAN;
   Real           AOP;
   Real           TA;
   
   StringArray    spkFilesToDelete;

   bool           ephemSrcChanged;
   bool           ephemFileChanged;
   bool           spkFileChanged;
   bool           naifIDChanged;
   bool           cBodyChanged;
   bool           epochChanged;
   bool           SMAChanged;
   bool           ECCChanged;
   bool           INCChanged;
   bool           RAANChanged;
   bool           AOPChanged;
   bool           TAChanged;
   bool           spkFilesDeleted;
   
   bool           userDef;
   bool           allowSpiceForDefaultBodies;
   
   bool           isSun;
   bool           spiceAvailable;
   bool           includeTwoBody;
   
   GmatPanel      *theCBPanel;
   
  
   void     Create();
   void     ResetChangeFlags(bool discardMods = false);
   
   //Event Handling
   DECLARE_EVENT_TABLE();
   void     OnEphemSourceComboBoxChange(wxCommandEvent &event);
   void     OnEphemFileTextCtrlChange(wxCommandEvent &event);
   void     OnEphemFileBrowseButton(wxCommandEvent &event);
   void     OnSpkFileBrowseButton(wxCommandEvent &event);
   void     OnSpkFileRemoveButton(wxCommandEvent &event);
   void     OnSpkFileListBoxChange(wxCommandEvent &event);
   void     OnNaifIdTextCtrlChange(wxCommandEvent &event);
   void     OnCentralBodyComboBoxChange(wxCommandEvent &event);
   void     OnEpochTextCtrlChange(wxCommandEvent &event);
   void     OnSMATextCtrlChange(wxCommandEvent &event);
   void     OnECCTextCtrlChange(wxCommandEvent &event);
   void     OnINCTextCtrlChange(wxCommandEvent &event);
   void     OnRAANTextCtrlChange(wxCommandEvent &event);
   void     OnAOPTextCtrlChange(wxCommandEvent &event);
   void     OnTATextCtrlChange(wxCommandEvent &event);
   
   // wx
   
   wxStaticText *ephemSourceStaticText;
   wxStaticText *ephemFileStaticText;
   wxStaticText *spkFileStaticText;
   wxStaticText *naifIDStaticText;
   wxStaticText *naifIDBlankText;
   wxStaticText *centralBodyStaticText;
   wxStaticText *initialEpochStaticText;
   wxStaticText *SMAStaticText;
   wxStaticText *ECCStaticText;
   wxStaticText *INCStaticText;
   wxStaticText *RAANStaticText;
   wxStaticText *AOPStaticText;
   wxStaticText *TAStaticText;

   wxStaticText *SMAUnitsStaticText;
   wxStaticText *ECCUnitsStaticText;
   wxStaticText *INCUnitsStaticText;
   wxStaticText *RAANUnitsStaticText;
   wxStaticText *AOPUnitsStaticText;
   wxStaticText *TAUnitsStaticText;

   wxTextCtrl   *ephemFileTextCtrl;
   wxTextCtrl   *naifIDTextCtrl;
   wxTextCtrl   *initialEpochTextCtrl;
   wxTextCtrl   *SMATextCtrl;
   wxTextCtrl   *ECCTextCtrl;
   wxTextCtrl   *INCTextCtrl;
   wxTextCtrl   *RAANTextCtrl;
   wxTextCtrl   *AOPTextCtrl;
   wxTextCtrl   *TATextCtrl;
   
   wxListBox    *spkFileListBox;
   
   wxComboBox   *ephemSourceComboBox;
   wxComboBox   *centralBodyComboBox;
   
   wxBitmapButton     *ephemFileBrowseButton;
   wxButton     *spkFileBrowseButton;
   wxButton     *spkFileRemoveButton;
   
   // strings for the combo boxes
   StringArray  sourceArray;
   StringArray  spkFileArray;
   
   // wxString arrays for the combo boxes
   wxString     *sourceArrayWX;
   wxString     *spkFileArrayWX;
   
   /// string versions of current data
   wxString     ephemSourceStringWX;
   wxString     ephemFileStringWX;
   wxString     spkFileStringWX;
   wxString     naifIDStringWX;
   wxString     centralBodyStringWX;
   wxString     initialEpochStringWX;
   wxString     SMAStringWX;
   wxString     ECCStringWX;
   wxString     INCStringWX;
   wxString     RAANStringWX;
   wxString     AOPStringWX;
   wxString     TAStringWX;
   
   
   wxBoxSizer      *mainBoxSizer;
   wxFlexGridSizer *orbitDataFlexGridSizer;

   
   /// IDs for the controls 
   enum
   {
      ID_TEXT = 7100,
      ID_COMBO_BOX_EPHEM_SOURCE,
      ID_TEXT_CTRL_EPHEM_FILE,
      ID_LIST_BOX_SPK_FILE,
      ID_BROWSE_BUTTON_EPHEM_FILE,
      ID_BROWSE_BUTTON_SPK_FILE,
      ID_REMOVE_BUTTON_SPK_FILE,
      ID_TEXT_CTRL_NAIF_ID,
      ID_COMBO_BOX_CENTRAL_BODY,
      ID_TEXT_CTRL_INITIAL_EPOCH,
      ID_TEXT_CTRL_SMA,
      ID_TEXT_CTRL_ECC,
      ID_TEXT_CTRL_INC,
      ID_TEXT_CTRL_RAAN,
      ID_TEXT_CTRL_AOP,
      ID_TEXT_CTRL_TA,
   };
   
                                
};
#endif // CelestialBodyOrbitPanel_hpp
