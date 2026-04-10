//$Id$
//------------------------------------------------------------------------------
//                            File: BurnParameters.hpp
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
// Author: Linda Jun
// Created: 2005/05/27
//
/**
 * Declares Burn related parameter classes.
 *   BurnElements, TotalMassFlowRate, TotalAcceleration1/2/3, TotalThrust1/2/3
 */
//------------------------------------------------------------------------------
#ifndef BurnParameters_hpp
#define BurnParameters_hpp

#include "gmatdefs.hpp"
#include "BurnReal.hpp"


//==============================================================================
//                               ImpBurnElements
//==============================================================================
/**
 * Declares Burn related Parameter classes.
 *    ImpBurnElements
 */
//------------------------------------------------------------------------------

class GMAT_API ImpBurnElements : public BurnReal
{
public:

   ImpBurnElements(const std::string &type = "",
                   const std::string &name = "",
                   GmatBase *obj = NULL);
   ImpBurnElements(const ImpBurnElements &copy);
   ImpBurnElements& operator=(const ImpBurnElements &right);
   virtual ~ImpBurnElements();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   Integer mElementId;
};


//==============================================================================
//                               TotalMassFlowRate
//==============================================================================
/**
 * Declares Burn related Parameter classes.
 *    TotalMassFlowRate
 */
//------------------------------------------------------------------------------

class GMAT_API TotalMassFlowRate : public BurnReal
{
public:

   TotalMassFlowRate(const std::string &type = "",
                     const std::string &name = "",
                     GmatBase *obj = NULL);
   TotalMassFlowRate(const TotalMassFlowRate &copy);
   TotalMassFlowRate& operator=(const TotalMassFlowRate &right);
   virtual ~TotalMassFlowRate();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   
};


//==============================================================================
//                               TotalAcceleration
//==============================================================================
/**
 * Declares Burn related Parameter classes.
 *    TotalAcceleration
 */
//------------------------------------------------------------------------------

class GMAT_API TotalAcceleration : public BurnReal
{
public:

   TotalAcceleration(const std::string &type = "",
                     const std::string &name = "",
                     GmatBase *obj = NULL);
   TotalAcceleration(const TotalAcceleration &copy);
   TotalAcceleration& operator=(const TotalAcceleration &right);
   virtual ~TotalAcceleration();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   Integer mTotalAccelerationId;
};


//==============================================================================
//                               TotalThrust
//==============================================================================
/**
 * Declares Burn related Parameter classes.
 *    TotalThrust
 */
//------------------------------------------------------------------------------

class GMAT_API TotalThrust : public BurnReal
{
public:

   TotalThrust(const std::string &type = "",
               const std::string &name = "",
               GmatBase *obj = NULL);
   TotalThrust(const TotalThrust &copy);
   TotalThrust& operator=(const TotalThrust &right);
   virtual ~TotalThrust();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   Integer mTotalThrustId;
};


#endif // BurnParameters_hpp
