//$Id$
//------------------------------------------------------------------------------
//                              ResourceTree
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
// Author: Allison Greene
// Created: 2003/08/29
//    2010.03.23 Thomas Grubb 
//      - Removed Clone menu item from the Formation popup menu
//    2010.03.16 Thomas Grubb 
//      - Moved the GroundStation folder in the mission tree to appear below 
//        Formation folder
//
/**
 * This class provides the resource tree and event handlers.
 */
//------------------------------------------------------------------------------
// Icons used in resource tree
#include "bitmaps/ClosedFolder.xpm"
#include "bitmaps/OpenFolder.xpm"
#include "bitmaps/file.xpm"
// Spacecraft, Formation, GroundStation
#include "bitmaps/rt_Spacecraft.xpm"
#include "bitmaps/rt_Formation.xpm"
#include "bitmaps/rt_GroundStation.xpm"
// Hardware
#include "bitmaps/rt_Hardware.xpm"
#include "bitmaps/rt_ChemicalTank.xpm"
#include "bitmaps/rt_ElectricTank.xpm"
#include "bitmaps/rt_ChemicalThruster.xpm"
#include "bitmaps/rt_ElectricThruster.xpm"
#include "bitmaps/rt_SolarPowerSystem.xpm"
#include "bitmaps/rt_NuclearPowerSystem.xpm"
#include "bitmaps/rt_Antenna.xpm"
#include "bitmaps/rt_Transmitter.xpm"
#include "bitmaps/rt_Receiver.xpm"
#include "bitmaps/rt_Transponder.xpm"
// Propagator
#include "bitmaps/rt_Propagator.xpm"
// Burns
#include "bitmaps/rt_ImpulsiveBurn.xpm"
#include "bitmaps/rt_FiniteBurn.xpm"
// SolarSystem
#include "bitmaps/sun.xpm"
#include "bitmaps/mercury.xpm"
#include "bitmaps/venus.xpm"
#include "bitmaps/earth.xpm"
#include "bitmaps/mars.xpm"
#include "bitmaps/jupiter.xpm"
#include "bitmaps/saturn.xpm"
#include "bitmaps/uranus.xpm"
#include "bitmaps/neptune.xpm"
#include "bitmaps/pluto.xpm"
#include "bitmaps/network.xpm"
#include "bitmaps/moon.xpm"
#include "bitmaps/moon_generic.xpm"
#include "bitmaps/planet_generic.xpm"
#include "bitmaps/asteroid.xpm"
#include "bitmaps/comet.xpm"
#include "bitmaps/rt_SolarSystem.xpm"
#include "bitmaps/rt_Barycenter.xpm"
#include "bitmaps/rt_LibrationPoint.xpm"
// Solver
#include "bitmaps/rt_Solver.xpm"
#include "bitmaps/rt_BoundaryValueSolver.xpm"
#include "bitmaps/rt_DifferentialCorrector.xpm"
#include "bitmaps/rt_Optimizer.xpm"
#include "bitmaps/rt_Simulator.xpm"
#include "bitmaps/rt_Estimator.xpm"
// Output
#include "bitmaps/rt_Subscriber.xpm"
#include "bitmaps/rt_ReportFile.xpm"
#include "bitmaps/rt_OrbitView.xpm"
#include "bitmaps/rt_XYPlot.xpm"
#include "bitmaps/rt_EphemerisFile.xpm"
#include "bitmaps/rt_GroundTrackPlot.xpm"
// Matlab Engine/Server
#include "bitmaps/rt_Matlab.xpm"
#include "bitmaps/rt_MatlabServer.xpm"
#include "bitmaps/rt_FileInterface.xpm"
// Function
#include "bitmaps/rt_GmatFunction.xpm"
#include "bitmaps/rt_MatlabFunction.xpm"
// CoordinateSystem
#include "bitmaps/rt_CoordinateSystem.xpm"
// EclipseLocator
#include "bitmaps/rt_ContactLocator.xpm"   
#include "bitmaps/rt_EclipseLocator.xpm"   
// User Variable/Array/String
#include "bitmaps/rt_Variable.xpm"
#include "bitmaps/rt_Array.xpm"
#include "bitmaps/rt_String.xpm"
#include "bitmaps/rt_Script.xpm"
// MeasurementModel
#include "bitmaps/rt_MeasurementModel.xpm"
// Resource Default
#include "bitmaps/rt_Default.xpm"

#include "GuiInterpreter.hpp"
#include "ResourceTree.hpp"
#include "GmatAppData.hpp"
#include "GmatMainFrame.hpp"
#include "ScriptPanel.hpp"
#ifdef __USE_STC_EDITOR__
#include "EditorPanel.hpp"
#endif
#include "MissionTree.hpp"
#include "MessageInterface.hpp"
#include "GmatTreeItemData.hpp"
#include "ParameterCreateDialog.hpp"
#include "CoordSysCreateDialog.hpp"
#include "RunScriptFolderDialog.hpp"
#include "ViewTextDialog.hpp"
#include "Function.hpp"               // for SetNewFunction()
#include "FileManager.hpp"            // for GetFullPathname()
#include "FileUtil.hpp"               // for CompareTextLines()
#include "GmatGlobal.hpp"             // for SetBatchMode()
#include "StringUtil.hpp"             // for GmatStringUtil::
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "UtilityException.hpp"
#include <sstream>
#include <fstream>
#include <ctime>                      // for clock()
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/config.h>

#include "GmatEventHandler.hpp"
#include "PluginItemManager.hpp"

// If we are ready to handle Constellations, enable this
//define __ENABLE_CONSTELLATIONS__

// If we are ready to handle GUI Navigation plugins, enable this
//#define __ENABLE_NAV_GUI__

//#define DEBUG_RESOURCE_TREE
//#define DEBUG_ADD_DEFAULT_OBJECTS
//#define DEBUG_ADD_OBJECT
//#define DEBUG_RENAME
//#define DEBUG_DELETE
//#define DEBUG_COMPARE_REPORT
//#define DEBUG_RUN_SCRIPT_FOLDER
//#define DEBUG_BUILD_OBJECT
//#define DEBUG_RESOURCE_TREE_UPDATE
//#define DEBUG_RESOURCE_ICON
//#define DEBUG_ADD_ICONS
//#define DEBUG_ADD_COMET
//#define DEBUG_ACTIVE_SCRIPT
//#define DEBUG_FIND_SCRIPT
//#define DEBUG_ADD_SCRIPT
//#define DEBUG_ADD_SOLVER
//#define DEBUG_USER_GUI
//#define DEBUG_USER_OBJECT
//#define DEBUG_MENU_MAP
//#define DEBUG_GMAT_FUNCTION
//#define DEBUG_ADD_LOCATOR
//#define DEBUG_SHOW_MENU
//#define DEBUG_FONT
//#define DEBUG_POPUP_MENU


// ID macros for plug-ins
#define SOLVER_BEGIN 150
#define SOLVER_END 200
#define HARDWARE_BEGIN 250
#define HARDWARE_END 300
#define PLUGIN_BEGIN 400
#define PLUGIN_END 600
#define LOCATOR_BEGIN 700
#define LOCATOR_END 799

#define SUBSCRIBER_BEGIN SOLVER_END + 1
#define SUBSCRIBER_END SUBSCRIBER_BEGIN + 50

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Integer ResourceTree::MAX_SUN_ORBITERS = 120;


//------------------------------------------------------------------------------
// event tables and other macros for wxWidgets
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ResourceTree, wxTreeCtrl)
   EVT_TREE_ITEM_RIGHT_CLICK(-1, ResourceTree::OnItemRightClick)
   EVT_TREE_ITEM_ACTIVATED(-1, ResourceTree::OnItemActivated)
   EVT_TREE_ITEM_EXPANDED(-1, ResourceTree::OnItemExpanded)
   EVT_TREE_BEGIN_LABEL_EDIT(-1, ResourceTree::OnBeginLabelEdit)
   EVT_TREE_END_LABEL_EDIT(-1, ResourceTree::OnEndLabelEdit)
   //EVT_TREE_BEGIN_DRAG(-1, ResourceTree::OnBeginDrag)
   //EVT_TREE_BEGIN_RDRAG(-1, ResourceTree::OnBeginRDrag)
   //EVT_TREE_END_DRAG(-1, ResourceTree::OnEndDrag)

   EVT_MENU(POPUP_ADD_GROUND_STATION, ResourceTree::OnAddGroundStation)
   EVT_MENU(POPUP_ADD_SPACECRAFT, ResourceTree::OnAddSpacecraft)
   EVT_MENU(POPUP_ADD_HARDWARE, ResourceTree::OnAddHardware)
   EVT_MENU(POPUP_ADD_FUELTANK_CHEMICAL, ResourceTree::OnAddChemicalFuelTank)
   EVT_MENU(POPUP_ADD_FUELTANK_ELECTRIC, ResourceTree::OnAddElectricFuelTank)
   EVT_MENU(POPUP_ADD_THRUSTER_CHEMICAL, ResourceTree::OnAddChemicalThruster)
   EVT_MENU(POPUP_ADD_THRUSTER_ELECTRIC, ResourceTree::OnAddElectricThruster)
   EVT_MENU(POPUP_ADD_NUCLEAR_POWER_SYSTEM, ResourceTree::OnAddNuclearPowerSystem)
   EVT_MENU(POPUP_ADD_SOLAR_POWER_SYSTEM, ResourceTree::OnAddSolarPowerSystem)
   EVT_MENU(POPUP_ADD_FORMATION, ResourceTree::OnAddFormation)
   EVT_MENU(POPUP_ADD_CONSTELLATION, ResourceTree::OnAddConstellation)
   EVT_MENU(POPUP_ADD_IMPULSIVE_BURN, ResourceTree::OnAddImpulsiveBurn)
   EVT_MENU(POPUP_ADD_FINITE_BURN, ResourceTree::OnAddFiniteBurn)
   EVT_MENU(POPUP_ADD_PROPAGATOR, ResourceTree::OnAddPropagator)
//   EVT_MENU(POPUP_ADD_SPK_PROPAGATOR, ResourceTree::OnAddSPKPropagator)
   EVT_MENU(POPUP_ADD_BODY, ResourceTree::OnAddBody)
   EVT_MENU(POPUP_ADD_DIFFERENTIAL_CORRECTOR, ResourceTree::OnAddDifferentialCorrector)
   EVT_MENU(POPUP_ADD_SQP, ResourceTree::OnAddSqp)
   EVT_MENU_RANGE(POPUP_ADD_HARDWARE + HARDWARE_BEGIN,    // Other Hardware
                  POPUP_ADD_HARDWARE + HARDWARE_END, ResourceTree::OnAddHardware)
   EVT_MENU_RANGE(POPUP_ADD_SOLVER + SOLVER_BEGIN,        // Other Solvers
                  POPUP_ADD_SOLVER + SOLVER_END, ResourceTree::OnAddSolver)
   EVT_MENU_RANGE(POPUP_ADD_LOCATOR + LOCATOR_BEGIN,        // Event Locators
                  POPUP_ADD_LOCATOR + LOCATOR_END, ResourceTree::OnAddLocator)
   EVT_MENU(POPUP_ADD_REPORT_FILE, ResourceTree::OnAddReportFile)
   EVT_MENU(POPUP_ADD_XY_PLOT, ResourceTree::OnAddXyPlot)
   EVT_MENU(POPUP_ADD_ORBIT_VIEW, ResourceTree::OnAddOrbitView)
   EVT_MENU(POPUP_ADD_EPHEMERIS_FILE, ResourceTree::OnAddEphemerisFile)
   EVT_MENU_RANGE(POPUP_ADD_SUBSCRIBER + SUBSCRIBER_BEGIN, // Other Subscribers
                  POPUP_ADD_SUBSCRIBER + SUBSCRIBER_END, ResourceTree::OnAddSubscriber)
   EVT_MENU(POPUP_ADD_VARIABLE, ResourceTree::OnAddVariable)
   EVT_MENU(POPUP_ADD_ARRAY, ResourceTree::OnAddArray)
   EVT_MENU(POPUP_ADD_STRING, ResourceTree::OnAddString)
   EVT_MENU(POPUP_ADD_MATLAB_FUNCTION, ResourceTree::OnAddMatlabFunction)
   EVT_MENU(POPUP_ADD_GMAT_FUNCTION, ResourceTree::OnAddGmatFunction)
   EVT_MENU(POPUP_NEW_GMAT_FUNCTION, ResourceTree::OnAddGmatFunction)
   EVT_MENU(POPUP_OPEN_GMAT_FUNCTION, ResourceTree::OnAddGmatFunction)
   EVT_MENU(POPUP_ADD_COORD_SYS, ResourceTree::OnAddCoordSystem)
   EVT_MENU(POPUP_ADD_BARYCENTER, ResourceTree::OnAddBarycenter)
   EVT_MENU(POPUP_ADD_LIBRATION, ResourceTree::OnAddLibration)
   EVT_MENU(POPUP_ADD_PLANET, ResourceTree::OnAddPlanet)
   EVT_MENU(POPUP_ADD_MOON, ResourceTree::OnAddMoon)
   EVT_MENU(POPUP_ADD_COMET, ResourceTree::OnAddComet)
   EVT_MENU(POPUP_ADD_ASTEROID, ResourceTree::OnAddAsteroid)
   EVT_MENU_RANGE(POPUP_ADD_PLUGIN + PLUGIN_BEGIN,        // Plugin objects
                  POPUP_ADD_PLUGIN + PLUGIN_END, ResourceTree::OnAddUserObject)
   EVT_MENU(POPUP_OPEN, ResourceTree::OnOpen)
   EVT_MENU(POPUP_CLOSE, ResourceTree::OnClose)
   EVT_MENU(POPUP_RENAME, ResourceTree::OnRename)
   EVT_MENU(POPUP_DELETE, ResourceTree::OnDelete)
   EVT_MENU(POPUP_CLONE, ResourceTree::OnClone)
   EVT_MENU(POPUP_ADD_SCRIPT, ResourceTree::OnAddScript)
   EVT_MENU(POPUP_ADD_SCRIPT_FOLDER, ResourceTree::OnAddScriptFolder)
   EVT_MENU(POPUP_RUN_SCRIPTS_FROM_FOLDER, ResourceTree::OnRunScriptsFromFolder)
   EVT_MENU(POPUP_QUIT_RUN_SCRIPTS_FROM_FOLDER, ResourceTree::OnQuitRunScriptsFromFolder)
   EVT_MENU(POPUP_REMOVE_SCRIPT_FOLDER, ResourceTree::OnRemoveScriptFolder)
   EVT_MENU(POPUP_REMOVE_ALL_SCRIPTS, ResourceTree::OnRemoveAllScripts)
   EVT_MENU(POPUP_REMOVE_SCRIPT, ResourceTree::OnRemoveScript)
   EVT_MENU(POPUP_RECOVER_SCRIPT, ResourceTree::OnScriptRecover)
   EVT_MENU(POPUP_BUILD_SCRIPT, ResourceTree::OnScriptBuildObject)
   EVT_MENU(POPUP_BUILD_AND_RUN_SCRIPT, ResourceTree::OnScriptBuildAndRun)

END_EVENT_TABLE()

//------------------------------------------------------------------------------
// ResourceTree(wxFrame *parent)
//------------------------------------------------------------------------------
/**
 * A constructor.
 *
 * @param <parent> parent window
 * @param <id> window id for referencing events
 * @param <pos> position
 * @param <size> size
 * @param <style> style flags
 */
//------------------------------------------------------------------------------
ResourceTree::ResourceTree(wxWindow *parent, const wxWindowID id,
                           const wxPoint &pos, const wxSize &size, long style)
   : wxTreeCtrl(parent, id, pos, size, style),
   nextGuiPluginId (10001)
{
   // Set GMAT default font
   SetFont(GmatAppData::Instance()->GetFont());
   #ifdef DEBUG_FONT
   wxFont currFont = GetFont();
   MessageInterface::ShowMessage
      ("In ResourceTree() constructor, currFont.FaceName = '%s'\ncurrFont.NativeFontInfoDesc = '%s'\n"
       "currFont.NativeFontInfoUserDesc = '%s'\ncurrFont.GetPointSize = %d\n",
       currFont.GetFaceName().WX_TO_C_STRING,
       currFont.GetNativeFontInfoDesc().WX_TO_C_STRING,
       currFont.GetNativeFontInfoUserDesc().WX_TO_C_STRING,
       currFont.GetPointSize());
   #endif
   
   theMainFrame = NULL;
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   mScriptFolderRunning = false;
   mScriptFolderRunInterrupted = false;
   mHasEventLocatorPlugin = GmatGlobal::Instance()->IsEventLocationAvailable();
   mScriptAdded = false;
   mMatlabServerStarted = false;
   
   // Setup the popup menus and register GUI plugins
   CreatePopupMenuMap();
   RegisterGuiPluginResources();

   // @todo Need more work on this automatic loading of icons
   // Use this mapping for getting icon id when work is complete
   // itemIcon = (GmatTree::ResourceIconType)typeIconMap[objTypeName];
   //InitializeIcons();
   
   AddIcons();
   AddDefaultResources();
   
   // Now AddDefaultResources() calls UpdateResource() which also adds user objects
   // so commented out (LOJ: 2014.09.16)
   //AddUserResources(theGuiInterpreter->GetUserResources());   
   
   // shortcut keys
   wxAcceleratorEntry entries[3];
   entries[0].Set(wxACCEL_NORMAL,  WXK_NUMPAD_DELETE, POPUP_DELETE);
   entries[1].Set(wxACCEL_SHIFT | wxACCEL_CTRL,  (int) 'C', POPUP_CLONE);
   entries[2].Set(wxACCEL_NORMAL, WXK_F2, POPUP_RENAME);
   wxAcceleratorTable accel(3, entries);
   SetAcceleratorTable(accel);
   
   #ifdef DEBUG_FONT
   MessageInterface::ShowMessage
      ("currFont.FaceName = '%s'\ncurrFont.NativeFontInfoDesc = '%s'\n"
       "currFont.NativeFontInfoUserDesc = '%s'\ncurrFont.GetPointSize = %d",
       currFont.GetFaceName().WX_TO_C_STRING,
       currFont.GetNativeFontInfoDesc().WX_TO_C_STRING,
       currFont.GetNativeFontInfoUserDesc().WX_TO_C_STRING,
       currFont.GetPointSize());
   #endif
   
   theGuiManager->UpdateAll();
}


//------------------------------------------------------------------------------
// void SetMainFrame(GmatMainFrame *gmf)
//------------------------------------------------------------------------------
void ResourceTree::SetMainFrame(GmatMainFrame *gmf)
{
   theMainFrame = gmf;
}


//------------------------------------------------------------------------------
// void ClearResource(bool leaveScripts, bool onlyChildNodes)
//------------------------------------------------------------------------------
void ResourceTree::ClearResource(bool leaveScripts, bool onlyChildNodes)
{
   // ag: collapse, so folder icon is closed
   #ifdef __WXMSW__
      Collapse(mGroundStationItem);
      Collapse(mSpacecraftItem);
      Collapse(mHardwareItem);
      Collapse(mFormationItem);
      Collapse(mPropagatorItem);
      Collapse(mBurnItem);
      Collapse(mSolverItem);
      Collapse(mSubscriberItem);
      Collapse(mVariableItem);
      Collapse(mFunctionItem);
      Collapse(mCoordSysItem);
      Collapse(mSpecialPointsItem);
   #endif

   // Delete children for all but interface and, optionally, script
   wxTreeItemId root = GetRootItem();
   wxTreeItemIdValue cookie;
   wxTreeItemId cf = GetFirstChild(root, cookie);
   for(UnsignedInt i = 0; i < GetChildrenCount(root, false); ++i)
   {
      {
         if (cf != mScriptItem)
         {
            #ifdef DEBUG_USER_GUI
               MessageInterface::ShowMessage("Clearing %s\n",
                     GetItemText(cf).WX_TO_C_STRING);
            #endif
            DeleteChildren(cf);
         }
         if (!leaveScripts && cf == mScriptItem)
         {
            #ifdef DEBUG_USER_GUI
               MessageInterface::ShowMessage("Clearing %s\n",
                     GetItemText(cf).c_str());
            #endif
            DeleteChildren(cf);
         }
      }
      cf = GetNextChild(root, cookie);
   }
   
   //----- SpecialPoint is child of Universe
   AddItemFolder(mUniverseItem, mSpecialPointsItem, "Special Points",
                 GmatTree::SPECIAL_POINT_FOLDER);
   
   //--------------- Boundary Value Solvers is a child of solvers
   AddItemFolder(mSolverItem, mBoundarySolverItem, "Boundary Value Solvers",
                 GmatTree::BOUNDARY_SOLVER_FOLDER);
   
   //--------------- Optimizers is a child of solvers
   AddItemFolder(mSolverItem, mOptimizerItem, "Optimizers",
                 GmatTree::OPTIMIZER_FOLDER);
   
   AddUserResources(theGuiInterpreter->GetUserResources(), onlyChildNodes);
}


//------------------------------------------------------------------------------
// void UpdateResource(bool restartCounter, bool onlyChildNodes)
//------------------------------------------------------------------------------
/**
 * Delete all nodes that are not folders, add default nodes
 *
 * @param restartCounter  Restarting the counter from zero if true.
 */
//------------------------------------------------------------------------------
void ResourceTree::UpdateResource(bool restartCounter, bool onlyChildNodes)
{
   #ifdef DEBUG_RESOURCE_TREE_UPDATE
   MessageInterface::ShowMessage
      ("ResourceTree::UpdateResource() entered, restartCounter = %d, "
       "onlyChildNodes = %d\n", restartCounter, onlyChildNodes);
   #endif
   
   if (restartCounter)
      theGuiInterpreter->ResetConfigurationChanged();
   
   ClearResource(true, onlyChildNodes);
   
   AddDefaultBodies(mUniverseItem);
   AddDefaultSpecialPoints(mSpecialPointsItem);
   AddDefaultSpacecraft(mSpacecraftItem, restartCounter);
   AddDefaultFormations(mFormationItem, restartCounter);
   AddDefaultGroundStation(mGroundStationItem, restartCounter);
   AddDefaultHardware(mHardwareItem, restartCounter);
   #ifdef __ENABLE__CONSTELLATIONS__
   AddDefaultConstellations(constellationItem);
   #endif
   AddDefaultPropagators(mPropagatorItem, restartCounter);
   AddDefaultLocators(mEventLocatorItem, restartCounter);
   AddDefaultBurns(mBurnItem, restartCounter);
   AddDefaultSolvers(mSolverItem, restartCounter);
   AddDefaultSubscribers(mSubscriberItem, restartCounter);
   AddDefaultInterfaces(interfaceItem);
   AddDefaultVariables(mVariableItem);
   AddDefaultFunctions(mFunctionItem);
   AddDefaultCoordSys(mCoordSysItem);
   
   AddUserObjects();
   
   theGuiManager->UpdateAll();

   // Alphabetize the variables on load
   SortChildren(mVariableItem);
   theGuiManager->SortUserVariables();
   ScrollTo(mSpacecraftItem);
   
   // Why the first item is always selected with WX3?
   // Add Unselect() (LOJ: 2014.09.16)
   Unselect();
   
   #ifdef DEBUG_RESOURCE_TREE_UPDATE
   MessageInterface::ShowMessage("ResourceTree::UpdateResource() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetActiveScript(const wxString &scriptWithPath)
//------------------------------------------------------------------------------
void ResourceTree::SetActiveScript(const wxString &scriptWithPath)
{
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("ResourceTree::SetActiveScript() entered, mLastActiveScript='%s', "
       "scriptWithPath='%s'\n", mLastActiveScript.c_str(), scriptWithPath.c_str());
   #endif
   
   // Make last active script normal face first
   MakeScriptActive(mLastActiveScript, false);
   
   // Make new active script bold face
   wxString newScript = (GmatFileUtil::ParseFileName(scriptWithPath.c_str())).c_str();
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("   scriptWithPath='%s'\n   newScript='%s'\n", scriptWithPath.c_str(), newScript.c_str());
   #endif
   
   MakeScriptActive(newScript, true);
   
   mLastActiveScript = newScript;
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage("ResourceTree::SetActiveScript() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void AddScript(wxString path)
//------------------------------------------------------------------------------
/*
 * Add script file path and name to resource tree and sets main frame title
 *
 * @param  path  script full path name
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::AddScript(wxString path)
{
   mScriptAdded = AddScriptItem(path);
   if (mScriptAdded)
   {
      // need to set the filename to MainFrame
      // We don't want to set the filename here (LOJ: 2010.12.30)
      // GmatMainFrame will call GetLastScriptAdded() instead
      //theMainFrame->SetScriptFileName(path.c_str());
      mLastScriptAdded = path;
   }
}


//------------------------------------------------------------------------------
// bool AddScriptItem(wxString path)
//------------------------------------------------------------------------------
/*
 * Adds script file path and name to resource tree.
 *
 * @param  path  script full path name
 *
 * @return true if script file successfully added, false otherwise
 */
//------------------------------------------------------------------------------
bool ResourceTree::AddScriptItem(wxString path)
{
   #ifdef DEBUG_ADD_SCRIPT
   MessageInterface::ShowMessage
      ("ResourceTree::AddScriptItem() path=<%s>\n", path.c_str());
   #endif

   wxTreeItemIdValue cookie;
   wxString scriptPath, childText;
   wxTreeItemId childId = GetFirstChild(mScriptItem, cookie);
   wxTreeItemId scriptId;
   bool scriptAdded = false;
   bool hasSameName = false;
   bool hasSamePath = false;
   
   // extract file name
   //wxFileName fn(path);
   //wxString filename = fn.GetName();
   // Use FileUtil for consistency (LOJ: 2010.01.04)
   wxString filename = (GmatFileUtil::ParseFileName(path.c_str())).c_str();
   
   #ifdef DEBUG_ADD_SCRIPT
   MessageInterface::ShowMessage("   filename=%s\n", filename.c_str());
   #endif

   // find child with same path
   while (childId.IsOk())
   {
      childText = GetItemText(childId);

      #ifdef DEBUG_ADD_SCRIPT
      MessageInterface::ShowMessage
         ("ResourceTree::OnAddScriptItem() childText=<%s>\n", childText.c_str());
      #endif
      
      scriptPath = ((GmatTreeItemData *)GetItemData(childId))->GetName();
      
      if (GmatFileUtil::IsSameFileName(childText.c_str(), filename.c_str()))
      {
         hasSameName = true;
         if (scriptPath == path)
         {
            hasSamePath = true;
            break;
         }
      }

      childId = GetNextChild(mScriptItem, cookie);
   }

   // if same path not found, add to tree
   if (!hasSamePath)
   {
      wxString fileNameToUse = filename;
      // If it has the same file name, add path
      if (hasSameName)
         fileNameToUse = path;
      
      scriptId =
         AppendItem(mScriptItem, fileNameToUse, GmatTree::RESOURCE_ICON_SCRIPT, -1,
                    new GmatTreeItemData(path, GmatTree::SCRIPT_FILE, path));
      
      Expand(mScriptItem);
      SelectItem(scriptId);
      UpdateRecentFiles(path);
      scriptAdded = true;
   }
   else
   {
      //MessageInterface::PopupMessage
      //   (Gmat::INFO_, "The script file \"%s\" is already in the tree", path.c_str());

      scriptAdded = true;
   }

   return scriptAdded;
}


//------------------------------------------------------------------------------
// void UpdateFormation()
//------------------------------------------------------------------------------
/**
 * Updates Formation node.
 */
//------------------------------------------------------------------------------
void ResourceTree::UpdateFormation()
{
   if(mFormationItem.IsOk()) DeleteChildren(mFormationItem);
   AddDefaultFormations(mFormationItem);
}


//------------------------------------------------------------------------------
// void UpdateRecentFiles()
//------------------------------------------------------------------------------
/**
 * Updates recent files
 */
//------------------------------------------------------------------------------
void ResourceTree::UpdateRecentFiles(wxString filename)
{
   wxArrayString files;
   wxString aFilename;
   wxString aKey;
   size_t i = 0;

   // get the config object
   wxConfigBase *pConfig = GmatAppData::Instance()->GetPersonalizationConfig();
   pConfig->SetPath(wxT("/RecentFiles"));

   // read filenames from config object
   while (pConfig->Read(wxString::Format(wxT("%d"), (int)i), &aFilename)) 
   {
      files.Add(aFilename);
      i++;
   }

   // if filename already in list, delete it (we will add on end)
   if (files.Index(filename.c_str()) != wxNOT_FOUND) 
   {
      files.Remove(filename.c_str());
   }

   // add latest filename to end of list
   files.Add(filename);

   // remove files from beginning of list
   while (files.GetCount() > 5)
      files.RemoveAt(0);

   // save filenames back to config IF we are supposed to
   if (GmatGlobal::Instance()->GetWritePersonalizationFile())
   {
      pConfig->SetPath(wxT("/"));
      pConfig->DeleteGroup("/RecentFiles");
      pConfig->SetPath(wxT("/RecentFiles"));
      for (i = 0; i < files.GetCount(); i++)
      {
         aFilename = files[i];
         aKey = wxString::Format(wxT("%d"), (int) i);
         pConfig->Write(aKey, aFilename);
         //pConfig->Write((GmatFileUtil::ParseFileName(aFilename.c_str())).c_str(), aFilename.c_str());
      }
      pConfig->Flush(false);
   }

   theMainFrame->UpdateRecentMenu(files);

}

//------------------------------------------------------------------------------
// void UpdateSpacecraft()
//------------------------------------------------------------------------------
/**
 * Updates Spacecraft node.
 */
//------------------------------------------------------------------------------
void ResourceTree::UpdateSpacecraft()
{
   #ifdef DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("ResourceTree::UpdateSpacecraft() entered, number of spacecraft: %d\n",
       GetChildrenCount(mSpacecraftItem));
   #endif

   // Update GUI spacecraft list
   theGuiManager->UpdateSpacecraft();
   
   // If Spacecraft node is empty, populate with default spacecraft
   if (GetChildrenCount(mSpacecraftItem) == 0)
   {
      DeleteChildren(mSpacecraftItem);
      AddDefaultSpacecraft(mSpacecraftItem);
   }
}

//------------------------------------------------------------------------------
// void UpdateVariable()
//------------------------------------------------------------------------------
/**
 * Updates Variable node.
 */
//------------------------------------------------------------------------------
void ResourceTree::UpdateVariable()
{
   DeleteChildren(mVariableItem);
   AddDefaultVariables(mVariableItem);
   SelectItem(GetLastChild(mVariableItem));
}


// @todo Need more work to automatically load icons via object type name
#if 0
//------------------------------------------------------------------------------
// void InitializeIcons()
//------------------------------------------------------------------------------
void ResourceTree::InitializeIcons()
{
   // Get list of items to show in the tree by querying the factory.
   ObjectTypeArray exceptList;
   exceptList.push_back(Gmat::PARAMETER);
   exceptList.push_back(Gmat::COMMAND);
   exceptList.push_back(Gmat::ODE_MODEL);
   exceptList.push_back(Gmat::INTERPOLATOR);
   exceptList.push_back(Gmat::ATMOSPHERE);
   exceptList.push_back(Gmat::MATH_NODE);
   exceptList.push_back(Gmat::AXIS_SYSTEM);
   exceptList.push_back(Gmat::ATTITUDE);
   exceptList.push_back(Gmat::PROPAGATOR);
   exceptList.push_back(Gmat::PHYSICAL_MODEL);
   exceptList.push_back(Gmat::STOP_CONDITION);
   exceptList.push_back(Gmat::INTERFACE);
   exceptList.push_back(Gmat::DATA_FILE);
   exceptList.push_back(Gmat::MEDIA_CORRECTION);
   
   StringArray objectTypeNames = theGuiInterpreter->GetListOfAllFactoryItemsExcept(exceptList);
   // We need to add Variable, Array, String to the list
   // @todo In some day we want to add SYSTEM_PARAMETER the list of object types in gmatdefa.hpp
   objectTypeNames.push_back("Variable");
   objectTypeNames.push_back("Array");
   objectTypeNames.push_back("String");
   
   int numTypes = objectTypeNames.size();
   MessageInterface::ShowMessage("==> There are %d object types\n", numTypes);
   
   // Create and assign icons to object types
   int sizeW = 16;
   int sizeH = 16;
   
   wxImageList *images = new wxImageList ( sizeW, sizeH, true );
   wxBitmap **bitmaps = new wxBitmap*[numTypes];
   std::string iconFileName;
   long bitmapType = wxBITMAP_TYPE_PNG;
   for (int i = 0; i < numTypes; i++)
   {
      MessageInterface::ShowMessage("   %2d  %s\n", i, objectTypeNames[i].c_str());
      typeIconMap[objectTypeNames[i]] = i;
      iconFileName = "rt_" + objectTypeNames[i];
      theGuiManager->LoadIcon(iconFileName.c_str(), bitmapType, &bitmaps[i], rt_Default_xpm); 
   }
   
   // Let's always rescale all icons since size of icon look different on different platforms
   for ( size_t i = 0; i < WXSIZEOF(bitmaps); i++ )
      images->Add(bitmaps[i]->ConvertToImage().Rescale(sizeW, sizeH));
   
   AssignImageList(images);
   
   // Why this causes crash when launching GMAT?
   //for (int i = 0; i < numTypes; i++)
   //   delete [] bitmaps[i];
   
   // Need to fix crash on exit
   delete [] bitmaps;
}
#endif

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &objType, const char *objName, ...)
//------------------------------------------------------------------------------
/**
 * Calls GuiInterpreter to create an object
 *
 * @param objType  Type of the object to be created
 * @param objName  Name of the object to be created
 * @param createDefault  Set this flag to true if default object to be created [false]
 */
//------------------------------------------------------------------------------
GmatBase* ResourceTree::CreateObject(const std::string &objType,
                                     const char *objName,
                                     bool createDefault)
{
   return CreateObject(objType, std::string(objName), createDefault);
}


//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &objType, const std::string &objName, ...)
//------------------------------------------------------------------------------
/**
 * Calls GuiInterpreter to create an object
 *
 * @param objType  Type of the object to be created
 * @param objName  Name of the object to be created
 * @param createDefault  Set this flag to true if default object to be created [false]
 */
//------------------------------------------------------------------------------
GmatBase* ResourceTree::CreateObject(const std::string &objType,
                                     const std::string &objName,
                                     bool createDefault)
{
   #ifdef DEBUG_CREATE_OBJECT
   MessageInterface::ShowMessage
      ("ResourceTree::CreateObject() entered, objType='%s', objName='%s', "
       "createDefault=%d\n", objType.c_str(), objName.c_str(), createDefault);
   #endif
   
   try
   {
      // Create default object
      GmatBase *obj = theGuiInterpreter->CreateObject(objType, objName, 1, createDefault);
      return obj;
   }
   catch (BaseException &be)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, be.GetFullMessage());
      return NULL;
   }
}


//------------------------------------------------------------------------------
// GmatBase* GetObject(const char *name)
//------------------------------------------------------------------------------
GmatBase* ResourceTree::GetObject(const char *name)
{
   return theGuiInterpreter->GetConfiguredObject(name);
}


//------------------------------------------------------------------------------
// GmatBase* GetObject(const std::string &name)
//------------------------------------------------------------------------------
GmatBase* ResourceTree::GetObject(const std::string &name)
{
   return theGuiInterpreter->GetConfiguredObject(name);
}


//------------------------------------------------------------------------------
// void UpdateGuiItem(GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
void ResourceTree::UpdateGuiItem(GmatTree::ItemType itemType)
{
   #ifdef DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("ResourceTree::UpdateGuiItem() entered, itemType=%d\n", itemType);
   #endif
   
   switch (itemType)
   {
   case GmatTree::PROPAGATOR:
      theGuiManager->UpdatePropagator();
      break;
   case GmatTree::PREDEFINED_COORDINATE_SYSTEM:
   case GmatTree::USER_COORDINATE_SYSTEM:
      theGuiManager->UpdateCoordSystem();
      break;
   case GmatTree::SPACECRAFT:
      theGuiManager->UpdateSpacecraft();
      break;
   case GmatTree::FORMATION:
      theGuiManager->UpdateFormation();
      break;
   case GmatTree::GROUND_STATION:
      theGuiManager->UpdateGroundStation();
      break;
   case GmatTree::EVENT_LOCATOR:
      theGuiManager->UpdateLocator();
      break;
   case GmatTree::FUELTANK_CHEMICAL:
   case GmatTree::FUELTANK_ELECTRIC:
      theGuiManager->UpdateFuelTank();
      break;
   case GmatTree::THRUSTER_CHEMICAL:
   case GmatTree::THRUSTER_ELECTRIC:
      theGuiManager->UpdateThruster();
      break;
   case GmatTree::POWER_SYSTEM:
      theGuiManager->UpdatePowerSystem();
      break;
   case GmatTree::HARDWARE:
      theGuiManager->UpdateImager();
      break;
   case GmatTree::IMPULSIVE_BURN:
   case GmatTree::FINITE_BURN:
      theGuiManager->UpdateBurn();
      break;
   case GmatTree::REPORT_FILE:
   case GmatTree::XY_PLOT:
   case GmatTree::ORBIT_VIEW:
   case GmatTree::GROUND_TRACK_PLOT:
   case GmatTree::EPHEMERIS_FILE:
      theGuiManager->UpdateSubscriber();
      break;
   case GmatTree::DIFFERENTIAL_CORRECTOR:
   case GmatTree::SQP:
   case GmatTree::SOLVER:
   case GmatTree::OPTIMIZER:
      theGuiManager->UpdateSolver();
      break;
   case GmatTree::PREDEFINED_BARYCENTER:
   case GmatTree::BARYCENTER:
   case GmatTree::LIBRATION_POINT:
      theGuiManager->UpdateCelestialPoint();
      break;
   case GmatTree::VARIABLE:
   case GmatTree::ARRAY:
   case GmatTree::STRING:
      theGuiManager->UpdateParameter();
      break;
   case GmatTree::MATLAB_FUNCTION:
   case GmatTree::GMAT_FUNCTION:
      theGuiManager->UpdateFunction();
      break;
   default:
      {
         if ((itemType >= GmatTree::USER_DEFINED_OBJECT ) &&
             (itemType < GmatTree::SCRIPT_FILE))
         {
            //MessageInterface::ShowMessage("Fell into default in UpdateGuiItem\n");
            /// @todo Add plugin objects here

            theGuiManager->UpdateAll();
         }
      }
      break;
   }

   #ifdef DEBUG_UPDATE
   MessageInterface::ShowMessage
      ("ResourceTree::UpdateGuiItem() leaving, itemType=%d\n", itemType);
   #endif
}


//------------------------------------------------------------------------------
// void AddItemFolder(wxTreeItemId itemId, const wxString &itemName)
//------------------------------------------------------------------------------
/*
 * Adds the item folder to the tree
 *
 * @param parentItemId  parent tree item id
 * @param itemId        tree item id
 * @param itemName      name of the item to be added
 * @param itemType      type of the item to be added
 */
//------------------------------------------------------------------------------
void ResourceTree::AddItemFolder(wxTreeItemId parentItemId, wxTreeItemId &itemId,
                                 const wxString &itemName, GmatTree::ItemType itemType)
{
   itemId =
      AppendItem(parentItemId, itemName, GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(itemName, itemType));
   
   SetItemImage(itemId, GmatTree::RESOURCE_ICON_OPEN_FOLDER, wxTreeItemIcon_Expanded);
}


//------------------------------------------------------------------------------
// wxTreeItemId AddObjectToTree(GmatBase *obj, bool showDebug)
//------------------------------------------------------------------------------
/**
 * Adds the cloned object to the resource tree and returns the node id.
 *
 * @param  obj  Cloned object to be added to the tree
 *
 * @return  The item id just added to tree
 */
//------------------------------------------------------------------------------
wxTreeItemId ResourceTree::AddObjectToTree(GmatBase *obj, bool showDebug)
{
   if (showDebug)
      MessageInterface::ShowMessage
         ("ResourceTree::AddObjectToTree() entered, obj=<%p><%s>'%s'\n", obj,
          obj->GetTypeName().c_str(), obj->GetName().c_str());
   
   wxString name = obj->GetName().c_str();
   GmatTree::ItemType itemType;
   GmatTree::ResourceIconType itemIcon;
   
   GetItemTypeAndIcon(obj, itemType, itemIcon);
   wxTreeItemId itemId = GetTreeItemId(itemType);
   Integer itemIconInt = itemIcon;

   if (obj->IsOfType("DataInterface"))
   {
      itemType = GmatTree::INTERFACE_OPENABLE;
   }
   
   if (itemType != GmatTree::UNKNOWN_ITEM)
   {
      #ifdef DEBUG_ADD_OBJECT
      MessageInterface::ShowMessage
         ("   Now appending '%s' to node %s\n", name.WX_TO_C_STRING,
               itemId.IsOk() ? "OK" : "Item Not Found");
      #endif

      GmatTreeItemData *newItemData = new GmatTreeItemData(name, itemType);
      if (obj->HasGuiPlugin())
         newItemData->SetPluginGui(true);
      if (obj->GetIcon())
      {
         itemIconInt = obj->GetIconIndex();
         if (itemIconInt < 0)
         {
            const char** iconData = obj->GetIcon();
            GmatTree::ResourceIconType objectsIcon;
            wxBitmap *bitmap;
            theGuiManager->LoadIcon("NoSuchFile", -1, &bitmap, iconData);
            wxImageList *treeImageList = GetImageList();
            itemIconInt = treeImageList->Add(*bitmap);
         }
      }
      AppendItem(itemId, name, itemIconInt, -1, newItemData);
   }
   
   PluginItemManager::Instance()->UpdateObjectList(obj->GetType());

   if (showDebug)
      MessageInterface::ShowMessage
               ("ResourceTree::AddObjectToTree() returning itemId = '%s'\n\n",
                itemId.IsOk() ? "OK" : "Item Not Found");
   return itemId;
}


//------------------------------------------------------------------------------
//  void AddDefaultResources()
//------------------------------------------------------------------------------
/**
 * Add default folders
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultResources()
{
   #ifdef DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage("ResourceTree::AddDefaultResources() entered\n");
   #endif
   
   wxTreeItemId resource =
      AddRoot(wxT("Resources"), -1, -1,
              new GmatTreeItemData(wxT("Resources"), GmatTree::RESOURCES_FOLDER));

   //----- Spacecraft
   mSpacecraftItem =
      AppendItem(resource, wxT("Spacecraft"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Spacecraft"),
                                      GmatTree::SPACECRAFT_FOLDER));

   SetItemImage(mSpacecraftItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);

   //----- Formations
   // Only do this if the Formation object is available
   // Now it uses its own object ID (LOJ: 2016.09.21)
   //StringArray scos = theGuiInterpreter->GetListOfFactoryItems(Gmat::SPACECRAFT);
   StringArray scos = theGuiInterpreter->GetListOfFactoryItems(Gmat::FORMATION);
   if (find(scos.begin(), scos.end(), "Formation") != scos.end())
   {
      mFormationItem =
         AppendItem(resource, wxT("Formations"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                    new GmatTreeItemData(wxT("Formations"),
                                         GmatTree::FORMATION_FOLDER));

      SetItemImage(mFormationItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                   wxTreeItemIcon_Expanded);
   }

   //----- GroundStation
   // Only do this if the groundstation object is available
   StringArray bfos = theGuiInterpreter->GetListOfFactoryItems(Gmat::SPACE_POINT);
   if (find(bfos.begin(), bfos.end(), "GroundStation") != bfos.end())
   {
      mGroundStationItem =
         AppendItem(resource, wxT("Ground Station"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                    new GmatTreeItemData(wxT("Ground Station"),
                                         GmatTree::GROUND_STATION_FOLDER));

      SetItemImage(mGroundStationItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                   wxTreeItemIcon_Expanded);
   }

   //----- Hardware
   mHardwareItem =
      AppendItem(resource, wxT("Hardware"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Hardware"),
                                      GmatTree::HARDWARE_FOLDER));

   SetItemImage(mHardwareItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);

   #ifdef __ENABLE_CONSTELLATIONS__
   //----- Constellations
   wxTreeItemId constellationItem =
      AppendItem(resource,
                 wxT("Constellations"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Constellations"),
                                      GmatTree::CONSTELLATION_FOLDER));

   SetItemImage(constellationItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);
   #endif

   //----- Burns
   mBurnItem =
      AppendItem(resource, wxT("Burns"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Burns"),
                                      GmatTree::BURN_FOLDER));

   SetItemImage(mBurnItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER, wxTreeItemIcon_Expanded);


   //----- Propagators
   mPropagatorItem =
      AppendItem(resource,
                 wxT("Propagators"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Propagators"),
                                      GmatTree::PROPAGATOR_FOLDER));

   SetItemImage(mPropagatorItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);

   //----- Universe
   mUniverseItem =
      AppendItem(resource, wxT("SolarSystem"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("SolarSystem"),
                                      GmatTree::SOLAR_SYSTEM));

   SetItemImage(mUniverseItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);

   //----- Solver
   mSolverItem =
      AppendItem(resource, wxT("Solvers"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Solvers"),
                                      GmatTree::SOLVER_FOLDER));

   SetItemImage(mSolverItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);

   //----- Subscribers
   mSubscriberItem =
      AppendItem(resource, wxT("Output"), GmatTree::RESOURCE_ICON_FOLDER,
                 -1, new GmatTreeItemData(wxT("Output"),
                                          GmatTree::SUBSCRIBER_FOLDER));

   SetItemImage(mSubscriberItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);

   //----- Interfaces
   interfaceItem =
      AppendItem(resource, wxT("Interfaces"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Interfaces"),
                                      GmatTree::INTERFACE_FOLDER));

   SetItemImage(interfaceItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);

   //----- Scripts
   mScriptItem =
      AppendItem(resource, wxT("Scripts"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Scripts"),
                                      GmatTree::SCRIPT_FOLDER));

   SetItemImage(mScriptItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);

   //----- Variables
   mVariableItem =
      AppendItem(resource, wxT("Variables/Arrays/Strings"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                 new GmatTreeItemData(wxT("Variables/Arrays/Strings"),
                                      GmatTree::VARIABLE_FOLDER));
   // ag:  Should the GmatTree type of variableItem change?

   SetItemImage(mVariableItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);

   //----- Coordinate System
   mCoordSysItem =
   AppendItem(resource, wxT("Coordinate Systems"), GmatTree::RESOURCE_ICON_FOLDER,
              -1, new GmatTreeItemData(wxT("Coordinate Systems"),
                                       GmatTree::COORDINATE_SYSTEM_FOLDER));
   SetItemImage(mCoordSysItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                wxTreeItemIcon_Expanded);

   //----- Event Locators
   if (GmatGlobal::Instance()->IsEventLocationAvailable())
   {
      mEventLocatorItem =
         AppendItem(resource, wxT("Event Locators"), GmatTree::RESOURCE_ICON_FOLDER, -1,
                    new GmatTreeItemData(wxT("Event Locators"),
                                         GmatTree::EVENT_LOCATOR_FOLDER));

      SetItemImage(mEventLocatorItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                   wxTreeItemIcon_Expanded);
   }

   //----- Functions
   functionTypes = theGuiInterpreter->GetListOfFactoryItems(Gmat::FUNCTION);
   #ifdef DEBUG_FUNCTION
      MessageInterface::ShowMessage("Found %d function types\n",
            functionTypes.size());

      MessageInterface::ShowMessage("Function types:\n");
      for (UnsignedInt i = 0; i < functionTypes.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", functionTypes[i].c_str());
   #endif
   if (functionTypes.size() > 0)
   {
      mFunctionItem =
         AppendItem(resource, wxT("Functions"), GmatTree::RESOURCE_ICON_FOLDER,
                 -1, new GmatTreeItemData(wxT("Functions"), GmatTree::FUNCTION_FOLDER));
      SetItemImage(mFunctionItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
                   wxTreeItemIcon_Expanded);
   }

   // Add default and user resources
   UpdateResource(true, false);
   
   // Set scroll bar to left to show resource nodes (Fix for GMT-4714 LOJ: 2014.09.11)
   SetScrollPos(wxHORIZONTAL, 0, true);
   
   theGuiInterpreter->ResetConfigurationChanged(true, false);
   
   #ifdef DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage("ResourceTree::AddDefaultResources() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void AddDefaultBodies(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default bodies
 *
 * @param <itemId> tree item for the solar system folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultBodies(wxTreeItemId itemId)
{
   SolarSystem   *ss         = theGuiInterpreter->GetSolarSystemInUse();
   StringArray   bodies      = ss->GetBodiesInUse();
   CelestialBody *body       = NULL;
   std::string   centralBody = "";

   std::string  star;
   StringArray  sunOrbiters;

   unsigned int      numBodies = bodies.size();

   GmatTree::ResourceIconType starIcon = GmatTree::RESOURCE_ICON_SUN;

   // find the star (assume there's one for now)
   for (unsigned int ii = 0; ii < numBodies; ii++)
   {
      body = ss->GetBody(bodies.at(ii));
      if (body->GetBodyType() == Gmat::STAR)  star = bodies.at(ii);
      break;   // assuming one star, for now at least
   }

   for (unsigned int ii = 0; ii < numBodies; ii++)
   {
      body = ss->GetBody(bodies.at(ii));
      centralBody = body->GetStringParameter(body->GetParameterID("CentralBody"));
      if (centralBody == star)
      {
         sunOrbiters.push_back(bodies.at(ii));
      }
   }
   unsigned int numSunOrbiters = sunOrbiters.size();
//   StringArray otherOrbiters[numSunOrbiters];  // 2009.02.20 MSVC won't allow this ... tsk tsk
   StringArray otherOrbiters[MAX_SUN_ORBITERS];
   
   for (unsigned int ii = 0; ii < numBodies; ii++)
   {
      // skip the sun for now, as it has the Earth set as its central body, for reference
      if (bodies.at(ii) == star) continue;
      body = ss->GetBody(bodies.at(ii));
      centralBody = body->GetStringParameter(body->GetParameterID("CentralBody"));
      for (unsigned int jj = 0; jj < numSunOrbiters; jj++)
      {
         if (centralBody == sunOrbiters.at(jj))
         {
            otherOrbiters[jj].push_back(bodies.at(ii));
            break;
         }
      }
   }

   AppendItem(itemId, wxString(star.c_str()), starIcon, -1,
              new GmatTreeItemData(wxString(star.c_str()), GmatTree::CELESTIAL_BODY_STAR));
   
   wxTreeItemId theSunOrbiter;

   for (unsigned int kk = 0; kk < numSunOrbiters; kk++)
   {
      GmatTree::ItemType         iType    = GmatTree::CELESTIAL_BODY;
      GmatTree::ResourceIconType iconType = GmatTree::RESOURCE_ICON_MOON;
      std::string bodyName = sunOrbiters.at(kk);
      GetBodyTypeAndIcon(bodyName, iType, iconType);
      theSunOrbiter = AppendItem(itemId, wxString(bodyName.c_str()), iconType, -1,
                                 new GmatTreeItemData(wxString(bodyName.c_str()), iType));
      for (unsigned int mm = 0; mm < (otherOrbiters[kk]).size(); mm++)
      {
         bodyName = otherOrbiters[kk].at(mm);
         GetBodyTypeAndIcon(bodyName, iType, iconType);
         AppendItem(theSunOrbiter, wxString(bodyName.c_str()), iconType, -1,
                    new GmatTreeItemData(wxString(bodyName.c_str()), iType));
      }
   }
}


//------------------------------------------------------------------------------
// void GetItemTypeAndIcon(GmatBase *obj, GmatTree::ItemType &itemType,
//                         GmatTree::ResourceIconType &itemIcon)
//------------------------------------------------------------------------------
/**
 * Returns item type and icon id to use when showing in the resource tree.
 * Notes: Until we figure out how to use user defined icons for plugins,
 *        provide all plugin object icons here.
 */
//------------------------------------------------------------------------------
void ResourceTree::GetItemTypeAndIcon(GmatBase *obj,
                                      GmatTree::ItemType &itemType,
                                      GmatTree::ResourceIconType &itemIcon)
{
   #ifdef DEBUG_RESOURCE_ICON
   MessageInterface::ShowMessage
      ("ResourceTree::GetItemTypeAndIcon() entered, obj=<%p><%s>'%s'\n", obj,
       obj->GetTypeName().c_str(), obj->GetName().c_str());
   #endif
   
   std::string objTypeName = obj->GetTypeName();
   itemType = GmatTree::UNKNOWN_ITEM;
   itemIcon = GmatTree::RESOURCE_ICON_DEFAULT;
   
   // Check in the order that are used more often
   
   // ------------------- Built-in object types
   // Spacecraft
   if (obj->IsOfType("Spacecraft"))
   {
      itemType = GmatTree::SPACECRAFT;
      itemIcon = GmatTree::RESOURCE_ICON_SPACECRAFT;
   }
   // CoordinateSystem
   else if (obj->IsOfType("CoordinateSystem"))
   {
      itemType = GmatTree::USER_COORDINATE_SYSTEM;
      itemIcon = GmatTree::RESOURCE_ICON_COORDINATE_SYSTEM;
   }
   // Propagator
   else if (obj->IsOfType("PropSetup"))
   {
      itemType = GmatTree::PROPAGATOR;
      itemIcon = GmatTree::RESOURCE_ICON_PROPAGATOR;
   }
   // Output
   else if (obj->IsOfType("ReportFile"))
   {
      itemType = GmatTree::REPORT_FILE;
      itemIcon = GmatTree::RESOURCE_ICON_REPORT_FILE;
   }
   else if (obj->IsOfType("EphemerisFile"))
   {
      itemType = GmatTree::EPHEMERIS_FILE;
      itemIcon = GmatTree::RESOURCE_ICON_EPHEMERIS_FILE;
   }
   else if (obj->IsOfType("OrbitView"))
   {
      itemType = GmatTree::ORBIT_VIEW;
      itemIcon = GmatTree::RESOURCE_ICON_ORBIT_VIEW;
   }
   else if (obj->IsOfType("XYPlot"))
   {
      itemType = GmatTree::XY_PLOT;
      itemIcon = GmatTree::RESOURCE_ICON_XY_PLOT;
   }
   else if (obj->IsOfType("GroundTrackPlot"))
   {
      itemType = GmatTree::GROUND_TRACK_PLOT;
      itemIcon = GmatTree::RESOURCE_ICON_GROUND_TRACK_PLOT;
   }
   else if (obj->IsOfType("GroundTrack"))
   {
      itemType = GmatTree::GROUND_TRACK_PLOT;
      itemIcon = GmatTree::RESOURCE_ICON_GROUND_TRACK_PLOT;
   }
   else if (obj->IsOfType("DynamicDataDisplay"))
   {
      itemType = GmatTree::DYNAMIC_DATA_DISPLAY;
      itemIcon = GmatTree::RESOURCE_ICON_SUBSCRIBER;
   }
   // Formation
   else if (obj->IsOfType("Formation"))
   {
      itemType = GmatTree::FORMATION;
      itemIcon = GmatTree::RESOURCE_ICON_FORMATION;
   }
   // GroundStation
   else if (obj->IsOfType("GroundStation"))
   {
      itemType = GmatTree::GROUND_STATION;
      itemIcon = GmatTree::RESOURCE_ICON_GROUND_STATION;
   }
   // Hardware
   else if (obj->IsOfType("ChemicalTank"))
   {
      itemType = GmatTree::FUELTANK_CHEMICAL;
      itemIcon = GmatTree::RESOURCE_ICON_CHEMICAL_TANK;   // need specific one
   }
   else if (obj->IsOfType("ElectricTank"))
   {
      itemType = GmatTree::FUELTANK_ELECTRIC;
      itemIcon = GmatTree::RESOURCE_ICON_ELECTRIC_TANK; // need specific one
   }
   else if (obj->IsOfType("ChemicalThruster"))
   {
      itemType = GmatTree::THRUSTER_CHEMICAL;
      itemIcon = GmatTree::RESOURCE_ICON_CHEMICAL_THRUSTER;  // need specific one
   }
   else if (obj->IsOfType("ElectricThruster"))
   {
      itemType = GmatTree::THRUSTER_ELECTRIC;
      itemIcon = GmatTree::RESOURCE_ICON_ELECTRIC_THRUSTER; // need specific one
   }
   else if (obj->IsOfType("PowerSystem"))
   {
      itemType = GmatTree::POWER_SYSTEM;
      if (obj->IsOfType("SolarPowerSystem"))
         itemIcon = GmatTree::RESOURCE_ICON_SOLAR_POWER_SYSTEM;
      else
         itemIcon = GmatTree::RESOURCE_ICON_NUCLEAR_POWER_SYSTEM;
   }
   // Burn
   else if (obj->IsOfType("ImpulsiveBurn"))
   {
      itemType = GmatTree::IMPULSIVE_BURN;
      itemIcon = GmatTree::RESOURCE_ICON_IMPULSIVE_BURN;
   }
   else if (obj->IsOfType("FiniteBurn"))
   {
      itemType = GmatTree::FINITE_BURN;
      itemIcon = GmatTree::RESOURCE_ICON_FINITE_BURN;
   }
   // SolarSystem
   else if (obj->IsOfType("Barycenter"))
   {
      itemType = GmatTree::BARYCENTER;
      itemIcon = GmatTree::RESOURCE_ICON_BARYCENTER;
   }
   else if (obj->IsOfType("LibrationPoint"))
   {
      itemType = GmatTree::LIBRATION_POINT;
      itemIcon = GmatTree::RESOURCE_ICON_LIBRATION_POINT;
   }
   else if (obj->IsOfType("Comet"))
   {
      itemType = GmatTree::CELESTIAL_BODY_COMET;
      itemIcon = GmatTree::RESOURCE_ICON_COMET;
   }
   else if (obj->IsOfType("Asteroid"))
   {
      itemType = GmatTree::CELESTIAL_BODY_ASTEROID;
      itemIcon = GmatTree::RESOURCE_ICON_ASTEROID;
   }
   // Solver
   else if (obj->IsOfType("DifferentialCorrector"))
   {
      itemType = GmatTree::DIFFERENTIAL_CORRECTOR;
      itemIcon = GmatTree::RESOURCE_ICON_DIFFERENTIAL_CORRECTOR;
   }
   // Variable/Array/String
   else if (obj->IsOfType("Variable") && obj->IsOfType("UserParameter"))
   {
      itemType = GmatTree::VARIABLE;
      itemIcon = GmatTree::RESOURCE_ICON_VARIABLE;
   }
   else if (obj->IsOfType("Array") && obj->IsOfType("UserParameter"))
   {
      itemType = GmatTree::ARRAY;
      itemIcon = GmatTree::RESOURCE_ICON_ARRAY;
   }
   else if (obj->IsOfType("String") && obj->IsOfType("UserParameter"))
   {
      itemType = GmatTree::STRING;
      itemIcon = GmatTree::RESOURCE_ICON_STRING;
   }
   // Functions
   else if (obj->IsOfType("GmatFunction"))
   {
      itemType = GmatTree::GMAT_FUNCTION;
      itemIcon = GmatTree::RESOURCE_ICON_GMAT_FUNCTION;
   }
   else if (obj->IsOfType("MatlabFunction"))
   {
      itemType = GmatTree::MATLAB_FUNCTION;
      itemIcon = GmatTree::RESOURCE_ICON_MATLAB_FUNCTION;
   }
   else if (obj->IsOfType("FileInterface"))
   {
      itemType = GmatTree::INTERFACE;
	  itemIcon = GmatTree::RESOURCE_ICON_FILEINTERFACE;
   }
   
   // ------------------- Plugin objects
   // Hardware
   else if (obj->IsOfType("Antenna"))
   {
      itemType = GmatTree::HARDWARE;
      itemIcon = GmatTree::RESOURCE_ICON_ANTENNA;
   }
   else if (obj->IsOfType("Transmitter"))
   {
      itemType = GmatTree::HARDWARE;
      itemIcon = GmatTree::RESOURCE_ICON_TRANSMITTER;
   }
   else if (obj->IsOfType("Receiver"))
   {
      itemType = GmatTree::HARDWARE;
      itemIcon = GmatTree::RESOURCE_ICON_RECEIVER;
   }
   else if (obj->IsOfType("Transponder"))
   {
      itemType = GmatTree::HARDWARE;
      itemIcon = GmatTree::RESOURCE_ICON_TRANSPONDER;
   }
   
   // Solver
   else if (obj->IsOfType("FminconOptimizer"))
   {
      itemType = GmatTree::SQP;
      itemIcon = GmatTree::RESOURCE_ICON_OPTIMIZER;
   }
   else if (obj->IsOfType("VF13ad"))
   {
      itemType = GmatTree::OPTIMIZER;
      itemIcon = GmatTree::RESOURCE_ICON_OPTIMIZER;
   }
   else if (obj->IsOfType("Optimizer"))
   {
      itemType = GmatTree::OPTIMIZER;
      itemIcon = GmatTree::RESOURCE_ICON_OPTIMIZER;
   }
   else if (obj->IsOfType("Simulator"))
   {
      itemType = GmatTree::SIMULATOR;
      itemIcon = GmatTree::RESOURCE_ICON_SIMULATOR;
   }
   else if (obj->IsOfType("Estimator"))
   {
      itemType = GmatTree::ESTIMATOR;
      itemIcon = GmatTree::RESOURCE_ICON_ESTIMATOR;
   }
   
   // EventLocator
   else if (obj->IsOfType("EventLocator") || obj->IsOfType(Gmat::EVENT_LOCATOR))
   {
      itemType = GmatTree::EVENT_LOCATOR;
      if (obj->IsOfType("EclipseLocator"))
      {
         itemIcon = GmatTree::RESOURCE_ICON_ECLIPSE_LOCATOR;
      }
      else
      {
         itemIcon = GmatTree::RESOURCE_ICON_CONTACT_LOCATOR;
      }
   }
   
   // MeasurementModel
   //else if (obj->IsOfType("MeasurementModel") || obj->IsOfType(Gmat::MEASUREMENT_MODEL))
   else if (obj->IsOfType(Gmat::MEASUREMENT_MODEL))
   {
      itemType = GmatTree::MEASUREMENT_MODEL;
      itemIcon = GmatTree::RESOURCE_ICON_MEASUREMENT_MODEL;
   }
   
   // ------------------- Generic object types
   else if (obj->IsOfType("Hardware"))
   {
      itemType = GmatTree::HARDWARE;
      itemIcon = GmatTree::RESOURCE_ICON_HARDWARE;
   }
   else if (obj->IsOfType("Solver"))
   {
      itemType = GmatTree::SOLVER;
      itemIcon = GmatTree::RESOURCE_ICON_SOLVER;
   }
   else if (obj->IsOfType("Subscriber"))
   {
      itemType = GmatTree::SUBSCRIBER;
      itemIcon = GmatTree::RESOURCE_ICON_SUBSCRIBER;
   }
   
   #ifdef DEBUG_RESOURCE_ICON
   MessageInterface::ShowMessage
      ("ResourceTree::GetItemTypeAndIcon() returning type to use: %d, icon id to use: %d\n",
       itemType, itemIcon);
   #endif
}


//------------------------------------------------------------------------------
// void AddDefaultGroundStation(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default ground station
 *
 * @param <itemId> tree item for the ground station folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultGroundStation(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::GROUND_STATION);
   int size = itemNames.size();
   wxString objName;

   #ifdef DEBUG_ADD_DEFAULT_OBJECTS
   MessageInterface::ShowMessage
      ("ResourceTree::AddDefaultGroundStation() size=%d\n", size);
   #endif

   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());

      #ifdef DEBUG_ADD_DEFAULT_OBJECTS
      MessageInterface::ShowMessage
         ("ResourceTree::AddDefaultGroundStation() objName=%s\n", objName.WX_TO_C_STRING);
      #endif
      
      AppendItem(itemId, objName, GmatTree::RESOURCE_ICON_GROUND_STATION, -1,
                 new GmatTreeItemData(objName, GmatTree::GROUND_STATION));
   };
   
   if (size > 0)
      Expand(itemId);
}


//------------------------------------------------------------------------------
// void AddDefaultSpacecraft(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default spacecraft
 *
 * @param <itemId> tree item for the spacecraft folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSpacecraft(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SPACECRAFT);
   int size = itemNames.size();
   wxString objName;
   
   #ifdef DEBUG_ADD_DEFAULT_OBJECTS
   MessageInterface::ShowMessage
      ("ResourceTree::AddDefaultSpacecraft() size=%d\n", size);
   #endif
   
   if (size > 0)
   {
      GmatBase *obj = GetObject(itemNames[0]);
      GmatTree::ItemType itemType;
      GmatTree::ResourceIconType itemIcon;
      GetItemTypeAndIcon(obj, itemType, itemIcon);
      
      for (int i = 0; i < size; i++)
      {
         objName = wxString(itemNames[i].c_str());
         
         #ifdef DEBUG_ADD_DEFAULT_OBJECTS
         MessageInterface::ShowMessage
            ("ResourceTree::AddDefaultSpacecraft() objName='%s'\n", objName.WX_TO_C_STRING);
         #endif
         
         AppendItem(itemId, objName, itemIcon, -1,
                    new GmatTreeItemData(objName, itemType));
      };
      
      Expand(itemId);
   }
}


//------------------------------------------------------------------------------
// void AddDefaultHardware(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default hardware
 *
 * @param <itemId> tree item for the hardware folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultHardware(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::HARDWARE);
   int size = itemNames.size();
   wxString objName;
   wxString objTypeName;
   
   bool debugHardware = false;
   #ifdef DEBUG_ADD_DEFAULT_OBJECTS
   MessageInterface::ShowMessage
      ("ResourceTree::AddDefaultHardware() size=%d\n", size);
   debugHardware = true;
   #endif
   
   for (int i = 0; i < size; i++)
   {
      GmatBase *obj = GetObject(itemNames[i]);
      #ifndef __ENABLE_NAV_GUI__
      if (obj->IsOfType("Antenna") || obj->IsOfType("RFHardware"))
         continue;
      #endif
      AddObjectToTree(obj, debugHardware);
   };
   
   if (size > 0)
      Expand(itemId);
}


//------------------------------------------------------------------------------
// void AddDefaultFormations(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default formations
 *
 * @param <itemId> tree item for the formation folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultFormations(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::FORMATION);
   int size = itemNames.size();
   wxString objName;
   
   if (size > 0)
   {
      GmatBase *obj = GetObject(itemNames[0]);
      GmatTree::ItemType itemType;
      GmatTree::ResourceIconType itemIcon;
      GetItemTypeAndIcon(obj, itemType, itemIcon);
      
      for (int i = 0; i<size; i++)
      {
         objName = wxString(itemNames[i].c_str());
         
         wxTreeItemId formationItem =
            AppendItem(itemId, objName, itemIcon, -1,
                       new GmatTreeItemData(objName, itemType));
         GmatBase *form = GetObject(itemNames[i].c_str());
         
         // get added spacecrafts
         int scListId = form->GetParameterID("Add");
         StringArray formSc = form->GetStringArrayParameter(scListId);
         int formSize = formSc.size();
         
         if (formSize > 0)
         {
            GmatBase *sc = GetObject(formSc[0]);
            if (sc != NULL)
            {
               GmatTree::ItemType scType;
               GmatTree::ResourceIconType scIcon;
               GetItemTypeAndIcon(sc, scType, scIcon);
            
               for (int j = 0; j<formSize; j++)
               {
                  objName = wxString(formSc[j].c_str());
                  AppendItem(formationItem, objName, scIcon, -1,
                             new GmatTreeItemData(objName, scType));
               }
            }
            else
            {
               MessageInterface::ShowMessage("**** WARNING **** The formation "
                  "member \"%s\" in Formation \"%s\" could not be found.\n",
                  formSc[0].c_str(), itemNames[i].c_str());
            }
            Expand(formationItem);
         }
      }
      Expand(itemId);
   }
}


//------------------------------------------------------------------------------
// void AddDefaultConstellations(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default constellations
 *
 * @param <itemId> tree item for the constellation folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultConstellations(wxTreeItemId itemId)
{
//   wxTreeItemId gps =
//      AppendItem(itemId, wxT("GPS"), -1, -1,
//                 new GmatTreeItemData(wxT("GPS"), GmatTree::CONSTELLATION_FOLDER));
//   SetItemImage(gps, GmatTree::RESOURCE_ICON_OPEN_FOLDER, wxTreeItemIcon_Expanded);
//
//   AppendItem(gps, wxT("GPS1"), GmatTree::RESOURCE_ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS1"), GmatTree::CONSTELLATION_SATELLITE));
//   AppendItem(gps, wxT("GPS2"), GmatTree::RESOURCE_ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS2"), GmatTree::CONSTELLATION_SATELLITE));
//   AppendItem(gps, wxT("GPS3"), GmatTree::RESOURCE_ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS3"), GmatTree::CONSTELLATION_SATELLITE));
//   AppendItem(gps, wxT("GPS4"), GmatTree::RESOURCE_ICON_SPACECRAFT, -1,
//              new GmatTreeItemData(wxT("GPS4"), GmatTree::CONSTELLATION_SATELLITE));
}

//------------------------------------------------------------------------------
// void AddDefaultPropagators(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default propagator
 *
 * @param <itemId> tree item for the propagators folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultPropagators(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames;
   int size;
   wxString objName;
   
   itemNames = theGuiInterpreter->GetListOfObjects(Gmat::PROP_SETUP);
   size = itemNames.size();
   if (size > 0)
   {
      GmatBase *obj = GetObject(itemNames[0]);
      GmatTree::ItemType itemType;
      GmatTree::ResourceIconType itemIcon;
      GetItemTypeAndIcon(obj, itemType, itemIcon);
      
      for (int i = 0; i<size; i++)
      {
         objName = wxString(itemNames[i].c_str());
         AppendItem(itemId, objName, itemIcon, -1,
                    new GmatTreeItemData(objName, itemType));
      };

//   int spkSize = 0;
//   #ifdef __USE_SPICE__
//   itemNames = theGuiInterpreter->GetListOfObjects("SPK");
//   spkSize = itemNames.size();
//   for (int i = 0; i<spkSize; i++)
//   {
//      objName = wxString(itemNames[i].c_str());
//      AppendItem(itemId, wxT(objName), GmatTree::RESOURCE_ICON_PROPAGATOR, -1,
//                 new GmatTreeItemData(wxT(objName), GmatTree::SPK_PROPAGATOR));
//   };
//   #endif
      
      //if (size > 0) // || spkSize > 0)
      Expand(itemId);
   }
}


//------------------------------------------------------------------------------
// void AddDefaultLocators(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the event locators
 *
 * @param <itemId> tree item for the event locators folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultLocators(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames;
   int size;
   wxString objName;

   itemNames = theGuiInterpreter->GetListOfObjects(Gmat::EVENT_LOCATOR);
   size = itemNames.size();

   if (size > 0)
   {
      for (int i = 0; i<size; i++)
      {
         GmatBase *obj = GetObject(itemNames[i]);
         AddObjectToTree(obj);
      };
      
      Expand(itemId);
   }
}

//------------------------------------------------------------------------------
// void AddDefaultBurns(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default burns
 *
 * @param <itemId> tree item for the burn folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultBurns(wxTreeItemId itemId, bool restartCounter)
{
   #ifdef DEBUG_DEFAULT_BURNS
   MessageInterface::ShowMessage("ResourceTree::AddDefaultBurns() entered\n");
   #endif

   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::BURN);
   int size = itemNames.size();
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *obj = GetObject(itemNames[i]);
      AddObjectToTree(obj);
   }
   
   if (size > 0)
      Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultSolvers(wxTreeItemId itemId, bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default solvers
 *
 * @param <itemId> tree item for the solvers folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSolvers(wxTreeItemId itemId, bool restartCounter)
{
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage
      ("ResourceTree::AddDefaultSolvers() entered, restartCounter=%d\n", restartCounter);
   #endif
   
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SOLVER);
   int size = itemNames.size();
   
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("   There are %d configured Solvers\n", size);
   #endif
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *obj = GetObject(itemNames[i]);
      AddObjectToTree(obj);
   };
   
   if (size > 0)
   {
      Expand(mBoundarySolverItem);
      Expand(mOptimizerItem);
      Expand(itemId);
   }
   
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("ResourceTree::AddDefaultSolvers() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void AddDefaultSubscribers(wxTreeItemId itemId, bool restartCounter)
//------------------------------------------------------------------------------
/**
 * Add the default subscribers
 *
 * @param <itemId> tree item for the subscribers folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSubscribers(wxTreeItemId itemId, bool restartCounter)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::SUBSCRIBER);
   int size = itemNames.size();

   for (int i = 0; i<size; i++)
   {
      GmatBase *obj = GetObject(itemNames[i]);
      AddObjectToTree(obj);
   }

   if (size > 0)
      Expand(itemId);
}


//------------------------------------------------------------------------------
// void AddDefaultInterfaces(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default interfaces
 *
 * @param <itemId> tree item for the interfaces folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultInterfaces(wxTreeItemId itemId)
{
   if (GmatGlobal::Instance()->IsMatlabAvailable())
   {
      AppendItem(itemId, wxT("Matlab"), GmatTree::RESOURCE_ICON_MATLAB, -1,
                 new GmatTreeItemData(wxT("Matlab"), GmatTree::MATLAB_INTERFACE));

      // Show MatlabServer if testing mode (LOJ: 2014.09.24)
      //if ((GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING) ||              // made changes by TUAN NGUYEN
      //    (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS))       // made changes by TUAN NGUYEN
      if ((GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING) ||         // made changes by TUAN NGUYEN
         (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING_NO_PLOTS))   // made changes by TUAN NGUYEN
      {
         //#ifdef __ADD_MATLAB_SERVER__         
         // Hide showing Matlab Server from the resource tree (GMT-3320)
         // @todo It should be called Gmat Server instead of Matlab Server since
         // GMAT is providing a service to MATLAB. (LOJ: 2013.02.19)
         AppendItem(itemId, wxT("Matlab Server"), GmatTree::RESOURCE_ICON_MATLAB_SERVER, -1,
                    new GmatTreeItemData(wxT("Matlab Server"), GmatTree::MATLAB_SERVER));
         //#endif
      }
   }
   
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::INTERFACE);
   int size = itemNames.size();
   
   for (int i = 0; i < size; i++)
   {
      GmatBase *obj = GetObject(itemNames[i]);
      AddObjectToTree(obj);
   }

   Expand(itemId);
}


//------------------------------------------------------------------------------
// void AddDefaultVariables(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default variables
 *
 * @param <itemId> tree item for the variables folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultVariables(wxTreeItemId itemId)
{
   StringArray itemNames = theGuiInterpreter->GetListOfObjects(Gmat::PARAMETER);
   int size = itemNames.size();
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *obj = GetObject(itemNames[i]);
      AddObjectToTree(obj);
   };
   
   if (size > 0)
      Expand(itemId);
}

//------------------------------------------------------------------------------
// void AddDefaultFunctions(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default interfaces
 *
 * @param <itemId> tree item for the interfaces folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultFunctions(wxTreeItemId itemId)
{
   StringArray itemNames =
      GmatAppData::Instance()->GetGuiInterpreter()->GetListOfObjects(Gmat::FUNCTION);
   int size = itemNames.size();
   
   for (int i = 0; i<size; i++)
   {
      GmatBase *obj = GetObject(itemNames[i]);
      AddObjectToTree(obj);
   }
   
   //----- Predefined functions
   //mPredefinedFunctItem =
   //   AppendItem(itemId, wxT("Predefined Functions"), GmatTree::RESOURCE_ICON_FOLDER, -1,
   //              new GmatTreeItemData(wxT("Functions"),
   //                                   GmatTree::PREDEFINED_FUNCTION_FOLDER));

   //SetItemImage(mPredefinedFunctItem, GmatTree::RESOURCE_ICON_OPEN_FOLDER,
   //             wxTreeItemIcon_Expanded);
   
   if (size > 0)
      Expand(itemId);

}

//------------------------------------------------------------------------------
// void AddDefaultCoordSys(wxTreeItemId itemId)
//------------------------------------------------------------------------------
/**
 * Add the default coordinate system
 *
 * @param <itemId> tree item for the coordinate system folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultCoordSys(wxTreeItemId itemId)
{
   StringArray itemNames = GmatAppData::Instance()->GetGuiInterpreter()->
      GetListOfObjects(Gmat::COORDINATE_SYSTEM);
   int size = itemNames.size();
   wxString objName;
   
   for (int i = 0; i < size; i++)
   {
      objName = wxString(itemNames[i].c_str());
      if (objName == "EarthMJ2000Eq" || objName == "EarthMJ2000Ec" ||
          objName == "EarthFixed" || objName == "EarthICRF")
      {
         AppendItem(itemId, objName, GmatTree::RESOURCE_ICON_COORDINATE_SYSTEM, -1,
                    new GmatTreeItemData(objName, GmatTree::PREDEFINED_COORDINATE_SYSTEM));
      }
      else
      {
         AppendItem(itemId, objName, GmatTree::RESOURCE_ICON_COORDINATE_SYSTEM, -1,
                    new GmatTreeItemData(objName, GmatTree::USER_COORDINATE_SYSTEM));
      }
   };
   
   if (size > 0)
      Expand(itemId);

}

//------------------------------------------------------------------------------
// void AddDefaultSpecialPoints(wxTreeItemId itemId, bool incLibCounter = true,
//                              bool restartCounter = true)
//------------------------------------------------------------------------------
/**
 * Add the default special points
 *
 * @param <itemId> tree item for the special points folder
 */
//------------------------------------------------------------------------------
void ResourceTree::AddDefaultSpecialPoints(wxTreeItemId itemId, bool incLibCounter,
                                           bool restartCounter)
{
   StringArray itemNames =
      GmatAppData::Instance()->GetGuiInterpreter()->GetListOfObjects(Gmat::CALCULATED_POINT);
   int size = itemNames.size();
   wxString objName;
   
   for (int i = 0; i<size; i++)
   {
      objName = wxString(itemNames[i].c_str());
      if ((objName == "SSB") || (objName == "SolarSystemBarycenter"))
      {
         AppendItem(itemId, objName, GmatTree::RESOURCE_ICON_BARYCENTER, -1,
                    new GmatTreeItemData(objName, GmatTree::PREDEFINED_BARYCENTER));
      }
      else
      {
         GmatBase *obj = GetObject(itemNames[i]);
         AddObjectToTree(obj);
      }
   };
   
   if (size > 0)
      Expand(itemId);

}


//------------------------------------------------------------------------------
// void ResourceTree::AddUserObjects()
//------------------------------------------------------------------------------
/**
 * Handle the objects not covered by the defaults
 */
//------------------------------------------------------------------------------
void ResourceTree::AddUserObjects()
{
   #ifdef DEBUG_USER_GUI
   MessageInterface::ShowMessage("ResourceTree::AddUserObjects() entered\n");
   #endif
   
   StringArray itemNames;
   UnsignedInt type;
   std::string subtype;
   wxString objName;
   wxString objTypeName;
   wxTreeItemId itemId;

   #ifdef DEBUG_USER_GUI
      MessageInterface::ShowMessage("   Found %d user nodes\n",
            mPluginItems.size());
   #endif
   for (UnsignedInt j = 0; j < mPluginItems.size(); ++j)
   {
      itemId = mPluginItems[j];
      type = nodeTypeMap[std::string(GetItemText(itemId).c_str())];
      subtype = nodeSubtypeMap[std::string(GetItemText(itemId).c_str())];

      itemNames =
            GmatAppData::Instance()->GetGuiInterpreter()->GetListOfObjects(type);
      int size = itemNames.size();

      #ifdef DEBUG_USER_GUI
         MessageInterface::ShowMessage("   Adding %d type %d subtype \"%s\" "
               "objects\n", size, type, subtype.c_str());
      #endif

      for (int i = 0; i < size; i++)
      {
         #ifdef DEBUG_USER_GUI
            MessageInterface::ShowMessage("   Object[%d] \"%s\"\n", i,
                  itemNames[i].c_str());
         #endif
         GmatBase *cp = GetObject(itemNames[i]);
         GmatTree::ItemType itemType;
         GmatTree::ResourceIconType itemIcon;
         GetItemTypeAndIcon(cp, itemType, itemIcon);
         #ifdef DEBUG_USER_GUI
         MessageInterface::ShowMessage
            ("   itemType = %d, itemIcon = %d\n", itemType, itemIcon);
         #endif
         if (subtype == "")
         {
            objName = wxString(itemNames[i].c_str());
            objTypeName = wxString(cp->GetTypeName().c_str());

            #ifdef DEBUG_USER_GUI
            MessageInterface::ShowMessage
               ("   => Adding '%s' of type '%s'\n", objName.WX_TO_C_STRING, objTypeName.WX_TO_C_STRING);
            #endif
            // Changed to use itemType since Rename() will not work on USER_DEFINED_OBJECT tree item
            // which will assign to UNKNOWN_OBJECT (LOJ: 2012.03.20)
            AppendItem(itemId, objName, itemIcon, -1,
                       new GmatTreeItemData(objName, itemType));
            //GmatTree::USER_DEFINED_OBJECT));
         }
         else if(cp->IsOfType(subtype))
         {
            objName = wxString(itemNames[i].c_str());
            objTypeName = wxString(cp->GetTypeName().c_str());
            
            #ifdef DEBUG_USER_GUI
            MessageInterface::ShowMessage
               ("   => Adding '%s' of type '%s'\n", objName.WX_TO_C_STRING, objTypeName.WX_TO_C_STRING);
            #endif
            // Changed to use itemType since Rename() will not work on USER_DEFINED_OBJECT tree item
            // which will assign to UNKNOWN_OBJECT (LOJ: 2012.03.20)
            AppendItem(itemId, objName, itemIcon, -1,
                       new GmatTreeItemData(objName, itemType));
            //GmatTree::USER_DEFINED_OBJECT));
         }
      }
      
      if (size > 0)
         Expand(itemId);
   }
   #ifdef DEBUG_USER_GUI
   MessageInterface::ShowMessage("ResourceTree::AddUserObjects() leaving\n");
   #endif
}

//==============================================================================
//                         On Action Events
//==============================================================================

//------------------------------------------------------------------------------
// void OnItemRightClick(wxTreeEvent& event)
//------------------------------------------------------------------------------
/**
 * On right click show the pop up menu
 */
//------------------------------------------------------------------------------
void ResourceTree::OnItemRightClick(wxTreeEvent& event)
{
   SelectItem(event.GetItem());
   ShowMenu(event.GetItem(), event.GetPoint());
}


//------------------------------------------------------------------------------
// void OnItemActivated(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Double click on tree item
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnItemActivated(wxTreeEvent &event)
{
   // get some info about this item
   wxTreeItemId itemId = event.GetItem();
   GmatTreeItemData *item = (GmatTreeItemData *)GetItemData(itemId);
   #ifdef DEBUG_ITEM_ACTIVATED
   MessageInterface::ShowMessage
      ("ResourceTree::OnItemActivated() title='%s'\n   name='%s'\n",
       item->GetTitle().c_str(), item->GetName().c_str());
   #endif
   theMainFrame->CreateChild(item);
}


//------------------------------------------------------------------------------
// void OnItemExpanded(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * On tree item expanded
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnItemExpanded(wxTreeEvent &event)
{
   #ifdef DEBUG_ITEM_EXPANDED
   // Get some info about this item
   wxTreeItemId itemId = event.GetItem();
   GmatTreeItemData *item = (GmatTreeItemData *)GetItemData(itemId);
   MessageInterface::ShowMessage
      ("ResourceTree::OnItemExpanded() title='%s'\n   name='%s'\n",
       item->GetTitle().WX_TO_C_STRING, item->GetName().WX_TO_C_STRING);
   #endif
   
   // Why first item is selected when item is expanded for the first time?
   // Unselect() didn't fix the problem
   // Well this is the same behavior as with wx-2.8
   //Unselect();
}


//------------------------------------------------------------------------------
// void OnOpen(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Open chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnOpen(wxCommandEvent &event)
{
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   theMainFrame->CreateChild(item);
}


//------------------------------------------------------------------------------
// void OnClose(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Close chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnClose(wxCommandEvent &event)
{
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());
   // if its open, its activated
   if (theMainFrame->IsChildOpen(item))
      // close the window
      theMainFrame->CloseActiveChild();
   else
      return;
}


//------------------------------------------------------------------------------
// void OnRename(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Rename chosen from popup menu
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnRename(wxCommandEvent &event)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("ResourceTree::OnRename() entered\n");
   #endif
   
   wxTreeItemId itemId = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(itemId);
   wxString oldName = selItemData->GetName();
   GmatTree::ItemType itemType = selItemData->GetItemType();
   wxTreeItemId parentId = GetItemParent(itemId);
   
   wxString newName = oldName;
   wxString name;
   if (GetNameFromUser(name, oldName, "Enter new name:", "Rename") == 1)
      newName = name;
#ifdef DEBUG_RENAME
MessageInterface::ShowMessage("ResourceTree::OnRename() oldName = %s, newName = %s\n",
      oldName.c_str(), newName.c_str());
#endif
   
   if ( !newName.IsEmpty() && !(newName.IsSameAs(oldName)))
   {
      UnsignedInt objType = GetObjectType(itemType);

      #ifdef DEBUG_RENAME
      MessageInterface::ShowMessage
         ("ResourceTree::OnRename() itemType = %d, objType = %d\n",
          itemType, objType);
      #endif

      if (objType == Gmat::UNKNOWN_OBJECT)
         return;
      
      // If user wants to save data from the currently opened panels
      if (theMainFrame->GetNumberOfChildOpen() > 0)
      {
         if (wxMessageBox("GMAT will save data from the currently opened "
                          "panels first.\nDo you want to continue?",
                          "Please confirm", wxICON_QUESTION | wxYES_NO) == wxYES)
         {
            // save any GUI data before renaming object from base code
            if (!theGuiManager->PrepareObjectNameChange())
            {
               wxMessageBox("GMAT cannot continue with Rename.\nPlease fix problem "
                            "with the current panel before renaming.",
                            "GMAT Warning");
               return;
            }
         }
         else
         {
            return;
         }
      }
      
      // update item only if successful
      if (theGuiInterpreter->
          RenameObject(objType, oldName.c_str(), newName.c_str()))
      {
         SetItemText(itemId, newName);
         theMainFrame->RenameChild(selItemData, newName);
         GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(itemId);
         
         // Set item name to new name and title to "" so that GmatMainFrame::CreateChild()
         // can append object type name to title (loj: 2009.06.08, Bug fix: 1478, 1479)
         selItemData->SetTitle("");
         selItemData->SetName(newName);
         
         // Update and notify object name changed to panels which listens to resource update
         // Changed to UpdateAll() for GMT-2622 fix (LOJ: 2012.05.08)
         //UpdateGuiItem(itemType);
         theGuiManager->UpdateAll(objType);
         theGuiManager->NotifyObjectNameChange(objType, oldName, newName);

         PluginItemManager::Instance()->RenameObject(std::string(oldName.mb_str()),
               std::string(newName.mb_str()), objType);

         // Handle potential FM name change due to PropSetup name change
         if ((objType == Gmat::PROPAGATOR) || (objType == Gmat::PROP_SETUP))
            theGuiManager->UpdateForceModel(true);

         if (objType == Gmat::SPACECRAFT)
         {
            // Why refresh spacecraft items?
            // Commented out (LOJ: 2012.05.08)
            //Collapse(mSpacecraftItem);
            //DeleteChildren(mSpacecraftItem);
            //AddDefaultSpacecraft(mSpacecraftItem);
            
            // update formation which may use new spacecraft name
            if(mFormationItem.IsOk())
            {
               Collapse(mFormationItem);
               DeleteChildren(mFormationItem);
               AddDefaultFormations(mFormationItem);
            }
         }
         
         // update variables which may use new object name
         Collapse(mVariableItem);
         DeleteChildren(mVariableItem);
         AddDefaultVariables(mVariableItem);
         
         // select renamed item
         SelectItem(FindIdOfNode(newName, parentId));
         
         // Set GUI dirty (LOJ: 2012.05.18)
         theMainFrame->UpdateGuiScriptSyncStatus(2, 0);
         
         // update MissionTree for resource rename
         GmatAppData::Instance()->GetMissionTree()->UpdateMissionForRename();
      }
      else
      {
         // MessageInterface::ShowMessage
         //    ("ResourceTree::OnRename() Unable to rename %s to %s.\n",
         //     oldName.WX_TO_C_STRING, newName.WX_TO_C_STRING);
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "ResourceTree cannot rename '%s' to '%s' due to "
             "internal error. See message window for details.\n",
             oldName.WX_TO_C_STRING, newName.WX_TO_C_STRING);
      }
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage("ResourceTree::OnRename() rename leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnDelete(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Delete chosen from popup menu
 *
 * @param <event> command event
 * @todo Finish this when items can be deleted from interpreter
 */
//------------------------------------------------------------------------------
void ResourceTree::OnDelete(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(itemId);
   #ifdef DEBUG_DELETE
   if (selItemData == NULL)
      MessageInterface::ShowMessage("selItemData is NULL!!!!!!!\n");
   MessageInterface::ShowMessage
      ("ResourceTree::OnDelete() entered, name='%s'\n", selItemData->GetName().WX_TO_C_STRING);
   #endif
   GmatTree::ItemType itemType = selItemData->GetItemType();
   wxString itemName = selItemData->GetName();
   
   // if panel is currently opened, give warning and return
   // Bug 547 fix (loj: 2008.11.25)
   if (theMainFrame->IsChildOpen(selItemData))
   {
      wxLogWarning("%s %s", selItemData->GetName().c_str(), "cannot be deleted "
                   "while panel is opened");
      wxLog::FlushActive();
      return;
   }
   
   int answer = wxMessageBox(wxT("Are you sure you want to delete " + selItemData->GetName() + "?"),
                         wxT("Please confirm"), wxYES_NO);
   if (answer == wxNO) return;

   UnsignedInt objType = GetObjectType(itemType);
   if (objType == Gmat::UNKNOWN_OBJECT)
      return;
   
   #ifdef DEBUG_DELETE
   MessageInterface::ShowMessage
      ("ResourceTree::OnDelete() now deleting name='%s'\n",
       selItemData->GetName().WX_TO_C_STRING);
   #endif
   
   // delete item if object successfully deleted
   if (theGuiInterpreter->RemoveObjectIfNotUsed(objType, itemName.c_str()))
   {
      // delete item and close all opened windows
      Delete(itemId);
      
      // We don't want to delete all children (bug 547 fix, loj: 2008.11.25)
      //theMainFrame->CloseAllChildren(false, true);
      
      #ifdef DEBUG_DELETE
      MessageInterface::ShowMessage
         ("ResourceTree::OnDelete() now calling theGuiManager->UpdateAll(%d)\n",
          objType);
      #endif
      theGuiManager->UpdateAll(objType);
      
      // Remove output also
      theMainFrame->RemoveOutput(itemName);
      
      // Set GUI dirty (LOJ: 2012.05.18)
      theMainFrame->UpdateGuiScriptSyncStatus(2, 0);
   }
   else
   {
      wxLogWarning("\"%s\" %s", selItemData->GetName().c_str(), "cannot be "
            "deleted.\nIt is currently used in a resource or in the command "
            "sequence");
      wxLog::FlushActive();
   }
   #ifdef DEBUG_DELETE
   MessageInterface::ShowMessage("ResourceTree::OnDelete() exiting\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnClone(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Clone chosen item
 *
 * @param <event> command event
 * @todo Finish this when all items can be cloned
 */
//------------------------------------------------------------------------------
void ResourceTree::OnClone(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   wxString name = selItemData->GetName();
   GmatTree::ItemType itemType = selItemData->GetItemType();

   #ifdef DEBUG_CLONE
   MessageInterface::ShowMessage
      ("ResourceTree::OnClone() entered, cloning '%s'\n", name.c_str());
   #endif
   
   // Do we need this listing?
   // The ShowMenu() already determins whether the item can be cloned or not
   // but using a new method GmatTreeItemData::IsClonable() instead. (LOJ: 2012.03.05)
   // I believe most of base creatable classes implemented Clone() method.
   
   // Disallow clone for Propagator until cloning works for Propagator (GMT-2627)
   if ((selItemData->IsClonable()) && (itemType != GmatTree::PROPAGATOR))
   {
      const std::string stdName = name.WX_TO_STD_STRING;
      std::string cloneName;
      GmatBase *clonedObj = theGuiInterpreter->AddClone(stdName, cloneName);
      
      #ifdef DEBUG_CLONE
      MessageInterface::ShowMessage("cloned name = '%s'\n", cloneName.c_str());
      #endif
      
      if (cloneName != "")
      {
         wxTreeItemId itemId = AddObjectToTree(clonedObj);
         SelectItem(GetLastChild(itemId));
         
         UpdateGuiItem(itemType);
         
         // Set GUI dirty (LOJ: 2012.05.18)
         theMainFrame->UpdateGuiScriptSyncStatus(2, 0);
      }
      else
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "Cannot clone the object named '%'s.\n", stdName.c_str());
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "Cannot clone this object type yet.\n");
   }
   
   #ifdef DEBUG_CLONE
   MessageInterface::ShowMessage("ResourceTree::OnClone() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnBeginLabelEdit(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Rename chosen from popup menu
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnBeginLabelEdit(wxTreeEvent &event)
{

   GmatTreeItemData *selItemData = (GmatTreeItemData *)
      GetItemData(event.GetItem());

   int itemType = selItemData->GetItemType();
   bool isDefaultFolder = ((itemType == GmatTree::RESOURCES_FOLDER)     ||
                           (itemType == GmatTree::GROUND_STATION_FOLDER)||
                           (itemType == GmatTree::SPACECRAFT_FOLDER)    ||
                           (itemType == GmatTree::HARDWARE_FOLDER)      ||
                           (itemType == GmatTree::FORMATION_FOLDER)     ||
                           (itemType == GmatTree::CONSTELLATION_FOLDER) ||
                           (itemType == GmatTree::BURN_FOLDER)          ||
                           (itemType == GmatTree::PROPAGATOR_FOLDER)    ||
                           (itemType == GmatTree::SOLAR_SYSTEM)         ||
                           (itemType == GmatTree::SOLVER_FOLDER)        ||
                           (itemType == GmatTree::SUBSCRIBER_FOLDER)    ||
                           (itemType == GmatTree::INTERFACE_FOLDER)     ||
                           (itemType == GmatTree::VARIABLE_FOLDER));

   bool isDefaultItem = ((itemType == GmatTree::PROPAGATOR)       ||
//                         (itemType == GmatTree::SPK_PROPAGATOR)   ||
                         (itemType == GmatTree::CELESTIAL_BODY)   ||
                         (itemType == GmatTree::DIFFERENTIAL_CORRECTOR)        ||
                         (itemType == GmatTree::REPORT_FILE)      ||
                         (itemType == GmatTree::XY_PLOT)          ||
                         (itemType == GmatTree::ORBIT_VIEW)       ||
                         (itemType == GmatTree::GROUND_TRACK_PLOT)||
                         (itemType == GmatTree::EPHEMERIS_FILE)   ||
                         (itemType == GmatTree::INTERFACE));

   //kind of redundant because OpenPage returns false for some
   //of the default folders
   if ((theMainFrame->IsChildOpen(selItemData))  ||
       (isDefaultFolder)                         ||
       (isDefaultItem))
   {
      event.Veto();
   }
}

//------------------------------------------------------------------------------
// void OnEndLabelEdit(wxTreeEvent &event)
//------------------------------------------------------------------------------
/**
 * Finished changing label on the tree
 *
 * @param <event> tree event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnEndLabelEdit(wxTreeEvent &event)
{
   wxString newLabel = event.GetLabel();

   // check to see if label is a single word
   if (newLabel.IsWord())
   {
      GmatTreeItemData *selItemData = (GmatTreeItemData *)
         GetItemData(GetSelection());

      wxString oldLabel = selItemData->GetName();
      int itemType = selItemData->GetItemType();

      selItemData->SetTitle(newLabel);

      // if label refers to an object reset the object name
      if (itemType == GmatTree::SPACECRAFT)
      {
         const std::string stdOldLabel = oldLabel.WX_TO_STD_STRING;
         const std::string stdNewLabel = newLabel.WX_TO_STD_STRING;

         //Spacecraft *theSpacecraft =
         //   theGuiInterpreter->GetSpacecraft(stdOldLabel);

         GmatBase *theSpacecraft = GetObject(stdOldLabel);

         theSpacecraft->SetName(stdNewLabel);

         // if (resetName)
         //     selItemData->SetTitle(label);
         //  else
         //     selItemData->SetTitle(oldLabel);

      }


   }
   else
   {
      event.Veto();
   }
}

//------------------------------------------------------------------------------
// void OnBeginDrag(wxTreeEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnBeginDrag(wxTreeEvent& event)
{
   // need to explicitly allow drag
   if ( event.GetItem() != GetRootItem() )
   {
      mDraggedItem = event.GetItem();

      // Get info from selected item
      GmatTreeItemData *theItem = (GmatTreeItemData *) GetItemData(mDraggedItem);
      int draggedId = theItem->GetItemType();

      if ((draggedId == GmatTree::SPACECRAFT )            ||
          (draggedId == GmatTree::FORMATION_SPACECRAFT ))
      {
         event.Allow();
      }
      else
      {
         event.Skip();
      }
   }
}


//------------------------------------------------------------------------------
// void OnEndDrag(wxTreeEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnEndDrag(wxTreeEvent& event)
{
   wxTreeItemId itemSrc = mDraggedItem,
      itemDst = event.GetItem();
   mDraggedItem = (wxTreeItemId)0l;

   if ( !itemDst.IsOk())
   {
      // error
      return;
   }
   
   // Get info from selected item
   GmatTreeItemData *theItem = (GmatTreeItemData *) GetItemData(itemDst);
   int destId = theItem->GetItemType();

   if ((destId == GmatTree::FORMATION )  ||
       (destId == GmatTree::SPACECRAFT_FOLDER ))
   {
      wxString text = GetItemText(itemSrc);

      AppendItem(itemDst, text, GmatTree::RESOURCE_ICON_SPACECRAFT, -1,
                 new GmatTreeItemData(text, GmatTree::SPACECRAFT));

      if (GetChildrenCount(mSpacecraftItem) <= 1)
      {
         Collapse(mSpacecraftItem);
      }

      Delete(itemSrc);
   }
}


//------------------------------------------------------------------------------
// void AddIcons()
//------------------------------------------------------------------------------
/**
 * Add icons for items in the tree
 */
//------------------------------------------------------------------------------
void ResourceTree::AddIcons()
{
   #ifdef DEBUG_ADD_ICONS
   MessageInterface::ShowMessage
      ("ResourceTree::AddIcons() entered, GmatTree::RESOURCE_ICON_COUNT=%d\n",
       GmatTree::RESOURCE_ICON_COUNT);
   #endif
   
   int sizeW = 16;
   int sizeH = 16;
   
   wxImageList *images = new wxImageList ( sizeW, sizeH, true );
   wxBitmap* bitmaps[GmatTree::RESOURCE_ICON_COUNT];
   int index = 0;
   long bitmapType = wxBITMAP_TYPE_PNG;
   
   // Note:
   // Add icons by the order of enum ResourceIconType in GmatTreeItemData.hpp
   
   theGuiManager->LoadIcon("ClosedFolder", bitmapType, &bitmaps[index], ClosedFolder_xpm); 
   theGuiManager->LoadIcon("file", bitmapType, &bitmaps[++index], file_xpm);   
   theGuiManager->LoadIcon("OpenFolder", bitmapType, &bitmaps[++index], OpenFolder_xpm);
   
   theGuiManager->LoadIcon("rt_Spacecraft", bitmapType, &bitmaps[++index], rt_Spacecraft_xpm);
   theGuiManager->LoadIcon("rt_Formation", bitmapType, &bitmaps[++index], rt_Formation_xpm);
   theGuiManager->LoadIcon("rt_GroundStation", bitmapType, &bitmaps[++index], rt_GroundStation_xpm);   
   
   theGuiManager->LoadIcon("rt_Hardware", bitmapType, &bitmaps[++index], rt_Hardware_xpm);
   theGuiManager->LoadIcon("rt_ChemicalTank", bitmapType, &bitmaps[++index], rt_ChemicalTank_xpm); 
   theGuiManager->LoadIcon("rt_ElectricTank", bitmapType, &bitmaps[++index], rt_ElectricTank_xpm); 
   theGuiManager->LoadIcon("rt_ChemicalThruster", bitmapType, &bitmaps[++index], rt_ChemicalThruster_xpm); 
   theGuiManager->LoadIcon("rt_ElectricThruster", bitmapType, &bitmaps[++index], rt_ElectricThruster_xpm); 
   theGuiManager->LoadIcon("rt_SolarPowerSystem", bitmapType, &bitmaps[++index], rt_SolarPowerSystem_xpm); 
   theGuiManager->LoadIcon("rt_NuclearPowerSystem", bitmapType, &bitmaps[++index], rt_NuclearPowerSystem_xpm); 
   theGuiManager->LoadIcon("rt_Antenna", bitmapType, &bitmaps[++index], rt_Antenna_xpm);
   theGuiManager->LoadIcon("rt_Transmitter", bitmapType, &bitmaps[++index], rt_Transmitter_xpm);
   theGuiManager->LoadIcon("rt_Receiver", bitmapType, &bitmaps[++index], rt_Receiver_xpm);
   theGuiManager->LoadIcon("rt_Transponder", bitmapType, &bitmaps[++index], rt_Transponder_xpm);
   
   theGuiManager->LoadIcon("rt_ImpulsiveBurn", bitmapType, &bitmaps[++index], rt_ImpulsiveBurn_xpm);
   theGuiManager->LoadIcon("rt_FiniteBurn", bitmapType, &bitmaps[++index], rt_FiniteBurn_xpm);
   
   theGuiManager->LoadIcon("rt_Propagator", bitmapType, &bitmaps[++index], rt_Propagator_xpm);
   
   theGuiManager->LoadIcon("rt_SolarSystem", bitmapType, &bitmaps[++index], rt_SolarSystem_xpm);
   theGuiManager->LoadIcon("sun", bitmapType, &bitmaps[++index], sun_xpm);
   theGuiManager->LoadIcon("mercury", bitmapType, &bitmaps[++index], mercury_xpm);
   theGuiManager->LoadIcon("venus", bitmapType, &bitmaps[++index], venus_xpm);
   theGuiManager->LoadIcon("earth", bitmapType, &bitmaps[++index], earth_xpm);
   theGuiManager->LoadIcon("mars", bitmapType, &bitmaps[++index], mars_xpm);
   theGuiManager->LoadIcon("jupiter", bitmapType, &bitmaps[++index], jupiter_xpm);
   theGuiManager->LoadIcon("saturn", bitmapType, &bitmaps[++index], saturn_xpm);
   theGuiManager->LoadIcon("uranus", bitmapType, &bitmaps[++index], uranus_xpm);
   theGuiManager->LoadIcon("neptune", bitmapType, &bitmaps[++index], neptune_xpm);
   theGuiManager->LoadIcon("pluto", bitmapType, &bitmaps[++index], pluto_xpm);
   
   theGuiManager->LoadIcon("moon", bitmapType, &bitmaps[++index], moon_xpm);
   theGuiManager->LoadIcon("moon_generic", bitmapType, &bitmaps[++index], moon_generic_xpm);
   theGuiManager->LoadIcon("planet_generic", bitmapType, &bitmaps[++index], planet_generic_xpm);
   theGuiManager->LoadIcon("comet", bitmapType, &bitmaps[++index], comet_xpm);
   theGuiManager->LoadIcon("asteroid", bitmapType, &bitmaps[++index], asteroid_xpm);
   theGuiManager->LoadIcon("rt_Barycenter", bitmapType, &bitmaps[++index], rt_Barycenter_xpm);
   theGuiManager->LoadIcon("rt_LibrationPoint", bitmapType, &bitmaps[++index], rt_LibrationPoint_xpm);
   
   theGuiManager->LoadIcon("rt_Solver", bitmapType, &bitmaps[++index], rt_Solver_xpm);
   theGuiManager->LoadIcon("rt_BoundaryValueSolver", bitmapType, &bitmaps[++index], rt_BoundaryValueSolver_xpm);
   theGuiManager->LoadIcon("rt_DifferentialCorrector", bitmapType, &bitmaps[++index], rt_DifferentialCorrector_xpm);
   theGuiManager->LoadIcon("rt_Optimizer", bitmapType, &bitmaps[++index], rt_Optimizer_xpm);
   theGuiManager->LoadIcon("rt_Simulator", bitmapType, &bitmaps[++index], rt_Simulator_xpm);
   theGuiManager->LoadIcon("rt_Estimator", bitmapType, &bitmaps[++index], rt_Estimator_xpm);
   
   theGuiManager->LoadIcon("rt_Subscriber", bitmapType, &bitmaps[++index], rt_Subscriber_xpm);
   theGuiManager->LoadIcon("rt_ReportFile", bitmapType, &bitmaps[++index], rt_ReportFile_xpm);
   theGuiManager->LoadIcon("rt_EphemerisFile", bitmapType, &bitmaps[++index], rt_EphemerisFile_xpm);
   theGuiManager->LoadIcon("rt_OrbitView", bitmapType, &bitmaps[++index], rt_OrbitView_xpm);
   theGuiManager->LoadIcon("rt_XYPlot", bitmapType, &bitmaps[++index], rt_XYPlot_xpm);
   theGuiManager->LoadIcon("rt_GroundTrackPlot", bitmapType, &bitmaps[++index], rt_GroundTrackPlot_xpm);
   
   theGuiManager->LoadIcon("rt_Matlab", bitmapType, &bitmaps[++index], rt_Matlab_xpm);
   theGuiManager->LoadIcon("rt_MatlabServer", bitmapType, &bitmaps[++index], rt_MatlabServer_xpm);
   theGuiManager->LoadIcon("rt_Script", bitmapType, &bitmaps[++index], rt_Script_xpm);
   
   theGuiManager->LoadIcon("rt_Variable", bitmapType, &bitmaps[++index], rt_Variable_xpm);
   theGuiManager->LoadIcon("rt_Array", bitmapType, &bitmaps[++index], rt_Array_xpm);
   theGuiManager->LoadIcon("rt_String", bitmapType, &bitmaps[++index], rt_String_xpm);
   
   theGuiManager->LoadIcon("rt_CoordinateSystem", bitmapType, &bitmaps[++index], rt_CoordinateSystem_xpm);
   
   theGuiManager->LoadIcon("rt_EclipseLocator", bitmapType, &bitmaps[++index], rt_EclipseLocator_xpm); 
   theGuiManager->LoadIcon("rt_ContactLocator", bitmapType, &bitmaps[++index], rt_ContactLocator_xpm); 

   theGuiManager->LoadIcon("rt_GmatFunction", bitmapType, &bitmaps[++index], rt_GmatFunction_xpm);
   theGuiManager->LoadIcon("rt_MatlabFunction", bitmapType, &bitmaps[++index], rt_MatlabFunction_xpm);
   theGuiManager->LoadIcon("rt_FileInterface", bitmapType, &bitmaps[++index], rt_FileInterface_xpm);
   
   theGuiManager->LoadIcon("rt_MeasurementModel", bitmapType, &bitmaps[++index], rt_MeasurementModel_xpm);
   theGuiManager->LoadIcon("network", bitmapType, &bitmaps[++index], network_xpm);
   
   theGuiManager->LoadIcon("rt_Default", bitmapType, &bitmaps[++index], rt_Default_xpm);
   
   
   // Let's always rescale all icons since size of icon look different on different platforms
   for ( size_t i = 0; i < WXSIZEOF(bitmaps); i++ )
      images->Add(bitmaps[i]->ConvertToImage().Rescale(sizeW, sizeH));
   
   AssignImageList(images);
   
   #ifdef DEBUG_ADD_ICONS
   MessageInterface::ShowMessage
      ("ResourceTree::AddIcons() exiting, %d icons added\n", index + 1);
   #endif
}

//---------------------------------
// Add items to tree
//---------------------------------

//------------------------------------------------------------------------------
// void OnAddBody(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a body to solar system folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddBody(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name, centralBody, prompt;
   
   // Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "Celestial Body") == 1)
   {
      if (!name.IsEmpty())
      {
         prompt = "Enter Central Body for " + name;
         if (GetNameFromUser(centralBody, centralBody, prompt, "Celestial Body") == 1)
         {
            if (!centralBody.IsEmpty())
            {
               const std::string newName = name.WX_TO_STD_STRING;
               const std::string newCentralBody = centralBody.WX_TO_STD_STRING;
               GmatBase *obj = CreateObject("Moon", newName); // ************
               
               if (obj != NULL)
               {
                  ((CelestialBody*) obj)->SetCentralBody(newCentralBody);
                  // need to get teh sub-item for the appropriate central body here ...
                  AppendItem(item, name, GmatTree::RESOURCE_ICON_MOON, -1,
                             new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY));
                  Expand(item);
                  SelectItem(GetLastChild(item));
                  theGuiManager->UpdateCelestialPoint();
               }
            }
         }
      }
   }
}


//------------------------------------------------------------------------------
// void OnAddGroundStation(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a ground station to ground station folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddGroundStation(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("GroundStation", 1);
   GmatBase *obj = CreateObject("GroundStation", newName);
   
   if (obj != NULL)
   {
      wxString name = newName.c_str();
      GmatTree::ItemType itemType;
      GmatTree::ResourceIconType itemIcon;
      
      GetItemTypeAndIcon(obj, itemType, itemIcon);
      AppendItem(item, name, itemIcon, -1, new GmatTreeItemData(name, itemType));
      
      Expand(item);
      SelectItem(GetLastChild(item));            
      theGuiManager->UpdateGroundStation();
   }
}


//------------------------------------------------------------------------------
// void OnAddSpacecraft(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a spacecraft to spacecraft folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddSpacecraft(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("Spacecraft", 1);
   GmatBase *obj = CreateObject("Spacecraft", newName, true);
   
   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      
      theGuiManager->UpdateSpacecraft();
      // Update CoordindateSystem folder since Moderator creates default
      // coordinate systems if not created already when creating a spacecraft
      theGuiManager->UpdateCoordSystem();
      DeleteChildren(mCoordSysItem);
      AddDefaultCoordSys(mCoordSysItem);
   }
}


//------------------------------------------------------------------------------
// void OnAddChemicalFuelTank(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a fuel tank to hardware folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddChemicalFuelTank(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("ChemicalTank", 1);
   GmatBase *obj = CreateObject("ChemicalTank", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);

      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateFuelTank();
   }
}

//------------------------------------------------------------------------------
// void OnAddElectricFuelTank(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a fuel tank to hardware folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddElectricFuelTank(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("ElectricTank", 1);
   GmatBase *obj = CreateObject("ElectricTank", newName);
   
   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));            
      theGuiManager->UpdateFuelTank();
   }
}


//------------------------------------------------------------------------------
// void OnAddChemicalThruster(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a thruster to hardware folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddChemicalThruster(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("ChemicalThruster", 1);
   GmatBase *obj = CreateObject("ChemicalThruster", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);

      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateThruster();
   }
}

//------------------------------------------------------------------------------
// void OnAddElectricThruster(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a thruster to hardware folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddElectricThruster(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("ElectricThruster", 1);
   GmatBase *obj = CreateObject("ElectricThruster", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));            
      theGuiManager->UpdateThruster();
   }
}

//------------------------------------------------------------------------------
// void OnAddNuclearPowerSystem(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a NuclearPowerSystem to hardware folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddNuclearPowerSystem(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("NuclearPowerSystem", 1);
   GmatBase *obj = CreateObject("NuclearPowerSystem", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));            
      theGuiManager->UpdatePowerSystem();
   }
}

//------------------------------------------------------------------------------
// void OnAddSolarPowerSystem(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a SolarPowerSystem to hardware folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddSolarPowerSystem(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("SolarPowerSystem", 1);
   GmatBase *obj = CreateObject("SolarPowerSystem", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));            
      theGuiManager->UpdatePowerSystem();
   }
}

//------------------------------------------------------------------------------
// void OnAddFormation(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a formation to formation folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddFormation(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("Formation", 1);
   GmatBase *obj = CreateObject("Formation", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateFormation();
   }
}


//------------------------------------------------------------------------------
// void OnAddConstellation(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a constellation to constellation folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddConstellation(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxTreeItemId constellation =
      AppendItem(item, wxT("New Constellation"), -1, -1,
                 new GmatTreeItemData(wxT("New Constellation"),
                                      GmatTree::CONSTELLATION_FOLDER));

   SetItemImage(constellation, GmatTree::RESOURCE_ICON_OPEN_FOLDER, wxTreeItemIcon_Expanded);

   AppendItem(constellation, wxT("GPS1"), GmatTree::RESOURCE_ICON_SPACECRAFT, -1,
              new GmatTreeItemData(wxT("GPS1"), GmatTree::CONSTELLATION_SATELLITE));
   AppendItem(constellation, wxT("GPS2"), GmatTree::RESOURCE_ICON_SPACECRAFT, -1,
              new GmatTreeItemData(wxT("GPS2"), GmatTree::CONSTELLATION_SATELLITE));

   Expand(item);
}


//------------------------------------------------------------------------------
// void OnAddPropagator(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a propagator to propagator folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddPropagator(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("Propagator", 1);
   GmatBase *obj = theGuiInterpreter->CreateDefaultPropSetup(newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdatePropagator();
   }
   else
   {
      MessageInterface::ShowMessage
         ("ResourceTree::OnAddPropagator() propSetup is NULL\n");
   }
}


////------------------------------------------------------------------------------
//// void OnAddSPKPropagator(wxCommandEvent &event)
////------------------------------------------------------------------------------
///**
// * Add a SPK propagator to propagator folder
// *
// * @param <event> command event
// */
////------------------------------------------------------------------------------
//void ResourceTree::OnAddSPKPropagator(wxCommandEvent &event)
//{
//   wxTreeItemId item = GetSelection();
//   std::string newName = theGuiInterpreter->GetNewName("SPKPropagator", 1);
//   GmatBase *obj = theGuiInterpreter->CreateDefaultPropSetup(newName);
//
//   if (obj != NULL)
//   {
//      wxString name = newName.c_str();
//      AppendItem(item, name, GetItemTypeAndIcon(obj), -1,
//                 new GmatTreeItemData(name, GmatTree::SPK_PROPAGATOR));
//      Expand(item);
//
//      theGuiManager->UpdatePropagator();
//   }
//   else
//   {
//      MessageInterface::ShowMessage
//         ("ResourceTree::OnAddSPKPropagator() propSetup is NULL\n");
//   }
//}


//------------------------------------------------------------------------------
// void OnAddImpulsiveBurn(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an impulsive burn to burn folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddImpulsiveBurn(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("ImpulsiveBurn", 1);
   GmatBase *obj = CreateObject("ImpulsiveBurn", newName);
   
   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateBurn();
   }
}


//------------------------------------------------------------------------------
// void OnAddFiniteBurn(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an finite burn to burn folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddFiniteBurn(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("FiniteBurn", 1);
   GmatBase *obj = CreateObject("FiniteBurn", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateBurn();
   }
}


//------------------------------------------------------------------------------
// void OnAddDifferentialCorrector(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a differential corrector to solvers folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddDifferentialCorrector(wxCommandEvent &event)
{
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("Solver event ID = %d\n", event.GetId());
   #endif

   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("DC", 1);
   GmatBase *obj = CreateObject("DifferentialCorrector", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateSolver();
   }
}


//------------------------------------------------------------------------------
// void OnAddSqp(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a sqp to solvers folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddSqp(wxCommandEvent &event)
{
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("Solver event ID = %d\n", event.GetId());
   #endif
   
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("SQP", 1);
   GmatBase *obj = CreateObject("FminconOptimizer", newName);
   
   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateSolver();
   }
}


//------------------------------------------------------------------------------
// void OnAddHardware(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a generic hardware to hardware folder
 *
 * The code used here should be generalizable for other plugin elements as well.
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddHardware(wxCommandEvent &event)
{
   bool isPowerSystem = false;
   GmatTree::ItemType itsType = GmatTree::HARDWARE;

   // Look up the plugin type based on the ID built with menu that selected it
   // NOTE: treating PowerSystems as plugins here as well
   std::string selected = pluginMap[event.GetId()];
   if (selected.find("PowerSystem") != std::string::npos)
   {
      isPowerSystem = true;
      itsType       = GmatTree::POWER_SYSTEM;
   }

   // The rest is like the other tree additions
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName(selected, 1);
   
   GmatBase *obj = CreateObject(selected, newName);
   
   if (obj != NULL)
   {
      GmatTree::ItemType dummyType;
      GmatTree::ResourceIconType iconToUse;
      GetItemTypeAndIcon(obj, dummyType, iconToUse);
      
      wxString name = newName.c_str();
      AppendItem(item, name, iconToUse, -1,
                 new GmatTreeItemData(name, itsType));
      Expand(item);
      SelectItem(GetLastChild(item));

      if (isPowerSystem)
         theGuiManager->UpdatePowerSystem();
      else
         theGuiManager->UpdateImager();
   }
}


//------------------------------------------------------------------------------
// void OnAddSolver(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a generic solver to solvers folder
 *
 * The code used here should be generalizable for other plugin elements as well.
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddSolver(wxCommandEvent &event)
{
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("ResourceTree::OnAddSolver() entered\n");
   #endif
   
   // Look up the plugin type based on the ID built with menu that selected it
   std::string selected = pluginMap[event.GetId()];

   // The rest is like the other tree additions
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName(selected, 1);

   GmatBase *obj = CreateObject(selected, newName);

   if (obj != NULL)
   {
      GmatTree::ItemType dummyType;
      GmatTree::ResourceIconType iconToUse;
      GetItemTypeAndIcon(obj, dummyType, iconToUse);
      
      wxString name = newName.c_str();
      AppendItem(item, name, iconToUse, -1,
                 new GmatTreeItemData(name, GmatTree::SOLVER));
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateSolver();
   }
   
   #ifdef DEBUG_ADD_SOLVER
   MessageInterface::ShowMessage("ResourceTree::OnAddSolver() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void ResourceTree::OnAddLocator(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an event locator to the event locators folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddLocator(wxCommandEvent &event)
{
   #ifdef DEBUG_ADD_LOCATOR
   MessageInterface::ShowMessage("ResourceTree::OnAddLocator() entered\n");
   #endif

   // Look up the plugin type based on the ID built with menu that selected it
   std::string selected = pluginMap[event.GetId()];

   // The rest is like the other tree additions
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName(selected, 1);
   
   #ifdef DEBUG_ADD_LOCATOR
      MessageInterface::ShowMessage("Creating a Locator of type %s named %s\n",
            selected.c_str(), newName.c_str());
   #endif

   // Call with createDefault set to true so we get the default spacecraft
   GmatBase *obj = CreateObject(selected, newName, true);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateLocator();
   }
#ifdef DEBUG_ADD_LOCATOR
   else
   {
      MessageInterface::ShowMessage("ResourceTree::OnAddLocator() Locator is NULL!!!!\n");
   }
#endif

   #ifdef DEBUG_ADD_LOCATOR
   MessageInterface::ShowMessage("ResourceTree::OnAddLocator() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnAddReportFile(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a report file to reports folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddReportFile(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("ReportFile", 1);
   GmatBase *obj = theGuiInterpreter->CreateSubscriber("ReportFile", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateSubscriber();
      // Repopulate Spacecraft node as default ReportFile creates default Spacecraft
      UpdateSpacecraft();
   }
}


//------------------------------------------------------------------------------
// void OnAddXyPlot(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an xy plot to output folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddXyPlot(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("XYPlot", 1);
   GmatBase *obj = theGuiInterpreter->CreateSubscriber("XYPlot", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateSubscriber();
      // Repopulate Spacecraft node as default XYPlot creates default Spacecraft
      UpdateSpacecraft();
   }
}


//------------------------------------------------------------------------------
// void OnAddOrbitView(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an OrbitView plot to output folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddOrbitView(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("OrbitView", 1);
   GmatBase *obj = theGuiInterpreter->CreateSubscriber("OrbitView", newName);
   
   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateSubscriber();
      // Repopulate Spacecraft node as default XYPlot creates default Spacecraft
      UpdateSpacecraft();
   }
}


//------------------------------------------------------------------------------
// void OnAddEphemerisFile(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an EphemerisFile to output folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddEphemerisFile(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("EphemerisFile", 1);
   GmatBase *obj = theGuiInterpreter->CreateSubscriber("EphemerisFile", newName);
   
   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateSubscriber();
      // Repopulate Spacecraft node as default XYPlot creates default Spacecraft
      UpdateSpacecraft();
   }
}


//------------------------------------------------------------------------------
// void OnAddSubscriber(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an generic subscriber to Subscribers folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddSubscriber(wxCommandEvent &event)
{
   std::string selected = pluginMap[event.GetId()];
   
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName(selected, 1);
   GmatBase *obj = theGuiInterpreter->CreateSubscriber(selected, newName);

   //MessageInterface::ShowMessage("Adding subscriber %p, a %s with name %s\n",
   //      obj, obj->GetTypeName().c_str(), newName.c_str());

   if (obj != NULL)
   {
      #ifdef DEBUG_ADD_SUBSCRIBER
         MessageInterface::ShowMessage("   Adding Subscriber %s\n", obj->GetName().c_str());
      #endif

      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateSubscriber();
      // Repopulate Spacecraft node as default ReportFile creates default Spacecraft
      UpdateSpacecraft();
   }
}


//------------------------------------------------------------------------------
// void OnAddVariable(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a variable to variable folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddVariable(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   // show dialog to create user parameter
   ParameterCreateDialog paramDlg(this, ParameterCreateDialog::VARIABLE);
   paramDlg.ShowModal();

   SortChildren(item);
   theGuiManager->SortUserVariables();
}


//------------------------------------------------------------------------------
// void OnAddArray(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add an array to variable folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddArray(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   // show dialog to create user parameter
   ParameterCreateDialog paramDlg(this, ParameterCreateDialog::ARRAY);
   paramDlg.ShowModal();

   SortChildren(item);
   theGuiManager->SortUserVariables();
}


//------------------------------------------------------------------------------
// void OnAddString(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a string to variable folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddString(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   // show dialog to create user parameter
   ParameterCreateDialog paramDlg(this, ParameterCreateDialog::STRING);
   paramDlg.ShowModal();

   SortChildren(item);
   theGuiManager->SortUserVariables();
}


//------------------------------------------------------------------------------
// void OnAddMatlabFunction(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a matlab function to the folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddMatlabFunction(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name;
   
   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "MATLAB function") == 1)
   {
      if (!name.IsEmpty())
      {
         const std::string newName = name.WX_TO_STD_STRING;
         GmatBase *obj = CreateObject("MatlabFunction", newName);
         if (obj != NULL)
         {
            AddObjectToTree(obj);
            
            Expand(item);
            SelectItem(GetLastChild(item));
            theGuiManager->UpdateFunction();
         }
      }
   }
}


//------------------------------------------------------------------------------
// void OnAddGmatFunction(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a gmat function to the folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddGmatFunction(wxCommandEvent &event)
{
   #ifdef DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage("ResourceTree::OnAddGmatFunction() entered\n");
   #endif

   wxTreeItemId item = GetSelection();
   wxString name;
   GmatBase *obj = NULL;
   
   //-----------------------------------------------------------------
   // if creating a new GmatFunction
   //-----------------------------------------------------------------
   if (event.GetId() == POPUP_NEW_GMAT_FUNCTION)
   {
      if (GetNameFromUser(name, name, "Enter name: ", "New GMAT function") == 1)
      {
         if (!name.IsEmpty())
         {
            obj = CreateObject("GmatFunction", name.c_str());
            if (obj)
               ((Function *)(obj))->SetNewFunction(true);
         }
      }
   }
   //-----------------------------------------------------------------
   // if opening an existing GmatFunction
   //-----------------------------------------------------------------
   else if (event.GetId() == POPUP_OPEN_GMAT_FUNCTION)
   {
      wxFileDialog dialog
         (this, _T("Choose a file"), _T(""), _T(""),
          "GMAT Function file (*.gmf|*.gmf|"
          "Text files (*.txt, *.text)|*.txt;*.text|"
          "All files (*.*)|*.*");
      
      if (dialog.ShowModal() == wxID_OK)
      {
         wxString path = dialog.GetPath().c_str();
         wxString filename = dialog.GetFilename().c_str();
         name = filename.Mid(0, (filename.find('.')));

         #ifdef DEBUG_GMAT_FUNCTION
         MessageInterface::ShowMessage("   path='%s'\n", path.c_str());
         wxString directory = dialog.GetDirectory().c_str();
         MessageInterface::ShowMessage("   directory='%s'\n", directory.c_str());
         MessageInterface::ShowMessage("   filename='%s'\n", filename.c_str());
         MessageInterface::ShowMessage("   name='%s'\n", name.c_str());
         #endif

         // Create GmatFunction object
         obj = CreateObject("GmatFunction", name.c_str());
         // Set function path
         if (obj != NULL)
            obj->SetStringParameter("FunctionPath", path.c_str());
      }
   }

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      theGuiManager->UpdateFunction();
      SelectItem(GetLastChild(item));
   }
   
   #ifdef DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage("ResourceTree::OnAddGmatFunction() exiting\n");
   #endif

}


//------------------------------------------------------------------------------
// void OnAddCoordSystem(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a coordinate system to the folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddCoordSystem(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   // show dialog to create user parameter
   CoordSysCreateDialog coordDlg(this);
   coordDlg.ShowModal();
   
   if (coordDlg.IsCoordCreated())
   {
      wxString name = coordDlg.GetCoordName();
      
      AppendItem(item, name, GmatTree::RESOURCE_ICON_COORDINATE_SYSTEM, -1,
                 new GmatTreeItemData(name, GmatTree::USER_COORDINATE_SYSTEM));
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateCoordSystem();
      PluginItemManager::Instance()->UpdateObjectList(Gmat::COORDINATE_SYSTEM);
   }
}


//------------------------------------------------------------------------------
// void OnAddBaryCenter(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a barycenter to special points folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddBarycenter(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("Barycenter", 1);
   GmatBase *obj = CreateObject("Barycenter", newName);

   if (obj != NULL)
   {
      AddObjectToTree(obj);
      
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateCelestialPoint();
   }
}


//------------------------------------------------------------------------------
// void OnAddLibration(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a libration point to special points folder
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddLibration(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   std::string newName = theGuiInterpreter->GetNewName("Libration", 1);
   GmatBase *obj = CreateObject("LibrationPoint", newName);

   if (obj != NULL)
   {
      DeleteChildren(mSpecialPointsItem);
      AddDefaultSpecialPoints(mSpecialPointsItem, true, false);
      Expand(item);
      SelectItem(GetLastChild(item));
      theGuiManager->UpdateCelestialPoint();
   }
}


//------------------------------------------------------------------------------
// void OnAddPlanet(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddPlanet(wxCommandEvent &event)
{
   // get the central body name
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   wxString cBodyName = selItemData->GetName();
   std::string cBody  = cBodyName.WX_TO_STD_STRING;
   wxString name;
   
   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "New Planet") == 1)
   {
      if (!name.IsEmpty())
      {
         const std::string newName = name.WX_TO_STD_STRING;
         SolarSystem   *ss         = theGuiInterpreter->GetSolarSystemInUse();
         if (ss->IsBodyInUse(newName))
         {
            std::string errmsg = "Error creating new planet: celestial body \"";
            errmsg += newName + "\" already exists.";
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.c_str());
            return;
         }
         GmatBase *obj = CreateObject("Planet", newName);
         if (obj)
         {
            ((CelestialBody*)obj)->SetCentralBody(cBody);
            // For now, we only have one solar system with one star ...
            if (cBody == GmatSolarSystemDefaults::SUN_NAME)
            {
               wxTreeItemId parent = GetItemParent(item);
               AppendItem(parent, name, GmatTree::RESOURCE_ICON_PLANET_GENERIC, -1,
                          new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_PLANET));
               Expand(parent);
               SelectItem(GetLastChild(parent));
            }
            else
            {
               AppendItem(item, name, GmatTree::RESOURCE_ICON_PLANET_GENERIC, -1,
                          new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_PLANET));
               Expand(item);
               SelectItem(GetLastChild(item));
            }
            
            theGuiManager->UpdateSolarSystem();
         }
      }
   }
}


//------------------------------------------------------------------------------
// void OnAddMoon(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddMoon(wxCommandEvent &event)
{
   // get the central body name
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   wxString cBodyName = selItemData->GetName();
   std::string cBody  = cBodyName.WX_TO_STD_STRING;
   wxString name;

   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "New Moon") == 1)
   {
      if (!name.IsEmpty())
      {
         const std::string newName = name.WX_TO_STD_STRING;
         SolarSystem   *ss         = theGuiInterpreter->GetSolarSystemInUse();
         if (ss->IsBodyInUse(newName))
         {
            std::string errmsg = "Error creating new Moon: celestial body \"";
            errmsg += newName + "\" already exists.";
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.c_str());
            return;
         }
         GmatBase *obj = CreateObject("Moon", newName);
         if (obj)
         {
            ((CelestialBody*)obj)->SetCentralBody(cBody);
            // For now, we only have one solar system with one star ...
            // Of course, a moon around a star makes no sense ...
            if (cBody == GmatSolarSystemDefaults::SUN_NAME)
            {
               wxTreeItemId parent = GetItemParent(item);
               AppendItem(parent, name, GmatTree::RESOURCE_ICON_MOON_GENERIC, -1,
                          new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_MOON));
               Expand(parent);
               SelectItem(GetLastChild(parent));
            }
            else
            {
               AppendItem(item, name, GmatTree::RESOURCE_ICON_MOON_GENERIC, -1,
                          new GmatTreeItemData(name, GmatTree::CELESTIAL_BODY_MOON));
               Expand(item);
               SelectItem(GetLastChild(item));
            }
            
            theGuiManager->UpdateSolarSystem();
         }
      }
   }
}


//------------------------------------------------------------------------------
// void OnAddComet(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddComet(wxCommandEvent &event)
{
   // get the central body name
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   wxString cBodyName = selItemData->GetName();
   std::string cBody  = cBodyName.WX_TO_STD_STRING;
   wxString name;
   
   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "New Comet") == 1)
   {
      if (!name.IsEmpty())
      {
         const std::string newName = name.WX_TO_STD_STRING;
         SolarSystem   *ss         = theGuiInterpreter->GetSolarSystemInUse();
         if (ss->IsBodyInUse(newName))
         {
            std::string errmsg = "Error creating new comet: celestial body \"";
            errmsg += newName + "\" already exists.";
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.c_str());
            return;
         }
         GmatBase *obj = CreateObject("Comet", newName);
         #ifdef DEBUG_ADD_COMET
         if (obj == NULL)
            MessageInterface::ShowMessage("ERROR creating body!!\n");
         else
            MessageInterface::ShowMessage("Body %s created!!\n", name.c_str());
         #endif
         if (obj)
         {
            ((CelestialBody*)obj)->SetCentralBody(cBody);
            // For now, we only have one solar system with one star ...
            wxTreeItemId parent = item;
            if (cBody == GmatSolarSystemDefaults::SUN_NAME)
               parent = GetItemParent(item);
            
            GmatTree::ItemType itemType;
            GmatTree::ResourceIconType itemIcon;
            GetItemTypeAndIcon(obj, itemType, itemIcon);
            
            AppendItem(parent, name, itemIcon, -1,
                       new GmatTreeItemData(name, itemType));
            Expand(parent);
            SelectItem(GetLastChild(parent));
            theGuiManager->UpdateSolarSystem();
         }
      }
   }
   
}


//------------------------------------------------------------------------------
// void OnAddAsteroid(wxCommandEvent &event)
//------------------------------------------------------------------------------
void ResourceTree::OnAddAsteroid(wxCommandEvent &event)
{
   // get the central body name
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   wxString cBodyName = selItemData->GetName();
   std::string cBody  = cBodyName.WX_TO_STD_STRING;
   wxString name;

   //Get name from the user first
   if (GetNameFromUser(name, name, "Enter name: ", "New Asteroid") == 1)
   {
      if (!name.IsEmpty())
      {
         const std::string newName = name.WX_TO_STD_STRING;
         SolarSystem   *ss         = theGuiInterpreter->GetSolarSystemInUse();
         if (ss->IsBodyInUse(newName))
         {
            std::string errmsg = "Error creating new asteroid: celestial body \"";
            errmsg += newName + "\" already exists.";
            MessageInterface::PopupMessage(Gmat::ERROR_, errmsg.c_str());
            return;
         }
         GmatBase *obj = CreateObject("Asteroid", newName);
         if (obj)
         {
           ((CelestialBody*)obj)->SetCentralBody(cBody);
            // For now, we only have one solar system with one star ...
            wxTreeItemId parent = item;
            if (cBody == GmatSolarSystemDefaults::SUN_NAME)
               parent = GetItemParent(item);
            
            GmatTree::ItemType itemType;
            GmatTree::ResourceIconType itemIcon;
            GetItemTypeAndIcon(obj, itemType, itemIcon);
            
            AppendItem(parent, name, itemIcon, -1,
                       new GmatTreeItemData(name, itemType));
            Expand(parent);
            SelectItem(GetLastChild(parent));
            
            theGuiManager->UpdateSolarSystem();
         }
      }
   }
}


//------------------------------------------------------------------------------
// void OnAddScript()
//------------------------------------------------------------------------------
/**
 * Add a script to the folder and opens it.
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddScript(wxCommandEvent &event)
{
   //---------------- debug
   //OnAddScriptFolder(event);
   //wxString cwd = wxGetCwd();
   //
   ////loj: 3/14/06 Why I need this to get the same precision as script folder?
   //wxDirDialog dirDialog(this, "Select a script directory", wxGetCwd());
   //
   //if (dirDialog.ShowModal() == wxID_OK)
   //{
   //}
   //---------------- debug


   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""),
                       "Script files (*.script, *.m)|*.script;*.m|"
                       "Text files (*.txt, *.text)|*.txt;*.text|"
                       "All files (*.*)|*.*");
   
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString path = dialog.GetPath().c_str();
      AddScript(path);
   }
   else
   {
      mScriptAdded = false;
   }
}


//------------------------------------------------------------------------------
// void OnAddUserObject(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a generic solver to solvers folder
 *
 * The code used here should be generalizable for other plugin elements as well.
 *
 * @param event command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddUserObject(wxCommandEvent &event)
{
   #ifdef DEBUG_USER_OBJECT
   MessageInterface::ShowMessage("OnAddUserObject() entered\n");
   #endif
   
   // Look up the plugin type based on the ID built with menu that selected it
   std::string selected = pluginMap[event.GetId()];
   
   // The rest is like the other tree additions
   wxTreeItemId item = GetSelection();
   GmatTreeItemData *selItemData = (GmatTreeItemData *) GetItemData(item);
   GmatTree::ItemType itemType = selItemData->GetItemType();
   std::string newName = theGuiInterpreter->GetNewName(selected, 1);
   
   GmatBase *obj = CreateObject(selected, newName);
   
   if (obj != NULL)
   {
      GmatTree::ItemType dummyType;
      GmatTree::ResourceIconType iconToUse;
      GetItemTypeAndIcon(obj, dummyType, iconToUse);
      
      wxString name = newName.c_str();
      if (obj->IsOfType("Interface"))
         AppendItem(item, name, iconToUse, -1,
                 new GmatTreeItemData(name, GmatTree::INTERFACE_OPENABLE));
      else
         AppendItem(item, name, iconToUse, -1,
                 new GmatTreeItemData(name, GmatTree::USER_DEFINED_OBJECT));
      
      Expand(item);
      SelectItem(GetLastChild(item));
      UpdateGuiItem(itemType);
   }
   
   #ifdef DEBUG_USER_OBJECT
   MessageInterface::ShowMessage("OnAddUserObject() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void UpdateMatlabServerItem(bool start)
//------------------------------------------------------------------------------
void ResourceTree::UpdateMatlabServerItem(bool started)
{
   mMatlabServerStarted = started;
}


//------------------------------------------------------------------------------
// bool WasScriptAdded()
//------------------------------------------------------------------------------
bool ResourceTree::WasScriptAdded()
{
   return mScriptAdded;
}


//------------------------------------------------------------------------------
// wxString GetLastScriptAdded()
//------------------------------------------------------------------------------
wxString ResourceTree::GetLastScriptAdded()
{
   return mLastScriptAdded;
}


//------------------------------------------------------------------------------
// bool IsScriptFolderRunning()
//------------------------------------------------------------------------------
bool ResourceTree::IsScriptFolderRunning()
{
   return mScriptFolderRunning;
}


//------------------------------------------------------------------------------
// void PanelObjectChanged( GmatBase *obj )
// Called when user clicks Ok/Apply and successful
//------------------------------------------------------------------------------
void ResourceTree::PanelObjectChanged( GmatBase *obj )
{
}


//------------------------------------------------------------------------------
// void QuitRunningScriptFolder()
//------------------------------------------------------------------------------
/**
 * Sets script folder run interrupted flag to true.
 * Usually this function is called from the GmatMainFrame when user tries
 * close the GMAT while script is still running.
 */
//------------------------------------------------------------------------------
void ResourceTree::QuitRunningScriptFolder()
{
   mScriptFolderRunInterrupted = true;

   // This yield seems misplaced -- seems like if needed it should be in
   // GmatMainFrame::OnClose near where this method gets called.
   wxYield();
}


//------------------------------------------------------------------------------
// void OnRemoveAllScripts(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a script to the folder and open it
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnRemoveAllScripts(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();

   Collapse(item);

   while (GetChildrenCount(item) > 0)
   {
      wxTreeItemId lastChild = GetLastChild(item);
      wxString name = ((GmatTreeItemData *)GetItemData(lastChild))->GetName();
      GmatTree::ItemType itemType =
         ((GmatTreeItemData *)GetItemData(lastChild))->GetItemType();

      // close window and delte item
      theMainFrame->RemoveChild(name, itemType);
      Delete(lastChild);
   }

   // arg: should script counter go back to 1?

}

//------------------------------------------------------------------------------
// void OnRemoveScript(wxCommandEvent &event)
//------------------------------------------------------------------------------
/**
 * Add a script to the folder and open it
 *
 * @param <event> command event
 */
//------------------------------------------------------------------------------
void ResourceTree::OnRemoveScript(wxCommandEvent &event)
{
   wxTreeItemId item = GetSelection();
   wxString name = ((GmatTreeItemData *)GetItemData(item))->GetName();
   GmatTree::ItemType itemType = ((GmatTreeItemData *)GetItemData(item))->GetItemType();
   wxTreeItemId parentItem = GetItemParent(item);

   // close window and delete item
   theMainFrame->RemoveChild(name, itemType);
   Delete(item);
}


//------------------------------------------------------------------------------
// bool OnScriptRecover(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnScriptRecover(wxCommandEvent& event)
{
   #ifdef DEBUG_BUILD_OBJECT
   MessageInterface::ShowMessage
      ("OnScriptRecover() entered and calling BuildScript()\n");
   #endif
   
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());

   if (theMainFrame->IsChildOpen(item))
	   theMainFrame->CloseChild(theMainFrame->GetChild(item->GetName()));

   wxString filename = item->GetName();

   BuildScript(filename, GmatGui::ALWAYS_OPEN_SCRIPT);
}


//------------------------------------------------------------------------------
// bool OnScriptBuildObject(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnScriptBuildObject(wxCommandEvent& event)
{
   #ifdef DEBUG_BUILD_OBJECT
   MessageInterface::ShowMessage
      ("OnScriptBuildObject() entered and calling BuildScript()\n");
   #endif
   
   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());

   if (theMainFrame->IsChildOpen(item))
   {
	   #ifdef __USE_STC_EDITOR__
	   EditorPanel *panel = (EditorPanel *) theMainFrame->GetChild(item->GetName())->GetAssociatedWindow();
	   panel->ClickButton(false);
	   #else
	   ScriptPanel *panel = (ScriptPanel *) theMainFrame->GetChild(item->GetName())->GetAssociatedWindow();
	   panel->ClickButton(false);
	   #endif
   }
   else
   {
	   wxString filename = item->GetName();
	   BuildScript(filename, GmatGui::ALWAYS_OPEN_SCRIPT);
   }
}


//------------------------------------------------------------------------------
// bool OnScriptBuildAndRun(wxCommandEvent& event)
//------------------------------------------------------------------------------
void ResourceTree::OnScriptBuildAndRun(wxCommandEvent& event)
{
   #ifdef DEBUG_RESOURCE_TREE
   MessageInterface::ShowMessage("ResourceTree::OnScriptBuildAndRun()\n");
   #endif

   // Get info from selected item
   GmatTreeItemData *item = (GmatTreeItemData *) GetItemData(GetSelection());

   if (!theMainFrame->IsChildOpen(item))
   {
	   wxString filename = item->GetName();
	   BuildScript(filename, GmatGui::ALWAYS_OPEN_SCRIPT);
   }
   #ifdef __USE_STC_EDITOR__
   EditorPanel *panel = (EditorPanel *) theMainFrame->GetChild(item->GetName())->GetAssociatedWindow();
   panel->ClickButton(true);
   #else
   ScriptPanel *panel = (ScriptPanel *) theMainFrame->GetChild(item->GetName())->GetAssociatedWindow();
   panel->ClickButton(true);
   #endif
}


//------------------------------------------------------------------------------
// void OnAddScriptFolder()
//------------------------------------------------------------------------------
/**
 * Add a script folder to the folder and open it
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnAddScriptFolder(wxCommandEvent &event)
{
   wxDirDialog dialog(this, "Select a script directory", wxGetCwd());

   if (dialog.ShowModal() == wxID_OK)
   {
      wxString dirname = dialog.GetPath();

      #ifdef DEBUG_RESOURCE_TREE
      MessageInterface::ShowMessage("OnAddScriptFolder() dirname=%s\n",
                                    dirname.c_str());
      #endif

      // add item to tree
      wxTreeItemId newItem =
         AppendItem(mScriptItem, dirname, GmatTree::RESOURCE_ICON_FOLDER, -1,
                    new GmatTreeItemData(dirname, GmatTree::ADDED_SCRIPT_FOLDER));

      Expand(mScriptItem);

      // add files under script diretory
      wxDir dir(dirname);
      wxString filename;
      wxString filepath;

      //How do I specify multiple file ext?
      //bool cont = dir.GetFirst(&filename, "*.script, *.m");
      bool cont = dir.GetFirst(&filename);
      while (cont)
      {
         if (filename.Contains(".script") || filename.Contains(".m"))
         {
            filepath = dirname + "/" + filename;

            // remove any backup files
            if (filename.Last() == 't' || filename.Last() == 'm')
            {
               // read first item to elliminate Matlab/Gmat function
               std::ifstream ifs(filepath.WX_TO_C_STRING);
               std::string item;
               ifs >> item;
               
               if (item != "function")
               {
                  GmatTreeItemData *newItemData =
                     new GmatTreeItemData(filepath, GmatTree::SCRIPT_FILE);
                  AppendItem(newItem, filename, GmatTree::RESOURCE_ICON_SCRIPT, -1,
                             newItemData);
               }
               
               ifs.close();
            }
         }
         
         cont = dir.GetNext(&filename);
      }
      
      // Make it bold face if script is the same as currently active script file
      if (mLastActiveScript != "")
         MakeScriptActive(mLastActiveScript, true);
   }
}


//------------------------------------------------------------------------------
// void OnRunScriptsFromFolder()
//------------------------------------------------------------------------------
/**
 * Build and run all script files in the folder.
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnRunScriptsFromFolder(wxCommandEvent &event)
{
   #ifdef DEBUG_RUN_SCRIPT_FOLDER
   MessageInterface::ShowMessage
      ("ResourceTree::OnRunScriptsFromFolder() entered\n");
   #endif
   
   if (theMainFrame == NULL)
   {
      MessageInterface::ShowMessage
         ("===> OnRunScriptsFromFolder() theMainFrame is not set\n");
      return;
   }

   wxTreeItemId itemId = GetSelection();
   wxString filename;
   wxTreeItemIdValue cookie;
   wxTreeItemId scriptId = GetFirstChild(itemId, cookie);
   int numScripts = 0;

   // find only script file, exclude script folder
   while (scriptId.IsOk())
   {
      #ifdef DEBUG_RUN_SCRIPT_FOLDER
      MessageInterface::ShowMessage
         ("ResourceTree::OnRunScriptsFromFolder() scriptText=<%s>\n",
          GetItemText(scriptId).c_str());
      #endif

      if (GetItemImage(scriptId) != GmatTree::RESOURCE_ICON_FOLDER)
         numScripts++;

      scriptId = GetNextChild(itemId, cookie);
   }

   int startNum = 1;
   int runCount = numScripts;
   int repeatCount = 1;
   
   Real absTol = GmatFileUtil::COMPARE_TOLERANCE;
   wxString compareDir1  = ((GmatTreeItemData*)GetItemData(itemId))->GetName();

   RunScriptFolderDialog dlg(this, numScripts, absTol, compareDir1);
   dlg.ShowModal();

   if (!dlg.RunScripts())
      return;
   
   startNum = dlg.GetStartingScriptNumber();
   runCount = dlg.GetNumScriptsToRun() + startNum - 1;
   repeatCount = dlg.GetNumTimesToRun();
   bool runFromSavedScripts = dlg.RunFromSavedScripts();
   bool compare = dlg.CompareResults();
   bool saveCompareResults = dlg.SaveCompareResults();
   bool excludeScripts = false;
   bool excludeScripts2 = false;
   wxString filterString = dlg.GetFilterString(excludeScripts);
   wxString filterString2 = dlg.Get2ndFilterString(excludeScripts2);
   bool builtOk = false;
   
   // for current output path
   FileManager *fm = FileManager::Instance();
   std::string oldOutPath = fm->GetFullPathname(FileManager::OUTPUT_PATH);
   std::string oldLogFile = MessageInterface::GetLogFileName();
   bool hasOutDirChanged = dlg.HasOutDirChanged();
   bool createRunFolder = dlg.CreateRunFolder();
   wxString sep = fm->GetPathSeparator().c_str();

   wxString currOutPath = dlg.GetCurrentOutDir() + sep;
   wxString savePath = dlg.GetSaveScriptsDir() + sep;

   // Create path if not exist
   if (!::wxDirExists(currOutPath))
      ::wxMkdir(currOutPath);

   int count = 0;
   mScriptFolderRunInterrupted = false;
   scriptId = GetFirstChild(itemId, cookie);

   wxTextCtrl *textCtrl = NULL;
   wxString tempStr;

   // create CompareReport Panel
   if (compare)
   {
      GmatMdiChildFrame *textFrame =
         theMainFrame->GetChild("CompareReport");

      if (textFrame == NULL)
      {
         GmatTreeItemData *compareItem =
            new GmatTreeItemData("CompareReport", GmatTree::OUTPUT_COMPARE_REPORT);

         textFrame = theMainFrame->CreateChild(compareItem);
      }

      textCtrl = textFrame->GetScriptTextCtrl();
      textCtrl->SetMaxLength(320000); // make long enough
      textFrame->Show();
      wxString msg;

      // Fix bug GMT-7399
      // msg.Printf(_T("GMAT Build Date: %s %s\n\n"),  __DATE__, __TIME__);
      msg.Printf(_T("GMAT Build Date: %s %s\n\n"),
         GmatGlobal::Instance()->GetGMATBuildDate(),
         GmatGlobal::Instance()->GetGMATBuildTime());

      textCtrl->AppendText(msg);

      //loj: Why Do I need to do this to show whole TextCtrl?
      // textFrame->Layout() didn't work.
      int w, h;
      textFrame->GetSize(&w, &h);
      textFrame->SetSize(w+1, h+1);
   }

   // Set batch mode to true, so that PopupMessage will be rerouted to ShowMessage
   GmatGlobal::Instance()->SetBatchMode(true);
   bool appendLog = false;
   mBuildErrorCount = 0;
   mFailedScriptsList.Clear();
   mScriptFolderRunning = true;
   wxArrayString initTimeErrors;
   wxArrayString unknownInitTimeErrors;
   wxArrayString runInterrupedByUser;
   wxArrayString runTimeErrors;
   wxArrayString unknownRunTimeErrors;
   
   // Set output path for reports and ephemeris files
   try
   {
      fm->SetAbsPathname(FileManager::OUTPUT_PATH, currOutPath.c_str());
      // Changed to use VEHICLE_EPHEM_PATH (LOJ: 2014.06.18)
      //fm->SetAbsPathname(FileManager::EPHEM_PATH, currOutPath.c_str());
      fm->SetAbsPathname(FileManager::VEHICLE_EPHEM_PATH, currOutPath.c_str());
   }
   catch (UtilityException &ue)
   {
      MessageInterface::ShowMessage(ue.GetFullMessage());
      return;
   }
   
   // Change log path and append log messages
   MessageInterface::SetLogPath(currOutPath.c_str(), appendLog);

   // Write run start time
   wxDateTime now = wxDateTime::Now();
   wxString wxNowStr = now.FormatISODate() + " " + now.FormatISOTime() + " ";
   std::string nowStr = wxNowStr.WX_TO_STD_STRING;

   MessageInterface::LogMessage(nowStr + "GMAT starting batch run.\n");

   clock_t t1 = clock();
   bool isFirstScript = true;

   while (scriptId.IsOk())
   {
      if (GetItemImage(scriptId) == GmatTree::RESOURCE_ICON_FOLDER)
      {
         scriptId = GetNextChild(itemId, cookie);
         continue;
      }
      
      if (mScriptFolderRunInterrupted)
      {
         MessageInterface::ShowMessage("===> Script folder run interrupted\n");
         break;
      }
      
      count++;

      if (count < startNum)
      {
         scriptId = GetNextChild(itemId, cookie);
         continue;
      }

      if (count > runCount)
         break;

      // Set main frame title to script file name
      filename = ((GmatTreeItemData*)GetItemData(scriptId))->GetName();
      
      // Filter scripts
      if (filterString != "")
      {
         if ((excludeScripts && filename.Contains(filterString)) ||
             (!excludeScripts && !filename.Contains(filterString)))
         {
            scriptId = GetNextChild(itemId, cookie);
            continue;
         }
         else
         {
            if (filterString2 != "")
            {
               if ((excludeScripts2 && filename.Contains(filterString2)) ||
                   (!excludeScripts2 && !filename.Contains(filterString2)))
               {
                  scriptId = GetNextChild(itemId, cookie);
                  continue;
               }
            }
         }
      }
      
      wxString titleText;
      titleText.Printf("%s - General Mission Analysis Tool (GMAT)", filename.c_str());
      theMainFrame->SetTitle(titleText);
      
      // Set running script text
      wxString text;
      text.Printf("Running script %d of %d;   %s\n", count, runCount,
                  filename.c_str());

      if (compare)
         textCtrl->AppendText(text);

      for (int i=0; i<repeatCount; i++)
      {
         wxString outPath = currOutPath;

         if (createRunFolder)
         {
            outPath << "Run_" << i+1;
            if (!::wxDirExists(outPath))
               ::wxMkdir(outPath);

            outPath = outPath + "/";

            // Set output path
            fm->SetAbsPathname(FileManager::OUTPUT_PATH, outPath.c_str());

            // Change log path and append log messages
            MessageInterface::SetLogPath(outPath.c_str(), appendLog);

            if (isFirstScript)
            {
               MessageInterface::LogMessage(nowStr + "GMAT starting batch run.\n");
               isFirstScript = false;
            }
         }

         MessageInterface::ShowMessage
            ("\nStarting script %d of %d: %s\n", count, runCount, filename.WX_TO_C_STRING);

         MessageInterface::ShowMessage
            ("==> Run Count: %d\n", i+1);

         // Write repeat and run count to main frame status bar
         wxString text;
         text.Printf("Repeat Count %d of %d;  Running script %d of %d;  File:%s",
                     i+1, repeatCount, count, runCount, filename.c_str());
         theMainFrame->SetStatusText(text, 2);

         if (compare)
         {
            tempStr.Printf("%d", i+1);
            textCtrl->AppendText("==> Run Count: " + tempStr + "\n");
         }

         try
         {
            // Create objects from script only first time, to test re-run
            if (i == 0)
               builtOk = BuildScript(filename, GmatGui::DO_NOT_OPEN_SCRIPT, true,
                                     runFromSavedScripts, savePath, true);
            
            if (builtOk)
            {
               Integer retval = theMainFrame->RunCurrentMission();
               #ifdef DEBUG_RUN_SCRIPT_FOLDER
               MessageInterface::ShowMessage("==> RunCurrentMission retval = %d\n", retval);
               #endif
               // if run failed, save to report
               if (retval == -2)
                  initTimeErrors.Add(filename);
               else if (retval == -3)
                  unknownInitTimeErrors.Add(filename);
               else if (retval == -4)
                  runInterrupedByUser.Add(filename);
               else if (retval == -5)
                  runTimeErrors.Add(filename);
               else if (retval <= -6)
                  unknownRunTimeErrors.Add(filename);
               
               if (compare)
               {
                  absTol = dlg.GetAbsTolerance();
                  CompareScriptRunResult(absTol, dlg.GetReplaceString(), compareDir1,
                                         dlg.GetCompareDirectory(), textCtrl);
               }
            }
         }
         catch(BaseException &e)
         {
            MessageInterface::ShowMessage
               ("*** Error running: %s\n   %s\n", filename.WX_TO_C_STRING,
                e.GetFullMessage().c_str());

            if (compare)
               textCtrl->AppendText(e.GetFullMessage().c_str());
         }
         catch (...)
         {
            MessageInterface::ShowMessage
               ("*** Unknown Error running: %s\n", filename.WX_TO_C_STRING);
         }
      }

      scriptId = GetNextChild(itemId, cookie);
      appendLog = true;
   }

   mScriptFolderRunning = false;

   // Write run end time
   now = wxDateTime::Now();
   wxNowStr = now.FormatISODate() + " " + now.FormatISOTime() + " ";
   nowStr = wxNowStr.c_str();

   MessageInterface::LogMessage(nowStr + "GMAT ended batch run.\n\n");

   clock_t t2 = clock();
   MessageInterface::ShowMessage
      ("===> Grand Total Run Time: %f seconds\n", (Real)(t2-t1)/CLOCKS_PER_SEC);

   // save compare results to a file
   if (compare && saveCompareResults)
      textCtrl->SaveFile(dlg.GetSaveFilename());

   // reset output path
   if (hasOutDirChanged)
   {
      fm->SetAbsPathname(FileManager::OUTPUT_PATH, oldOutPath);
      //MessageInterface::SetLogFile(oldLogFile);
   }
   
   #ifdef DEBUG_RUN_SCRIPT_FOLDER
   MessageInterface::ShowMessage("   Now report script batch run status\n");
   #endif
   
   // Report completion
   wxString text;
   text.Printf("Finished running %d scripts\n", runCount);
   theMainFrame->SetStatusText(text, 2);
   theMainFrame->SetStatusText("", 1);

   // Set batch mode to false
   GmatGlobal::Instance()->SetBatchMode(false);
   
   // Popup errors found message
   wxString msg1, msg2, msg3, msg4, msg5, msg6;
   wxString msg1Str;
   wxString scriptNames;
   int numFailed = 0;
   
   if (mBuildErrorCount > 0)
   {
      scriptNames = "";
      
      // Will this fix allocation error in the MessageInterface
      if (mBuildErrorCount <= 100)
      {
         msg1.Printf("Script errors found in the following %d script(s):\n",
                     mBuildErrorCount);
         for (int i = 0; i < mBuildErrorCount; i++)
            scriptNames = scriptNames + mFailedScriptsList[i] + "\n";
         
         msg1 = msg1 + scriptNames;
      }
      else
      {
         msg1.Printf("%d script error(s) found. Check GmatLog.txt\n", mBuildErrorCount);
         MessageInterface::ShowMessage
            ("Script errors found in the following %d script(s):\n", mBuildErrorCount);
         for (int i = 0; i < mBuildErrorCount; i++)
            MessageInterface::ShowMessage("%s\n", mFailedScriptsList[i].WX_TO_C_STRING);
      }
   }
   
   if (initTimeErrors.GetCount() > 0)
   {
      numFailed = initTimeErrors.GetCount();
      scriptNames = "";
      msg2.Printf("\nThe following %d script(s) failed to initialize:\n",
                  numFailed);
      
      for (int i = 0; i < numFailed; i++)
         scriptNames = scriptNames + initTimeErrors[i] + "\n";
      
      msg2 = msg2 + scriptNames;
   }
   
   if (unknownInitTimeErrors.GetCount() > 0)
   {
      numFailed = unknownInitTimeErrors.GetCount();
      scriptNames = "";
      msg3.Printf("\nThe following %d script(s) encountered unknown initialization error:\n",
                  numFailed);
      
      for (int i = 0; i < numFailed; i++)
         scriptNames = scriptNames + unknownInitTimeErrors[i] + "\n";
      
      msg3 = msg3 + scriptNames;
   }
   
   if (runInterrupedByUser.GetCount() > 0)
   {
      numFailed = runInterrupedByUser.GetCount();
      scriptNames = "";
      msg4.Printf("\nThe following %d script(s) were interrupted by user:\n",
                  numFailed);
      
      // Why following line causes crash?
      for (int i = 0; i < numFailed; i++)
      {
         scriptNames = scriptNames + runInterrupedByUser[i] + "\n";
      }
      
      msg4 = msg4 + scriptNames;
   }
   
   if (runTimeErrors.GetCount() > 0)
   {
      numFailed = runTimeErrors.GetCount();
      scriptNames = "";
      msg5.Printf("\nThe following %d script(s) failed to run to completion:\n",
                  numFailed);
      
      for (int i = 0; i < numFailed; i++)
         scriptNames = scriptNames + runTimeErrors[i] + "\n";
      
      msg5 = msg5 + scriptNames;
   }
   
   if (unknownRunTimeErrors.GetCount() > 0)
   {
      numFailed = unknownRunTimeErrors.GetCount();
      scriptNames = "";
      msg6.Printf("\nThe following %d script(s) encountered unknown run time error:\n",
                  numFailed);
      
      // Why following line causes crash?
      for (int i = 0; i < numFailed; i++)
      {
         scriptNames = scriptNames + unknownRunTimeErrors[i] + "\n";
      }
      
      msg6 = msg6 + scriptNames;
   }
   
   
   if (msg1 != "" || msg2 != "" || msg3 != "" || msg4 != "" || msg5 != "" || msg6 != "")
   {
      wxString msg = msg1 + msg2 + msg3 + msg4 + msg5 + msg6;
      
      // Show errors to message window
      MessageInterface::ShowMessage(msg.WX_TO_STD_STRING);
      
      // Show errors to view text dialog
      ViewTextDialog *dlg =
         new ViewTextDialog(this, _T("Information"), false, wxDefaultPosition, wxSize(550, 300));
      wxTextCtrl *text = dlg->GetTextCtrl();
      text->AppendText(msg);
      dlg->ShowModal();
      
      // Log run folder summary report to a separate file
      std::string summaryFile = currOutPath.WX_TO_STD_STRING;
      summaryFile += "FolderRunSummary.txt";
      MessageInterface::ShowMessage("Writing folder run summary to '%s'\n", summaryFile.c_str());
      MessageInterface::SetLogFile(summaryFile);
      MessageInterface::LogMessage(msg.WX_TO_STD_STRING);
      MessageInterface::SetLogFile(oldLogFile);
   }
   
   #ifdef DEBUG_RUN_SCRIPT_FOLDER
   MessageInterface::ShowMessage
      ("ResourceTree::OnRunScriptsFromFolder() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnQuitRunScriptsFromFolder()
//------------------------------------------------------------------------------
/**
 * Sets user interrupt flag.
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnQuitRunScriptsFromFolder(wxCommandEvent &event)
{
   int answer = wxMessageBox("Do you really want to quit after the current run?",
                             "Confirm", wxYES_NO, this);

   if (answer == wxYES)
      mScriptFolderRunInterrupted = true;
}


//------------------------------------------------------------------------------
// void OnRemoveScriptFolder()
//------------------------------------------------------------------------------
/**
 * Sets user interrupt flag.
 *
 */
//------------------------------------------------------------------------------
void ResourceTree::OnRemoveScriptFolder(wxCommandEvent &event)
{
   wxTreeItemId itemId = GetSelection();

   OnRemoveAllScripts(event);

   Delete(itemId);
   Collapse(mScriptItem);
}


//------------------------------------------------------------------------------
// bool BuildScript(const wxString &filename, Integer scriptOpenOpt,
//                  bool closeScript, bool readBack, const wxString &savePath,
//                  bool multScript)
//------------------------------------------------------------------------------
/**
 * Creates objects from script file.
 *
 * @param <filename> input script file name
 * @param <scriptOpenOpt> open script editor option after build script (0)
 *         0, script file to be opened on error only
 *         1, script file to be opened always
 *         2, NO script file to be opened
 * @param <closeScript> true will close opened script editor
 * @param <readBack> true will read scripts, save, and read back in (false)
 * @param <newPath> new path to be used for saving scripts ("")
 * @param <multiScripts> true if running scripts from the folder (false)
 *
 * @return true if successful; false otherwise
 */
//------------------------------------------------------------------------------
bool ResourceTree::BuildScript(const wxString &filename, Integer scriptOpenOpt,
                               bool closeScript, bool readBack,
                               const wxString &savePath, bool multiScripts)
{
   #ifdef DEBUG_BUILD_OBJECT
   MessageInterface::ShowMessage
      ("ResourceTree::BuildScript() filename=%s, scriptOpenOpt=%d, closeScript=%d, "
       "readBack=%d, multiScripts=%d\n   savePath=%s\n", filename.c_str(),
       scriptOpenOpt, closeScript, readBack, multiScripts, savePath.c_str());
   #endif

   // Set the filename to mainframe, so save will not bring up the file dialog
   bool fileSet =
      theMainFrame->SetScriptFileName(filename.c_str());

   if (fileSet)
   {
      // Interpret script
      bool status =
         theMainFrame->InterpretScript(filename, scriptOpenOpt, closeScript,
                                       readBack, savePath, multiScripts);

      if (!status)
      {
         mBuildErrorCount++;
         mFailedScriptsList.Add(filename);
      }

      #ifdef DEBUG_BUILD_OBJECT
      MessageInterface::ShowMessage
         ("ResourceTree::BuildScript() returning %s\n", (status ? "true" : "false"));
      #endif

      return status;
   }

   #ifdef DEBUG_BUILD_OBJECT
   MessageInterface::ShowMessage("ResourceTree::BuildScript() returning false\n");
   #endif

   return false;
}


//------------------------------------------------------------------------------
// void ShowMenu(wxTreeItemId itemId, const wxPoint& pt)
//------------------------------------------------------------------------------
/**
 * Create the popup menu
 *
 * @param <itemId> tree item that was right clicked
 * @param <pt> location of right click
 */
//------------------------------------------------------------------------------
void ResourceTree::ShowMenu(wxTreeItemId itemId, const wxPoint& pt)
{
   GmatTreeItemData *selItemData = (GmatTreeItemData *)GetItemData(itemId);
   wxString title = selItemData->GetName();
   GmatTree::ItemType itemType = selItemData->GetItemType();
   bool showRenameDelete = true;
   
   // We don't want to show Rename and Delete menu if any panel is open
   // except MissionTree and script editor panels (LOJ: 2012.01.06)
   //@tbs Do we want to show any message? Gray out Rename and Delete?
   //Integer numChildren = theMainFrame->GetNumberOfChildOpen(false, false, false, false);
   // Included plot windows (LOJ: 2013.04.10) to temporarily fix crash renaming plots
   Integer numChildren = theMainFrame->GetNumberOfChildOpen(false, true, false, false);
   if (numChildren > 0)
      showRenameDelete = false;
   
   #ifdef DEBUG_SHOW_MENU
   MessageInterface::ShowMessage
      ("ResourceTree::ShowMenu() title=%s, itemType=%d\n", title.c_str(),
       itemType);
   #endif

#if wxUSE_MENUS
   wxMenu menu;
   
   // How can I change menu text size?
   
   switch (itemType)
   {
   case GmatTree::GROUND_STATION_FOLDER:
      menu.Append(POPUP_ADD_GROUND_STATION, wxT("Add Ground Station"));
      break;
   case GmatTree::SPACECRAFT_FOLDER:
      menu.Append(POPUP_ADD_SPACECRAFT, wxT("Add Spacecraft"));
      break;
   case GmatTree::HARDWARE_FOLDER:
      menu.Append(POPUP_ADD_HARDWARE, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::FORMATION_FOLDER:
      menu.Append(POPUP_ADD_FORMATION, wxT("Add Formation"));
      break;
   case GmatTree::BURN_FOLDER:
      menu.Append(POPUP_ADD_BURN, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::PROPAGATOR_FOLDER:
      menu.Append(POPUP_ADD_PROPAGATOR, wxT("Add Propagator"));
//      #ifdef __USE_SPICE__
//      menu.Append(POPUP_ADD_SPK_PROPAGATOR, wxT("Add SPK Propagator"));
//      #endif
      break;
   case GmatTree::BOUNDARY_SOLVER_FOLDER:
      menu.Append(POPUP_ADD_BOUNDARY_SOLVER, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::OPTIMIZER_FOLDER:
      menu.Append(POPUP_ADD_OPTIMIZER, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::EVENT_LOCATOR_FOLDER:
      menu.Append(POPUP_ADD_LOCATOR, wxT("Add"), CreatePopupMenu(itemType));
      break;

   case GmatTree::SOLAR_SYSTEM:
      //menu.Append(POPUP_ADD_BODY, wxT("Add Body"));
      //menu.Enable(POPUP_ADD_BODY, false);
      break;
   //case GmatTree::CELESTIAL_BODY:
   case GmatTree::CELESTIAL_BODY_STAR:
   case GmatTree::CELESTIAL_BODY_PLANET:
   case GmatTree::CELESTIAL_BODY_MOON:
   case GmatTree::CELESTIAL_BODY_COMET:
   case GmatTree::CELESTIAL_BODY_ASTEROID:
      menu.Append(POPUP_ADD_BODY, _T("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::SPECIAL_POINT_FOLDER:
      menu.Append(POPUP_ADD_SPECIAL_POINT, _T("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::INTERFACE_FOLDER:
      menu.Append(POPUP_ADD_INTERFACE, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::SUBSCRIBER_FOLDER:
      menu.Append(POPUP_ADD_SUBSCRIBER, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::VARIABLE_FOLDER:
      menu.Append(POPUP_ADD_VARIABLE, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::FUNCTION_FOLDER:
      menu.Append(POPUP_ADD_FUNCTION, wxT("Add"), CreatePopupMenu(itemType));
      break;
   case GmatTree::SCRIPT_FOLDER:
      menu.Append(POPUP_ADD_SCRIPT, wxT("Add Script"));
	   //if ((GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING) ||                  // made changes by TUAN NGUYEN
		//   (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS))            // made changes by TUAN NGUYEN
      if ((GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING) ||             // made changes by TUAN NGUYEN
         (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING_NO_PLOTS))       // made changes by TUAN NGUYEN
		menu.Append(POPUP_ADD_SCRIPT_FOLDER, wxT("Add Script Folder"));
      menu.AppendSeparator();
      menu.Append(POPUP_REMOVE_ALL_SCRIPTS, wxT("Remove All"));
      break;
   case GmatTree::ADDED_SCRIPT_FOLDER:
      menu.Append(POPUP_RUN_SCRIPTS_FROM_FOLDER, wxT("Run Scripts"));
      menu.Append(POPUP_QUIT_RUN_SCRIPTS_FROM_FOLDER, wxT("Quit Running Scripts"));

      if (mScriptFolderRunning)
      {
         menu.Enable(POPUP_RUN_SCRIPTS_FROM_FOLDER, false);
         menu.Enable(POPUP_QUIT_RUN_SCRIPTS_FROM_FOLDER, true);
      }
      else
      {
         menu.Enable(POPUP_RUN_SCRIPTS_FROM_FOLDER, true);
         menu.Enable(POPUP_QUIT_RUN_SCRIPTS_FROM_FOLDER, false);
      }
      menu.AppendSeparator();
      menu.Append(POPUP_REMOVE_ALL_SCRIPTS, wxT("Remove All"));
      break;
   case GmatTree::SCRIPT_FILE:
      menu.Append(POPUP_OPEN, wxT("Open"));
      menu.Append(POPUP_CLOSE, wxT("Close"));
      menu.AppendSeparator();
      menu.Append(POPUP_BUILD_SCRIPT, wxT("Save, Sync"));
      menu.Append(POPUP_BUILD_AND_RUN_SCRIPT, wxT("Save, Sync, Run"));
      menu.Append(POPUP_RECOVER_SCRIPT, wxT("Reload"));
      menu.AppendSeparator();
      menu.Append(POPUP_REMOVE_SCRIPT, wxT("Remove"));
      break;
   case GmatTree::COORDINATE_SYSTEM_FOLDER:
      menu.Append(POPUP_ADD_COORD_SYS, wxT("Add Coordinate System"));
      break;
   case GmatTree::PLUGIN_FOLDER:
      {
         // Get type from itemId
         UnsignedInt gmatType = nodeTypeMap[std::string(GetItemText(itemId).c_str())];
         std::string subtype = nodeSubtypeMap[std::string(GetItemText(itemId).c_str())];
         menu.Append(POPUP_ADD_SPECIAL_POINT, _T("Add"),
               CreatePopupMenu(itemType, gmatType, subtype));
      }
      break;
   default:
      break;
   }
   
   // Add MATLAB menu if it is installed
   if (GmatGlobal::Instance()->IsMatlabAvailable())
   {
      switch (itemType)
      {
         // Matlab needs GmatMainFrame interface, so use GmatMenu::
      case GmatTree::MATLAB_INTERFACE:
         menu.Append(GmatMenu::MENU_MATLAB_OPEN, wxT("Open"));
         menu.Append(GmatMenu::MENU_MATLAB_CLOSE, wxT("Close"));
         break;
      case GmatTree::MATLAB_SERVER:
         //if ((GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING) ||            // made changes by TUAN NGUYEN
         //    (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS))     // made changes by TUAN NGUYEN
         //{                                                                               // made changes by TUAN NGUYEN
            // Show MatlabServer items if testing mode (LOJ: 2014.09.24)
            //if ((GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING) ||              // made changes by TUAN NGUYEN
            //    (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING_NO_PLOTS))       // made changes by TUAN NGUYEN
            if ((GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING) ||         // made changes by TUAN NGUYEN
                (GmatGlobal::Instance()->GetRunModeStartUp() == GmatGlobal::TESTING_NO_PLOTS))  // made changes by TUAN NGUYEN

            {
               //#ifdef __ADD_MATLAB_SERVER__
               menu.Append(GmatMenu::MENU_MATLAB_SERVER_START, wxT("Start"));
               menu.Append(GmatMenu::MENU_MATLAB_SERVER_STOP, wxT("Stop"));
               if (mMatlabServerStarted)
               {
                  menu.Enable(GmatMenu::MENU_MATLAB_SERVER_START, false);
                  menu.Enable(GmatMenu::MENU_MATLAB_SERVER_STOP, true);
               }
               else
               {
                  menu.Enable(GmatMenu::MENU_MATLAB_SERVER_START, true);
                  menu.Enable(GmatMenu::MENU_MATLAB_SERVER_STOP, false);
               }
               //#endif
            }
         //}                                                                               // made changes by TUAN NGUYEN
         break;
      default:
         break;
      }   
   }
   
   // menu items applies to most non-folder items
   if (itemType >= GmatTree::BEGIN_OF_RESOURCE &&
       itemType <= GmatTree::END_OF_RESOURCE)
   {
      switch (itemType)
      {
      case GmatTree::SOLAR_SYSTEM:
      case GmatTree::CELESTIAL_BODY:
      case GmatTree::CELESTIAL_BODY_STAR:
      case GmatTree::CELESTIAL_BODY_PLANET:
      case GmatTree::CELESTIAL_BODY_MOON:
      case GmatTree::CELESTIAL_BODY_COMET:
      case GmatTree::CELESTIAL_BODY_ASTEROID:
      case GmatTree::PREDEFINED_BARYCENTER:
      case GmatTree::PREDEFINED_COORDINATE_SYSTEM:
         menu.Append(POPUP_OPEN, wxT("Open"));
         menu.Append(POPUP_CLOSE, wxT("Close"));
         break;
      case GmatTree::SCRIPT_FILE:
         break; // nothing to add
      default:
         menu.Append(POPUP_OPEN, wxT("Open"));
         menu.Append(POPUP_CLOSE, wxT("Close"));
         if (showRenameDelete)
         {
            menu.AppendSeparator();
            menu.Append(POPUP_RENAME, wxT("Rename\tF2"));
            menu.Append(POPUP_DELETE, wxT("Delete\tDEL"));
         }
         menu.AppendSeparator();
         menu.Append(POPUP_CLONE, wxT("Clone\tCtrl+Shift+C"));
         // Disallow clone for Propagator until cloning works for Propagator (GMT-2627)
         if (itemType == GmatTree::PROPAGATOR)
            menu.Enable(POPUP_CLONE, false);
         break;
      }
   }

   PopupMenu(&menu, pt);
#endif // wxUSE_MENUS
}



//------------------------------------------------------------------------------
// void CreatePopupMenuMap()
//------------------------------------------------------------------------------
/**
 *
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
void ResourceTree::CreatePopupMenuMap()
{
   std::vector<MenuData> creatables;
   MenuData entry;

   // Hardware
   entry.guiID = POPUP_ADD_FUELTANK_CHEMICAL;
   entry.menuText = "Chemical Tank";
   creatables.push_back(entry);

   entry.guiID = POPUP_ADD_FUELTANK_ELECTRIC;
   entry.menuText = "Electric Tank";
   creatables.push_back(entry);

   entry.guiID = POPUP_ADD_THRUSTER_CHEMICAL;
   entry.menuText = "Chemical Thruster";
   creatables.push_back(entry);

   entry.guiID = POPUP_ADD_THRUSTER_ELECTRIC;
   entry.menuText = "Electric Thruster";
   creatables.push_back(entry);

   entry.guiID = POPUP_ADD_NUCLEAR_POWER_SYSTEM;
   entry.menuText = "Nuclear Power System";
   creatables.push_back(entry);

   entry.guiID = POPUP_ADD_SOLAR_POWER_SYSTEM;
   entry.menuText = "Solar Power System";
   creatables.push_back(entry);

//   StringArray listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::HARDWARE);
//
//   for (UnsignedInt i = 0; i < listOfObjects.size(); ++i)
//   {
//      if ((listOfObjects[i] == "FuelTank") || (listOfObjects[i] == "ChemicalTank"))
//         entry.guiID = POPUP_ADD_FUELTANK_CHEMICAL;
//      else if (listOfObjects[i]  == "ElectricTank")
//         entry.guiID = POPUP_ADD_FUELTANK_ELECTRIC;
//      else if ((listOfObjects[i]  == "Thruster") || (listOfObjects[i]  == "ChemicalThruster"))
//         entry.guiID = POPUP_ADD_THRUSTER_CHEMICAL;
//      else if (listOfObjects[i]  == "ElectricThruster")
//         entry.guiID = POPUP_ADD_THRUSTER_ELECTRIC;
//      else if (listOfObjects[i]  == "NuclearPowerSystem")
//         entry.guiID = POPUP_ADD_NUCLEAR_POWER_SYSTEM;
//      else if (listOfObjects[i]  == "SolarPowerSystem")
//         entry.guiID = POPUP_ADD_SOLAR_POWER_SYSTEM;
//      else
//         entry.guiID = POPUP_ADD_HARDWARE;
//      entry.menuText = listOfObjects[i];
//      creatables.push_back(entry);
//   }

   #ifdef DEBUG_MENU_MAP
      MessageInterface::ShowMessage("Hardware creatables has %d entries\n",
            creatables.size());
   #endif
   popupMenuMap[GmatTree::HARDWARE_FOLDER] = creatables;

   // Burns
   creatables.clear();
   entry.guiID = POPUP_ADD_IMPULSIVE_BURN;
   entry.menuText = "ImpulsiveBurn";
   creatables.push_back(entry);

   entry.guiID = POPUP_ADD_FINITE_BURN;
   entry.menuText = "FiniteBurn";
   creatables.push_back(entry);

   popupMenuMap[GmatTree::BURN_FOLDER] = creatables;

//      case GmatTree::BOUNDARY_SOLVER_FOLDER:
//         menu->Append(POPUP_ADD_DIFFERENTIAL_CORRECTOR, wxT("DifferentialCorrector"));
//         //menu->Append(POPUP_ADD_BROYDEN, wxT("Broyden"));
//         menu->Enable(POPUP_ADD_BROYDEN, false);
//         break;
//      case GmatTree::OPTIMIZER_FOLDER:
//         //menu->Append(POPUP_ADD_QUASI_NEWTON, wxT("Quasi-Newton"));
//         if (GmatGlobal::Instance()->IsMatlabAvailable())
//            menu->Append(POPUP_ADD_SQP, wxT("SQP (fmincon)"));
//         menu->Enable(POPUP_ADD_QUASI_NEWTON, false);
//
//         listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::SOLVER,
//               "Optimizer");
//         newId = SOLVER_BEGIN;
//         for (StringArray::iterator i = listOfObjects.begin();
//              i != listOfObjects.end(); ++i, ++newId)
//         {
//            // Drop the ones that are already there for now
//            std::string solverType = (*i);
//            if (solverType != "FminconOptimizer")
//            {
//               // Save the ID and type name for event handling
//               pluginMap[POPUP_ADD_SOLVER + newId] = solverType;
//               menu->Append(POPUP_ADD_SOLVER + newId, wxString(solverType.c_str()));
//            }
//         }
//         break;
//      case GmatTree::EVENT_LOCATOR_FOLDER:
//         listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::EVENT_LOCATOR);
//         newId = LOCATOR_BEGIN;
//         for (StringArray::iterator i = listOfObjects.begin();
//              i != listOfObjects.end(); ++i, ++newId)
//         {
//            // Drop the ones that are already there for now
//            std::string locatorType = (*i);
//            // Save the ID and type name for event handling
//            pluginMap[POPUP_ADD_LOCATOR + newId] = locatorType;
//            menu->Append(POPUP_ADD_LOCATOR + newId, wxString(locatorType.c_str()));
//         }
//         break;

   // Subscribers
   creatables.clear();
   entry.guiID = POPUP_ADD_REPORT_FILE;
   entry.menuText = "ReportFile";
   creatables.push_back(entry);

   entry.guiID = POPUP_ADD_XY_PLOT;
   entry.menuText = "XYPlot";
   creatables.push_back(entry);

   entry.guiID = POPUP_ADD_ORBIT_VIEW;
   entry.menuText = "OrbitView";
   creatables.push_back(entry);

   entry.guiID = POPUP_ADD_EPHEMERIS_FILE;
   entry.menuText = "EphemerisFile";
   creatables.push_back(entry);

   popupMenuMap[GmatTree::SUBSCRIBER_FOLDER] = creatables;

   //      case GmatTree::VARIABLE_FOLDER:
//         menu->Append(POPUP_ADD_VARIABLE, wxT("Variable"));
//         menu->Append(POPUP_ADD_ARRAY, wxT("Array"));
//         menu->Append(POPUP_ADD_STRING, wxT("String"));
//         break;
//      case GmatTree::FUNCTION_FOLDER:
//         {
//            #ifdef DEBUG_FUNCTION
//               MessageInterface::ShowMessage("Function types:\n");
//               for (UnsignedInt i = 0; i < functionTypes.size(); ++i)
//                  MessageInterface::ShowMessage("   %s\n", functionTypes[i].c_str());
//            #endif
//            if (find(functionTypes.begin(), functionTypes.end(), "MatlabFunction") !=
//                  functionTypes.end())
//            {
//               menu->Append(POPUP_ADD_MATLAB_FUNCTION, wxT("MATLAB Function"));
//            }
//
//            if (find(functionTypes.begin(), functionTypes.end(), "GmatFunction") !=
//                  functionTypes.end())
//            {
//               wxMenu *gfMenu = new wxMenu;
//               gfMenu->Append(POPUP_NEW_GMAT_FUNCTION, wxT("New"));
//               gfMenu->Append(POPUP_OPEN_GMAT_FUNCTION, wxT("Open"));
//               menu->Append(POPUP_ADD_GMAT_FUNCTION, wxT("GMAT Function"), gfMenu);
//            }
//            break;
//         }
//      case GmatTree::CELESTIAL_BODY:
//      case GmatTree::CELESTIAL_BODY_STAR:
//         menu->Append(POPUP_ADD_PLANET, wxT("Planet"));
//         menu->Append(POPUP_ADD_COMET, wxT("Comet"));
//         menu->Append(POPUP_ADD_ASTEROID, wxT("Asteroid"));
//         break;
//      case GmatTree::CELESTIAL_BODY_PLANET:
//         menu->Append(POPUP_ADD_MOON, wxT("Moon"));
//         break;
//      case GmatTree::CELESTIAL_BODY_MOON:
//         menu->Append(POPUP_ADD_MOON, wxT("Moon"));  // huh?
//         break;
//      case GmatTree::CELESTIAL_BODY_COMET:
//         menu->Append(POPUP_ADD_COMET, wxT("Comet"));  // huh?
//         break;
//      case GmatTree::CELESTIAL_BODY_ASTEROID:
//         menu->Append(POPUP_ADD_ASTEROID, wxT("Asteroid"));
//         break;
//      case GmatTree::SPECIAL_POINT_FOLDER:
//         menu->Append(POPUP_ADD_BARYCENTER, wxT("Barycenter"));
//         menu->Append(POPUP_ADD_LIBRATION, wxT("Libration Point"));
//         break;
//      case GmatTree::INTERFACE_FOLDER:
//         {
//            listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::INTERFACE);
//            if (listOfObjects.size() > 0)
//            {
//               newId = PLUGIN_BEGIN;
//               for (StringArray::iterator i = listOfObjects.begin();
//                    i != listOfObjects.end(); ++i, ++newId)
//               {
//                  // Ignore MatlabInterface
//                  if (*i != "MatlabInterface")
//                  {
//                     // Drop the ones that are already there for now
//                     std::string itemType = (*i);
//                     // Save the ID and type name for event handling
//                     pluginMap[POPUP_ADD_PLUGIN + newId] = itemType;
//                     menu->Append(POPUP_ADD_PLUGIN + newId, wxString(itemType.c_str()));
//                  }
//               }
//            }
//            //else
//            //{
//            //   pluginMap[POPUP_ADD_PLUGIN + PLUGIN_BEGIN] =
//            //         GmatBase::GetObjectTypeString(gmatType);
//            //   menu->Append(POPUP_ADD_PLUGIN + PLUGIN_BEGIN,
//            //         wxT(GmatBase::GetObjectTypeString(gmatType).c_str()));
//            //}
//         }
//         break;
//      case GmatTree::PLUGIN_FOLDER:
//         {
//            listOfObjects = theGuiInterpreter->GetCreatableList(gmatType, subtype);
//
//            if (listOfObjects.size() > 0)
//            {
//               newId = PLUGIN_BEGIN;
//               for (StringArray::iterator i = listOfObjects.begin();
//                    i != listOfObjects.end(); ++i, ++newId)
//               {
//                  // Drop the ones that are already there for now
//                  std::string itemType = (*i);
//                  // Save the ID and type name for event handling
//                  pluginMap[POPUP_ADD_PLUGIN + newId] = itemType;
//                  menu->Append(POPUP_ADD_PLUGIN + newId, wxString(itemType.c_str()));
//               }
//            }
//            else
//            {
//               pluginMap[POPUP_ADD_PLUGIN + PLUGIN_BEGIN] =
//                     GmatBase::GetObjectTypeString(gmatType);
//               menu->Append(POPUP_ADD_PLUGIN + PLUGIN_BEGIN,
//                     wxString(GmatBase::GetObjectTypeString(gmatType).c_str()));
//            }
//         }
//         break;
//      default:
//         break;

   #ifdef DEBUG_MENU_MAP
      MessageInterface::ShowMessage("PopupMenuMap has %d entries\n",
            popupMenuMap.size());
      MessageInterface::ShowMessage("  PopupMenuMap[HARDWARE] has %d entries\n",
            (popupMenuMap[GmatTree::HARDWARE_FOLDER]).size());
      MessageInterface::ShowMessage("  PopupMenuMap[SUBSCRIBER] has %d entries\n",
            (popupMenuMap[GmatTree::SUBSCRIBER_FOLDER]).size());
   #endif
}


//------------------------------------------------------------------------------
// bool UpdatePopupMenuMap(const Integer itemType, const std::string &newItem)
//------------------------------------------------------------------------------
/**
 *
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
bool ResourceTree::UpdatePopupMenuMap(const Integer itemType,
      const Integer itemID, const std::string &newItem)
{
   bool retval = false;

   Integer treeItemType = itemType;    // May need a lookup here

   // -1 indicates a new tree item
   if (treeItemType >= 0)
   {
      if (popupMenuMap.find(treeItemType) != popupMenuMap.end())
      {
         std::vector<MenuData> *entries = &popupMenuMap[treeItemType];
         bool alreadyThere = false;
         for (UnsignedInt i = 0; i < entries->size(); ++i)
            if ((*entries)[i].menuText == newItem)
               alreadyThere = true;

         // Add the entry if it is not in the map
         if (!alreadyThere)
         {
            MenuData entry;
            entry.guiID    = itemID;
            entry.menuText = newItem;
            entries->push_back(entry);
            retval = true;
            #ifdef DEBUG_MENU_MAP
               MessageInterface::ShowMessage("   Adding %s\n", newItem.c_str());
            #endif
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// wxMenu* CreatePopupMenu(GmatTree::ItemType itemType,
//       const UnsignedInt gmatType, const std::string &subtype)
//------------------------------------------------------------------------------
/**
 * Create the popup menu
 *
 * @param itemType Type of object to create and add to tree
 * @param gmatType UnsignedInt of object being created (used for plugin
 *                 objects)
 * @param subtype  Identifier for objects that are a subtype of a core type
 *                 (used for plugin objects)
 *
 * @return <wxMenu> Return popup menu
 */
//------------------------------------------------------------------------------
wxMenu* ResourceTree::CreatePopupMenu(GmatTree::ItemType itemType,
      const UnsignedInt gmatType, const std::string &subtype)
{
   #ifdef DEBUG_POPUP_MENU
   MessageInterface::ShowMessage
      ("ResourceTree::CreatePopupMenu() entered, itemType=%d, gmatType=%d, "
       "sybtype='%s'\n", itemType, gmatType, subtype.c_str());
   #endif
   wxMenu *menu = new wxMenu;

   StringArray listOfObjects;
   Integer newId;
   std::vector<MenuData> entries;

   switch (itemType)
   {
   case GmatTree::HARDWARE_FOLDER:
      entries = popupMenuMap[GmatTree::HARDWARE_FOLDER];
      for (UnsignedInt i = 0; i < entries.size(); ++i)
         menu->Append(entries[i].guiID, entries[i].menuText.c_str());
      #ifdef __ENABLE_NAV_GUI__
      listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::HARDWARE);
      newId = HARDWARE_BEGIN;
      for (StringArray::iterator i = listOfObjects.begin();
           i != listOfObjects.end(); ++i, ++newId)
      {
         // Drop the ones that are already there for now
         std::string hardwareType = (*i);
         if ((hardwareType.find("Thruster") == hardwareType.npos) &&
             (hardwareType.find("Tank") == hardwareType.npos) &&
             (hardwareType.find("PowerSystem") == hardwareType.npos))
         {
            #ifdef DEBUG_POPUP_MENU
            MessageInterface::ShowMessage
               ("   ==> Adding hardwareType '%s' to menu\n", hardwareType.c_str());
            #endif
            // Save the ID and type name for event handling
            pluginMap[POPUP_ADD_HARDWARE + newId] = hardwareType;
            menu->Append(POPUP_ADD_HARDWARE + newId, wxString(hardwareType.c_str()));
         }
      }
      #endif
      break;
   case GmatTree::BURN_FOLDER:
      entries = popupMenuMap[GmatTree::BURN_FOLDER];
      for (UnsignedInt i = 0; i < entries.size(); ++i)
         menu->Append(entries[i].guiID, entries[i].menuText.c_str());
      break;
   case GmatTree::BOUNDARY_SOLVER_FOLDER:
      menu->Append(POPUP_ADD_DIFFERENTIAL_CORRECTOR, wxT("DifferentialCorrector"));
      //menu->Append(POPUP_ADD_BROYDEN, wxT("Broyden"));
      menu->Enable(POPUP_ADD_BROYDEN, false);
      break;
   case GmatTree::OPTIMIZER_FOLDER:
      //menu->Append(POPUP_ADD_QUASI_NEWTON, wxT("Quasi-Newton"));
      if (GmatGlobal::Instance()->IsMatlabAvailable())
      {
#if defined(_WIN32) || defined(__WXMSW__)
         menu->Append(POPUP_ADD_SQP, wxT("SQP (fmincon)"));
#endif
      }
      menu->Enable(POPUP_ADD_QUASI_NEWTON, false);
      
      listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::SOLVER,
            "Optimizer");
      newId = SOLVER_BEGIN;
      for (StringArray::iterator i = listOfObjects.begin();
           i != listOfObjects.end(); ++i, ++newId)
      {
         // Drop the ones that are already there for now
         std::string solverType = (*i);
         if (solverType != "FminconOptimizer")
         {
            // Save the ID and type name for event handling
            pluginMap[POPUP_ADD_SOLVER + newId] = solverType;
            menu->Append(POPUP_ADD_SOLVER + newId, wxString(solverType.c_str()));
         }
      }
      break;
   case GmatTree::EVENT_LOCATOR_FOLDER:
      listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::EVENT_LOCATOR);
      newId = LOCATOR_BEGIN;
      for (StringArray::iterator i = listOfObjects.begin();
           i != listOfObjects.end(); ++i, ++newId)
      {
         // Drop the ones that are already there for now
         std::string locatorType = (*i);
         // Save the ID and type name for event handling
         pluginMap[POPUP_ADD_LOCATOR + newId] = locatorType;
         menu->Append(POPUP_ADD_LOCATOR + newId, wxString(locatorType.c_str()));
      }
      break;
   case GmatTree::SUBSCRIBER_FOLDER:
      entries = popupMenuMap[GmatTree::SUBSCRIBER_FOLDER];
      for (UnsignedInt i = 0; i < entries.size(); ++i)
         menu->Append(entries[i].guiID, entries[i].menuText.c_str());
      
      listOfObjects = theGuiInterpreter->GetListOfViewableSubtypesOf(Gmat::SUBSCRIBER);
      newId = SUBSCRIBER_BEGIN;
      for (StringArray::iterator i = listOfObjects.begin();
           i != listOfObjects.end(); ++i, ++newId)
      {
         // Drop the ones that are already there for now
         std::string subscriberType = (*i);
         if ((subscriberType != "ReportFile") &&
             (subscriberType != "XYPlot") &&
             (subscriberType != "OrbitView") &&
             (subscriberType != "EphemerisFile") &&
             (subscriberType != "TextEphemFile") &&
             (subscriberType != "MessageWindow") &&
             (subscriberType != "MatlabWS"))
         {
            // Save the ID and type name for event handling
            if (menu->FindItem(subscriberType.c_str()) == wxNOT_FOUND)
            {
               pluginMap[POPUP_ADD_SUBSCRIBER + newId] = subscriberType;
               menu->Append(POPUP_ADD_SUBSCRIBER + newId,
                        wxString(subscriberType.c_str()));
               #ifdef DEBUG_USER_OBJECT
                  MessageInterface::ShowMessage("Added subscriber plugin compo"
                        "nent; pluginMap[%d] = %s\n", POPUP_ADD_SUBSCRIBER + newId,
                        subscriberType.c_str());
               #endif
            }
            else
            {
               pluginMap[userIdMap[subscriberType]] = subscriberType;
               #ifdef DEBUG_USER_OBJECT
                  MessageInterface::ShowMessage("Found subscriber plugin compo"
                        "nent; pluginMap[%d] = %s\n", userIdMap[subscriberType],
                        subscriberType.c_str());
               #endif
            }
         }
      }
      
      break;
   case GmatTree::VARIABLE_FOLDER:
      menu->Append(POPUP_ADD_VARIABLE, wxT("Variable"));
      menu->Append(POPUP_ADD_ARRAY, wxT("Array"));
      menu->Append(POPUP_ADD_STRING, wxT("String"));
      break;
   case GmatTree::FUNCTION_FOLDER:
      {
         #ifdef DEBUG_FUNCTION
            MessageInterface::ShowMessage("Function types:\n");
            for (UnsignedInt i = 0; i < functionTypes.size(); ++i)
               MessageInterface::ShowMessage("   %s\n", functionTypes[i].c_str());
         #endif
         if (find(functionTypes.begin(), functionTypes.end(), "MatlabFunction") !=
               functionTypes.end())
         {
            menu->Append(POPUP_ADD_MATLAB_FUNCTION, wxT("MATLAB Function"));
         }
         
         if (find(functionTypes.begin(), functionTypes.end(), "GmatFunction") !=
               functionTypes.end())
         {
            wxMenu *gfMenu = new wxMenu;
            gfMenu->Append(POPUP_NEW_GMAT_FUNCTION, wxT("New"));
            gfMenu->Append(POPUP_OPEN_GMAT_FUNCTION, wxT("Open"));
            menu->Append(POPUP_ADD_GMAT_FUNCTION, wxT("GMAT Function"), gfMenu);
         }
         break;
      }
   case GmatTree::CELESTIAL_BODY:
   case GmatTree::CELESTIAL_BODY_STAR:
      menu->Append(POPUP_ADD_PLANET, wxT("Planet"));
      menu->Append(POPUP_ADD_COMET, wxT("Comet"));
      menu->Append(POPUP_ADD_ASTEROID, wxT("Asteroid"));
      break;
   case GmatTree::CELESTIAL_BODY_PLANET:
      menu->Append(POPUP_ADD_MOON, wxT("Moon"));
      break;
   case GmatTree::CELESTIAL_BODY_MOON:
      menu->Append(POPUP_ADD_MOON, wxT("Moon"));  // huh?
      break;
   case GmatTree::CELESTIAL_BODY_COMET:
      menu->Append(POPUP_ADD_COMET, wxT("Comet"));  // huh?
      break;
   case GmatTree::CELESTIAL_BODY_ASTEROID:
      menu->Append(POPUP_ADD_ASTEROID, wxT("Asteroid"));
      break;
   case GmatTree::SPECIAL_POINT_FOLDER:
      menu->Append(POPUP_ADD_BARYCENTER, wxT("Barycenter"));
      menu->Append(POPUP_ADD_LIBRATION, wxT("Libration Point"));
      break;
   case GmatTree::INTERFACE_FOLDER:
      {
         listOfObjects = theGuiInterpreter->GetCreatableList(Gmat::INTERFACE);
         if (listOfObjects.size() > 0)
         {
            newId = PLUGIN_BEGIN;
            for (StringArray::iterator i = listOfObjects.begin();
                 i != listOfObjects.end(); ++i, ++newId)
            {
               // Ignore MatlabInterface
               if (*i != "MatlabInterface")
               {
                  // Drop the ones that are already there for now
                  std::string itemType = (*i);
                  // Save the ID and type name for event handling
                  pluginMap[POPUP_ADD_PLUGIN + newId] = itemType;
                  menu->Append(POPUP_ADD_PLUGIN + newId, wxString(itemType.c_str()));
               }
            }
         }
         //else
         //{
         //   pluginMap[POPUP_ADD_PLUGIN + PLUGIN_BEGIN] =
         //         GmatBase::GetObjectTypeString(gmatType);
         //   menu->Append(POPUP_ADD_PLUGIN + PLUGIN_BEGIN,
         //         wxT(GmatBase::GetObjectTypeString(gmatType).c_str()));
         //}
      }
      break;
   case GmatTree::PLUGIN_FOLDER:
      {
         listOfObjects = theGuiInterpreter->GetCreatableList(gmatType, subtype);

         if (listOfObjects.size() > 0)
         {
            newId = PLUGIN_BEGIN;
            for (StringArray::iterator i = listOfObjects.begin();
                 i != listOfObjects.end(); ++i, ++newId)
            {
               // Drop the ones that are already there for now
               std::string itemType = (*i);
               // Save the ID and type name for event handling
               pluginMap[POPUP_ADD_PLUGIN + newId] = itemType;
               menu->Append(POPUP_ADD_PLUGIN + newId, wxString(itemType.c_str()));
            }
         }
         else
         {
            pluginMap[POPUP_ADD_PLUGIN + PLUGIN_BEGIN] =
                  GmatBase::GetObjectTypeString(gmatType);
            menu->Append(POPUP_ADD_PLUGIN + PLUGIN_BEGIN,
                  wxString(GmatBase::GetObjectTypeString(gmatType).c_str()));
         }
      }
      break;
   default:
      break;
   }

   #ifdef DEBUG_POPUP_MENU
   MessageInterface::ShowMessage
      ("ResourceTree::CreatePopupMenu() returning menu <%p>\n", menu);
   #endif
   return menu;
}


//------------------------------------------------------------------------------
// UnsignedInt GetObjectType(GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
UnsignedInt ResourceTree::GetObjectType(GmatTree::ItemType itemType)
{
   UnsignedInt objType;

   switch (itemType)
   {
   case GmatTree::GROUND_STATION:
      objType = Gmat::GROUND_STATION;
      break;
   case GmatTree::SPACECRAFT:
      objType = Gmat::SPACECRAFT;
      break;
   case GmatTree::FORMATION:
      objType = Gmat::FORMATION;
      break;
   case GmatTree::IMPULSIVE_BURN:
      objType = Gmat::IMPULSIVE_BURN;
      break;
   case GmatTree::FINITE_BURN:
      objType = Gmat::FINITE_BURN;
      break;
   case GmatTree::PROPAGATOR:
//   case GmatTree::SPK_PROPAGATOR:
      objType = Gmat::PROP_SETUP;
      break;
   case GmatTree::DIFFERENTIAL_CORRECTOR:
   case GmatTree::SQP:
   case GmatTree::SOLVER:
   case GmatTree::OPTIMIZER:
   case GmatTree::SIMULATOR:
   case GmatTree::ESTIMATOR:
      objType = Gmat::SOLVER;
      break;
   case GmatTree::REPORT_FILE:
   case GmatTree::XY_PLOT:
   case GmatTree::ORBIT_VIEW:
   case GmatTree::GROUND_TRACK_PLOT:      
   case GmatTree::EPHEMERIS_FILE:
   case GmatTree::DYNAMIC_DATA_DISPLAY:
   case GmatTree::SUBSCRIBER:       // Added support for plugin Subscribers
      objType = Gmat::SUBSCRIBER;
      break;
   case GmatTree::VARIABLE:
   case GmatTree::ARRAY:
   case GmatTree::STRING:
      objType = Gmat::PARAMETER;
      break;
   case GmatTree::MATLAB_FUNCTION:
   case GmatTree::GMAT_FUNCTION:
      objType = Gmat::FUNCTION;
      break;
   case GmatTree::FUELTANK_CHEMICAL:
   case GmatTree::FUELTANK_ELECTRIC:
   case GmatTree::THRUSTER_CHEMICAL:
   case GmatTree::THRUSTER_ELECTRIC:
   case GmatTree::POWER_SYSTEM:
   case GmatTree::HARDWARE:
   case GmatTree::SENSOR:
      objType = Gmat::HARDWARE;
      break;
   case GmatTree::PREDEFINED_BARYCENTER:
   case GmatTree::BARYCENTER:
   case GmatTree::LIBRATION_POINT:
      objType = Gmat::CALCULATED_POINT;
      break;
   case GmatTree::PREDEFINED_COORDINATE_SYSTEM:
   case GmatTree::USER_COORDINATE_SYSTEM:
      objType = Gmat::COORDINATE_SYSTEM;
      break;
   case GmatTree::EVENT_LOCATOR:
      objType = Gmat::EVENT_LOCATOR;
      break;
   case GmatTree::MEASUREMENT_MODEL:
      objType = Gmat::MEASUREMENT_MODEL;
      break;
   case GmatTree::INTERFACE_OPENABLE:
      objType = Gmat::INTERFACE;
      break;
   default:
      objType = Gmat::UNKNOWN_OBJECT;
      MessageInterface::ShowMessage
         ("**** INTERNAL ERROR **** ResourceTree::GetObjectType() unknown item type: %d\n",
          itemType);
      break;
   }
   
   return objType;

}


//------------------------------------------------------------------------------
// wxTreeItemId GetTreeItemId(GmatTree::ItemType itemType)
//------------------------------------------------------------------------------
wxTreeItemId ResourceTree::GetTreeItemId(GmatTree::ItemType itemType)
{
   #ifdef DEBUG_ITEM_ID
   MessageInterface::ShowMessage
      ("ResourceTree::GetTreeItemId() entered, itemType=%d\n", itemType);
   #endif
   
   switch (itemType)
   {
   case GmatTree::SPACECRAFT:
      return mSpacecraftItem;
      
   case GmatTree::GROUND_STATION:
      return mGroundStationItem;
      
   case GmatTree::FORMATION:
      return mFormationItem;
      
   case GmatTree::IMPULSIVE_BURN:
   case GmatTree::FINITE_BURN:
      return mBurnItem;
      
   case GmatTree::PROPAGATOR:
   //case GmatTree::SPK_PROPAGATOR:
      return mPropagatorItem;
      
   case GmatTree::SOLVER:
      return mSolverItem;
   case GmatTree::DIFFERENTIAL_CORRECTOR:
      return mBoundarySolverItem;
   case GmatTree::OPTIMIZER:
   case GmatTree::SQP:
      return mOptimizerItem;
      
   case GmatTree::SUBSCRIBER:       // Added for GUI Plugin prototype
   case GmatTree::REPORT_FILE:
   case GmatTree::XY_PLOT:
   case GmatTree::ORBIT_VIEW:
   case GmatTree::GROUND_TRACK_PLOT:      
   case GmatTree::EPHEMERIS_FILE:
   case GmatTree::DYNAMIC_DATA_DISPLAY:
      return mSubscriberItem;
      
   case GmatTree::VARIABLE:
   case GmatTree::ARRAY:
   case GmatTree::STRING:
      return mVariableItem;
      
   case GmatTree::PREDEFINED_COORDINATE_SYSTEM:
   case GmatTree::USER_COORDINATE_SYSTEM:
      return mCoordSysItem;
      
   case GmatTree::FUELTANK_CHEMICAL:
   case GmatTree::FUELTANK_ELECTRIC:
   case GmatTree::THRUSTER_CHEMICAL:
   case GmatTree::THRUSTER_ELECTRIC:
   case GmatTree::POWER_SYSTEM:
   case GmatTree::HARDWARE:
      return mHardwareItem;
      
   case GmatTree::PREDEFINED_BARYCENTER:
   case GmatTree::BARYCENTER:
   case GmatTree::LIBRATION_POINT:
      return mSpecialPointsItem;
      
   case GmatTree::INTERFACE:
      return interfaceItem;

   case GmatTree::MATLAB_FUNCTION:
   case GmatTree::GMAT_FUNCTION:
      return mFunctionItem;
      
   case GmatTree::EVENT_LOCATOR:
      return mEventLocatorItem;
      
   default:
      #ifdef DEBUG_ITEM_ID
      MessageInterface::ShowMessage
         ("**** ERROR ****ResourceTree::GetTreeItemId() unknown tree item id.\n");
      #endif
      break;
   }
   
   wxTreeItemId nullId;
   return nullId;

}


//------------------------------------------------------------------------------
// void GetBodyTypeAndIcon(const std::string bodyName, ...)
//------------------------------------------------------------------------------
void ResourceTree::GetBodyTypeAndIcon(const std::string bodyName,
                                      GmatTree::ItemType &bodyItemType,
                                      GmatTree::ResourceIconType &iconType)
{
   SolarSystem   *ss           = theGuiInterpreter->GetSolarSystemInUse();
   CelestialBody *body         = ss->GetBody(bodyName);
   Gmat::BodyType typeOfBody   = body->GetBodyType();
   // These bodies have specific icons ...
   if (bodyName      == GmatSolarSystemDefaults::MERCURY_NAME) iconType = GmatTree::RESOURCE_ICON_MERCURY;
   else if (bodyName == GmatSolarSystemDefaults::VENUS_NAME)   iconType = GmatTree::RESOURCE_ICON_VENUS;
   else if (bodyName == GmatSolarSystemDefaults::EARTH_NAME)   iconType = GmatTree::RESOURCE_ICON_EARTH;
   else if (bodyName == GmatSolarSystemDefaults::MARS_NAME)    iconType = GmatTree::RESOURCE_ICON_MARS;
   else if (bodyName == GmatSolarSystemDefaults::JUPITER_NAME) iconType = GmatTree::RESOURCE_ICON_JUPITER;
   else if (bodyName == GmatSolarSystemDefaults::SATURN_NAME)  iconType = GmatTree::RESOURCE_ICON_SATURN;
   else if (bodyName == GmatSolarSystemDefaults::URANUS_NAME)  iconType = GmatTree::RESOURCE_ICON_URANUS;
   else if (bodyName == GmatSolarSystemDefaults::NEPTUNE_NAME) iconType = GmatTree::RESOURCE_ICON_NEPTUNE;
   else if (bodyName == GmatSolarSystemDefaults::PLUTO_NAME)   iconType = GmatTree::RESOURCE_ICON_PLUTO;
   else if (bodyName == GmatSolarSystemDefaults::MOON_NAME)    iconType = GmatTree::RESOURCE_ICON_MOON;
   else // these bodies don't have specific icons ...
   {
      if (typeOfBody   == Gmat::STAR)        iconType = GmatTree::RESOURCE_ICON_SUN;  // should NOT happen
      else if (typeOfBody == Gmat::PLANET)   iconType = GmatTree::RESOURCE_ICON_PLANET_GENERIC; // GENERIC PLANET ICON TBD
      else if (typeOfBody == Gmat::MOON)     iconType = GmatTree::RESOURCE_ICON_MOON_GENERIC;  // GENERIC MOON ICON TBD
      else if (typeOfBody == Gmat::COMET)    iconType = GmatTree::RESOURCE_ICON_COMET;  // COMET ICON TBD
      else if (typeOfBody == Gmat::ASTEROID) iconType = GmatTree::RESOURCE_ICON_ASTEROID; // ASTEROID ICON TBD
   }
   switch (typeOfBody)
   {
   case Gmat::STAR :
      bodyItemType = GmatTree::CELESTIAL_BODY_STAR;
      break;
   case Gmat::PLANET :
      bodyItemType = GmatTree::CELESTIAL_BODY_PLANET;
      break;
   case Gmat::MOON :
      bodyItemType = GmatTree::CELESTIAL_BODY_MOON;
      break;
   case Gmat::COMET :
      bodyItemType = GmatTree::CELESTIAL_BODY_COMET;
      break;
   case Gmat::ASTEROID :
      bodyItemType = GmatTree::CELESTIAL_BODY_ASTEROID;
      break;
   default:
      bodyItemType = GmatTree::CELESTIAL_BODY;
      break;
   }
}


//------------------------------------------------------------------------------
// int GetNameFromUser(wxString &newName, const wxString &oldName = "",
//                     const wxString &msg = "", const wxString &caption = "")
//------------------------------------------------------------------------------
/**
 * Gets user text input until valid name is entered or canceld
 *
 * @return  1  Valid name entered
 *          2  User canceled or no changes made
 */
//------------------------------------------------------------------------------
int ResourceTree::GetNameFromUser(wxString &newName, const wxString &oldName,
                                  const wxString &msg, const wxString &caption)
{
   newName = wxGetTextFromUser(msg, caption, oldName, this);
   
   // @note
   // There is no way of kwowing whether user entered blank and clicked OK
   // or just clicked Cancel, since wxGetTextFromUser() returns blank for
   // both cases. So if blank is returned, no change is assumed. (loj: 2008.08.29)
   if (newName == "")
      return 2;
   
   // Remove leading and trailing white spaces
   newName = newName.Strip(wxString::both);

   // Check if name starts with number or contains non-alphanumeric character
   if (IsValidName(newName.c_str()))
   {
      // Now check for name already in use
      if (GetObject(newName.c_str()) != NULL)
      {
         MessageInterface::PopupMessage
            (Gmat::ERROR_, "\"%s\" already exists. Please enter a different name.", newName.WX_TO_C_STRING);
         
         return GetNameFromUser(newName, newName, msg, caption);
      }
      // Now check if name is any command type name
      else if (theGuiInterpreter->IsCommandType(newName.c_str()))
      {
         std::string format = GmatStringUtil::GetInvalidNameMessageFormat();
         MessageInterface::PopupMessage
            (Gmat::WARNING_, format.c_str(), newName.WX_TO_C_STRING);
         
         return GetNameFromUser(newName, newName, msg, caption);
      }
      else
      {
         return 1;
      }
   }
   else
   {
      return GetNameFromUser(newName, newName, msg, caption);
   }
}


//------------------------------------------------------------------------------
// void CompareScriptRunResult(Real absTol, const wxString &replaceStr.
//                             const wxString &dir1, const wxString &dir2,
//                             wxTextCtrl *textCtrl)
//------------------------------------------------------------------------------
void ResourceTree::CompareScriptRunResult(Real absTol, const wxString &replaceStr,
                                          const wxString &dir1, const wxString &dir2,
                                          wxTextCtrl *textCtrl)
{
   #ifdef DEBUG_COMPARE_REPORT
   MessageInterface::ShowMessage
      ("ResourceTree::CompareScriptRunResult() absTol=%g, replaceStr=%s\n"
       "   dir1=%s\n   dir2=%s\n   textCtrl=%d\n", absTol, replaceStr.c_str(),
       dir1.c_str(), dir2.c_str(), textCtrl);
   #endif

   if (textCtrl == NULL)
   {
      MessageInterface::ShowMessage
         ("ResourceTree::CompareScriptRunResult() textCtrl is NULL\n");
      return;
   }

   StringArray itemNames =
      theGuiInterpreter->GetListOfObjects(Gmat::SUBSCRIBER);
   int size = itemNames.size();
   std::string objName;
   GmatBase *reportFile;
   int reportCount = 0;

   for (int i = 0; i<size; i++)
   {
      GmatBase *sub = GetObject(itemNames[i]);
      objName = itemNames[i];
      
      if (sub->GetTypeName() == "ReportFile")
      {
         reportFile = GetObject(objName);

         if (!reportFile)
         {
            MessageInterface::ShowMessage
               ("ResourceTree::CompareScriptRunResult() The ReportFile: %s is NULL.\n",
                objName.c_str());
            return;
         }
         
         std::string basefilename = reportFile->GetStringParameter("Filename");
         StringArray colTitles = reportFile->GetRefObjectNameArray(Gmat::PARAMETER);
         
         #ifdef DEBUG_COMPARE_REPORT
         MessageInterface::ShowMessage("   colTitles.size=%d\n", colTitles.size());
         MessageInterface::ShowMessage("   basefilename=%s\n", basefilename.c_str());
         #endif
         
         wxString filename1 = basefilename.c_str();
         wxFileName fname(filename1);
         wxString name2 = fname.GetFullName();
         size_t numReplaced = name2.Replace("GMAT", replaceStr.c_str());
         
         if (numReplaced == 0)
         {
            textCtrl->AppendText
               ("***Cannot compare results. The report file doesn't contain GMAT.\n");
            MessageInterface::ShowMessage
               ("ResourceTree::CompareScriptRunResult() Cannot compare results.\n"
                "The report file doesn't contain GMAT.\n\n");
            return;
         }

         if (numReplaced > 1)
         {
            textCtrl->AppendText
               ("***Cannot compare results. The report file name contains more "
                "than 1 GMAT string.\n");
            MessageInterface::ShowMessage
               ("ResourceTree::CompareScriptRunResult() Cannot compare results.\n"
                "The report file name contains more than 1 GMAT string.\n\n");
            return;
         }

         // set filename1
         filename1 = dir2 + "/" + name2;

         #ifdef DEBUG_COMPARE_REPORT
         MessageInterface::ShowMessage("   filename1=%s\n", filename1.c_str());
         #endif
         
         int file1DiffCount = 0;
         int file2DiffCount = 0;
         int file3DiffCount = 0;
         
         StringArray output =
            GmatFileUtil::CompareTextLines(
               0, basefilename.c_str(), filename1.c_str(), "", "", file1DiffCount,
               file2DiffCount, file3DiffCount);
         
         // append text
         for (unsigned int i=0; i<output.size(); i++)
            textCtrl->AppendText(wxString(output[i].c_str()));

         textCtrl->Show();

         reportCount++;
      }
   }

   if (reportCount == 0)
   {
      textCtrl->AppendText("** There is no report file to compare.\n\n");
      MessageInterface::ShowMessage("** There is no report file to compare.\n");
   }
   else
   {
      textCtrl->AppendText
         ("========================================================\n\n");
   }
}


// Code added to support plugins

//------------------------------------------------------------------------------
// void RegisterGuiPluginResources()
//------------------------------------------------------------------------------
/**
 * Registers plugin GUI elements with the popup menus
 */
//------------------------------------------------------------------------------
void ResourceTree::RegisterGuiPluginResources()
{
   #ifdef DEBUG_USER_GUI
      MessageInterface::ShowMessage("Entered ResourceTree::"
            "RegisterGuiPluginResources()\n");
   #endif

   std::vector<Gmat::PluginResource*> *pr =
         theGuiInterpreter->GetUserResources();
   Integer treeID;

   #ifdef DEBUG_USER_GUI
      MessageInterface::ShowMessage("Updating with %d user resources\n",
            pr->size());
   #endif

   for (UnsignedInt i = 0; i < pr->size(); ++i)
   {
      Gmat::PluginResource *rec = pr->at(i);

      if (rec->handler)
      {
         #ifdef DEBUG_USER_GUI
            MessageInterface::ShowMessage("   %d: %s, under %s\n", i,
                  rec->subtype.c_str(), rec->parentNodeName.c_str());
         #endif

         if ((rec->nodeName != "") || (rec->parentNodeName != ""))
         {
            /// @todo: Add registration mechanism for GmatTree identifiers!  Like
            /// this:  treeID = LookupTreeIdentifier(rec->parentNodeName);
            // Find the matching popup identifier
            if (rec->parentNodeName == "Hardware")
               treeID = GmatTree::HARDWARE_FOLDER;
            if (rec->parentNodeName == "Burn")
               treeID = GmatTree::BURN_FOLDER;
            if (rec->parentNodeName == "Output")
               treeID = GmatTree::SUBSCRIBER_FOLDER;

            rec->firstId = nextGuiPluginId;
            UpdatePopupMenuMap(treeID, nextGuiPluginId, rec->subtype);
            BindPluginGuiPanels(rec);
            ++nextGuiPluginId;
         }
         else
            MessageInterface::ShowMessage("New top level types are not yet "
                  "supported; ignoring GUI entries for %s objects\n",
                  rec->subtype.c_str());
      }
   }
}

//------------------------------------------------------------------------------
// void BindPluginGuiPanels(Gmat::PluginResource *rec)
//------------------------------------------------------------------------------
/**
 * Connects wx event system to plugin GUI panels
 *
 * @param rec The container that contains the plugin panel elements
 */
//------------------------------------------------------------------------------
void ResourceTree::BindPluginGuiPanels(Gmat::PluginResource *rec)
{
   Integer id = rec->firstId;
   if (id < 0)
      id = GetUserID(rec->subtype);

   static Integer uid = 10001;
   if (id == -1)
   {
      id = uid;
      ++uid;
   }

   rec->handler->ConnectEvent(*rec, this, id);
}

//------------------------------------------------------------------------------
// Integer GetUserID(std::string typeName)
//------------------------------------------------------------------------------
/**
 * Retrieves IDs for components
 *
 * @param typeName The type of the component
 *
 * @return The ID for that component type
 */
//------------------------------------------------------------------------------
Integer ResourceTree::GetUserID(std::string typeName)
{
   Integer retval = GmatTree::USER_DEFINED_OBJECT;

   if (userIdMap.find(typeName) != userIdMap.end())
      retval = userIdMap[typeName];
   else
   {
      retval += userIdMap.size();
      userIdMap[typeName] = retval;
   }

   return retval;
}

//------------------------------------------------------------------------------
// void AddUserResources(std::vector<Gmat::PluginResource*> *rcs,
//       bool onlyChildNodes = false)
//------------------------------------------------------------------------------
/**
 * Updates the resource tree with new nodes provided by a plugin.
 *
 * @param rcs The raw data used to update the resource tree
 * @param onlyChildNodes Flag used to indicate that the top level nodes do not
 *                       need to be updated.
 */
//------------------------------------------------------------------------------
void ResourceTree::AddUserResources(std::vector<Gmat::PluginResource*> *rcs,
      bool onlyChildNodes)
{
   #ifdef DEBUG_USER_GUI
   MessageInterface::ShowMessage
      ("ResourceTree::AddUserResources() entered, onlyChildNodes = %d:\n",
       onlyChildNodes);
   #endif
   
   mPluginItems.clear();
   nodeTypeMap.clear();
   nodeSubtypeMap.clear();

   for (UnsignedInt i = 0; i < rcs->size(); ++i)
   {
      Gmat::PluginResource *r = (*rcs)[i];

      // Don't register unnamed nodes
      if (r->nodeName != "")
      {
         wxTreeItemId id;

         #ifdef DEBUG_USER_GUI
            MessageInterface::ShowMessage("Tree resource node:\n"
                  "   Name: %s\n   Parent: %s\n   type: %d\n   subtype: %s\n",
                  r->nodeName.c_str(), r->parentNodeName.c_str(), r->type,
                  r->subtype.c_str());
         #endif

         #ifndef __ENABLE_NAV_GUI__
         // Skip non-viewable resource (LOJ: 2016.06.22)
         if (r->nodeName == "Simulators" || r->nodeName == "Estimators" ||
             r->nodeName == "Measurements")
            continue;
         #endif

         if (onlyChildNodes && (r->parentNodeName == ""))
            id = FindIdOfNode(r->nodeName.c_str(), GetRootItem());
         else
            id = AddUserTreeNode(r->nodeName, r->parentNodeName);

         mPluginItems.push_back(id);
         nodeTypeMap[std::string(GetItemText(id).c_str())] = ((*rcs)[i])->type;
         nodeSubtypeMap[std::string(GetItemText(id).c_str())] = ((*rcs)[i])->subtype;
      }
      
      #ifdef DEBUG_USER_GUI
         MessageInterface::ShowMessage("%d plugin modes registered\n",
               mPluginItems.size());
//            MessageInterface::ShowMessage("   %d: %s %d %s\n", k,
//               GetItemText(mPluginItems[k]).WX_TO_C_STRING,
//               nodeTypeMap[GetItemText(mPluginItems[k]).WX_TO_STD_STRING],
//               nodeSubtypeMap[GetItemText(mPluginItems[k]).WX_TO_STD_STRING].c_str());
      #endif
   }
}


//------------------------------------------------------------------------------
// void AddUserTreeNode(const std::string &newNodeName,
//       const std::string &parent)
//------------------------------------------------------------------------------
/**
 * Adds a folder node into the resource tree
 *
 * @param newNodeName The label for the new node
 * @param parent The parent node's label, or an empty string if the parent is
 *               the root node.
 */
//------------------------------------------------------------------------------
wxTreeItemId ResourceTree::AddUserTreeNode(const std::string &newNodeName,
      const std::string &parent)
{
   #ifdef DEBUG_USER_GUI
   MessageInterface::ShowMessage
      ("ResourceTree::AddUserTreeNode() entered, newNodeName='%s', parent='%s'\n",
       newNodeName.c_str(), parent.c_str());
   #endif
   wxTreeItemId parentId = GetRootItem();
   
   if (parent != "")
   {
      // Find the ID of the parent node
      parentId = FindIdOfNode(parent.c_str(), parentId);
   }
   
   wxTreeItemId newTreeItem;
   AddItemFolder(parentId, newTreeItem, newNodeName.c_str(),
                 GmatTree::PLUGIN_FOLDER);
   
   #ifdef DEBUG_USER_GUI
   MessageInterface::ShowMessage
      ("ResourceTree::AddUserTreeNode() returning newTreeItem\n");
   #endif
   
   return newTreeItem;
}


//------------------------------------------------------------------------------
// wxTreeItemId FindIdOfNode(const wxString &txt, wxTreeItemId start)
//------------------------------------------------------------------------------
/**
 * Finds the tree item ID of a node given the node's text string.
 *
 * This method call is set to be recursive, but the recursion is not yet hooked
 * up.  Currently, only the children of the start node are searched, but not
 * grandchildren and below.
 *
 * @param txt The node label that is being sought
 * @param The id of the initial node
 *
 * @return The id of the found node, to the start node if a child was not found
 *         with the text label.
 */
//------------------------------------------------------------------------------
wxTreeItemId ResourceTree::FindIdOfNode(const wxString &txt, wxTreeItemId start)
{
   wxTreeItemId retval = start;
   bool located = false;

   // Traverse the tree, looking for a node with the input name
   if (GetItemText(start) != txt)
   {
      if (GetChildrenCount(start, false) > 0)
      {
         // Check the node's children
         wxTreeItemIdValue cookie;
         wxTreeItemId cf = GetFirstChild(start, cookie);
         for(UnsignedInt i = 0; i < GetChildrenCount(start, false); ++i)
         {
            if (GetItemText(cf) == txt)
            {
               retval = cf;
               located = true;
               break;
            }
            cf = GetNextChild(start, cookie);
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void MakeScriptActive(const wxString &scriptName, bool active)
//------------------------------------------------------------------------------
/**
 * Makes script active or inactive by changing script text bold face or normal
 *
 * @param  scriptName  Item name of script
 * @param  active      Set this to true if script is active, false otherwise
 */
//------------------------------------------------------------------------------
void ResourceTree::MakeScriptActive(const wxString &scriptName, bool active)
{
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("ResourceTree::MakeScriptActive() entered, scriptName='%s', active=%d\n",
       scriptName.c_str(), active);
   #endif
   
   bool scriptFound = false;
   MakeChildScriptActive(mScriptItem, scriptName, active, scriptFound);
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("ResourceTree::MakeScriptActive() leaving, scriptFound=%d\n", scriptFound);
   #endif
}


//------------------------------------------------------------------------------
// void MakeChildScriptActive(const wxTreeItemId &scriptItem,
//                           const wxString &scriptName, bool active,
//                           bool &scriptFound)
//------------------------------------------------------------------------------
/**
 * Makes child script active or inactive by changing script text bold face or normal
 *
 * @param  scriptItem  Id of script node
 * @param  scriptName  Name of script to find
 * @param  scriptFound Output flag indicating script found
 * @return  wxTreeItemId of last script found
 */
//------------------------------------------------------------------------------
void ResourceTree::MakeChildScriptActive(const wxTreeItemId &scriptItem,
                                         const wxString &scriptName, bool active,
                                         bool &scriptFound)
{
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("ResourceTree::MakeChildScriptActive() entered, scriptItem='%s', "
       "scriptName='%s', active=%d\n", GetItemText(scriptItem).c_str(),
       scriptName.c_str(), active);
   #endif
   
   scriptFound = false;
   wxTreeItemIdValue cookie;
   wxTreeItemId childId = GetFirstChild(scriptItem, cookie);
   wxTreeItemId scriptId = childId;
   wxString scriptPath, nodeText;
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage
      ("   numChildren=%d, firstChildId is %s\n",
       GetChildrenCount(scriptItem), childId.IsOk() ? "OK" : "not OK");
   #endif
   
   wxTreeItemId grandChildId;
   while (childId.IsOk())
   {
      if (ItemHasChildren(childId))
      {
         #ifdef DEBUG_ACTIVE_SCRIPT
         MessageInterface::ShowMessage
            ("   item '%s' has %d children\n", GetItemText(childId).c_str(),
             GetChildrenCount(childId));
         #endif
         MakeChildScriptActive(childId, scriptName, active, scriptFound);
      }
      
      scriptId = childId;
      
      // find child with same path
      nodeText = GetItemText(scriptId);
      
      #ifdef DEBUG_ACTIVE_SCRIPT
      MessageInterface::ShowMessage("   nodeText=<%s>\n", nodeText.c_str());
      #endif
      
      scriptPath = ((GmatTreeItemData *)GetItemData(scriptId))->GetName();
      
      #ifdef DEBUG_ACTIVE_SCRIPT
      MessageInterface::ShowMessage("   scriptPath=<%s>\n", scriptPath.c_str());
      #endif
      
      if (GmatFileUtil::IsSameFileName(nodeText.c_str(), scriptName.c_str()))
      {
         scriptFound = true;
         #ifdef DEBUG_ACTIVE_SCRIPT
         MessageInterface::ShowMessage
            ("   Setting %d %s\n", scriptId, active ? "ACTIVE" : "INACTIVE");
         #endif
         SetItemBold(scriptId, active);
      }
      
      childId = GetNextChild(scriptItem, cookie);
   }
   
   #ifdef DEBUG_ACTIVE_SCRIPT
   MessageInterface::ShowMessage("ResourceTree::MakeChildScriptActive() leaving\n");
   #endif
   
}

