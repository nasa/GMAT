//$Id$
//------------------------------------------------------------------------------
//                            VisualModelCanvas
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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
// number NNG04CC06P.
//
//
// Author: Phillip Silvia
// Created: 2009/08/13
/**
 * Canvas for displaying the loaded model inside the VisualModelPanel.
 */
//------------------------------------------------------------------------------

#ifndef _VISUALMODELCANVAS_H
#define _VISUALMODELCANVAS_H

#include "gmatwxdefs.hpp"
#include "Spacecraft.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "ModelObject.hpp"

class VisualModelCanvas : public wxGLCanvas 
{
public:
   VisualModelCanvas(wxWindow *parent, Spacecraft *spacecraft, const wxWindowID id = -1, 
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, 
                  const wxString& name = wxT("VisualModelCanvas"),
                  long style = 0);
   ~VisualModelCanvas();
   bool LoadModel(const wxString &filePath);
   void LoadModel();
   void Rotate(bool useDegrees, float xAngle, float yAngle, float zAngle);
   void Translate(float x, float y, float z);
   void Scale(float scale);
   void ToggleEarth()
      { showEarth = !showEarth; Refresh(false); }

protected:
   DECLARE_EVENT_TABLE();
   
   void OnMouse(wxMouseEvent &event);
   void OnKeyDown(wxKeyEvent &event);
   void OnPaint(wxPaintEvent &event);

private:

   bool glInitialized;
   
   // Camera and light used in the canvas
   Camera mCamera;
   Light mLight;

   // Pointers to the current spacecraft and current model from the model manager
   Spacecraft *currentSpacecraft;
public:
   ModelObject *loadedModel;
private:

   // The path to the model and a flag for loading the model
   wxString modelPath;
   bool needToLoadModel, showEarth;   

   wxGLContext *theContext;

   // Last mouse position
   float lastMouseX, lastMouseY;

   // The parent of the canvas
   wxWindow *vParent;
   
   void DrawAxes();
   bool SetGLContext();
};

#endif

