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
// Author: Allison Greene
// Created: 2003/08/28
//
/**
 * This class provides the resource tree and event handlers.
 */
//------------------------------------------------------------------------------
#ifndef ResourceTree_hpp
#define ResourceTree_hpp

#include "gmatwxdefs.hpp"
#include "wx/treectrl.h"
#include "wx/image.h"
#include "wx/imaglist.h"

#include "GuiInterpreter.hpp"
#include "GuiItemManager.hpp"
#include "GmatTreeItemData.hpp"
#include "UserInputValidator.hpp"

class GmatMainFrame;

class ResourceTree : public wxTreeCtrl, public UserInputValidator
{
public:

   // constructor
   ResourceTree(wxWindow *parent, const wxWindowID id,
                const wxPoint& pos, const wxSize& size,
                long style);
   
   void SetMainFrame(GmatMainFrame *gmf);
   void ClearResource(bool leaveScripts, bool onlyChildNodes = true);
   void UpdateResource(bool resetCounter, bool onlyChildNodes = true);
   void SetActiveScript(const wxString &script);
   void AddScript(wxString path);
   bool AddScriptItem(wxString path);
   void PanelObjectChanged( GmatBase *obj );
   void UpdateFormation();
   void UpdateSpacecraft();
   void UpdateVariable();
   void UpdateRecentFiles(wxString filename);
   void OnAddScript(wxCommandEvent &event);
   void UpdateMatlabServerItem(bool start);
   bool WasScriptAdded();
   wxString GetLastScriptAdded();
   
   bool IsScriptFolderRunning();
   void QuitRunningScriptFolder();
   
   // Added so plugin code can update the tree structure
   void RegisterGuiPluginResources();
   void BindPluginGuiPanels(Gmat::PluginResource *rec);
   void AddUserResources(std::vector<Gmat::PluginResource*> *rcs,
         bool onlyChildNodes = false);
   wxTreeItemId AddUserTreeNode(const std::string &newNodeName,
         const std::string &parent = "");
   
protected:
   GmatMainFrame  *theMainFrame;
   GuiInterpreter *theGuiInterpreter;
   GuiItemManager *theGuiManager;
   bool mScriptFolderRunning;
   bool mScriptFolderRunInterrupted;
   bool mScriptAdded;
   bool mMatlabServerStarted;
   bool mHasEventLocatorPlugin;
   wxString mLastScriptAdded;
   wxString mLastActiveScript;
   
   // for script error log
   int mBuildErrorCount;
   wxArrayString mFailedScriptsList;
   
   wxTreeItemId mDraggedItem;
   wxTreeItemId mGroundStationItem;
   wxTreeItemId mSpacecraftItem;
   wxTreeItemId mHardwareItem;
   wxTreeItemId mFormationItem;
   wxTreeItemId mPropagatorItem;
   wxTreeItemId mReportItem;
   wxTreeItemId mBurnItem;
   wxTreeItemId mSubscriberItem;
   wxTreeItemId mSolverItem;
   wxTreeItemId mBoundarySolverItem;
   wxTreeItemId mOptimizerItem;
   wxTreeItemId mVariableItem;
   wxTreeItemId mFunctionItem;
   wxTreeItemId mCoordSysItem;
   //wxTreeItemId mPredefinedFunctItem;
   wxTreeItemId mSpecialPointsItem;
   wxTreeItemId mScriptItem;
   wxTreeItemId mEventLocatorItem;
   wxTreeItemId mScriptFolderItem;
   wxTreeItemId mUniverseItem;
   wxTreeItemId interfaceItem;

   // Plugin top level items go in this collection
   std::vector<wxTreeItemId> mUserItems;

   /// Mapping for plug-in objects
   std::map<Integer, std::string> pluginMap;
   /// Index used for GUI message connections
   Integer nextGuiPluginId;

   /// Structure setting wx ID and string settings
   struct MenuData
   {
      Integer guiID;
      std::string menuText;
   };

   /// Mapping for pop-up menus
   std::map<Integer, std::vector<MenuData>> popupMenuMap;
   /// Plugin tree item IDs
   std::vector<wxTreeItemId> mPluginItems;
   /// node Id -> Type mapping
   std::map<std::string, UnsignedInt> nodeTypeMap;
   /// nodeId -> subtype mapping
   std::map<std::string, std::string> nodeSubtypeMap;
   /// List of the function types supported, so they can move to plugins
   StringArray functionTypes;
   /// User (plugin) type mapping
   std::map<std::string, Integer> userIdMap;
   
   /// Tree item icon Mapping for all object type names (for future use)
   //std::map<std::string, Integer> typeIconMap;
   
   // MSVC compiler will not accept a non-constant size for std::vector
   static const Integer MAX_SUN_ORBITERS;
   
   // icon initialization (for future use)
   //void InitializeIcons();
   
   // objects
   GmatBase* CreateObject(const std::string &objType, const char *objName,
                          bool createDefault = false);
   GmatBase* CreateObject(const std::string &objType, const std::string &objName,
                          bool createDefault = false);
   GmatBase* GetObject(const char *name);
   GmatBase* GetObject(const std::string &name);
   void UpdateGuiItem(GmatTree::ItemType itemType);
   
   // resource tree
   void AddItemFolder(wxTreeItemId parentItemId, wxTreeItemId &itemId,
                      const wxString &itemName, GmatTree::ItemType itemType);
   
public:
   wxTreeItemId AddObjectToTree(GmatBase *obj, bool showDebug = false);
   
protected:
   void AddDefaultResources();
   void AddDefaultBodies(wxTreeItemId itemId);
   void AddDefaultGroundStation(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultSpacecraft(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultHardware(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultFormations(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultConstellations(wxTreeItemId itemId);
   void AddDefaultPropagators(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultLocators(wxTreeItemId itemId, bool restartCounter);
   void AddDefaultBurns(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultSolvers(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultSubscribers(wxTreeItemId itemId, bool resetCounter = true);
   void AddDefaultInterfaces(wxTreeItemId itemId);
   void AddDefaultVariables(wxTreeItemId itemId);
   void AddDefaultFunctions(wxTreeItemId itemId);
   void AddDefaultCoordSys(wxTreeItemId itemId);
   void AddDefaultScripts(wxTreeItemId itemId);
   void AddDefaultSpecialPoints(wxTreeItemId itemId, bool incLibCounter = true,
                                bool resetCounter = true);
   void AddUserObjects();
   
   // event handlers
   void OnItemRightClick(wxTreeEvent& event);
   void OnItemActivated(wxTreeEvent &event);
   void OnItemExpanded(wxTreeEvent &event);
   void OnOpen(wxCommandEvent &event);
   void OnClose(wxCommandEvent &event);
   void OnRename(wxCommandEvent &event);
   void OnDelete(wxCommandEvent &event);
   void OnClone(wxCommandEvent &event);
   void OnEndLabelEdit(wxTreeEvent &event);
   void OnBeginLabelEdit(wxTreeEvent &event);

   void OnBeginDrag(wxTreeEvent &event);
   //void OnBeginRDrag(wxTreeEvent &event);
   void OnEndDrag(wxTreeEvent &event);

   void AddIcons();
   void OnAddBody(wxCommandEvent &event);
   void OnAddImpulsiveBurn(wxCommandEvent &event);
   void OnAddFiniteBurn(wxCommandEvent &event);
   void OnAddPropagator(wxCommandEvent &event);
//   void OnAddSPKPropagator(wxCommandEvent &event);
   void OnAddConstellation(wxCommandEvent &event);
   void OnAddFormation(wxCommandEvent &event);
   void OnAddGroundStation(wxCommandEvent &event);
   void OnAddSpacecraft(wxCommandEvent &event);
   void OnAddChemicalFuelTank(wxCommandEvent &event);
   void OnAddElectricFuelTank(wxCommandEvent &event);
   void OnAddChemicalThruster(wxCommandEvent &event);
   void OnAddElectricThruster(wxCommandEvent &event);
   void OnAddNuclearPowerSystem(wxCommandEvent &event);
   void OnAddSolarPowerSystem(wxCommandEvent &event);
   void OnAddReportFile(wxCommandEvent &event);
   void OnAddXyPlot(wxCommandEvent &event);
   //void OnAddOpenGlPlot(wxCommandEvent &event);
   void OnAddOrbitView(wxCommandEvent &event);
   void OnAddEphemerisFile(wxCommandEvent &event);
   void OnAddSubscriber(wxCommandEvent &event);
   void OnAddDifferentialCorrector(wxCommandEvent &event);
   void OnAddSqp(wxCommandEvent &event);
   void OnAddHardware(wxCommandEvent &event);
   void OnAddSolver(wxCommandEvent &event);
   void OnAddLocator(wxCommandEvent &event);
   void OnAddVariable(wxCommandEvent &event);
   void OnAddArray(wxCommandEvent &event);
   void OnAddString(wxCommandEvent &event);
   void OnAddMatlabFunction(wxCommandEvent &event);
   void OnAddGmatFunction(wxCommandEvent &event);
   void OnAddCoordSystem(wxCommandEvent &event);
   void OnAddBarycenter(wxCommandEvent &event);
   void OnAddLibration(wxCommandEvent &event);
   void OnAddPlanet(wxCommandEvent &event);
   void OnAddMoon(wxCommandEvent &event);
   void OnAddComet(wxCommandEvent &event);
   void OnAddAsteroid(wxCommandEvent &event);
   void OnAddUserObject(wxCommandEvent &event);


   //void OnAddScript(wxCommandEvent &event);
   //void OnNewScript(wxCommandEvent &event);
   void OnRemoveAllScripts(wxCommandEvent &event);
   void OnRemoveScript(wxCommandEvent &event);
   void OnScriptRecover(wxCommandEvent& event);
   void OnScriptBuildObject(wxCommandEvent& event);
   void OnScriptBuildAndRun(wxCommandEvent& event);

   // script folder
   void OnAddScriptFolder(wxCommandEvent &event);
   void OnRunScriptsFromFolder(wxCommandEvent& event);
   void OnQuitRunScriptsFromFolder(wxCommandEvent& event);
   void OnRemoveScriptFolder(wxCommandEvent& event);

   bool BuildScript(const wxString &filename, Integer openScriptOpt = 0,
                    bool closeScript = false, bool readBack = false,
                    const wxString &savePath = "", bool multiScripts = false);

   // menu
   void ShowMenu(wxTreeItemId id, const wxPoint& pt);
   void CreatePopupMenuMap();
   bool UpdatePopupMenuMap(const Integer itemType, const Integer itemID,
         const std::string &newItem);
   wxMenu* CreatePopupMenu(GmatTree::ItemType type,
         const UnsignedInt gmatType = Gmat::UNKNOWN_OBJECT,
         const std::string &subtype = "");
   UnsignedInt GetObjectType(GmatTree::ItemType itemType);
   wxTreeItemId GetTreeItemId(GmatTree::ItemType type);
   
   // icon
   void GetBodyTypeAndIcon(const std::string bodyName,
                           GmatTree::ItemType &bodyItemType,
                           GmatTree::ResourceIconType &iconType);
   
   void GetItemTypeAndIcon(GmatBase *obj, GmatTree::ItemType &itemType,
                           GmatTree::ResourceIconType &itemIcon);
   
   // user text input
   int GetNameFromUser(wxString &newName, const wxString &oldName = "",
                       const wxString &msg = "", const wxString &caption = "");
   
   // compare
   void CompareScriptRunResult(Real absTol, const wxString &replaceStr,
                               const wxString &dir1, const wxString &dir2,
                               wxTextCtrl *textCtrl);

   wxTreeItemId FindIdOfNode(const wxString &txt, wxTreeItemId start);
   void MakeScriptActive(const wxString &scriptName, bool active);
   void MakeChildScriptActive(const wxTreeItemId &scriptItem,
                              const wxString &scriptName, bool active,
                              bool &scriptFound);
   
   Integer GetUserID(std::string);

   DECLARE_EVENT_TABLE();

   // for popup menu
   enum
   {
      POPUP_ADD_SPACECRAFT = 23000,
      POPUP_ADD_GROUND_STATION,
      POPUP_ADD_FORMATION,
      POPUP_ADD_CONSTELLATION,

      POPUP_ADD_HARDWARE,  // includes PowerSystem
      POPUP_ADD_THRUSTER_CHEMICAL,
      POPUP_ADD_THRUSTER_ELECTRIC,
      POPUP_ADD_FUELTANK_CHEMICAL,
      POPUP_ADD_FUELTANK_ELECTRIC,
      POPUP_ADD_NUCLEAR_POWER_SYSTEM,
      POPUP_ADD_SOLAR_POWER_SYSTEM,
      
      POPUP_ADD_PROPAGATOR,
//      POPUP_ADD_SPK_PROPAGATOR,
      POPUP_ADD_BODY,

      POPUP_ADD_BURN,
      POPUP_ADD_IMPULSIVE_BURN,
      POPUP_ADD_FINITE_BURN,

      POPUP_ADD_SOLVER,
      POPUP_ADD_BOUNDARY_SOLVER,
      POPUP_ADD_OPTIMIZER,
      POPUP_ADD_DIFFERENTIAL_CORRECTOR,
      POPUP_ADD_BROYDEN,
      POPUP_ADD_QUASI_NEWTON,
      POPUP_ADD_SQP,

      POPUP_ADD_SUBSCRIBER,
      POPUP_ADD_REPORT_FILE,
      POPUP_ADD_XY_PLOT,
      //POPUP_ADD_OPENGL_PLOT,
      POPUP_ADD_ORBIT_VIEW,
      POPUP_ADD_EPHEMERIS_FILE,
      
      POPUP_ADD_VARIABLE,
      POPUP_ADD_ARRAY,
      POPUP_ADD_STRING,

      POPUP_ADD_COORD_SYS,

      // DataInterface classes
      POPUP_ADD_INTERFACE,
      POPUP_ADD_FILEINTERFACE,

      POPUP_ADD_FUNCTION,
      POPUP_ADD_MATLAB_FUNCTION,
      POPUP_ADD_GMAT_FUNCTION,
      POPUP_NEW_GMAT_FUNCTION,
      POPUP_OPEN_GMAT_FUNCTION,

      POPUP_ADD_SCRIPT,
      POPUP_ADD_SCRIPT_FOLDER,
      POPUP_RUN_SCRIPTS_FROM_FOLDER,
      POPUP_QUIT_RUN_SCRIPTS_FROM_FOLDER,
      POPUP_REMOVE_SCRIPT_FOLDER,
      POPUP_RECOVER_SCRIPT,
      POPUP_BUILD_SCRIPT,
      POPUP_BUILD_AND_RUN_SCRIPT,
      POPUP_REMOVE_ALL_SCRIPTS,
      POPUP_REMOVE_SCRIPT,

      POPUP_ADD_SPECIAL_POINT,
      POPUP_ADD_BARYCENTER,
      POPUP_ADD_LIBRATION,
      POPUP_ADD_PLANET,
      POPUP_ADD_MOON,
      POPUP_ADD_COMET,
      POPUP_ADD_ASTEROID,

      POPUP_ADD_LOCATOR,

      POPUP_ADD_PLUGIN,

      POPUP_OPEN,
      POPUP_CLOSE,
      POPUP_RENAME,
      POPUP_DELETE,
      POPUP_CLONE,
   };
};

#endif // ResourceTree_hpp
