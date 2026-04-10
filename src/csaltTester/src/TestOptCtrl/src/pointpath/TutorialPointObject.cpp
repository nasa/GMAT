//------------------------------------------------------------------------------
//                         TutorialPointObject
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
// Author: Noble Hatten
// Created: 2019/07/03
//
/**
 * Developed based on BrachistichronePointObject.cpp
 */
 //------------------------------------------------------------------------------

#include "TutorialPointObject.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_Tutorial_POINT

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

TutorialPointObject::TutorialPointObject() :
	UserPointFunction()
{
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
TutorialPointObject::~TutorialPointObject()
{
}

//------------------------------------------------------------------------------
// void EvaluateFunctions()
//-----------------------------------------------------------------------------
void TutorialPointObject::EvaluateFunctions()
{
	// Extract parameter data
	Rvector stateInit = GetInitialStateVector(0);
	Rvector stateFinal = GetFinalStateVector(0);
	Real tInit = GetInitialTime(0);
	Real tFinal = GetFinalTime(0);
	Real xInit = stateInit(0);
	Real yInit = stateInit(1);
	Real vInit = stateInit(2);
	Real xFinal = stateFinal(0);
	Real yFinal = stateFinal(1);
	Real vFinal = stateFinal(2);

	// Dimension the function array and bounds arrays
	Integer numFunctions = 8;
	Rvector funcValues(numFunctions);
	Rvector lowerBounds(numFunctions);
	Rvector upperBounds(numFunctions);

	// Initial Time Constraint: t_0 = 0;
	funcValues(0) = tInit;
	lowerBounds(0) = 0.0;
	upperBounds(0) = 0.0;
	// Constraint on intial x value: x(0) = 0.0;
	funcValues(1) = xInit;
	lowerBounds(1) = 0.0;
	upperBounds(1) = 0.0;
	// Constraint on initial y value : y(0) = 0.0;
	funcValues(2) = yInit;
	lowerBounds(2) = 0.0;
	upperBounds(2) = 0.0;
	// Constraint on initial v value : v(0) = 0.0;
	funcValues(3) = vInit;
	lowerBounds(3) = 0.0;
	upperBounds(3) = 0.0;
	// Final time constraint : 0.0 <= t_F = 10;
	funcValues(4) = tFinal;
	lowerBounds(4) = 0;
	upperBounds(4) = 10.00;
	// Constraint on final x value : x(t_f) = 1.0;
	funcValues(5) = xFinal;
	lowerBounds(5) = 1.0;
	upperBounds(5) = 1.0;
	// Constraint on final y value; -10 <= y(t_f) <= 10
	funcValues(6) = yFinal;
	lowerBounds(6) = -10.0;
	upperBounds(6) = 10.0;
	// Constraint of final v value: -10 <= v(t_f) <= 0.0
	funcValues(7) = vFinal;
	lowerBounds(7) = -10.0;
	upperBounds(7) = 0.0;

	// Set the cost and constraint functions
	Rvector costFunc(1, tFinal);
	SetFunctions(COST, costFunc);
	SetFunctions(ALGEBRAIC, funcValues);

	// If initializing, set the bounds
	//if (isInitializing)
	//{
		SetFunctionBounds(ALGEBRAIC, LOWER, lowerBounds);
		SetFunctionBounds(ALGEBRAIC, UPPER, upperBounds);
	//}
}

//------------------------------------------------------------------------------
// void EvaluateJacobians()
//-----------------------------------------------------------------------------
void TutorialPointObject::EvaluateJacobians()
{
	// Currently does nothing
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
