//------------------------------------------------------------------------------
//                              SnoptOptimizer
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

#ifndef SnoptOptimizer_hpp
#define SnoptOptimizer_hpp

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

class SnoptOptimizer
{
public:
	
	// SnoptOptimizer();
	SnoptOptimizer(Trajectory * trajectory_in);
	
	SnoptOptimizer(const SnoptOptimizer &copy);
	
	SnoptOptimizer& operator=(const SnoptOptimizer &copy);
	
	virtual ~SnoptOptimizer();
	
	virtual void Initialize();
	
	virtual void SetScaling(bool ifScaling);

	virtual void SetOptimizerOutputFile(const std::string &optFile);
   
   /// @todo - when we use OptimizerBase as the base class, should these be
   /// moved there?
   virtual void SetFeasibilityTolerance(Real feasTol);
   virtual void SetMajorOptimalityTolerance(Real optTol);
   virtual void SetMajorIterationsLimit(Integer majorIterLimit);
   virtual void SetTotalIterationsLimit(Integer totalIterLimit);
   virtual void SetOptimizationMode(const std::string &optMode);
   virtual void SetSnoptConsoleOutputLevel(const Integer level);
   virtual void SetCurrentIterationData(Integer iterCount,
      Integer majorIterCount, Real objValue);
   


	virtual void Optimize(Rvector        &decVec,
                         const Rvector  &decVecLB,
                         const Rvector  &decVecUB,
                         const Rvector  &funLB,
                         const Rvector  &funUB,
                         const RSMatrix &spMat,
                         Rvector        &F,
                         Rvector        &xmul,
                         Rvector        &Fmul,
                         Integer        &exitFlag);
				
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

   bool stopOptimization;

   /// The total number of iterations taken by the optimizer
   Integer totalNumIter;
   /// The total number of major iterations taken by the optimizer
   Integer totalNumMajorIter;
   /// The final value of the objective function from the optimizer
   Real objFinalVal;
   
//   Real        feasibilityTol;
//   Real        majorOptimalityTol;
//   Integer     majorIterationLimit;
//   std::string optimizationMode;
   

};

#endif // SnoptOptimizer_hpp
