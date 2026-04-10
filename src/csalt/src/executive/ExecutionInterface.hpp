//------------------------------------------------------------------------------
//                                ExecutionInterface
//------------------------------------------------------------------------------
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
// contract, Task Order 08.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2018.08.14
//
/**
* Declares the ExecutionInterface class.
*/
//------------------------------------------------------------------------------
#ifndef ExecutionInterface_hpp
#define ExecutionInterface_hpp

#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "Phase.hpp"

class CSALT_API ExecutionInterface
{
public:
   ExecutionInterface();
   ExecutionInterface(const ExecutionInterface &copy);
   ExecutionInterface& operator=(const ExecutionInterface &copy);
   ~ExecutionInterface();

   virtual void Publish(std::string &currState) = 0;
   Rmatrix GetStateArray(Integer phaseIdx);
   Rmatrix GetControlArray(Integer phaseIdx);
   Rvector GetTimeArray(Integer phaseIdx);
   void SetPhaseList(std::vector<Phase*> newPhaseList);

protected:
   /// The list of phases in the trajectory
   std::vector<Phase*> phaseList;
};
#endif
