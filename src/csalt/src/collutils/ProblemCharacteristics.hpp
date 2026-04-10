//------------------------------------------------------------------------------
//                         Problem Characteristics
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Youngkwang Kim
// Created: 2016/02/12
//
/**
 * Definition for the ProblemCharacteristics class
 */
//------------------------------------------------------------------------------
#ifndef ProblemCharacteristics_hpp
#define ProblemCharacteristics_hpp

#include "csaltdefs.hpp"
#include "GmatConstants.hpp"
#include "Rvector.hpp"

/**
 * Problem Characteristics class
 */
class ProblemCharacteristics 
{
public:
   /// default constructor
   ProblemCharacteristics();
   
   /// copy constructor
   ProblemCharacteristics(const ProblemCharacteristics &prb);
   
   /// assignment operator
   ProblemCharacteristics&   operator=(const ProblemCharacteristics &prb);
   
   /// default destructor
   virtual ~ProblemCharacteristics();

   /// ===== Optimal control parameter dimensions ==============================
   void    SetNumStateVars(Integer nVars);
   void    SetNumControlVars(Integer nVars);
   void    SetNumIntegralVars(Integer nVars);
   void    SetNumStaticVars(Integer nVars);

   Integer GetNumStateVars();
   Integer GetNumControlVars();
   Integer GetNumIntegralVars();
   Integer GetNumStaticVars();
   Integer GetNumTimeVars();

   bool    HasStateVars();
   bool    HasControlVars();
   bool    HasIntegralVars();
   bool    HasStaticVars();

   
   /// NLP parameter dimensions ================================================

   void    SetNumStateVarsNLP(Integer nVarsNLP);
   void    SetNumControlVarsNLP(Integer nVarsNLP);
   void    SetNumDecisionVarsNLP(Integer nVarsNLP);
   /// num time vars NLP must be two always, so no set method
   void    SetNumDefectConNLP(Integer nDefectConNLP);
   void    SetNumTotalConNLP(Integer nTotalConNLP);

   Integer GetNumStateVarsNLP();
   Integer GetNumControlVarsNLP();
   Integer GetNumTimeVarsNLP();
   Integer GetNumDecisionVarsNLP();
   Integer GetNumDefectConNLP();
   Integer GetNumTotalConNLP();

   /// ===== Get\Set for optimal control function types
   void    SetHasDefectCons(bool hasCons);
   void    SetHasAlgPathCons(bool hasCons);
   void    SetHasIntegralCons(bool hasCons);
   void    SetHasIntegralCost(bool hasCost);
   void    SetHasAlgebraicCost(bool hasCost);

   bool    HasDefectCons();
   bool    HasAlgPathCons();
   bool    HasIntegralCons();
   bool    HasIntegralCost();
   bool    HasAlgebraicCost();

   /// ===== State vector related quantities
   void    SetStateLowerBound(const Rvector &boundVec);
   void    SetStateUpperBound(const Rvector &bound);
   void    SetStateInitialGuess(const Rvector &initGuess);
   void    SetStateFinalGuess(const Rvector &finalGuess);

   Rvector GetStateLowerBound();
   Rvector GetStateUpperBound();
   Rvector GetStateInitialGuess();
   Rvector GetStateFinalGuess();

   void    ValidateStateProperties();
   
   /// ===== Control related quantities
   void    SetControlLowerBound(const Rvector &bound);
   void    SetControlUpperBound(const Rvector &bound);

   Rvector GetControlLowerBound();
   Rvector GetControlUpperBound();

   void    ValidateControlProperties();
   
   /// === Discretization related quantities
   void    SetMeshIntervalFractions(const Rvector &fractions);
   void    SetMeshIntervalNumPoints(IntegerArray numPoints);

   Rvector GetMeshIntervalFractions();
   IntegerArray GetMeshIntervalNumPoints();
   Integer GetNumMeshIntervals();

   Integer GetNumPointsInMeshInterval(Integer mIdx);
   Real    GetMeshIntervalFraction(Integer meshIdx);

   void    ValidateMeshConfig();
   
   /// ===== Time vector related quantities
   void    SetTimeLowerBound(Real bound);
   void    SetTimeUpperBound(Real bound);
   void    SetTimeInitialGuess(Real initGuess);
   void    SetTimeFinalGuess(Real finalGuess);

   Real    GetTimeLowerBound();
   Real    GetTimeUpperBound();
   Real    GetTimeInitialGuess();
   Real    GetTimeFinalGuess();
	        
   void    ValidateTimeProperties();
   
   /// ===== Static parameter related quantities; // YK mod static params
   void    SetStaticLowerBound(const Rvector &boundVec);
   void    SetStaticUpperBound(const Rvector &bound);
   void    SetStaticVector(const Rvector &guess);

   Rvector GetStaticLowerBound();
   Rvector GetStaticUpperBound();
   Rvector GetStaticVector();

   void    ValidateStaticProperties();

protected:      
   // Parameters
   
   /// ===== Optimal control parameter dimensions

   /// The number of state variables in opt. control prob.
   Integer   numStateVars;
   /// The number of control variables in opt. control prob.
   Integer   numControlVars;
   /// The number of integral variables in opt. control prob.
   Integer   numIntegralVars;
   /// The number of static variables in opt. control prob.
   Integer   numStaticVars;
   /// The number of time variables in opt. control prob.
   Integer   numTimeVars;

   /// ===== NLP parameter dimensions

   /// The number of state variables in the NLP problem
   Integer   numStateVarsNLP;
   /// The number of time variables in the NLP problem
   Integer   numControlVarsNLP;
   /// The total number of decision vars in the NLP problem
   Integer   numTimeVarsNLP;
   /// The number of control variables in the NLP problem
   Integer   numDecisionVarsNLP;
   /// Number of defect constraints in the NLP problem
   Integer   numDefectConNLP;
   /// Number of total contraints in NLP problem
   Integer   numTotalConNLP;


   /// ===== Number of variables by type in optimal control problem
        
   /// Indicates whether phase has state variables
   bool      hasStateVars;
   /// Indicates whether phase has control variables
   bool      hasControlVars;
   /// Indicates whether phase has integral variables
   bool      hasIntegralVars;
   /// Indicates whether phase has static variables
   bool      hasStaticVars;
   /// Indicates whether phase has time variables
   bool      hasTimeVars;
        
   // ===== Flags describing function types in optimal control problem
        
   /// Indicates whether phase has defect constraints
   bool      hasDefectCons;
   /// Indicates whether a phase has integral constraints
   bool      hasIntegralCons;
   /// Indicates whether phase has algebraic path constraints
   bool      hasAlgPathCons;
   /// Flag to indicate if phase has a cost function
   bool      hasIntegralCost;
   /// Flag indicating there is an algebraic term to cost
   bool      hasAlgebraicCost;
        
   /// ==== Discretization properties
   /// RVector defining the number of mesh intervalas and
   /// how the phase is partioned among them
   /// For example, [-1 -.5 0 0.5 1] (for Radau) means there 4 equally
   /// sized segements in the phase.
   /// For HS, [0 0.5 1] means there are two equally sized segments in
   /// the phase.
   Rvector      meshIntervalFractions;
   /// Integer Array. Row number of control points per mesh interval
   IntegerArray meshIntervalNumPoints;
   /// Integer.  Number of mesh intervals
   Integer      numMeshIntervals;
        
   /// ==== State related quantities
        
   /// Lower bound on state vector
   Rvector      stateLowerBound;
   /// Upper bound on state vector
   Rvector      stateUpperBound;
   /// State guess at initial time
   Rvector      stateInitialGuess;
   /// State guess at final time
   Rvector      stateFinalGuess;
   /// Lower bound on time
   Real         timeLowerBound;
   /// Upper bound on time
   Real         timeUpperBound;
   /// Time guess at beginning of phase
   Real         timeInitialGuess;
   /// Time guess at end of phase
   Real         timeFinalGuess;
   
   /// Has the state initial guess been set?
   bool         stateInitialGuessSet;
   /// Has the state final guess been set?
   bool         stateFinalGuessSet;
   /// Has the time initial guess been set?
   bool         timeInitialGuessSet;
   /// Has the time final guess been set?
   bool         timeFinalGuessSet;
   /// Has the static parameter geuss been set?
   bool         staticVectorSet; // YK mod static params; necessary?

   /// ==== Control related quantities
   /// Lower bound on control vector
   Rvector      controlLowerBound;
   /// Upper bound on control vector
   Rvector      controlUpperBound;
   
   /// ==== Static params related quantities; YK mod static params
   /// Lower bound on static vector
   Rvector      staticLowerBound;
   /// Upper bound on static vector
   Rvector      staticUpperBound;
   /// Static vector
   Rvector      staticVector;

   /// Tolerance on bounds
   static const Real boundTolerance;

   /// Copies the data from the input object
   void CopyData(const ProblemCharacteristics &prb);

};

#endif // ProblemCharacteristics_hpp
