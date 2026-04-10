//$Id$
//------------------------------------------------------------------------------
//                               ClearPlot
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Darrel J. Conway
// Created: 2004/02/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Class implementation for the ClearPlot command
 */
//------------------------------------------------------------------------------


#include "ClearPlot.hpp"
#include "XyPlot.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"

//#define DEBUG_CLEARPLOT


//------------------------------------------------------------------------------
// ClearPlot()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ClearPlot::ClearPlot() :
   PlotCommand    ("ClearPlot")
{
}


//------------------------------------------------------------------------------
// ~ClearPlot()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ClearPlot::~ClearPlot()
{
}


//------------------------------------------------------------------------------
// ClearPlot(const ClearPlot &c)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 * 
 * @param <c> The command that gets copied.
 */
//------------------------------------------------------------------------------
ClearPlot::ClearPlot(const ClearPlot &c) :
   PlotCommand    (c)
{
}


//------------------------------------------------------------------------------
// ClearPlot& operator=(const ClearPlot &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <c> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ClearPlot& ClearPlot::operator=(const ClearPlot &c)
{
   if (&c != this)
   {
      PlotCommand::operator=(c);
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Override of the GmatBase clone method.
 * 
 * @return A new copy of this instance.
 */
//------------------------------------------------------------------------------
GmatBase* ClearPlot::Clone() const
{
   return new ClearPlot(*this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Method that initializes the internal data structures.
 * 
 * @return true if initialization succeeds.
 */
//------------------------------------------------------------------------------
bool ClearPlot::Initialize()
{
   #ifdef DEBUG_CLEARPLOT
      MessageInterface::ShowMessage("ClearPlot::Initialize() entered\n");
   #endif
      
   PlotCommand::Initialize();
   
   GmatBase *xy;
   thePlotList.clear();
   
   for (unsigned int ii = 0; ii < plotNameList.size(); ii++)
   {
      if ((xy = FindObject(plotNameList.at(ii))) != NULL) 
      {
         if (xy->GetTypeName() == "XYPlot") 
            thePlotList.push_back((XyPlot*) xy);
         else
            throw CommandException(
               "Object named \"" + plotNameList.at(ii) + "\" should be an XYPlot to use the "
               "ClearPlot command for this object, but it is a " + 
               xy->GetTypeName());      
      }
      else 
      {
         MessageInterface::ShowMessage
            ("ClearPlot command cannot find XY Plot \"%s\"; command has no effect."
            "\n", (plotNameList.at(ii)).c_str());
         return false;
      }
   }
   
   #ifdef DEBUG_CLEARPLOT
      MessageInterface::ShowMessage("ClearPlot::Initialize() returning true\n");
   #endif
   return true;
}


//---------------------------------------------------------------------------
//  bool PlotCommand::Execute()
//---------------------------------------------------------------------------
/**
 * The method that is fired to perform the PlotCommand.
 *
 * Derived classes implement this method to perform their actions on
 * GMAT objects.
 *
 * @return true if the PlotCommand runs to completion, false if an error
 *         occurs.
 */
//---------------------------------------------------------------------------
bool ClearPlot::Execute()
{
   for (unsigned int ii = 0; ii < thePlotList.size(); ii++)
   {
      if (thePlotList.at(ii))
         if (!(thePlotList.at(ii)->TakeAction("ClearData"))) return false;
   }
   
   // Build command summary
   BuildCommandSummary(true);
   
   return true;
}
