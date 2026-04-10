//$Id$
//------------------------------------------------------------------------------
//                           GmatBaseSetupPanel
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
// number NNG06CA54C
//
// Author: Linda Jun
// Created: 2008/11/19
/**
 * This class is a generic setup panel used by objects derived from GmatBase
 */
//------------------------------------------------------------------------------

#include "GmatBaseSetupPanel.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"         // for GetAbsPathname()
#include <wx/confbase.h>
#include <wx/fileconf.h>
#include <wx/config.h>
#include "StringUtil.hpp"
#include "GmatStaticBoxSizer.hpp"
#include "FileUtil.hpp"
#include <map>
#include <algorithm>
#include <utility> // make_pair
#include "GmatBaseException.hpp"
#include "UtilityException.hpp"
#include "bitmaps/OpenFolder.xpm"

//#define DEBUG_CREATE
//#define DEBUG_LOAD
//#define DEBUG_SAVE
//#define DEBUG_CREATE_CONTROL
//#define DEBUG_CREATE_GROUPS
//#define DEBUG_LOAD_LAYOUT
//#define DEBUG_BUILD_CONTROL
//#define DEBUG_REFRESH
//#define DEBUG_REFRESH_PROPERTY
//#define DEBUG_LOAD_CONTROL
//#define DEBUG_SAVE_CONTROL
//#define DEBUG_COMBOBOX

//-----------------------------------------
// static members
//-----------------------------------------
const wxString GmatBaseSetupPanel::TF_SCHEMES[2] =
   {
      wxT("Off"),
      wxT("On")
   };

/// wxWidget event mappings for the panel
BEGIN_EVENT_TABLE(GmatBaseSetupPanel, GmatPanel)
   EVT_COMBOBOX(ID_COMBOBOX, GmatBaseSetupPanel::OnComboBoxChange)
   EVT_TEXT(ID_COMBOBOX, GmatBaseSetupPanel::OnComboBoxTextChange)
   EVT_TEXT(ID_TEXTCTRL, GmatBaseSetupPanel::OnTextChange)
   EVT_CHECKBOX(ID_CHECKBOX, GmatBaseSetupPanel::OnCheckBoxChange)
   EVT_BUTTON(ID_BUTTON_BROWSE, GmatBaseSetupPanel::OnBrowseButton)
   EVT_COMMAND_RANGE(ID_CHECKLISTBOX, ID_CHECKLISTBOX_LAST, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED, GmatBaseSetupPanel::OnCheckListBoxChange)
END_EVENT_TABLE()


//-----------------------------------------
// public methods
//-----------------------------------------

//------------------------------------------------------------------------------
// GmatBaseSetupPanel(wxWindow *parent, const wxString &name, bool reloadOnCBChange)
//------------------------------------------------------------------------------
/**
 * Panel constructor
 *
 * @param parent Owner for this panel
 * @param name Name of the object that is to be configured
 * @param reloadOnCBChange If this flag is set to true all gui item values will be
 *                         reloadded from the cloned base object when ComboBox
 *                         selection changes
 */
//------------------------------------------------------------------------------
GmatBaseSetupPanel::GmatBaseSetupPanel(wxWindow *parent, const wxString &name,
													bool reloadOnCBChange)
   : GmatPanel(parent)
{
   mObject = theGuiInterpreter->GetConfiguredObject(name.c_str());
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage
      ("\nGmatBaseSetupPanel::Constructor() entered, name='%s', mObject=<%p>\n",
       name.WX_TO_C_STRING, mObject);
   #endif
	reloadOnComboBoxChange = reloadOnCBChange;
   clbNumber = 0;
	
   if (mObject != NULL)
   {
      Create();
      Show();
   }
   else
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The object named \"%s\" does not exist\n", name.WX_TO_C_STRING);
   }

}


//------------------------------------------------------------------------------
// ~GmatBaseSetupPanel()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GmatBaseSetupPanel::~GmatBaseSetupPanel()
{
   // Unregister automatically registered ComboBoxes
   std::map<wxString, wxComboBox *>::iterator iter1;
   std::map<wxString, wxCheckListBox *>::iterator iter2;
   for (iter1 = managedComboBoxMap.begin(); iter1 != managedComboBoxMap.end(); ++iter1)
      theGuiManager->UnregisterComboBox(iter1->first, iter1->second);
   for (iter2 = managedCheckListBoxMap.begin(); iter2 != managedCheckListBoxMap.end(); ++iter2)
      theGuiManager->UnregisterCheckListBox(iter2->first, iter2->second);
   delete localObject;
}


//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Inherited function that is called to create the panel.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::Create()
{
   // create local copy of mObject
   localObject = mObject->Clone();
   
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::Create() entered, localObject=<%p><%s>'%s'\n",
       localObject, localObject->GetTypeName().c_str(), localObject->GetName().c_str());
   #endif
   
   wxFlexGridSizer *mainSizer = new wxFlexGridSizer(1);
   CreateControls(mainSizer, localObject);
   
   // fix tab order
   FixTabOrder( NULL, mainSizer );
   theMiddleSizer->Add(mainSizer, 0, wxALL|wxALIGN_LEFT, 5);
   
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("GmatBaseSetupPanel::Create() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// SizerMapType* CreateGroups(wxFlexGridSizer *mainSizer, wxFileConfig *config)
//------------------------------------------------------------------------------
SizerMapType* GmatBaseSetupPanel::CreateGroups(wxFlexGridSizer *mainSizer,
                                               wxFileConfig *config)
{
   #ifdef DEBUG_CREATE_GROUPS
   MessageInterface::ShowMessage("GmatBaseSetupPanel::CreateGroups() entered\n");
   #endif
   SizerMapType *groups = new SizerMapType;
   wxString groupName;
   wxSizer *sizer;
   wxSizer *parentSizer;
   wxString groupProp;
   wxString parentName;
   long dummy;
   bool doDefaultAction;
   SizerMapType::iterator sizerItem, item;

   // first get all of the groups
   bool bCont = config->GetFirstGroup(groupName, dummy);
   while (bCont)
   {
      #ifdef DEBUG_CREATE_GROUPS
      MessageInterface::ShowMessage("   groupName='%s'\n", groupName.WX_TO_C_STRING);
      #endif
      
      // if not Main group, then it is a group/sizer
      if ((groupName != "main") && (config->Read("/"+groupName+"/Type", &groupProp)))
      {
         // "Type" can be VERTICAL, HORIZONTAL, FLEX, or GRID
         groupProp = groupProp.Lower();
         if (groupProp == "vertical")
           sizer = new GmatStaticBoxSizer( wxVERTICAL, this, config->Read("/"+groupName+"/Label") );
         else if (groupProp == "horizontal")
           sizer = new GmatStaticBoxSizer( wxHORIZONTAL, this, config->Read("/"+groupName+"/Label") );
         else if (groupProp == "flex")
         {
           sizer = new wxFlexGridSizer( config->Read("/"+groupName+"/Rows", (long) 0),
                           config->Read("/"+groupName+"/Columns", (long) 0),
                           config->Read("/"+groupName+"/VerticalGap", (long) 0),
                           config->Read("/"+groupName+"/HorizontalGap", (long) 0));
         }
         else if (groupProp == "grid")
         {
           sizer = new wxGridSizer( config->Read("/"+groupName+"/Rows", (long) 0),
                           config->Read("/"+groupName+"/Columns", (long) 0),
                           config->Read("/"+groupName+"/VerticalGap", (long) 0),
                           config->Read("/"+groupName+"/HorizontalGap", (long) 0));
         }
         else // property or unknown type
            continue;
         groups->insert(std::make_pair(groupName.Lower(), sizer));
      }
      // get next group
      bCont = config->GetNextGroup(groupName, dummy);
   }

   // create an ordered list of groups
   #ifdef DEBUG_CREATE_GROUPS
   MessageInterface::ShowMessage("   Creating an orderd list of groups\n");
   #endif
   StringArray groupNames;
   for (item = groups->begin(); item != groups->end(); ++item)
   {
		groupName = item->first;
		groupNames.push_back(std::string(groupName.Lower().c_str()));

   }
   SortGroups(&groupNames, config);

   // now, for all the groups, add them to their parent
   for (unsigned int i = 0; i < groupNames.size(); i++)
   {
      #ifdef DEBUG_CREATE_GROUPS
      MessageInterface::ShowMessage("   groupNames[%d]='%s'\n", i, groupNames[i].c_str());
      #endif
      item = groups->find(groupNames[i].c_str());
      groupName = item->first;
      sizer = (wxSizer *) item->second;
      // set the parent of the sizer, if doesn't exist, then it is the main sizer
      if (config->Read("/"+groupName+"/Parent", &parentName))
      {
         sizerItem = groups->find(parentName.Lower());
         doDefaultAction = sizerItem == groups->end();
         if (!doDefaultAction)
         {
            parentSizer = ((wxSizer *) sizerItem->second);
            parentSizer->Add(sizer, 0, wxALL|wxALIGN_LEFT|wxEXPAND, border);
         }
         else
            mainSizer->Add(sizer, 0, wxALL|wxALIGN_LEFT|wxEXPAND, border);
      }
      else
         mainSizer->Add(sizer, 0, wxALL|wxALIGN_LEFT|wxEXPAND, border);
   }
   #ifdef DEBUG_CREATE_GROUPS
   MessageInterface::ShowMessage("GmatBaseSetupPanel::CreateGroups() returning groups\n");
   #endif
   return groups;
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Populates the panel with the configurable property data in the Solver
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::LoadData()
{
   #ifdef DEBUG_LOAD
   MessageInterface::ShowMessage("\nGmatBaseSetupPanel::LoadData() entered\n");
   #endif
   // load data from the core engine
   try
   {
      std::string label;
      Integer propertyCount = localObject->GetParameterCount();
      
      #ifdef DEBUG_LOAD
      MessageInterface::ShowMessage("   There are %d properties\n", propertyCount);
      #endif
      
      for (Integer i = 0; i < propertyCount; ++i)
      {
         if (localObject->IsParameterReadOnly(i) == false &&
             localObject->IsParameterVisible(i))
         {
            label = localObject->GetParameterText(i);
            LoadControl(localObject, label);
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("GmatBaseSetupPanel:LoadData() error occurred!\n%s\n",
          e.GetFullMessage().c_str());
   }

   // explicitly disable apply button
   // it is turned on in each of the panels
   EnableUpdate(false);
   
   #ifdef DEBUG_LOAD
   MessageInterface::ShowMessage("GmatBaseSetupPanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
/**
 * Passes configuration data from the panel to the object
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::SaveData()
{
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage("\nGmatBaseSetupPanel::SaveData() entered\n");
   #endif
   
   canClose = true;

   try
   {
      // copy the input to a local variable first to see if there are
      // any errors
      for (std::map<Integer, wxControl *>::iterator i = controlMap.begin();
            i != controlMap.end(); ++i)
      {
         SaveControl(localObject, localObject->GetParameterText(i->first), true);
         if (!canClose)
            return;
      }
      if (!localObject->Validate())
          throw GmatBaseException("Invalid input\n");
      
      // if no errors, copy the object again
      for (std::map<Integer, wxControl *>::iterator i = controlMap.begin();
            i != controlMap.end(); ++i)
      {
         SaveControl(mObject, localObject->GetParameterText(i->first), false);
         if (!canClose)
            return;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
      canClose = false;
      return;
   }
   
   EnableUpdate(false);
   
   #ifdef DEBUG_SAVE
   MessageInterface::ShowMessage("GmatBaseSetupPanel::SaveData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void CreateControls(wxFlexGridSizer *mainSizer, GmatBase *theObject)
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::CreateControls(wxFlexGridSizer *mainSizer, GmatBase *theObject)
{
   #ifdef DEBUG_CREATE_CONTROL
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::CreateControls() entered, theObject=<%p>[%s]'%s'\n", theObject,
       theObject->GetTypeName().c_str(), theObject->GetName().c_str());
   #endif
   
   wxFileConfig *pConfig;
   SizerMapType *groups;
   StringArray propertyNames;
   std::vector<wxString> propertyGroups;
   
   #ifdef DEBUG_CREATE_CONTROL
   MessageInterface::ShowMessage
      ("Creating controls for %s...\n", theObject->GetTypeName().c_str());
   #endif
   GetLayoutConfig( theObject, &pConfig );
   
   wxFlexGridSizer *mainItemSizer = new wxFlexGridSizer(1); // for properties that don't have parents
   mainSizer->Add(mainItemSizer, 0, wxALL|wxALIGN_LEFT, border);
   
   // create the groups
   groups = CreateGroups(mainSizer, pConfig);
   
   // get the property names
   Integer propertyCount = theObject->GetParameterCount();
   for (Integer i = 0; i < propertyCount; ++i)
   {
      #ifdef DEBUG_CREATE_CONTROL
      if (theObject->IsParameterReadOnly(i))
         MessageInterface::ShowMessage
           ("   ParameterText(%d)='%s' (ReadOnly=True)\n", i, theObject->GetParameterText(i).c_str());
      else
        MessageInterface::ShowMessage
          ("   ParameterText(%d)='%s' ReadOnly=False\n", i, theObject->GetParameterText(i).c_str());
      #endif
      if (!theObject->IsParameterReadOnly(i) &&
          theObject->IsParameterVisible(i))
         propertyNames.push_back(theObject->GetParameterText(i));
   }
   
   // Create the properties
   CreateProperties(mainItemSizer, theObject, &propertyNames, groups, pConfig);
   
   delete pConfig;
   delete groups;
   
   #ifdef DEBUG_CREATE_CONTROL
   MessageInterface::ShowMessage("GmatBaseSetupPanel::CreateControls() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// wxControl* CreateControls(wxWindow *parent, Integer index, ...)
//------------------------------------------------------------------------------
/**
 * Creates label, control, and unit widgets for a property
 *
 * @param index The index for the property that the constructed control
 *              represents
 *
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::CreateControls(GmatBase *theObject, Integer index,
                                        wxStaticText **aLabel, wxControl **aControl,
                                        wxControl **aUnit, wxFileConfig *config)
{
   #ifdef DEBUG_CREATE_CONTROL
   MessageInterface::ShowMessage
      ("\nGmatBaseSetupPanel::CreateControls() entered, index=%d\n", index);
   #endif
   
   wxBitmap openBitmap = wxBitmap(OpenFolder_xpm);
   #if __WXMAC__
   int buttonWidth = 40;
   #else
   int buttonWidth = 25;
   #endif
   std::string labelText;
   // set the path to the section that contains the parameter's items
   config->SetPath(wxString(("/"+theObject->GetParameterText(index)).c_str()));
   labelText = GetParameterLabel(theObject, index, config);
   labelText = AssignAcceleratorKey(labelText);
   
   if (theObject->GetParameterType(index) != Gmat::BOOLEAN_TYPE)
      *aLabel = new wxStaticText(this, ID_TEXT, labelText.c_str());
   else
      *aLabel = new wxStaticText(this, ID_TEXT, "");
   
   *aControl = BuildControl(this, theObject, index, labelText, config);
   (*aControl)->Enable(theObject->IsParameterEnabled(index));
   
   Gmat::ParameterType pt = theObject->GetParameterType(index);
   if (pt == Gmat::FILENAME_TYPE)
   {
      *aUnit =
         new wxBitmapButton(this, ID_BUTTON_BROWSE, openBitmap, wxDefaultPosition,
                            wxSize(buttonWidth, -1));
	  //BrowseButtonWildcard = BMP and GIF files (*.bmp;*.gif)|*.bmp;*.gif|PNG files (*.png)|*.png|All Files (*.*)|*.*;
      idInfoMap[(wxObject *) *aUnit] = config->Read(_T("BrowseButtonWildcard"), "All Files (*.*)|*.*");
      (*aUnit)->SetToolTip(config->Read(_T("BrowseButtonHint")));
   }
   else
      *aUnit =
         new wxStaticText(this, ID_TEXT,
                          wxString(GetParameterUnit(theObject, index, config).c_str()));
   
   #ifdef DEBUG_CREATE_CONTROL
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::CreateControls() leaving, index=%d\n", index);
   #endif
}


//------------------------------------------------------------------------------
// wxControl* CreateProperties(wxFlexGridSizer *mainSizer,
//            StringArray propertyNames, SizerMapType *groups, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Creates controls for all the properties of an object and
 * put them in the right groups
 *
 *
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::CreateProperties(wxFlexGridSizer *mainSizer,
                                          GmatBase *theObject, StringArray *propertyNames,
                                          SizerMapType *groups, wxFileConfig *config)
{
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("\nGmatBaseSetupPanel::CreateProperties() entered\n");
   #endif
   Integer j = 0;
   Integer i = 0;
   wxString groupProp;
   wxFlexGridSizer *itemSizer;
   wxSizer *sizer;
   std::vector<wxString> propertyGroups;
   wxStaticText *aLabel;
   wxControl *aControl;
   wxControl *aUnit;
   /// Labels used for the configurable properties
   std::vector<wxStaticText*>       propertyDescriptors;
   /// GUI controls that are used to configure the properties
   std::vector<wxControl*>          propertyControls;
   /// Units used for the configurable properties
   std::vector<wxControl*>          propertyUnits;
   
   
   // sort the properties per the config file
   SortProperties( propertyNames, config );
   
   // now go through the properties and create their controls
   StringArray::iterator propertyItem;
   for(propertyItem = propertyNames->begin(), j=0;
       propertyItem != propertyNames->end(); ++propertyItem, ++j)
   {
      i = theObject->GetParameterID(*propertyItem);
      CreateControls(theObject, i, &aLabel, &aControl, &aUnit, config);
      propertyDescriptors.push_back(aLabel);
      controlMap[i] = aControl;
      inverseControlMap[aControl] = i;
      propertyControls.push_back(aControl);
      propertyUnits.push_back(aUnit);
   }
   
   // three columns: description, control, unit
   std::vector<wxStaticText*>::iterator item;
   
   // find the best minimum size for the description/unit for each sizer/group
   
   // Add the 3 columns to a sizer
   SizerMapType::iterator sizerItem;
   bool doDefaultAction;
   
   for(item = propertyDescriptors.begin(), j = 0;
       item != propertyDescriptors.end(); ++item, ++j)
   {
      itemSizer = new wxFlexGridSizer(3);
      itemSizer->Add(*item, 0, wxALL|wxALIGN_RIGHT, border);
      itemSizer->Add(propertyControls[j], 0, wxALL|wxALIGN_LEFT, border);
      itemSizer->Add(propertyUnits[j], 0, wxALL|wxALIGN_LEFT, border);
      // set the path to the section that contains the parameter's items
      config->SetPath(STD_TO_WX_STRING(("/"+(*propertyNames)[j])).WX_TO_STD_STRING);
      groupProp = "Parent";
      doDefaultAction = !(config->Read(groupProp, &groupProp));
      if (!doDefaultAction)
      {
         // save the group for normalizing labels later
         propertyGroups.push_back(groupProp);
         // get the group and its itemsizer inside
         sizerItem = groups->find(groupProp.Lower());
         doDefaultAction = sizerItem == groups->end();
         if (!doDefaultAction)
         {
            sizer = ((wxSizer *) sizerItem->second);
            sizer->Add(itemSizer, 0, wxALL|wxALIGN_LEFT, 0);
         }
      }
      if (doDefaultAction)
      {
         // save the group for normalizing labels later
         propertyGroups.push_back("Main");
         mainSizer->Add(itemSizer, 0, wxALL|wxALIGN_LEFT, 0);
      }
   }

   
   
//    // see if there are any LoadSubProperties
//    wxString areSubProperties;
//    std::string refObjectName;
//    GmatBase *subObject;
//    for(propertyItem = propertyNames->begin(), j=0;
//        propertyItem != propertyNames->end(); ++propertyItem, ++j)
//    {
//       // set the path to the section that contains the parameter's items
//       config->SetPath(wxT(("/"+(*propertyNames)[j])).c_str());
//       doDefaultAction = config->Read(wxT("LoadSubProperties"), &areSubProperties);
//       if ((doDefaultAction) && (areSubProperties.Lower() == "true"))
//       {
//          groupProp = "SubPropertiesParent";
//          doDefaultAction = !(config->Read(groupProp, &groupProp));
//          if (!doDefaultAction)
//          {
//             // get the group and its itemsizer inside
//             sizerItem = groups->find(groupProp.Lower());
//             doDefaultAction = sizerItem == groups->end();
//             if (!doDefaultAction)
//             {
//                sizer = ((wxSizer *) sizerItem->second);
//                itemSizer = new wxFlexGridSizer(1);
//                sizer->Add(itemSizer, 0, wxALL|wxALIGN_LEFT, 0);
//                try
//                {
//                   refObjectName = theObject->
//                      GetRefObjectName(theObject->GetPropertyObjectType
//                                       (theObject->GetParameterID((*propertyNames)[j])));
//                   if (refObjectName != "")
//                   {
//                      subObject = theObject->GetRefObject
//                         (theObject->GetPropertyObjectType
//                          (theObject->GetParameterID((*propertyNames)[j])), refObjectName);
//                      if (subObject == NULL)
//                         MessageInterface::ShowMessage
//                            ("Programmer error: Object property is NULL for %s...\n",
//                             refObjectName.c_str());
//                      else
//                         CreateControls(mainSizer, subObject);
//                   }
//                }
//                catch (BaseException &e)
//                {
//                   MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
//                }
//             }
//          }
//          else
//          {
            
//          }
//       }
//    }
   
   
   NormalizeLabels( *propertyNames, propertyGroups, propertyDescriptors, propertyControls, propertyUnits );
   
   #ifdef DEBUG_CREATE
   MessageInterface::ShowMessage("GmatBaseSetupPanel::CreateProperties() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// wxControl* BuildControl(wxWindow *parent, Integer index)
//------------------------------------------------------------------------------
/**
 * Builds a wxWidget control for an object property
 *
 * @param parent The window that owns the control
 * @param index The index for the property that the constructed control
 *              represents
 *
 * @return The new control
 */
//------------------------------------------------------------------------------
wxControl *GmatBaseSetupPanel::BuildControl(wxWindow *parent, GmatBase *theObject,
                                            Integer index, const std::string &label,
                                            wxFileConfig *config)
{
   #ifdef DEBUG_BUILD_CONTROL
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::BuildControl() entered, label = '%s'\n", label.c_str());
   #endif
   
   wxControl *control = NULL;
   
   Gmat::ParameterType paramType = theObject->GetParameterType(index);
   UnsignedInt paramObjType = theObject->GetPropertyObjectType(index);
   #ifdef DEBUG_BUILD_CONTROL
   MessageInterface::ShowMessage
      ("BuildControl() object type is %s\n", GmatBase::GetObjectTypeString(paramObjType).c_str());
   #endif
   
   #ifdef DEBUG_BUILD_CONTROL
   MessageInterface::ShowMessage
      ("Building control %s (type=%s)\n", label.c_str(),
       theObject->GetParameterTypeString(index).c_str());
   #endif
   switch (paramType)
   {
   case Gmat::ON_OFF_TYPE:
      {
         wxComboBox *cbControl =
            new wxComboBox(parent, ID_COMBOBOX, "true",
                           wxDefaultPosition, wxDefaultSize, 2, TF_SCHEMES,
                           wxCB_READONLY);
         
         control = cbControl;
      }
      break;
   case Gmat::BOOLEAN_TYPE:
      {
         wxCheckBox *cbControl = new wxCheckBox( this, ID_CHECKBOX, wxString(label.c_str()));
         cbControl->SetToolTip(config->Read(_T("Hint")));
         control = cbControl;
      }
      break;
   case Gmat::OBJECT_TYPE:
      {
         if (paramObjType== Gmat::SPACE_POINT)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetSpacePointComboBox(this, ID_COMBOBOX,
                                                    wxSize(200,-1), false);
            managedComboBoxMap.insert(std::make_pair("SpacePoint", cbControl));
            control = cbControl;
         }
         else if (paramObjType== Gmat::CELESTIAL_BODY)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any CelestialBody updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
            theGuiManager->GetCelestialBodyComboBox(this, ID_COMBOBOX,
                                                    wxSize(200,-1));
            managedComboBoxMap.insert(std::make_pair("CelestialBody", cbControl));
            control = cbControl;
         }
         else if (paramObjType== Gmat::SPACECRAFT)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any Spacecraft updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetSpacecraftComboBox(this, ID_COMBOBOX,
                                                    wxSize(200,-1));
            managedComboBoxMap.insert(std::make_pair("Spacecraft", cbControl));
            control = cbControl;
         }
         else if (paramObjType== Gmat::COORDINATE_SYSTEM)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any CoordinateSystem updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetCoordSysComboBox(this, ID_COMBOBOX,
                                                  wxSize(200,-1));
            managedComboBoxMap.insert(std::make_pair("CoordinateSystem", cbControl));
            control = cbControl;
         }
         else if (paramObjType== Gmat::ANTENNA)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any Antenna updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetAntennaComboBox(this, ID_COMBOBOX,
                                                  wxSize(200,-1));
            managedComboBoxMap.insert(std::make_pair("Antenna", cbControl));
            control = cbControl;
         }
         else if (paramObjType== Gmat::SENSOR)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any Sensor updates, so need to unregister
            // in the destructor
            wxComboBox *cbControl =
               theGuiManager->GetSensorComboBox(this, ID_COMBOBOX,
                                                  wxSize(200,-1));
            managedComboBoxMap.insert(std::make_pair("Sensor", cbControl));
            control = cbControl;
         }
         else
         {
            // Check if enumeration strings available for owned object types
            wxArrayString enumList;
            StringArray enumStrings = theGuiInterpreter->GetListOfObjects(paramObjType);
            for (UnsignedInt i=0; i<enumStrings.size(); i++)
               enumList.Add(enumStrings[i].c_str());
            control = new wxComboBox(parent, ID_COMBOBOX, wxT(""),
                                     wxDefaultPosition, wxSize(200,-1), enumList,
                                     wxCB_READONLY);
         }
      }
      break;
   case Gmat::OBJECTARRAY_TYPE:
      {
         if (paramObjType== Gmat::SPACE_POINT)
         {
            // The GuiItemManager automatically registers wxComboBox in order to
            // listen for any SpacePoint updates, so need to unregister
            // in the destructor
            wxCheckListBox *clbControl =
               theGuiManager->GetSpacePointCheckListBox(this, ID_CHECKLISTBOX+clbNumber,
                                                        wxSize(200,100), true, true, true, false);
            managedCheckListBoxMap.insert(std::make_pair("CelestialBody", clbControl));
            control = clbControl;
            clbNumber++;
         }
         else
         {
            wxArrayString theList;
            // Get the list of types needed
            ObjectTypeArray types = theObject->GetTypesForList(index);
            // Add the existing objects to the list
            for (UnsignedInt i = 0; i < types.size(); ++i)
            {
               StringArray objs = theGuiInterpreter->GetListOfObjects(types[i]);
               for (UnsignedInt j = 0; j < objs.size(); ++j)
               {
                  theList.Add(wxString(objs[j].c_str()));
               }
            }
            control = new wxCheckListBox(parent, ID_CHECKLISTBOX+clbNumber, wxDefaultPosition,
                                         wxSize(200,100), theList, wxLB_SINGLE|wxLB_SORT|wxLB_HSCROLL);
            clbNumber++;
         }
      }
      break;
   case Gmat::ENUMERATION_TYPE:
      {
         StringArray enumStrings = theObject->GetPropertyEnumStrings(index);
         wxArrayString enumList;
         for (UnsignedInt i=0; i<enumStrings.size(); i++)
            enumList.Add(enumStrings[i].c_str());
         
         wxComboBox *cbControl = NULL;
         if (enumStrings.size() == 1)
         {
            // Show the value even if value is not in the list
            cbControl =
               new wxComboBox(parent, ID_COMBOBOX, "", wxDefaultPosition,
                              wxSize(200,-1), enumList, 0);
         }
         else
         {
            // Do not show the value if value is not in the list
            cbControl =
               new wxComboBox(parent, ID_COMBOBOX, "", wxDefaultPosition,
                              wxSize(200,-1), enumList, wxCB_READONLY);
         }
         
         control = cbControl;
      }
      break;
   case Gmat::REAL_TYPE:
   case Gmat::INTEGER_TYPE:
      {
         control = new wxTextCtrl(parent, ID_TEXTCTRL,
                                  wxT(""), wxDefaultPosition, wxSize(200,-1), 0,
                                  wxTextValidator(wxGMAT_FILTER_NUMERIC));
      }
      break;
   case Gmat::FILENAME_TYPE:
      control = new wxTextCtrl(parent, ID_TEXTCTRL,
                               wxT(""), wxDefaultPosition, wxSize(250,-1));
		break;
   case Gmat::STRING_TYPE:
   default:
      control = new wxTextCtrl(parent, ID_TEXTCTRL,
                               wxT(""), wxDefaultPosition, wxSize(200,-1));
      break;
   }
   
   control->SetToolTip(config->Read(_T("Hint")));

   #ifdef DEBUG_BUILD_CONTROL
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::BuildControl() leaving, label = '%s'\n", label.c_str());
   #endif
   
   return control;
}


//------------------------------------------------------------------------------
// void LoadControl(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Sets the data for a control
 *
 * @param label The control's label
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::LoadControl(GmatBase *theObject, const std::string &label)
{
   #ifdef DEBUG_LOAD_CONTROL
   MessageInterface::ShowMessage
      ("\nGmatBaseSetupPanel::LoadControl() entered, label = '%s'\n", label.c_str());
   #endif
   
   Integer paramId = theObject->GetParameterID(label);
   Gmat::ParameterType paramType = theObject->GetParameterType(paramId);
   
   #ifdef DEBUG_LOAD_CONTROL
   MessageInterface::ShowMessage("   paramId=%d, paramType=%d\n", paramId, paramType);
   #endif
   
   if (controlMap.find(paramId) == controlMap.end())
   {
      #ifdef DEBUG_LOAD_CONTROL
      MessageInterface::ShowMessage
         ("GmatBaseSetupPanel::LoadControl() just leaving, wxControl not found for "
          "label:'%s', paramId:%d\n", label.c_str(), paramId);
      #endif
      return;
   }
   
   wxControl *theControl = controlMap[paramId];
   wxString valueString;
   
   switch (paramType)
   {
      case Gmat::ON_OFF_TYPE:
         ((wxComboBox*)(theControl))->
            SetValue(wxString(theObject->GetOnOffParameter(paramId).c_str()));
         break;
      case Gmat::BOOLEAN_TYPE:
         #ifdef DEBUG_LOAD_CONTROL
         MessageInterface::ShowMessage("   It is BOOLEAN_TYPE\n");
         #endif
         ((wxCheckBox*) theControl)->SetValue(theObject->GetBooleanParameter(paramId));
         break;
      case Gmat::REAL_TYPE:
         {
            Real val = theObject->GetRealParameter(paramId);
            std::stringstream valstr;
            valstr.precision(16);
            valstr << val;
            std::string cvtr = valstr.str();
            valueString << wxString(cvtr.c_str());
            ((wxTextCtrl*)theControl)->ChangeValue(valueString);
         }
         break;

      case Gmat::INTEGER_TYPE:
         {
            #ifdef DEBUG_LOAD_CONTROL
            MessageInterface::ShowMessage("   It is INTEGER_TYPE\n");
            #endif
            Integer val = theObject->GetIntegerParameter(paramId);
            valueString << val;
            ((wxTextCtrl*)theControl)->ChangeValue(valueString);
         }
         break;

      case Gmat::FILENAME_TYPE:
      case Gmat::STRING_TYPE:
         valueString = wxString(theObject->GetStringParameter(label).c_str());
         ((wxTextCtrl*)theControl)->ChangeValue(valueString);
			((wxTextCtrl*)theControl)->SetInsertionPointEnd();
         break;
         
      case Gmat::OBJECT_TYPE:
         valueString = (theObject->GetStringParameter(paramId)).c_str();
         #ifdef DEBUG_LOAD_CONTROL
         MessageInterface::ShowMessage("   valueString = '%s'\n", valueString.c_str());
         #endif         
         ((wxComboBox*)theControl)->SetStringSelection(valueString);
         //((wxComboBox*)theControl)->Append(valueString); // removed for Bug 1621 wcs 2009.11.10
         break;
      case Gmat::OBJECTARRAY_TYPE:
         {
            StringArray objs = theObject->GetStringArrayParameter(paramId);
            wxCheckListBox *clb = (wxCheckListBox*)theControl;
            int count = clb->GetCount();
            for (UnsignedInt i = 0; i < objs.size(); i++)
            {
               wxString objName = objs[i].c_str();
               #ifdef DEBUG_LOAD_CONTROL
               MessageInterface::ShowMessage("   objName = '%s'\n", objName.c_str());
               #endif
               for (int j = 0; j < count; j++)
               {
                  if (objName == clb->GetString(j))
                     clb->Check(j, true);
               }
            }
            break;
         }
      case Gmat::STRINGARRAY_TYPE:
         {
            StringArray strings = theObject->GetStringArrayParameter(paramId);
            wxString theList;
            for (UnsignedInt i = 0; i < strings.size(); i++)
            {
               theList += wxString(strings[i].c_str());
               if (i < strings.size() - 1)
                  theList += wxT(", ");
            }
            ((wxTextCtrl*)theControl)->ChangeValue(theList);
            break;
         }
      case Gmat::ENUMERATION_TYPE:
         {
            #ifdef DEBUG_LOAD_CONTROL
            MessageInterface::ShowMessage("   It is ENUMERATION_TYPE\n");
            #endif
            valueString = (theObject->GetStringParameter(paramId)).c_str();
            #ifdef DEBUG_LOAD_CONTROL
            MessageInterface::ShowMessage("   valueString='%s'\n", valueString.c_str());
            #endif
            wxComboBox *cb = (wxComboBox*)theControl;
            cb->SetValue(valueString);
            
            // if ComboBox is not read only, add value to list
            if (cb->GetWindowStyleFlag() != wxCB_READONLY)
               cb->Append(valueString);
            break;
         }
      default:
         #ifdef DEBUG_LOAD_CONTROL
         MessageInterface::ShowMessage("===> This type is not handled yet\n");
         #endif
         break;
   }
   #ifdef DEBUG_LOAD_CONTROL
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::LoadControl() leaving, label = '%s'\n", label.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void SaveControl(GmatBase *theObject, const std::string &label, ...)
//------------------------------------------------------------------------------
/**
 * Passes a control's data to the object.  Returns true if parameter was saved to
 * object
 *
 * @param label The string associated with the control.
 */
//------------------------------------------------------------------------------
bool GmatBaseSetupPanel::SaveControl(GmatBase *theObject, const std::string &label,
                                     bool showErrorMsgs)
{
   #ifdef DEBUG_SAVE_CONTROL
   MessageInterface::ShowMessage
      ("SaveControl() entered, theObject=<%p>, label='%s', "
       "showErrorMsgs=%d\n", theObject, label.c_str(), showErrorMsgs);
   #endif
   
   Integer paramId = theObject->GetParameterID(label);
   Gmat::ParameterType type = theObject->GetParameterType(paramId);
   
   wxControl *theControl = controlMap[paramId];
   std::string valueString;
   
   switch (type)
   {
   case Gmat::ON_OFF_TYPE:
      {
         valueString = ((wxComboBox*)theControl)->GetValue();
         theObject->SetOnOffParameter(paramId, valueString);
      }
      break;
   case Gmat::BOOLEAN_TYPE:
      {
         theObject->SetBooleanParameter(paramId, ((wxCheckBox*)theControl)->GetValue());
      }
      break;
   case Gmat::REAL_TYPE:
      {
         Real val;
         valueString = ((wxTextCtrl*)theControl)->GetValue();
         if (showErrorMsgs)
         {
            CheckReal(val, valueString, label, "Real Number", false);
            if (!canClose)
               return false;
         }
         else if (!GmatStringUtil::ToReal(valueString, &val))
            return false;
         theObject->SetRealParameter(paramId, val);
      }
      break;
      
   case Gmat::INTEGER_TYPE:
      {
         Integer val;
         valueString = ((wxTextCtrl*)theControl)->GetValue();
         if (showErrorMsgs)
         {
            CheckInteger(val, valueString, label, "Integer", false);
            if (!canClose)
               return false;
            
         }
         else if (!GmatStringUtil::ToInteger(valueString, &val))
            return false;
         theObject->SetIntegerParameter(paramId, val);
      }
      break;
      
   case Gmat::FILENAME_TYPE:
   case Gmat::STRING_TYPE:
      valueString = ((wxTextCtrl*)theControl)->GetValue();
      theObject->SetStringParameter(paramId, valueString.c_str());
      break;
      
   case Gmat::OBJECT_TYPE:
   case Gmat::ENUMERATION_TYPE:
      valueString = ((wxComboBox*)theControl)->GetValue();
      theObject->SetStringParameter(paramId, valueString);
      break;
      
   case Gmat::OBJECTARRAY_TYPE:
      {
         theObject->TakeAction("Clear", label);
         wxCheckListBox *clb = (wxCheckListBox*)theControl;
         int count = clb->GetCount();
         for (int i = 0; i < count; i++)
         {
            if (clb->IsChecked(i))
            {
               std::string objName =  clb->GetString(i).WX_TO_STD_STRING;
               #ifdef DEBUG_SAVE_CONTROL
               MessageInterface::ShowMessage("   objName = '%s'\n", objName.c_str());
               #endif
               theObject->SetStringParameter(paramId, objName);
            }
         }
         break;
      }
      
   case Gmat::STRINGARRAY_TYPE:
      {
         StringArray strings = theObject->GetStringArrayParameter(paramId);
         wxString theList;
         for (UnsignedInt i = 0; i < strings.size(); i++)
         {
            theList += wxString(strings[i].c_str());
            if (i < strings.size() - 1)
               theList += wxT(", ");
         }
         ((wxTextCtrl*)theControl)->ChangeValue(theList);
         break;
      }


   default:
      break;
   }
   
   #ifdef DEBUG_SAVE_CONTROL
   MessageInterface::ShowMessage("SaveControl() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// std::string AssignAcceleratorKey(std::string text)
//------------------------------------------------------------------------------
std::string GmatBaseSetupPanel::AssignAcceleratorKey(std::string text)
{
   // find best candidate
   // 1) Beginning of word
   // accelKeys contains list of already used accelerator keys (lower case)
   
   unsigned int accelIndex = 0;
   bool haveCandidate = false;
   bool findWord = true;  // true when flying over spaces in string
   unsigned int iText;
   std::string titleText = "";
   
   // first, check to see if accelerator has already been assigned (can happen if label
   // is loaded from INI file
   size_t found = text.find(GUI_ACCEL_KEY);
   if ( found != std::string::npos)
   {
      // add to accel keys
      accelKeys.push_back(tolower(text[found+1]));
      return text;
   }
   
   for (iText=0;iText<text.length();iText++)
   {
      // if character has not already been used
      if (find (accelKeys.begin(), accelKeys.end(), tolower(text[iText])) == accelKeys.end())
      {
         // if it is the beginning of a word alphanumeric
         if ((findWord) && (isalnum(text[iText])))
         {
            accelIndex = iText;
            accelKeys.push_back(tolower(text[iText]));
            if (accelIndex > 0)
               titleText.append(text, 0, accelIndex);
            titleText += GUI_ACCEL_KEY;
            titleText.append(text, accelIndex, text.length());
            return titleText;
         }
         // if in the middle of a word and we don't already have a candidate
         if ((!haveCandidate) && (isalnum(text[iText])))
         {
            accelIndex = iText;
            haveCandidate = true;
         }
      }
      if ((!findWord) && (!isalnum(text[iText])))
         findWord = true;
      if ((findWord) && (isalnum(text[iText])))
         findWord = false;
   }
   if (haveCandidate)
   {
      accelKeys.push_back(tolower(text[accelIndex]));
      titleText.append(text, 0, accelIndex);
      titleText += GUI_ACCEL_KEY;
      titleText.append(text, accelIndex, text.length());
   }
   return titleText;
}


//------------------------------------------------------------------------------
// bool GetLayoutConfig(wxConfigBase *config)
//------------------------------------------------------------------------------
/**
 * Creates the configuration object that provides layout data
 *
 */
//------------------------------------------------------------------------------
bool GmatBaseSetupPanel::GetLayoutConfig(GmatBase *theObject, wxFileConfig **config)
{
   #ifdef DEBUG_LOAD_LAYOUT
   MessageInterface::ShowMessage("GmatBaseSetupPanel::GetLayoutConfig() entered\n");
   #endif
   std::string configPath;
   // get the GUI_CONFIG_PATH
   FileManager *fm = FileManager::Instance();
   try
   {
      configPath = fm->GetAbsPathname(FileManager::GUI_CONFIG_PATH);
   }
   catch (UtilityException &e)
   {
      MessageInterface::ShowMessage
         ("GmatBaseSetupPanel:Create() error occurred!\n%s\n",
          e.GetFullMessage().c_str());
      configPath = "";
   }
   
   std::string configFileName = configPath+theObject->GetTypeName()+".ini";
   bool configFileExists = GmatFileUtil::DoesFileExist(configFileName);
   
   // Do we need to show this message?
   // Commented out for now (LOJ: 2011.09.09)
   #ifdef DEBUG_LOAD_LAYOUT
   static bool loadMessageWritten = false;
   if (!loadMessageWritten)
   {
      if (configFileExists)
         MessageInterface::ShowMessage
            ("GmatBaseSetupPanel:Attempting to load layout from file: %s\n",
             configFileName.c_str());
      else
         MessageInterface::ShowMessage
            ("GmatBaseSetupPanel:Unable to find layout file: %s\n",
             configFileName.c_str());
      loadMessageWritten = true;
   }
   #endif
   
   *config = new wxFileConfig(wxEmptyString, wxEmptyString,
                              (configFileName).c_str(),
                              wxEmptyString, wxCONFIG_USE_LOCAL_FILE |
                              wxCONFIG_USE_RELATIVE_PATH );
   #ifdef DEBUG_LOAD_LAYOUT
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::GetLayoutConfig() returning %d\n", configFileExists);
   #endif
   return configFileExists;
}


//------------------------------------------------------------------------------
// wxString GetParameterLabel(Integer i, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Creates a label for a parameter text.  It uses INI (if available) or
 * just a TitleCase string representation of the parameter text
 *
 * @param text input text
 */
//------------------------------------------------------------------------------
std::string GmatBaseSetupPanel::GetParameterLabel(GmatBase *theObject, Integer i,
                                                  wxFileConfig *config) const
{
   std::string text = theObject->GetParameterText(i);
   std::string titleText;
   wxString str;
   
   // first, see if the parameter is in the object's INI file
   // set the path to the section that contains the parameter's items
   config->SetPath(wxString(("/"+text).c_str()));
   if (config->Read("Label", &str))
   {
      titleText = str.c_str();
      return titleText;
   }
   
   bool findword = false;
   if (text.length() > 0)
      titleText = text[0];
   for (unsigned int i=1;i<text.length();i++)
   {
      
      if (!findword)
      {
         // if not finding a word and is not uppercase, find the next word
         // e.g., NASATextFile -> NASAT (find word (next upper case char))
         //       Text File -> T (Find word (next upper case char))
         if (!((isupper(text[i])) || (isdigit(text[i]))))
            findword = true;
      }
      else
      {
         if ((isupper(text[i])) || (isdigit(text[i])))
         {
            findword = false;
            titleText += " ";
         }
      }
      titleText += text[i];
   }
   return titleText;
}


//------------------------------------------------------------------------------
// wxString GetParameterUnit(Integer i, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Creates a label for a parameter text.  It uses INI (if available) or
 * just a TitleCase string representation of the parameter text
 *
 * @param text input text
 */
//------------------------------------------------------------------------------
std::string GmatBaseSetupPanel::GetParameterUnit(GmatBase *theObject, Integer i,
                                                 wxFileConfig *config) const
{
   std::string text = theObject->GetParameterText(i);
   wxString str;
   
   // first, see if the parameter is in the object's INI file
   // set the path to the section that contains the parameter's items
   config->SetPath(wxString(("/"+text).c_str()));
   if (config->Read("Unit", &str))
      return std::string(str.c_str());
   else
      return theObject->GetParameterUnit(i);
}


//------------------------------------------------------------------------------
// void OnBrowseButton(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for browse button click
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::OnBrowseButton(wxCommandEvent& event)
{
   wxBitmapButton *bb = (wxBitmapButton *) event.GetEventObject();
   wxTextCtrl *control = (wxTextCtrl *) bb->GetPrevSibling();
   wxString oldname = control->GetValue();
   std::string wildCard = "All Files (*.*)|*.*";
   if (idInfoMap.find((wxObject *) bb) != idInfoMap.end())
	 wildCard = idInfoMap.find((wxObject *) bb)->second;
   wxFileDialog dialog(this, _T("Choose a file"), _T(""), _T(""), 
	   wildCard);
   
   if (dialog.ShowModal() == wxID_OK)
   {
      wxString filename;
      
      filename = dialog.GetPath().c_str();
      
      if (!filename.IsSameAs(oldname))
      {
         control->SetValue(filename);
			control->SetInsertionPointEnd();
         EnableUpdate(true);
      }
   }
}


//------------------------------------------------------------------------------
// void OnCheckBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for comboboxes
 *
 * Activates the Apply button when selection is changed.
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::OnCheckBoxChange(wxCommandEvent& event)
{
   // We don't want save every text change here so removed (LOJ: 2011.06.02)
   // The change will be save when an user hits apply button
   #if 0
   std::string label = localObject->GetParameterText(inverseControlMap[(wxControl *) event.GetEventObject()]);
   try
   {
      if (SaveControl(localObject, label))
         RefreshProperties(localObject);
   }
   catch (BaseException &)
   {
   }
   #endif
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnComboBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for comboboxes
 *
 * Activates the Apply button when selection is changed.
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::OnComboBoxChange(wxCommandEvent& event)
{
   // We don't want save every text change here so removed (LOJ: 2011.06.02)
   // The change will be save when an user hits apply button
	// Actually in some cases we need to reload the value upon ComboBox selection change.
	// For example, when ComboBox selection changed from Earth to Mars for GroundTrackPlot,
	// we also want to change the texture map file to Mars. So changed back (LOJ: 2011.11.30)

   #ifdef DEBUG_COMBOBOX
   MessageInterface::ShowMessage
      ("\nGmatBaseSetupPanel::OnComboBoxChange() entered, reloadOnComboBoxChange = %s\n",
       reloadOnComboBoxChange ? "true" : "false");
   #endif
   
   if (reloadOnComboBoxChange)
	{
		std::string label = localObject->GetParameterText(inverseControlMap[(wxControl *) event.GetEventObject()]);
      Integer id = localObject->GetParameterID(label);
      #ifdef DEBUG_COMBOBOX
      MessageInterface::ShowMessage("   label = '%s', id = %d\n", label.c_str(), id);
      #endif
		try
		{
         // Special handling for GroundTrackPlot since we don't want to create
         // customized GroundTrackPlot panel. We need to get new texture map file
         // from the base when new central body is saved. (LOJ: 2012.09.17)
         if (localObject->IsOfType("GroundTrackPlot") && label == "CentralBody")
         {
            #ifdef DEBUG_COMBOBOX
            MessageInterface::ShowMessage
               ("   Saving GroundTrackPlot CentralBody and gettng new texture file.\n");
            #endif
            
            // Save current control
            if (SaveControl(localObject, label))
               RefreshProperties(localObject, "", "TextureMap");
         }
		}
		catch (BaseException &be)
		{
         MessageInterface::ShowMessage(be.GetFullMessage());
		}
   }
   
   EnableUpdate(true);
   
   #ifdef DEBUG_COMBOBOX
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::OnComboBoxChange() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void OnComboBoxTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for ComboBox text change
 *
 * Activates the Apply button when selection is changed.
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::OnComboBoxTextChange(wxCommandEvent& event)
{
   // We don't want save every text change here so removed (LOJ: 2011.06.02)
   // The change will be save when an user hits apply button
   #if 0
   std::string label = localObject->GetParameterText(inverseControlMap[(wxControl *) event.GetEventObject()]);
   try
   {
      if (SaveControl(localObject, label))
         RefreshProperties(localObject, label);
   }
   catch (BaseException &)
   {
   }
   #endif
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnTextChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for text boxes
 *
 * Activates the Apply button when text is changed.
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::OnTextChange(wxCommandEvent& event)
{
   // We don't want save every text change here so removed (LOJ: 2011.06.02)
   // The change will be save when an user hits apply button
   #if 0
   std::string label = localObject->GetParameterText(inverseControlMap[(wxControl *) event.GetEventObject()]);
   try
   {
      if (SaveControl(localObject, label))
         RefreshProperties(localObject, label);
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage
         ("GmatBaseSetupPanel::OnTextChange() %s\n", be.GetFullMessage().c_str());
   }
   #endif
   
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void OnCheckListBoxChange(wxCommandEvent& event)
//------------------------------------------------------------------------------
/**
 * Event handler for wxCheckListBox
 *
 * Activates the Apply button when ListBox is changed.
 *
 * @param event The triggering event.
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::OnCheckListBoxChange(wxCommandEvent& event)
{
   EnableUpdate(true);
}


//------------------------------------------------------------------------------
// void SortProperties(StringArray *propertyNames, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Sort properties based on INI PositionBefore statements
 *
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::SortProperties(StringArray *propertyNames, wxFileConfig *config)
{
   // first, see if the INI file wants properties alphabetically sorted
   wxString alpha_sort;
   if (config->Read("/Main/Sort Properties", &alpha_sort))
   {
      if (alpha_sort.Lower() == "true")
         std::sort(propertyNames->begin(), propertyNames->end());
   }

   // Now, go through all the properties and order them according to "Position before"
   wxString position;
   StringArray::iterator item;
   StringArray::iterator beforeItem;
   StringArray origOrder = *propertyNames;

   for (unsigned int i = 0; i < origOrder.size(); i++)
   {
      // see if position defined
      if (config->Read(("/"+origOrder[i]+"/Position Before").c_str(), &position))
      {
         //MessageInterface::ShowMessage
         //   ("Sorting %s before %s\n", origOrder[i].c_str(), position.c_str());
         // find the property to move before
         if (position != "")
         {
            beforeItem = find(propertyNames->begin(), propertyNames->end(), position.c_str());
            if (beforeItem == propertyNames->end()) continue;
         }
         // find the property to move
         item = find(propertyNames->begin(), propertyNames->end(), origOrder[i].c_str());
         propertyNames->erase(item);
         // if no position before specified, move to the end
         if (position == "")
            propertyNames->push_back(origOrder[i]);
         else
         {
            // beforeItem is now wrong because of the erase...find it again
             beforeItem = find(propertyNames->begin(), propertyNames->end(), position.c_str());
             propertyNames->insert(beforeItem, origOrder[i]);
         }
      }
   }
}


//------------------------------------------------------------------------------
// void SortGroups(StringArray *groupNames, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Sort groups based on INI PositionBefore statements
 *
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::SortGroups(StringArray *groupNames, wxFileConfig *config)
{
   // go through all the groups and order them according to "Position before"
   wxString position;
   StringArray::iterator item;
   StringArray::iterator beforeItem;
   StringArray origOrder = *groupNames;

   for (unsigned int i = 0; i < origOrder.size(); i++)
   {
      // see if position defined
      if (config->Read(("/"+origOrder[i]+"/Position Before").c_str(), &position))
      {
         // find the group to move before
         if (position != "")
         {
            beforeItem = find(groupNames->begin(), groupNames->end(), position.Lower().c_str());
            if (beforeItem == groupNames->end()) continue;
         }
         // find the group to move
         item = find(groupNames->begin(), groupNames->end(), origOrder[i].c_str());
         groupNames->erase(item);
         // if no position before specified, move to the end
         if (position == "")
            groupNames->push_back(origOrder[i]);
         else
         {
            // beforeItem is now wrong because of the erase...find it again
             beforeItem = find(groupNames->begin(), groupNames->end(), position.Lower().c_str());
             groupNames->insert(beforeItem, origOrder[i]);
         }
      }
   }
}


//------------------------------------------------------------------------------
// void NormalizeLabels( StringArray propertyNames, std::vector<wxString> propertyGroups,
//                       std::vector<wxStaticText*> propertyDescriptors,
//                       std::vector<wxControl*> propertyControls,
//                       std::vector<wxStaticText*> propertyUnits )
//------------------------------------------------------------------------------
/**
 * Make all labels in each group the same size
 *
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::NormalizeLabels( StringArray propertyNames,
                      std::vector<wxString> propertyGroups,
                      std::vector<wxStaticText*> propertyDescriptors,
                      std::vector<wxControl*> propertyControls,
                      std::vector<wxControl*> propertyUnits )
{
   #ifdef DEBUG_CONTROL
   MessageInterface::ShowMessage("GmatBaseSetupPanel::NormalizeLabels() entered\n");
   #endif
   
   // initialize all groups with a max size
   SizerSizeType labelSizes, unitSizes;
   SizerSizeType::iterator sizeItem;
   std::vector<wxStaticText*>::iterator item;
   unsigned int j;

   // initialize the sizes first
   for (j = 0; j < propertyGroups.size(); j++)
   {
      if (labelSizes.find(propertyGroups[j].Lower()) == labelSizes.end())
      {
         labelSizes.insert( std::make_pair(propertyGroups[j].Lower(), 0) );
         unitSizes.insert( std::make_pair(propertyGroups[j].Lower(), 0) );
      }
   }

   // now find the minimums for the descriptions and units
   for(item = propertyDescriptors.begin(), j = 0;
       item != propertyDescriptors.end(); ++item, ++j)
   {
      // get the label size so far for the group
      sizeItem = labelSizes.find(propertyGroups[j].Lower());
      if (((int) sizeItem->second) < (*item)->GetBestSize().GetWidth())
         sizeItem->second = (*item)->GetBestSize().GetWidth();
      // get the unit size so far for the group
      sizeItem = unitSizes.find(propertyGroups[j].Lower());
      if (((int) sizeItem->second) < propertyUnits[j]->GetBestSize().GetWidth())
         sizeItem->second = propertyUnits[j]->GetBestSize().GetWidth();
   }

   // adjust grouplabels and groupunits
   for(item = propertyDescriptors.begin(), j = 0;
       item != propertyDescriptors.end(); ++item, ++j)
   {
      // set the label min size
      sizeItem = labelSizes.find(propertyGroups[j].Lower());
      (*item)->SetMinSize(wxSize(((int) sizeItem->second), (*item)->GetMinHeight()));
      // set the unit min size
      sizeItem = unitSizes.find(propertyGroups[j].Lower());
      propertyUnits[j]->SetMinSize(wxSize(((int) sizeItem->second), propertyUnits[j]->GetMinHeight()));
   }
   
   #ifdef DEBUG_CONTROL
   MessageInterface::ShowMessage("==> GmatBaseSetupPanel::NormalizeLabels() leaving\n");
   #endif
}

//------------------------------------------------------------------------------
// void FixTabOrder( wxWindow *lastControl, wxSizer *sizer )
//------------------------------------------------------------------------------
/**
 * Fixes the tab order so that tab button follows the visual order of the controls
 *
 *
 * @param lastControl the control that is BEFORE in the tab order for every control
 *                   in the sizer.  lastControl is NULL if no control is before any
 *                   of these controls
 * @param sizer the sizer that contains the controls to fix tab order for
 */
//------------------------------------------------------------------------------
wxWindow *GmatBaseSetupPanel::FixTabOrder( wxWindow *lastControl, wxSizer *sizer )
{
   wxSizerItemList list = sizer->GetChildren();
   wxSizerItemList::iterator iter;
   for (iter = list.begin(); iter != list.end(); ++iter)
   {
      if ((*iter)->IsSizer())
         lastControl = FixTabOrder( lastControl, (*iter)->GetSizer() );
      else
      {
         if (lastControl != NULL)
            (*iter)->GetWindow()->MoveAfterInTabOrder(lastControl);
         lastControl = (*iter)->GetWindow();
      }
   }
   return lastControl;

}


//------------------------------------------------------------------------------
// RefreshProperties(GmatBase *theObject, const std::string &ignoreControl)
//------------------------------------------------------------------------------
/**
 * Refreshes the Parameters/Properties controls (label, control, and unit) by
 * re-inspecting the object
 *
 *
 * @param theObject the object to re-inspect
 * @param ignoreControl if not "", then ignore the controls for that parameter (user
 *                      is probably currently typing in them
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::RefreshProperties(GmatBase *theObject, const std::string &ignoreControl,
                                           const std::string &onlyOneControl)
{
   #ifdef DEBUG_REFRESH
   MessageInterface::ShowMessage
      ("\nGmatBaseSetupPanel::RefreshProperties() entered. Ignore=%s, RefreshOnlyOne=%s\n",
       ignoreControl.c_str(), onlyOneControl.c_str());
   #endif
   
   wxFileConfig *pConfig;
   GetLayoutConfig( theObject, &pConfig );
   
   if (onlyOneControl == "")
   {
      // iterate through the map
      for (std::map<Integer, wxControl *>::iterator i = controlMap.begin();
           i != controlMap.end(); ++i)
      {
         if ((ignoreControl == "") || (theObject->GetParameterText(i->first) != ignoreControl))
            RefreshProperty(theObject, i->first, i->second, pConfig);
      }
   }
   else
   {
      // iterate through the map and save only one control specified
      for (std::map<Integer, wxControl *>::iterator i = controlMap.begin();
           i != controlMap.end(); ++i)
      {
         if ((theObject->GetParameterText(i->first) == onlyOneControl))
         {
            RefreshProperty(theObject, i->first, i->second, pConfig);
            break;
         }
      }
   }
   
   #ifdef DEBUG_REFRESH
   MessageInterface::ShowMessage("GmatBaseSetupPanel::RefreshProperties() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// RefreshProperty(GmatBase *theObject, Integer index, wxControl *control, wxFileConfig *config)
//------------------------------------------------------------------------------
/**
 * Refreshes the Parameter/Property controls (label, control, and unit) for one parameter
 *
 *
 * @param theObject the object to re-inspect
 * @param index the index of the parameter in the object
 * @param control the edit control to refresh
 */
//------------------------------------------------------------------------------
void GmatBaseSetupPanel::RefreshProperty(GmatBase *theObject, Integer index, wxControl *control,
                                         wxFileConfig *config)
{
   #ifdef DEBUG_REFRESH_PROPERTY
   MessageInterface::ShowMessage
      ("GmatBaseSetupPanel::RefreshProperty() entered, index=%d\n", index);
   #endif
   
   wxControl *aSibling;
   
   // refresh the label, the value, and the unit
   std::string labelText = theObject->GetParameterText(index);
   
   #ifdef DEBUG_REFRESH_PROPERTY
   MessageInterface::ShowMessage("   labelText = '%s'\n", labelText.c_str());
   #endif
   
   // set the path to the section that contains the parameter's items
   config->SetPath(wxString(("/"+labelText).c_str()));
   
   // refresh value
   LoadControl(theObject, labelText);
   control->Enable(theObject->IsParameterEnabled(index));
   
   // refresh label
   if (theObject->GetParameterType(index) != Gmat::BOOLEAN_TYPE)
      aSibling = (wxControl *) control->GetPrevSibling();
   else
      aSibling = control;
   
   labelText = GetParameterLabel(theObject, index, config);
   if ((aSibling->GetLabelText()).c_str() != labelText)
   {
      labelText = AssignAcceleratorKey(labelText);
      aSibling->SetLabel(labelText.c_str());
   }
   
   // refresh units
   control->GetNextSibling()->SetLabel(wxString(GetParameterUnit(theObject, index, config).c_str()));
   
   #ifdef DEBUG_REFRESH_PROPERTY
   MessageInterface::ShowMessage("GmatBaseSetupPanel::RefreshProperty() leaving\n");
   #endif
}

