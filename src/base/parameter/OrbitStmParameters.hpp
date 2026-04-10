//$Id$
//------------------------------------------------------------------------------
//                             File: OrbitStmParameters
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun
// Created: 2009.03.20
//
/**
 * Declares orbit state transition related parameter classes.
 *    OrbitSTM, OrbitStmA, OrbitStmB, OrbitStmC, OrbitStmD
 */
//------------------------------------------------------------------------------
#ifndef OrbitStmParameters_hpp
#define OrbitStmParameters_hpp

#include "gmatdefs.hpp"
#include "OrbitRmat66.hpp"
#include "OrbitRmat33.hpp"

//==============================================================================
//                              OrbitStm
//==============================================================================

class GMAT_API OrbitStm : public OrbitRmat66
{
public:

   OrbitStm(const std::string &name = "", GmatBase *obj = NULL);
   OrbitStm(const OrbitStm &copy);
   OrbitStm& operator=(const OrbitStm &right);
   virtual ~OrbitStm();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              OrbitStmA
//==============================================================================
class GMAT_API OrbitStmA : public OrbitRmat33
{
public:

   OrbitStmA(const std::string &name = "", GmatBase *obj = NULL);
   OrbitStmA(const OrbitStmA &copy);
   OrbitStmA& operator=(const OrbitStmA &right);
   virtual ~OrbitStmA();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              OrbitStmB
//==============================================================================
class GMAT_API OrbitStmB : public OrbitRmat33
{
public:

   OrbitStmB(const std::string &name = "", GmatBase *obj = NULL);
   OrbitStmB(const OrbitStmB &copy);
   OrbitStmB& operator=(const OrbitStmB &right);
   virtual ~OrbitStmB();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              OrbitStmD
//==============================================================================
class GMAT_API OrbitStmC : public OrbitRmat33
{
public:

   OrbitStmC(const std::string &name = "", GmatBase *obj = NULL);
   OrbitStmC(const OrbitStmC &copy);
   OrbitStmC& operator=(const OrbitStmC &right);
   virtual ~OrbitStmC();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};


//==============================================================================
//                              OrbitStmD
//==============================================================================
class GMAT_API OrbitStmD : public OrbitRmat33
{
public:

   OrbitStmD(const std::string &name = "", GmatBase *obj = NULL);
   OrbitStmD(const OrbitStmD &copy);
   OrbitStmD& operator=(const OrbitStmD &right);
   virtual ~OrbitStmD();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

#endif //OrbitStmParameters_hpp
