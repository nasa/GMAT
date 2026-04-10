//$Id$
//------------------------------------------------------------------------------
//                              MdiChildViewFrame
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
// Author: Linda Jun
// Created: 2010/04/19
/**
 * Implements MdiChildViewFrame for 3D visualization.
 */
//------------------------------------------------------------------------------
#include "gmatwxrcs.hpp"
#include "Subscriber.hpp"
#include "SubscriberException.hpp"
#include "MdiChildViewFrame.hpp"
#include "MdiGlPlotData.hpp"
#include "ViewCanvas.hpp"
#include "OrbitViewCanvas.hpp"
#include "CelesBodySelectDialog.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"      // for namespace GmatMenu
#include "ColorTypes.hpp"         // for namespace GmatColor::
#include "MessageInterface.hpp"


BEGIN_EVENT_TABLE(MdiChildViewFrame, GmatMdiChildFrame)
   EVT_ACTIVATE(MdiChildViewFrame::OnActivate)
   EVT_SIZE(MdiChildViewFrame::OnPlotSize)
   EVT_MOVE(MdiChildViewFrame::OnMove)
   EVT_CLOSE(MdiChildViewFrame::OnPlotClose) 
END_EVENT_TABLE()

//#define DEBUG_VIEW_FRAME
//#define DEBUG_MDI_CHILD_FRAME_CLOSE
//#define DEBUG_PLOT_PERSISTENCY

//------------------------------------------------------------------------------
// MdiChildViewFrame(wxMDIParentFrame *parent, const wxString& title, ...)
//------------------------------------------------------------------------------
MdiChildViewFrame::MdiChildViewFrame(wxMDIParentFrame *parent,
                                     const wxString& plotName, const wxString& title,
                                     const wxPoint& pos, const wxSize& size,
                                     const long style, const wxString &viewType,
                                     const GmatTree::ItemType itemType)
   : GmatMdiChildFrame(parent, plotName, title, itemType, -1,
                       pos, size, style | wxNO_FULL_REPAINT_ON_RESIZE)
{
   mCanvas = (ViewCanvas *) NULL;
   mPlotTitle = plotName;
   mViewType = viewType;
   mOverlapPlot = false;
   mInFunction = false;
   
   // add Earth as default body
   mBodyNames.Add("Earth");
   mBodyColors.push_back(GmatColor::GREEN);
   
   MdiGlPlot::mdiChildren.Append(this);
   
   // Give it an icon
#ifdef __WXMSW__
   SetIcon(wxIcon(_T("chrt_icn")));
#else
   SetIcon(wxIcon( mondrian_xpm ));
#endif
   
}


//------------------------------------------------------------------------------
// ~MdiChildViewFrame()
//------------------------------------------------------------------------------
MdiChildViewFrame::~MdiChildViewFrame()
{   
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("~MdiChildViewFrame() mChildName=%s\n", mChildName.c_str());
   #endif
   
   //MdiGlPlot::mdiChildren.DeleteObject(this);
   //MdiGlPlot::numChildren--;
   
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("~MdiChildViewFrame() exiting MdiGlPlot::numChildren=%d\n",
       MdiGlPlot::numChildren);
   #endif
}



//------------------------------------------------------------------------------
// bool GetOverlapPlot()
//------------------------------------------------------------------------------
bool MdiChildViewFrame::GetOverlapPlot()
{
   return mOverlapPlot;
}


//------------------------------------------------------------------------------
// ViewCanvas* GetGlCanvas()
//------------------------------------------------------------------------------
ViewCanvas* MdiChildViewFrame::GetGlCanvas()
{
   return mCanvas;
}


//------------------------------------------------------------------------------
// bool GetUseViewPointInfo()
//------------------------------------------------------------------------------
bool MdiChildViewFrame::GetUseViewPointInfo()
{
   if (mCanvas)
      return mCanvas->GetUseInitialViewDef();
   
   return false;
}


//------------------------------------------------------------------------------
// bool GetDrawWireFrame()
//------------------------------------------------------------------------------
bool MdiChildViewFrame::GetDrawWireFrame()
{
   if (mCanvas)
      return mCanvas->GetDrawWireFrame();

   return false;
}


//------------------------------------------------------------------------------
// bool GetDrawXyPlane()
//------------------------------------------------------------------------------
bool MdiChildViewFrame::GetDrawXyPlane()
{
   if (mCanvas && mViewType == "3D")
      return ((OrbitViewCanvas*)mCanvas)->GetDrawXyPlane();
   
   return false;
}


//------------------------------------------------------------------------------
// bool GetDrawEcPlane()
//------------------------------------------------------------------------------
bool MdiChildViewFrame::GetDrawEcPlane()
{
   if (mCanvas && mViewType == "3D")
      return ((OrbitViewCanvas*)mCanvas)->GetDrawEcPlane();

   return false;
}


//------------------------------------------------------------------------------
// bool GetDrawSunLine()
//------------------------------------------------------------------------------
bool MdiChildViewFrame::GetDrawSunLine()
{
   if (mCanvas && mViewType == "3D")
      return ((OrbitViewCanvas*)mCanvas)->GetDrawSunLine();

   return false;
}


//------------------------------------------------------------------------------
// bool GetDrawAxes()
//------------------------------------------------------------------------------
bool MdiChildViewFrame::GetDrawAxes()
{
   if (mCanvas && mViewType == "3D")
      return ((OrbitViewCanvas*)mCanvas)->GetDrawAxes();

   return false;
}

//------------------------------------------------------------------------------
// bool GetDrawGrid()
//------------------------------------------------------------------------------
bool MdiChildViewFrame::GetDrawGrid()
{
   if (mCanvas && mViewType == "3D")
      return ((OrbitViewCanvas*)mCanvas)->GetDrawGrid();

   return false;
}


//------------------------------------------------------------------------------
// UnsignedInt GetXyPlaneColor()
//------------------------------------------------------------------------------
UnsignedInt MdiChildViewFrame::GetXyPlaneColor()
{
   if (mCanvas && mViewType == "3D")
      return ((OrbitViewCanvas*)mCanvas)->GetXyPlaneColor();

   return 0;
}


//------------------------------------------------------------------------------
// UnsignedInt GetSunLineColor()
//------------------------------------------------------------------------------
UnsignedInt MdiChildViewFrame::GetSunLineColor()
{
   if (mCanvas && mViewType == "3D")
      return ((OrbitViewCanvas*)mCanvas)->GetSunLineColor();

   return 0;
}

//------------------------------------------------------------------------------
// bool IsAnimationRunning()
//------------------------------------------------------------------------------
bool MdiChildViewFrame::IsAnimationRunning()
{
   if (mCanvas)
      return mCanvas->IsAnimationRunning();
   
   return false;
}


//------------------------------------------------------------------------------
// Integer GetAnimationUpdateInterval()
//------------------------------------------------------------------------------
Integer MdiChildViewFrame::GetAnimationUpdateInterval()
{
   if (mCanvas)
      return mCanvas->GetAnimationUpdateInterval();

   return 0;
}


//------------------------------------------------------------------------------
// Integer GetAnimationFrameIncrement()
//------------------------------------------------------------------------------
Integer MdiChildViewFrame::GetAnimationFrameIncrement()
{
   if (mCanvas)
      return mCanvas->GetAnimationFrameIncrement();

   return 0;
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetSolarSystem(SolarSystem *ss)
{
   if (mCanvas)
      mCanvas->SetSolarSystem(ss);
}


//------------------------------------------------------------------------------
// void SetUserInterrupt()
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetUserInterrupt()
{
   if (mCanvas)
      mCanvas->SetUserInterrupt();
}


//------------------------------------------------------------------------------
// void SetPlotName(const wxString &name)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetPlotName(const wxString &name)
{
   #ifdef DEBUG_VIEW_FRAME
      MessageInterface::ShowMessage
         ("MdiChildViewFrame::SetPlotName() name=%s\n", name.c_str());
   #endif
   
   GmatMdiChildFrame::SetPlotName(name);
   mPlotTitle = name;
   SetTitle(mPlotTitle);
}


//------------------------------------------------------------------------------
// void ResetShowViewOption()
//------------------------------------------------------------------------------
void MdiChildViewFrame::ResetShowViewOption()
{
   #ifdef DEBUG_VIEW_FRAME
   MessageInterface::ShowMessage
      ("MdiChildViewFrame::ResetShowViewOption()\n");
   #endif

    theMenuBar->Check(GmatPlot::MDI_GL_SHOW_OPTION_PANEL, false);
}


//------------------------------------------------------------------------------
// void SetOverlapPlot(bool overlap)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetOverlapPlot(bool overlap)
{
   mOverlapPlot = overlap;
   
   if (mCanvas)
   {
      if (!overlap)
         mCanvas->ClearPlot();
   }
}


//------------------------------------------------------------------------------
// void SetUseInitialViewDef(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetUseInitialViewDef(bool flag)
{
   if (mCanvas)
      mCanvas->SetUseInitialViewDef(flag);
}


//------------------------------------------------------------------------------
// void SetAnimationUpdateInterval(nt value)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetAnimationUpdateInterval(int value)
{
   if (mCanvas)
      mCanvas->SetAnimationUpdateInterval(value);
}


//------------------------------------------------------------------------------
// void SetAnimationFrameIncrement(nt val)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetAnimationFrameIncrement(int value)
{
   if (mCanvas)
      mCanvas->SetAnimationFrameIncrement(value);
}


//------------------------------------------------------------------------------
// void SetDrawWireFrame(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetDrawWireFrame(bool flag)
{
   if (mCanvas)
   {
      theMenuBar->Check(GmatPlot::MDI_GL_SHOW_WIRE_FRAME, flag);
      mCanvas->SetDrawWireFrame(flag);
   }
}

//------------------------------------------------------------------------------
// void SetDrawStars(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetDrawStars(bool flag)
{
   if (mCanvas && mViewType == "3D")
   {
      ((OrbitViewCanvas*)mCanvas)->SetDrawStars(flag);
   }
}

//------------------------------------------------------------------------------
// void SetDrawConstellations(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetDrawConstellations(bool flag)
{
   if (mCanvas && mViewType == "3D")
   {
      ((OrbitViewCanvas*)mCanvas)->SetDrawConstellations(flag);
   }
}

//------------------------------------------------------------------------------
// void SetStarCount(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetStarCount(int count)
{
   if (mCanvas && mViewType == "3D")
   {
      ((OrbitViewCanvas*)mCanvas)->SetStarCount(count);
   }
}

//------------------------------------------------------------------------------
// void SetDrawXyPlane(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetDrawXyPlane(bool flag)
{
   if (mCanvas && mViewType == "3D")
   {
      theMenuBar->Check(GmatPlot::MDI_GL_SHOW_EQUATORIAL_PLANE, flag);   
      ((OrbitViewCanvas*)mCanvas)->SetDrawXyPlane(flag);
   }
}


//------------------------------------------------------------------------------
// void SetDrawEcPlane(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetDrawEcPlane(bool flag)
{
   if (mCanvas && mViewType == "3D")
      ((OrbitViewCanvas*)mCanvas)->SetDrawEcPlane(flag);
}


//------------------------------------------------------------------------------
// void SetDrawSunLine(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetDrawSunLine(bool flag)
{
   if (mCanvas && mViewType == "3D")
      ((OrbitViewCanvas*)mCanvas)->SetDrawSunLine(flag);
}


//------------------------------------------------------------------------------
// void SetDrawAxes(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetDrawAxes(bool flag)
{
   if (mCanvas && mViewType == "3D")
      ((OrbitViewCanvas*)mCanvas)->SetDrawAxes(flag);
}


//------------------------------------------------------------------------------
// void SetDrawGrid(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetDrawGrid(bool flag)
{
   if (mCanvas && mViewType == "3D")
      ((OrbitViewCanvas*)mCanvas)->SetDrawGrid(flag);
}


//------------------------------------------------------------------------------
// void SetIsNewFrame(bool flag)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetIsNewFrame(bool flag)
{
   if (mCanvas && mViewType == "3D")
      ((OrbitViewCanvas*)mCanvas)->SetIsNewFrame(flag);
}


//------------------------------------------------------------------------------
// void SetXyPlaneColor(UnsignedInt color)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetXyPlaneColor(UnsignedInt color)
{
   if (mCanvas && mViewType == "3D")
      ((OrbitViewCanvas*)mCanvas)->SetXyPlaneColor(color);
}


//------------------------------------------------------------------------------
// void SetSunLineColor(UnsignedInt color)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetSunLineColor(UnsignedInt color)
{
   if (mCanvas && mViewType == "3D")
      ((OrbitViewCanvas*)mCanvas)->SetSunLineColor(color);
}


//------------------------------------------------------------------------------
// void SetNumPointsToRedraw(Integer numPoints)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetNumPointsToRedraw(Integer numPoints)
{
   if (mCanvas)
      mCanvas->SetNumPointsToRedraw(numPoints);
}

#if 0
//------------------------------------------------------------------------------
// void SetObjectColors(const wxStringColorMap &objectColorMap)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetObjectColors(const wxStringColorMap &objectColorMap)
{
   if (mCanvas)
      mCanvas->SetObjectColors(objectColorMap);
}
#endif

//------------------------------------------------------------------------------
// void SetShowObjects(const wxStringBoolMap &showObjMap)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetShowObjects(const wxStringBoolMap &showObjMap)
{
   if (mCanvas)
      mCanvas->SetShowObjects(showObjMap);
}


//------------------------------------------------------------------------------
// void DrawInOtherCoordSystem(const wxString &csName)
//------------------------------------------------------------------------------
void MdiChildViewFrame::DrawInOtherCoordSystem(const wxString &csName)
{
   if (mCanvas)
      mCanvas->DrawInOtherCoordSystem(csName);
}


//------------------------------------------------------------------------------
// void RedrawPlot(bool viewAnimation)
//------------------------------------------------------------------------------
void MdiChildViewFrame::RedrawPlot(bool viewAnimation)
{
   #ifdef DEBUG_VIEW_FRAME
   MessageInterface::ShowMessage("MdiChildViewFrame::RedrawPlot() entered.\n");
   #endif
   
   if (mCanvas)
      mCanvas->RedrawPlot(viewAnimation);
}


// menu actions
//------------------------------------------------------------------------------
// void OnClearPlot(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnClearPlot(wxCommandEvent& WXUNUSED(event))
{
   if (mCanvas)
      mCanvas->ClearPlot();
}


//------------------------------------------------------------------------------
// void OnChangeTitle(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnChangeTitle(wxCommandEvent& WXUNUSED(event))
{
   wxString title = wxGetTextFromUser(_T("Enter the new title for MDI child"),
                                      _T(""),
                                      mPlotTitle, //s_title,
                                      GetParent()->GetParent());
   if ( !title )
      return;

   mPlotTitle = title;
   SetTitle(title);
}


//------------------------------------------------------------------------------
// void OnShowDefaultView(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnShowDefaultView(wxCommandEvent& event)
{
   if (mCanvas)
      mCanvas->ShowDefaultView();
}


//------------------------------------------------------------------------------
// void OnDrawWireFrame(wxCommandEvent& event)
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnDrawWireFrame(wxCommandEvent& event)
{
   if (mCanvas)
      mCanvas->DrawWireFrame(event.IsChecked());
}


//------------------------------------------------------------------------------
// void OnDrawXyPlane(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnDrawXyPlane(wxCommandEvent& event)
{
   if (mCanvas && mViewType == "3D")
      ((OrbitViewCanvas*)mCanvas)->DrawXyPlane(event.IsChecked());
}


//------------------------------------------------------------------------------
// void OnQuit(wxCommandEvent& WXUNUSED(event))
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
   Close(TRUE);
}


//------------------------------------------------------------------------------
// void OnActivate(wxActivateEvent& event)
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnActivate(wxActivateEvent& event)
{
   if ( event.GetActive() && mCanvas )
      mCanvas->SetFocus();
   
   GmatMdiChildFrame::OnActivate(event);
}


//------------------------------------------------------------------------------
// void OnPlotSize(wxSizeEvent& event)
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnPlotSize(wxSizeEvent& event)
{
   // VZ: under MSW the size event carries the client size (quite
   //     unexpectedly) *except* for the very first one which has the full
   //     size... what should it really be? TODO: check under wxGTK
   
   //wxSize size1 = event.GetSize();
   //wxSize size2 = GetSize();
   //wxSize size3 = GetClientSize();

   //wxLogStatus(GmatAppData::Instance()->GetMainFrame(),
   //            wxT("size from event: %dx%d, from frame %dx%d, client %dx%d"),
   //            size1.x, size1.y, size2.x, size2.y, size3.x, size3.y);
   
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnMove(wxMoveEvent& event)
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnMove(wxMoveEvent& event)
{
   // Refresh canvas when frame moves (LOJ: 2011.09.16)
   // Implemented here so that when user moves scroll bar the plot will be repainted
   // Without this, OrbitView or GroundTrack plot shows only white background.
   if ( mCanvas )
   {
      // Do not use Refresh, it makes flickering
      mCanvas->Refresh(false);
      mCanvas->Update();
   }
   
   // VZ: here everything is totally wrong under MSW, the positions are
   //     different and both wrong (pos2 is off by 2 pixels for me which seems
   //     to be the width of the MDI canvas border)
   
   //wxPoint pos1 = event.GetPosition();
   //wxPoint pos2 = GetPosition();
   
   //wxLogStatus(GmatAppData::Instance()->GetMainFrame(),
   //            wxT("position from event: (%d, %d), from frame (%d, %d)"),
   //            pos1.x, pos1.y, pos2.x, pos2.y);
   
   event.Skip();
}


//------------------------------------------------------------------------------
// void OnPlotClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnPlotClose(wxCloseEvent &event)
{
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("MdiChildViewFrame::OnPlotClose() '%s' entered\n", mChildName.c_str());
   #endif
   
   CheckFrame();
   
   if (mCanClose)
   {
      // Remove this from the list of GmatMainFrame mdi children but do not delete,
      // OnClose() calls destructor whiere it can be removed from the plot list and deleted.
      GmatAppData::Instance()->GetMainFrame()->RemoveChild(GetName(), mItemType, false);   
      event.Skip();
   }
   else
   {
      int answer =
         wxMessageBox(wxString("GMAT is running the the animation.\n"
                               "Are you sure you want to stop the animation and close?"),
                      wxT("GMAT Warning"), wxYES_NO);
      
      if (answer == wxYES)
      {
         #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
         MessageInterface::ShowMessage
            ("   Stopping animation and removing this from the list of GmatMainFrame mdi children\n");
         #endif
         
         mCanvas->SetUserInterrupt();
         mCanClose = true;
         // Remove this from the list of GmatMainFrame mdi children but do not delete,
         // OnClose() calls destructor whiere it can be removed from the plot list and deleted.
         GmatAppData::Instance()->GetMainFrame()->RemoveChild(GetName(), mItemType, false);
         event.Skip();
      }
      else
         event.Veto();
   }
   
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("MdiChildViewFrame::OnPlotClose() '%s' exiting\n", mChildName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void OnClose(wxCloseEvent &event)
//------------------------------------------------------------------------------
void MdiChildViewFrame::OnClose(wxCloseEvent &event)
{
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("\nMdiChildViewFrame::OnClose() '%s' entered, mCanClose=%d\n", mChildName.c_str(), mCanClose);
   #endif
   
   GmatMdiChildFrame::OnClose(event);
   event.Skip();

   MdiGlPlot::mdiChildren.DeleteObject(this);
   MdiGlPlot::numChildren--;
   
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("MdiChildViewFrame::OnClose() '%s' exiting, mCanClose=%d\n", mChildName.c_str(), mCanClose);
   #endif
}


//------------------------------------------------------------------------------
// void SetGlObject(const StringArray &objNames, ...
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetGlObject(const StringArray &objNames,
                                    //const UnsignedIntArray &objOrbitColors,
                                    const std::vector<SpacePoint*> &objArray)
{
   if (mCanvas)
      //mCanvas->SetGlObject(objNames, objOrbitColors, objArray);
      mCanvas->SetGlObject(objNames, objArray);
}


//------------------------------------------------------------------------------
// void SetGlObject(CoordinateSystem *viewCs, ...
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetGlCoordSystem(CoordinateSystem *internalCs,
                                         CoordinateSystem *viewCs,
                                         CoordinateSystem *viewUpCs)
{
   if (mCanvas)
   {         
      mCanvas->SetGlCoordSystem(internalCs, viewCs, viewUpCs);
   }
}


//------------------------------------------------------------------------------
// void SetGl2dDrawingOption(Integer footPrintOption)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetGl2dDrawingOption(const std::string &centralBodyName,
                                             const std::string &textureMap,
                                             Integer footPrintOption)
{
   // do nothing here
   // derived class should orvrride this
}


//------------------------------------------------------------------------------
// void SetGl3dDrawingOption(bool showLabels, bool drawEcPlane, ...)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetGl3dDrawingOption(bool showLabels, bool drawEcPlane,
                                             bool drawXyPlane, bool drawWireFrame,
                                             bool drawAxes, bool drawGrid, bool drawSunLine,
                                             bool overlapPlot, bool usevpInfo,
                                             bool drawStars, bool drawConstellations,
                                             Integer starCount)
{
   // do nothing here
   // derived class should orvrride this
}


//------------------------------------------------------------------------------
// void SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj, ...
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetGl3dViewOption(SpacePoint *vpRefObj, SpacePoint *vpVecObj,
                                          SpacePoint *vdObj, Real vsFactor,
                                          const Rvector3 &vpRefVec, const Rvector3 &vpVec,
                                          const Rvector3 &vdVec, const std::string &upAxis,
                                          bool usevpRefVec, bool usevpVec, bool usevdVec)
{
   // do nothing here
   // derived class should orvrride this
}


//------------------------------------------------------------------------------
// void SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetGlDrawOrbitFlag(const std::vector<bool> &drawArray)
{
   if (mCanvas)
      mCanvas->SetGlDrawOrbitFlag(drawArray);
}


//------------------------------------------------------------------------------
// void SetGlShowObjectFlag(const std::vector<bool> &showArray)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetGlShowObjectFlag(const std::vector<bool> &showArray)
{
   if (mCanvas)
      mCanvas->SetGlShowObjectFlag(showArray);
}


//------------------------------------------------------------------------------
// void SetGlUpdateFrequency(Integer updFreq)
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetGlUpdateFrequency(Integer updFreq)
{
   if (mCanvas)
      mCanvas->SetUpdateFrequency(updFreq);
}


//------------------------------------------------------------------------------
// void UpdatePlot(const StringArray &scNames, ...
//------------------------------------------------------------------------------
void MdiChildViewFrame::UpdatePlot(const StringArray &scNames, const Real &time,
                                   const RealArray &posX, const RealArray &posY,
                                   const RealArray &posZ, const RealArray &velX,
                                   const RealArray &velY, const RealArray &velZ,
                                   //const UnsignedIntArray &scColors,
                                   const ColorMap &orbitColorMap,
                                   const ColorMap &targetColorMap,
                                   bool solving, Integer solverOption, bool updateCanvas,
                                   bool drawing, bool inFunction)
{
   #ifdef DEBUG_UPDATE_PLOT
   MessageInterface::ShowMessage
      ("MdiChildViewFrame::UpdatePlot() entered, time=%f, updateCanvas=%d\n",
       time, updateCanvas);
   #endif
   
   if (mCanvas)
   {
      mCanvas->UpdatePlot(scNames, time, posX, posY, posZ, velX, velY, velZ,
                          //scColors, solving, solverOption, drawing, inFunction);
                          orbitColorMap, targetColorMap,solving, solverOption,
                          drawing, inFunction);
      
      mInFunction = inFunction;
      
      if (updateCanvas)
      {
         //-----------------------------------------------------------
         // Notes:
         // Added mCanvas->Refresh(false) here since Refresh() is
         // removed from the ViewCanvas::UpdatePlot().
         //-----------------------------------------------------------
         mCanvas->Refresh(false);
         Update();
      }
   }
}


//------------------------------------------------------------------------------
// void TakeAction(const std::string &action)
//------------------------------------------------------------------------------
void MdiChildViewFrame::TakeAction(const std::string &action)
{
   if (mCanvas)
      mCanvas->TakeAction(action);
}


//------------------------------------------------------------------------------
// void InitializePlot()
//------------------------------------------------------------------------------
/*
 * Activates OnPaint() event
 */
//------------------------------------------------------------------------------
void MdiChildViewFrame::InitializePlot()
{
   #ifdef DEBUG_REFRESH_PLOT
   MessageInterface::ShowMessage
      ("MdiChildViewFrame::InitializePlot() entered\n");
   #endif
   
   if (mCanvas)
   {
      mCanvas->InitializePlot();
      Update();
   }
}


//------------------------------------------------------------------------------
// void RefreshPlot()
//------------------------------------------------------------------------------
/*
 * Activates OnPaint() event
 */
//------------------------------------------------------------------------------
void MdiChildViewFrame::RefreshPlot()
{
   #ifdef DEBUG_REFRESH_PLOT
   MessageInterface::ShowMessage
      ("MdiChildViewFrame::RefreshPlot() entered\n");
   #endif
   
   if (mCanvas)
   {
      mCanvas->SetEndOfData(true);
      mCanvas->Refresh(false);
      Update();
   }
}


//------------------------------------------------------------------------------
// void DeletePlot()
//------------------------------------------------------------------------------
void MdiChildViewFrame::DeletePlot()
{
   // This will call OnClose()
   Close(TRUE);
}


//------------------------------------------------------------------------------
// void SetEndOfRun()
//------------------------------------------------------------------------------
/*
 * Sets end of run flag.
 */
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetEndOfRun()
{
   if (mCanvas)
   {
      mCanvas->SetEndOfRun(true);
      mCanvas->Refresh(false);
      Update();
   }
}


//------------------------------------------------------------------------------
// void CheckFrame()
//------------------------------------------------------------------------------
/**
 * Checks if animation is running. If animation is running it sets mCanClose
 * flag to false.
 */
//------------------------------------------------------------------------------
void MdiChildViewFrame::CheckFrame()
{
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage("MdiChildViewFrame::CheckFrame() entered\n");
   #endif

   mCanClose = false;
   
   if (mCanvas)
   {
      #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
      MessageInterface::ShowMessage
         ("MdiChildViewFrame::CheckFrame() IsAnimationRunning=%d\n",
          mCanvas->IsAnimationRunning());
      #endif
      
      if (mCanvas->IsAnimationRunning())
         mCanClose = false;
      else
         mCanClose = true;
   }
   
   #ifdef DEBUG_MDI_CHILD_FRAME_CLOSE
   MessageInterface::ShowMessage
      ("MdiChildViewFrame::CheckFrame() exiting, mCanClose=%d\n", mCanClose);
   #endif
}

//------------------------------------------------------------------------------
// void MdiChildViewFrame::SetMaxDataPoints(Integer maxData)
//------------------------------------------------------------------------------
/**
* Sets custom max amount of data points to plot
*
* @param maxData The maximum number of data points to plot
*/
//------------------------------------------------------------------------------
void MdiChildViewFrame::SetMaxDataPoints(Integer maxData)
{
   if (mCanvas)
   {
      mCanvas->SetMaxDataPoints(maxData);
   }
}
