//$Id$
//------------------------------------------------------------------------------
//                             File: OrbitalParameters.hpp
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
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/03/12
// Modified: Wendy Shoan 2004/04/01 added OrbitPeriod
//
/**
 * Declares other orbit related parameter classes.
 *   VelApoapsis, VelPeriapsis, Apoapsis, Periapsis, OrbitPeriod,
 *   RadApoapsis, RadPeriapais, C3Energy, Energy
 *
 */
//------------------------------------------------------------------------------
#ifndef OrbitalParameters_hpp
#define OrbitalParameters_hpp

#include "OrbitReal.hpp"

//==============================================================================
//                              VelApoapsis
//==============================================================================
/**
 * Declares Velocity at Apoapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API VelApoapsis : public OrbitReal
{
public:

   VelApoapsis(const std::string &name = "", GmatBase *obj = NULL);
   VelApoapsis(const VelApoapsis &copy);
   const VelApoapsis& operator=(const VelApoapsis &right);
   virtual ~VelApoapsis();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              VelPeriapsis
//==============================================================================
/**
 * Declares Velocity at Periapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API VelPeriapsis : public OrbitReal
{
public:

   VelPeriapsis(const std::string &name = "", GmatBase *obj = NULL);
   VelPeriapsis(const VelPeriapsis &copy);
   const VelPeriapsis& operator=(const VelPeriapsis &right);
   virtual ~VelPeriapsis();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              Apoapsis
//==============================================================================
/**
 * Declares Apoapsis class.
 */
//------------------------------------------------------------------------------

class GMAT_API Apoapsis : public OrbitReal
{
public:

   Apoapsis(const std::string &name = "", GmatBase *obj = NULL);
   Apoapsis(const Apoapsis &copy);
   const Apoapsis& operator=(const Apoapsis &right);
   virtual ~Apoapsis();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              Periapsis
//==============================================================================
/**
 * Declares Periapsis class.
*/
//------------------------------------------------------------------------------

class GMAT_API Periapsis : public OrbitReal
{
public:

   Periapsis(const std::string &name = "", GmatBase *obj = NULL);
   Periapsis(const Periapsis &copy);
   const Periapsis& operator=(const Periapsis &right);
   virtual ~Periapsis();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              OrbitPeriod
//==============================================================================
/**
 * Declares OrbitPeriod class.
 */
//------------------------------------------------------------------------------

class GMAT_API OrbitPeriod : public OrbitReal
{
public:

   OrbitPeriod(const std::string &name = "", GmatBase *obj = NULL);
   OrbitPeriod(const OrbitPeriod &copy);
   const OrbitPeriod& operator=(const OrbitPeriod &right);
   virtual ~OrbitPeriod();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              C3Energy
//==============================================================================
/**
 * Declares C3Energy class.
 */
//------------------------------------------------------------------------------

class GMAT_API C3Energy : public OrbitReal
{
public:

   C3Energy(const std::string &name = "", GmatBase *obj = NULL);
   C3Energy(const C3Energy &copy);
   const C3Energy& operator=(const C3Energy &right);
   virtual ~C3Energy();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              Energy
//==============================================================================
/**
 * Declares Energy class.
 */
//------------------------------------------------------------------------------

class GMAT_API Energy : public OrbitReal
{
public:

   Energy(const std::string &name = "", GmatBase *obj = NULL);
   Energy(const Energy &copy);
   const Energy& operator=(const Energy &right);
   virtual ~Energy();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

#endif // OrbitalParameters_hpp
