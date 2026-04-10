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

// This class is a factory for the invisible version of OpenFramesView.
// This class is needed so that it doesn't appear in the script output if saved.
// It is created automatically. 

#ifndef InvisOpenFramesViewFactory_hpp
#define InvisOpenFramesViewFactory_hpp

#include "OVtoOFI_defs.hpp"
#include "Factory.hpp"
#include "Subscriber.hpp"

class OVTOOFI_API InvisOpenFramesViewFactory : public Factory
{
public:
    virtual GmatBase* CreateObject(const std::string& ofType,
        const std::string& withName = "");

    // default constructor
    InvisOpenFramesViewFactory();
    // copy constructor
    InvisOpenFramesViewFactory(const InvisOpenFramesViewFactory& fact);
    // assignment operator
    InvisOpenFramesViewFactory& operator=(const InvisOpenFramesViewFactory& fact);

    virtual ~InvisOpenFramesViewFactory();
};

#endif // InvisOpenFramesViewFactory_hpp
