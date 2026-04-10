//$Id$
//------------------------------------------------------------------------------
//                             GuiPlotReceiver
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
// Author: Linda Jun
// Created: 2003/12/18
//
/**
 * Implements GuiPlotReceiver class.
 * This class updates OpenGL canvas, XY plot window
 */
//------------------------------------------------------------------------------
#if !defined __CONSOLE_APP__

#include "gmatwxdefs.hpp"
#include "gmatwxrcs.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "MdiGlPlotData.hpp"            // for 3D Visualization
#include "MdiChildViewFrame.hpp"        // for 3D Visualization
#include "MdiChild3DViewFrame.hpp"      // for 3D Visualization
#include "MdiTsPlotData.hpp"            // for XY plot
#include "MdiChildTsFrame.hpp"          // for XY plot
#include "MdiChildGroundTrackFrame.hpp" // for GroundTrackPlot
#include "MdiChildDynamicDataFrame.hpp" // for DynamicDataDisplay

#endif

#include "GuiPlotReceiver.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include "GroundTrackWindow.hpp"

//#define DEBUG_PLOTIF_GL_SET 1
//#define DEBUG_PLOTIF_GL_CREATE 1
//#define DEBUG_PLOTIF_GL_DELETE 1
//#define DEBUG_PLOTIF_GL_UPDATE 1
//#define DEBUG_PLOTIF_XY 1
//#define DEBUG_PLOTIF_XY_UPDATE 1
//#define DEBUG_RENAME 1
//#define DEBUG_PLOT_PERSISTENCE
//#define DEBUG_PLOTIF_XY_UPDATE

//---------------------------------
//  static data
//---------------------------------
GuiPlotReceiver* GuiPlotReceiver::theGuiPlotReceiver = NULL;

//---------------------------------
//  public functions
//---------------------------------

//------------------------------------------------------------------------------
// GuiPlotReceiver* GuiPlotReceiver::Instance()
//------------------------------------------------------------------------------
/**
 * Method used to initialize and retrieve teh GuiPlotReceiver singleton
 *
 * @return The singleton pointer
 */
//------------------------------------------------------------------------------
GuiPlotReceiver* GuiPlotReceiver::Instance()
{
   if (theGuiPlotReceiver == NULL)
      theGuiPlotReceiver = new GuiPlotReceiver();

   return theGuiPlotReceiver;
}

//------------------------------------------------------------------------------
//  GuiPlotReceiver()
//------------------------------------------------------------------------------
/**
 * Constructor used to create the singleton
 */
//------------------------------------------------------------------------------
GuiPlotReceiver::GuiPlotReceiver()
{
}


//------------------------------------------------------------------------------
//  ~GuiPlotReceiver()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GuiPlotReceiver::~GuiPlotReceiver()
{
}


//------------------------------------------------------------------------------
//  bool CreateGlPlotWindow(const std::string &plotName, ...)
//------------------------------------------------------------------------------
/*
 * Creates OpenGlPlot window
 *
 * @param <plotName> plot name
 * @param <oldName>  old plot name, this is needed for renaming plot
 * @param <positionX, positionY>     position of the plot in screen coordinates
 * @param <width, height>     size of the plot in screen coordinates
 * @param <numPtsToRedraw>  number of points to redraw during the run
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::CreateGlPlotWindow(const std::string &plotName,
                                         const std::string &oldName,
                                         Real positionX, Real positionY,
                                         Real width, Real height, bool isMaximized,
                                         Integer numPtsToRedraw)
{
   //-------------------------------------------------------
   // check if new MDI child frame is needed
   //-------------------------------------------------------
   bool createNewFrame = true;
   wxString currPlotName;
   MdiChildViewFrame *frame = NULL;
   
   #if DEBUG_PLOTIF_GL_CREATE
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::CreateGlPlotWindow() MdiGlPlot::numChildren=%d, "
       "plotName='%s', numPtsToRedraw=%d\n   oldName=%s\n", MdiGlPlot::numChildren,
       plotName.c_str(), oldName.c_str(), numPtsToRedraw);
   #endif
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame)
         currPlotName = frame->GetPlotName();
      else
         break;
      
      #if DEBUG_PLOTIF_GL_CREATE
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::CreateGlPlotWindow() currPlotName[%d]=<%p>'%s'\n",
          i, frame, currPlotName.WX_TO_C_STRING);
      #endif
      
      if (currPlotName.IsSameAs(plotName.c_str()))
      {
         if (!frame->ChildIsClosing())
            createNewFrame = false;
         break;
      }
      else if (currPlotName.IsSameAs(oldName.c_str()))
      {
         // change plot name
         frame->SetPlotName(wxString(plotName.c_str()));
         if (!frame->ChildIsClosing())
            createNewFrame = false;
         break;
      }
   }
   
   //-------------------------------------------------------
   // create MDI child GL frame if not exist
   //-------------------------------------------------------
   if (createNewFrame)
   {
      #if DEBUG_PLOTIF_GL_CREATE
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::CreateGlPlotWindow() Creating MdiChildViewFrame "
          "%s\n", plotName.c_str());
      #endif
      
      Integer x, y, w, h;
      Integer plotCount = MdiGlPlot::numChildren + MdiTsPlot::numChildren;
      
      #ifdef DEBUG_PLOT_PERSISTENCE
      MessageInterface::ShowMessage
         ("plotName = '%s', plotCount = %d\n", plotName.c_str(), plotCount);
      #endif
      bool isUsingSaved = false;
      ComputePlotPositionAndSize(true, positionX, positionY,
                                 width, height, x, y, w, h,
                                 isUsingSaved);
      
      
      // create a frame, containing a plot canvas
      if (currentView == GmatPlot::ENHANCED_3D_VIEW)
      {
         #if DEBUG_PLOTIF_GL_CREATE
         MessageInterface::ShowMessage("   Creating MdiChild3DViewFrame...\n");
         #endif
         frame = new MdiChild3DViewFrame
            (GmatAppData::Instance()->GetMainFrame(),
             wxString(plotName.c_str()), wxString(plotName.c_str()),
             wxPoint(x, y), wxSize(w, h), wxDEFAULT_FRAME_STYLE);
      }
      else if (currentView == GmatPlot::GROUND_TRACK_PLOT)
      {
         #if DEBUG_PLOTIF_GL_CREATE
         MessageInterface::ShowMessage("   Creating MdiChildGroundTrackFrame...\n");
         #endif
         frame = new MdiChildGroundTrackFrame
            (GmatAppData::Instance()->GetMainFrame(),
             wxString(plotName.c_str()), wxString(plotName.c_str()),
             wxPoint(x, y), wxSize(w, h), wxDEFAULT_FRAME_STYLE);
      }
      else
      {
         MessageInterface::ShowMessage
            ("**** ERROR **** Unknown view type %d\n", currentView);
         return false;
      }

      if (frame)
      {
		 // TGG: Commented out until we can fix the painting problem when maximizing right here
		 //if (isMaximized)
		 //{
		 //   frame->Maximize();
		 //}
         frame->SetSavedConfigFlag(isUsingSaved);
         frame->SetIsNewFrame(true);
         
         if (GmatGlobal::Instance()->GetGuiMode() == GmatGlobal::MINIMIZED_GUI)
            frame->Show(false);
         else
            frame->Show(true);
      }
      else
         return false;
      
      #if __WXMAC__
         frame->SetSize(w-1, h-1);
      #else
         if (w != -1 && h != -1)
         {
            // Why not showing the plot without resetting the size?
            // Used SetSize with wxSIZE_NO_ADJUSTMENTS (LOJ: 2012.01.05)
            frame->SetSize(w-1, h-1);
            //frame->SetSize(w+1, h+1);
            frame->SetSize(x, y, w+1, h+1, wxSIZE_NO_ADJUSTMENTS);
         }
      #endif
         
      #if DEBUG_PLOTIF_GL_CREATE
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::CreateGlPlotWindow() frame created, frame->GetPlotName()=%s\n",
          frame->GetPlotName().WX_TO_C_STRING);
      #endif
      
      ++MdiGlPlot::numChildren;
      plotCount = MdiGlPlot::numChildren + MdiTsPlot::numChildren;
      
      // Tile plots if TILED_PLOT mode is set from the startup file
      if (GmatGlobal::Instance()->GetPlotMode() == GmatGlobal::TILED_PLOT)
      {
         // Set wxVERTICAL or wxHORIZONTAL based on main frame width and height
         // if run mode is TESTING (LOJ: 2015.07.23)
         wxOrientation tileMode = wxVERTICAL;
         //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)         // made changes by TUAN NGUYEN
         if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING)    // made changes by TUAN NGUYEN
         {
            int w, h;
            GmatAppData::Instance()->GetMainFrame()->GetActualClientSize(&w, &h, false);
            if (w < h)
               tileMode = wxHORIZONTAL;
         }
         GmatAppData::Instance()->GetMainFrame()->Tile(tileMode);
      }
   }
   else
   {
      frame->SetIsNewFrame(false);
      
      #if DEBUG_PLOTIF_GL_CREATE
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::CreateGlPlotWindow() PlotName:%s already exist.\n",
          plotName.c_str());
      #endif
   }
   
   #if DEBUG_PLOTIF_GL_CREATE
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::CreateGlPlotWindow() setting num points to redraw for %s\n",
       frame->GetPlotName().WX_TO_C_STRING);
   #endif
   
   frame->SetNumPointsToRedraw(numPtsToRedraw);
   
   #if DEBUG_PLOTIF_GL_CREATE
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::CreateGlPlotWindow() returning true, there are %d plots.\n",
       MdiGlPlot::numChildren);
   #endif
   
   return true;
} //CreateGlPlotWindow()


//------------------------------------------------------------------------------
// void SetGlSolarSystem(const std::string &plotName, SolarSystem *ss)
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetGlSolarSystem(const std::string &plotName, SolarSystem *ss)
{
   #if DEBUG_PLOTIF_GL_SET
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::SetGlSolarSystem() SolarSystem=%p\n", ss);
   #endif

   wxString owner = wxString(plotName.c_str());

   MdiChildViewFrame *frame = NULL;
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->SetSolarSystem(ss);
         }
   }
}


//------------------------------------------------------------------------------
// void SetGlObject(const std::string &plotName,  ...
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetGlObject(const std::string &plotName,
                                  const StringArray &objNames,
                                  //const UnsignedIntArray &objOrbitColors,
                                  const std::vector<SpacePoint*> &objArray)
{
   #if DEBUG_PLOTIF_GL_SET
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::SetGlObject() plotName:%s\n", plotName.c_str());
   #endif

   wxString owner = wxString(plotName.c_str());

   MdiChildViewFrame *frame = NULL;
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            //frame->SetGlObject(objNames, objOrbitColors, objArray);
            frame->SetGlObject(objNames, objArray);
         }
   }
}


//------------------------------------------------------------------------------
// static void SetGlCoordSystem(const std::string &plotName, ...
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetGlCoordSystem(const std::string &plotName,
                                       CoordinateSystem *internalCs,
                                       CoordinateSystem *viewCs,
                                       CoordinateSystem *viewUpCs)
{
   #if DEBUG_PLOTIF_GL_SET
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::SetGlCoordSystem() plotName:%s\n", plotName.c_str());
   #endif

   wxString owner = wxString(plotName.c_str());
   MdiChildViewFrame *frame = NULL;

   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->SetGlCoordSystem(internalCs, viewCs, viewUpCs);
         }
   }
}


//------------------------------------------------------------------------------
// void SetGl2dDrawingOption(const std::string &plotName, ...)
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetGl2dDrawingOption(const std::string &plotName,
                                           const std::string &centralBodyName,
                                           const std::string &textureMap,
                                           Integer footPrintOption)
{
   wxString owner = wxString(plotName.c_str());
   MdiChildViewFrame *frame = NULL;
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {         
            frame->SetGl2dDrawingOption(centralBodyName, textureMap, footPrintOption);         
            break;
         }
   }
}


//------------------------------------------------------------------------------
// void SetGl3dDrawingOption(const std::string &plotName, ...)
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetGl3dDrawingOption(const std::string &plotName,
                                           bool showLabels, bool drawEcPlane,
                                           bool drawXyPlane, bool drawWireFrame,
                                           bool drawAxes, bool drawGrid, bool drawSunLine,
                                           bool overlapPlot, bool usevpInfo,
                                           bool drawStars, bool drawConstellations,
                                           Integer starCount)
{
   wxString owner = wxString(plotName.c_str());
   MdiChildViewFrame *frame = NULL;

   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
      
      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->SetGl3dDrawingOption(showLabels, drawEcPlane, drawXyPlane, drawWireFrame,
                                        drawAxes, drawGrid, drawSunLine, overlapPlot,
                                        usevpInfo, drawStars, drawConstellations,
                                        starCount);
            break;
         }
   }
}


//------------------------------------------------------------------------------
// void SetGl3dViewOption(const std::string &plotName, SpacePoint *vpRefObj, ...
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetGl3dViewOption(const std::string &plotName,
                                      SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                      SpacePoint *vdObj, Real vsFactor,
                                      const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                      const Rvector3 &vdVec, const std::string &upAxis,
                                      bool usevpRefVec, bool usevpVec, bool usevdVec)
{
   #if DEBUG_PLOTIF_GL_SET
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::SetGl3dViewOption() plotName:%s\n", plotName.c_str());
   #endif

   wxString owner = wxString(plotName.c_str());
   MdiChildViewFrame *frame = NULL;

   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            #if DEBUG_PLOTIF_GL_SET
            MessageInterface::ShowMessage
               ("GuiPlotReceiver::SetGl3dViewOption() vpRefObj=%d, vsFactor=%f\n",
                vpRefObj, vsFactor);
            #endif
         
            frame->SetGl3dViewOption(vpRefObj, vpVecObj, vdObj, vsFactor, vpRefVec,
                                     vpVec, vdVec, upAxis, usevpRefVec,usevpVec,
                                     usevdVec);
         }
   }
}


//------------------------------------------------------------------------------
// void SetGlDrawOrbitFlag(const std::string &plotName, ...
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetGlDrawOrbitFlag(const std::string &plotName,
                                         const std::vector<bool> &drawArray)
{
   #if DEBUG_PLOTIF_GL_SET
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::SetGlDrawOrbitFlag() plotName:%s\n", plotName.c_str());
   #endif

   wxString owner = wxString(plotName.c_str());
   MdiChildViewFrame *frame = NULL;

   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->SetGlDrawOrbitFlag(drawArray);
         }
   }
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::string &plotName, ...
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetGlShowObjectFlag(const std::string &plotName,
                                        const std::vector<bool> &showArray)
{
   #if DEBUG_PLOTIF_GL_SET
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::SetGlShowObjectFlag() plotName:%s\n", plotName.c_str());
   #endif

   wxString owner = wxString(plotName.c_str());
   MdiChildViewFrame *frame = NULL;

   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->SetGlShowObjectFlag(showArray);
         }
   }
}


//------------------------------------------------------------------------------
// void SetGlUpdateFrequency(const std::string &plotName, Integer updFreq)
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetGlUpdateFrequency(const std::string &plotName,
                                         Integer updFreq)
{
   wxString owner = wxString(plotName.c_str());
   MdiChildViewFrame *frame = NULL;

   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->SetGlUpdateFrequency(updFreq);
         }
   }
}


//------------------------------------------------------------------------------
//  bool IsThere(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Checks if OpenGlPlot exist.
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::IsThere(const std::string &plotName)
{
   if (GmatAppData::Instance()->GetMainFrame() != NULL)
   {
      wxString owner = wxString(plotName.c_str());

      MdiChildViewFrame *frame  = NULL;
      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

         if (frame && !frame->ChildIsClosing())
            if (frame->GetPlotName().IsSameAs(owner.c_str()))
            {
               return true;
            }
      }
   }

   return false;
}


//------------------------------------------------------------------------------
//  bool InitializeGlPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Refreshes OpenGlPlot.
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::InitializeGlPlot(const std::string &plotName)
{
   if (GmatAppData::Instance()->GetMainFrame() != NULL)
   {
      #if DEBUG_PLOTIF_GL
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::InitializeGlPlot() plotName=%s\n",plotName.c_str());
      #endif

      wxString owner = wxString(plotName.c_str());
      MdiChildViewFrame *frame = NULL;

      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

         if (frame && !frame->ChildIsClosing())
            if (frame->GetPlotName().IsSameAs(owner.c_str()))
            {
               frame->InitializePlot();
            }
      }
   }

   return true;
}


//------------------------------------------------------------------------------
//  bool RefreshGlPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Refreshes OpenGlPlot.
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::RefreshGlPlot(const std::string &plotName)
{
   if (GmatAppData::Instance()->GetMainFrame() != NULL)
   {
      #if DEBUG_PLOTIF_GL
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::RefreshGlPlot() plotName=%s\n",plotName.c_str());
      #endif

      wxString owner = wxString(plotName.c_str());
      MdiChildViewFrame *frame = NULL;

      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

         if (frame && !frame->ChildIsClosing())
            if (frame->GetPlotName().IsSameAs(owner.c_str()))
            {
               frame->RefreshPlot();
            }
      }
   }

   return true;
}


//------------------------------------------------------------------------------
//  bool DeleteGlPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Deletes OpenGlPlot by plot name.
 *
 * @param <plotName> name of plot to be deleted
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::DeleteGlPlot(const std::string &plotName)
{
   GmatAppData *gmatAppData = GmatAppData::Instance();

   if (gmatAppData->GetMainFrame() != NULL)
   {
      #if DEBUG_PLOTIF_GL_DELETE
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::DeleteGlPlot() plotName=%s\n", plotName.c_str());
      #endif

      wxString owner = wxString(plotName.c_str());
      MdiChildViewFrame *frame = NULL;

      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());
         
         if (frame)
            if (!frame->ChildIsClosing())
            {
               // Delete GL plot by plot name and type
               GmatTree::ItemType plotType = frame->GetItemType();
               wxString plotName = frame->GetPlotName();
               if (plotName.IsSameAs(owner.c_str()))
               {
                  #if DEBUG_PLOTIF_GL_DELETE
                  MessageInterface::ShowMessage
                     ("GuiPlotReceiver::DeleteGlPlot() closing child '%s' from the "
                      "output tab\n", plotName.WX_TO_C_STRING);
                  #endif
                  gmatAppData->GetMainFrame()->CloseChild(owner, plotType);
               }
            }
      }
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetGlEndOfRun(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Sets end of run flag to OpenGlPlot.
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::SetGlEndOfRun(const std::string &plotName)
{
   if (GmatAppData::Instance()->GetMainFrame() != NULL)
   {
      #if DEBUG_PLOTIF_GL_SET
         MessageInterface::ShowMessage
            ("GuiPlotReceiver::SetGlEndOfRun() plotName=%s\n",plotName.c_str());
      #endif
      wxString owner = wxString(plotName.c_str());

      MdiChildViewFrame *frame = NULL;
      for (int i=0; i<MdiGlPlot::numChildren; i++)
      {
         frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

         if (frame && !frame->ChildIsClosing())
            if (frame->GetPlotName().IsSameAs(owner.c_str()))
            {
               frame->SetEndOfRun();
            }
      }
   }

   return true;
}


//------------------------------------------------------------------------------
//  bool UpdateGlPlot(const std::string &plotName, ...
//------------------------------------------------------------------------------
/*
 * Buffers data and updates OpenGL plow window if updateCanvas is true
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::UpdateGlPlot(const std::string &plotName,
                                   const std::string &oldName,
                                   const StringArray &scNames, const Real &time,
                                   const RealArray &posX, const RealArray &posY,
                                   const RealArray &posZ, const RealArray &velX,
                                   const RealArray &velY, const RealArray &velZ,
                                   //const UnsignedIntArray &scColors,
                                   const ColorMap &orbitColorMap,
                                   const ColorMap &targetColorMap,
                                   bool solving, Integer solverOption,
                                   bool updateCanvas, bool drawing, bool inFunction)
{
   #if DEBUG_PLOTIF_GL_UPDATE
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::UpdateGlPlot() entered. time = %f, number of plots = %d\n",
       time, MdiGlPlot::numChildren);
   #endif
   
   bool updated = false;
   wxString owner = wxString(plotName.c_str());
   
   MdiChildViewFrame *frame = NULL;
   
   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

      #if DEBUG_PLOTIF_GL_UPDATE
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::UpdateGlPlot() frame[%d]->GetPlotName()=%s "
          "owner=%s\n", i, frame->GetPlotName().WX_TO_C_STRING,
          owner.WX_TO_C_STRING);
      #endif
      
      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            #if DEBUG_PLOTIF_GL_UPDATE
            MessageInterface::ShowMessage
               ("GuiPlotReceiver::UpdateGlPlot() now updating '%s'...\n",
                frame->GetPlotName().WX_TO_C_STRING);
            #endif
            frame->UpdatePlot(scNames, time, posX, posY, posZ, velX, velY, velZ,
                              //scColors,
                              orbitColorMap, targetColorMap,
                              solving, solverOption, updateCanvas, drawing,
                              inFunction);
         
            updated = true;
            break;
         }
   }
   
   return updated;
} // end UpdateGlPlot()


//------------------------------------------------------------------------------
// bool TakeGlAction(const std::string &plotName, const std::string &action)
//------------------------------------------------------------------------------
bool GuiPlotReceiver::TakeGlAction(const std::string &plotName,
                                 const std::string &action)
{
   #if DEBUG_PLOTIF_GL_CLEAR
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::ClearGlSolverData() entered\n");
   #endif

   bool retval = false;
   wxString owner = wxString(plotName.c_str());

   MdiChildViewFrame *frame = NULL;

   for (int i=0; i<MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

      #if DEBUG_PLOTIF_GL_CLEAR
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::ClearGlSolverData() frame[%d]->GetPlotName()=%s "
          "owner=%s\n", i, frame->GetPlotName().c_str(), owner.c_str());
      #endif

      if (frame && !frame->ChildIsClosing())
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->TakeAction(action);
            retval = true;
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void GuiPlotReceiver::SetMaxGlDataPoints(const std::string &plotName,
//                                          Integer maxDataPoints)
//------------------------------------------------------------------------------
/**
* Sets the custom max data point setting
*
* @param plotName The name of the plot receiving the data
* @param maxDataPoints The max data points to be plotted
*/
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetMaxGlDataPoints(const std::string &plotName,
                                         Integer maxDataPoints)
{
   MdiChildViewFrame *frame = NULL;

   for (int i = 0; i < MdiGlPlot::numChildren; i++)
   {
      frame = (MdiChildViewFrame*)(MdiGlPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
            frame->SetMaxDataPoints(maxDataPoints);
   }
}


//------------------------------------------------------------------------------
//  bool CreateXyPlotWindow(const std::string &plotName,
//       const std::string &oldName, const std::string &plotTitle,
//       const std::string &xAxisTitle, const std::string &yAxisTitle,
//       bool drawGrid = false)
//------------------------------------------------------------------------------
/*
 * Creates a XyPlot window.
 *
 * @param plotName Name of the plot
 * @param oldName Former name of the plot
 * @param positionX, positionY    Position of the plot in screen coordinates
 * @param width, height    Size of the plot in screen coordinates
 * @param plotTitle Title of the plot
 * @param xAxisTitle X-axis label for the plot
 * @param yAxisTitle Y-axis label for the plot
 * @param drawGrid Flag indicating if the grid lines should be drawn
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::CreateXyPlotWindow(const std::string &plotName,
                                       const std::string &oldName,
                                       Real positionX, Real positionY,
                                       Real width, Real height, bool isMaximized,
                                       const std::string &plotTitle,
                                       const std::string &xAxisTitle,
                                       const std::string &yAxisTitle,
                                       bool drawGrid,
                                       bool canSaveLocation)
{
   //-------------------------------------------------------
   // check if new MDI child frame needed
   //-------------------------------------------------------
   bool createNewFrame = true;
   wxString currPlotName;
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      currPlotName = frame->GetPlotName();

      if (currPlotName.IsSameAs(plotName.c_str()))
      {
         if (!frame->ChildIsClosing())
            createNewFrame = false;
         break;
      }
      else if (currPlotName.IsSameAs(oldName.c_str()))
      {
         #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("GuiPlotReceiver::CreateXyPlotWindow() currPlotName=%s, "
                  "oldName=%s\n", currPlotName.c_str(), oldName.c_str());
         #endif

         // change plot name
         frame->SetPlotName(wxString(plotName.c_str()));
         if (!frame->ChildIsClosing())
            createNewFrame = false;
         break;
      }
   }

   //-------------------------------------------------------
   // create MDI child XY frame
   //-------------------------------------------------------
   if (createNewFrame)
   {
      #if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::CreateXyPlotWindow() Creating new "
          "MdiChildTsFrame\n   X Axis Title = %s  Y Axis Title = %s\n",
          xAxisTitle.c_str(), yAxisTitle.c_str());
      #endif
      
      Integer x, y, w, h;
      Integer plotCount = MdiGlPlot::numChildren + MdiTsPlot::numChildren;
      bool isUsingSaved = false;
      
      #ifdef DEBUG_PLOT_PERSISTENCE
      MessageInterface::ShowMessage
         ("plotName = '%s', plotCount = %d\n", plotName.c_str(), plotCount);
      #endif
      
      ComputePlotPositionAndSize(false, positionX, positionY,
                                 width, height, x, y, w, h,
                                 isUsingSaved);
      #ifdef DEBUG_PLOT_PERSISTENCE
      MessageInterface::ShowMessage
         ("About to create an XYPlot at position x = %d, y = %d, and size w = %d, h = %d\n", x, y, w, h);
      #endif
      
      // create a frame, containing a XY plot canvas
      frame =
         new MdiChildTsFrame(GmatAppData::Instance()->GetMainFrame(), true,
                             wxString(plotName.c_str()),
                             wxString(plotTitle.c_str()),
                             wxString(xAxisTitle.c_str()),
                             wxString(yAxisTitle.c_str()),
                             wxPoint(x, y), wxSize(w, h),
                             wxDEFAULT_FRAME_STYLE);
		 // TGG: Commented out until we can fix the painting problem when maximizing right here
		 //if (isMaximized)
		 //{
		 //   frame->Maximize();
		 //}
      
      if (GmatGlobal::Instance()->GetGuiMode() == GmatGlobal::MINIMIZED_GUI)
         frame->Show(false);
      else
         frame->Show(true);
      frame->SetSavedConfigFlag(isUsingSaved);
      frame->SetSaveLocationFlag(canSaveLocation);
      
      ++MdiTsPlot::numChildren;
      
      #if __WXMAC__
         frame->SetSize(w-1, h-1);
      #else
         if (w != -1 && h != -1)
         {
            // Why not showing the plot without resetting the size?
            // Used SetSize with wxSIZE_NO_ADJUSTMENTS (LOJ: 2012.01.05)
            frame->SetSize(w-1, h-1);
            //frame->SetSize(w+1, h+1);
            frame->SetSize(x, y, w+1, h+1, wxSIZE_NO_ADJUSTMENTS);
         }
      #endif
         
      plotCount = MdiGlPlot::numChildren + MdiTsPlot::numChildren;
      
      // Do no tile at all (LOj: 2011.09.23)
      //if (!isPresetSizeUsed && plotCount > 5)
      //   GmatAppData::Instance()->GetMainFrame()->Tile(wxVERTICAL);
      // Tile plots if TILED_PLOT mode is set from the startup file
      if (GmatGlobal::Instance()->GetPlotMode() == GmatGlobal::TILED_PLOT)
      {
         // Set wxVERTICAL or wxHORIZONTAL based on main frame width and height
         // if run mode is TESTING (LOJ: 2015.07.23)
         wxOrientation tileMode = wxVERTICAL;
         //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)          // made changes by TUAN NGUYEN
         if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING)     // made changes by TUAN NGUYEN
         {
            int w, h;
            GmatAppData::Instance()->GetMainFrame()->GetActualClientSize(&w, &h, false);
            if (w < h)
               tileMode = wxHORIZONTAL;
         }
         GmatAppData::Instance()->GetMainFrame()->Tile(tileMode);
      }
      frame->RedrawCurve();
   }
   
   frame->SetShowGrid(drawGrid);
   frame->ResetZoom();
   
   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::CreateXyPlotWindow() leaving\n");
   #endif

   return true;
}

//------------------------------------------------------------------------------
//  bool DeleteXyPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Deletes XyPlot by plot name.
 *
 * @param plotName name of plot to be deleted
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::DeleteXyPlot(const std::string &plotName)
{
   GmatAppData *gmatAppData = GmatAppData::Instance();

   if (gmatAppData->GetMainFrame() != NULL)
   {
      #if DEBUG_PLOTIF_XY
         MessageInterface::ShowMessage("GuiPlotReceiver::DeleteXyPlot()\n");
      #endif

      wxString owner = wxString(plotName.c_str());
      MdiChildTsFrame *frame = NULL;

      for (int i=0; i<MdiTsPlot::numChildren; i++)
      {
         frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

         if (frame && !frame->ChildIsClosing())
            if (frame->GetPlotName().IsSameAs(owner.c_str()))
            {
               gmatAppData->GetMainFrame()->CloseChild(owner,
                     GmatTree::OUTPUT_XY_PLOT);
               break;
            }
      }
   }
   
   return true;
}

//------------------------------------------------------------------------------
// bool AddXyPlotCurve(const std::string &plotName, int curveIndex,
//                     Real yMin, Real yMax,
//                     const std::string &curveTitle,
//                     UnsignedInt penColor)
//------------------------------------------------------------------------------
/*
 * Adds a plot curve to XY plow window.
 *
 * @param plotName The name of the plot that receives the new curve
 * @param curveIndex The index for the curve
 * @param curveTitle Label for the curve
 * @param penColor Default color for the curve
 *
 * @return true on success, false is no curve was added
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::AddXyPlotCurve(const std::string &plotName,
      int curveIndex, const std::string &curveTitle, UnsignedInt penColor)
{
   UnsignedInt localPenColor = penColor;
   if (penColor == 0)
      localPenColor = 0xFFFFFF;

   bool added = false;

   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::AddXyPlotCurve() entered."
       " plotName = " + plotName + " curveTitle = " +
       curveTitle + "\n");

   MessageInterface::ShowMessage
      ("GuiPlotReceiver::AddXyPlotCurve() numChildren = %d\n",
       MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;
   for (int i = 0; i < MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->AddPlotCurve(curveIndex, wxString(curveTitle.c_str()), localPenColor);
            added = true;
         }
   }

   return added;
}

//------------------------------------------------------------------------------
// bool DeleteAllXyPlotCurves(const std::string &plotName,
//                            const std::string &oldName)
//------------------------------------------------------------------------------
/*
 * Deletes all plot curves in XY plow window.
 *
 * @param plotName The name of the plot that receives the new curve
 * @param oldName The previous name of the plot that receives the new curve
 *
 * @return true on success, false if no action was taken
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::DeleteAllXyPlotCurves(const std::string &plotName,
                                          const std::string &oldName)
{
   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::DeleteAllPlotCurve() plotName = %s "
       "numChildren = %d\n", plotName.c_str(),
       MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()) ||
             frame->GetPlotName().IsSameAs(oldName.c_str()))
         {
            frame->DeleteAllPlotCurves();
         }
   }

   return true;
}


//------------------------------------------------------------------------------
// bool DeleteXyPlotCurve(const std::string &plotName, int curveIndex)
//------------------------------------------------------------------------------
/*
 * Deletes a plot curve to XY plow window.
 *
 * @param plotName The name of the plot that receives the new curve
 * @param curveIndex Index of the curve that is to be deleted
 *
 * @return true on success, false if no curve was deleted
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::DeleteXyPlotCurve(const std::string &plotName,
      int curveIndex)
{
   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::DeleteXyPlotCurve() entered plotName = %s "
       "curveIndex = %d\n", plotName.c_str(), curveIndex);

   MessageInterface::ShowMessage
      ("GuiPlotReceiver::DeleteXyPlotCurve() numChildren = %d\n",
       MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->DeletePlotCurve(curveIndex);
         }
   }

   return true;
}


//------------------------------------------------------------------------------
// void ClearXyPlotData(const std::string &plotName))
//------------------------------------------------------------------------------
/**
 * Removes all data from the plot curves, leaving the curve containers in place
 * but empty.
 *
 * @param plotName The name of the plot that is being cleared
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::ClearXyPlotData(const std::string &plotName)
{
   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::ClearXyPlotData() numChildren = %d\n",
       MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->ClearPlotData();
         }
   }
}

//------------------------------------------------------------------------------
// void XyPlotPenUp(const std::string &plotName))
//------------------------------------------------------------------------------
/**
 * Tells a plot to stop drawing received data.  This method is idempotent.
 *
 * @param plotName The name of the plot that is being cleared
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotPenUp(const std::string &plotName)
{
   #if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::XyPlotPenUp() numChildren = %d\n",
          MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->PenUp();
         }
   }
}

//------------------------------------------------------------------------------
// void XyPlotPenDown(const std::string &plotName))
//------------------------------------------------------------------------------
/**
 * Tells a plot to resume drawing received data.  This method is idempotent.
 *
 * @param plotName The name of the plot that is being cleared
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotPenDown(const std::string &plotName)
{
   #if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::XyPlotPenDown() numChildren = %d\n",
          MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->PenDown();
         }
   }
}


void GuiPlotReceiver::XyPlotDarken(const std::string &plotName, Integer factor,
         Integer index, Integer forCurve)
{
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->Darken(factor, index, forCurve);
         }
   }
}


void GuiPlotReceiver::XyPlotLighten(const std::string &plotName, Integer factor,
         Integer index, Integer forCurve)
{
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->Lighten(factor, index, forCurve);
         }
   }
}


//------------------------------------------------------------------------------
// void XyPlotMarkPoint(const std::string &plotName, Integer index,
//       Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Marks a specific point on a specific curve of a XyPlot with an oversized X
 *
 * @param plotName The plot that contains the curve
 * @param index The index of the point that gets marked
 * @param curveNumber The index of the curve containing the point to mark.  Set
 *                    curveNumber to -1 to mark all curves.
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotMarkPoint(const std::string &plotName,
      Integer index, Integer forCurve)
{
   #if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::XyPlotPenDown() numChildren = %d\n",
          MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->MarkPoint(index, forCurve);
         }
   }
}

//------------------------------------------------------------------------------
// void XyPlotMarkBreak(const std::string &plotName, Integer index, 
//       Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Marks a break point on a curve or plot
 * 
 * @param plotName The plot that contains the curve
 * @param index The index of the point where the break occurs
 * @param forCurve The index of the curve that contains the break (-1 for all)
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotMarkBreak(const std::string &plotName,
      Integer index, Integer forCurve)
{
   #ifdef DEBUG_MESSSAGE_FLOW
      MessageInterface::ShowMessage("GuiPlotReceiver::XyPlotMarkBreak(%s, %d, "
            "%d) called\n", plotName.c_str(), index, forCurve);
   #endif
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->MarkBreak(index, forCurve);
         }
   }
}


//------------------------------------------------------------------------------
// void GuiPlotReceiver::XyPlotClearFromBreak(const std::string &plotName,
//       Integer startBreakNumber, Integer endBreakNumber, Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Removes a range of data from a curve or plot
 * 
 * @param plotName The plot that contains the curve
 * @param startBreakNumber The index of the breakpoint where the break occurs
 * @param endBreakNumber The index of the breakpoint where the break ends (-1
 *                       to remove to the end of the data)
 * @param forCurve The index of the curve that contains the break (-1 for all)
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotClearFromBreak(const std::string &plotName,
      Integer startBreakNumber, Integer endBreakNumber, Integer forCurve)
{
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->ClearFromBreak(startBreakNumber, endBreakNumber, forCurve);
         }
   }
}


//------------------------------------------------------------------------------
// void XyPlotChangeColor(const std::string &plotName, Integer index,
//       UnsignedInt newColor, Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Changes the color of a curve partway into a run
 *
 * @param plotName The plot that contains the curve
 * @param index The index of the first point that gets the new color
 * @param newColor The new color
 * @param forCurve The index of the curve that is changing color.
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotChangeColor(const std::string &plotName,
      Integer index, UnsignedInt newColor, Integer forCurve)
{
   #if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::XyPlotChangeColor() numChildren = %d\n",
          MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->ChangeColor(index, newColor, forCurve);
         }
   }
}


//------------------------------------------------------------------------------
// void XyPlotChangeMarker(const std::string &plotName, Integer index,
//       Integer newMarker, Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Changes the marker used on a curve, starting at a specified point
 *
 * @param plotName The plot that contains the curve
 * @param index The index of the first point that gets the new color
 * @param newMarker The new marker
 * @param forCurve The index of the curve that is changing color.
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotChangeMarker(const std::string &plotName,
      Integer index, Integer newMarker, Integer forCurve)
{
   #if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::XyPlotPenDown() numChildren = %d\n",
          MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->ChangeMarker(index, newMarker, forCurve);
         }
   }
}


//------------------------------------------------------------------------------
// void XyPlotChangeWidth(const std::string &plotName, Integer index = -1,
//       Integer newWidth = 1, int forCurve = -1)
//------------------------------------------------------------------------------
/**
 * Changes the line width for a curve.
 *
 * Note that this is currently a setting for the entire curve; the index
 * parameter is not yet used.
 *
 * @param plotName The plot that contains the curve
 * @param index The index of the first point that gets the new width (currently
 *              not used)
 * @param newWidth The new width
 * @param forCurve The index of the curve that is changing width.
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotChangeWidth(const std::string &plotName,
      Integer index, Integer newWidth, int forCurve)
{
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->SetLineWidth(newWidth, forCurve);
         }
   }
}


//------------------------------------------------------------------------------
// void XyPlotChangeStyle(const std::string &plotName, Integer index = -1,
//       Integer newStyle = 100, int forCurve = -1)
//------------------------------------------------------------------------------
/**
 * Changes the drawing style for a curve.
 *
 * Note that this is currently a setting for the entire curve; the index
 * parameter is not yet used.
 *
 * @param plotName The plot that contains the curve
 * @param index The index of the first point that gets the new style (currently
 *              not used)
 * @param newStyle The new line style
 * @param forCurve The index of the curve that is changing style.
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotChangeStyle(const std::string &plotName,
      Integer index, Integer newStyle, int forCurve)
{
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->SetLineStyle(newStyle, forCurve);
         }
   }
}


//------------------------------------------------------------------------------
// void XyPlotRescale(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Sends a rescale message to the plot
 *
 * @param plotName The plot that is to be rescaled
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotRescale(const std::string &plotName)
{
   #if DEBUG_PLOTIF_XY
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::XyPlotRescale() numChildren = %d\n",
          MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->Rescale();
         }
   }
}


//------------------------------------------------------------------------------
// void XyPlotCurveSettings(const std::string &plotName, bool useLines,
//       Integer lineWidth, Integer lineStyle, bool useMarkers,
//       Integer markerSize, Integer marker, bool useHiLow, Integer forCurve)
//------------------------------------------------------------------------------
/**
 * Sets the default settings for a curve
 *
 * @param plotName The name of the plot that contains the curve
 * @param useLines Flag that is set if the curve should have lines connecting
 *                 the curve points
 * @param lineWidth The width, in pixels, of all drawn lines
 * @param lineStyle The style of the lines
 * @param useMarkers Flag used to toggle on markers at each point on the curve
 * @param markerSize The size of the marker
 * @param marker The marker to be used
 * @param useHiLow Flag used to turn error bars on
 * @param forCurve The index of the curve receiving the settings
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::XyPlotCurveSettings(const std::string &plotName,
      bool useLines, Integer lineWidth, Integer lineStyle, bool useMarkers,
      Integer markerSize, Integer marker, bool useHiLow, Integer forCurve)
{
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            frame->CurveSettings(useLines, lineWidth, lineStyle, useMarkers,
                  markerSize, marker, useHiLow, forCurve);
         }
   }
}

//------------------------------------------------------------------------------
// void SetXyPlotTitle(const std::string &plotName,
//       const std::string &plotTitle)
//------------------------------------------------------------------------------
/**
 * Sets the title for a plot
 *
 * @param plotName The name of the plot
 * @param plotTitle The new title for the plot
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::SetXyPlotTitle(const std::string &plotName,
                                   const std::string &plotTitle)
{
   #if DEBUG_PLOTIF_XY
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::SetXyPlotTitle() plotName = %s "
       "plotTitle = %s\n", plotName.c_str(), plotTitle.c_str());
   #endif

   MdiChildTsFrame *frame = NULL;
   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
         {
            #if DEBUG_PLOTIF_XY
               MessageInterface::ShowMessage
                  ("GuiPlotReceiver::SetXyPlotTitle() calling "
                   " frame->SetPlotTitle() \n");
            #endif

            frame->SetPlotTitle(wxString(plotTitle.c_str()));
         }
   }
}


//------------------------------------------------------------------------------
// void GuiPlotReceiver::ShowXyPlotLegend(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Turns on display of the plot legend
 *
 * @param plotName The name of the plot
 *
 * @note This method is not yet implemented
 */
//------------------------------------------------------------------------------
void GuiPlotReceiver::ShowXyPlotLegend(const std::string &plotName)
{
//   MdiChildTsFrame *frame = NULL;
//   for (int i=0; i<MdiTsPlot::numChildren; i++)
//   {
//      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
//
//      if (frame && !frame->ChildIsClosing())
//         if (frame->GetPlotName().IsSameAs(plotName.c_str()))
//         {
//            #if DEBUG_PLOTIF_XY
//               MessageInterface::ShowMessage
//                  ("GuiPlotReceiver::ShowXyPlotLegend() calling  frame->ShowPlotLegend() \n");
//            #endif
//
//            frame->ShowPlotLegend();
//         }
//   }
}


//------------------------------------------------------------------------------
// bool RefreshXyPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/*
 * Refreshes the XY plot.
 *
 * @param plotName name of xy plot
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::RefreshXyPlot(const std::string &plotName)
{
   if (GmatAppData::Instance()->GetMainFrame() != NULL)
   {
      #ifdef DEBUG_PLOTIF_XY_UPDATE
         MessageInterface::ShowMessage
            ("GuiPlotReceiver::RefreshXyPlot() plotName=%s, numChildren=%d\n",
             plotName.c_str(), MdiTsPlot::numChildren);
      #endif

      wxString owner = wxString(plotName.c_str());

      MdiChildTsFrame *frame = NULL;

      for (int i=0; i<MdiTsPlot::numChildren; i++)
      {
         frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
         if (frame && !frame->ChildIsClosing())
         {
            if (frame->GetPlotName().IsSameAs(owner.c_str()))
            {
               frame->RedrawCurve();
               #if __WXMAC__
                  frame->Refresh(true,NULL);
               #endif
            }
         }
      }
   }

   return true;
}


//------------------------------------------------------------------------------
// bool UpdateXyPlot(const std::string &plotName, const std::string &oldName,
//                   const Real &xval, const Rvector &yvals,
//                   const std::string &plotTitle,
//                   const std::string &xAxisTitle,
//                   const std::string &yAxisTitle, bool updateCanvas,
//                   bool drawGrid)
//------------------------------------------------------------------------------
/*
 * Updates a XyPlot window.
 *
 * @param plotName name of xy plot
 * @param oldName Former name of the plot, or an empty string
 * @param xval x value
 * @param yvals y values, should be in the order of curve added
 * @param plotTitle The plot's Title
 * @param xAxisTitle The plot's X axis title
 * @param yAxisTitle The plot's Y axis title
 * @param updateCanvas Flag indicating if the canvas should update immediately
 * @param drawGrid flag indicating if the grid should be drawn
 *
 * @return true if an update occurred, false otherwise
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::UpdateXyPlot(const std::string &plotName,
                                 const std::string &oldName,
                                 const Real &xval, const Rvector &yvals,
                                 const std::string &plotTitle,
                                 const std::string &xAxisTitle,
                                 const std::string &yAxisTitle,
                                 bool updateCanvas, bool drawGrid)
{
   bool updated = false;
   wxString owner = wxString(plotName.c_str());

   #ifdef DEBUG_PLOTIF_XY_UPDATE
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::UpdateXyPlot() xval = %f, numChildren = %d\n",
       xval, MdiTsPlot::numChildren);
   #endif

   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            int numCurves = frame->GetCurveCount();
            #ifdef DEBUG_PLOTIF_XY_UPDATE
               MessageInterface::ShowMessage
               ("GuiPlotReceiver::UpdateXyPlot() numCurves = %d\n", numCurves);
            #endif

            for (int j=0; j<numCurves; j++)
            {
               #ifdef DEBUG_PLOTIF_XY_UPDATE
                  MessageInterface::ShowMessage
                  ("GuiPlotReceiver::UpdateXyPlot() yvals[%d] = %f\n", j, yvals(j));
               #endif

               frame->AddDataPoints(j, xval, yvals(j));
            }

            if (updateCanvas)
               frame->RedrawCurve();

            updated = true;
         }
      }
   }

//   ::wxYield();

   return updated;
}

//------------------------------------------------------------------------------
// bool UpdateXyPlotData(const std::string &plotName, const Real &xval,
//       const Rvector &yvals, const Rvector *yhis, const Rvector *ylows)
//------------------------------------------------------------------------------
/**
 * Updates the data on a plot, passing in a set of y values for a given x, and
 * optionally the data used to draw error bars
 *
 * @param plotName The name of the plot receiving the data
 * @param xval The X value associated with the points
 * @param yvals The Y values associated with the points; these are assigned to
 *              the curves indexed in the order contained in the array
 * @param yhis +sigma error data for the error bars; NULL if not used
 * @param ylows -sigma error for the error bars; if NULL, the low error is
 *              assumed to have the same magnitude as the high error
 *
 * @return true if the data was processed, false if not
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::UpdateXyPlotData(const std::string &plotName,
      const Real &xval, const Rvector &yvals, const Rvector *yhis,
      const Rvector *ylows)
{
   bool updated = false;

   wxString owner = wxString(plotName.c_str());
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            int numCurves = frame->GetCurveCount();
            #ifdef DEBUG_PLOTIF_XY_UPDATE
               MessageInterface::ShowMessage
               ("GuiPlotReceiver::UpdateXyPlot() numCurves = %d\n", numCurves);
            #endif

            for (int j=0; j<numCurves; j++)
            {
               #ifdef DEBUG_PLOTIF_XY_UPDATE
                  MessageInterface::ShowMessage
                  ("GuiPlotReceiver::UpdateXyPlot() yvals[%d] = %f\n", j, yvals(j));
               #endif

               if (yhis != NULL)
               {
                  if (ylows != NULL)
                     frame->AddDataPoints(j, xval, yvals(j), (*yhis)(j),
                           (*ylows)(j));
                  else
                     frame->AddDataPoints(j, xval, yvals(j), (*yhis)(j));
               }
               else
                  frame->AddDataPoints(j, xval, yvals(j));
            }

            if (frame->IsActive())
            {
               frame->RedrawCurve();
            }
            updated = true;
         }
      }
   }

   return updated;
}


//------------------------------------------------------------------------------
// bool UpdateXyPlotCurve(const std::string &plotName, const Integer whichCurve,
//       const Real xval, const Real yval, const Real yhi, const Real ylow)
//------------------------------------------------------------------------------
/**
 * Adds a point to the plot data for a specific curve on a plot
 *
 * @param plotName The name of the plot receiving the data
 * @param whichCurve Index of the curve receiving the data
 * @param xval The X value associated with the point
 * @param yval The Y value associated with the points
 * @param yhi +sigma error data for the point's error bar; only used if hi > 0.0
 * @param ylow -sigma error for the point's error bar; if <= 0.0, the low error
 *            is assumed to have the same magnitude as the high error
 *
 * @return true if the data was processed, false if not
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::UpdateXyPlotCurve(const std::string &plotName,
                      const Integer whichCurve, const Real xval,
                      const Real yval, const Real yhi, const Real ylow)
{
   bool updated = false;

   if (whichCurve >= 0)
   {
      wxString owner = wxString(plotName.c_str());
      MdiChildTsFrame *frame = NULL;

      for (int i=0; i<MdiTsPlot::numChildren; i++)
      {
         frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

         if (frame && !frame->ChildIsClosing())
         {
            if (frame->GetPlotName().IsSameAs(owner.c_str()))
            {
               int numCurves = frame->GetCurveCount();
               #ifdef DEBUG_PLOTIF_XY_UPDATE
                  MessageInterface::ShowMessage
                        ("GuiPlotReceiver::UpdateXyPlotCurve() numCurves = %d\n",
                         numCurves);
               #endif

               if (whichCurve < numCurves)
                  frame->AddDataPoints(whichCurve, xval, yval, yhi, ylow);
               if (frame->IsActive())
                  frame->RedrawCurve();
               updated = true;
            }
         }
      }
   }

   return updated;
}


//------------------------------------------------------------------------------
// bool DeactivateXyPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Disables redrawing for a plot.  This method is used when a plot is receiving
 * a large amount of data all at once, so that the update performance doesn't
 * degrade.
 *
 * @param plotName The name of the plot receiving the data
 *
 * @return true is a plot received the message, false if not
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::DeactivateXyPlot(const std::string &plotName)
{
   bool deactivated = false;

   wxString owner = wxString(plotName.c_str());
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->IsActive(false);
            deactivated = true;
         }
      }
   }

   return deactivated;
}


//------------------------------------------------------------------------------
// bool ActivateXyPlot(const std::string &plotName)
//------------------------------------------------------------------------------
/**
 * Enables redrawing for a plot, and forces an immediate update.  This method is
 * used to redraw a plot after it has been disabled and  received a large amount
 * of data all at once.
 *
 * @param plotName The name of the plot receiving the data
 *
 * @return true is a plot received the message, false if not
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::ActivateXyPlot(const std::string &plotName)
{
   bool activated = false;

   wxString owner = wxString(plotName.c_str());
   MdiChildTsFrame *frame = NULL;

   for (int i=0; i<MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            frame->IsActive(true);
            frame->RedrawCurve();
            activated = true;
         }
      }
   }

   return activated;
}

//------------------------------------------------------------------------------
// bool GuiPlotReceiver::TakeXYAction(const std::string &plotName,
//                         const std::string &action)
//------------------------------------------------------------------------------
/**
 *
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
bool GuiPlotReceiver::TakeXYAction(const std::string &plotName,
                        const std::string &action)
{
   bool retval = false;
   wxString owner = wxString(plotName.c_str());

   MdiChildTsFrame *frame = NULL;

   for (int i = 0; i < MdiTsPlot::numChildren; i++)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            ((MdiChildTsFrame*)frame)->TakeAction(action);
            retval = true;
         }
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool CreateDynamicDataDisplay(const std::string &plotName, 
//                               const std::string &oldName,
//                               const std::string &plotTitle,
//                               Real positionX, Real positionY,
//                               Real width, Real height)
//------------------------------------------------------------------------------
/**
* Creates a DynamicDataDisplay window.
*
* @param plotName Name of the table
* @param oldName Former name of the table
* @param positionX, positionY  position of the table in screen coordinates
* @param w, h  size of the table in screen coordinates
* @param plotTitle Title of the table
*
* @return true on success, false when no table was created
*/
//------------------------------------------------------------------------------
bool GuiPlotReceiver::CreateDynamicDataDisplay(const std::string &plotName,
                             const std::string &oldName,
                             const std::string &plotTitle, Real positionX,
                             Real positionY, Real width, Real height)
{
   //-------------------------------------------------------
   // check if new MDI child frame needed
   //-------------------------------------------------------
   bool createNewFrame = true;
   wxString currTableName;
   MdiChildDynamicDataFrame *frame = NULL;

   for (int i = 0; i<MdiTsPlot::numChildren; i++)
   {
      frame =
         (MdiChildDynamicDataFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      currTableName = frame->GetPlotName();

      if (currTableName.IsSameAs(plotName.c_str()))
      {
         createNewFrame = false;
         break;
      }
      else if (currTableName.IsSameAs(oldName.c_str()))
      {
      #if DEBUG_RENAME
            MessageInterface::ShowMessage
               ("GuiPlotReceiver::CreateXyPlotWindow() currPlotName=%s, "
               "oldName=%s\n", currPlotName.c_str(), oldName.c_str());
      #endif

         // change plot name
         frame->SetPlotName(wxString(plotName.c_str()));
         createNewFrame = false;
         break;
      }
   }

   if (createNewFrame)
   {
      Integer x, y, w, h;
      bool isSizePreset = false;
      bool isUsingSaved = false;
      ComputePlotPositionAndSize(false, positionX, positionY, width, height, x,
         y, w, h, isUsingSaved);

      if (!GmatMathUtil::IsEqual(width, 0.0) || !GmatMathUtil::IsEqual(height, 0.0))
         isSizePreset = true;

      frame =
         new MdiChildDynamicDataFrame(GmatAppData::Instance()->GetMainFrame(),
         plotName, plotTitle, wxPoint(x, y), wxSize(w, h), isSizePreset,
         wxDEFAULT_FRAME_STYLE);

      if (GmatGlobal::Instance()->GetGuiMode() == GmatGlobal::MINIMIZED_GUI)
         frame->Show(false);
      else
         frame->Show(true);
      //frame->SetSavedConfigFlag(isUsingSaved);
      //frame->SetSaveLocationFlag(canSaveLocation);

      ++MdiTsPlot::numChildren;
   }

   return true;
}

//------------------------------------------------------------------------------
// bool SetDynamicDataTableSize(std::string &plotName, Integer maxRowCount,
//                              Integer maxColCount)
//------------------------------------------------------------------------------
/**
* Sets the number of rows and columns of the DynamicDataDisplay
*
* @param plotName Name of the table
* @param maxRowCount The number of rows required
* @param maxColCount The number of columns required
*
* @return true on success, false when the table was not sized
*/
//------------------------------------------------------------------------------
bool GuiPlotReceiver::SetDynamicDataTableSize(const std::string &plotName,
                                              Integer maxRowCount,
                                              Integer maxColCount)
{
   wxString owner = plotName;
   MdiChildDynamicDataFrame *frame = NULL;

   for (Integer i = 0; i < MdiTsPlot::numChildren; ++i)
   {
      frame =
         (MdiChildDynamicDataFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame)
      {
         if (frame->GetPlotName().IsSameAs(owner))
         {
            frame->SetTableSize(maxRowCount, maxColCount);
         }
      }
   }

   return true;
}

//------------------------------------------------------------------------------
// bool UpdateDynamicDataDisplay(std::string &plotName, StringARray &paramNames
//                               Rvector newData, Rvector rowIdxs)
//------------------------------------------------------------------------------
/**
* Updates the data being watched to their current values
*
* @param plotName Name of the table
* @param paramNames The names of the parameters being watched
* @param newData The new data used to update the parameter values being
*        displayed
* @param rowIdxs The row indices for each of the parameters, used to place
*        the parameters in the desired locations
*
* @return true on success, false when the table was not sized
*/
//------------------------------------------------------------------------------
bool GuiPlotReceiver::UpdateDynamicDataDisplay(const std::string &plotName,
                                               std::vector<std::vector<DDD>> newData)
{
   wxString owner = plotName;
   MdiChildDynamicDataFrame *frame = NULL;

   for (Integer i = 0; i < MdiTsPlot::numChildren; ++i)
   {
      frame =
         (MdiChildDynamicDataFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame)
      {
         if (frame->GetPlotName().IsSameAs(owner))
         {
            frame->UpdateDynamicData(newData);
         }
      }
   }

   return true;
}

//------------------------------------------------------------------------------
// bool DeleteDynamicData(const std::string &plotName,
//                        const std::string &oldname)
//------------------------------------------------------------------------------
/**
* Deletes the grid of the specified DynamicDataDisplay
*
* @param plotName Name of the table
* @param oldName Former name of the table
*
* @return true on success, false when the grid was not deleted
*/
//------------------------------------------------------------------------------
bool GuiPlotReceiver::DeleteDynamicData(const std::string &plotName,
                                        const std::string &oldName)
{
   wxString owner = plotName;
   MdiChildDynamicDataFrame *frame = NULL;

   for (Integer i = 0; i < MdiTsPlot::numChildren; ++i)
   {
      frame =
         (MdiChildDynamicDataFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame)
      {
         if (frame->GetPlotName().IsSameAs(owner))
            frame->DeleteDynamicDataGrid();
      }
   }

   return true;
}

bool GuiPlotReceiver::SetDynamicDataTextColor(const std::string &plotName,
                                              std::vector<std::vector<DDD>> newColors)
{
   wxString owner = plotName;
   MdiChildDynamicDataFrame *frame = NULL;

   for (Integer i = 0; i < MdiTsPlot::numChildren; ++i)
   {
      frame = (MdiChildDynamicDataFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame)
      {
         if (frame->GetPlotName().IsSameAs(owner))
         {
            frame->SetDynamicDataCellTextColor(newColors);
         }
      }
   }

   return true;
}

//------------------------------------------------------------------------------
// bool ComputePlotPositionAndSize(bool isGLPlot, Integer &x, Integer &y, Integer &w, ...)
//------------------------------------------------------------------------------
bool GuiPlotReceiver::ComputePlotPositionAndSize(bool isGLPlot, Real positionX,
                                                 Real positionY, Real width, Real height,
                                                 Integer &x, Integer &y, Integer &w, Integer &h,
                                                 bool usingSaved)
{
   #ifdef DEBUG_PLOT_PERSISTENCE
   MessageInterface::ShowMessage
      ("ComputePlotPositionAndSize() entered, %s, usingSaved = %d\n   positionX = %12.10f, "
       "positionY = %12.10f, width = %12.10f, height = %12.10f\n", isGLPlot ? "GLPlot" : "XYPlot",
       usingSaved, positionX, positionY, width, height);
   #endif
   
   if (GmatGlobal::Instance()->GetPlotMode() == GmatGlobal::TILED_PLOT)
   {
      x = -1;
      y = -1;
      w = -1;
      h = -1;
      return false;
   }
   
   Integer plotCount = MdiGlPlot::numChildren + MdiTsPlot::numChildren;
   bool isPresetSizeUsed = false;
   
   Integer screenWidth = 0;
   Integer screenHeight = 0;
   
   #ifdef __WXMAC__
      screenWidth  = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
      screenHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
   #else
      GmatMainFrame *mainFrame = GmatAppData::Instance()->GetMainFrame();
      Integer missionTreeX = 0;
      Integer missionTreeY = 0;
      Integer missionTreeW = 0;
      mainFrame->GetActualClientSize(&screenWidth, &screenHeight, true);
   #endif
   
   #ifdef DEBUG_PLOT_PERSISTENCE
   MessageInterface::ShowMessage("   screen size  : w = %4d, h = %4d\n", screenWidth, screenHeight);
   #ifndef __WXMAC__
      MessageInterface::ShowMessage("   mission tree : w = %4d\n", missionTreeW);
   #endif
   #endif
   
   // if position and size were not saved from an earlier run, figure out the initial values
   if (GmatMathUtil::IsEqual(positionX,0.0) && GmatMathUtil::IsEqual(positionY,0.0) &&
       GmatMathUtil::IsEqual(width,0.0)     && GmatMathUtil::IsEqual(height,0.0))
   {
      
      #ifdef DEBUG_PLOT_PERSISTENCE
      MessageInterface::ShowMessage("   ==> position and size are zero\n");
      #endif
      
      if (MdiGlPlot::usePresetSize || MdiTsPlot::usePresetSize)
         isPresetSizeUsed = true;
      
      usingSaved = false;
      #ifdef __WXMAC__
         wxSize size = wxGetDisplaySize();
         w = (size.GetWidth() - 239) / 2;
         h = 350;
         Integer hLoc = plotCount % 2;
         Integer vLoc = (Integer)((plotCount) / 2);
         x = 238 + hLoc * w + 1;
         y = 20  + vLoc * (h+10);
      #else
         // If MissionTree is undocked, subtract its width before computing w
         if (mainFrame->IsMissionTreeUndocked(missionTreeX, missionTreeY, missionTreeW))
            screenWidth -= missionTreeW;
         
         // Get the count of plots active for this run
         Integer activePlotCount = mainFrame->GetNumberOfActivePlots();
         int newCount = plotCount + 1;
         
         // compute plot size depends on number of active plots
         if (activePlotCount == 1)
         {
            w = (Integer)((Real)screenWidth * 0.8);
            h = (Integer)((Real)screenHeight * 0.85);
         }
         else
         {
            w = (Integer)((Real)screenWidth * 0.5);
            h = (Integer)((Real)screenHeight * 0.45);
         }
         
         // compute plot position
         if (newCount == 1)
         {
            x = missionTreeW + 5;
            y = 0;
         }
         else if (newCount == 2)
         {
            x = missionTreeW + 5;
            y = h * (newCount - 1) + (newCount - 1) * 5;
         }
         else
         {
            x = missionTreeW + newCount * 20;               
            y = h + 5 + (newCount - 1) * 20;
         }
         
      #endif
   }
   else
   {
      #ifdef DEBUG_PLOT_PERSISTENCE
      MessageInterface::ShowMessage("   ==> position and size are non-zero\n");
      #endif
      
      if (isGLPlot)
         MdiGlPlot::usePresetSize = true;
      else
         MdiTsPlot::usePresetSize = true;
      
      isPresetSizeUsed = true;
      usingSaved       = true;
      
      // @note
      // Since position and size are truncated to whole integer,
      // actual plot position and size can differ a pixel or so for
      // a consecutive run after plots are closed.
      x = (Integer) (positionX * (Real) screenWidth);
      y = (Integer) (positionY * (Real) screenHeight);
      w = (Integer) (width     * (Real) screenWidth);
      h = (Integer) (height    * (Real) screenHeight);
   }
   
   #ifdef DEBUG_PLOT_PERSISTENCE
   MessageInterface::ShowMessage("   computed     : x = %4d, y = %4d, w = %4d, h = %4d\n", x, y, w, h);
   MessageInterface::ShowMessage
      ("ComputePlotPositionAndSize() returning isPresetSizeUsed = %d\n", isPresetSizeUsed);
   #endif
   
   return isPresetSizeUsed;
}

// Ground track window methods

bool GuiPlotReceiver::CreateGroundTrackWindow(const std::string &trackName,
                           const std::string &oldName,
                           const std::string &title,
                           Real positionX, Real positionY,
                           Real width, Real height, bool isMaximized)
{
   //-------------------------------------------------------
   // check if new MDI child frame needed
   //-------------------------------------------------------
   bool createNewFrame = true;
   wxString currPlotName;
   GmatMdiChildFrame *frame = NULL;

   for (int i=0; i < MdiTsPlot::numChildren; ++i)
   {
      frame = (MdiChildTsFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());
      currPlotName = frame->GetPlotName();

      if (currPlotName.IsSameAs(trackName.c_str()))
      {
         if (!frame->ChildIsClosing())
            createNewFrame = false;
         break;
      }
      else if (currPlotName.IsSameAs(oldName.c_str()))
      {
         #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("GuiPlotReceiver::CreateGroundTrackWindow() currPlotName=%s, "
                  "oldName=%s\n", currPlotName.c_str(), oldName.c_str());
         #endif

         // change plot name
         frame->SetPlotName(wxString(trackName.c_str()));
         if (!frame->ChildIsClosing())
            createNewFrame = false;
         break;
      }
   }

   //-------------------------------------------------------
   // create MDI child GroundTrackWindow
   //-------------------------------------------------------
   if (createNewFrame)
   {
      #if DEBUG_GROUNDTRACKWINDOW_CREATION
      MessageInterface::ShowMessage
         ("GuiPlotReceiver::CreateGroundTrackWindow() Creating new MdiChildFrame\n");
      #endif

      Integer x, y, w, h;
      Integer plotCount = MdiGlPlot::numChildren + MdiTsPlot::numChildren;
      bool isUsingSaved = false;

      #ifdef DEBUG_GROUNDTRACKWINDOW
      MessageInterface::ShowMessage
         ("trackName = '%s', plotCount = %d\n", trackName.c_str(), plotCount);
      #endif

      ComputePlotPositionAndSize(false, positionX, positionY,
                                 width, height, x, y, w, h,
                                 isUsingSaved);
      #ifdef DEBUG_GROUNDTRACKWINDOW
      MessageInterface::ShowMessage
         ("About to create an XYPlot at position x = %d, y = %d, and size w = %d, h = %d\n", x, y, w, h);
      #endif

      // Create the ground track window
      frame =
         new GroundTrackWindow(GmatAppData::Instance()->GetMainFrame(),
                             wxString(trackName.c_str()),
                             wxString(title.c_str()),
                             GmatTree::GROUND_TRACK_AREA,
                             wxPoint(x, y), wxSize(w, h),
                             wxDEFAULT_FRAME_STYLE);

      frame->SetSavedConfigFlag(isUsingSaved);
// -->>     frame->SetIsNewFrame(true);

      if (GmatGlobal::Instance()->GetGuiMode() == GmatGlobal::MINIMIZED_GUI)
         frame->Show(false);
      else
         frame->Show(true);

      #if __WXMAC__
         frame->SetSize(w-1, h-1);
      #else
         if (w != -1 && h != -1)
         {
            // Why not showing the plot without resetting the size?
            // Used SetSize with wxSIZE_NO_ADJUSTMENTS (LOJ: 2012.01.05)
            frame->SetSize(w-1, h-1);
            //frame->SetSize(w+1, h+1);
            frame->SetSize(x, y, w+1, h+1, wxSIZE_NO_ADJUSTMENTS);
         }
      #endif

      plotCount = MdiGlPlot::numChildren + MdiTsPlot::numChildren;

      if (GmatGlobal::Instance()->GetPlotMode() == GmatGlobal::TILED_PLOT)
      {
         // Set wxVERTICAL or wxHORIZONTAL based on main frame width and height
         // if run mode is TESTING (LOJ: 2015.07.23)
         wxOrientation tileMode = wxVERTICAL;
         //if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)          // made changes by TUAN NGUYEN
         if (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING)     // made changes by TUAN NGUYEN
         {
            int w, h;
            GmatAppData::Instance()->GetMainFrame()->GetActualClientSize(&w, &h, false);
            if (w < h)
               tileMode = wxHORIZONTAL;
         }
         GmatAppData::Instance()->GetMainFrame()->Tile(tileMode);
      }
   }
//   else
//      frame->SetIsNewFrame(false);

   #if DEBUG_GROUNDTRACKWINDOW
   MessageInterface::ShowMessage
      ("GuiPlotReceiver::CreateGroundTrackWindow() leaving\n");
   #endif

   return true;
}

void GuiPlotReceiver::SetGroundTrackOption(const std::string &plotName,
                        const std::string &optionSetting,
                        const std::string &optionValue)
{
   wxString owner = wxString(plotName.c_str());
   GmatMdiChildFrame *frame = NULL;

   for (int i = 0; i < MdiTsPlot::numChildren; ++i)
   {
      frame = (GmatMdiChildFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
            ((GroundTrackWindow*)frame)->SetOption(optionSetting, optionValue);
      }
   }
}

bool GuiPlotReceiver::UpdateGroundTrackData(const std::string &plotName,
                        const double epoch, const double *longlat,
                        const int satcount)
{
   bool updated = false;

   wxString owner = wxString(plotName.c_str());
   GmatMdiChildFrame *frame = NULL;

   for (int i = 0; i < MdiTsPlot::numChildren; ++i)
   {
      frame = (GmatMdiChildFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
         {
            ((GroundTrackWindow*)frame)->AddData(epoch, longlat, satcount);
            updated = true;
         }
      }
   }

   return updated;
}

bool GuiPlotReceiver::TakeGroundTrackAction(const std::string &plotName,
                     const std::string &action)
{
   bool retval = false;

   wxString owner = wxString(plotName.c_str());
   GmatMdiChildFrame *frame = NULL;

   for (int i = 0; i < MdiTsPlot::numChildren; ++i)
   {
      frame = (GmatMdiChildFrame*)(MdiTsPlot::mdiChildren.Item(i)->GetData());

      if (frame && !frame->ChildIsClosing())
      {
         if (frame->GetPlotName().IsSameAs(owner.c_str()))
            retval = ((GroundTrackWindow*)frame)->TakeAction(action);
      }
   }

   return retval;
}
