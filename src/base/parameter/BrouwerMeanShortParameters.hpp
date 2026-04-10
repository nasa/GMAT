//$Id: BrouwerMeanShortParameters.hpp 2013-05-24 Youngkwang Kim $
//------------------------------------------------------------------------------
//                            File: BrouwerMeanShortParameters.hpp
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
// Author: Youngkwang Kim
// Created: 2013/09/15
//
/**
 * Declares Incoming Asymptote related parameter classes.
 * BLshort_SMAP, BLshort_ECCP, BLshort_INCP, BLshort_RAANP, BLshort_AOPP, BLshort_MAP, BLshort_State
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API BLshortSMAP : public OrbitReal
{
public:

   BLshortSMAP(const std::string &name = "", GmatBase *obj = NULL);
   BLshortSMAP(const BLshortSMAP &copy);
   BLshortSMAP& operator=(const BLshortSMAP &right);
   virtual ~BLshortSMAP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};

class GMAT_API BLshortECCP : public OrbitReal
{
public:

   BLshortECCP(const std::string &name = "", GmatBase *obj = NULL);
   BLshortECCP(const BLshortECCP &copy);
   BLshortECCP& operator=(const BLshortECCP &right);
   virtual ~BLshortECCP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};
class GMAT_API BLshortINCP : public OrbitReal
{
public:

   BLshortINCP(const std::string &name = "", GmatBase *obj = NULL);
   BLshortINCP(const BLshortINCP &copy);
   BLshortINCP& operator=(const BLshortINCP &right);
   virtual ~BLshortINCP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};

class GMAT_API BLshortRAANP : public OrbitReal
{
public:

   BLshortRAANP(const std::string &name = "", GmatBase *obj = NULL);
   BLshortRAANP(const BLshortRAANP &copy);
   BLshortRAANP& operator=(const BLshortRAANP &right);
   virtual ~BLshortRAANP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};
class GMAT_API BLshortAOPP : public OrbitReal
{
public:

   BLshortAOPP(const std::string &name = "", GmatBase *obj = NULL);
   BLshortAOPP(const BLshortAOPP &copy);
   BLshortAOPP& operator=(const BLshortAOPP &right);
   virtual ~BLshortAOPP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};

class GMAT_API BLshortMAP : public OrbitReal
{
public:

   BLshortMAP(const std::string &name = "", GmatBase *obj = NULL);
   BLshortMAP(const BLshortMAP &copy);
   BLshortMAP& operator=(const BLshortMAP &right);
   virtual ~BLshortMAP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};
class GMAT_API BLshortState : public OrbitRvec6
{
public:

   BLshortState(const std::string &name = "", GmatBase *obj = NULL);
   BLshortState(const BLshortState &copy);
   BLshortState& operator=(const BLshortState &right);
   virtual ~BLshortState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};


