//$Id$
//------------------------------------------------------------------------------
//                              Jacobian
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
// number NNG06CA54C
//
// Created: 2008/03/28
//
/**
 * Base class for Jacobian calculations used by the Solvers.
 */
//------------------------------------------------------------------------------

#ifndef Jacobian_hpp
#define Jacobian_hpp

#include "DerivativeModel.hpp"

/**
 * Implements Jacobian calculations using finite differencing in one of three
 * modes: forward differenced, central differenced, or backwards differenced.
 * The class also makes provision for a user specified Jacobian.
 */
class GMAT_API Jacobian : public DerivativeModel
{
public:
	Jacobian();
	virtual ~Jacobian();
	Jacobian(const Jacobian &jac);
	Jacobian&            operator=(const Jacobian &jac);
   
   virtual bool         Initialize(UnsignedInt varCount, 
                                   UnsignedInt componentCount);
   virtual void         Achieved(Integer pertNumber, Integer componentId,
                                 Real dx, Real value, bool plusEffect = true);
   bool                 Calculate(std::vector<Real> &jac);

protected:
   /// The number of dependent parameters in the Jacobian matrix
   UnsignedInt                         numComponents;    
   /// Results from a nominal run, used for forward and backward differencing
   std::vector<Real>                   nominal;
   /// The final Jacobian matrix, collapsed to a single vector for simplicity
   std::vector<Real>                   jacobian;
};

#endif /*Jacobian_hpp*/
