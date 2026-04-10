//$Id$
//------------------------------------------------------------------------------
//                           SubscriberSetupPanel
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
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2008/04/17
/**
 * This class is a generic setup panel used by plug-in subscribers.
 */
//------------------------------------------------------------------------------

#ifndef SubscriberSetupPanel_hpp
#define SubscriberSetupPanel_hpp

#include "GmatPanel.hpp"
#include "Subscriber.hpp"

/**
 * Generic configuration panel for Subscribers
 *
 * This class defines a generic Subscriber configuration panel that is used when
 * a custom panel has not been coded.  It provides access to all of the
 * Subscriber's writable parameters using text controls and comboboxes.
 *
 * The code has been tested using two Optimizers: the VF13ad optimizer and the
 * (removed) SteepestDescent optimizer.  The panel functions correctly for
 * both of these optimizers running either as part of a base GMAT build or
 * through the plug-in interfaces.
 */
class SubscriberSetupPanel : public GmatPanel
{
public:
	SubscriberSetupPanel(wxWindow *parent, const wxString &name);
	virtual ~SubscriberSetupPanel();

protected:
   /// The subscriber that the panel configures
   Subscriber           *theSubscriber;
   /// Flag indicating that a change was made to the panel
   bool                 isTextModified;

   virtual void         Create();
   virtual void         LoadData();
   virtual void         SaveData();
   void                 Setup(wxWindow *parent);
   wxControl *          BuildControl(wxWindow *parent, Integer index);
   void                 LoadControl(const std::string &label);
   void                 SaveControl(const std::string &label);

   // Text control event method
   void OnTextUpdate(wxCommandEvent& event);

   // any class wishing to process wxWindows events must use this macro
   DECLARE_EVENT_TABLE();

   void OnComboBoxChange(wxCommandEvent& event);
   void OnTextChange(wxCommandEvent& event);

   /// Labels used for the configurable properties
   std::vector<wxStaticText*>       propertyDescriptors;
   /// GUI controls that are used to configure the properties
   std::vector<wxControl*>          propertyControls;
   /// Mapping between text strings and the index for the associated control
   std::map<std::string, Integer>   controlMap;

   /// IDs used for event management
   enum
   {
      ID_TEXT = 55000,
      ID_TEXTCTRL,
      ID_COMBOBOX
   };

   /// True-false strings (just a convenience here)
   static const wxString TF_SCHEMES[2];

};

#endif /* SubscriberSetupPanel_hpp */
