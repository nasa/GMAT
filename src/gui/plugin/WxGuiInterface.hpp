//------------------------------------------------------------------------------
//                           WxGuiInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// THIS COMPONENT Copyright (c) 2022, Emergent Space Technologies, Inc.
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
// Developed jointly by Emergent Space Technologies, Inc. and
// Thinking Systems, Inc. under SBIR contract NNX16CG16C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 30, 2017
/**
 * Interface code used for GUI plugin components, wxWidgets implementation
 */
//------------------------------------------------------------------------------

#ifndef WxGuiInterface_hpp
#define WxGuiInterface_hpp

#include "gmatdefs.hpp"
#include "GuiInterface.hpp"
#include "ResourceTree.hpp"


/**
 * Class defining wxWidgets interfaces plugins can access to add GUI elements
 *
 * This class implements GUI specific components used in the GMAT wxWidgets GUI.
 *
 * WxGuiInterface is a singleton.
 */
class WxGuiInterface: public GuiInterface
{
public:
   static GuiInterface *Instance();
   void        SetResourceTree(ResourceTree *rt);

   GmatBase*   CreateObject(const std::string& ofType,
                     const std::string& withName);
   void*       CreateGuiElement(const std::string& ofType,
                     const std::string& withName);

protected:
   /// The resource tree pointer
   ResourceTree *resourceTree;

   WxGuiInterface();
   virtual ~WxGuiInterface();
};

#endif /* WxGuiInterface_hpp */
