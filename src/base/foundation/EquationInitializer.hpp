//------------------------------------------------------------------------------
//                           EquationInitializer
//------------------------------------------------------------------------------
// GMAT:  General Mission Analysis Tool
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 19, 2018
/**
 * Initializer for equations
 */
//------------------------------------------------------------------------------

#ifndef EquationInitializer_hpp
#define EquationInitializer_hpp

#include "gmatdefs.hpp"

class SolarSystem;
class CoordinateSystem;

/**
 * Initializer for equations in objects in a Sandbox
 */
class GMAT_API EquationInitializer
{
public:
   EquationInitializer(SolarSystem *solSys, ObjectMap *objMap,
         ObjectMap *globalObjMap, CoordinateSystem* intCS/*,
         bool useGOS = false, bool fromFunction = false*/);
   virtual ~EquationInitializer();

   bool PrepareEquationsForMapObjects();

//   // Placeholder for moving equations from commands (i.e. from Assignment)
//   bool PrepareEquations(GmatCommand *cmd);

protected:
   /// The solar system in use
   SolarSystem      *theSolarSystem;
   /// Mapping of (local) objects in the Sandbox
   ObjectMap        *theLocalObjectStore;
   /// Mapping of (global) objects in the Sandbox
   ObjectMap        *theGlobalObjectStore;
   /// The current reference coordinate system
   CoordinateSystem *theInternalCS;

//   Moderator        *theModerator;
//   Publisher        *thePublisher;

//   bool includeGOS;
//   bool registerSubscribers;
//   bool inFunction;

   bool PrepareEquations(GmatBase *forObj);


   // Don't allow these this week
   EquationInitializer(const EquationInitializer &eqi);
   EquationInitializer operator=(const EquationInitializer &eqi);
};

#endif /* EquationInitializer_hpp */
