//$Id: BrouwerMeanLongParameters.hpp 2013-05-24 Youngkwang Kim $
//------------------------------------------------------------------------------
//                            File: BrouwerMeanLongParameters.hpp
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
 * BLlong_SMADP, BLlong_ECCDP, BLlong_INCDP, BLlong_RAANDP, BLlong_AOPDP, BLlong_MADP, BLlong_State
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"

class GMAT_API BLlongSMADP : public OrbitReal
{
public:

   BLlongSMADP(const std::string &name = "", GmatBase *obj = NULL);
   BLlongSMADP(const BLlongSMADP &copy);
   BLlongSMADP& operator=(const BLlongSMADP &right);
   virtual ~BLlongSMADP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};

class GMAT_API BLlongECCDP : public OrbitReal
{
public:

   BLlongECCDP(const std::string &name = "", GmatBase *obj = NULL);
   BLlongECCDP(const BLlongECCDP &copy);
   BLlongECCDP& operator=(const BLlongECCDP &right);
   virtual ~BLlongECCDP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};
class GMAT_API BLlongINCDP : public OrbitReal
{
public:

   BLlongINCDP(const std::string &name = "", GmatBase *obj = NULL);
   BLlongINCDP(const BLlongINCDP &copy);
   BLlongINCDP& operator=(const BLlongINCDP &right);
   virtual ~BLlongINCDP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};

class GMAT_API BLlongRAANDP : public OrbitReal
{
public:

   BLlongRAANDP(const std::string &name = "", GmatBase *obj = NULL);
   BLlongRAANDP(const BLlongRAANDP &copy);
   BLlongRAANDP& operator=(const BLlongRAANDP &right);
   virtual ~BLlongRAANDP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};
class GMAT_API BLlongAOPDP : public OrbitReal
{
public:

   BLlongAOPDP(const std::string &name = "", GmatBase *obj = NULL);
   BLlongAOPDP(const BLlongAOPDP &copy);
   BLlongAOPDP& operator=(const BLlongAOPDP &right);
   virtual ~BLlongAOPDP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};

class GMAT_API BLlongMADP : public OrbitReal
{
public:

   BLlongMADP(const std::string &name = "", GmatBase *obj = NULL);
   BLlongMADP(const BLlongMADP &copy);
   BLlongMADP& operator=(const BLlongMADP &right);
   virtual ~BLlongMADP();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};
class GMAT_API BLlongState : public OrbitRvec6
{
public:

   BLlongState(const std::string &name = "", GmatBase *obj = NULL);
   BLlongState(const BLlongState &copy);
   BLlongState& operator=(const BLlongState &right);
   virtual ~BLlongState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};


