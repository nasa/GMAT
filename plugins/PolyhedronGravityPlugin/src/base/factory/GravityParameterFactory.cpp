//------------------------------------------------------------------------------
//                           GravityParameterFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 31, 2016
/**
 * Factory used to create parameters that reference gravity model data
 */
//------------------------------------------------------------------------------

#include "GravityParameterFactory.hpp"
#include "SurfaceHeight.hpp"


//------------------------------------------------------------------------------
// GravityParameterFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
GravityParameterFactory::GravityParameterFactory() :
   Factory                (Gmat::PARAMETER),
   registrationComplete   (false)
{
   if (creatables.empty())
   {
      creatables.push_back("SurfaceHeight");
   }
}

//------------------------------------------------------------------------------
// ~GravityParameterFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GravityParameterFactory::~GravityParameterFactory()
{
}

//------------------------------------------------------------------------------
// GravityParameterFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * Constructor that uses an input list of objects
 *
 * @param reateList The list of creatable objects
 */
//------------------------------------------------------------------------------
GravityParameterFactory::GravityParameterFactory(StringArray createList) :
   Factory                (createList, Gmat::PARAMETER),
   registrationComplete   (false)
{
}

//------------------------------------------------------------------------------
// GravityParameterFactory(const GravityParameterFactory& gpf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param gpf The factory copied here
 */
//------------------------------------------------------------------------------
GravityParameterFactory::GravityParameterFactory(
      const GravityParameterFactory& gpf) :
   Factory                (gpf),
   registrationComplete   (false)
{
   if (creatables.empty())
   {
      creatables.push_back("SurfaceHeight");
   }
}

//------------------------------------------------------------------------------
// GravityParameterFactory& operator=(const GravityParameterFactory& gpf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param gpf The factory setting assignmetn data here
 *
 * @return This factory, set to match gpf
 */
//------------------------------------------------------------------------------
GravityParameterFactory& GravityParameterFactory::operator=(
      const GravityParameterFactory& gpf)
{
   Factory::operator=(gpf);
   return *this;
}

bool GravityParameterFactory::DoesObjectTypeMatchSubtype(
      const std::string& theType, const std::string& theSubtype)
{
   bool retval = true;

   return retval;
}

//------------------------------------------------------------------------------
// StringArray GetListOfCreatableObjects(const std::string& qualifier)
//------------------------------------------------------------------------------
/**
 * Accessor for the list of creatable objects
 *
 * Registers the parameters if needed, and then accesses the list of ones that
 * can be created by this factory.
 *
 * @param qualifier The list qualifier (unused here)
 *
 * @return The list of creatable objects
 */
//------------------------------------------------------------------------------
StringArray GravityParameterFactory::GetListOfCreatableObjects(
      const std::string& qualifier)
{
   // A hack to register the new parameters with the ParameterInfo database
   // prior to the usage of the Parameter in a run.  This piece is needed in
   // order for the Parameter to appear on the GUI before one has been created
   // for use, because the ParameterInfo data is filled from the parameter's
   // constructor.
   //
   /// @todo Parameter registration via construction is klunky and should be
   /// fixed
   if (!registrationComplete)
   {
      Parameter *param = CreateParameter("SurfaceHeight",
            "DefaultSC.DefaultFM.SurfaceHeight");
      delete param;

//      // Here's how I think we might do this -- but this doesn't work:
//      // register parameter names with info
//      ParameterInfo *pInfo = ParameterInfo::Instance();
//      pInfo->Add("AtmosDensity", Gmat::SPACECRAFT, Gmat::ORIGIN,
//            "DefaultSC.AtmosDensity", GmatParam::ATTACHED_OBJ, true, true,
//            false, false, "Atmospheric Density");

      registrationComplete = true;
   }

   return Factory::GetListOfCreatableObjects(qualifier);
}

//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string& ofType, const std::string& withName)
//------------------------------------------------------------------------------
/**
 * @see CreateParameter()
 */
//------------------------------------------------------------------------------
GmatBase* GravityParameterFactory::CreateObject(const std::string& ofType,
                                                const std::string& withName)
{
   return CreateParameter(ofType, withName);
}

//------------------------------------------------------------------------------
// Parameter* CreateParameter(const std::string& ofType,
//       const std::string& withName)
//------------------------------------------------------------------------------
/**
 * Parameter creator
 *
 * @param ofType The type of object requested
 * @param withName The name for teh new object
 *
 * @return A new object (or NULL of the typs is not supported here)
 */
//------------------------------------------------------------------------------
Parameter* GravityParameterFactory::CreateParameter(const std::string& ofType,
      const std::string& withName)
{
   Parameter *retval = NULL;
   #ifdef DEBUG_CREATION
      MessageInterface::ShowMessage("Attempting to create a \"%s\" "
            "Parameter...", ofType.c_str());
   #endif

   if (ofType == "SurfaceHeight")
      retval = new SurfaceHeight(withName);

   #ifdef DEBUG_CREATION
      MessageInterface::ShowMessage("output pointer is <%p>\n", retval);
   #endif

   return retval;
}
