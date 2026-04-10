//$Id$
//------------------------------------------------------------------------------
//                              SphericalParameters
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
 * Declares Spacecraft Spehrical parameter classes.
 *   RMag, VMag, SphRA, SphDec, SphElem
 */
//------------------------------------------------------------------------------
#ifndef SphericalParameters_hpp
#define SphericalParameters_hpp

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"


//==============================================================================
//                              SphRMag
//==============================================================================
/**
 * Declares Magnitude of Position class.
 */
//------------------------------------------------------------------------------

class GMAT_API SphRMag : public OrbitReal
{
public:

   SphRMag(const std::string &name = "", GmatBase *obj = NULL);
   SphRMag(const SphRMag &copy);
   SphRMag& operator=(const SphRMag &right);
   virtual ~SphRMag();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
    
protected:

};


//==============================================================================
//                              SphRA
//==============================================================================
/**
 * Declares Spherical Right Ascension parameter class.
 */
//------------------------------------------------------------------------------
class GMAT_API SphRA : public OrbitReal
{
public:

   SphRA(const std::string &name = "", GmatBase *obj = NULL);
   SphRA(const SphRA &copy);
   SphRA& operator=(const SphRA &right);
   virtual ~SphRA();

    // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              SphDec
//==============================================================================
/**
 * Declares Spherical Declination parameter class.
 */
//------------------------------------------------------------------------------
class GMAT_API SphDec : public OrbitReal
{
public:

   SphDec(const std::string &name = "", GmatBase *obj = NULL);
   SphDec(const SphDec &copy);
   SphDec& operator=(const SphDec &right);
   virtual ~SphDec();
   
   // The inherited methods from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              SphVMag
//==============================================================================
/**
 * Declares Magnitude of Velocity class.
 */
//------------------------------------------------------------------------------

class GMAT_API SphVMag : public OrbitReal
{
public:

   SphVMag(const std::string &name = "", GmatBase *obj = NULL);
   SphVMag(const SphVMag &copy);
   SphVMag& operator=(const SphVMag &right);
   virtual ~SphVMag();
    
   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
    
protected:

};


//==============================================================================
//                              SphRAV
//==============================================================================
/**
 * Declares Spherical Right Ascension of velocity parameter class.
 */
//------------------------------------------------------------------------------
class GMAT_API SphRAV : public OrbitReal
{
public:

   SphRAV(const std::string &name = "", GmatBase *obj = NULL);
   SphRAV(const SphRAV &copy);
   SphRAV& operator=(const SphRAV &right);
   virtual ~SphRAV();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//==============================================================================
//                              SphDecV
//==============================================================================
/**
 * Declares Spherical Declination of Velocity parameter class.
 */
//------------------------------------------------------------------------------
class GMAT_API SphDecV : public OrbitReal
{
public:

   SphDecV(const std::string &name = "", GmatBase *obj = NULL);
   SphDecV(const SphDecV &copy);
   SphDecV& operator=(const SphDecV &right);
   virtual ~SphDecV();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//loj: 2/15/05 Added
//==============================================================================
//                              SphAzi
//==============================================================================
/**
 * Declares Spherical Right Ascension of velocity parameter class.
 */
//------------------------------------------------------------------------------
class GMAT_API SphAzi : public OrbitReal
{
public:

   SphAzi(const std::string &name = "", GmatBase *obj = NULL);
   SphAzi(const SphAzi &copy);
   SphAzi& operator=(const SphAzi &right);
   virtual ~SphAzi();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//loj: 2/15/05 Added
//==============================================================================
//                              SphFPA
//==============================================================================
/**
 * Declares Spherical Declination of Velocity parameter class.
 */
//------------------------------------------------------------------------------
class GMAT_API SphFPA : public OrbitReal
{
public:

   SphFPA(const std::string &name = "", GmatBase *obj = NULL);
   SphFPA(const SphFPA &copy);
   SphFPA& operator=(const SphFPA &right);
   virtual ~SphFPA();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//loj: 2/15/05 Changed from SphElem
//==============================================================================
//                              SphRaDecElem
//==============================================================================
/**
 * Declares Spherical RA/DEC type Elements class.
 *   Elements are SphRMag, SphRA, SphDec, SphVMag, SphRAV, SphDecV
 */
//------------------------------------------------------------------------------
class GMAT_API SphRaDecElem : public OrbitRvec6
{
public:

   SphRaDecElem(const std::string &name = "", GmatBase *obj = NULL);
   SphRaDecElem(const SphRaDecElem &copy);
   SphRaDecElem& operator=(const SphRaDecElem &right);
   virtual ~SphRaDecElem();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


//loj: 2/15/05 Added
//==============================================================================
//                              SphAzFpaElem
//==============================================================================
/**
 * Declares Spherical AZ/FPA type Elements class.
 *   Elements are SphRMag, SphRA, SphDec, SphVMag, SphAzi, SphFPA
 */
//------------------------------------------------------------------------------
class GMAT_API SphAzFpaElem : public OrbitRvec6
{
public:

   SphAzFpaElem(const std::string &name = "", GmatBase *obj = NULL);
   SphAzFpaElem(const SphAzFpaElem &copy);
   SphAzFpaElem& operator=(const SphAzFpaElem &right);
   virtual ~SphAzFpaElem();

   // The inherited methods from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};

#endif // SphericalParameters_hpp
