//$Id$
//------------------------------------------------------------------------------
//                             File: EnvParameters.hpp
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/29
//
/**
 * Declares ODEModel based acceleration parameter class.
 */
//------------------------------------------------------------------------------
#ifndef FMAcceleration_hpp
#define FMAcceleration_hpp

#include "newparameter_defs.hpp"
#include "OdeReal.hpp"

//==============================================================================
//                              FMAcceleration
//==============================================================================
/**
 * Declares atmospheric density parameter class.
 */
//------------------------------------------------------------------------------

class NEW_PARAMETER_API FMAcceleration : public OdeReal
{
public:

   FMAcceleration(const std::string &name = "", GmatBase *obj = NULL);
   FMAcceleration(const FMAcceleration &copy);
   const FMAcceleration& operator=(const FMAcceleration &right);
   virtual ~FMAcceleration();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;
   
protected:

};

class NEW_PARAMETER_API FMAccelerationX : public OdeReal
{
public:

   FMAccelerationX(const std::string &name = "", GmatBase *obj = NULL);
   FMAccelerationX(const FMAccelerationX &copy);
   const FMAccelerationX& operator=(const FMAccelerationX &right);
   virtual ~FMAccelerationX();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class NEW_PARAMETER_API FMAccelerationY : public OdeReal
{
public:

   FMAccelerationY(const std::string &name = "", GmatBase *obj = NULL);
   FMAccelerationY(const FMAccelerationY &copy);
   const FMAccelerationY& operator=(const FMAccelerationY &right);
   virtual ~FMAccelerationY();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

class NEW_PARAMETER_API FMAccelerationZ : public OdeReal
{
public:

   FMAccelerationZ(const std::string &name = "", GmatBase *obj = NULL);
   FMAccelerationZ(const FMAccelerationZ &copy);
   const FMAccelerationZ& operator=(const FMAccelerationZ &right);
   virtual ~FMAccelerationZ();

   // methods inherited from Parameter
   virtual bool Evaluate();

   // methods inherited from GmatBase
   virtual GmatBase* Clone() const;

protected:

};

#endif //FMAcceleration_hpp
