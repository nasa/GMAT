//$Id$
//------------------------------------------------------------------------------
//                           SolverVariablesPanel
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
// Author: Waka Waktola
// Created: 2004/21/01
// Modified: 2004/05/06 by Allison Greene to inherit from GmatPanel
/**
 * This class contains the Solver Variables window.
 */
//------------------------------------------------------------------------------

#ifndef SolverVariablesPanel_hpp
#define SolverVariablesPanel_hpp

#include "gmatwxdefs.hpp"

// base includes
#include "gmatdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GmatPanel.hpp"

class SolverVariablesPanel : public GmatPanel
{
public:
    // constructors
    SolverVariablesPanel(wxWindow *parent);
    ~SolverVariablesPanel(); 
    
private:             
    wxGrid *varsGrid;
    
    wxStaticText *descStaticText;
    wxStaticText *solverStaticText;
    wxStaticText *varStaticText;
    wxStaticText *pertStaticText;
    wxStaticText *maxStaticText;
    wxStaticText *lowerStaticText;
    wxStaticText *upperStaticText;
    
    wxTextCtrl *descTextCtrl;
    wxTextCtrl *varTextCtrl;
    wxTextCtrl *pertTextCtrl;
    wxTextCtrl *maxTextCtrl;
    wxTextCtrl *lowerTextCtrl;
    wxTextCtrl *upperTextCtrl;
    
    wxButton *editButton;
    wxButton *updateButton;
    
    wxComboBox *solverComboBox;
    
    wxString solverString;
    wxString propertyString;
    wxString descriptionString;
    
    Integer numOfVars;
    Integer nextRow;
        
    // methods inherited from GmatPanel
    virtual void Create();
    virtual void LoadData();
    virtual void SaveData();
    //loj: 2/27/04 commented out
    //virtual void OnHelp();
    //virtual void OnScript();

    // Layout & data handling methods
    void Initialize();
    void Setup(wxWindow *parent);
    
    // Grid table event method
    void OnCellValueChanged(wxGridEvent &event);
    
    // Text control event method
    void OnTextUpdate(wxCommandEvent& event);    
    
    // Combobox event method
    void OnSolverSelection(wxCommandEvent &event);
    
    // Button event methods
    void OnButton(wxCommandEvent& event);

    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE();
  
    // IDs for the controls and the menu commands
    enum
    {     
        ID_TEXT = 53000,
        ID_TEXTCTRL,
        ID_BUTTON,
        ID_COMBO,
        ID_GRID,
        SOL_COL,
        PRO_COL,
        DES_COL
    };
};

#endif // SolverGoalsPanel_hpp

