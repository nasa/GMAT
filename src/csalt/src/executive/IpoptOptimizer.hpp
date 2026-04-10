//------------------------------------------------------------------------------
//                              IpoptOptimizer
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
// Author: Jeremy Knittel / NASA
// Created: 2016.08.09 
//
/**
 * From original MATLAB prototype:
 * Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef IpoptOptimizer_hpp
#define IpoptOptimizer_hpp

#define SNOPT_INTEGER int
#define SNOPT_DOUBLE double

#include "Trajectory.hpp"
#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "snoptProblem.hpp"
#include "snopt.h"
#include "SNOPTFunctionWrapper.hpp"
#include "SparseMatrixUtil.hpp"

//class Trajectory;

class IpoptOptimizer
{
public:
	
	// IpoptOptimizer();
	IpoptOptimizer(Trajectory * trajectory_in);
	
	IpoptOptimizer(const IpoptOptimizer &copy);
	
	IpoptOptimizer& operator=(const IpoptOptimizer &copy);
	
	virtual ~IpoptOptimizer();
	
	virtual void Initialize();
	
	virtual void SetScaling(bool ifScaling);

	virtual void SetIpoptOptimizerOutputFile(const std::string &optFile);

	virtual void Optimize(Rvector        &decVec,
                         const Rvector  &decVecLB,
                         const Rvector  &decVecUB,
                         const Rvector  &funLB,
                         const Rvector  &funUB,
                         const RSMatrix &spMat,
                         Rvector        &F,
                         Rvector        &xmul,
                         Rvector        &Fmul);
				
	// TODO: Should the following things be made protected 
	// or private with accessor functions?
   // WCS - for performance we can leave it as is for now
							  
  	// Pointer to the Owner class
  	Trajectory * traj;

	// Jacobian sparsity pattern indices
	std::vector<SNOPT_INTEGER> iGfun;
	std::vector<SNOPT_INTEGER> jGvar;

protected:
		
	// SNOPT problem object
	snoptProblemA Problem;
};

#endif // IpoptOptimizer_hpp
