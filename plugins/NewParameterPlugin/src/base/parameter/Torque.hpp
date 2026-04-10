//$Id$
//------------------------------------------------------------------------------
//                              Torque
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: Nov 16, 2021
//
/**
 * Declares the Torque classes.
 */
//------------------------------------------------------------------------------
#ifndef Torque_hpp
#define Torque_hpp

#include "newparameter_defs.hpp"
#include "OdeRvec3.hpp"

class NEW_PARAMETER_API BurnTorque : public OdeRvec3
{
public:
   BurnTorque(const std::string &name = "", GmatBase *obj = NULL);
   BurnTorque(const BurnTorque &copy);
   const BurnTorque& operator=(const BurnTorque& right);
   virtual ~BurnTorque();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

   // methods inherited from GmatBase
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;

protected:
   enum
   {
      VECTOR_SIZE = OdeDataObjectCount,
      TotalTorqueParamCount
   };

};

class NEW_PARAMETER_API GravityTorque : public OdeRvec3
{
public:
   GravityTorque(const std::string &name = "", GmatBase *obj = NULL);
   GravityTorque(const GravityTorque &copy);
   const GravityTorque& operator=(const GravityTorque& right);
   virtual ~GravityTorque();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

   // methods inherited from GmatBase
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;

protected:
   enum
   {
      VECTOR_SIZE = OdeDataObjectCount,
      TotalTorqueParamCount
   };

};

class NEW_PARAMETER_API SRPTorque : public OdeRvec3
{
public:
   SRPTorque(const std::string &name = "", GmatBase *obj = NULL);
   SRPTorque(const SRPTorque &copy);
   const SRPTorque& operator=(const SRPTorque& right);
   virtual ~SRPTorque();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

   // methods inherited from GmatBase
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;

protected:
   enum
   {
      VECTOR_SIZE = OdeDataObjectCount,
      TotalTorqueParamCount
   };

};

class NEW_PARAMETER_API TotalTorque : public OdeRvec3
{
public:
   TotalTorque(const std::string &name = "", GmatBase *obj = NULL);
   TotalTorque(const TotalTorque &copy);
   const TotalTorque& operator=(const TotalTorque& right);
   virtual ~TotalTorque();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

   // methods inherited from GmatBase
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;

protected:
   enum
   {
      VECTOR_SIZE = OdeDataObjectCount,
      TotalTorqueParamCount
   };

};

#endif
