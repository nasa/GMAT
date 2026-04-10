//$Id$
//------------------------------------------------------------------------------
//                            File: EquinoctialParameters.hpp
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
// Author: Daniel Hunter
// Created: 2006/6/27
//
/**
 * Declares Equinoctial related parameter classes.
 *   EquinSma, EquinEy, EquinEx, EquinNy, EquinNx, EquinMlong, EquinState
 */
//------------------------------------------------------------------------------
#ifndef EQUINOCTIALPARAMETERS_HPP_
#define EQUINOCTIALPARAMETERS_HPP_

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API EquinSma : public OrbitReal
{
public:

   EquinSma(const std::string &name = "", GmatBase *obj = NULL);
   EquinSma(const EquinSma &copy);
   EquinSma& operator=(const EquinSma &right);
   virtual ~EquinSma();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API SMADot : public OrbitReal
{
public:

   SMADot(const std::string &name = "", GmatBase *obj = NULL);
   SMADot(const SMADot &copy);
   SMADot& operator=(const SMADot &right);
   virtual ~SMADot();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinEy : public OrbitReal
{
public:

   EquinEy(const std::string &name = "", GmatBase *obj = NULL);
   EquinEy(const EquinEy &copy);
   EquinEy& operator=(const EquinEy &right);
   virtual ~EquinEy();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinEyDot : public OrbitReal
{
public:

   EquinEyDot(const std::string &name = "", GmatBase *obj = NULL);
   EquinEyDot(const EquinEyDot &copy);
   EquinEyDot& operator=(const EquinEyDot &right);
   virtual ~EquinEyDot();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinEx : public OrbitReal
{
public:

   EquinEx(const std::string &name = "", GmatBase *obj = NULL);
   EquinEx(const EquinEx &copy);
   EquinEx& operator=(const EquinEx &right);
   virtual ~EquinEx();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinExDot : public OrbitReal
{
public:

   EquinExDot(const std::string &name = "", GmatBase *obj = NULL);
   EquinExDot(const EquinExDot &copy);
   EquinExDot& operator=(const EquinExDot &right);
   virtual ~EquinExDot();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API EquinNy : public OrbitReal
{
public:

   EquinNy(const std::string &name = "", GmatBase *obj = NULL);
   EquinNy(const EquinNy &copy);
   EquinNy& operator=(const EquinNy &right);
   virtual ~EquinNy();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinNyDot : public OrbitReal
{
public:

   EquinNyDot(const std::string &name = "", GmatBase *obj = NULL);
   EquinNyDot(const EquinNyDot &copy);
   EquinNyDot& operator=(const EquinNyDot &right);
   virtual ~EquinNyDot();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinNx : public OrbitReal
{
public:

   EquinNx(const std::string &name = "", GmatBase *obj = NULL);
   EquinNx(const EquinNx &copy);
   EquinNx& operator=(const EquinNx &right);
   virtual ~EquinNx();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinNxDot : public OrbitReal
{
public:

   EquinNxDot(const std::string &name = "", GmatBase *obj = NULL);
   EquinNxDot(const EquinNxDot &copy);
   EquinNxDot& operator=(const EquinNxDot &right);
   virtual ~EquinNxDot();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinMlong : public OrbitReal
{
public:

   EquinMlong(const std::string &name = "", GmatBase *obj = NULL);
   EquinMlong(const EquinMlong &copy);
   EquinMlong& operator=(const EquinMlong &right);
   virtual ~EquinMlong();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinTlongDot : public OrbitReal
{
public:

   EquinTlongDot(const std::string &name = "", GmatBase *obj = NULL);
   EquinTlongDot(const EquinTlongDot &copy);
   EquinTlongDot& operator=(const EquinTlongDot &right);
   virtual ~EquinTlongDot();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:

};


class GMAT_API EquinState : public OrbitRvec6
{
public:

   EquinState(const std::string &name = "", GmatBase *obj = NULL);
   EquinState(const EquinState &copy);
   EquinState& operator=(const EquinState &right);
   virtual ~EquinState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};

#endif /*EQUINOCTIALPARAMETERS_HPP_*/
