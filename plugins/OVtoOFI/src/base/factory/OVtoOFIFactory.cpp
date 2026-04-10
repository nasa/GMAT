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

// This class is a factory for the OFtoOFI class.
// This class launches OpenFramesInterface in place of OrbitView in order
// to improve performance and stability.

#include "OVtoOFIFactory.hpp"
#include "OVtoOFI.hpp"
#include "MessageInterface.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested class
 * in generic way.
 *
 * @param <ofType> the object to create and return.
 * @param <withName> the name to give the newly-created object.
 *
 */
//------------------------------------------------------------------------------
GmatBase* OVtoOFIFactory::CreateObject(const std::string& ofType,
    const std::string& withName)
{
    return CreateSubscriber(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateSubscriber(const std::string &ofType, const std::string &withName,
//                   const std::string &fileName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Subscriber class
 *
 * @param <ofType>   the Subscriber object to create and return.
 * @param <withName> the name to give the newly-created Subscriber object.
 *
 */
//------------------------------------------------------------------------------
Subscriber* OVtoOFIFactory::CreateSubscriber(const std::string& ofType,
    const std::string& withName)
{
    if ((ofType == "OrbitView") || (ofType == "Enhanced3DView") ||
        (ofType == "OpenGLPlot"))
    {
        Subscriber *obj = new OVtoOFI("OpenFramesInterface", withName);
        if (obj)
           MessageInterface::ShowMessage("The OrbitView \"%s\" has been "
                 "converted to an OpenFramesInterface object by the "
                 "libOVtoOFI plugin.\n", withName.c_str());
        return obj;
    }

    return nullptr;
}


//------------------------------------------------------------------------------
//  OVtoOFIFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class OVtoOFIFactory
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
OVtoOFIFactory::OVtoOFIFactory()
{
    if (creatables.empty())
    {
        creatables.push_back("OrbitView");
        creatables.push_back("Enhanced3DView");
        creatables.push_back("OpenGLPlot");
    }

    if (overrides.empty())
    {
        overrides.push_back("OrbitView");
        overrides.push_back("Enhanced3DView");
        overrides.push_back("OpenGLPlot");
    }
}


//------------------------------------------------------------------------------
//  OVtoOFIFactory(const OVtoOFIFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class OVtoOFIFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
OVtoOFIFactory::OVtoOFIFactory(const OVtoOFIFactory& fact) :
    Factory(fact)
{
    if (creatables.empty())
    {
        creatables.push_back("OrbitView");
        creatables.push_back("Enhanced3DView");
        creatables.push_back("OpenGLPlot");
    }

    if (overrides.empty())
    {
        overrides.push_back("OrbitView");
        overrides.push_back("Enhanced3DView");
        overrides.push_back("OpenGLPlot");
    }
}


//------------------------------------------------------------------------------
// OVtoOFIFactory& operator= (const OVtoOFIFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the OVtoOFIFactory class.
 *
 * @param <fact> the OVtoOFIFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" OVtoOFIFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
OVtoOFIFactory& OVtoOFIFactory::operator= (const OVtoOFIFactory& fact)
{
    if (this != &fact)
    {
        Factory::operator=(fact);
    }
    return *this;
}

//------------------------------------------------------------------------------
// ~OVtoOFIFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the OVtoOFIFactory base class.
 *
 */
//------------------------------------------------------------------------------
OVtoOFIFactory::~OVtoOFIFactory()
{
    // deletes handled by Factory destructor
}

