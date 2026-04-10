//------------------------------------------------------------------------------
//                           GmatEventHandler
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
// Created: Mar 27, 2017
/**
 * Base class for event handlers for GUI plugin components.
 */
//------------------------------------------------------------------------------

#ifndef GmatEventHandler_hpp
#define GmatEventHandler_hpp

#include "gmatdefs.hpp"

class GuiInterface;


/**
 * Event handling base class for GUI plugin components.
 *
 * Implementations should access the toolkit specific event handling
 * functionality by deriving a class from this one and using the ConnectEvent
 * method to buuild the event communications.
 */
class GMAT_API GmatEventHandler
{
public:
   GmatEventHandler();
   virtual ~GmatEventHandler();
   GmatEventHandler(const GmatEventHandler& geh);
   GmatEventHandler& operator=(const GmatEventHandler& geh);

   /// Call that is made to connect event handlers to resources
   virtual bool ConnectEvent(Gmat::PluginResource &rec, void *parent=NULL,
         Integer messageID = -1) = 0;
};

#endif /* GmatEventHandler_hpp */
