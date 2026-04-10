//$Id$
//------------------------------------------------------------------------------
//                           FormationInterface
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2013/01/09
//
/**
 * Proxy code for Formation objects, used to define interfaces implemented in 
 * the Formation plugin. 
 */
//------------------------------------------------------------------------------


#include "FormationInterface.hpp"


//------------------------------------------------------------------------------
// FormationInterface(UnsignedInt typeId, const std::string &typeStr,
//       const std::string &instName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param typeId The object's ObjectType
 * @param typeStr The text type of the object
 * @param instName The object's name
 */
//------------------------------------------------------------------------------
FormationInterface::FormationInterface(UnsignedInt typeId,
	  const std::string &typeStr, const std::string &instName) :
   SpaceObject    (typeId, typeStr, instName)
{
   objectTypes.push_back(Gmat::FORMATION);
   objectTypeNames.push_back("FormationInterface");
}


//------------------------------------------------------------------------------
// ~FormationInterface()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
FormationInterface::~FormationInterface()
{
}


//------------------------------------------------------------------------------
// FormationInterface(const FormationInterface& fi)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fi The interface copied here
 */
//------------------------------------------------------------------------------
FormationInterface::FormationInterface(const FormationInterface& fi) :
   SpaceObject       (fi)
{
}


//------------------------------------------------------------------------------
// FormationInterface& operator=(const FormationInterface& fi)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fi The interface copied here
 *
 * @return this object set to look like fi.
 */
//------------------------------------------------------------------------------
FormationInterface& FormationInterface::operator=(const FormationInterface& fi)
{
   if (this != &fi)
   {
      SpaceObject::operator=(fi);
   }

   return *this;
}
