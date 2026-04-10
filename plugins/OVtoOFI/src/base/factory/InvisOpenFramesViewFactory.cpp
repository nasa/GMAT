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

#include "InvisOpenFramesViewFactory.hpp"
#include "OVtoOFI.hpp"

//------------------------------------------------------------------------------
//GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Returns a GmatBase object pointer to a new object.
 *
 * @param ofType   specific type of InvisOpenFramesViewFactory object to create.
 * @param withName name to give to the newly created InvisOpenFramesViewFactory object.
 *
 * @return pointer to a new InvisOpenFramesViewFactory object.
 */
//------------------------------------------------------------------------------
GmatBase* InvisOpenFramesViewFactory::CreateObject(const std::string& ofType, const std::string& withName)
{
    if (ofType == "OpenFramesView")
    {
        GmatBase* obj = new InvisOpenFramesView(ofType, withName);
        return obj;
    }

    return nullptr;
}

//------------------------------------------------------------------------------
//  InvisOpenFramesViewFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class InvisOpenFramesViewFactory
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
InvisOpenFramesViewFactory::InvisOpenFramesViewFactory() :
    Factory(GmatType::RegisterType("OpenFramesView"))
{
    if (creatables.empty())
    {
        creatables.push_back("OpenFramesView");
    }

    if (overrides.empty())
    {
        overrides.push_back("OpenFramesView");
    }
}

//------------------------------------------------------------------------------
//  InvisOpenFramesViewFactory(const InvisOpenFramesViewFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class InvisOpenFramesViewFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
InvisOpenFramesViewFactory::InvisOpenFramesViewFactory(const InvisOpenFramesViewFactory& fact) :
    Factory(fact)
{
    if (creatables.empty())
    {
        creatables.push_back("OpenFramesView");
    }

    if (overrides.empty())
    {
        overrides.push_back("OpenFramesView");
    }
}

//------------------------------------------------------------------------------
// InvisOpenFramesViewFactory& operator= (const InvisOpenFramesViewFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the InvisOpenFramesViewFactory class.
 *
 * @param <fact> the InvisOpenFramesViewFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" InvisOpenFramesViewFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
InvisOpenFramesViewFactory& InvisOpenFramesViewFactory::operator=(const InvisOpenFramesViewFactory& fact)
{
    if (this != &fact)
    {
        Factory::operator=(fact);
    }
    return *this;
}

//------------------------------------------------------------------------------
// ~InvisOpenFramesViewFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the InvisOpenFramesViewFactory base class.
 *
 */
//------------------------------------------------------------------------------
InvisOpenFramesViewFactory::~InvisOpenFramesViewFactory()
{
    // deletes handled by Factory destructor
}
