//$Id$
//------------------------------------------------------------------------------
//                                  PlanetaryEphem
//------------------------------------------------------------------------------
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
// Author: Wendy C. Shoan
// Created: 2004/02/18
//
/**
 * Implementation of the PlanetaryEphem class.
 *
 * @note This class was created (basically) from Swingby code.  Major mods were:
 * - changed #defines to static const parameters
 * - removed _MAX_PATH (not standard on platforms) - use new static
 *   const MAX_PATH_LEN instead
 */
//------------------------------------------------------------------------------

#include "PlanetaryEphem.hpp"
#include "TimeTypes.hpp"
#include <cstdio>  // Fix for header rearrangement in gcc 4.4

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  PlanetaryEphem(std::string withFileName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the PlanetaryEphem class
 * (default constructor).
 *
 * @param <name> parameter indicating the full path name of the File.
 */
//------------------------------------------------------------------------------
PlanetaryEphem::PlanetaryEphem(std::string withFileName) :
   jdMjdOffset (GmatTimeConstants::JD_JAN_5_1941),
   itsName     (withFileName)
{
   strcpy(g_pef_dcb.full_path,withFileName.c_str());
   g_pef_dcb.recl           = 0;
   g_pef_dcb.fptr           = NULL;
   theTimeConverter = TimeSystemConverter::Instance();
}

//------------------------------------------------------------------------------
//  PlanetaryEphem(const PlanetaryEphem& pef)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the PlanetaryEphem class
 * (constructor).
 *
 * @param <pef> PlanetaryEphem object whose values to copy to create a new
 *              PlanetaryEphem.
 */
//------------------------------------------------------------------------------
PlanetaryEphem::PlanetaryEphem(const PlanetaryEphem& pef) :
   jdMjdOffset   (pef.jdMjdOffset),
   itsName       (pef.itsName),
   g_pef_dcb     (pef.g_pef_dcb)
{
   theTimeConverter = TimeSystemConverter::Instance();
}

//------------------------------------------------------------------------------
//  PlanetaryEphem& operator= (const PlanetaryEphem& pef)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the PlanetaryEphem class.
 *
 * @param <pef> the PlanetaryEphem object whose data to assign to "this"
 *            File.
 *
 * @return "this" PlanetaryEphem with data of input PlanetaryEphem pef.
 */
//------------------------------------------------------------------------------
PlanetaryEphem& PlanetaryEphem::operator=(const PlanetaryEphem& pef)
{
   if (this == &pef) return *this;
   jdMjdOffset = pef.jdMjdOffset;
   itsName     = pef.itsName;
   g_pef_dcb   = pef.g_pef_dcb;
   return *this;
}

//------------------------------------------------------------------------------
//  ~PlanetaryEphem()
//------------------------------------------------------------------------------
/**
 * Destructor for the PlanetaryEphem class.
 */
//------------------------------------------------------------------------------
PlanetaryEphem::~PlanetaryEphem()
{
   if (g_pef_dcb.fptr)  fclose(g_pef_dcb.fptr);   
}

//------------------------------------------------------------------------------
//  std::string GetName()
//------------------------------------------------------------------------------
/**
 * Returns the name of the file (full path name).
 *
 * @return the full path name of the file.
 */
//------------------------------------------------------------------------------
std::string PlanetaryEphem::GetName() const
{
   return itsName;
}
