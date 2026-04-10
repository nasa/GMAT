//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Darrel Conway
// Created: 2019/09/26
//
// Copyright (c) 2019, Thinking Systems, Inc.
// All rights reserved

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * Definition for library code interfaces.
 */


#include "GmatPluginFunctions.hpp"
#include "MessageInterface.hpp"

#include "TlePropFactory.hpp"

extern "C"
{
   Integer GetFactoryCount()
   {
      return 1;
   }
   
   Factory* GetFactoryPointer(Integer index)
   {
      Factory* factory = NULL;

      switch (index)
      {
         case 0:
            factory = new TlePropFactory;
            break;
            
         default:
            break;
      }

      return factory;
   }
   
   void SetMessageReceiver(MessageReceiver* mr)
   {
      MessageInterface::SetMessageReceiver(mr);
   }
};
