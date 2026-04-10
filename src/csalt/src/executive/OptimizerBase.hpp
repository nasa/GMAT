//------------------------------------------------------------------------------
//                              OptimizerBase
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
// Author: Youngkwang Kim / Yonsei University
// Created: 2017.09.28
//
/**
* the base class for SNOPT/IPOPT optimizer classes
*/
//------------------------------------------------------------------------------

#ifndef OptimizerBase_hpp
#define OptimizerBase_hpp


#include "Trajectory.hpp"
#include "csaltdefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "SparseMatrixUtil.hpp"

//class Trajectory;

class OptimizerBase
{
public:
	
	// OptimizerBase();
	OptimizerBase(Trajectory * trajectory_in);
	
	OptimizerBase(const OptimizerBase &copy);
	
	OptimizerBase& operator=(const OptimizerBase &copy);
	
	virtual ~OptimizerBase();
	
	virtual void Initialize() = 0;
	
	virtual void SetScaling(bool ifScaling) = 0;

	virtual void SetOptimizerOutputFile(const std::string &optFile) = 0;

	virtual void Optimize(Rvector        &decVec,
                         const Rvector  &decVecLB,
                         const Rvector  &decVecUB,
                         const Rvector  &funLB,
                         const Rvector  &funUB,
                         const RSMatrix &spMat,
                         Rvector        &F,
                         Rvector        &xmul,
                         Rvector        &Fmul) = 0;
				
	// TODO: Should the following things be made protected 
	// or private with accessor functions?
   // WCS - for performance we can leave it as is for now
							  
  	// Pointer to the Owner class
  	Trajectory * traj;

protected:
		
};

#endif // OptimizerBase_hpp
