//$Id$
//------------------------------------------------------------------------------
//                             File: AngularParameters.hpp
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
//
/**
 * Declares angle related parameter classes.
 *   SemilatusRectum, AngularMomemtumMag, AngularMomentumX, AngularMomentumY,
 *   AngularMomentumZ, BetaAngle
 */
//------------------------------------------------------------------------------
#ifndef AngularParameters_hpp
#define AngularParameters_hpp

#include "OrbitReal.hpp"

//==============================================================================
//                              SemilatusRectum
//==============================================================================
/**
 * Declares Semilatus Rectum parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API SemilatusRectum : public OrbitReal
{
public:

   SemilatusRectum(const std::string &name = "", GmatBase *obj = NULL);
   SemilatusRectum(const SemilatusRectum &copy);
   const SemilatusRectum& operator=(const SemilatusRectum &right);
   virtual ~SemilatusRectum();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                              AngularMomentumMag
//==============================================================================
/**
 * Declares angular momentum magnitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API AngularMomentumMag : public OrbitReal
{
public:

   AngularMomentumMag(const std::string &name = "", GmatBase *obj = NULL);
   AngularMomentumMag(const AngularMomentumMag &copy);
   const AngularMomentumMag& operator=(const AngularMomentumMag &right);
   virtual ~AngularMomentumMag();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              AngularMomentumX
//==============================================================================
/**
 * Declares angular momentum magnitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API AngularMomentumX : public OrbitReal
{
public:

   AngularMomentumX(const std::string &name = "", GmatBase *obj = NULL);
   AngularMomentumX(const AngularMomentumX &copy);
   const AngularMomentumX& operator=(const AngularMomentumX &right);
   virtual ~AngularMomentumX();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              AngularMomentumY
//==============================================================================
/**
 * Declares angular momentum magnitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API AngularMomentumY : public OrbitReal
{
public:

   AngularMomentumY(const std::string &name = "", GmatBase *obj = NULL);
   AngularMomentumY(const AngularMomentumY &copy);
   const AngularMomentumY& operator=(const AngularMomentumY &right);
   virtual ~AngularMomentumY();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              AngularMomentumZ
//==============================================================================
/**
 * Declares angular momentum magnitude parameter class.
 */
//------------------------------------------------------------------------------

class GMAT_API AngularMomentumZ : public OrbitReal
{
public:

   AngularMomentumZ(const std::string &name = "", GmatBase *obj = NULL);
   AngularMomentumZ(const AngularMomentumZ &copy);
   const AngularMomentumZ& operator=(const AngularMomentumZ &right);
   virtual ~AngularMomentumZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                                     BetaAngle
//==============================================================================
/**
 * Declares BetaAngle parameter class. BetaAngle is the angle between the
 * orbit normal vector, and the vector from the celestial body to the sun.
 */
//------------------------------------------------------------------------------

class GMAT_API BetaAngle : public OrbitReal
{
public:

   BetaAngle(const std::string &name = "", GmatBase *obj = NULL);
   BetaAngle(const BetaAngle &copy);
   const BetaAngle& operator=(const BetaAngle &right);
   virtual ~BetaAngle();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

//==============================================================================
//                                     RLA
//==============================================================================
/**
 * Declares DLA parameter class. RLA is the right ascension of the outgoing
 * asymptote of a hyperbolic trajectory.
 */
//------------------------------------------------------------------------------

class GMAT_API RLA : public OrbitReal
{
public:

   RLA(const std::string &name = "", GmatBase *obj = NULL);
   RLA(const RLA &copy);
   const RLA& operator=(const RLA &right);
   virtual ~RLA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

//==============================================================================
//                                     DLA
//==============================================================================
/**
 * Declares DLA parameter class. DLA is the declination of the outgoing asymptote
 * of a hyperbolic trajectory.
 */
//------------------------------------------------------------------------------

class GMAT_API DLA : public OrbitReal
{
public:

   DLA(const std::string &name = "", GmatBase *obj = NULL);
   DLA(const DLA &copy);
   const DLA& operator=(const DLA &right);
   virtual ~DLA();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

#endif //AngularParameters_hpp
