//$Id$
//------------------------------------------------------------------------------
//                           GuiPublisher
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
// Created: Aug 14, 2018
/**
 * The publisher used in the GMAT GUI.
 */
//------------------------------------------------------------------------------

#include "GuiPublisher.hpp"
#include "gmatwxdefs.hpp"

#define MILLISECONDS_BETWEEN_WX_YIELD 33

//------------------------------------------------------------------------------
// Publisher *Instance()
//------------------------------------------------------------------------------
/**
 * Singleton accessor
 *
 * @return The singleton
 */
//------------------------------------------------------------------------------
Publisher *GuiPublisher::Instance()
{
   if (instance == NULL)
   {
      instance = new GuiPublisher();
   }

   return instance;
}


//------------------------------------------------------------------------------
// GuiPublisher():
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
GuiPublisher::GuiPublisher():
   Publisher      ()
{
}


//------------------------------------------------------------------------------
// ~GuiPublisher()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GuiPublisher::~GuiPublisher()
{
}


//------------------------------------------------------------------------------
// bool Publish(GmatBase *provider, Integer id, Real *data, Integer count, ...)
//------------------------------------------------------------------------------
/**
 * This method is called from the propagation enabled command to publish data
 * so that subscribers can receive data.
 *
 * @param provider The command who is calling this method
 * @param id       The id given by the Publisher
 * @param data     The real type data to publish
 * @param count    Number of data points to publish
 * @param propDir  The direction of propagation (1.0 = forward, -1.0 = backward,
 *                 Note: the following settings are used to only publish to
 *                 ephems in step mode.  Remove these propdir cases
 *                 to implement GMT-6110:
 *                      2.0  = forward, ephems only,
 *                      -2.0 = backwards, ephems only
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool GuiPublisher::Publish(GmatBase *provider, Integer id, Real *data,
      Integer count, Real propDir)
{
   bool retval = Publisher::Publish(provider, id, data, count, propDir);
   Ping();

   return retval;
}

//------------------------------------------------------------------------------
// bool Publish(Integer id, char *data, Integer count)
//------------------------------------------------------------------------------
/**
 * Publishes a character string
 *
 * @param id       The id given by the Publisher
 * @param data     The data for publishing
 * @param count    Number of characters to publish
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool GuiPublisher::Publish(Integer id, char *data, Integer count)
{
   bool retval = Publisher::Publish(id, data, count);
   Ping();

   return retval;
}

//------------------------------------------------------------------------------
// bool Publish(Integer id, Integer *data, Integer count)
//------------------------------------------------------------------------------
/**
 * Publishes integer data
 *
 * @param id       The id given by the Publisher
 * @param data     The data for publishing
 * @param count    Number of integers to publish
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool GuiPublisher::Publish(Integer id, Integer *data, Integer count)
{
   bool retval = Publisher::Publish(id, data, count);
   Ping();

   return retval;
}


//------------------------------------------------------------------------------
// void Ping()
//------------------------------------------------------------------------------
/**
 * Yields to the GUI at fixed intervals so that user actions can be processed
 */
//------------------------------------------------------------------------------
void GuiPublisher::Ping()
{
   // Check for need to wxYield
   if (yieldStopWatch.Time() > MILLISECONDS_BETWEEN_WX_YIELD)
   {
      wxYield();
      yieldStopWatch.Start(0); // Reset stopwatch
   }
}
