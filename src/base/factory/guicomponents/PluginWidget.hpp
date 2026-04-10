//------------------------------------------------------------------------------
//                           PluginWidget
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
// Created: Oct 6, 2017
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef PluginWidget_hpp
#define PluginWidget_hpp

#include "gmatdefs.hpp"
#include <string>

class GMAT_API PluginWidget
{
public:
   PluginWidget();
   virtual ~PluginWidget();

   virtual void RenameObject(const std::string &oldName, const std::string newName,
                     const UnsignedInt ofType);
   virtual void UpdateObjectList(const UnsignedInt ofType);
   bool MinimizeOnRun();
   const char* GetIcon();

protected:
   /// Flag used to indicate if a widget should be minimized when a script is run
   bool minimizeOnRun;
   /// The component's icon (if any)
   char* theIcon;
};

#endif /* PluginWidget_hpp */
