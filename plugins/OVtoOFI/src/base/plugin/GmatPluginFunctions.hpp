//$Id$
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
// Author: Ed Behn
// Created: 2025/12/22
//
// Developed jointly by NASA/GSFC and Pearl River Technologies

#ifndef GmatPluginFunctions_hpp
#define GmatPluginFunctions_hpp

#include "OVtoOFI_defs.hpp"
#include "Factory.hpp"

class MessageReceiver;

extern "C"
{
   Integer    OVTOOFI_API GetFactoryCount();
   Factory    OVTOOFI_API *GetFactoryPointer(Integer index);
   void       OVTOOFI_API SetMessageReceiver(MessageReceiver* mr);
};


#endif /*GmatPluginFunctions_hpp*/
