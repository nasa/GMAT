//$Id: ModEquinoctialParameters.hpp 2013-05-24 Minhee Kim $
//------------------------------------------------------------------------------
//                            File: ModEquinoctialParameters.hpp
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
// Created: 2013/05/24
//
/**
 * Declares Modified Equinoctial related parameter classes.
 *ModEquinP, ModEquinF, ModEquinG, ModEquinH, ModEquinK, ModEquinTLONG, ModEquinState
 */
//------------------------------------------------------------------------------
#ifndef MODEQUINOCTIALPARAMETERS_HPP_
#define MODEQUINOCTIALPARAMETERS_HPP_

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API ModEquinP : public OrbitReal
{
public:

   ModEquinP(const std::string &name = "", GmatBase *obj = NULL);
   ModEquinP(const ModEquinP &copy);
   ModEquinP& operator=(const ModEquinP &right);
   virtual ~ModEquinP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};


class GMAT_API ModEquinF : public OrbitReal
{
public:

   ModEquinF(const std::string &name = "", GmatBase *obj = NULL);
   ModEquinF(const ModEquinF &copy);
   ModEquinF& operator=(const ModEquinF &right);
   virtual ~ModEquinF();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API ModEquinG : public OrbitReal
{
public:

   ModEquinG(const std::string &name = "", GmatBase *obj = NULL);
   ModEquinG(const ModEquinG &copy);
   ModEquinG& operator=(const ModEquinG &right);
   virtual ~ModEquinG();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API ModEquinH : public OrbitReal
{
public:

   ModEquinH(const std::string &name = "", GmatBase *obj = NULL);
   ModEquinH(const ModEquinH &copy);
   ModEquinH& operator=(const ModEquinH &right);
   virtual ~ModEquinH();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API ModEquinK : public OrbitReal
{
public:

   ModEquinK(const std::string &name = "", GmatBase *obj = NULL);
   ModEquinK(const ModEquinK &copy);
   ModEquinK& operator=(const ModEquinK &right);
   virtual ~ModEquinK();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API ModEquinTLONG : public OrbitReal
{
public:

   ModEquinTLONG(const std::string &name = "", GmatBase *obj = NULL);
   ModEquinTLONG(const ModEquinTLONG &copy);
   ModEquinTLONG& operator=(const ModEquinTLONG &right);
   virtual ~ModEquinTLONG();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API ModEquinState : public OrbitRvec6
{
public:

   ModEquinState(const std::string &name = "", GmatBase *obj = NULL);
   ModEquinState(const ModEquinState &copy);
   ModEquinState& operator=(const ModEquinState &right);
   virtual ~ModEquinState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};

#endif /*MODEQUINOCTIALPARAMETERS_HPP_*/

