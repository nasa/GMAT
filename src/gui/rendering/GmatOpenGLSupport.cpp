//$Id$
//------------------------------------------------------------------------------
//                              gmatOpenGLSupport
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
// Author: Linda Jun, refactored and extracted from TrajPlotCanvas.cpp by Gene Stillman
//				Adjustments made by Phil Silvia
// Created: 2003/11/25
// Refactored here for visualization purposes
/**
* Implements gmatOpenGLSupport for opengl plot.
*/
//------------------------------------------------------------------------------
#include "GmatOpenGLSupport.hpp"   // for OpenGL support
#include "gmatwxdefs.hpp"          // for WX and GL
#include "MessageInterface.hpp"
#include "FallbackFont.hpp"

//#define DEBUG_INIT_GL
//#define TRAP_PIXEL_FORMAT_ERRORS  // Enable this to work on pixel format error
                                    // (see GMT-4316)


//------------------------------------------------------------------------------
// bool SetPixelFormatDescriptor()
//------------------------------------------------------------------------------
/**
* Sets pixel format on Windows.
*/
//------------------------------------------------------------------------------
bool SetPixelFormatDescriptor()
{
#ifdef __WXMSW__

   #ifdef DEBUG_INIT_GL
   MessageInterface::ShowMessage("SetPixelFormatDescriptor() entered\n");
   #endif
   
   // On Windows, for OpenGL, you have to set the pixel format
   // once before doing your drawing stuff. This function
   // properly sets it up.

   HDC hdc = wglGetCurrentDC();

   PIXELFORMATDESCRIPTOR pfd =
   {
      sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd
      1,                     // version number
      PFD_DRAW_TO_WINDOW |   // support window
      PFD_SUPPORT_OPENGL |   // support OpenGL
      PFD_DOUBLEBUFFER,      // double buffered
      PFD_TYPE_RGBA,         // RGBA type
      //24,                    // 24-bit color depth
      32,                    // 32-bit color depth
      0, 0, 0, 0, 0, 0,      // color bits ignored
      0,                     // no alpha buffer
      0,                     // shift bit ignored
      0,                     // no accumulation buffer
      0, 0, 0, 0,            // accum bits ignored
      32,                    // 32-bit z-buffer
      //16,                    // 16-bit z-buffer
      0,                     // no stencil buffer
      0,                     // no auxiliary buffer
      PFD_MAIN_PLANE,        // main layer
      0,                     // reserved
      0, 0, 0                // layer masks ignored
   };

   // get the device context's best-available-match pixel format
   int pixelFormatId = ChoosePixelFormat(hdc, &pfd);

   #ifdef DEBUG_INIT_GL
   MessageInterface::ShowMessage
      ("SetPixelFormatDescriptor() pixelFormatId = %d \n",
      pixelFormatId);
   #endif
	
   if(pixelFormatId == 0)
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** Failed to find a matching pixel format\n");
      #ifdef DEBUG_INIT_GL
      MessageInterface::ShowMessage("SetPixelFormatDescriptor() returning false\n");
      #endif
      return false;
   }
   
   // set the pixel format of the device context
   if (!SetPixelFormat(hdc, pixelFormatId, &pfd))
   {
      #ifdef TRAP_PIXEL_FORMAT_ERRORS
         MessageInterface::ShowMessage
            ("**** ERROR **** Failed to set pixel format id %d\n", pixelFormatId);
         #ifdef DEBUG_INIT_GL
         MessageInterface::ShowMessage("SetPixelFormatDescriptor() returning false\n");
         #endif
         return false;
      #endif
   }
   
   #ifdef DEBUG_INIT_GL
   MessageInterface::ShowMessage("SetPixelFormatDescriptor() returning true\n");
   #endif
   return true;
#else
   // Should we return true for non-Window system?
   //return false;
   return true;
#endif
}

//------------------------------------------------------------------------------
//  void SetDefaultGLFont()
//------------------------------------------------------------------------------
/**
* Sets default GL font.
*/
//------------------------------------------------------------------------------
void SetDefaultGLFont()
{
#ifdef __WXMSW__
   // Set up font stuff for windows -
   // Make the Current font the device context's selected font
   //SelectObject(dc, Font->Handle);
   HDC hdc = wglGetCurrentDC();

   wglUseFontBitmaps(hdc, 0, 255, 1000);
   glListBase(1000); // base for displaying
#else
   //RRC: Added fallback: cheap fixed-width font baked in (BET, 2016-10-19)
   unsigned int charidx;
   for(charidx = 0; charidx < 256; charidx++)
   {
      //Archaic GL, but equivalent to the above
      glNewList(charidx + 1000, GL_COMPILE);
      glBitmap(8, 8, 0, 0, 8, 0, &(FallbackFont[charidx*8]));
      glEndList();
   }

   glListBase(1000);
#endif
}

//------------------------------------------------------------------------------
// bool InitGL()
//------------------------------------------------------------------------------
/**
* Initializes GL and IL.
*/
//------------------------------------------------------------------------------
void InitGL()
{
	#ifdef DEBUG_INIT_GL
	MessageInterface::ShowMessage("\nInitGL() entered\n");
	#endif
	
   // remove back faces
   glEnable(GL_CULL_FACE);
   //glDisable(GL_CULL_FACE);
	
   // enable depth testing, so that objects further away from the
   // viewer aren't drawn over closer objects
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_TRUE);

   glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
   glDepthFunc(GL_LEQUAL);
   glClearDepth(1.0f);
   //glDepthFunc(GL_LESS);
   //glDepthRange(0.0, 100.0); //loj: just tried - made no difference

   // speedups
   glEnable(GL_DITHER);

   // set polygons to be smoothly shaded (i.e. interpolate lighting equations
   // between points on polygons).
   glShadeModel(GL_SMOOTH);
   glFrontFace(GL_CCW);

	glEnable(GL_LINE_SMOOTH);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

   // pixel format
   if (!SetPixelFormatDescriptor())
   {
      //throw SubscriberException("SetPixelFormatDescriptor failed\n");
   }

   // font
   SetDefaultGLFont();
	
	#ifdef DEBUG_INIT_GL
	MessageInterface::ShowMessage("InitGL() leaving\n\n");
	#endif
}


//------------------------------------------------------------------------------
// void ScreenShotSave(char* ImagePath)
//------------------------------------------------------------------------------
void ScreenShotSave(char* ImagePath)
{
   GLint vp[4];
   // Get the viewport dimensions
   glGetIntegerv(GL_VIEWPORT, vp);
   // Store the viewport width and height, width = vp[2], height = vp[3]
   wxImage image(vp[2], vp[3]);
   // glReadPixels can align the first pixel in each row at 1-, 2-, 4- and 
   // 8-byte boundaries. We have allocated the exact size needed for the image 
   // so we have to use 1-byte alignment (otherwise glReadPixels would write 
   // out of bounds)
   glPixelStorei(GL_PACK_ALIGNMENT, 1);
   glReadBuffer(GL_FRONT);
   glReadPixels(0, 0, vp[2], vp[3], GL_RGB, GL_UNSIGNED_BYTE, image.GetData());

   // glReadPixels reads the given rectangle from bottom-left to top-right, so we must
   // vertically flip the image to the proper orientation (option false is vertical flip)
   image = image.Mirror(false); 
   image.SaveFile(ImagePath, wxBITMAP_TYPE_PNG);
}
