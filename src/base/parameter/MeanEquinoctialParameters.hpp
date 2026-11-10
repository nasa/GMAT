//------------------------------------------------------------------------------
//                            File: MeanEquinoctialParameters.hpp
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
// Author: Allen Fu
// Created: 2026/08/10
//
#ifndef MEANEQUINOCTIALPARAMETERS_HPP_
#define MEANEQUINOCTIALPARAMETERS_HPP_

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API MeanEquinSma : public OrbitReal
{
public:

   MeanEquinSma(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinSma(const MeanEquinSma &copy);
   MeanEquinSma& operator=(const MeanEquinSma &right);
   virtual ~MeanEquinSma();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinMM : public OrbitReal
{
public:

   MeanEquinMM(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinMM(const MeanEquinMM &copy);
   MeanEquinMM& operator=(const MeanEquinMM &right);
   virtual ~MeanEquinMM();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinH : public OrbitReal
{
public:

   MeanEquinH(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinH(const MeanEquinH &copy);
   MeanEquinH& operator=(const MeanEquinH &right);
   virtual ~MeanEquinH();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinHDot : public OrbitReal
{
public:

   MeanEquinHDot(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinHDot(const MeanEquinHDot &copy);
   MeanEquinHDot& operator=(const MeanEquinHDot &right);
   virtual ~MeanEquinHDot();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinK : public OrbitReal
{
public:

   MeanEquinK(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinK(const MeanEquinK &copy);
   MeanEquinK& operator=(const MeanEquinK &right);
   virtual ~MeanEquinK();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinKDot : public OrbitReal
{
public:

   MeanEquinKDot(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinKDot(const MeanEquinKDot &copy);
   MeanEquinKDot& operator=(const MeanEquinKDot &right);
   virtual ~MeanEquinKDot();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinP : public OrbitReal
{
public:

   MeanEquinP(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinP(const MeanEquinP &copy);
   MeanEquinP& operator=(const MeanEquinP &right);
   virtual ~MeanEquinP();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinPDot : public OrbitReal
{
public:

   MeanEquinPDot(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinPDot(const MeanEquinPDot &copy);
   MeanEquinPDot& operator=(const MeanEquinPDot &right);
   virtual ~MeanEquinPDot();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinQ : public OrbitReal
{
public:

   MeanEquinQ(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinQ(const MeanEquinQ &copy);
   MeanEquinQ& operator=(const MeanEquinQ &right);
   virtual ~MeanEquinQ();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinQDot : public OrbitReal
{
public:

   MeanEquinQDot(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinQDot(const MeanEquinQDot &copy);
   MeanEquinQDot& operator=(const MeanEquinQDot &right);
   virtual ~MeanEquinQDot();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinMlong : public OrbitReal
{
public:

   MeanEquinMlong(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinMlong(const MeanEquinMlong &copy);
   MeanEquinMlong& operator=(const MeanEquinMlong &right);
   virtual ~MeanEquinMlong();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class GMAT_API MeanEquinState : public OrbitRvec6
{
public:

   MeanEquinState(const std::string &name = "", GmatBase *obj = NULL);
   MeanEquinState(const MeanEquinState &copy);
   MeanEquinState& operator=(const MeanEquinState &right);
   virtual ~MeanEquinState();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};

#endif /*MEANEQUINOCTIALPARAMETERS_HPP_*/
