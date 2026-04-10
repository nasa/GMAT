//$Id$
//------------------------------------------------------------------------------
//                                FileTypes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun (GSFC)
// Created: 2007/05/15
//
/**
 * Provides constants for file types.
 */
//------------------------------------------------------------------------------
#ifndef FileTypes_hpp
#define FileTypes_hpp

#include "utildefs.hpp"

namespace GmatFile
{
   static const Integer MAX_PATH_LEN = 232;
   static const Integer MAX_FILE_LEN = 232;
   static const Integer MAX_LINE_LEN = 512;
}

#endif // FileTypes_hpp
