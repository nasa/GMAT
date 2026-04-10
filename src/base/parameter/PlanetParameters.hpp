//$Id$
//------------------------------------------------------------------------------
//                             File: PlanetParameters.hpp
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
// Created: 2004/12/13
// Modified:
//   2005/6/10 Linda Jun - Moved BetaAngle to AngularParamters.hpp
//
/**
 * Declares planet related parameter classes.
 *   MHA, Longitude, Altitude(Geodetic), Latitude(Geodetic), LST
 */
//------------------------------------------------------------------------------
#ifndef PlanetParameters_hpp
#define PlanetParameters_hpp

#include "PlanetReal.hpp"


//==============================================================================
//                                     MHA
//==============================================================================
/**
 * Declares Greenwich hour angle parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API MHA : public PlanetReal
{
public:

   MHA(const std::string &name = "", GmatBase *obj = NULL);
   MHA(const MHA &copy);
   const MHA& operator=(const MHA &right);
   virtual ~MHA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                                 Longitude
//==============================================================================
/**
 * Declares Longitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API Longitude : public PlanetReal
{
public:

   Longitude(const std::string &name = "", GmatBase *obj = NULL);
   Longitude(const Longitude &copy);
   const Longitude& operator=(const Longitude &right);
   virtual ~Longitude();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              Altitude
//==============================================================================
/**
 * Declares Altitude class.
 */
//------------------------------------------------------------------------------

class GMAT_API Altitude : public PlanetReal
{
public:
   
   Altitude(const std::string &name = "", GmatBase *obj = NULL);
   Altitude(const Altitude &copy);
   const Altitude& operator=(const Altitude &right);
   virtual ~Altitude();
   
   // The inherited methods from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   
};


//==============================================================================
//                                 Latitude
//==============================================================================
/**
 * Declares Latitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API Latitude : public PlanetReal
{
public:

   Latitude(const std::string &name = "", GmatBase *obj = NULL);
   Latitude(const Latitude &copy);
   const Latitude& operator=(const Latitude &right);
   virtual ~Latitude();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                                     LST
//==============================================================================
/**
 * Declares LST parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API LST : public PlanetReal
{
public:

   LST(const std::string &name = "", GmatBase *obj = NULL);
   LST(const LST &copy);
   const LST& operator=(const LST &right);
   virtual ~LST();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

#endif //PlanetParameters_hpp
