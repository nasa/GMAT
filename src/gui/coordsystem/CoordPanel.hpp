//$Id$
//------------------------------------------------------------------------------
//                              CoordPanel
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
// Created: 2005/03/11
/**
 * This class contains the Coordinate System Panel for CoordSystemConfigPanel
 * and CoordSysCreateDialog.
 */
//------------------------------------------------------------------------------
#ifndef CoordPanel_hpp
#define CoordPanel_hpp

#include "gmatwxdefs.hpp"
#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatAppData.hpp"
#include "CoordinateSystem.hpp"
#include "UserInputValidator.hpp"

class CoordPanel : public wxPanel, public UserInputValidator
{
public:
    // constructors/destructors
    CoordPanel(wxWindow *parent, bool enableAll);
    ~CoordPanel();
    
   wxString epochValue;   // why are these public?  wcs 2012.09.20
   wxString epochFormatValue;
   
   wxComboBox *GetOriginComboBox() {return originComboBox;}
   wxComboBox *GetTypeComboBox() {return typeComboBox;}
   wxComboBox *GetPrimaryComboBox() {return primaryComboBox;}
   wxComboBox *GetSecondaryComboBox() {return secondaryComboBox;}
   wxComboBox *GetReferenceObjectComboBox() {return refObjectComboBox;}
   wxComboBox *GetConstraintCSComboBox() {return constraintCSComboBox;}
   wxComboBox *GetFormatComboBox() {return formatComboBox;}
   
   wxComboBox *GetXComboBox() {return xComboBox;}
   wxComboBox *GetYComboBox() {return yComboBox;}
   wxComboBox *GetZComboBox() {return zComboBox;}
   
   bool GetShowPrimaryBody() {return mShowPrimaryBody;}
   bool GetShowSecondaryBody() {return mShowSecondaryBody;}
   bool GetShowAlignmentConstraint() {return mShowAlignmentConstraint;}
   bool GetShowEpoch() {return mShowEpoch;}
   bool GetShowXyz() {return mShowXyz;}
   bool GetShowUpdateInterval() {return mShowUpdate;}
   
   wxTextCtrl *GetEpochTextCtrl() {return epochTextCtrl;}
   wxTextCtrl *GetIntervalTextCtrl() {return intervalTextCtrl;}
   
   bool IsAlignmentConstraintTextModified();
   bool CheckAlignmentConstraintValues(bool setOnAxis = false, AxisSystem* theAxis = NULL);

   void EnableOptions(AxisSystem *axis = NULL);
   void SetDefaultAxis();
   void SetDefaultEpochRefAxis();
   void SetDefaultObjectRefAxis();
   void SetDefaultAlignmentConstraintAxis();
   void ShowAxisData(AxisSystem *axis);
   AxisSystem* CreateAxis();
   bool SaveData(const std::string &coordName, AxisSystem *axis,
                 wxString &epochFormat);

private:
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   
   bool mShowPrimaryBody;
   bool mShowSecondaryBody;
   bool mShowAlignmentConstraint;
   bool mShowEpoch;
   bool mShowXyz;
   bool mShowUpdate;
   bool mEnableAll;
   
   bool setThoughDisabled;

   wxStaticText *originStaticText;
   wxStaticText *typeStaticText;
   wxStaticText *primaryStaticText;
   wxStaticText *formatStaticText;
   wxStaticText *secondaryStaticText;
   wxStaticText *epochStaticText;
   wxStaticText *refObjectStaticText;
   wxStaticText *constraintCSStaticText;

   wxStaticText *alignXStaticText;
   wxStaticText *alignYStaticText;
   wxStaticText *alignZStaticText;
   wxStaticText *constraintXStaticText;
   wxStaticText *constraintYStaticText;
   wxStaticText *constraintZStaticText;
   wxStaticText *constraintRefXStaticText;
   wxStaticText *constraintRefYStaticText;
   wxStaticText *constraintRefZStaticText;

   wxStaticText *xStaticText;
   wxStaticText *yStaticText;
   wxStaticText *zStaticText;

   wxStaticText *updateStaticText;
   wxStaticText *secStaticText;

   wxTextCtrl *epochTextCtrl;
   wxTextCtrl *intervalTextCtrl;

   wxTextCtrl *alignXTextCtrl;
   wxTextCtrl *alignYTextCtrl;
   wxTextCtrl *alignZTextCtrl;
   wxTextCtrl *constraintXTextCtrl;
   wxTextCtrl *constraintYTextCtrl;
   wxTextCtrl *constraintZTextCtrl;
   wxTextCtrl *constraintRefXTextCtrl;
   wxTextCtrl *constraintRefYTextCtrl;
   wxTextCtrl *constraintRefZTextCtrl;

   wxComboBox *originComboBox;
   wxComboBox *typeComboBox;
   wxComboBox *primaryComboBox;
   wxComboBox *formatComboBox;
   wxComboBox *secondaryComboBox;
   wxComboBox *refObjectComboBox;
   wxComboBox *constraintCSComboBox;

   wxComboBox *xComboBox;
   wxComboBox *yComboBox;
   wxComboBox *zComboBox;
   
   wxSizer  *flexgridsizerAlignment;
   wxSizer  *flexgridsizerConstraint;
   
   wxSizer  *flexgridsizer1;
   wxSizer  *boxsizer2;
   
   wxSizer  *staticboxsizerA;
   wxSizer  *staticboxsizerC;
   wxSizer  *staticboxsizerAxes;
   
   void Create();
   void LoadData();
   
   void EnableAlignmentConstraint(bool enableAll = true);

   bool IsValidAxis(const wxString &axisType, const wxString &priName,
                    const wxString &secName,  const wxString &x,
                    const wxString &y,        const wxString &z,
                    const wxString &refName,  const wxString &constraintCSName);
   bool IsValidXYZ(const wxString &x, const wxString &y, const wxString &z);
   
   // IDs for the controls and the menu commands
   enum
   {
       ID_TEXTCTRL = 46000,
       ID_COMBO,
       ID_TEXT,
   };
};

#endif // CoordPanel_hpp
