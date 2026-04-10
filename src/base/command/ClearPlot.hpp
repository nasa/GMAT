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


#ifndef ClearPlot_hpp
#define ClearPlot_hpp

#include "PlotCommand.hpp"


/**
 * Command used to remove data from an XY plot during a run
 */
class GMAT_API ClearPlot : public PlotCommand
{
public:
   ClearPlot();
   virtual          ~ClearPlot();
   ClearPlot(const ClearPlot &c);
   ClearPlot&      operator=(const ClearPlot &c);
   
   virtual GmatBase* Clone() const;   
   
   bool              Initialize();
   bool              Execute();

   DEFAULT_TO_NO_CLONES

protected:

};

#endif /* ClearPlot_hpp */
