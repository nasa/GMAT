//------------------------------------------------------------------------------
//                         HohmannTransferPathObject
//------------------------------------------------------------------------------
// CSALT: Collocation Stand Alone Library and Toolkit
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: Mar 20, 2018
//------------------------------------------------------------------------------

#include "HohmannTransferPathObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_HohmannTransfer_PATH

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------

HohmannTransferPathObject::HohmannTransferPathObject() :
   UserPathFunction()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
HohmannTransferPathObject::HohmannTransferPathObject(const HohmannTransferPathObject &copy) :
   UserPathFunction(copy)
{
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
HohmannTransferPathObject& HohmannTransferPathObject::operator=(const HohmannTransferPathObject &copy)
{
   if (&copy == this)
      return *this;
   
   UserPathFunction::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
HohmannTransferPathObject::~HohmannTransferPathObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void HohmannTransferPathObject::EvaluateFunctions()
{

   // mu = 1.0, mass = 1.0

   //Extract parameter data
   Rvector stateVec = GetStateVector();

   Real r = stateVec(0);
   Real vr = stateVec(2);
   Real vt = stateVec(3);

#ifdef DEBUG_PATH_FUNCTION
   MessageInterface::ShowMessage("[%lf %lf %lf %lf %lf] -> ", r, vr, vt, A, B);
#endif
   Real dynFunction2 = vt / r;
   Real dynFunction3 = (vt*vt) / r - 1.0 / (r*r) ;
   Real dynFunction4 = -(vr*(vt / r)) ;

   Rvector dynVector(4, vr, dynFunction2, dynFunction3, dynFunction4);

#ifdef DEBUG_PATH_FUNCTION
   MessageInterface::ShowMessage("DynFun = %s\n", dynVector.ToString(15).c_str());
#endif

   SetFunctions(DYNAMICS, dynVector);
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void HohmannTransferPathObject::EvaluateJacobians()
{
   //Extract parameter data
   Rvector stateVec = GetStateVector();

   Real r = stateVec(0);
   Real vr = stateVec(2);
   Real vt = stateVec(3);

   //TODO: Figure out how to do this DynStateJac
   Rmatrix dynStateJac(4, 4);

   dynStateJac(0, 0) = 0.0;
   dynStateJac(0, 1) = 0.0;
   dynStateJac(0, 2) = 1.0;
   dynStateJac(0, 3) = 0.0;

   dynStateJac(1, 0) = -vt / (r*r); //(vr*vr);
   dynStateJac(1, 1) = 0.0;
   dynStateJac(1, 2) = 0.0;
   dynStateJac(1, 3) = 1.0 / r;

   dynStateJac(2, 0) = 2.0 / (r*r*r) - (vt*vt) / (r*r);
   dynStateJac(2, 1) = 0.0;
   dynStateJac(2, 2) = 0.0;
   dynStateJac(2, 3) = 2.0*vt / r;

   dynStateJac(3, 0) = (vr*vt) / (r*r);
   dynStateJac(3, 1) = 0.0;
   dynStateJac(3, 2) = -vt / r;
   dynStateJac(3, 3) = -vr / r;


   Rmatrix dynTimeJac(4, 1);
   dynTimeJac(0, 0) = 0.0;
   dynTimeJac(1, 0) = 0.0;
   dynTimeJac(2, 0) = 0.0;
   dynTimeJac(3, 0) = 0.0;

   SetJacobian(DYNAMICS, STATE, dynStateJac);
   SetJacobian(DYNAMICS, TIME, dynTimeJac);

#ifdef DEBUG_PATH_JACOBIANS
   MessageInterface::ShowMessage("SJac = %s\n", dynStateJac.ToString(15).c_str());
   MessageInterface::ShowMessage("\nCJac = %s\n", dynControlJac.ToString(15).c_str());
   MessageInterface::ShowMessage("\nTJac = %s\n\n\n\n", dynTimeJac.ToString(15).c_str());
#endif
}



//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
