//$Id: PlanetodeticParameters.hpp 2013-07-31 Minhee Kim $
//------------------------------------------------------------------------------
//                            File: PlanetodeticParameters.hpp
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
// Author: Minhee Kim 
// Created: 2013/5/24
//
/**
 * Declares Planetodetic related parameter classes.
 *   PlanetodeticRMAG, PlanetodeticLON, PlanetodeticLAT, PlanetodeticVMAG, PlanetodeticAZI, PlanetodeticHFPA, PlanetodeticState
 */
//------------------------------------------------------------------------------
#ifndef PLANETODETICPARAMETERS_HPP_
#define PLANETODETICPARAMETERS_HPP_

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API PldRMAG : public OrbitReal
{
public:

   PldRMAG(const std::string &name = "", GmatBase *obj = NULL);
   PldRMAG(const PldRMAG &copy);
   PldRMAG& operator=(const PldRMAG &right);
   virtual ~PldRMAG();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API PldLON : public OrbitReal
{
public:

   PldLON(const std::string &name = "", GmatBase *obj = NULL);
   PldLON(const PldLON &copy);
   PldLON& operator=(const PldLON &right);
   virtual ~PldLON();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API PldLAT : public OrbitReal
{
public:

   PldLAT(const std::string &name = "", GmatBase *obj = NULL);
   PldLAT(const PldLAT &copy);
   PldLAT& operator=(const PldLAT &right);
   virtual ~PldLAT();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API PldVMAG : public OrbitReal
{
public:

   PldVMAG(const std::string &name = "", GmatBase *obj = NULL);
   PldVMAG(const PldVMAG &copy);
   PldVMAG& operator=(const PldVMAG &right);
   virtual ~PldVMAG();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API PldAZI : public OrbitReal
{
public:

   PldAZI(const std::string &name = "", GmatBase *obj = NULL);
   PldAZI(const PldAZI &copy);
   PldAZI& operator=(const PldAZI &right);
   virtual ~PldAZI();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API PldHFPA : public OrbitReal
{
public:

   PldHFPA(const std::string &name = "", GmatBase *obj = NULL);
   PldHFPA(const PldHFPA &copy);
   PldHFPA& operator=(const PldHFPA &right);
   virtual ~PldHFPA();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API PldState : public OrbitRvec6
{
public:

   PldState(const std::string &name = "", GmatBase *obj = NULL);
   PldState(const PldState &copy);
   PldState& operator=(const PldState &right);
   virtual ~PldState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};

#endif /*PLANETODETICPARAMETERS_HPP_*/
