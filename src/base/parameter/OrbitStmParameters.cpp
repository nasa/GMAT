//$Id$
//------------------------------------------------------------------------------
//                            File: OrbitStmParameters
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
// Created: 2009.03.30
//
/**
 * Implements orbit state transition related parameter classes.
 *    OrbitSTM, OrbitStmA, OrbitStmB, OrbitStmC, OrbitStmD
 */
//------------------------------------------------------------------------------

#include "OrbitStmParameters.hpp"
#include "ColorTypes.hpp"


// To use preset colors, uncomment this line:
//#define USE_PREDEFINED_COLORS

//==============================================================================
//                              OrbitStm
//==============================================================================
/**
 * Implements OrbitStm class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// OrbitStm(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
OrbitStm::OrbitStm(const std::string &name, GmatBase *obj)
   : OrbitRmat66(name, "OrbitSTM", obj, "Orbit STM", "", GmatParam::NO_DEP,
         true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::RED32;
   #endif
}


//------------------------------------------------------------------------------
// OrbitStm(const OrbitStm &copy)
//------------------------------------------------------------------------------
OrbitStm::OrbitStm(const OrbitStm &copy)
   : OrbitRmat66(copy)
{
}


//------------------------------------------------------------------------------
// OrbitStm& operator=(const OrbitStm &right)
//------------------------------------------------------------------------------
OrbitStm& OrbitStm::operator=(const OrbitStm &right)
{
   if (this != &right)
      OrbitRmat66::operator=(right);
   
   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitStm()
//------------------------------------------------------------------------------
OrbitStm::~OrbitStm()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool OrbitStm::Evaluate()
{
   mRmat66Value = OrbitData::GetStmRmat66(ORBIT_STM);
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* OrbitStm::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* OrbitStm::Clone(void) const
{
   return new OrbitStm(*this);
}


//==============================================================================
//                              OrbitStmA
//==============================================================================
/**
 * Implements OrbitStmA class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// OrbitStmA(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
OrbitStmA::OrbitStmA(const std::string &name, GmatBase *obj)
   : OrbitRmat33(name, "OrbitSTMA", obj, "Orbit STM_A", "", GmatParam::NO_DEP, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::YELLOW32;
   #endif
}


//------------------------------------------------------------------------------
// OrbitStmA(const OrbitStmA &copy)
//------------------------------------------------------------------------------
OrbitStmA::OrbitStmA(const OrbitStmA &copy)
   : OrbitRmat33(copy)
{
}


//------------------------------------------------------------------------------
// OrbitStmA& operator=(const OrbitStmA &right)
//------------------------------------------------------------------------------
OrbitStmA& OrbitStmA::operator=(const OrbitStmA &right)
{
   if (this != &right)
      OrbitRmat33::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitStmA()
//------------------------------------------------------------------------------
OrbitStmA::~OrbitStmA()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool OrbitStmA::Evaluate()
{
   mRmat33Value = OrbitData::GetStmRmat33(ORBIT_STM_A);
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* OrbitStmA::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* OrbitStmA::Clone(void) const
{
   return new OrbitStmA(*this);
}


//==============================================================================
//                              OrbitStmB
//==============================================================================
/**
 * Implements OrbitStmB class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// OrbitStmB(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
OrbitStmB::OrbitStmB(const std::string &name, GmatBase *obj)
   : OrbitRmat33(name, "OrbitSTMB", obj, "Orbit STM_B", "", GmatParam::NO_DEP, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::BLUE32;
   #endif
}


//------------------------------------------------------------------------------
// OrbitStmB(const OrbitStmB &copy)
//------------------------------------------------------------------------------
OrbitStmB::OrbitStmB(const OrbitStmB &copy)
   : OrbitRmat33(copy)
{
}


//------------------------------------------------------------------------------
// OrbitStmB& operator=(const OrbitStmB &right)
//------------------------------------------------------------------------------
OrbitStmB& OrbitStmB::operator=(const OrbitStmB &right)
{
   if (this != &right)
      OrbitRmat33::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitStmB()
//------------------------------------------------------------------------------
OrbitStmB::~OrbitStmB()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool OrbitStmB::Evaluate()
{
   mRmat33Value = OrbitData::GetStmRmat33(ORBIT_STM_B);
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* OrbitStmB::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* OrbitStmB::Clone(void) const
{
   return new OrbitStmB(*this);
}


//==============================================================================
//                              OrbitStmC
//==============================================================================
/**
 * Implements OrbitStmC class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// OrbitStmC(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
OrbitStmC::OrbitStmC(const std::string &name, GmatBase *obj)
   : OrbitRmat33(name, "OrbitSTMC", obj, "Orbit STM_C", "", GmatParam::NO_DEP, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::GREEN32;
   #endif
}


//------------------------------------------------------------------------------
// OrbitStmC(const OrbitStmC &copy)
//------------------------------------------------------------------------------
OrbitStmC::OrbitStmC(const OrbitStmC &copy)
   : OrbitRmat33(copy)
{
}


//------------------------------------------------------------------------------
// OrbitStmC& operator=(const OrbitStmC &right)
//------------------------------------------------------------------------------
OrbitStmC& OrbitStmC::operator=(const OrbitStmC &right)
{
   if (this != &right)
      OrbitRmat33::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitStmC()
//------------------------------------------------------------------------------
OrbitStmC::~OrbitStmC()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool OrbitStmC::Evaluate()
{
   mRmat33Value = OrbitData::GetStmRmat33(ORBIT_STM_C);
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* OrbitStmC::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* OrbitStmC::Clone(void) const
{
   return new OrbitStmC(*this);
}


//==============================================================================
//                              OrbitStmD
//==============================================================================
/**
 * Implements OrbitStmD class.
 */
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// OrbitStmD(const std::string &name, GmatBase *obj)
//------------------------------------------------------------------------------
OrbitStmD::OrbitStmD(const std::string &name, GmatBase *obj)
   : OrbitRmat33(name, "OrbitSTMD", obj, "Orbit STM_D", "", GmatParam::NO_DEP, true)
{
   mDepObjectName = "EarthMJ2000Eq";
   SetRefObjectName(Gmat::COORDINATE_SYSTEM, mDepObjectName);
   #ifdef USE_PREDEFINED_COLORS
      mColor = GmatColor::ORANGE32;
   #endif
}


//------------------------------------------------------------------------------
// OrbitStmD(const OrbitStmD &copy)
//------------------------------------------------------------------------------
OrbitStmD::OrbitStmD(const OrbitStmD &copy)
   : OrbitRmat33(copy)
{
}


//------------------------------------------------------------------------------
// OrbitStmD& operator=(const OrbitStmD &right)
//------------------------------------------------------------------------------
OrbitStmD& OrbitStmD::operator=(const OrbitStmD &right)
{
   if (this != &right)
      OrbitRmat33::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitStmD()
//------------------------------------------------------------------------------
OrbitStmD::~OrbitStmD()
{
}


//------------------------------------------------------------------------------
// bool Evaluate()
//------------------------------------------------------------------------------
bool OrbitStmD::Evaluate()
{
   mRmat33Value = OrbitData::GetStmRmat33(ORBIT_STM_D);
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* OrbitStmD::Clone(void) const
//------------------------------------------------------------------------------
GmatBase* OrbitStmD::Clone(void) const
{
   return new OrbitStmD(*this);
}

