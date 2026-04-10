//$Id$
//------------------------------------------------------------------------------
//                            ThrusterConfigPanel
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
// Author: Waka Waktola
// Created: 2005/01/06
// Modifications:
//    2009.02.10  Linda Jun - Derived from the BurnThrusterPanel
//
/**
 * This class contains information needed to setup users spacecraft thruster 
 * parameters.
 */
//------------------------------------------------------------------------------
#include "ThrusterConfigPanel.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BURNPANEL_SAVE
//#define DEBUG_BURNPANEL_CREATE

//------------------------------
// public methods
//------------------------------

//------------------------------------------------------------------------------
// ThrusterConfigPanel(wxWindow *parent, const wxString &name)
//------------------------------------------------------------------------------
/**
 * Constructs ThrusterConfigPanel object.
 */
//------------------------------------------------------------------------------
ThrusterConfigPanel::ThrusterConfigPanel(wxWindow *parent, 
                                         const wxString &name)
   : BurnThrusterPanel(parent, name)
{
   theObject = theGuiInterpreter->GetConfiguredObject(name.c_str()); // this is done in BurnThrusterPanel???
   
   #ifdef DEBUG_BURNPANEL_CREATE
   MessageInterface::ShowMessage
      ("ThrusterConfigPanel() constructor entered, theObject=<%p>'%s'\n",
       theObject, theObject->GetTypeName().c_str());
   #endif
   
   isElectric = (theObject->IsOfType("ElectricThruster"));

   // To set panel object and show warning if object is NULL
   if (SetObject(theObject))
   {
      Create();
      Show();
   }
}


//------------------------------------------------------------------------------
// ~ThrusterConfigPanel()
//------------------------------------------------------------------------------
ThrusterConfigPanel::~ThrusterConfigPanel()
{
}


//------------------------------------------------------------------------------
// void LoadData()
//------------------------------------------------------------------------------
void ThrusterConfigPanel::LoadData()
{
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("ThrusterConfigPanel::LoadData() entered\n");
   #endif
   
   try
   {
      Integer paramID;
      paramID = theObject->GetParameterID("DutyCycle");
      dutyCycleTextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
      
      paramID = theObject->GetParameterID("ThrustScaleFactor");
      scaleFactorTextCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));

      if (isElectric)
      {
         paramID     = theObject->GetParameterID("ThrustModel");
         thrustModel = theObject->GetStringParameter(paramID);
         thrustModelCB->SetValue(thrustModel.c_str());

         paramID = theObject->GetParameterID("MinimumUsablePower");
         minPowerTxtCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));

         paramID = theObject->GetParameterID("MaximumUsablePower");
         maxPowerTxtCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));

         paramID = theObject->GetParameterID("FixedEfficiency");
         efficiencyTxtCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));

         paramID = theObject->GetParameterID("Isp");
         ispTxtCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));

         paramID = theObject->GetParameterID("ConstantThrust");
         constantThrustTxtCtrl->SetValue(wxVariant(theObject->GetRealParameter(paramID)));
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, e.GetFullMessage());
   }
   
   BurnThrusterPanel::LoadData();
   if (isElectric) EnableDataForThrustModel(thrustModel);
   
   #ifdef DEBUG_BURNPANEL_LOAD
   MessageInterface::ShowMessage("ThrusterConfigPanel::LoadData() entered\n");
   #endif
}


//------------------------------------------------------------------------------
// void SaveData()
//------------------------------------------------------------------------------
void ThrusterConfigPanel::SaveData()
{
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("ThrusterConfigPanel::SaveData() entered\n");
   #endif
   
   canClose = true;
   std::string str;
   std::string tModel;
   Real dutyCycle, scaleFactor;
   Real minPower, maxPower, isp, eff, constThrust;
   Integer paramID;
   bool dutyCycleScaleFactorChanged = false;

   bool thrustModelChanged          = false;
   bool minMaxPowerChanged          = false;
   bool efficiencyChanged           = false;
   bool ispChanged                  = false;
   bool constThrustChanged          = false;
   
   try 
   {
      if (dutyCycleTextCtrl->IsModified() || scaleFactorTextCtrl->IsModified())
      {
         str = dutyCycleTextCtrl->GetValue();      
         CheckReal(dutyCycle, str, "DutyCycle", "Real Number");
         
         str = scaleFactorTextCtrl->GetValue();      
         CheckReal(scaleFactor, str, "ThrustScaleFactor", "Real Number");
         
         dutyCycleScaleFactorChanged = true;
      }
      
      if (!canClose)
         return;
      
      if (dutyCycleScaleFactorChanged)
      {
         paramID = theObject->GetParameterID("DutyCycle");
         theObject->SetRealParameter(paramID, dutyCycle);
      
         paramID = theObject->GetParameterID("ThrustScaleFactor");
         theObject->SetRealParameter(paramID, scaleFactor);
      }
      if (isElectric)
      {
         // Min and Max Usable Power
         if (minPowerTxtCtrl->IsModified() || maxPowerTxtCtrl->IsModified())
         {
            str = minPowerTxtCtrl->GetValue();
            CheckReal(minPower, str, "MinimumUsablePower", "Real Number > 0",
                  false, true, true, false);

            str = maxPowerTxtCtrl->GetValue();
            CheckReal(maxPower, str, "MaximumUsablePower", "Real Number > 0",
                  false, true, true, false);

            // check for coupling between min and max here <<<<<<<<<<<<<<<<

            minMaxPowerChanged = true;
         }
         // Efficiency
         if (efficiencyTxtCtrl->IsModified())
         {
            str = efficiencyTxtCtrl->GetValue();
            CheckReal(eff, str, "FixedEfficiency", "Real Number");

            efficiencyChanged = true;
         }
         // ISP
         if (ispTxtCtrl->IsModified())
         {
            str = ispTxtCtrl->GetValue();
            CheckReal(isp, str, "Isp", "Real Number");

            ispChanged = true;
         }
         // Constant Thrust
         if (constantThrustTxtCtrl->IsModified())
         {
            str = constantThrustTxtCtrl->GetValue();
            CheckReal(constThrust, str, "ConstantThrust", "Real Number");

            constThrustChanged = true;
         }
         if (!canClose)
            return;

         // Thrust Model
         if (isThrustModelChanged)
         {
            paramID = theObject->GetParameterID("ThrustModel");
            theObject->SetStringParameter(paramID, thrustModel);
            isThrustModelChanged = false;
         }
         // Min and Max Usable Power
         if (minMaxPowerChanged)
         {
            paramID = theObject->GetParameterID("MinimumUsablePower");
            theObject->SetRealParameter(paramID, minPower);

            paramID = theObject->GetParameterID("MaximumUsablePower");
            theObject->SetRealParameter(paramID, maxPower);
         }
         if (efficiencyChanged)
         {
            paramID = theObject->GetParameterID("FixedEfficiency");
            theObject->SetRealParameter(paramID, eff);
         }
         if (ispChanged)
         {
            paramID = theObject->GetParameterID("Isp");
            theObject->SetRealParameter(paramID, isp);
         }
         if (constThrustChanged)
         {
            paramID = theObject->GetParameterID("ConstantThrust");
            theObject->SetRealParameter(paramID, constThrust);
         }
      }
   }
   catch(BaseException &ex)
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, ex.GetFullMessage());
      canClose = false;
   }
   
   if (canClose) BurnThrusterPanel::SaveData();
   
   #ifdef DEBUG_BURNPANEL_SAVE
   MessageInterface::ShowMessage("ThrusterConfigPanel::SaveData() exiting\n");
   #endif
}
