//$Id$
//------------------------------------------------------------------------------
//                                  CelestialBodyOrientationPanel
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
// Created: 2009.01.30
//
/**
 * This is the panel for the Orientation tab on the notebook on the CelestialBody
 * Panel.
 *
 */
//------------------------------------------------------------------------------

#ifndef CelestialBodyOrientationPanel_hpp
#define CelestialBodyOrientationPanel_hpp

#include "gmatdefs.hpp"
#include "CelestialBody.hpp"
#include "gmatwxdefs.hpp"
#include "GuiItemManager.hpp"
#include "GmatPanel.hpp"
#include "GmatStaticBoxSizer.hpp"

class CelestialBodyOrientationPanel : public wxPanel
{
public:
   CelestialBodyOrientationPanel(GmatPanel *cbPanel, wxWindow *parent, CelestialBody *body);
   ~CelestialBodyOrientationPanel();
   
   void SaveData();
   void LoadData();
   
   bool IsDataChanged() { return dataChanged;};
   bool CanClosePanel() { return canClose;};
   
private:
   
   bool           userDef;
   bool           allowSpiceForDefaultBodies;
   bool           spiceAvailable;

   bool           dataChanged;
   bool           canClose;
   bool           fkFilesDeleted;

   StringArray    fkFiles;
   StringArray    fkFilesToDelete;

   
   CelestialBody  *theBody;
   GuiInterpreter *guiInterpreter;
   GuiItemManager *guiManager;
   SolarSystem    *ss;

   std::string    rotationDataSource;
   Real           nutationUpdateInterval;;
   Real           spinAxisRAConstant;
   Real           spinAxisRARate;
   Real           spinAxisDECConstant;
   Real           spinAxisDECRate;
   Real           rotationConstant;
   Real           rotationRate;
   std::string    spiceFrameID;
   
   bool           rotationDataSourceChanged;
   bool           nutationUpdateIntervalChanged;
   bool           spinAxisRAConstantChanged;
   bool           spinAxisRARateChanged;
   bool           spinAxisDECConstantChanged;
   bool           spinAxisDECRateChanged;
   bool           rotationConstantChanged;
   bool           rotationRateChanged;
   bool           spiceFrameIDChanged;
   bool           fkChanged;

   bool           isEarth;
   bool           isLuna;
   
   GmatPanel      *theCBPanel;
   
  
   void     Create();
   void     ResetChangeFlags(bool discardMods = false);
   
   //Event Handling
   DECLARE_EVENT_TABLE();
   void     OnRotationDataSourceComboBoxChange(wxCommandEvent &event);
   void     OnNutationUpdateIntervalTextCtrlChange(wxCommandEvent &event);
   void     OnSpinAxisRAConstantTextCtrlChange(wxCommandEvent &event);
   void     OnSpinAxisRARateTextCtrlChange(wxCommandEvent &event);
   void     OnSpinAxisDECConstantTextCtrlChange(wxCommandEvent &event);
   void     OnSpinAxisDECRateTextCtrlChange(wxCommandEvent &event);
   void     OnRotationConstantTextCtrlChange(wxCommandEvent &event);
   void     OnRotationRateTextCtrlChange(wxCommandEvent &event);
   void     OnSpiceFrameIDTextCtrlChange(wxCommandEvent &event);
   void     OnFkFileBrowseButton(wxCommandEvent &event);
   void     OnFkFileRemoveButton(wxCommandEvent &event);
   void     OnFkFileListBoxChange(wxCommandEvent &event);
   
   wxString ToString(Real rval);
   
   // wx
   
   wxStaticText *rotationDataSourceStaticText;
   wxStaticText *nutationUpdateIntervalStaticText;
   wxStaticText *spinAxisRAConstantStaticText;
   wxStaticText *spinAxisRARateStaticText;
   wxStaticText *spinAxisDECConstantStaticText;
   wxStaticText *spinAxisDECRateStaticText;
   wxStaticText *rotationConstantStaticText;
   wxStaticText *rotationRateStaticText;
   wxStaticText *spiceFrameIDStaticText;
   wxStaticText *fkStaticText;

   wxStaticText *nutationUpdateIntervalUnitsStaticText;
   wxStaticText *spinAxisRAConstantUnitsStaticText;
   wxStaticText *spinAxisRARateUnitsStaticText;
   wxStaticText *spinAxisDECConstantUnitsStaticText;
   wxStaticText *spinAxisDECRateUnitsStaticText;
   wxStaticText *rotationConstantUnitsStaticText;
   wxStaticText *rotationRateUnitsStaticText;

   wxTextCtrl   *nutationUpdateIntervalTextCtrl;
   wxTextCtrl   *spinAxisRAConstantTextCtrl;
   wxTextCtrl   *spinAxisRARateTextCtrl;
   wxTextCtrl   *spinAxisDECConstantTextCtrl;
   wxTextCtrl   *spinAxisDECRateTextCtrl;
   wxTextCtrl   *rotationConstantTextCtrl;
   wxTextCtrl   *rotationRateTextCtrl;
   wxTextCtrl   *spiceFrameIDTextCtrl;
   wxTextCtrl   *fkTextCtrl;
   
   wxComboBox   *rotationDataSourceComboBox;
   
   // strings for the combo box
   StringArray  sourceArray;
   
   // strings for the FK kernel names
   StringArray  fkFileArray;

   wxString     *fkFileArrayWX;

   // wxString arrays for the combo box
   wxString     *sourceArrayWX;
   
   /// string versions of current data
   wxString     rotationDataSourceStringWX;
   wxString     nutationUpdateIntervalStringWX;
   wxString     spinAxisRAConstantStringWX;
   wxString     spinAxisRARateStringWX;
   wxString     spinAxisDECConstantStringWX;
   wxString     spinAxisDECRateStringWX;
   wxString     rotationConstantStringWX;
   wxString     rotationRateStringWX;
   wxString     spiceFrameIDStringWX;

   
   wxListBox    *fkFileListBox;

   wxButton     *fkFileBrowseButton;
   wxButton     *fkFileRemoveButton;

   GmatStaticBoxSizer *mainBoxSizer;
   
   /// IDs for the controls 
   enum
   {
      ID_TEXT = 7100,
      ID_COMBO_BOX_ROTATION_DATA_SOURCE,
      ID_TEXT_CTRL_NUTATION_UPDATE_INTERVAL,
      ID_TEXT_CTRL_SPIN_AXIS_RA_CONSTANT,
      ID_TEXT_CTRL_SPIN_AXIS_RA_RATE,
      ID_TEXT_CTRL_SPIN_AXIS_DEC_CONSTANT,
      ID_TEXT_CTRL_SPIN_AXIS_DEC_RATE,
      ID_TEXT_CTRL_ROTATION_CONSTANT,
      ID_TEXT_CTRL_ROTATION_RATE,
      ID_TEXT_CTRL_SPICE_FRAME_ID,
      ID_LIST_BOX_FK_FILE,
      ID_BROWSE_BUTTON_FK_FILE,
      ID_REMOVE_BUTTON_FK_FILE,
   };
};
#endif // CelestialBodyOrientationPanel_hpp
