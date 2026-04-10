//------------------------------------------------------------------------------
//                               TestOptimizer
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
// Author: Jeremy Knittel
// Created: 2016.08.25
//
/**
 * Test driver for Optimizer class and its interfaces.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "Rvector.hpp"
#include "gmatdefs.hpp"
#include "Optimizer.hpp"
#include "SparseMatrixUtil.hpp"

using namespace std;
using namespace GmatMathConstants;
using namespace GmatMathUtil;

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   
	Optimizer Opt = Optimizer();
	
	Opt.Initialize();
    Opt.SetScaling(0);
	
	Rvector decVec(1,10.0);
	Rvector decVecLB(1,-100.0);
	Rvector decVecUB(1,100.0);
	Rvector funLB(2,-200.0,-200.0);
	Rvector funUB(2,200.0,0.0);
	Rvector F(2,0.0,0.0);
	Rvector xmul(1,0.0,0.0);
	Rvector Fmul(2,0.0,0.0);
	
	RSMatrix spMat;
	SparseMatrixUtil::SetSize(spMat,2,1);
	SparseMatrixUtil::SetElement(spMat,0,0,1.0);
	SparseMatrixUtil::SetElement(spMat,1,0,1.0);
	
	Opt.Optimize(decVec,
			     decVecLB,
			  	 decVecUB,
			 	 funLB,
			 	 funUB,
				 spMat,
				 F,
			 	 xmul,	
			 	 Fmul);
				 
	cout<<"Optimal value is "<<F[0]<<endl;
	cout<<"With a decision variable of "<<decVec[0]<<endl;

}

