//------------------------------------------------------------------------------
//                           ShellDriver
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Claire R. Conway, Thinking Systems, Inc.
// Created: Mar 13, 2017
/**
 * INSTRUCTIONS:
 *  - Find & replace "Shell" with the actual name for your driver.
 */
//------------------------------------------------------------------------------

#ifndef SRC_SHELLDRIVER_HPP_
#define SRC_SHELLDRIVER_HPP_

#include "ShellPathObject.hpp"
#include "ShellPointObject.hpp"
#include "csaltdefs.hpp"
#include "csalt.hpp"
#include "gmatdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"

class ShellDriver
{
public:
   ShellDriver();
   virtual ~ShellDriver();
   Real GetMaxError(const Rvector &vec);
   Real GetMaxError(const Rmatrix &mat);
   int Run();
};

#endif /* SRC_SHELLDRIVER_HPP_ */
