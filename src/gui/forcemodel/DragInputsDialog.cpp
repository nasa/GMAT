//$Id$
//------------------------------------------------------------------------------
//                              DragDialog
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/01/19
//
/**
 * This class allows user to edit Jacchia-Roberts or MSISE90 drag parameters.
 */
//------------------------------------------------------------------------------

#include "DragInputsDialog.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "MessageInterface.hpp"
#include "bitmaps/OpenFolder.xpm"
#include "StringUtil.hpp"          // for ToReal()
#include <wx/variant.h>

// #define DEBUG_DRAG_SAVE
//------------------------------------------------------------------------------
// Event tables and other macros for wxWindows
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(DragInputsDialog, GmatDialog)
   EVT_BUTTON(ID_BUTTON, DragInputsDialog::OnBrowse)
   EVT_TEXT(ID_TEXTCTRL, DragInputsDialog::OnTextChange)
   EVT_BUTTON(ID_BUTTON_CSSI,DragInputsDialog::OnCSSIFileSelect)
   EVT_BUTTON(ID_BUTTON_SCHATTEN,DragInputsDialog::OnSchattenFileSelect)
END_EVENT_TABLE()

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// DragInputsDialog(wxWindow *parent, DragForce *dragForce,
//                  const wxString& title = "")
//------------------------------------------------------------------------------
/**
 * Constructs DragInputsDialog object.
 *
 * @param  parent  Parent window
 * @param  dragForce  Input drag force
 * @param  title  Title of the dialog
 *
 * @note Creates the JacchiaRoberts drag dialog
 */
//------------------------------------------------------------------------------
DragInputsDialog::DragInputsDialog(wxWindow *parent, DragForce *dragForce, Real *dragBuffer,
      std::vector<std::string> * dragStringBuffer, const wxString& title)
   : GmatDialog(parent, -1, title)
{
   #ifdef DEBUG_DRAG_INPUT
   MessageInterface::ShowMessage
      ("DragInputsDialog::DragInputsDialog() entered, dragForce=<%p><%s>'%s'\n",
       dragForce, dragForce ? dragForce->GetTypeName().c_str() : "NULL",
       dragForce ? dragForce->GetName().c_str() : "NULL");
   #endif
   
   theDragForce = dragForce;
   theForceData = dragBuffer;
   theForceStringArray = dragStringBuffer;
   isTextModified = false;

   if (theForceData != NULL)
   {
      Create();
      ShowData();
   }
   else
   {
      MessageInterface::ShowMessage
         ("***  ERROR *** DragInputsDialog() input drag data is NULL\n");
   }
   
   #ifdef DEBUG_DRAG_INPUT
   MessageInterface::ShowMessage("DragInputsDialog::DragInputsDialog() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// ~DragInputsDialog()
//------------------------------------------------------------------------------
DragInputsDialog::~DragInputsDialog()
{
}

//-------------------------------
// private methods
//-------------------------------


//------------------------------------------------------------------------------
// virtual void Create()
//------------------------------------------------------------------------------
void DragInputsDialog::Create()
{
   int bsize = 3; // border size
   
   //-----------------------------------------------------------------
   // user input
   //-----------------------------------------------------------------
   
   solarFluxStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Solar Flux"),
                       wxDefaultPosition, wxDefaultSize, 0);
   solarFluxTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(150,-1), 0);
   avgSolarFluxStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Average Solar Flux"),
                       wxDefaultPosition, wxDefaultSize, 0);
   avgSolarFluxTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                      wxDefaultPosition, wxSize(150,-1), 0);
   geomagneticIndexStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Geomagnetic Index (Kp)"),
                       wxDefaultPosition, wxDefaultSize, 0);
   historicIndexStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Historic/Near Term"),
                       wxDefaultPosition, wxDefaultSize, 0);
   predictedIndexStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Long-Term Predict"),
                       wxDefaultPosition, wxDefaultSize, 0);
   cssiSpaceWeatherStaticText =
      new wxStaticText(this, ID_TEXT, wxT("CSSISpaceWeatherFile"),
                       wxDefaultPosition, wxDefaultSize, 0);
   schattenFileStaticText =
      new wxStaticText(this, ID_TEXT, wxT("SchattenFile"),
                       wxDefaultPosition, wxDefaultSize, 0);
   schattenErrorStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Schatten Error Model"),
                       wxDefaultPosition, wxDefaultSize, 0);
   schattenTimingStaticText =
      new wxStaticText(this, ID_TEXT, wxT("Schatten Timing Model"),
                       wxDefaultPosition, wxDefaultSize, 0);
   geomagneticIndexTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL, wxT(""),
                     wxDefaultPosition, wxSize(150,-1), 0);
  
   cssiFileTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_CSSI, wxT(""),
                     wxDefaultPosition, wxSize(150,-1), wxTE_READONLY);
   schattenFileTextCtrl =
      new wxTextCtrl(this, ID_TEXTCTRL_SCHATTEN, wxT(""),
                     wxDefaultPosition, wxSize(150,-1), wxTE_READONLY);
  
   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);

   // define dropdowns data for file inputs
   historicInputsArray.Add("ConstantFluxAndGeoMag");
   historicInputsArray.Add("CSSISpaceWeatherFile");
   predictedInputsArray.Add("ConstantFluxAndGeoMag");

   // Turned off for R2015a
//   predictedInputsArray.Add("CSSISpaceWeatherFile");
   predictedInputsArray.Add("SchattenFile");
   schattentimingArray.Add("NominalCycle");
   schattentimingArray.Add("EarlyCycle");
   schattentimingArray.Add("LateCycle");
   schattenErrorArray.Add("Nominal");
   schattenErrorArray.Add("PlusTwoSigma");
   schattenErrorArray.Add("MinusTwoSigma");

   #if __WXMAC__
     int buttonWidth = 40;
   #else
     int buttonWidth = 25;
   #endif
       
   //-----------------------------------------------------------------
   // add to sizer
   //-----------------------------------------------------------------

   // add comobo box to hold model types
   predictedFileComboBox = new wxComboBox( this, ID_MODEL, predictedInputsArray[0], wxDefaultPosition,
                                          wxDefaultSize, predictedInputsArray, wxCB_DROPDOWN|wxCB_READONLY);

   historicFileComboBox = new wxComboBox( this, ID_MODEL, historicInputsArray[0], wxDefaultPosition,
                                          wxDefaultSize, historicInputsArray, wxCB_DROPDOWN|wxCB_READONLY);
   // Model Selection 
   wxFlexGridSizer *modelFileInputFlexGridSizer = new wxFlexGridSizer(2, 1, 0);
   modelFileInputFlexGridSizer->Add(historicIndexStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   modelFileInputFlexGridSizer->Add(historicFileComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   modelFileInputFlexGridSizer->Add(predictedIndexStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   modelFileInputFlexGridSizer->Add(predictedFileComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // Data and File selection
   wxFlexGridSizer * dataFilesSizer = new wxFlexGridSizer(2, 3, 0, 0);
   schattenErrorComboBox = new wxComboBox( this, ID_ERROR_SCHATTEN, schattenErrorArray[0], wxDefaultPosition,
                                          wxDefaultSize, schattenErrorArray, wxCB_DROPDOWN|wxCB_READONLY);
   
   wxSize ecbSize(schattenErrorComboBox->GetSize());
   schattenTimingComboBox = new wxComboBox(this, ID_TIMING_SCHATTEN, schattentimingArray[0], wxDefaultPosition,
                                           ecbSize/*wxDefaultSize*/, schattentimingArray, wxCB_DROPDOWN | wxCB_READONLY);
   cssiFileButton = new wxBitmapButton(this, ID_BUTTON_CSSI, openBitmap, wxDefaultPosition, wxSize(buttonWidth, -1));
   schattenFileButton = new wxBitmapButton(this, ID_BUTTON_SCHATTEN, openBitmap, wxDefaultPosition, wxSize(buttonWidth, -1));
   dataFilesSizer->Add(cssiSpaceWeatherStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   dataFilesSizer->Add(cssiFileTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   dataFilesSizer->Add(cssiFileButton, 0, wxALIGN_CENTER|wxALL, bsize);
   dataFilesSizer->Add(schattenFileStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   dataFilesSizer->Add(schattenFileTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   dataFilesSizer->Add(schattenFileButton, 0, wxALIGN_CENTER|wxALL, bsize);

   // constant value stuff
   wxFlexGridSizer *userConstantInputFlexGridSizer = new wxFlexGridSizer(10, 2, 0, 0);
   userConstantInputFlexGridSizer->Add(solarFluxStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   userConstantInputFlexGridSizer->Add(solarFluxTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   userConstantInputFlexGridSizer->Add(avgSolarFluxStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   userConstantInputFlexGridSizer->Add(avgSolarFluxTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   userConstantInputFlexGridSizer->Add(geomagneticIndexStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   userConstantInputFlexGridSizer->Add(geomagneticIndexTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize);
   userConstantInputFlexGridSizer->Add(schattenErrorStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   userConstantInputFlexGridSizer->Add(schattenErrorComboBox, 0, wxALIGN_LEFT|wxALL, bsize);
   userConstantInputFlexGridSizer->Add(schattenTimingStaticText, 0, wxALIGN_LEFT|wxALL, bsize);
   userConstantInputFlexGridSizer->Add(schattenTimingComboBox, 0, wxALIGN_LEFT|wxALL, bsize);

   // static boxes to place the data within the main page
   GmatStaticBoxSizer *fileModelInputGroup = new GmatStaticBoxSizer(wxVERTICAL, this, "Model Selection");
   GmatStaticBoxSizer *dataFileInputGroup = new GmatStaticBoxSizer(wxVERTICAL, this, "Files");
   GmatStaticBoxSizer *constantInputGroup = new GmatStaticBoxSizer(wxVERTICAL, this,"Model Configuration");

   // add combo boxes, file input, and constants input fields
   fileModelInputGroup->Add(modelFileInputFlexGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   dataFileInputGroup->Add(dataFilesSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   constantInputGroup->Add(userConstantInputFlexGridSizer, 0, wxALIGN_LEFT|wxALL, bsize);
   
   // add all data to the main page 
   wxBoxSizer *mainPageSizer = new wxBoxSizer(wxVERTICAL);
   mainPageSizer->Add(fileModelInputGroup, 0,  wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, bsize);
   mainPageSizer->Add(dataFileInputGroup, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   mainPageSizer->Add(constantInputGroup, 0, wxALIGN_LEFT|wxGROW|wxALL, bsize);
   theMiddleSizer->Add(mainPageSizer, 0, wxALIGN_CENTER|wxALL, bsize);
}


//------------------------------------------------------------------------------
// virtual void LoadData()
//------------------------------------------------------------------------------
void DragInputsDialog::LoadData()
{
   #ifdef DEBUG_DRAG_LOAD
   MessageInterface::ShowMessage("DragInputsDialog::LoadData() entered\n");
   #endif
 
   // set the model type
   predictedFileComboBox->SetValue(wxVariant((*theForceStringArray)[0]));
   historicFileComboBox->SetValue(wxVariant((*theForceStringArray)[1]));

   //set the values as saved in the force model
   solarFluxTextCtrl->SetValue(wxVariant(theForceData[0]));
   avgSolarFluxTextCtrl->SetValue(wxVariant(theForceData[1]));
   geomagneticIndexTextCtrl->SetValue(wxVariant(theForceData[2]));
   cssiFileTextCtrl->SetValue(wxVariant((*theForceStringArray)[2]));
   schattenFileTextCtrl->SetValue(wxVariant((*theForceStringArray)[3]));
   schattenErrorComboBox->SetValue(wxVariant((*theForceStringArray)[4]));
   schattenTimingComboBox->SetValue(wxVariant((*theForceStringArray)[5]));

   
   #ifdef DEBUG_DRAG_LOAD
   MessageInterface::ShowMessage("DragInputsDialog::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void SaveData()
//------------------------------------------------------------------------------
void DragInputsDialog::SaveData()
{
   #ifdef DEBUG_DRAG_SAVE
   MessageInterface::ShowMessage("DragInputsDialog::SaveData() entered\n");
   #endif
   
   canClose = true;
   Real flux, avgFlux, magIndex;
   
   //-----------------------------------------------------------------
   // check values from text field
   //-----------------------------------------------------------------
   if (isTextModified)
   {
      std::string str;
      
      str = solarFluxTextCtrl->GetValue();
      CheckReal(flux, str, "Solar Flux (F10.7)", "Real Number >= 0.0", false, true, true , true);
      
      str = avgSolarFluxTextCtrl->GetValue();
      CheckReal(avgFlux, str, "Average Solar Flux (F10.7A)", "Real Number >= 0.0", false, true, true, true);
      
      str = geomagneticIndexTextCtrl->GetValue();
      bool isValid = CheckReal(magIndex, str, "Geomagnetic Index (Kp)", "0.0 <= Real Number <= 9.0");
      if (isValid)
         isValid = CheckRealRange(str, magIndex, "Geomagnetic Index", 0.0, 9.0, true, true, true, true);

      if (!isValid)
         canClose = false;

      std::string fileToCheck;

      // Check the flux files
      if (strcmp(historicFileComboBox->GetStringSelection().WX_TO_C_STRING,
                 "ConstantFluxAndGeoMag") != 0)
      {
         fileToCheck = cssiFileTextCtrl->GetValue().WX_TO_C_STRING;

         if (theDragForce->CheckFluxFile(fileToCheck, true) == "")
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "The flux file %s does not contain valid historic data",
                     fileToCheck.c_str());
            canClose = false;
         }
      }
      
      if (strcmp(predictedFileComboBox->GetStringSelection().WX_TO_C_STRING,
                 "ConstantFluxAndGeoMag") != 0)
      {
         if (strcmp(predictedFileComboBox->GetStringSelection().WX_TO_C_STRING,
                    "CSSISpaceWeatherFile") == 0)
         {
            MessageInterface::PopupMessage
               (Gmat::ERROR_, "CSSI predict data is not supported in GMAT");
            canClose = false;

//            fileToCheck = cssiFileTextCtrl->GetValue().WX_TO_C_STRING;
//            if (theDragForce->CheckFluxFile(fileToCheck, false) == "")
//            {
//               MessageInterface::PopupMessage
//                  (Gmat::ERROR_, "The flux file %s does not contain valid predict data",
//                        fileToCheck.c_str());
//               canClose = false;
//            }
         }
         else
         {
            fileToCheck = schattenFileTextCtrl->GetValue().WX_TO_C_STRING;
            if (theDragForce->CheckFluxFile(fileToCheck, false) == "")
            {
               MessageInterface::PopupMessage
                  (Gmat::ERROR_, "The flux file %s does not contain valid predict data",
                        fileToCheck.c_str());
               canClose = false;
            }
         }
      }

      if (!canClose)
         return;
   }
   
   //-----------------------------------------------------------------
   // save values to base, base code should do the range checking
   //-----------------------------------------------------------------
   try
   {
      std::string cssifileName = cssiFileTextCtrl->GetValue().WX_TO_C_STRING;
      std::string schattenfileName = schattenFileTextCtrl->GetValue().WX_TO_C_STRING;
      std::string schattenError = schattenErrorComboBox->GetValue().WX_TO_C_STRING;
      std::string schattenTiming = schattenTimingComboBox->GetValue().WX_TO_C_STRING;
      (*theForceStringArray)[0] = predictedFileComboBox->GetStringSelection().WX_TO_C_STRING;
      (*theForceStringArray)[1] = historicFileComboBox->GetStringSelection().WX_TO_C_STRING;
      
     if (cssifileName == "")
     {
        MessageInterface::PopupMessage
           (Gmat::ERROR_, "The CSSI flux file name is empty, please enter file name");
        canClose = false;
        return;
     }

     // save off the data
     (*theForceStringArray)[2] = cssifileName;

     if (schattenfileName=="")
     {
          MessageInterface::PopupMessage
             (Gmat::ERROR_, "The Schatten flux file name is empty, please enter file name");
          canClose = false;
          return;
     }

     // save the data
     (*theForceStringArray)[3] = schattenfileName;
     (*theForceStringArray)[4] = schattenError; 
     (*theForceStringArray)[5] = schattenTiming;

    #ifdef DEBUG_DRAG_SAVE
       MessageInterface::ShowMessage
      ("theForcesStringArray has size %d:\n", theForceStringArray->size());

      for (UnsignedInt q = 0; q < theForceStringArray->size(); ++q)
         MessageInterface::ShowMessage("   %d:  %s\n", q, theForceStringArray->at(q).c_str());
    #endif


    #ifdef DEBUG_DRAG_SAVE
       MessageInterface::ShowMessage
          ("   ==> Saved filename%s\n", fileNameTextCtrl->GetValue().c_str());
    #endif

    if (isTextModified)
    {
       theForceData[0] = flux;
       theForceData[1] = avgFlux;
       theForceData[2] = magIndex;
        
        isTextModified = false;
     }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
   }
  

   #ifdef DEBUG_DRAG_SAVE
   MessageInterface::ShowMessage("DragInputsDialog::SaveData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// virtual void ResetData()
//------------------------------------------------------------------------------
void DragInputsDialog::ResetData()
{
}


//------------------------------------------------------------------------------
// void OnTextChange()
//------------------------------------------------------------------------------
void DragInputsDialog::OnTextChange(wxCommandEvent &event)
{
   if (((wxTextCtrl*)event.GetEventObject())->IsModified())
   {
      EnableUpdate(true);
      isTextModified = true;
   }
}


//------------------------------------------------------------------------------
// void OnBrowse()
//------------------------------------------------------------------------------
void DragInputsDialog::OnBrowse(wxCommandEvent &event)
{
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;
        
      filename = dialog.GetPath().c_str();
   }
   
   EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnCSSIFileSelect()
//------------------------------------------------------------------------------
void DragInputsDialog::OnCSSIFileSelect(wxCommandEvent &event)
{
    wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename;
        filename = dialog.GetPath().c_str();
        cssiFileTextCtrl->SetValue(filename);
        isTextModified = true;
    }

}
//------------------------------------------------------------------------------
// void OnSchattenFileSelect()
//------------------------------------------------------------------------------
void DragInputsDialog::OnSchattenFileSelect(wxCommandEvent &event)
{
    wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
    if (dialog.ShowModal() == wxID_OK)
    {
        wxString filename;
        filename = dialog.GetPath().c_str();
        schattenFileTextCtrl->SetValue(filename);
        isTextModified = true;
    }

}
