//$Id$
//------------------------------------------------------------------------------
//                              ThrusterCoefficientDialog
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
// Created: 2005/01/13
//
/**
 * Implements ThrusterCoefficientDialog class. This class shows dialog window where
 * thruster coefficients can be modified.
 * 
 */
//------------------------------------------------------------------------------

#include "ThrusterCoefficientDialog.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include <wx/variant.h>

//#define DEBUG_COEFS
//#define DEBUG_COEF_SAVE

//------------------------------------------------------------------------------
// event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ThrusterCoefficientDialog, GmatDialog)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ThrusterCoefficientDialog(wxWindow *parent, wxWindowID id, 
//                           const wxString &title, GmatBase *obj,Integer numCoefs,
//                           const RealArray &coefs1, const RealArray &coefs2);
//------------------------------------------------------------------------------
ThrusterCoefficientDialog::
ThrusterCoefficientDialog(wxWindow *parent, wxWindowID id, 
        const wxString &title, GmatBase *obj, Integer numCoefs,
        const RealArray &coefs1, const RealArray &coefs2)
: GmatDialog(parent, id, title, obj, wxDefaultPosition, wxDefaultSize)
{
   theObject      = obj;
   isElectric     = theObject->IsOfType("ElectricThruster");
   coefsCount     = numCoefs;
   coefs1Modified = false;
   coefs2Modified = false;

   coefs1Names.clear();
   coefs2Names.clear();

   coefs1Values.clear();
   coefs2Values.clear();
   
   coefs1Values   = coefs1;
   coefs2Values   = coefs2;
   
   #ifdef DEBUG_COEFS
   MessageInterface::ShowMessage("In TCD constructor, size of 1st coefs array is %d\n", (Integer) coefs1Values.size());
   for (unsigned int ii = 0; ii < coefs1Values.size(); ii++)
   MessageInterface::ShowMessage("coefs1Values[%d] = %lf\n", ii, coefs1Values.at(ii));
   MessageInterface::ShowMessage("In TCD constructor, size of K coefs array is %d\n", (Integer) coefs2Values.size());
   for (unsigned int ii = 0; ii < coefs2Values.size(); ii++)
   MessageInterface::ShowMessage("coefs2Values[%d] = %lf\n", ii, coefs2Values.at(ii));
   #endif


   if (obj != NULL)
   {
      Create();
      ShowData();
   }
}

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::Create()
{
   coefNotebook = new
      wxNotebook( this, ID_NOTEBOOK, wxDefaultPosition,
                  wxDefaultSize, wxGROW );

   coefGrid1 =
      new wxGrid( coefNotebook, ID_GRID1, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);

   coefGrid1->EnableDragGridSize(false);
   coefGrid1->EnableDragColSize(false);
   coefGrid1->CreateGrid(coefsCount, 3);
   coefGrid1->SetRowLabelSize(0);
   coefGrid1->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);

   coefGrid1->SetColLabelValue(0, _T("Coefficient"));
   coefGrid1->SetColSize(0, 115);
   coefGrid1->SetColLabelValue(1, _T("Value"));
   coefGrid1->SetColSize(1, 135);
   coefGrid1->SetColLabelValue(2, _T("Unit"));
   coefGrid1->SetColSize(2, 80);

   // The first and third columns are read only
   for (int i=0; i<coefsCount; i++)
   {
      coefGrid1->SetReadOnly(i, 0);
      coefGrid1->SetReadOnly(i, 2);
   }


   coefGrid2 =
      new wxGrid( coefNotebook, ID_GRID2, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS);

   coefGrid2->EnableDragGridSize(false);
   coefGrid2->EnableDragColSize(false);
   coefGrid2->CreateGrid(coefsCount, 3);
   coefGrid2->SetRowLabelSize(0);
   coefGrid2->SetDefaultCellAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);

   coefGrid2->SetColLabelValue(0, _T("Coefficient"));
   coefGrid2->SetColSize(0, 115);
   coefGrid2->SetColLabelValue(1, _T("Value"));
   coefGrid2->SetColSize(1, 135);
   coefGrid2->SetColLabelValue(2, _T("Unit"));
   coefGrid2->SetColSize(2, 80);

   // The first and third columns are read only
   for (int i=0; i<coefsCount; i++)
   {
      coefGrid2->SetReadOnly(i, 0);
      coefGrid2->SetReadOnly(i, 2);
   }

   if (isElectric)
   {
      coefNotebook->AddPage( coefGrid1, wxT("Thrust Coefficients") );
      coefNotebook->AddPage( coefGrid2, wxT("Mass Flow Coefficients") );
   }
   else
   {
      coefNotebook->AddPage( coefGrid1, wxT("Thrust Coefficients") );
      coefNotebook->AddPage( coefGrid2, wxT("Impulse Coefficients") );
   }

   theMiddleSizer->Add(coefNotebook, 1, wxALIGN_CENTRE|wxGROW|wxALL, 5);
}

//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::LoadData()
{
   #ifdef DEBUG_COEFS_LOAD
      MessageInterface::ShowMessage(
            "In ThrusterConfigDialog::LoadData ..., coefsCount = %d\n",
            coefsCount);
   #endif

   std::stringstream c1Strings("");
   std::stringstream c2Strings("");

   coefs1Names.clear();
   coefs2Names.clear();

   std::string coefs1Label;
   std::string coefs2Label;
   StringArray coefs1Units;
   StringArray coefs2Units;

   if (isElectric)
   {
      coefs1Units = theObject->GetStringArrayParameter("T_UNITS");
      coefs2Units = theObject->GetStringArrayParameter("MF_UNITS");
      coefs1Label = "ThrustCoeff";
      coefs2Label = "MassFlowCoeff";
   }
   else
   {
      coefs1Units = theObject->GetStringArrayParameter("C_UNITS");
      coefs2Units = theObject->GetStringArrayParameter("K_UNITS");
      coefs1Label = "C";
      coefs2Label = "K";
   }

   for (Integer ii = 0; ii < coefsCount; ii++)
   {
      c1Strings << coefs1Label << ii + 1;
      coefs1Names.push_back(c1Strings.str());
      c1Strings.str("");

      c2Strings << coefs2Label << ii + 1;
      coefs2Names.push_back(c2Strings.str());
      c2Strings.str("");
      #ifdef DEBUG_COEFS_LOAD
         MessageInterface::ShowMessage(
               "In TCDialog::LoadData, coefs1Names[%d] = %s, units = %s\n",
               ii, coefs1Names.at(ii).c_str(), coefs1Units.at(ii).c_str());
         MessageInterface::ShowMessage(
               "In TCDialog::LoadData, coefs2Names[%d] = %s, units = %s\n",
               ii, coefs2Names.at(ii).c_str(), coefs2Units.at(ii).c_str());
      #endif
   }

   for (Integer jj = 0; jj < coefsCount; jj++)
   {
      #ifdef DEBUG_COEFS_LOAD
         MessageInterface::ShowMessage(
               "In TCDialog::LoadData, %d, setting name = %s,value = %12.10f, units = %s\n",
               jj, coefs1Names[jj].c_str(), coefs1Values[jj], coefs1Units[jj].c_str() );
      #endif
      coefGrid1->SetCellValue(jj, 0, coefs1Names[jj].c_str());
      coefGrid1->SetCellValue(jj, 1, wxVariant(coefs1Values[jj]));
      coefGrid1->SetCellValue(jj, 2, coefs1Units[jj].c_str());

      #ifdef DEBUG_COEFS_LOAD
         MessageInterface::ShowMessage(
               "In TCDialog::LoadData, %d, setting name = %s,value = %12.10f, units = %s\n",
               jj, coefs2Names[jj].c_str(), coefs2Values[jj],coefs2Units[jj].c_str() );
      #endif
      coefGrid2->SetCellValue(jj, 0, coefs2Names[jj].c_str());
      coefGrid2->SetCellValue(jj, 1, wxVariant(coefs2Values[jj]));
      coefGrid2->SetCellValue(jj, 2, coefs2Units[jj].c_str());
   }
}

//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::SaveData()
{
   #ifdef DEBUG_COEF_SAVE
   MessageInterface::ShowMessage
      ("ThrusterConfigDialog::SaveData() entered\n");
   #endif
   
   canClose = true;
   
   // Validate input values
   std::string field = "";
   std::string input = "";
   Real cellValue;
   bool cellValueOK = false;
   // C
   for (int i = 0; i < coefsCount; i++)
   {
      field = coefGrid1->GetCellValue(i, 0).c_str();
      input = coefGrid1->GetCellValue(i, 1).c_str();
      #ifdef DEBUG_COEF_SAVE
         MessageInterface::ShowMessage("   %s = '%s'\n", field.c_str(), input.c_str());
      #endif
      cellValueOK = CheckReal(cellValue, input, field, "Real Number");
      #ifdef DEBUG_COEF_SAVE
         MessageInterface::ShowMessage("   check real is %s, cellValue = %lf, coefs1Values[%d] = %lf\n",
               (cellValueOK? "true" : "false"), cellValue, (Integer) i, coefs1Values.at(i));
      #endif
      if (cellValueOK)
      {
         if (cellValue != coefs1Values.at(i))  coefs1Modified = true;
         coefs1Values.at(i) = cellValue;
      }
      else
      {
         canClose = false;
      }
   }

   cellValueOK = false;
   // K
   for (int i = 0; i < coefsCount; i++)
   {
      field = coefGrid2->GetCellValue(i, 0).c_str();
      input = coefGrid2->GetCellValue(i, 1).c_str();
      #ifdef DEBUG_COEF_SAVE
         MessageInterface::ShowMessage("   %s = '%s'\n", field.c_str(), input.c_str());
      #endif
      cellValueOK = CheckReal(cellValue, input, field, "Real Number");
      #ifdef DEBUG_COEF_SAVE
         MessageInterface::ShowMessage("   check real is %s, cellValue = %lf, coefs2Values[%d] = %lf\n",
               (cellValueOK? "true" : "false"), cellValue, (Integer) i, coefs2Values.at(i));
      #endif
      if (cellValueOK)
      {
         if (cellValue != coefs2Values.at(i))  coefs2Modified = true;
         coefs2Values.at(i) = cellValue;
      }
      else
      {
         canClose = false;
      }
   }
   

   if (!canClose)
   {
      ResetData();
      return;
   }
   
   #ifdef DEBUG_COEF_SAVE
   MessageInterface::ShowMessage
      ("ThrusterConfigDialog::SaveData() exiting\n");
   #endif
}  

//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void ThrusterCoefficientDialog::ResetData()
{
   coefs1Modified = false;
   coefs2Modified = false;
}     

