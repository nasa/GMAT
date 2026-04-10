//$Id$
//------------------------------------------------------------------------------
//                            CelestialBodyVisualizationPanel
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
// Author: Linda Jun
// Created: 2015.02.02
//
/**
 * This is the panel for the Visualization tab on the notebook on the CelestialBody
 * Panel.
 *
 */
//------------------------------------------------------------------------------

#include "CelestialBodyVisualizationPanel.hpp"
#include "GmatColorPanel.hpp"
#include "MessageInterface.hpp"
#include "bitmaps/OpenFolder.xpm"
#include <wx/config.h>
#include <fstream>
#include "StringUtil.hpp"           // for GmatStringUtil::

//#define DEBUG_CREATE
//#define DEBUG_LOAD_DATA
//#define DEBUG_SAVE_DATA


//------------------------------------------------------------------------------
// event tables for wxMac/Widgets
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(CelestialBodyVisualizationPanel, wxPanel)
   EVT_BUTTON(ID_BUTTON_BROWSE, CelestialBodyVisualizationPanel::OnBrowseButton)
   EVT_TEXT(ID_TEXT_CTRL_TEXTURE_FILE, CelestialBodyVisualizationPanel::OnTextureFileTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_3D_MODEL_FILE, CelestialBodyVisualizationPanel::OnView3dModelFileTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_3D_MODEL_OFFSET, CelestialBodyVisualizationPanel::OnView3dModelOffsetTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_3D_MODEL_ROT, CelestialBodyVisualizationPanel::OnView3dModelRotTextCtrlChange)
   EVT_TEXT(ID_TEXT_CTRL_3D_MODEL_SCALE, CelestialBodyVisualizationPanel::OnView3dModelScaleTextCtrlChange)
END_EVENT_TABLE()

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CelestialBodyVisualizationPanel(GmatPanel *cbPanel, wxWindow *parent,
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
CelestialBodyVisualizationPanel::CelestialBodyVisualizationPanel(GmatPanel *cbPanel, 
                              wxWindow *parent, CelestialBody *body) :
   wxPanel        (parent),
   dataChanged    (false),
   canClose       (true),
   theBody        (body),
   textureFileChanged (false),
   view3dModelFileChanged (false),
   view3dModelOffsetChanged (false),
   view3dModelRotChanged (false),
   view3dModelScaleChanged (false),
   theCBPanel     (cbPanel)
{
   guiManager     = GuiItemManager::GetInstance();
   
   Create();
}

//------------------------------------------------------------------------------
// ~CelestialBodyVisualizationPanel()
//------------------------------------------------------------------------------
/**
 * Destroys the panel (destructor).
 *
 */
//------------------------------------------------------------------------------
CelestialBodyVisualizationPanel::~CelestialBodyVisualizationPanel()
{
}

//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data from the body object to the panel widgets.
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyVisualizationPanel::LoadData()
{
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage
      ("CelestialBodyVisualizationPanel::LoadData() entered, body = '%s'\n",
       theBody->GetName().c_str());
   #endif
   try
   {
      std::string strVal;
      strVal = theBody->GetStringParameter(theBody->GetParameterID("TextureMapFileName"));
      textureFileTextCtrl->SetValue(strVal.c_str());
      
      strVal = theBody->GetStringParameter(theBody->GetParameterID("3DModelFile"));
      view3dModelTextCtrl->SetValue(strVal.c_str());
      
      Real realVal;
      wxString wxstrVal;
      realVal = theBody->GetRealParameter(theBody->GetParameterID("3DModelOffsetX"));
      wxstrVal = guiManager->ToWxString(realVal);
      view3dModelOffsetXTextCtrl->SetValue(wxstrVal);
      
      realVal = theBody->GetRealParameter(theBody->GetParameterID("3DModelOffsetY"));
      wxstrVal = guiManager->ToWxString(realVal);
      view3dModelOffsetYTextCtrl->SetValue(wxstrVal);
      
      realVal = theBody->GetRealParameter(theBody->GetParameterID("3DModelOffsetZ"));
      wxstrVal = guiManager->ToWxString(realVal);
      view3dModelOffsetZTextCtrl->SetValue(wxstrVal);
      
      realVal = theBody->GetRealParameter(theBody->GetParameterID("3DModelRotationX"));
      wxstrVal = guiManager->ToWxString(realVal);
      view3dModelRotXTextCtrl->SetValue(wxstrVal);
      
      realVal = theBody->GetRealParameter(theBody->GetParameterID("3DModelRotationY"));
      wxstrVal = guiManager->ToWxString(realVal);
      view3dModelRotYTextCtrl->SetValue(wxstrVal);
      
      realVal = theBody->GetRealParameter(theBody->GetParameterID("3DModelRotationZ"));
      wxstrVal = guiManager->ToWxString(realVal);
      view3dModelRotZTextCtrl->SetValue(wxstrVal);
      
      realVal = theBody->GetRealParameter(theBody->GetParameterID("3DModelScale"));
      wxstrVal = guiManager->ToWxString(realVal);
      view3dModelScaleTextCtrl->SetValue(wxstrVal);
      
      ResetChangeFlags();
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   #ifdef DEBUG_LOAD_DATA
   MessageInterface::ShowMessage
      ("CelestialBodyVisualizationPanel::LoadData() leaving, body = '%s'\n",
       theBody->GetName().c_str());
   #endif
}

//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Saves the data from the panel widgets to the body object.
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyVisualizationPanel::SaveData()
{
   #ifdef DEBUG_SAVE_DATA
   MessageInterface::ShowMessage
      ("CelestialBodyVisualizationPanel::SaveData() entered, dataChanged = %s\n",
       (dataChanged? "true" : "false"));
   MessageInterface::ShowMessage
      ("    textureFileChanged       = %s\n", (textureFileChanged? "true" : "false"));
   MessageInterface::ShowMessage
      ("    view3dModelFileChanged   = %s\n", (view3dModelFileChanged? "true" : "false"));
   MessageInterface::ShowMessage
      ("    view3dModelOffsetChanged = %s\n", (view3dModelOffsetChanged? "true" : "false"));
   MessageInterface::ShowMessage
      ("    view3dModelRotChanged    = %s\n", (view3dModelRotChanged? "true" : "false"));
   MessageInterface::ShowMessage
      ("    view3dModelScaleChanged  = %s\n", (view3dModelScaleChanged? "true" : "false"));
   #endif
   
   bool realsOK   = true;
   bool stringsOK = true;
   std::string strval;
   std::string textureFile;
   std::string modelFile;
   Real realval;
   Real modelOffsetX, modelOffsetY, modelOffsetZ;
   Real modelRotX, modelRotY, modelRotZ;
   Real modelScale;
   
   // don't do anything if no data has been changed.
   // note that dataChanged will be true if the user modified any combo box or
   // text ctrl, whether or not he/she actually changed the value
   if (!dataChanged) return;
   
   canClose = true;
   
   if (view3dModelOffsetChanged)
   {
      // Check if view3dModelOffsetXTextCtrl modified
      if (view3dModelOffsetXTextCtrl->IsModified())
      {
         strval = view3dModelOffsetXTextCtrl->GetValue();
         if (!theBody->IsParameterValid("3DModelOffsetX", strval))
         {
            std::string errmsg = theBody->GetLastErrorMessage();
            realsOK = false;
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
         }
         else
         {
            GmatStringUtil::ToReal(strval, &realval, false, false);
            modelOffsetX = realval;
         }
      }
      
      // Check if view3dModelOffsetYTextCtrl modified
      if (view3dModelOffsetYTextCtrl->IsModified())
      {
         strval = view3dModelOffsetYTextCtrl->GetValue();
         if (!theBody->IsParameterValid("3DModelOffsetY", strval))
         {
            std::string errmsg = theBody->GetLastErrorMessage();
            realsOK = false;
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
         }
         else
         {
            GmatStringUtil::ToReal(strval, &realval, false, false);
            modelOffsetY = realval;
         }
      }
      
      // Check if view3dModelOffsetZTextCtrl modified
      if (view3dModelOffsetZTextCtrl->IsModified())
      {
         strval = view3dModelOffsetZTextCtrl->GetValue();
         if (!theBody->IsParameterValid("3DModelOffsetZ", strval))
         {
            std::string errmsg = theBody->GetLastErrorMessage();
            realsOK = false;
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
         }
         else
         {
            GmatStringUtil::ToReal(strval, &realval, false, false);
            modelOffsetZ = realval;
         }
      }
   }
   
   if (view3dModelRotChanged)
   {
      // Check if view3dModelRotXTextCtrl modified
      if (view3dModelRotXTextCtrl->IsModified())
      {
         strval = view3dModelRotXTextCtrl->GetValue();
         if (!theBody->IsParameterValid("3DModelRotationX", strval))
         {
            std::string errmsg = theBody->GetLastErrorMessage();
            realsOK = false;
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
         }
         else
         {
            GmatStringUtil::ToReal(strval, &realval, false, false);
            modelRotX = realval;
         }
      }
      
      // Check if view3dModelRotYTextCtrl modified
      if (view3dModelRotYTextCtrl->IsModified())
      {
         strval = view3dModelRotYTextCtrl->GetValue();
         if (!theBody->IsParameterValid("3DModelRotationY", strval))
         {
            std::string errmsg = theBody->GetLastErrorMessage();
            realsOK = false;
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
         }
         else
         {
            GmatStringUtil::ToReal(strval, &realval, false, false);
            modelRotY = realval;
         }
      }
      
      // Check if view3dModelRotZTextCtrl modified
      if (view3dModelRotZTextCtrl->IsModified())
      {
         strval = view3dModelRotZTextCtrl->GetValue();
         if (!theBody->IsParameterValid("3DModelRotationZ", strval))
         {
            std::string errmsg = theBody->GetLastErrorMessage();
            realsOK = false;
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
         }
         else
         {
            GmatStringUtil::ToReal(strval, &realval, false, false);
            modelRotZ = realval;
         }
      }      
   }
   
   if (view3dModelScaleChanged)
   {
      strval = view3dModelScaleTextCtrl->GetValue();
      if (!theBody->IsParameterValid("3DModelScale", strval))
      {
         std::string errmsg = theBody->GetLastErrorMessage();
         realsOK = false;
         MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
      }
      else
      {
         GmatStringUtil::ToReal(strval, &realval, false, false);
         modelScale = realval;
      }
   }
      
   if (textureFileChanged)
   {
      strval = textureFileTextCtrl->GetValue();
      textureFile = strval;
      #ifdef DEBUG_SAVE_DATA
      MessageInterface::ShowMessage
         ("textureFileChanged is true : %s\n", strval.c_str());
      #endif
      
      // Checking for texture file existance will not work since valid
      // texture file can be just file name without full path such as Sun.jpg.
      // Usually validation of new value is done in the base code when setting
      // the value and we don't want to partially set the value when any of new
      // value fails validation. Since cloned body is not used here for setting
      // texture file name, IsParameterValid() is called before setting actual
      // valid value to the body. (Fix for GMT-4189 LOJ: 2014.10.31)
      if (!theBody->IsParameterValid("TextureMapFileName", textureFile))
      {
         std::string errmsg = theBody->GetLastErrorMessage();
         stringsOK = false;
         MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
      }
   }
   
   if (view3dModelFileChanged)
   {
      strval = view3dModelTextCtrl->GetValue();
      modelFile = strval;
      #ifdef DEBUG_SAVE_DATA
      MessageInterface::ShowMessage
         ("view3dModelFileChanged is true : %s\n", strval.c_str());
      #endif
      
      if (!theBody->IsParameterValid("3DModelFile", modelFile))
      {
         std::string errmsg = theBody->GetLastErrorMessage();
         stringsOK = false;
         MessageInterface::PopupMessage(Gmat::ERROR_, errmsg);
      }
   }
   
   if (realsOK && stringsOK)
   {
      #ifdef DEBUG_SAVE_DATA
      MessageInterface::ShowMessage("Reals and Strings are OK - setting them\n");
      MessageInterface::ShowMessage("in Properties panel, body pointer is %p\n",
                                    theBody);
      #endif
      
      if (view3dModelOffsetChanged)
      {
         if (view3dModelOffsetXTextCtrl->IsModified())
            theBody->SetRealParameter(theBody->GetParameterID("3DModelOffsetX"), modelOffsetX);
         if (view3dModelOffsetYTextCtrl->IsModified())
            theBody->SetRealParameter(theBody->GetParameterID("3DModelOffsetY"), modelOffsetY);
         if (view3dModelOffsetZTextCtrl->IsModified())
            theBody->SetRealParameter(theBody->GetParameterID("3DModelOffsetZ"), modelOffsetZ);
      }
      
      if (view3dModelRotChanged)
      {
         if (view3dModelRotXTextCtrl->IsModified())
            theBody->SetRealParameter(theBody->GetParameterID("3DModelRotationX"), modelRotX);
         if (view3dModelRotYTextCtrl->IsModified())
            theBody->SetRealParameter(theBody->GetParameterID("3DModelRotationY"), modelRotY);
         if (view3dModelRotZTextCtrl->IsModified())
            theBody->SetRealParameter(theBody->GetParameterID("3DModelRotationZ"), modelRotZ);
      }
      
      if (view3dModelScaleChanged)
         theBody->SetRealParameter(theBody->GetParameterID("3DModelScale"), modelScale);
      
      if (textureFileChanged)
      {
         Integer textureId = theBody->GetParameterID("TextureMapFileName");
         theBody->SetStringParameter(textureId, textureFile);
         // If texture map file is blank, display default one
         if (textureFile == "")
         {
            wxString wxDefMap = STD_TO_WX_STRING(theBody->GetStringParameter(textureId).c_str());
            textureFileTextCtrl->SetValue(wxDefMap);
         }
      }
      
      if (view3dModelFileChanged)
      {
         Integer id = theBody->GetParameterID("3DModelFile");
         theBody->SetStringParameter(id, modelFile);
      }
      
      dataChanged = false;
      ResetChangeFlags(true);
   }
   else
   {
      canClose = false;
   }
   
   #ifdef DEBUG_SAVE_DATA
   MessageInterface::ShowMessage
      ("CelestialBodyVisualizationPanel::SaveData()leaving, canClose = %s\n",
       (canClose? "true" : "false"));
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
void CelestialBodyVisualizationPanel::Create()
{
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage
      ("CelestialBodyVisualizationPanel::Create() entered, theBody=<%p>'%s'\n",
       theBody, theBody->GetName().c_str());
   #endif
   
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
   pConfig->SetPath(wxT("/Celestial Body Properties"));
   
   // texture file
   wxStaticText *textureStaticText = new wxStaticText(this, ID_TEXT, wxString("Te" GUI_ACCEL_KEY "xture File"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   textureFileTextCtrl   = new wxTextCtrl(this, ID_TEXT_CTRL_TEXTURE_FILE, wxT(""),
                       wxDefaultPosition, wxSize(300,-1), 0);
   textureFileTextCtrl->SetToolTip(pConfig->Read(_T("TextureMapFileHint")));
   textureFileBrowseButton = new wxBitmapButton(this, ID_BUTTON_BROWSE, 
                       openBitmap, wxDefaultPosition,
                       wxSize(buttonWidth, -1));
   textureFileBrowseButton->SetToolTip(pConfig->Read(_T("BrowseTextureMapFileHint"), "Browse for file"));
   
   // view 3D model file
   wxStaticText *view3dModelStaticText = new wxStaticText(this, ID_TEXT, wxString("3" GUI_ACCEL_KEY "D Model File"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   view3dModelTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL_3D_MODEL_FILE, wxT(""),
                       wxDefaultPosition, wxSize(300,-1), 0);
   view3dModelTextCtrl->SetToolTip(pConfig->Read(_T("3DModelFileHint")));
   view3dModelBrowseButton = new wxBitmapButton(this, ID_BUTTON_BROWSE, 
                       openBitmap, wxDefaultPosition,
                       wxSize(buttonWidth, -1));
   view3dModelBrowseButton->SetToolTip(pConfig->Read(_T("Browse3DModelFileHint"), "Browse for file"));
   
   // view 3D model offset
   wxStaticText *view3dModelOffsetXStaticText = new wxStaticText(this, ID_TEXT, wxString("3D Model " GUI_ACCEL_KEY "Offset X"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   view3dModelOffsetXTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL_3D_MODEL_OFFSET, wxT(""),
                       wxDefaultPosition, wxSize(300,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   view3dModelOffsetXTextCtrl->SetToolTip(pConfig->Read("3DModelOffsetXHint"));
   wxStaticText *view3dModelOffsetYStaticText = new wxStaticText(this, ID_TEXT, wxString("3D Model " GUI_ACCEL_KEY "Offset Y"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   view3dModelOffsetYTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL_3D_MODEL_OFFSET, wxT(""),
                       wxDefaultPosition, wxSize(100,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   view3dModelOffsetYTextCtrl->SetToolTip(pConfig->Read("3DModelOffsetYHint"));
   wxStaticText *view3dModelOffsetZStaticText = new wxStaticText(this, ID_TEXT, wxString("3D Model " GUI_ACCEL_KEY "Offset Z"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   view3dModelOffsetZTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL_3D_MODEL_OFFSET, wxT(""),
                       wxDefaultPosition, wxSize(100,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   view3dModelOffsetZTextCtrl->SetToolTip(pConfig->Read("3DModelOffsetZHint"));
   
   // view 3D model rotation
   wxStaticText *view3dModelRotXStaticText = new wxStaticText(this, ID_TEXT, wxString("3D Model " GUI_ACCEL_KEY "Rotation X"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   view3dModelRotXTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL_3D_MODEL_ROT, wxT(""),
                       wxDefaultPosition, wxSize(300,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   view3dModelRotXTextCtrl->SetToolTip(pConfig->Read("3DModelRotationXHint"));
   wxStaticText *view3dModelRotYStaticText = new wxStaticText(this, ID_TEXT, wxString("3D Model " GUI_ACCEL_KEY "Rotation Y"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   view3dModelRotYTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL_3D_MODEL_ROT, wxT(""),
                       wxDefaultPosition, wxSize(300,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   view3dModelRotYTextCtrl->SetToolTip(pConfig->Read("3DModelRotationYHint"));
   wxStaticText *view3dModelRotZStaticText = new wxStaticText(this, ID_TEXT, wxString("3D Model " GUI_ACCEL_KEY "Rotation Z"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   view3dModelRotZTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL_3D_MODEL_ROT, wxT(""),
                       wxDefaultPosition, wxSize(300,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   view3dModelRotZTextCtrl->SetToolTip(pConfig->Read("3DModelRotationZHint"));
   wxStaticText *view3dModelRotXUnitStaticText = new wxStaticText(this, ID_TEXT, wxT("deg"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *view3dModelRotYUnitStaticText = new wxStaticText(this, ID_TEXT, wxT("deg"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   wxStaticText *view3dModelRotZUnitStaticText = new wxStaticText(this, ID_TEXT, wxT("deg"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   
   // view 3D model scale
   wxStaticText *view3dModelScaleStaticText = new wxStaticText(this, ID_TEXT, wxString("3D Model S" GUI_ACCEL_KEY "cale"),
                       wxDefaultPosition, wxSize(-1,-1), 0);
   view3dModelScaleTextCtrl = new wxTextCtrl(this, ID_TEXT_CTRL_3D_MODEL_SCALE, wxT(""),
                       wxDefaultPosition, wxSize(300,-1), 0, wxTextValidator(wxGMAT_FILTER_NUMERIC));
   view3dModelTextCtrl->SetToolTip(pConfig->Read(_T("3DModelScaleHint")));
   
   // Add to sizers
   wxFlexGridSizer *cbVisGridSizer = new wxFlexGridSizer(3,0,0);
   // We don't want second column to extend so commented out
   cbVisGridSizer->AddGrowableCol(1);
   
   cbVisGridSizer->Add(textureStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbVisGridSizer->Add(textureFileTextCtrl,0, wxALIGN_LEFT|wxGROW|wxALL, bSize);
   cbVisGridSizer->Add(textureFileBrowseButton,0, wxALIGN_CENTRE|wxALL, bSize);
   
   cbVisGridSizer->Add(view3dModelStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelTextCtrl,0, wxALIGN_LEFT|wxGROW|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelBrowseButton,0, wxALIGN_CENTRE|wxALL, bSize);
   
   cbVisGridSizer->Add(view3dModelOffsetXStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelOffsetXTextCtrl,0, wxALIGN_LEFT|wxGROW|wxALL, bSize);
   cbVisGridSizer->Add(20, 20);
   
   cbVisGridSizer->Add(view3dModelOffsetYStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelOffsetYTextCtrl,0, wxALIGN_LEFT|wxGROW|wxALL, bSize);
   cbVisGridSizer->Add(20, 20);
   
   cbVisGridSizer->Add(view3dModelOffsetZStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelOffsetZTextCtrl,0, wxALIGN_LEFT|wxGROW|wxALL, bSize);
   cbVisGridSizer->Add(20, 20);
   
   cbVisGridSizer->Add(view3dModelRotXStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelRotXTextCtrl,0, wxALIGN_LEFT|wxGROW|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelRotXUnitStaticText, 0, wxALIGN_LEFT|wxALL, bSize);
   
   cbVisGridSizer->Add(view3dModelRotYStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelRotYTextCtrl,0, wxALIGN_LEFT|wxGROW|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelRotYUnitStaticText, 0, wxALIGN_LEFT|wxALL, bSize);
   
   cbVisGridSizer->Add(view3dModelRotZStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelRotZTextCtrl,0, wxALIGN_LEFT|wxGROW|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelRotZUnitStaticText, 0, wxALIGN_LEFT|wxALL, bSize);
   
   cbVisGridSizer->Add(view3dModelScaleStaticText,0, wxALIGN_LEFT|wxALL, bSize);
   cbVisGridSizer->Add(view3dModelScaleTextCtrl,0, wxALIGN_LEFT|wxGROW|wxALL, bSize);
   cbVisGridSizer->Add(20, 20);
   
   GmatStaticBoxSizer *VisSizer = new GmatStaticBoxSizer(wxVERTICAL, this, "Visualization Data");
   // We don't want second column to extend so commented out
   VisSizer->Add(cbVisGridSizer, 0, wxALIGN_CENTER, bSize); 
   
   pageSizer = new GmatStaticBoxSizer(wxVERTICAL, this, "");
   
   // Create color panel
   GmatColorPanel *colorPanel = new GmatColorPanel(this, theCBPanel, theBody);
   
   pageSizer->Add(VisSizer, 0, wxALIGN_CENTER|wxGROW, bSize); 
   pageSizer->Add(colorPanel, 0, wxALIGN_CENTER|wxGROW, bSize); 
   
   this->SetAutoLayout(true);
   this->SetSizer(pageSizer);
   pageSizer->Fit(this);
   
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("CelestialBodyVisualizationPanel::Create() leaving\n");
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
void CelestialBodyVisualizationPanel::ResetChangeFlags(bool discardMods)
{
   textureFileChanged = false;
   view3dModelFileChanged = false;
   view3dModelOffsetChanged = false;
   view3dModelRotChanged = false;
   view3dModelScaleChanged = false;
   
   if (discardMods)
   {
      textureFileTextCtrl->DiscardEdits();
      view3dModelTextCtrl->DiscardEdits();
      view3dModelOffsetXTextCtrl->DiscardEdits();
      view3dModelOffsetYTextCtrl->DiscardEdits();
      view3dModelOffsetZTextCtrl->DiscardEdits();
      view3dModelRotXTextCtrl->DiscardEdits();
      view3dModelRotYTextCtrl->DiscardEdits();
      view3dModelRotZTextCtrl->DiscardEdits();
      view3dModelScaleTextCtrl->DiscardEdits();
   }
   dataChanged = false;
}

//------------------------------------------------------------------------------
//Event Handling
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void OnTextureFileTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the texture filename
 * text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyVisualizationPanel::OnTextureFileTextCtrlChange(wxCommandEvent &event)
{
   if (textureFileTextCtrl->IsModified())
   {
      textureFileChanged  = true;
      dataChanged     = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnView3dModelFileTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the 3D model file text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyVisualizationPanel::OnView3dModelFileTextCtrlChange(wxCommandEvent &event)
{
   if (view3dModelTextCtrl->IsModified())
   {
      view3dModelFileChanged = true;
      dataChanged = true;
      theCBPanel->EnableUpdate(true);
   }
}

//------------------------------------------------------------------------------
// void OnView3dModelOffsetTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the 3D model offset text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyVisualizationPanel::OnView3dModelOffsetTextCtrlChange(wxCommandEvent &event)
{
   view3dModelOffsetChanged  = true;
   dataChanged = true;
   theCBPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnView3dModelRotTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the 3D model rotation text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyVisualizationPanel::OnView3dModelRotTextCtrlChange(wxCommandEvent &event)
{
   view3dModelRotChanged  = true;
   dataChanged = true;
   theCBPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnView3dModelScaleTextCtrlChange(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user modifies the 3D model scale text box.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyVisualizationPanel::OnView3dModelScaleTextCtrlChange(wxCommandEvent &event)
{
   view3dModelScaleChanged  = true;
   dataChanged = true;
   theCBPanel->EnableUpdate(true);
}

//------------------------------------------------------------------------------
// void OnBrowseButton(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Handle the event triggered when the user selects the browse button.
 *
 * @param <event>    the handled event
 *
 */
//------------------------------------------------------------------------------
void CelestialBodyVisualizationPanel::OnBrowseButton(wxCommandEvent &event)
{
   if (event.GetEventObject() == textureFileBrowseButton)
   {
      wxString oldTexture = textureFileTextCtrl->GetValue();
      wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), _T("*.*"));
      if (dialog.ShowModal() == wxID_OK)
      {
         wxString fileName = (dialog.GetPath()).c_str();
         if (!fileName.IsSameAs(oldTexture))
         {
            textureFileTextCtrl->SetValue(fileName);
            textureFileChanged = true;
            theCBPanel->EnableUpdate(true);
            dataChanged = true;
         }
      }
   }
   else if (event.GetEventObject() == view3dModelBrowseButton)
   {
      wxString oldShapeModel = view3dModelTextCtrl->GetValue();
      wxFileDialog dialog(this, "Choose a file", "", "", "*.3ds");
      if (dialog.ShowModal() == wxID_OK)
      {
         wxString fileName = (dialog.GetPath()).c_str();
         if (!fileName.IsSameAs(oldShapeModel))
         {
            view3dModelTextCtrl->SetValue(fileName);
            view3dModelFileChanged = true;
            theCBPanel->EnableUpdate(true);
            dataChanged = true;
         }
      }
   }
}

