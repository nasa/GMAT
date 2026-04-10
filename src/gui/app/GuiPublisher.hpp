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

#ifndef SRC_GUI_APP_GUIPUBLISHER_HPP_
#define SRC_GUI_APP_GUIPUBLISHER_HPP_

#include "gmatdefs.hpp"
#include "Publisher.hpp"
#include <wx/stopwatch.h>


/**
 * A lightweight override of the GMAT base library publisher.
 *
 * The GuiPublisher adds a call to the wxYield function after data has been
 * published.  The yield is set to be called after a set amount of time,
 * defined by a macro to be 33 ms.
 */
class GuiPublisher: public Publisher
{
public:
   static  Publisher *Instance();
   virtual bool Publish(GmatBase *provider, Integer id, Real *data, Integer count, Real propDir = 1.0);
   virtual bool Publish(Integer id, char *data, Integer count = 0);
   virtual bool Publish(Integer id, Integer *data, Integer count);
   virtual void Ping();

private:
   /// Stopwatch used for yield timing
   wxStopWatch yieldStopWatch;

   GuiPublisher();
   virtual ~GuiPublisher();
};

#endif /* SRC_GUI_APP_GUIPUBLISHER_HPP_ */
