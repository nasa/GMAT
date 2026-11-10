//$Id$
//------------------------------------------------------------------------------
//                             SpaceWeatherParameters
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Actalent under the FDSS-III contract
// 
//
// Author: Allen Fu
// Created: 2026/09/15
//
#ifndef SpaceWeatherParameters_hpp
#define SpaceWeatherParameters_hpp

#include "OdeReal.hpp"
#include "OdeData.hpp"

class NEW_PARAMETER_API F107 : public OdeReal
{
public:

   F107(const std::string &name = "", GmatBase *obj = NULL);
   F107(const F107 &copy);
   const F107& operator=(const F107 &right);
   virtual ~F107();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class NEW_PARAMETER_API F107Avg : public OdeReal
{
public:

   F107Avg(const std::string &name = "", GmatBase *obj = NULL);
   F107Avg(const F107Avg &copy);
   const F107Avg& operator=(const F107Avg &right);
   virtual ~F107Avg();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class NEW_PARAMETER_API Kp : public OdeReal
{
public:

   Kp(const std::string &name = "", GmatBase *obj = NULL);
   Kp(const Kp &copy);
   const Kp& operator=(const Kp &right);
   virtual ~Kp();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class NEW_PARAMETER_API Ap : public OdeReal
{
public:

   Ap(const std::string &name = "", GmatBase *obj = NULL);
   Ap(const Ap &copy);
   const Ap& operator=(const Ap &right);
   virtual ~Ap();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};


class NEW_PARAMETER_API ApAvg : public OdeReal
{
public:

   ApAvg(const std::string &name = "", GmatBase *obj = NULL);
   ApAvg(const ApAvg &copy);
   const ApAvg& operator=(const ApAvg &right);
   virtual ~ApAvg();

   virtual bool Evaluate();

   virtual GmatBase* Clone() const;

protected:

};

#endif
