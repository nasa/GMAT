//$Id: GmatPluginFunctions.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/08/03
//
/**
 * Definition for library code interfaces.
 */
//------------------------------------------------------------------------------

#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"
#include "TriggerManager.hpp"

class MessageReceiver;

extern "C"
{
   Integer          ESTIMATION_API GetFactoryCount();
   Integer          ESTIMATION_API GetTriggerManagerCount();
   Factory          ESTIMATION_API *GetFactoryPointer(Integer index);
   TriggerManager   ESTIMATION_API *GetTriggerManager(Integer index);
   void             ESTIMATION_API SetMessageReceiver(MessageReceiver* mr);

   // GUI plugin elements
   Integer              ESTIMATION_API GetMenuEntryCount();
   Gmat::PluginResource ESTIMATION_API *GetMenuEntry(Integer index);
};


#endif /* GmatPluginFunctions_hpp */


/**
 * \mainpage API for the GMAT Estimation Plugin
 *
 * \section intro_sec Introduction
 *
 * The estimation capabilities of GMAT are provided through plug-in code loaded
 * into GMAT at run time.  The design for the components of the plug-in can be
 * found in the GMAT Estimation Specification, which is volume 2 of GMAT's
 * Architectural Specification.
 *
 * This document provides a static, low level guide to the code in the
 * estimation plug-in.  The text of this document is generated directly from the
 * source code for the plug-in.  It is built on the comments contained in that
 * code, and matches the contents to the extent that the comments match the
 * implementation.  The class attributes and methods are parsed directly from
 * the source, and are therefore guaranteed to match the source code at the time
 * this document was generated.
 *
 * GMAT's estimation capabilities are a work in progress.  As such, this
 * document will become stale over time.  It is built using the Doxygen
 * documentation tool.  We recommend that a fresh version of the document
 * be generated prior to use as a reference guide for new developers.
 */
