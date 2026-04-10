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

#include "guicomponents/GuiFactory.hpp"

GuiFactory::GuiFactory()
{
}

GuiFactory::~GuiFactory()
{
}

//------------------------------------------------------------------------------
// bool GuiFactory::SupportsType(const std::string theType)
//------------------------------------------------------------------------------
/**
 * Identifies if the type is in the factory's list of creatable objects
 *
 * @param theType The requested widget type
 *
 * @return true if the factory creates objects of the requested type
 */
//------------------------------------------------------------------------------
bool GuiFactory::SupportsType(const std::string theType)
{
   return (find(creatables.begin(), creatables.end(), theType) !=
         creatables.end());
}


GuiFactory::GuiFactory(const GuiFactory& gf) :
   creatables     (gf.creatables)
{
}

GuiFactory& GuiFactory::operator=(const GuiFactory& gf)
{
   if (this != &gf)
   {
      creatables = gf.creatables;
   }
   return *this;
}
