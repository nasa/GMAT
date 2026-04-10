//$Id: DelaunaylParameters.hpp 2013-05-24 Minhee Kim $
//------------------------------------------------------------------------------
//                            File: DelaunayParameters.hpp
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
 * Declares Delaunay related parameter classes.
 *   Delaunayl, Delaunayg, Delaunayh, DelaunayL, DelaunayG, DelaunayH, DelaState
 */
//------------------------------------------------------------------------------
#ifndef DELAUNAYPARAMETERS_HPP_
#define DELAUNAYPARAMETERS_HPP_

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API Dela_l : public OrbitReal
{
public:

   Dela_l(const std::string &name = "", GmatBase *obj = NULL);
   Dela_l(const Dela_l &copy);
   Dela_l& operator=(const Dela_l &right);
   virtual ~Dela_l();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};


class GMAT_API Dela_g : public OrbitReal
{
public:

   Dela_g(const std::string &name = "", GmatBase *obj = NULL);
   Dela_g(const Dela_g &copy);
   Dela_g& operator=(const Dela_g &right);
   virtual ~Dela_g();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API Dela_h : public OrbitReal
{
public:

   Dela_h(const std::string &name = "", GmatBase *obj = NULL);
   Dela_h(const Dela_h &copy);
   Dela_h& operator=(const Dela_h &right);
   virtual ~Dela_h();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API DelaL : public OrbitReal
{
public:

   DelaL(const std::string &name = "", GmatBase *obj = NULL);
   DelaL(const DelaL &copy);
   DelaL& operator=(const DelaL &right);
   virtual ~DelaL();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API DelaG : public OrbitReal
{
public:

   DelaG(const std::string &name = "", GmatBase *obj = NULL);
   DelaG(const DelaG &copy);
   DelaG& operator=(const DelaG &right);
   virtual ~DelaG();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API DelaH : public OrbitReal
{
public:

   DelaH(const std::string &name = "", GmatBase *obj = NULL);
   DelaH(const DelaH &copy);
   DelaH& operator=(const DelaH &right);
   virtual ~DelaH();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API DelaState : public OrbitRvec6
{
public:

   DelaState(const std::string &name = "", GmatBase *obj = NULL);
   DelaState(const DelaState &copy);
   DelaState& operator=(const DelaState &right);
   virtual ~DelaState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};

#endif /*DELAUNAYPARAMETERS_HPP_*/

