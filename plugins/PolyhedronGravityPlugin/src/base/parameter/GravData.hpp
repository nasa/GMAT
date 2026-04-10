//$Id$
//------------------------------------------------------------------------------
//                                  GravData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// contract, Task Order 28.
//
// Author: Darrel Conway, Thinking Systems, Inc
// Created: 2013/05/24  (Based on EnvData)
//
/**
 * Declares class which provides data from a force model, such as acceleration,
 * atmospheric density, current or average F10.7 value, relativistic correction,
 * etc.
 */
//------------------------------------------------------------------------------
#ifndef GravData_hpp
#define GravData_hpp

#include "polyhedrongravitymodel_defs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"
#include "ODEModel.hpp"          // In case force lookup is needed
#include "PhysicalModel.hpp"     // Allow Parameters that use specific forces


class POLYHEDRONGRAVITYMODEL_API GravData : public RefData
{
public:
   
   GravData();
   GravData(const GravData &data);
   GravData& operator= (const GravData& right);
   virtual ~GravData();
   
   Real GetGravReal(const std::string &str);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   const static Real GRAV_REAL_UNDEFINED;// = -9876543210.1234;
   
protected:
   
   enum ModelType
   {
      POINT_MASS,
      HARMONIC,
      POLYHEDRAL,
      UNDEFINED
   };

   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(UnsignedInt type);
   
   Spacecraft                    *mSpacecraft;
   SolarSystem                   *mSolarSystem;
   PhysicalModel                 *mModel;
   PhysicalModel                 *cbForce;
   CelestialBody                 *body;

   ModelType                     forceType;
   Real                          bodyRadius;

   enum
   {
      SPACECRAFT = 0,
      SOLAR_SYSTEM,
      PHYSICAL_MODEL,
      GravDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[GravDataObjectCount];
};
#endif // GravData_hpp
