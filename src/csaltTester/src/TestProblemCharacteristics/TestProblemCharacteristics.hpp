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


#include "ProblemCharacteristics.hpp"
#include <iostream>

#ifndef TestProblemCharacteristics_hpp
#define TestProblemCharacteristics_hpp

class TestProblemCharacteristics
{
public:
   static void RunTests()
   {     
      std::cout << "\nProblemCharacteristics Unit-Tester: begins" << std::endl;

      ProblemCharacteristics Config;
	  
      /// ===== Optimal control parameter dimensions
      
      //// The number of state variables in opt. control prob.
      Integer numStateVars = 1;
      Config.SetNumStateVars(numStateVars);

      if (Config.GetNumStateVars() != numStateVars)
	      std::cout << "setting and getting numStateVars test fails" << std::endl;

      //// The number of control variables in opt. control prob.
      Integer  numControlVars = 1;
      Config.SetNumControlVars(numControlVars);

      if (Config.GetNumControlVars() != numControlVars)
	      std::cout << "setting and getting numControlVars test fails" << std::endl;

      //// The number of integral variables in opt. control prob.
      Integer  numIntegralVars = 0;

      Config.SetNumIntegralVars(0);

      if (Config.GetNumIntegralVars() != numIntegralVars)
	      std::cout << "setting and getting numIntegralVars test fails" << std::endl;

      //// The number of static variables in opt. control prob.
      Integer  numStaticVars = 0;

      Config.SetNumStaticVars(numStaticVars);

      if (Config.GetNumStaticVars() != numStaticVars)
	      std::cout << "setting and getting numStaticVars test fails" << std::endl;

      //// The number of time variables in opt. control prob.
      Integer  numTimeVars = 2;

      if (Config.GetNumTimeVars() != numTimeVars)
	      std::cout << "tests fails, invalid number of time variables" << std::endl;

      /// ===== NLP parameter dimensions
   
      //// The number of state variables in the NLP problem
      Integer  numStateVarsNLP = 41;

      Config.SetNumStateVarsNLP(numStateVarsNLP);
      if (Config.GetNumStateVarsNLP() != numStateVarsNLP)
	      std::cout << "setting and getting numStateVarsNLP test fails" << std::endl;

      //// The number of time variables in the NLP problem
      Integer  numTimeVarsNLP = 2;

      if (Config.GetNumTimeVarsNLP() != numTimeVarsNLP)
	      std::cout << "tests fails, invalid number of time NLP variables" << std::endl;

      /// The number of control variables in the NLP problem
      Integer  numControlVarsNLP = 40;
      Config.SetNumControlVarsNLP(numControlVarsNLP);
      
      if (Config.GetNumControlVarsNLP() != numControlVarsNLP)
	      std::cout << "setting and getting numControlVarsNLP test fails" << std::endl;

      //// The total number of decision vars in the NLP problem
      Integer  numDecisionVarsNLP = 83;
      Config.SetNumDecisionVarsNLP(numDecisionVarsNLP);
      
      if (Config.GetNumDecisionVarsNLP() != numDecisionVarsNLP)
	      std::cout << "setting and getting numDecisionVarsNLP test fails" << std::endl;

      //// Number of defect constraints in the NLP problem
      Integer  numDefectConNLP = 40;
      Config.SetNumDefectConNLP(numDefectConNLP);
      
      if (Config.GetNumDefectConNLP() != numDefectConNLP)
	      std::cout << "setting and getting numDefectConNLP test fails" << std::endl;
	  
      //// Number of total contraints in NLP problem
      Integer  numTotalConNLP = 40;
      Config.SetNumTotalConNLP(numTotalConNLP);
      
      if (Config.GetNumTotalConNLP() != numTotalConNLP)
	      std::cout << "setting and getting numTotalCon test fails" << std::endl;

	  
      /// ===== Number of variables by type in optimal control problem
      
      //// Indicates whether phase has state variables
      bool hasStateVars = true; 

      if (Config.HasStateVars() != hasStateVars)
	      std::cout << "setting and getting hasStateVars test fails" << std::endl;

      //// Indicates whether phase has control variables
      bool hasControlVars = true;

      if (Config.HasControlVars() != hasControlVars)
	      std::cout << "setting and getting hasControlVars test fails" << std::endl;

      //// Indicates whether phase has integral variables
      bool hasIntegralVars = false;

      if (Config.HasIntegralVars() != hasIntegralVars)
	      std::cout << "setting and getting hasIntegralVars test fails" << std::endl;

      //// Indicates whether phase has static variables
      bool hasStaticVars = false;
      
      if (Config.HasStaticVars() != hasStaticVars)
	      std::cout << "setting and getting hasStaticVars test fails" << std::endl;

      //// Indicates whether phase has time variables
      bool hasTimeVars = true;
      // recommend to delete hasTimeVars because it is not used now

	  
	  
      // ===== Flags describing function types in optimal control problem
      	
      //// Indicates whether phase has defect constraints
      bool hasDefectCons = true;
      
      Config.SetHasDefectCons(hasDefectCons);
      
      if (Config.HasDefectCons() != hasDefectCons)
	      std::cout << "setting and getting hasDefectCons test fails" << std::endl;

      //// Indicates whether a phase has integral constraints
      bool hasIntegralCons = false;

      Config.SetHasIntegralCons(hasIntegralCons);
      
      if (Config.HasIntegralCons() != hasIntegralCons)
	      std::cout << "setting and getting hasIntegralCons test fails" << std::endl;

      //// Indicates whether phase has algebraic path constraints
      bool hasAlgPathCons =false;
      
      Config.SetHasAlgPathCons(hasAlgPathCons);
      
      if (Config.HasAlgPathCons() != hasAlgPathCons)
	      std::cout << "setting and getting hasAlgPathCons test fails" << std::endl;

      //// Flag to indicate if phase has a cost function
      bool hasIntegralCost = true;

      Config.SetHasIntegralCost(hasIntegralCost);
      
      if (Config.HasIntegralCost() != hasIntegralCost)
	      std::cout << "setting and getting hasIntegralCost test fails" << std::endl;

      //// Flag indicating there is an algebraic term to cost
      bool hasAlgebraicCost = false;
      
      Config.SetHasAlgebraicCost(hasAlgebraicCost);
      
      if (Config.HasAlgebraicCost() != hasAlgebraicCost)
	      std::cout << "setting and getting hasAlgebraicCost test fails" << std::endl;

    

      /// ==== Discretization properties
      //// RVector defining the number of mesh intervalas and
      //// how the phase is partioned among them
      //// For example, [-1 -.5 0 0.5 1] (for Radau) means there 4 equally
      //// sized segements in the phase.
      //// For HS, [0 0.5 1] means there are two equally sized segments in
      //// the phase.
      Rvector meshIntervalFractions;
      meshIntervalFractions.SetSize(5);
      meshIntervalFractions(0) = -1;
      meshIntervalFractions(1) = -0.25;
      meshIntervalFractions(2) = 0;
      meshIntervalFractions(3) = 0.25;
      meshIntervalFractions(4) = 1;		  
      
	  
      Config.SetMeshIntervalFractions(meshIntervalFractions);
	  
	  
      if (Config.GetMeshIntervalFractions() != meshIntervalFractions)
	      std::cout << "setting and getting meshIntervalFractions test fails" << std::endl;


      //// Integer.  Number of mesh intervals
      Integer numMeshIntervals = 4;
      	
      if (Config.GetNumMeshIntervals() != numMeshIntervals)
	      std::cout << "setting and getting numMeshIntervals test fails" << std::endl;

	  

      //// Integer Array. Row number of control points per mesh interval
      IntegerArray meshIntervalNumPoints;
      meshIntervalNumPoints.push_back(5);
      meshIntervalNumPoints.push_back(3);
      meshIntervalNumPoints.push_back(3);
      meshIntervalNumPoints.push_back(5);

	
      Config.SetMeshIntervalNumPoints(meshIntervalNumPoints);

      if (Config.GetMeshIntervalNumPoints() != meshIntervalNumPoints)
	      std::cout << "setting and getting meshIntervalNumPoints test fails" << std::endl;

	   
      	  
      for (Integer meshIdx = 0; meshIdx < numMeshIntervals; ++meshIdx)
      {
	      if (Config.GetNumPointsInMeshInterval(meshIdx) != meshIntervalNumPoints[meshIdx])
               std::cout << "getting numPointsInMeshInterval test fails" << std::endl;

	      if (Config.GetMeshIntervalFraction(meshIdx) != meshIntervalFractions(meshIdx))	
               std::cout << "getting meshIntervalFraction test fails" << std::endl;

      }
	  
      if (Config.GetMeshIntervalFraction(numMeshIntervals) != meshIntervalFractions(numMeshIntervals))	
         std::cout << "getting meshIntervalFraction test fails" << std::endl;
	  
	  
      Config.ValidateMeshConfig();
      
      /// ==== State related quantities
      	
      //// Lower bound on state vector
      Rvector stateLowerBound;
      stateLowerBound.SetSize(1);
      stateLowerBound(0) = -50;
      
      Config.SetStateLowerBound(stateLowerBound);
	  
      if (Config.GetStateLowerBound() != stateLowerBound)	
         std::cout << "setting and getting stateLowerBound test fails" << std::endl;

      //// Real array.  Upper bound on state vector
      Rvector stateUpperBound;
      stateUpperBound.SetSize(1);
      stateUpperBound(0) = 50;
      
      Config.SetStateUpperBound(stateUpperBound);
      if (Config.GetStateUpperBound() != stateUpperBound)	
         std::cout << "setting and getting stateUpperBound test fails" << std::endl;

      //// Real array. State guess at initial time
      Rvector stateInitialGuess;
      stateInitialGuess.SetSize(1);
      stateInitialGuess(0) = 1;

      Config.SetStateInitialGuess(stateInitialGuess);
      if (Config.GetStateInitialGuess() != stateInitialGuess)	
         std::cout << "setting and getting stateInitialGuess test fails" << std::endl;


      //// Real array. State guess at final time
      Rvector stateFinalGuess;
      stateFinalGuess.SetSize(1);
      stateFinalGuess(0) = 1;

      Config.SetStateFinalGuess(stateFinalGuess);
      if (Config.GetStateFinalGuess() != stateFinalGuess)	
         std::cout << "setting and getting stateFinalGuess test fails" << std::endl;

      Config.ValidateStateProperties();
   
	  
      //// Real. Time guess at beginning of phase
      Real timeInitialGuess = 0;
      
      Config.SetTimeInitialGuess(timeInitialGuess);
      if (Config.GetTimeInitialGuess() != timeInitialGuess)	
         std::cout << "setting and getting timeInitialGuess test fails" << std::endl;

      //// Real. Time guess at end of phase
      Real timeFinalGuess =50;

      Config.SetTimeFinalGuess(timeFinalGuess);
      if (Config.GetTimeFinalGuess() != timeFinalGuess)	
         std::cout << "setting and getting timeFinalGuess test fails" << std::endl;

      //// Real. Lower bound on time
      Real timeLowerBound = 0;

      Config.SetTimeLowerBound(timeLowerBound);
      if (Config.GetTimeLowerBound() != timeLowerBound)	
         std::cout << "setting and getting timeLowerBound test fails" << std::endl;

      //// Real. Upper bound on time
      Real timeUpperBound = 50;

      Config.SetTimeUpperBound(timeUpperBound);
      if (Config.GetTimeUpperBound() != timeUpperBound)	
         std::cout << "setting and getting timeUpperBound test fails" << std::endl;

      Config.ValidateTimeProperties();

      /// ==== Control related quantities
      //// Real array.  ower bound on control vector
      Rvector controlUpperBound;
      controlUpperBound.SetSize(1);
      controlUpperBound(0) = 50;

      Config.SetControlUpperBound(controlUpperBound);
      if (Config.GetControlUpperBound() != controlUpperBound)	
         std::cout << "setting and getting controlUpperBound test fails" << std::endl;

      //// Real array. Upper bound on control vector
      Rvector controlLowerBound;
      controlLowerBound.SetSize(1);
      controlLowerBound(0) = -50;      

      Config.SetControlLowerBound(controlLowerBound);
      if (Config.GetControlLowerBound() != controlLowerBound)	
         std::cout << "setting and getting controlLowerBound test fails" << std::endl;
	  
      Config.ValidateControlProperties();

	  
      // test assign, copy constructor operators
      ProblemCharacteristics Config2;
      Config2 = Config;      
      ProblemCharacteristics Config3 = Config2;

      std::cout << "If there is no fail flag, problem characteristic tests succeed. \n" << std::endl;	  
	  // test validation methods
	  /*
	  std::cout << "You must see control configuration validation fail" << std::endl;
	  Config3.SetControlUpperBound(controlLowerBound);
	  Config3.ValidateControlProperties();
	  */

	  /*
	  std::cout << "You must see time configuration validation fail" << std::endl;
	  Config3.SetTimeUpperBound(timeLowerBound);
	  Config3.ValidateTimeProperties();
	  */
	  /*
	  std::cout << "You must see state configuration validation fail" << std::endl;
	  Config3.SetStateUpperBound(stateLowerBound);
	  Config3.ValidateStateProperties();
	  */
	  
	  /*
	  std::cout << "You must see mesh configuration validation fail" << std::endl;
	  
	  meshIntervalNumPoints.push_back(1);
	  Config3.SetMeshIntervalNumPoints(meshIntervalNumPoints);
	  Config3.ValidateMeshConfig();
	  */
	  // test successful so far
	  
   };      
};
#endif // TestProblemCharacteristics_hpp