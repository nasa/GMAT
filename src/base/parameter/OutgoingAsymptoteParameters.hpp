//$Id: OutgoingAsymptoteParameters.hpp 2013-05-24 Youngkwang Kim $
//------------------------------------------------------------------------------
//                            File: OutgoingAsymptoteParameters.hpp
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
// Developed jointly by NASA/GSFC and Thinking Systems, Out. under contract
// number S-67573-G
//
// Author: Youngkwang Kim
// Created: 2013/09/15
//
/**
 * Declares Outgoing Asymptote related parameter classes.
 * OutAsymRadPer, OutAsymC3Energy, OutAsymRHA, OutAsymDHA, OutAsymBVAZI, OutAsymState
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "OrbitReal.hpp"
#include "OrbitRvec6.hpp"


//==============================================================================
//                               OutAsymRadPer
//==============================================================================
class GMAT_API OutAsymRadPer : public OrbitReal
{
public:

   OutAsymRadPer(const std::string &name = "", GmatBase *obj = NULL);
   OutAsymRadPer(const OutAsymRadPer &copy);
   OutAsymRadPer& operator=(const OutAsymRadPer &right);
   virtual ~OutAsymRadPer();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};


//==============================================================================
//                               OutAsymC3Energy
//==============================================================================
class GMAT_API OutAsymC3Energy : public OrbitReal
{
public:

   OutAsymC3Energy(const std::string &name = "", GmatBase *obj = NULL);
   OutAsymC3Energy(const OutAsymC3Energy &copy);
   OutAsymC3Energy& operator=(const OutAsymC3Energy &right);
   virtual ~OutAsymC3Energy();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};


//==============================================================================
//                               OutAsymRHA
//==============================================================================
class GMAT_API OutAsymRHA : public OrbitReal
{
public:

   OutAsymRHA(const std::string &name = "", GmatBase *obj = NULL);
   OutAsymRHA(const OutAsymRHA &copy);
   OutAsymRHA& operator=(const OutAsymRHA &right);
   virtual ~OutAsymRHA();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};


//==============================================================================
//                               OutAsymDHA
//==============================================================================
class GMAT_API OutAsymDHA : public OrbitReal
{
public:

   OutAsymDHA(const std::string &name = "", GmatBase *obj = NULL);
   OutAsymDHA(const OutAsymDHA &copy);
   OutAsymDHA& operator=(const OutAsymDHA &right);
   virtual ~OutAsymDHA();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                               OutAsymDBVAZI
//==============================================================================
class GMAT_API OutAsymBVAZI : public OrbitReal
{
public:

   OutAsymBVAZI(const std::string &name = "", GmatBase *obj = NULL);
   OutAsymBVAZI(const OutAsymBVAZI &copy);
   OutAsymBVAZI& operator=(const OutAsymBVAZI &right);
   virtual ~OutAsymBVAZI();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
	   
protected:

};


//==============================================================================
//                               OutAsymState
//==============================================================================
class GMAT_API OutAsymState : public OrbitRvec6
{
public:

   OutAsymState(const std::string &name = "", GmatBase *obj = NULL);
   OutAsymState(const OutAsymState &copy);
   OutAsymState& operator=(const OutAsymState &right);
   virtual ~OutAsymState();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
    
};


