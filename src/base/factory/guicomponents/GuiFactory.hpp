//$Id$
//------------------------------------------------------------------------------
//                           GuiFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 20, 2017
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef GuiFactory_hpp
#define GuiFactory_hpp

#include "GmatWidget.hpp"


/**
 * Factory base class for GUI factories, defining core interfaces for GUI add-ins.
 *
 * The GUI specific code provides an intermediate factory class in its plugin
 * folder, so that GUIs built with different tool sets (e.g. wxWidgets, Qt) can
 * add functionality at that level.  Derive your GUI widget factories off of the
 * toolkit specific intermediary.
 */
class GMAT_API GuiFactory
{
public:
   GuiFactory();
   virtual ~GuiFactory();

   virtual GmatWidget* CreateWidget(const std::string &ofType, void *parent,
                                    GmatBase *forObj = NULL) = 0;
   virtual bool SupportsType(const std::string theType);

protected:
   // Not user accessible
   GuiFactory(const GuiFactory& gf);
   GuiFactory& operator=(const GuiFactory& gf);

   // can be created by this factory.
   StringArray              creatables;
};

#endif /* GuiFactory_hpp */
