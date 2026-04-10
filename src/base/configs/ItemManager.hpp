//------------------------------------------------------------------------------
//                           ItemManager
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
// Created: Oct 4, 2017
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef ItemManager_hpp
#define ItemManager_hpp

#include "gmatdefs.hpp"


/**
 * Base class for the item managers
 *
 * This is the class providing common functionality for the PluginItemManager
 * and the GMAT GuiItemManager (found in the wxWidgets GUI).
 */
class GMAT_API ItemManager
{
public:
   ItemManager();
   // Virtual destructor to enforce abstract nature of the base class
   virtual ~ItemManager() = 0;

   virtual void UpdateObjects(const UnsignedInt ofType = Gmat::UNKNOWN_OBJECT);
};

#endif /* ItemManager_hpp */
