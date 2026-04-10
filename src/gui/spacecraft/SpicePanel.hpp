//$Id:$
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
/* This class contains information needed to setup spacecraft spice
 * configurations (kernels, IDs)
 *
 */
//------------------------------------------------------------------------------
#ifndef SpicePanel_hpp
#define SpicePanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatAppData.hpp"
#include "GmatPanel.hpp"

#include "Spacecraft.hpp"

class SpicePanel: public wxPanel
{
public:
   SpicePanel(GmatPanel *scPanel, wxWindow *parent,
              Spacecraft *spacecraft);
   ~SpicePanel();

   void LoadData();
   void SaveData();

   bool IsDataChanged() { return dataChanged; }
   bool CanClosePanel() { return canClose; }

private:

   /// SPK kernel names - orbit
   StringArray spkKernelNames;
   /// CK kernel names - attitude/pointing
   StringArray ckKernelNames;
   /// SCLK kernel names - spacecraft clock
   StringArray sclkKernelNames;
   /// FK kernel names - frames
   StringArray fkKernelNames;

   Integer naifId;
   Integer naifIdRefFrame;

   StringArray spkFilesToDelete;
   StringArray ckFilesToDelete;
   StringArray sclkFilesToDelete;
   StringArray fkFilesToDelete;


   bool dataChanged;
   bool canClose;

   Spacecraft     *theSpacecraft;
   GuiItemManager *guiManager;
   GuiInterpreter *guiInterpreter;

   bool spkNameChanged;
   bool ckNameChanged;
   bool sclkNameChanged;
   bool fkNameChanged;
   bool naifIdChanged;
   bool naifIdRefFrameChanged;

   bool spkFilesDeleted;
   bool ckFilesDeleted;
   bool sclkFilesDeleted;
   bool fkFilesDeleted;



   void Create();
   void AddElements(wxWindow *parent);

   // Event Handling
   DECLARE_EVENT_TABLE();
   void OnTextChange(wxCommandEvent& event);
   void OnSpkFileBrowseButton(wxCommandEvent &event);
   void OnSpkFileRemoveButton(wxCommandEvent &event);
   void OnSpkFileListBoxChange(wxCommandEvent &event);
   void OnCkFileBrowseButton(wxCommandEvent &event);
   void OnCkFileRemoveButton(wxCommandEvent &event);
   void OnCkFileListBoxChange(wxCommandEvent &event);
   void OnSclkFileBrowseButton(wxCommandEvent &event);
   void OnSclkFileRemoveButton(wxCommandEvent &event);
   void OnSclkFileListBoxChange(wxCommandEvent &event);
   void OnFkFileBrowseButton(wxCommandEvent &event);
   void OnFkFileRemoveButton(wxCommandEvent &event);
   void OnFkFileListBoxChange(wxCommandEvent &event);
   void OnNaifIdTextCtrlChange(wxCommandEvent &event);
   void OnNaifIdRefTextCtrlChange(wxCommandEvent &event);

   wxString ToString(Real rval);

   bool IsStateModified();
   void ResetChangeFlags(bool discardEdits = false);

   GmatPanel    *theScPanel;

   wxStaticText *spkFileStaticText;
   wxStaticText *ckFileStaticText;
   wxStaticText *sclkFileStaticText;
   wxStaticText *fkFileStaticText;
   wxStaticText *naifIdStaticText;
   wxStaticText *naifIdRefFrameStaticText;
   wxStaticText *naifIdBlankText;
   wxStaticText *naifIdRefFrameBlankText;

   wxTextCtrl   *naifIdTextCtrl;
   wxTextCtrl   *naifIdRefFrameTextCtrl;

   wxListBox    *spkFileListBox;
   wxListBox    *ckFileListBox;
   wxListBox    *sclkFileListBox;
   wxListBox    *fkFileListBox;

   wxButton     *spkFileBrowseButton;
   wxButton     *spkFileRemoveButton;
   wxButton     *ckFileBrowseButton;
   wxButton     *ckFileRemoveButton;
   wxButton     *sclkFileBrowseButton;
   wxButton     *sclkFileRemoveButton;
   wxButton     *fkFileBrowseButton;
   wxButton     *fkFileRemoveButton;

   wxString     *spkFileArrayWX;
   wxString     *ckFileArrayWX;
   wxString     *sclkFileArrayWX;
   wxString     *fkFileArrayWX;

   wxString     spkFileStringWX;
   wxString     ckFileStringWX;
   wxString     sclkFileStringWX;
   wxString     fkFileStringWX;
   wxString     naifIdStringWX;
   wxString     naifIdRefFrameStringWX;

   wxBoxSizer      *mainBoxSizer;
   wxFlexGridSizer *leftFlexGridSizer;
   wxFlexGridSizer *rightFlexGridSizer;

   // IDs for the controls and the menu commands
   enum
   {
      ID_TEXT = 30500,
      ID_TEXTCTRL,
      ID_LIST_BOX_SPK_FILE,
      ID_BROWSE_BUTTON_SPK_FILE,
      ID_REMOVE_BUTTON_SPK_FILE,
      ID_LIST_BOX_CK_FILE,
      ID_BROWSE_BUTTON_CK_FILE,
      ID_REMOVE_BUTTON_CK_FILE,
      ID_LIST_BOX_SCLK_FILE,
      ID_BROWSE_BUTTON_SCLK_FILE,
      ID_REMOVE_BUTTON_SCLK_FILE,
      ID_LIST_BOX_FK_FILE,
      ID_BROWSE_BUTTON_FK_FILE,
      ID_REMOVE_BUTTON_FK_FILE,
      ID_TEXT_CTRL_NAIF_ID,
      ID_TEXT_CTRL_NAIF_ID_REF_FRAME,
   };
};
#endif
