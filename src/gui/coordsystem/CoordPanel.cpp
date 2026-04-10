//$Id$
//------------------------------------------------------------------------------
//                              CoordPanel
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
// Created: 2005/03/11
/**
 * This class contains the Coordinate System Panel for CoordSystemConfigPanel
 * and CoordSysCreateDialog.
 */
//------------------------------------------------------------------------------
#include "CoordPanel.hpp"
#include "AxisSystem.hpp"
#include "SpacePoint.hpp"
#include "TimeSystemConverter.hpp"  // for Convert()
#include "StringUtil.hpp"           // for ToReal()
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "GmatStaticBoxSizer.hpp"
#include <wx/config.h>
#include <sstream>

//#define DEBUG_COORD_PANEL
//#define DEBUG_COORD_PANEL_CREATE
//#define DEBUG_COORD_PANEL_LOAD
//#define DEBUG_COORD_PANEL_SAVE
//#define DEBUG_COORD_EPOCH
//#define DEBUG_COORD_PANEL_PRIMARY_SECONDARY
//#define DEBUG_COORD_PANEL_XYZ

//------------------------------------------------------------------------------
// CoordPanel()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
CoordPanel::CoordPanel(wxWindow *parent, bool enableAll)
   : wxPanel(parent)
{
   theGuiInterpreter = GmatAppData::Instance()->GetGuiInterpreter();
   theGuiManager = GuiItemManager::GetInstance();
   
   mShowPrimaryBody         = false;
   mShowSecondaryBody       = false;
   mShowEpoch               = false;
   mShowXyz                 = false;
   mShowUpdate              = false;   // currently, nutation update interval is not shown
   mShowAlignmentConstraint = false;
   
   mEnableAll               = enableAll;
   setThoughDisabled        = false;
   
   Create();
   LoadData();
}


//------------------------------------------------------------------------------
// ~CoordPanel()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
CoordPanel::~CoordPanel()
{
   // Unregister GUI components
   theGuiManager->UnregisterComboBox("SpacePoint", originComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", primaryComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", secondaryComboBox);
   theGuiManager->UnregisterComboBox("SpacePoint", refObjectComboBox);
   theGuiManager->UnregisterComboBox("CoordinateSystem", constraintCSComboBox);
}


//------------------------------------------------------------------------------
// void EnableOptions(AxisSystem *axis)
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
void CoordPanel::EnableOptions(AxisSystem *axis)
{
   #ifdef DEBUG_COORD_PANEL
   MessageInterface::ShowMessage
      ("CoordPanel::EnableOptions() ENTERED ----------------------\n"
       "axis=(%p)%s\n", axis, typeComboBox->GetStringSelection().c_str());
   #endif
   
   // save epoch value locally
   epochValue = epochTextCtrl->GetValue();
   
   wxString typeStr = typeComboBox->GetStringSelection();
   AxisSystem* tmpAxis = NULL;
   
   if (typeStr == "")
      typeStr = "MJ2000Eq";
   #ifdef DEBUG_COORD_PANEL
   MessageInterface::ShowMessage("typeStr=%s\n", typeStr.c_str());
   #endif

   if (axis == NULL)
      // create a temp axis to use flags
      tmpAxis = (AxisSystem *)theGuiInterpreter->
         CreateObject(typeStr.c_str(), ""); // Use no name
   else
      tmpAxis = axis;
   
   if (tmpAxis == NULL)
   {
      #ifdef DEBUG_COORD_PANEL
      MessageInterface::ShowMessage
         ("CoordPanel::EnableOptions() EXITING ---------------------- because tmpAxis is NULL!!!!!!\n");
      #endif
      return;
   }
   #ifdef DEBUG_COORD_PANEL
   MessageInterface::ShowMessage("tmpAxis = <%p>\n", tmpAxis);
   MessageInterface::ShowMessage("mEnableAll = %s\n", (mEnableAll? "true" : "false"));
   #endif
   
   if ((tmpAxis->UsesPrimary() == GmatCoordinate::NOT_USED) ||
       (tmpAxis->UsesPrimary() == GmatCoordinate::REQUIRED_UNMODIFIABLE))
      mShowPrimaryBody = false;
   else
      mShowPrimaryBody = true; 
   
   if ((tmpAxis->UsesSecondary() == GmatCoordinate::NOT_USED) ||
       (tmpAxis->UsesSecondary() == GmatCoordinate::REQUIRED_UNMODIFIABLE))
      mShowSecondaryBody = false;
   else
      mShowSecondaryBody = true; 
   
   if (tmpAxis->UsesReferenceObject() == GmatCoordinate::NOT_USED)
      mShowAlignmentConstraint    = false;
   else
      mShowAlignmentConstraint    = true;

   if (tmpAxis->UsesEpoch() == GmatCoordinate::NOT_USED)
      mShowEpoch = false;
   else
   {
      mShowEpoch = true; 
      
      // get the epoch format and value from tmpAxis
      Real epoch = tmpAxis->GetEpoch().Get();
      epochValue = theGuiManager->ToWxString(epoch);

      #ifdef DEBUG_COORD_PANEL
      MessageInterface::ShowMessage
         ("About to set epoch value to %12.10f (string = %s)\n", epoch, epochValue.c_str());
      #endif
      // set the text ctrl
      epochTextCtrl->SetValue(epochValue);
   }
   #ifdef DEBUG_COORD_EPOCH
      MessageInterface::ShowMessage("mShowEpoch = %s\n", (mShowEpoch? "true" : "false"));
   #endif
   
   if ((tmpAxis->UsesXAxis() == GmatCoordinate::NOT_USED) &&
       (tmpAxis->UsesYAxis() == GmatCoordinate::NOT_USED) &&
       (tmpAxis->UsesZAxis() == GmatCoordinate::NOT_USED))
      mShowXyz = false;
   else
      mShowXyz = true; 
   
   if (tmpAxis->UsesNutationUpdateInterval() == GmatCoordinate::NOT_USED)
      mShowUpdate = false;
   else
      mShowUpdate = true; 
   
   #ifdef DEBUG_COORD_PANEL
   MessageInterface::ShowMessage
      ("Calling SetDefaultEpochRefAxis() or SetDefaultAlignmentConstraintAxis()\n");
   #endif
   
   if (typeStr == "ObjectReferenced")
      SetDefaultObjectRefAxis();
   else if ((typeStr == "TOEEq") || (typeStr == "TOEEc"))
      SetDefaultEpochRefAxis();
   else if ((typeStr == "TODEq") || (typeStr == "TODEc"))
      SetDefaultEpochRefAxis();
   else if((typeStr == "MOEEq") || (typeStr == "MOEEc"))
      SetDefaultEpochRefAxis();
   else if (typeStr == "LocalAlignedConstrained")
      SetDefaultAlignmentConstraintAxis();
   
   #ifdef DEBUG_COORD_PANEL
   MessageInterface::ShowMessage
      ("mShowAlignmentConstraint = %d, mShowXyz = %d\n",mShowAlignmentConstraint, mShowXyz);
   #endif
   
   if (mEnableAll)
   {

      if (mShowAlignmentConstraint)
      {
         // Hide and disable primary, secondary, epoch, X, Y, Z
//         primaryStaticText->Show(false);
//         primaryComboBox->Show(false);
//         secondaryStaticText->Show(false);
//         secondaryComboBox->Show(false);
//         epochStaticText->Show(false);
//         epochTextCtrl->Show(false);
//         xStaticText->Show(false);
//         xComboBox->Show(false);
//         yStaticText->Show(false);
//         yComboBox->Show(false);
//         zStaticText->Show(false);
//         zComboBox->Show(false);

         flexgridsizer1->Show(false);
         boxsizer2->Show(false);

         primaryStaticText->Enable(false);
         primaryComboBox->Enable(false);
         secondaryStaticText->Enable(false);
         secondaryComboBox->Enable(false);
         epochStaticText->Enable(false);
         epochTextCtrl->Enable(false);
         xStaticText->Enable(false);
         xComboBox->Enable(false);
         yStaticText->Enable(false);
         yComboBox->Enable(false);
         zStaticText->Enable(false);
         zComboBox->Enable(false);

         // Show and enable the alignment and constraint widgets
         EnableAlignmentConstraint(true);
      }
      else
      {
         // Show and enable primary, secondary, epoch, X, Y, Z
//         primaryStaticText->Show(true);
//         primaryComboBox->Show(true);
//         secondaryStaticText->Show(true);
//         secondaryComboBox->Show(true);
//         epochStaticText->Show(true);
//         epochTextCtrl->Show(true);
//         xStaticText->Show(true);
//         xComboBox->Show(true);
//         yStaticText->Show(true);
//         yComboBox->Show(true);
//         zStaticText->Show(true);
//         zComboBox->Show(true);

         flexgridsizer1->Show(true);
         boxsizer2->Show(true);

         primaryStaticText->Enable(mShowPrimaryBody);
         primaryComboBox->Enable(mShowPrimaryBody);
         secondaryStaticText->Enable(mShowSecondaryBody);
         secondaryComboBox->Enable(mShowSecondaryBody);
         epochStaticText->Enable(mShowEpoch);
         #ifdef DEBUG_COORD_PANEL
         MessageInterface::ShowMessage("Calling epochTextCtrl->Enable(mShowEpoch)\n");
         #endif
         epochTextCtrl->Enable(mShowEpoch);

         // Mod by LOJ (2014.08.01)
         // epochTextCtrl->Clear() causes crash on static link build so
         // changed epochTextCtrl->Clear() to epochTextCtrl->ChangeValue().
         // wxTextCtrl::Clear() Clears the text in the control.
         // Note that this function WILL generate a wxEVT_COMMAND_TEXT_UPDATED event.
         // if (!mShowEpoch)
         //    epochTextCtrl->Clear();
         // wxTextCtrl::SetValue() is deprecated and should not be used in new code.
         // Please use the ChangeValue function instead.
         // wxTextCtrl::ChangeValue() Sets the text value and marks the control as
         // not-modified (which means that IsModified would return false immediately
         // after the call to ChangeValue).
         // Note that this function WILL NOT generate the wxEVT_COMMAND_TEXT_UPDATED event.
         // This is the only difference with SetValue.
         if (!mShowEpoch)
            epochTextCtrl->ChangeValue("");
         #ifdef DEBUG_COORD_PANEL
         MessageInterface::ShowMessage("Calling xStaticText->Enable(mShowXyz)\n");
         #endif
         xStaticText->Enable(mShowXyz);
         xComboBox->Enable(mShowXyz);
         yStaticText->Enable(mShowXyz);
         yComboBox->Enable(mShowXyz);
         zStaticText->Enable(mShowXyz);
         zComboBox->Enable(mShowXyz);
         
         #ifdef DEBUG_COORD_PANEL
         MessageInterface::ShowMessage("Calling EnableAlignmentConstraint(false)\n");
         #endif
         // Hide and disable the alignment and constraint widgets
         EnableAlignmentConstraint(false);
      }
      
      #ifdef DEBUG_COORD_PANEL
      MessageInterface::ShowMessage("Disabling some items for GSE, GSM, BodySpinSun\n");
      #endif
      
      // disable some items
      if (typeStr == "GSE" || typeStr == "GSM")
      {
         primaryComboBox->SetStringSelection("Earth");
         secondaryComboBox->SetStringSelection("Sun");
         
         primaryStaticText->Enable(false);
         primaryComboBox->Enable(false);
         secondaryStaticText->Enable(false);
         secondaryComboBox->Enable(false);
         setThoughDisabled = true;
      }
      else if (typeStr == "BodySpinSun")
      {
         primaryComboBox->SetStringSelection("Sun");
         secondaryComboBox->SetStringSelection(originComboBox->GetValue());

         primaryStaticText->Enable(false);
         primaryComboBox->Enable(false);
         secondaryStaticText->Enable(false);
         secondaryComboBox->Enable(false);
         setThoughDisabled = true;
      }
      else
      {
         setThoughDisabled = false;
      }
   }
   else  // disable all of them
   {
      // Disable primary, secondary, epoch, X, Y, Z
      originStaticText->Enable(false);
      originComboBox->Enable(false);
      typeStaticText->Enable(false);
      typeComboBox->Enable(false);
      primaryStaticText->Enable(false);
      primaryComboBox->Enable(false);
      secondaryStaticText->Enable(false);
      secondaryComboBox->Enable(false);
      epochStaticText->Enable(false);
      epochTextCtrl->Enable(false);

      xStaticText->Enable(false);
      xComboBox->Enable(false);
      yStaticText->Enable(false);
      yComboBox->Enable(false);
      zStaticText->Enable(false);
      zComboBox->Enable(false);

      // Hide and disable the alignment and constraint widgets
      EnableAlignmentConstraint(false);
   }
   primaryStaticText->SetFocus();
   staticboxsizerAxes->Layout();
   Refresh();
   #ifdef DEBUG_COORD_PANEL
   MessageInterface::ShowMessage
      ("CoordPanel::EnableOptions() EXITING ----------------------\n");
   #endif
}

bool CoordPanel::IsAlignmentConstraintTextModified()
{
   if (alignXTextCtrl->IsModified() || alignYTextCtrl->IsModified() ||
       alignZTextCtrl->IsModified())
      return true;
   if (constraintXTextCtrl->IsModified() || constraintYTextCtrl->IsModified() ||
       constraintZTextCtrl->IsModified())
      return true;
   if (constraintRefXTextCtrl->IsModified() || constraintRefYTextCtrl->IsModified() ||
       constraintRefZTextCtrl->IsModified())
      return true;
   return false;
}

bool CoordPanel::CheckAlignmentConstraintValues(bool setOnAxis, AxisSystem *theAxis)
{

   if (setOnAxis && (theAxis == NULL))
      return false;

   std::string str = "";
   Real x, y, z;
   bool isValid = true;
   // Set the alignment vector
   if (alignXTextCtrl->IsEnabled() && alignYTextCtrl->IsEnabled() &&
       alignZTextCtrl->IsEnabled())
   {
      str = alignXTextCtrl->GetValue().c_str();
      isValid = isValid && CheckReal(x, str, "AlignmentVectorX", "Real Number");
      str = alignYTextCtrl->GetValue().c_str();
      isValid = isValid && CheckReal(y, str, "AlignmentVectorY", "Real Number");
      str = alignZTextCtrl->GetValue().c_str();
      isValid = isValid && CheckReal(z, str, "AlignmentVectorZ", "Real Number");
      if (isValid && setOnAxis)
      {
         theAxis->SetRealParameter("AlignmentVectorX", x);
         theAxis->SetRealParameter("AlignmentVectorY", y);
         theAxis->SetRealParameter("AlignmentVectorZ", z);
      }
   }
   if (constraintXTextCtrl->IsEnabled() && constraintYTextCtrl->IsEnabled() &&
       constraintZTextCtrl->IsEnabled())
   {
      str = constraintXTextCtrl->GetValue().c_str();
      isValid = isValid && CheckReal(x, str, "ConstraintVectorX", "Real Number");
      str = constraintYTextCtrl->GetValue().c_str();
      isValid = isValid && CheckReal(y, str, "ConstraintVectorY", "Real Number");
      str = constraintZTextCtrl->GetValue().c_str();
      isValid = isValid && CheckReal(z, str, "ConstraintVectorZ", "Real Number");
      if (isValid && setOnAxis)
      {
         theAxis->SetRealParameter("ConstraintVectorX", x);
         theAxis->SetRealParameter("ConstraintVectorY", y);
         theAxis->SetRealParameter("ConstraintVectorZ", z);
      }
   }
   if (constraintRefXTextCtrl->IsEnabled() && constraintRefYTextCtrl->IsEnabled() &&
       constraintRefZTextCtrl->IsEnabled())
   {
      str = constraintRefXTextCtrl->GetValue().c_str();
      isValid = isValid && CheckReal(x, str, "ConstraintReferenceVectorX", "Real Number");
      str = constraintRefYTextCtrl->GetValue().c_str();
      isValid = isValid && CheckReal(y, str, "ConstraintReferenceVectorY", "Real Number");
      str = constraintRefZTextCtrl->GetValue().c_str();
      isValid = isValid && CheckReal(z, str, "ConstraintReferenceVectorZ", "Real Number");
      if (isValid && setOnAxis)
      {
         theAxis->SetRealParameter("ConstraintReferenceVectorX", x);
         theAxis->SetRealParameter("ConstraintReferenceVectorY", y);
         theAxis->SetRealParameter("ConstraintReferenceVectorZ", z);
      }
   }
   return isValid;
}


//------------------------------------------------------------------------------
// void SetDefaultAxis()
//------------------------------------------------------------------------------
/**
 * Sets the default axis.
 */
//------------------------------------------------------------------------------
void CoordPanel::SetDefaultAxis()
{
   // default settings
   typeComboBox->SetValue("MJ2000Eq");
   originComboBox->SetValue("Earth");
   primaryComboBox->SetValue("Earth");
   secondaryComboBox->SetValue("Luna");
   epochTextCtrl->SetValue(epochValue);
   xComboBox->SetValue("R");
   yComboBox->SetValue("");;
   zComboBox->SetValue("N");
}


//------------------------------------------------------------------------------
// void SetDefaultEpochRefAxis()
//------------------------------------------------------------------------------
/**
 * Sets the default Epoch Reference axis.
 */
//------------------------------------------------------------------------------
void CoordPanel::SetDefaultEpochRefAxis()
{
   // default settings
    epochTextCtrl->SetValue(epochValue);
}


//------------------------------------------------------------------------------
// void SetDefaultObjectRefAxis()
//------------------------------------------------------------------------------
/**
 * Sets the default Object Reference axis.
 */
//------------------------------------------------------------------------------
void CoordPanel::SetDefaultObjectRefAxis()
{
   // default settings
   primaryComboBox->SetValue("Earth");
   secondaryComboBox->SetValue("Luna");
   xComboBox->SetValue("R");
   yComboBox->SetValue("");;
   zComboBox->SetValue("N");
}

//------------------------------------------------------------------------------
// void SetDefaultAlignmentConstraintAxis()
//------------------------------------------------------------------------------
/**
 * Sets the default LocalAlignedConstrained axis.
 */
//------------------------------------------------------------------------------
void CoordPanel::SetDefaultAlignmentConstraintAxis()
{
   // default settings
   refObjectComboBox->SetValue("Luna");
   constraintCSComboBox->SetValue("MJ2000Eq");
   alignXTextCtrl->SetValue(wxT("1.0"));
   alignYTextCtrl->SetValue(wxT("0.0"));
   alignZTextCtrl->SetValue(wxT("0.0"));
   constraintXTextCtrl->SetValue(wxT("0.0"));
   constraintYTextCtrl->SetValue(wxT("0.0"));
   constraintZTextCtrl->SetValue(wxT("1.0"));
   constraintRefXTextCtrl->SetValue(wxT("0.0"));
   constraintRefYTextCtrl->SetValue(wxT("0.0"));
   constraintRefZTextCtrl->SetValue(wxT("1.0"));
}


//------------------------------------------------------------------------------
// void ShowAxisData(AxisSystem *axis)
//------------------------------------------------------------------------------
/**
 * Shows the axis data.
 *
 * @param axis  axis system for which to show data
 */
//------------------------------------------------------------------------------
void CoordPanel::ShowAxisData(AxisSystem *axis)
{
   #ifdef DEBUG_COORD_PANEL
   MessageInterface::ShowMessage
      ("CoordPanel::ShowAxisData() axis=(%p)%s\n", axis,
       axis->GetTypeName().c_str());
   #endif
   
   try
   {
      int sel = typeComboBox->FindString(axis->GetTypeName().c_str());
      typeComboBox->SetSelection(sel);
      EnableOptions(axis);
      
      #ifdef DEBUG_COORD_PANEL
      MessageInterface::ShowMessage
         ("mShowPrimaryBody=%d, mShowSecondaryBody=%d, mShowEpoch=%d, "
          "mShowXyz=%d, mShowUpdate\n", mShowPrimaryBody, mShowSecondaryBody,
          mShowEpoch, mShowXyz, mShowUpdate);
      #endif
      
      if (mShowPrimaryBody)
      {
         #ifdef DEBUG_COORD_PANEL_PRIMARY_SECONDARY
            MessageInterface::ShowMessage("Primary is %s\n", (axis->GetStringParameter("Primary")).c_str());
         #endif
         primaryComboBox->
            SetStringSelection(axis->GetStringParameter("Primary").c_str());
      }
      
      if (mShowSecondaryBody)
      {
         #ifdef DEBUG_COORD_PANEL_PRIMARY_SECONDARY
            MessageInterface::ShowMessage("Secondary is %s\n", (axis->GetStringParameter("Secondary")).c_str());
         #endif
         secondaryComboBox->
            SetStringSelection(axis->GetStringParameter("Secondary").c_str());
      }
      
      if (mShowEpoch)
      {
         Real epoch = axis->GetEpoch().Get();
         #ifdef DEBUG_COORD_PANEL
            MessageInterface::ShowMessage
               ("CoordPanel::ShowAxisData() about to set the value of epoch to %12.10f\n", epoch);
         #endif
         epochTextCtrl->SetValue(theGuiManager->ToWxString(epoch));
      }
      
      if (mShowXyz)
      {
         xComboBox->SetStringSelection(axis->GetXAxis().c_str());
         yComboBox->SetStringSelection(axis->GetYAxis().c_str());
         zComboBox->SetStringSelection(axis->GetZAxis().c_str());
      }

      if (mShowAlignmentConstraint)
      {
         #ifdef DEBUG_COORD_PANEL_PRIMARY_SECONDARY
         MessageInterface::ShowMessage("Reference Object is %s\n", (axis->GetStringParameter("ReferenceObject")).c_str());
         MessageInterface::ShowMessage("Constraint CS is %s\n", (axis->GetStringParameter("ConstraintCoordinateSystem")).c_str());
         #endif
         refObjectComboBox->
            SetStringSelection(axis->GetStringParameter("ReferenceObject").c_str());
         constraintCSComboBox->
            SetStringSelection(axis->GetStringParameter("ConstraintCoordinateSystem").c_str());

         // Load alignment and constraint data here
         Real realVal;
         realVal = axis->GetRealParameter("AlignmentVectorX");
         alignXTextCtrl->SetValue(theGuiManager->ToWxString(realVal));
         realVal = axis->GetRealParameter("AlignmentVectorY");
         alignYTextCtrl->SetValue(theGuiManager->ToWxString(realVal));
         realVal = axis->GetRealParameter("AlignmentVectorZ");
         alignZTextCtrl->SetValue(theGuiManager->ToWxString(realVal));

         realVal = axis->GetRealParameter("ConstraintVectorX");
         constraintXTextCtrl->SetValue(theGuiManager->ToWxString(realVal));
         realVal = axis->GetRealParameter("ConstraintVectorY");
         constraintYTextCtrl->SetValue(theGuiManager->ToWxString(realVal));
         realVal = axis->GetRealParameter("ConstraintVectorZ");
         constraintZTextCtrl->SetValue(theGuiManager->ToWxString(realVal));

         realVal = axis->GetRealParameter("ConstraintReferenceVectorX");
         constraintRefXTextCtrl->SetValue(theGuiManager->ToWxString(realVal));
         realVal = axis->GetRealParameter("ConstraintReferenceVectorY");
         constraintRefYTextCtrl->SetValue(theGuiManager->ToWxString(realVal));
         realVal = axis->GetRealParameter("ConstraintReferenceVectorZ");
            constraintRefZTextCtrl->SetValue(theGuiManager->ToWxString(realVal));

      }

   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CoordPanel::ShowAxisData() error occurred in getting data!\n%s\n",
          e.GetFullMessage().c_str());
   }
}


//------------------------------------------------------------------------------
// AxisSystem* CreateAxis()
//------------------------------------------------------------------------------
/**
 * Creates an axis system.
 *
 * @return  newly-created axis system
 */
//------------------------------------------------------------------------------
AxisSystem* CoordPanel::CreateAxis()
{
   wxString priName  = primaryComboBox->GetValue().Trim();
   wxString secName  = secondaryComboBox->GetValue().Trim();
   wxString axisType = typeComboBox->GetValue().Trim();
   wxString epochStr = epochTextCtrl->GetValue().Trim();
   wxString xStr     = xComboBox->GetValue();
   wxString yStr     = yComboBox->GetValue();
   wxString zStr     = zComboBox->GetValue();
   wxString refName  = refObjectComboBox->GetValue().Trim();
   wxString cCSName  = constraintCSComboBox->GetValue().Trim();
   
   AxisSystem *axis = NULL;
   
   if (IsValidAxis(axisType, priName, secName, xStr, yStr, zStr, refName, cCSName))  // need all vectors too?
   {
      #ifdef DEBUG_COORD_PANEL_XYZ
         MessageInterface::ShowMessage("CoordPanel::IsValidAxis returned true\n");
      #endif
      // Create AxisSystem
      axis = (AxisSystem *)theGuiInterpreter->CreateObject(axisType.c_str(), "");

      if (axis != NULL)
      {
         try
         {
            if (axis->UsesPrimary())
            {
               SpacePoint *primary = (SpacePoint *)theGuiInterpreter->
                  GetConfiguredObject(std::string(priName.c_str()));
               axis->SetPrimaryObject(primary);
            }

            if (axis->UsesSecondary())
            {
               SpacePoint *secondary = (SpacePoint *)theGuiInterpreter->
                  GetConfiguredObject(std::string(secName.c_str()));
               axis->SetSecondaryObject(secondary);
            }

            if (axis->UsesXAxis() || axis->UsesYAxis() || axis->UsesZAxis())
            {
               // set the x, y, and z
               axis->SetXAxis(std::string(xStr.c_str()));
               axis->SetYAxis(std::string(yStr.c_str()));
               axis->SetZAxis(std::string(zStr.c_str()));
            }

            // convert epoch to a1mjd
            // if Epoch is not in A1ModJulian, convert to A1ModJulian(loj: 1/23/07)
            if (axis->UsesEpoch())
            {
               Real a1mjd;
               GmatStringUtil::ToReal(epochStr.c_str(), a1mjd);

               #ifdef DEBUG_COORD_PANEL
                  MessageInterface::ShowMessage
                     ("CoordPanel::CreateAxis() about to set the value of epoch on axis to %12.10f\n",
                           a1mjd);
               #endif
               axis->SetEpoch(a1mjd);
            }
            if (axis->UsesReferenceObject())
            {
               // Assuming that if it uses a reference object, it will also use
               // a constraint coordinate system
               SpacePoint *refObject = (SpacePoint *)theGuiInterpreter->
                  GetConfiguredObject(std::string(refName.c_str()));
               axis->SetStringParameter("ReferenceObject", refObject->GetName());
               axis->SetReferenceObject(refObject);
               CoordinateSystem *constraintCoord = (CoordinateSystem *)theGuiInterpreter->
                  GetConfiguredObject(std::string(cCSName.c_str()));
               axis->SetStringParameter("ConstraintCoordinateSystem", constraintCoord->GetName());
               axis->SetRefObject(constraintCoord, Gmat::COORDINATE_SYSTEM, constraintCoord->GetName());

               CheckAlignmentConstraintValues(true, axis);
            }
         }
         catch (BaseException &e)
         {
            MessageInterface::ShowMessage
               ("CoordPanel::CreateAxis() error occurred in setting data!\n%s\n",
                e.GetFullMessage().c_str());

            delete axis;
            axis = NULL;
         }
      }
   }
   else
   {
      #ifdef DEBUG_COORD_PANEL_XYZ
         MessageInterface::ShowMessage("CoordPanel::IsValidAxis returned false, returning NULL from CreateAxis\n");
      #endif
   }
   
   return axis;
}

//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void EnableAlignmentConstraint(bool enableAll)
//------------------------------------------------------------------------------
void CoordPanel::EnableAlignmentConstraint(bool enableAll)
{
	staticboxsizerA->Show(enableAll);
	staticboxsizerC->Show(enableAll);
   refObjectComboBox->Enable(enableAll);
   constraintCSComboBox->Enable(enableAll);
   alignXTextCtrl->Enable(enableAll);
   alignYTextCtrl->Enable(enableAll);
   alignZTextCtrl->Enable(enableAll);
   constraintXTextCtrl->Enable(enableAll);
   constraintYTextCtrl->Enable(enableAll);
   constraintZTextCtrl->Enable(enableAll);
   constraintRefXTextCtrl->Enable(enableAll);
   constraintRefYTextCtrl->Enable(enableAll);
   constraintRefZTextCtrl->Enable(enableAll);
}


//------------------------------------------------------------------------------
// bool IsValidAxis(const wxString &axisType, const wxString &priName,
//                  const wxString &secName,  const wxString &xStr,
//                  const wxString &yStr,     const wxString &zStr,
//                  const wxString &refName,  const wxString &constraintCSName)
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the axes are valid
 *
 * @param axisType         axis type
 * @param priName          primary object name
 * @param secName          secondary object name
 * @param xStr             X-axis value
 * @param yStr             Y-axis value
 * @param zStr             Z-axis value
 * @param refName          name of the reference object
 * @param constraintCSName name of the constraint coordinate system
 *
 * @return   true if valid; false otherwise
 */
//------------------------------------------------------------------------------
bool CoordPanel::IsValidAxis(const wxString &axisType, const wxString &priName,
                             const wxString &secName,  const wxString &xStr,
                             const wxString &yStr,     const wxString &zStr,
                             const wxString &refName,
                             const wxString &constraintCSName)
{
   #ifdef DEBUG_COORD_PANEL_XYZ
      MessageInterface::ShowMessage("Entering CoordPanel::IsValidAxis with X = %s, Y = %s, Z = %s\n",
            xStr.c_str(), yStr.c_str(), zStr.c_str());
   #endif
   if (axisType == "")
   {
      MessageInterface::PopupMessage(Gmat::INFO_, "Please select Axis.");
      return false;
   }
   
   if (axisType == "ObjectReferenced")
   {
      if (priName == "" && secName == "")
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "ObjectReferenced must have a primary and secondary body.");
         return false;
      }
      else if (priName == secName)
      {
         MessageInterface::PopupMessage
            (Gmat::WARNING_,
             "The primary and the secondary body must be different.");
         return false;
      }
      
      #ifdef DEBUG_COORD_PANEL_XYZ
         MessageInterface::ShowMessage("In CoordPanel::IsValidAxis, calling IsValidXYZ\n");
      #endif
      return IsValidXYZ(xStr, yStr, zStr);
   }
   else if (axisType == "LocalAlignedConstrained")
   {
      // ******* TBD ***** need to check ref object and alignment/constraint info in here
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool IsValidXYZ(const wxString &xStr, const wxString &yStr,
//                 const wxString &zStr)
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the X, Y, and Z values are valid
 *
 * @param xStr          X-axis value
 * @param yStr          Y-axis value
 * @param zStr          Z-axis value
 *
 * @return   true if valid; false otherwise
 */
//------------------------------------------------------------------------------
bool CoordPanel::IsValidXYZ(const wxString &xStr, const wxString &yStr,
                            const wxString &zStr)
{
   #ifdef DEBUG_COORD_PANEL_XYZ
      MessageInterface::ShowMessage("Entering CoordPanel::IsValidXYZ\n");
   #endif
   // Check to see if x,y,z are valid axes
   if (xStr.IsSameAs("") && (yStr.IsSameAs("") || zStr.IsSameAs("")))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_,  "Please select 2 coordinates from X, Y, and Z.");
      return false;
   }
   else if (xStr.Contains("R") && (yStr.Contains("R") || zStr.Contains("R")))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axes must be orthogonal.");
      return false;
   }
   else if (xStr.Contains("V") && (yStr.Contains("V") || zStr.Contains("V")))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axes must be orthogonal.");
      return false;
   }
   else if (xStr.Contains("N") && (yStr.Contains("N") || zStr.Contains("N")))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axes must be orthogonal.");
      return false;
   }
   
   if (yStr.Contains("R") && zStr.Contains("R"))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axes must be orthogonal.");
      return false;
   }
   else if (yStr.Contains("V") && zStr.Contains("V"))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axes must be orthogonal.");
      return false;
   }
   else if (yStr.Contains("N") && zStr.Contains("N"))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axes must be orthogonal.");
      return false;
   }
   else if (yStr.IsSameAs("") && zStr.IsSameAs(""))
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_, "The X, Y, and Z axes must be orthogonal.");
      return false;
   }
   
   // Check to make sure at least one is blank
   if (xStr.IsSameAs("") || yStr.IsSameAs("") || zStr.IsSameAs(""))
      return true;
   else
   {
      MessageInterface::PopupMessage
         (Gmat::WARNING_,  "One coordinate must be a blank string.");
      return false;
   }
}


//-------------------------------
// private methods
//-------------------------------

//------------------------------------------------------------------------------
// void Create()
//------------------------------------------------------------------------------
/**
 * Creates the panel.
 */
//------------------------------------------------------------------------------
void CoordPanel::Create()
{
   #ifdef DEBUG_COORD_PANEL_CREATE
   MessageInterface::ShowMessage("CoordPanel::Create() entered\n");
   #endif
   
   // get the config object
   wxConfigBase *pConfig = wxConfigBase::Get();
   // SetPath() understands ".."
   pConfig->SetPath(wxT("/Coordinate System"));

    // wxStaticText
   originStaticText = new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Origin",
      wxDefaultPosition, wxDefaultSize, 0 );

   //causing VC++ error => wxString emptyList[] = {};
   wxArrayString emptyList;

   // wxComboBox
   originComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(150,-1), false);
   originComboBox->SetToolTip(pConfig->Read(_T("OriginHint")));
   typeStaticText = new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Type",
      wxDefaultPosition, wxDefaultSize, 0 );
   typeComboBox = new wxComboBox
      ( this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(150,-1), //0,
        emptyList, wxCB_DROPDOWN|wxCB_READONLY );
   typeComboBox->SetToolTip(pConfig->Read(_T("TypeHint")));
   primaryStaticText = new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Primary",
      wxDefaultPosition, wxDefaultSize, 0 );
   primaryComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);
   primaryComboBox->SetToolTip(pConfig->Read(_T("PrimaryHint")));
   secondaryStaticText = new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Secondary",
      wxDefaultPosition, wxDefaultSize, 0 );
   secondaryComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);
   secondaryComboBox->SetToolTip(pConfig->Read(_T("SecondaryHint")));
//   refObjectComboBox->SetToolTip(pConfig->Read(_T("RefObjectHint")));  // TBD
   xStaticText = new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"X: ",
      wxDefaultPosition, wxDefaultSize, 0 );
   xComboBox = new wxComboBox
      ( this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(60,-1), //0,
        emptyList, wxCB_DROPDOWN|wxCB_READONLY );
   xComboBox->SetToolTip(pConfig->Read(_T("XHint")));
   yStaticText = new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Y: ",
      wxDefaultPosition, wxDefaultSize, 0 );
   yComboBox = new wxComboBox
      ( this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(60,-1), //0,
        emptyList, wxCB_DROPDOWN|wxCB_READONLY );
   yComboBox->SetToolTip(pConfig->Read(_T("YHint")));
   zStaticText = new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Z: ",
      wxDefaultPosition, wxDefaultSize, 0 );
   zComboBox = new wxComboBox
      ( this, ID_COMBO, wxT(""), wxDefaultPosition, wxSize(60,-1), //0,
        emptyList, wxCB_DROPDOWN|wxCB_READONLY );
   zComboBox->SetToolTip(pConfig->Read(_T("ZHint")));

   //wxTextCtrl
   epochStaticText = new wxStaticText( this, ID_TEXT, "A1MJD " GUI_ACCEL_KEY "Epoch",
      wxDefaultPosition, wxDefaultSize, 0 );
   epochTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
   epochTextCtrl->SetToolTip(pConfig->Read(_T("EpochHint")));

   alignXStaticText = new wxStaticText( this, ID_TEXT, "AlignmentVector" GUI_ACCEL_KEY "X",
      wxDefaultPosition, wxDefaultSize, 0 );
   alignXTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
//   alignXTextCtrl->SetToolTip(pConfig->Read(_T("AlignXHint")));
   alignYStaticText = new wxStaticText( this, ID_TEXT, "AlignmentVector" GUI_ACCEL_KEY "Y",
      wxDefaultPosition, wxDefaultSize, 0 );
   alignYTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
//   alignYTextCtrl->SetToolTip(pConfig->Read(_T("AlignYHint")));
   alignZStaticText = new wxStaticText( this, ID_TEXT, "AlignmentVector" GUI_ACCEL_KEY "Z",
      wxDefaultPosition, wxDefaultSize, 0 );
   alignZTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
//   alignZTextCtrl->SetToolTip(pConfig->Read(_T("AlignZHint")));
   refObjectStaticText = new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"ReferenceObject",
      wxDefaultPosition, wxDefaultSize, 0 );
   refObjectComboBox = theGuiManager->GetSpacePointComboBox(this, ID_COMBO,
      wxSize(120,-1), false);

   //Get CordinateSystem ComboBox from the GuiItemManager.
   constraintCSStaticText = new wxStaticText( this, ID_TEXT, GUI_ACCEL_KEY"Constraint Coord. Sys.",
      wxDefaultPosition, wxDefaultSize, 0 );
   constraintCSComboBox =
      theGuiManager->GetCoordSysComboBox(this, ID_COMBO, wxSize(120,-1));
      constraintCSComboBox->SetToolTip(pConfig->Read(_T("ConstraintCSHint")));  // TBD

   constraintXStaticText = new wxStaticText( this, ID_TEXT, "ConstraintVector" GUI_ACCEL_KEY "X",
      wxDefaultPosition, wxDefaultSize, 0 );
   constraintXTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
//   constraintXTextCtrl->SetToolTip(pConfig->Read(_T("ConstraintXHint")));
   constraintYStaticText = new wxStaticText( this, ID_TEXT, "ConstraintVector" GUI_ACCEL_KEY "Y",
      wxDefaultPosition, wxDefaultSize, 0 );
   constraintYTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
//   constraintYTextCtrl->SetToolTip(pConfig->Read(_T("ConstraintYHint")));
   constraintZStaticText = new wxStaticText( this, ID_TEXT, "ConstraintVector" GUI_ACCEL_KEY "Z",
      wxDefaultPosition, wxDefaultSize, 0 );
   constraintZTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
//   constraintZTextCtrl->SetToolTip(pConfig->Read(_T("ConstraintZHint")));
   constraintRefXStaticText = new wxStaticText( this, ID_TEXT, "Constraint Ref. Vector" GUI_ACCEL_KEY "X",
      wxDefaultPosition, wxDefaultSize, 0 );
   constraintRefXTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
//   constraintRefXTextCtrl->SetToolTip(pConfig->Read(_T("ConstraintRefXHint")));
   constraintRefYStaticText = new wxStaticText( this, ID_TEXT, "Constraint Ref. Vector" GUI_ACCEL_KEY "Y",
      wxDefaultPosition, wxDefaultSize, 0 );
   constraintRefYTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
//   constraintRefYTextCtrl->SetToolTip(pConfig->Read(_T("ConstraintRefYHint")));
   constraintRefZStaticText = new wxStaticText( this, ID_TEXT, "Constraint Ref. Vector" GUI_ACCEL_KEY "Z",
      wxDefaultPosition, wxDefaultSize, 0 );
   constraintRefZTextCtrl = new wxTextCtrl( this, ID_TEXTCTRL, wxT(""),
      wxDefaultPosition, wxSize(120,-1), 0 );
//   constraintRefZTextCtrl->SetToolTip(pConfig->Read(_T("ConstraintRefZHint")));

   // wx*Sizers
   wxBoxSizer *theMainSizer = new wxBoxSizer( wxVERTICAL );
   int bsize = 2;
   
   staticboxsizerAxes = new GmatStaticBoxSizer( wxVERTICAL, this, wxT("Axes") );
   staticboxsizerA = new GmatStaticBoxSizer( wxVERTICAL, this, wxT("Alignment Vector") );
   staticboxsizerC = new GmatStaticBoxSizer( wxVERTICAL, this, wxT("Constraint Vectors") );
   wxBoxSizer *boxsizerType = new wxBoxSizer( wxHORIZONTAL );

   flexgridsizerAlignment  = new wxFlexGridSizer( 3, 4, 0, 0 ); // for Alignment
   flexgridsizerConstraint = new wxFlexGridSizer( 4, 4, 0, 0 ); // for Constraint

   flexgridsizer1 = new wxFlexGridSizer( 2, 4, 0, 0 );  // use when not LAC

   wxBoxSizer *boxsizerOrigin = new wxBoxSizer( wxHORIZONTAL );
   boxsizer2 = new wxBoxSizer( wxHORIZONTAL );

   // Set up Origin widget
   boxsizerOrigin->Add( originStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   boxsizerOrigin->Add( originComboBox, 0, wxALIGN_CENTER|wxALL, bsize );

   // Set up Type widget
   boxsizerType->Add( typeStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   boxsizerType->Add( typeComboBox, 0, wxALIGN_CENTER|wxALL, bsize );

   // Set up the primary/secondary widgets
   flexgridsizer1->Add( primaryStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizer1->Add( primaryComboBox, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizer1->Add( secondaryStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizer1->Add( secondaryComboBox, 0, wxALIGN_LEFT|wxALL, bsize );

   // Set up the epoch widgets
   flexgridsizer1->Add( epochStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizer1->Add( epochTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );


   // Set up the X/Y/Z widgets
   boxsizer2->Add(xStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   boxsizer2->Add(xComboBox, 0, wxALIGN_CENTER|wxALL, bsize );
   boxsizer2->Add(yStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   boxsizer2->Add(yComboBox, 0, wxALIGN_CENTER|wxALL, bsize );
   boxsizer2->Add(zStaticText, 0, wxALIGN_CENTER|wxALL, bsize );
   boxsizer2->Add(zComboBox, 0, wxALIGN_CENTER|wxALL, bsize );

   // Set up the Alignment Vector widgets
   // Row 1
   flexgridsizerAlignment->Add(alignXStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerAlignment->Add(alignXTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerAlignment->Add(refObjectStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerAlignment->Add(refObjectComboBox, 0, wxALIGN_LEFT|wxALL, bsize );

   // Row 2
   flexgridsizerAlignment->Add(alignYStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerAlignment->Add(alignYTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerAlignment->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerAlignment->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, bsize );

   // Row 3
   flexgridsizerAlignment->Add(alignZStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerAlignment->Add(alignZTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerAlignment->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerAlignment->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, bsize );

   // Set up the Constraint Vectors widgets
   // Row 1
   flexgridsizerConstraint->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add( 20, 20, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintCSStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintCSComboBox, 0, wxALIGN_LEFT|wxALL, bsize );

   // Row 2
   flexgridsizerConstraint->Add(constraintXStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintXTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintRefXStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintRefXTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );

   // Row 3
   flexgridsizerConstraint->Add(constraintYStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintYTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintRefYStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintRefYTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );

   // Row 4
   flexgridsizerConstraint->Add(constraintZStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintZTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintRefZStaticText, 0, wxALIGN_LEFT|wxALL, bsize );
   flexgridsizerConstraint->Add(constraintRefZTextCtrl, 0, wxALIGN_LEFT|wxALL, bsize );

   staticboxsizerA->Add(flexgridsizerAlignment, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   staticboxsizerC->Add(flexgridsizerConstraint, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   
   staticboxsizerAxes->Add( boxsizerType, 0, wxALIGN_CENTRE|wxALL, bsize);
   staticboxsizerAxes->Add( flexgridsizer1, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize );
   staticboxsizerAxes->Add( boxsizer2, 0, wxALIGN_CENTER|wxALL, bsize );
   staticboxsizerAxes->Add( staticboxsizerA, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   staticboxsizerAxes->Add( staticboxsizerC, 0, wxGROW|wxALIGN_CENTER|wxALL, bsize);
   staticboxsizerAxes->Add( 10, 20, 0, wxGROW|wxALIGN_CENTER|wxALL, 0);
   
   // Initially hide alignment to reduce the blank space at the bottom
   staticboxsizerAxes->Hide(staticboxsizerA);
   
   theMainSizer->Add(boxsizerOrigin, 0, wxALIGN_CENTRE|wxALL, bsize);
   theMainSizer->Add(staticboxsizerAxes, 0, wxGROW|wxALIGN_CENTRE|wxALL, bsize);
   
   if (!mEnableAll)
   {
      wxStaticText *msg =
         new wxStaticText(this, ID_TEXT,
                          "This is a default Coordinate "
                          "System and cannot be modified.",
                          wxDefaultPosition, wxDefaultSize, 0);
      msg->SetForegroundColour(*wxRED);
      theMainSizer->Add(msg, 0, wxALIGN_CENTRE|wxALL, bsize);
   }
   
   this->SetAutoLayout( true );
   this->SetSizer( theMainSizer );
   theMainSizer->Fit( this );
   theMainSizer->SetSizeHints( this );
   
   #ifdef DEBUG_COORD_PANEL_CREATE
   MessageInterface::ShowMessage("CoordPanel::Create() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
/**
 * Loads the data onto the panel.
 */
//------------------------------------------------------------------------------
void CoordPanel::LoadData()
{
   #ifdef DEBUG_COORD_PANEL_LOAD
   MessageInterface::ShowMessage("CoordPanel::LoadData() entered\n");
   #endif
   
   try
   {
      // Load axes types
      StringArray itemNames =
         theGuiInterpreter->GetListOfFactoryItems(Gmat::AXIS_SYSTEM);
      for (unsigned int i = 0; i<itemNames.size(); i++)
         typeComboBox->Append(wxString(itemNames[i].c_str()));
      
      StringArray reps = TimeSystemConverter::Instance()->GetValidTimeRepresentations();

      wxString xyzStrs[] =
      {
         wxT(""),
         wxT("R"),
         wxT("-R"),
         wxT("V"),
         wxT("-V"),
         wxT("N"),
         wxT("-N"),
      };
      
      for (unsigned int i=0; i<7; i++)
      {
         xComboBox->Append(wxString(xyzStrs[i].c_str()));
         yComboBox->Append(wxString(xyzStrs[i].c_str()));
         zComboBox->Append(wxString(xyzStrs[i].c_str()));
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("CoordPanel:LoadData() error occurred!\n%s\n",
            e.GetFullMessage().c_str());
   }
   
   EnableOptions();

   #ifdef DEBUG_COORD_PANEL_LOAD
   MessageInterface::ShowMessage("CoordPanel::LoadData() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// bool SaveData(const std::string &coordName, AxisSystem *axis,
//               const wxString &epochFormat)
//------------------------------------------------------------------------------
/**
 * Saves the data from the panel to the coordinate system object.
 */
//------------------------------------------------------------------------------
bool CoordPanel::SaveData(const std::string &coordName, AxisSystem *axis,
                          wxString &epochFormat)
{
   #ifdef DEBUG_COORD_PANEL_SAVE
   MessageInterface::ShowMessage
      ("CoordPanel::SaveData() entered, coordName=%s, epochFormat=%s, epoch = %s\n",
       coordName.c_str(), epochFormat.c_str(), epochValue.c_str());
   MessageInterface::ShowMessage("   and axis <%p> of type %s\n", axis, axis->GetTypeName().c_str());
   #endif
   
   bool canClose              = true;
   CoordinateSystem *coordSys = NULL;
   CoordinateSystem *csClone  = NULL;
   
   try
   {
      std::string inputString;
      std::string msg = "The value of \"%s\" for field \"%s\" on object \""
                         + coordName + 
                        "\" is not an allowed value. \n"
                        "The allowed values are: [%s].";                        

      // create CoordinateSystem if not exist
      coordSys =
         (CoordinateSystem*)theGuiInterpreter->GetConfiguredObject(coordName);
      
      if (coordSys == NULL)
      {
         #ifdef DEBUG_COORD_PANEL_SAVE
         MessageInterface::ShowMessage
            ("CoordPanel::SaveData() coordName=%s NOT FOUND in configuration, so must create.\n",
             coordName.c_str());
         #endif
         coordSys = (CoordinateSystem*)
            theGuiInterpreter->CreateObject("CoordinateSystem", coordName);
         
         if (coordSys)
         {
            #ifdef DEBUG_COORD_PANEL_SAVE
            MessageInterface::ShowMessage
               ("CoordPanel::SaveData() coordName=%s created.\n",
                coordName.c_str());
            #endif
         }
         else
         {
            #ifdef DEBUG_COORD_PANEL_SAVE
            MessageInterface::ShowMessage
               ("CoordPanel::SaveData() coordName %s was not able to be created.\n",
                coordName.c_str());
            #endif
            canClose = false;
            return false;
         }
      }
      #ifdef DEBUG_COORD_PANEL_SAVE
      else
      {
      MessageInterface::ShowMessage
         ("CoordPanel::SaveData() coordName=%s WAS FOUND in configuration at <%p>!!!\n",
          coordName.c_str(), coordSys);
      }
      #endif

      // Save the original coordSys, so we can get back to it if there are errors
      csClone = (CoordinateSystem*) coordSys->Clone();

      //-------------------------------------------------------
      // set Axis and Origin
      //-------------------------------------------------------
      coordSys->SetRefObject(axis, Gmat::AXIS_SYSTEM, "");
      axis->SetCoordinateSystemName(coordName);
      #ifdef DEBUG_COORD_PANEL_SAVE
      MessageInterface::ShowMessage
         ("CoordPanel::SaveData() axis <%p> set on coordSys %s\n",
          axis, coordName.c_str());
      #endif

      wxString originName = originComboBox->GetValue().Trim();
      SpacePoint *origin = (SpacePoint*)theGuiInterpreter->GetConfiguredObject(originName.c_str());
      coordSys->SetStringParameter("Origin", std::string(originName.c_str()));
      #ifdef DEBUG_COORD_PANEL_SAVE
      MessageInterface::ShowMessage
         ("CoordPanel::SaveData() About to set origin to %s\n", originName.c_str());
      #endif
      try
      {
         coordSys->SetRefObject(origin, Gmat::SPACE_POINT, origin->GetName());
      }
      catch (BaseException &be)
      {
         // Need to popup this error here
         MessageInterface::PopupMessage(Gmat::ERROR_, be.GetFullMessage().c_str());
         canClose = false;
         // Since there was an error, copy the clone's data back to the object
         if (csClone != NULL)
         {
            coordSys->Copy(csClone);
            delete csClone;
         }
         return canClose;
      }

//      coordSys->SetOrigin(origin);
      #ifdef DEBUG_COORD_PANEL_SAVE
      if (axis)
      {
         MessageInterface::ShowMessage
            ("CoordPanel::SaveData() axis of type %s set on coordSys %s, and origin set to %s <%p>.\n",
             axis->GetTypeName().c_str(), coordName.c_str(), originName.c_str(), origin);
      }
      else
      {
         MessageInterface::ShowMessage
            ("CoordPanel::SaveData() axis (NULL!) set on coordSys %s, and origin set to %s <%p>.\n",
             coordName.c_str(), originName.c_str(), origin);
      }
      #endif
      
      CelestialBody *j2000body =
         (CelestialBody*)theGuiInterpreter->GetConfiguredObject("Earth"); // @todo this will need to be changed when we have
                                                                          // a non-Earth J2000 body - WCS 2012.07.28
      
      // set Earth as J000Body if NULL
      if (origin->GetJ2000Body() == NULL)
      {
         j2000body->SetJ2000Body(j2000body);
         origin->SetJ2000Body(j2000body);
      }

      coordSys->SetJ2000Body(j2000body);
      
      //-------------------------------------------------------
      // set primary and secondary 
      //-------------------------------------------------------
      #ifdef DEBUG_COORD_PANEL_PRIMARY_SECONDARY
         MessageInterface::ShowMessage("CoordPanel::SD, primary enabled? %s\n",
               (primaryComboBox->IsEnabled()? "YES" : "no"));
         MessageInterface::ShowMessage("CoordPanel::SD, secondary enabled? %s\n",
               (secondaryComboBox->IsEnabled()? "YES" : "no"));
      #endif
      // Set primary body if exist
      if (primaryComboBox->IsEnabled() || setThoughDisabled)
      {
         wxString primaryName = primaryComboBox->GetValue().Trim();
         SpacePoint *primary = (SpacePoint*)theGuiInterpreter->
            GetConfiguredObject(primaryName.c_str());
         #ifdef DEBUG_COORD_PANEL_PRIMARY_SECONDARY
            MessageInterface::ShowMessage("CoordPanel::SD, primary name = %s\n",
                  primaryName.WX_TO_STD_STRING.c_str());
            MessageInterface::ShowMessage("CoordPanel::SD, primary object is %s\n",
                  (primary? "NOT NULL" : "NULL"));
            MessageInterface::ShowMessage("CoordPanel::SD, axis pointer (%s) is %p\n",
                  axis->GetTypeName().c_str(), axis);
         #endif
         if (!setThoughDisabled)
         {
            Integer primaryID = axis->GetParameterID("Primary");
            axis->SetStringParameter(primaryID, primaryName.c_str());
         }
         // when we need to set it even though it is not modifiable, we directly
         // set the object (which sets the name) because the SetStringParameter
         // will reject the setting of the name
         axis->SetPrimaryObject(primary);
      }
      
      // set secondary body if exist
      if (secondaryComboBox->IsEnabled() ||setThoughDisabled)
      {
         wxString secondaryName = secondaryComboBox->GetValue().Trim();

         if (!setThoughDisabled)
         {
            axis->SetStringParameter("Secondary", secondaryName.c_str());
         }
         // when we need to set it even though it is not modifiable, we directly
         // set the object (which sets the name) because the SetStringParameter
         // will reject the setting of the name
         if (secondaryName != "")
         {
            SpacePoint *secondary = (SpacePoint*)theGuiInterpreter->
               GetConfiguredObject(secondaryName.c_str());
         
            axis->SetSecondaryObject(secondary);
            if (secondary->GetJ2000Body() == NULL)
               secondary->SetJ2000Body(j2000body);
         }
      }
      #ifdef DEBUG_COORD_PANEL_PRIMARY_SECONDARY
         MessageInterface::ShowMessage("CoordPanel::SD, ref object enabled? %s\n",
               (refObjectComboBox->IsEnabled()? "YES" : "no"));
         MessageInterface::ShowMessage("CoordPanel::SD, constraint CS enabled? %s\n",
               (constraintCSComboBox->IsEnabled()? "YES" : "no"));
      #endif
      // Set reference object, if it exists
      if (refObjectComboBox->IsEnabled())
      {
         wxString refName   = refObjectComboBox->GetValue().Trim();
         SpacePoint *refObj = (SpacePoint*)theGuiInterpreter->
                              GetConfiguredObject(refName.c_str());
         #ifdef DEBUG_COORD_PANEL_PRIMARY_SECONDARY
            MessageInterface::ShowMessage("CoordPanel::SD, ref object name = %s\n",
                  refName.WX_TO_STD_STRING.c_str());
            MessageInterface::ShowMessage("CoordPanel::SD, reference object is %s\n",
                  (refObj? "NOT NULL" : "NULL"));
            MessageInterface::ShowMessage("CoordPanel::SD, axis pointer (%s) is %p\n",
                  axis->GetTypeName().c_str(), axis);
         #endif
         Integer refObjID = axis->GetParameterID("ReferenceObject");
         axis->SetStringParameter(refObjID, refName.c_str());
         axis->SetReferenceObject(refObj);
      }

      std::string str = "";
      Real x, y, z;
      bool isValid = true;
      // Set the alignment vector
      if (alignXTextCtrl->IsEnabled() && alignYTextCtrl->IsEnabled() &&
          alignZTextCtrl->IsEnabled())
      {
         str = alignXTextCtrl->GetValue().c_str();
         isValid = isValid && CheckReal(x, str, "AlignmentVectorX", "Real Number");
         str = alignYTextCtrl->GetValue().c_str();
         isValid = isValid && CheckReal(y, str, "AlignmentVectorY", "Real Number");
         str = alignZTextCtrl->GetValue().c_str();
         isValid = isValid && CheckReal(z, str, "AlignmentVectorZ", "Real Number");
         if (isValid)
         {
            axis->SetRealParameter("AlignmentVectorX", x);
            axis->SetRealParameter("AlignmentVectorY", y);
            axis->SetRealParameter("AlignmentVectorZ", z);
         }
         else
         {
            canClose = false;
         }
      }

      // Set constraint coordinate system, if it exists
      if (constraintCSComboBox->IsEnabled())
      {
         try
         {
            wxString cCSName      = constraintCSComboBox->GetValue().Trim();
            CoordinateSystem *constraintCoord = (CoordinateSystem *)theGuiInterpreter->
               GetConfiguredObject(std::string(cCSName.c_str()));
            axis->SetStringParameter("ConstraintCoordinateSystem", cCSName.c_str());
            axis->SetRefObject(constraintCoord, Gmat::COORDINATE_SYSTEM, constraintCoord->GetName());
            #ifdef DEBUG_COORD_PANEL_PRIMARY_SECONDARY
               MessageInterface::ShowMessage("CoordPanel::SD, constraint CS name = %s\n",
                     cCSName.WX_TO_STD_STRING.c_str());
               MessageInterface::ShowMessage("CoordPanel::SD, constraint CS is %s\n",
                     (constraintCoord? "NOT NULL" : "NULL"));
            #endif
         }
         catch (BaseException &be)
         {
            MessageInterface::PopupMessage(Gmat::ERROR_, be.GetFullMessage());
            canClose = false;
         }
      }
      // Set the constraint vectors
      if (constraintXTextCtrl->IsEnabled() && constraintYTextCtrl->IsEnabled() &&
          constraintZTextCtrl->IsEnabled())
      {
         isValid = true;
         str = constraintXTextCtrl->GetValue().c_str();
         isValid = isValid && CheckReal(x, str, "ConstraintVectorX", "Real Number");
         str = constraintYTextCtrl->GetValue().c_str();
         isValid = isValid && CheckReal(y, str, "ConstraintVectorY", "Real Number");
         str = constraintZTextCtrl->GetValue().c_str();
         isValid = isValid && CheckReal(z, str, "ConstraintVectorZ", "Real Number");
         if (isValid)
         {
            axis->SetRealParameter("ConstraintVectorX", x);
            axis->SetRealParameter("ConstraintVectorY", y);
            axis->SetRealParameter("ConstraintVectorZ", z);
         }
         else
         {
            canClose = false;
         }
      }
      // Set the constraint reference vectors
      if (constraintRefXTextCtrl->IsEnabled() && constraintRefYTextCtrl->IsEnabled() &&
          constraintRefZTextCtrl->IsEnabled())
      {
         isValid = true;
         str = constraintRefXTextCtrl->GetValue().c_str();
         isValid = isValid && CheckReal(x, str, "ConstraintReferenceVectorX", "Real Number");
         str = constraintRefYTextCtrl->GetValue().c_str();
         isValid = isValid && CheckReal(y, str, "ConstraintReferenceVectorY", "Real Number");
         str = constraintRefZTextCtrl->GetValue().c_str();
         isValid = isValid && CheckReal(z, str, "ConstraintReferenceVectorZ", "Real Number");
         if (isValid)
         {
            axis->SetRealParameter("ConstraintReferenceVectorX", x);
            axis->SetRealParameter("ConstraintReferenceVectorY", y);
            axis->SetRealParameter("ConstraintReferenceVectorZ", z);
         }
         else
         {
            canClose = false;
         }
      }

      
      //-------------------------------------------------------
      // set new direction
      //-------------------------------------------------------
      // set X value if exist
      if (xComboBox->IsEnabled())
         axis->SetXAxis(std::string(xComboBox->GetValue().Trim().c_str()));
      
      // set Y value if exist
      if (yComboBox->IsEnabled())
         axis->SetYAxis(std::string(yComboBox->GetValue().Trim().c_str()));
      
      // set z value if exist
      if (zComboBox->IsEnabled())
         axis->SetZAxis(std::string(zComboBox->GetValue().Trim().c_str()));
      
      //-------------------------------------------------------
      // set new epoch format and epoch
      //-------------------------------------------------------
      if (epochTextCtrl->IsEnabled())
      {     
         Real epoch, a1mjd;
         std::string savedEpoch = epochValue.WX_TO_STD_STRING;
         #ifdef DEBUG_COORD_PANEL_SAVE
            MessageInterface::ShowMessage("In CoordPanel::SaveData, saving current epoch value (%s)\n",
                  savedEpoch.c_str());
         #endif
         
         inputString = epochTextCtrl->GetValue();
         if ((GmatStringUtil::ToReal(inputString, &epoch)) &&
             (epoch >= DateUtil::EARLIEST_VALID_MJD_VALUE) && (epoch <= DateUtil::LATEST_VALID_MJD_VALUE))
         {
            epochValue = epochTextCtrl->GetValue();
            a1mjd      = epoch;
            #ifdef DEBUG_COORD_PANEL_SAVE
               MessageInterface::ShowMessage("In CoordPanel::SaveData, setting epoch on axis to %12.10f\n",
                     a1mjd);
            #endif
            axis->SetEpoch(a1mjd);
         }
         else
         {
            std::string warnmsg = DateUtil::EARLIEST_VALID_MJD;
            warnmsg += " <= Real Number <= " + DateUtil::LATEST_VALID_MJD;
            MessageInterface::PopupMessage(Gmat::ERROR_, msg.c_str(), 
               inputString.c_str(),"Epoch",warnmsg.c_str());
            canClose = false;
         }
      }

      // set solar system
      coordSys->SetSolarSystem(theGuiInterpreter->GetSolarSystemInUse());
      try
      {
         #ifdef DEBUG_COORD_PANEL_SAVE
            MessageInterface::ShowMessage("In CoordPanel::SaveData, about to initialize coordSys\n");
         #endif
         coordSys->Initialize();
      }
      catch (BaseException &be)
      {
         // Need to popup this error here
         MessageInterface::PopupMessage(Gmat::ERROR_, be.GetFullMessage().c_str());
         canClose = false;
         // Since there was an error, copy the clone's data back to the object
         if (csClone != NULL)
         {
            coordSys->Copy(csClone);
            delete csClone;
         }
         return canClose;
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage
         ("*** Error *** %s\n", e.GetFullMessage().c_str());
      canClose = false;
      // Since there was an error, copy the clone's data back to the object
      if (csClone != NULL)
      {
         coordSys->Copy(csClone);
         delete csClone;
      }
   }

   #ifdef DEBUG_COORD_PANEL_SAVE
   MessageInterface::ShowMessage
      ("CoordPanel::SaveData() returning canClose = %d\n", canClose);
   #endif
   
   return canClose;
}

