//$Id$
//------------------------------------------------------------------------------
//                             PlotInterface
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
// number S-67573-G
//
// Author: Thomas G. Grubb
// Created: 2015/08/21
//
/**
 * Implements ListenerManagerInterface class.
 */
//------------------------------------------------------------------------------

#include "ListenerManagerInterface.hpp"
#include "MessageInterface.hpp"


//---------------------------------
//  static data
//---------------------------------
ListenerManager *ListenerManagerInterface::theListenerManager = NULL;

//---------------------------------
//  public functions
//---------------------------------


//------------------------------------------------------------------------------
// void SetListenerManager(ListenerManager *lm)
//------------------------------------------------------------------------------
void ListenerManagerInterface::SetListenerManager(ListenerManager *lm)
{
   theListenerManager = lm;
}


//------------------------------------------------------------------------------
//  ListenerManagerInterface()
//------------------------------------------------------------------------------
ListenerManagerInterface::ListenerManagerInterface()
{
}


//------------------------------------------------------------------------------
//  ~ListenerManagerInterface()
//------------------------------------------------------------------------------
ListenerManagerInterface::~ListenerManagerInterface()
{
}


//------------------------------------------------------------------------------
//  bool CreateListener()
//------------------------------------------------------------------------------
/*
 * Creates OpenGlPlot window
 *
 * @param <plotName> plot name
 */
//------------------------------------------------------------------------------
ISolverListener* ListenerManagerInterface::CreateSolverListener(const std::string &tableName,
                                         const std::string &oldName,
                                         Real positionX, Real positionY,
                                         Real width, Real height, bool isMaximized)
{
   #if DEBUG_PLOTIF_GL_CREATE
   MessageInterface::ShowMessage
      ("PI::CreateListener() %s entered, x = %12.10f, y = %12.10f, w = %12.10f, h = %12.10f,"
            "theListenerManager=<%p>\n", tableName.c_str(), positionX, positionY, width, height,
       thePlotReceiver);
   #endif
   
   if (theListenerManager != NULL)
      return theListenerManager->CreateSolverListener(tableName, oldName, positionX, positionY,
            width, height, isMaximized);
   
   return NULL;
}


