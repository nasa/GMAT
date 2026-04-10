//------------------------------------------------------------------------------
//                         ProblemCharacteristics
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
 * Implementation of the ProblemCharacteristics class
 */
//------------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include "ProblemCharacteristics.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_BOUNDS_VALUES
//#define DEBUG_INIT_VALIDATE


//------------------------------------------------------------------------------
//  static data
//------------------------------------------------------------------------------
const Real ProblemCharacteristics::boundTolerance = 1.0e-5;


//------------------------------------------------------------------------------
//  public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  ProblemCharacteristics()
//------------------------------------------------------------------------------
/**
 * Constructs a default ProblemCharacteristics
 * 
 */
//------------------------------------------------------------------------------
ProblemCharacteristics::ProblemCharacteristics() :
   numStateVars       (0),     // Optimal control parameter dimensions
   numControlVars     (0),
   numIntegralVars    (0),
   numStaticVars      (0),
   numTimeVars        (2),

   numStateVarsNLP    (0),     // NLP parameter dimensions
   numControlVarsNLP  (0),     //   Number of variables by type
   numTimeVarsNLP     (2),     //   in optimal control problem
   numDecisionVarsNLP (0),
   numDefectConNLP    (0),
   numTotalConNLP     (0),

   hasStateVars       (false),
   hasControlVars     (false),
   hasIntegralVars    (false),
   hasStaticVars      (false),
   hasTimeVars        (true),  // time vars are initialized

   hasDefectCons      (false), // Flags describing function types in
   hasIntegralCons    (false), // optimal control problem
   hasAlgPathCons     (false),
   hasIntegralCost    (false),
   hasAlgebraicCost   (false),


   timeLowerBound     (- GmatRealConstants::REAL_MAX), // Lower bound on time
   timeUpperBound     (GmatRealConstants::REAL_MAX),   // Upper bound on time
   timeInitialGuess   (0),    // Time guess at beginning of phase
   timeFinalGuess     (1),    // Time guess at end of phase

   stateInitialGuessSet    (false),
   stateFinalGuessSet      (false),
   timeInitialGuessSet    (false),
   timeFinalGuessSet      (false),
   staticVectorSet         (false)  // YK mod static params
{
   // ==== Discretization properties
   // define default mesh interval fraction of no internal points
   meshIntervalFractions.SetSize(2); 
   meshIntervalFractions(0) = -1.0;
   meshIntervalFractions(1) =  1.0;
   meshIntervalNumPoints.push_back(5);
   numMeshIntervals         =  1;
}

//------------------------------------------------------------------------------
//  ProblemCharacteristics(const ProblemCharacteristics &prb)
//------------------------------------------------------------------------------
/**
 * Copy Constructor of problem characteristic; 
 * 
 */
//------------------------------------------------------------------------------
ProblemCharacteristics::ProblemCharacteristics(
                                    const ProblemCharacteristics &prb)
{
   CopyData(prb);
}

//------------------------------------------------------------------------------
//  ProblemCharacteristics&   operator=
//                                  (const ProblemCharacteristics &bli)
//------------------------------------------------------------------------------
/**
 * Assignment operator of problem characteristic.
 * 
 */
//------------------------------------------------------------------------------
ProblemCharacteristics&   ProblemCharacteristics::operator=
                                    (const ProblemCharacteristics &prb)
{
   if (this == &prb)
      return *this;
   
   CopyData(prb);
   
   return *this;

}

//------------------------------------------------------------------------------
//  ~ProblemCharacteristicsInterpolator()
//------------------------------------------------------------------------------
/**
 * Destroys the core elements of an Interpolator.
 */
//------------------------------------------------------------------------------
ProblemCharacteristics::~ProblemCharacteristics() 
{
   /* do nothing*/
}

       
//------------------------------------------------------------------------------
// void SetNumStateVars(Integer nVars)
//------------------------------------------------------------------------------
/**
 * This method sets the number of state variables
 *
 * @param <nVars> the number of state variables
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetNumStateVars(Integer nVars)
{
   // Sets number of state vars and hasStateVars flag
   numStateVars = nVars;
   if (numStateVars != 0)
   {
      hasStateVars = true;
      stateInitialGuess.SetSize(numStateVars);
      stateFinalGuess.SetSize(numStateVars);
      stateLowerBound.SetSize(numStateVars);
      stateUpperBound.SetSize(numStateVars);
   }
   else
      hasStateVars = false;

   return;
}

//------------------------------------------------------------------------------
// void SetNumControlVars(Integer nVars)
//------------------------------------------------------------------------------
/**
 * This method sets the number of control variables
 *
 * @param <nVars> the number of control variables
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetNumControlVars(Integer nVars)
{
   // Sets number of Control vars and hasControlVars flag
   numControlVars = nVars;
   if (numControlVars != 0)
   {
      hasControlVars = true;
      controlLowerBound.SetSize(numControlVars);
      controlUpperBound.SetSize(numControlVars);
   }
   else
      hasControlVars = false;

   return;
}        

//------------------------------------------------------------------------------
// void SetNumIntegralVars(Integer nVars)
//------------------------------------------------------------------------------
/**
 * This method sets the number of integral variables
 *
 * @param <nVars> the number of integral variables
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetNumIntegralVars(Integer nVars)
{
   // Sets number of Integral vars and hasIntegralVars flag
   numIntegralVars = nVars;
   if (numIntegralVars != 0)
      hasIntegralVars = true;
   else
      hasIntegralVars = false;

   return;
}
        
//------------------------------------------------------------------------------
// void SetNumStaticVars(Integer nVars)
//------------------------------------------------------------------------------
/**
 * This method sets the number of static variables
 *
 * @param <nVars> the number of static variables
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetNumStaticVars(Integer nVars)
{
   // Sets number of Static vars and hasStaticVars flag
   numStaticVars = nVars;
   if (numStaticVars != 0)
      hasStaticVars = true;
   else
      hasStaticVars = false;

   return;
}

//------------------------------------------------------------------------------
// Integer GetNumStateVars()
//------------------------------------------------------------------------------
/**
 * This method returns the number of state variables
 *
 * @return the number of state variables
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumStateVars()
{
   return numStateVars;
};

//------------------------------------------------------------------------------
// Integer GetNumControlVars()
//------------------------------------------------------------------------------
/**
 * This method returns the number of control variables
 *
 * @return the number of control variables
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumControlVars()
{
   return numControlVars;
};

//------------------------------------------------------------------------------
// Integer GetNumIntegralVars()
//------------------------------------------------------------------------------
/**
 * This method returns the number of integral variables
 *
 * @return the number of integral variables
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumIntegralVars()
{
   return numIntegralVars;
};

//------------------------------------------------------------------------------
// Integer GetNumStaticVars()
//------------------------------------------------------------------------------
/**
 * This method returns the number of static variables
 *
 * @return the number of static variables
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumStaticVars()
{
   return numStaticVars;
};

//------------------------------------------------------------------------------
// Integer GetNumTimeVars()
//------------------------------------------------------------------------------
/**
 * This method returns the number of time variables
 *
 * @return the number of time variables
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumTimeVars()
{
   return numTimeVars;
};

//------------------------------------------------------------------------------
// bool HasStateVars()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not there are state 
 * variables
 *
 * @return true, if there are state variables; false, otherwise
 */
//------------------------------------------------------------------------------
bool ProblemCharacteristics::HasStateVars()
{
   return hasStateVars;
};

//------------------------------------------------------------------------------
// bool HasControlVars()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not there are control
 * variables
 *
 * @return true, if there are control variables; false, otherwise
 */
//------------------------------------------------------------------------------
bool ProblemCharacteristics::HasControlVars()
{
   return hasControlVars;
};

//------------------------------------------------------------------------------
// bool HasIntegralVars()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not there are integral
 * variables
 *
 * @return true, if there are integral variables; false, otherwise
 */
//------------------------------------------------------------------------------
bool ProblemCharacteristics::HasIntegralVars()
{
   return hasIntegralVars;
};

//------------------------------------------------------------------------------
// bool HasStaticVars()
//------------------------------------------------------------------------------
/**
 * This method returns a flag indicating whether or not there are static
 * variables
 *
 * @return true, if there are static variables; false, otherwise
 */
//------------------------------------------------------------------------------
bool ProblemCharacteristics::HasStaticVars()
{
   return hasStaticVars;
};

//------------------------------------------------------------------------------
// void SetNumStateVarsNLP(Integer nVarsNLP)
//------------------------------------------------------------------------------
/**
 * This method sets the number of state variables NLP
 *
 * @param <nVarsNLP> the number of state variables NLP
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetNumStateVarsNLP(Integer nVarsNLP)
{
   numStateVarsNLP = nVarsNLP;
};

//------------------------------------------------------------------------------
// void SetNumControlVarsNLP(Integer nVarsNLP)
//------------------------------------------------------------------------------
/**
 * This method sets the number of control variables NLP
 *
 * @param <nVarsNLP> the number of control variables NLP
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetNumControlVarsNLP(Integer nVarsNLP)
{
   numControlVarsNLP = nVarsNLP;
};

//------------------------------------------------------------------------------
// void SetNumDecisionVarsNLP(Integer nVarsNLP)
//------------------------------------------------------------------------------
/**
 * This method sets the number of decision variables NLP
 *
 * @param <nVarsNLP> the number of decision variables NLP
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetNumDecisionVarsNLP(Integer nVarsNLP)
{
   numDecisionVarsNLP = nVarsNLP;
};

//------------------------------------------------------------------------------
// void SetNumDefectConNLP(Integer nDefectConNLP)
//------------------------------------------------------------------------------
/**
 * This method sets the number of defect constraint variables NLP
 *
 * @param <nDefectConNLP> the number of defect constraint variables NLP
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetNumDefectConNLP(Integer nDefectConNLP)
{
   numDefectConNLP = nDefectConNLP;
};

//------------------------------------------------------------------------------
// void SetNumTotalConNLP(Integer nTotalConNLP)
//------------------------------------------------------------------------------
/**
 * This method sets the number of total constraint variables NLP
 *
 * @param <nTotalConNLP> the number of total constraint variables NLP
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetNumTotalConNLP(Integer nTotalConNLP)
{
   numTotalConNLP = nTotalConNLP;
};

//------------------------------------------------------------------------------
// Integer GetNumStateVarsNLP()
//------------------------------------------------------------------------------
/**
 * This method returns the number of state variables NLP
 *
 * @return the number of state variables NLP
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumStateVarsNLP()
{
   return numStateVarsNLP;
};

//------------------------------------------------------------------------------
// Integer GetNumControlVarsNLP()
//------------------------------------------------------------------------------
/**
 * This method returns the number of control variables NLP
 *
 * @return the number of control variables NLP
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumControlVarsNLP()
{
   return numControlVarsNLP;
};

//------------------------------------------------------------------------------
// Integer GetNumTimeVarsNLP()
//------------------------------------------------------------------------------
/**
 * This method returns the number of time variables NLP
 *
 * @return the number of time variables NLP
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumTimeVarsNLP()
{
   return numTimeVarsNLP;
};

//------------------------------------------------------------------------------
// Integer GetNumDecisionVarsNLP()
//------------------------------------------------------------------------------
/**
 * This method returns the number of decision variables NLP
 *
 * @return the number of decision variables NLP
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumDecisionVarsNLP()
{
   return numDecisionVarsNLP;
};

//------------------------------------------------------------------------------
// Integer GetNumDefectConNLP()
//------------------------------------------------------------------------------
/**
 * This method returns the number of defect constraint variables NLP
 *
 * @return the number of defect constraint variables NLP
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumDefectConNLP()
{
   return numDefectConNLP;
};

//------------------------------------------------------------------------------
// Integer GetNumTotalConNLP()
//------------------------------------------------------------------------------
/**
 * This method returns the number of total constraint variables NLP
 *
 * @return the number of total constraint variables NLP
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumTotalConNLP()
{
   return numTotalConNLP;
};

//------------------------------------------------------------------------------
// void SetHasDefectCons(bool hasCons)
//------------------------------------------------------------------------------
/**
 * This method sets the flag indicating whether or not there are defect
 * constraints
 *
 * @param <hasCons> flag indicating defect constraints or not
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetHasDefectCons(bool hasCons)
{
   hasDefectCons = hasCons;
};

//------------------------------------------------------------------------------
// void SetHasAlgPathCons(bool hasCons)
//------------------------------------------------------------------------------
/**
 * This method sets the flag indicating whether or not there are algebraic
 * path constraints
 *
 * @param <hasCons> flag indicating algebraic path constraints or not
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetHasAlgPathCons(bool hasCons)
{
   hasAlgPathCons = hasCons;
};

//------------------------------------------------------------------------------
// void SetHasIntegralCons(bool hasCons)
//------------------------------------------------------------------------------
/**
 * This method sets the flag indicating whether or not there are integral
 * constraints
 *
 * @param <hasCons> flag indicating integral constraints or not
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetHasIntegralCons(bool hasCons)
{
   hasIntegralCons = hasCons;
};

//------------------------------------------------------------------------------
// void SetHasIntegralCost(bool hasCost)
//------------------------------------------------------------------------------
/**
 * This method sets the flag indicating whether or not there are integral
 * cost constraints
 *
 * @param <hasCost> flag indicating integral cost constraints or not
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetHasIntegralCost(bool hasCost)
{
   hasIntegralCost = hasCost;
};

//------------------------------------------------------------------------------
// void SetHasAlgebraicCost(bool hasCost)
//------------------------------------------------------------------------------
/**
 * This method sets the flag indicating whether or not there are algebraic
 * cost constraints
 *
 * @param <hasCost> flag indicating algebraic cost constraints or not
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetHasAlgebraicCost(bool hasCost)
{
   hasAlgebraicCost = hasCost;
};

//------------------------------------------------------------------------------
// bool HasDefectCons()
//------------------------------------------------------------------------------
/**
 * This method returns the flag indicating whether or not there are defect
 * constraints
 *
 * @return flag indicating if there are defect constraints or not
 */
//------------------------------------------------------------------------------
bool ProblemCharacteristics::HasDefectCons()
{
   return hasDefectCons;
};

//------------------------------------------------------------------------------
// bool HasAlgPathCons()
//------------------------------------------------------------------------------
/**
 * This method returns the flag indicating whether or not there are algebraic
 * path constraints
 *
 * @return flag indicating if there are algebraic path constraints or not
 */
//------------------------------------------------------------------------------
bool ProblemCharacteristics::HasAlgPathCons()
{
   return hasAlgPathCons;
};

//------------------------------------------------------------------------------
// bool HasIntegralCons()
//------------------------------------------------------------------------------
/**
 * This method returns the flag indicating whether or not there are integral
 * constraints
 *
 * @return flag indicating if there are integral constraints or not
 */
//------------------------------------------------------------------------------
bool ProblemCharacteristics::HasIntegralCons()
{
   return hasIntegralCons;
};

//------------------------------------------------------------------------------
// bool HasIntegralCost()
//------------------------------------------------------------------------------
/**
 * This method returns the flag indicating whether or not there are integral
 * cost constraints
 *
 * @return flag indicating if there are integral cost constraints or not
 */
//------------------------------------------------------------------------------
bool ProblemCharacteristics::HasIntegralCost()
{
   return hasIntegralCost;
};

//------------------------------------------------------------------------------
// bool HasAlgebraicCost()
//------------------------------------------------------------------------------
/**
 * This method returns the flag indicating whether or not there are algebraic
 * cost constraints
 *
 * @return flag indicating if there are algebraic cost constraints or not
 */
//------------------------------------------------------------------------------
bool ProblemCharacteristics::HasAlgebraicCost()
{
   return hasAlgebraicCost;
};

//------------------------------------------------------------------------------
// void SetStateLowerBound(const Rvector &boundVec)
//------------------------------------------------------------------------------
/**
 * This method sets the state lower bound vector
 *
 * @param <boundVec> state lower bound vector
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetStateLowerBound(const Rvector &boundVec)
{
   stateLowerBound = boundVec;
};

//------------------------------------------------------------------------------
// void SetStateUpperBound(const Rvector &boundVec)
//------------------------------------------------------------------------------
/**
 * This method sets the state upper bound vector
 *
 * @param <boundVec> state upper bound vector
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetStateUpperBound(const Rvector &bound)
{
   stateUpperBound = bound;
};

//------------------------------------------------------------------------------
// void SetStateInitialGuess(const Rvector &initGuess)
//------------------------------------------------------------------------------
/**
 * This method sets the state initial guess vector
 *
 * @param <initGuess> state initial guess vector
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetStateInitialGuess(const Rvector &initGuess)
{
   if ((!stateInitialGuess.IsSized()) ||
       (stateInitialGuess.GetSize() != initGuess.GetSize()))
      stateInitialGuess.Resize(initGuess.GetSize());
   stateInitialGuess    = initGuess;
   stateInitialGuessSet = true;
};

//------------------------------------------------------------------------------
// void SetStateFinalGuess(const Rvector &finalGuess)
//------------------------------------------------------------------------------
/**
 * This method sets the state final guess vector
 *
 * @param <finalGuess> state final guess vector
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetStateFinalGuess(const Rvector &finalGuess)
{
   if (stateFinalGuess.GetSize() != finalGuess.GetSize())
      stateFinalGuess.Resize(finalGuess.GetSize());
   stateFinalGuess    = finalGuess;
   stateFinalGuessSet = true;
};

//------------------------------------------------------------------------------
// Rvector GetStateLowerBound()
//------------------------------------------------------------------------------
/**
 * This method returns the state lower bound vector
 *
 * @return state lower bound vector
 */
//------------------------------------------------------------------------------
Rvector ProblemCharacteristics::GetStateLowerBound()
{
   return stateLowerBound;
};

//------------------------------------------------------------------------------
// Rvector GetStateUpperBound()
//------------------------------------------------------------------------------
/**
 * This method returns the state upper bound vector
 *
 * @return state upper bound vector
 */
//------------------------------------------------------------------------------
Rvector ProblemCharacteristics::GetStateUpperBound()
{
   return stateUpperBound;
};

//------------------------------------------------------------------------------
// Rvector GetStateInitialGuess()
//------------------------------------------------------------------------------
/**
 * This method returns the state initial guess vector
 *
 * @return state initial guess vector
 */
//------------------------------------------------------------------------------
Rvector ProblemCharacteristics::GetStateInitialGuess()
{
   return stateInitialGuess;
};

//------------------------------------------------------------------------------
// Rvector GetStateFinalGuess()
//------------------------------------------------------------------------------
/**
 * This method returns the state final guess vector
 *
 * @return state final guess vector
 */
//------------------------------------------------------------------------------
Rvector ProblemCharacteristics::GetStateFinalGuess()
{
   return stateFinalGuess;
};

//------------------------------------------------------------------------------
// void ValidateStateProperties()
//------------------------------------------------------------------------------
/**
 * This method validates the state properties
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::ValidateStateProperties()
{
   #ifdef DEBUG_INIT_VALIDATE
      std::cout << "Entering PC::ValidateStateProperties ...\n";
      std::cout << "   stateInitialGuessSet = " <<
                   (Integer) stateInitialGuessSet << std::endl;
   #endif
   // Validate that state related properties are valid
   
   // Check dimensions of lower bounds
   if (stateLowerBound.GetSize() != numStateVars)
   {
      std::string errmsg="For ProblemCharacteristics::StateProperties,";
      errmsg += "length of stateLowerBounds must be numStateVars";
      throw LowThrustException(errmsg);
   }
   // Check dimensions of upper bounds
   if (stateUpperBound.GetSize() != numStateVars)
   {
      std::string errmsg="For ProblemCharacteristics::StateProperties,";
      errmsg += "length of stateUpperBounds must be numStateVars";
      throw LowThrustException(errmsg);
   }
   // Only check the initial guess if it has been set
   if (stateInitialGuessSet)
   {
      // Check dimensions of initial guess
      if (stateInitialGuess.GetSize() != numStateVars)
      {
         std::string errmsg="For ProblemCharacteristics::StateProperties,";
         errmsg += "length of stateInitialGuess must be numStateVars";
         throw LowThrustException(errmsg);
      }
      // Check that initial guess falls between upper and lower bounds
      for (Integer stateIdx = 0; stateIdx < numStateVars; ++stateIdx)
      {
         #ifdef DEBUG_BOUNDS_VALUES
            std::stringstream debugMsg;
            debugMsg << "In ProbChar::Validate, stateInitialGuess(";
            debugMsg << stateIdx << ") = " << stateInitialGuess(stateIdx);
            std::cout << debugMsg.str().c_str() << std::endl;
         #endif
         if ((stateInitialGuess(stateIdx) > stateUpperBound(stateIdx)) ||
             (stateInitialGuess(stateIdx) < stateLowerBound(stateIdx)))
         {
            std::string errmsg="For ProblemCharacteristics::StateProperties,";
            errmsg += "stateInitialGuess elements must fall between upper ";
            errmsg += "and lower bounds";
            throw LowThrustException(errmsg);
         }
      }
   }
   
   if (stateFinalGuessSet)
   {
      // Check dimensions of final guess
      if (stateFinalGuess.GetSize() != numStateVars)
      {
         std::string errmsg="For ProblemCharacteristics::StateProperties,";
         errmsg += "length of stateFinalGuess must be numStateVars";
         throw LowThrustException(errmsg);
      }
      
      
      // Check that final guess falls between upper and lower bounds
      for (Integer stateIdx = 0; stateIdx < numStateVars; ++stateIdx)
      {
         if ((stateFinalGuess(stateIdx) > stateUpperBound(stateIdx)) ||
             (stateFinalGuess(stateIdx) < stateLowerBound(stateIdx)))
         {
            std::string errmsg="For ProblemCharacteristics::StateProperties,";
            errmsg += "stateFinalGuessGuess elements must fall between upper ";
            errmsg += "and lower bounds";
            throw LowThrustException(errmsg);
         }
      }
   }
   
   // Lower bound must be less than upper bound
   //   Real boundDiff = 1e-5;
   #ifdef DEBUG_INIT_VALIDATE
      std::cout << "In PC::ValidateStateProperties ...\n";
      std::cout << "numStateVars = " << numStateVars << std::endl;
      std::cout << "   stateLowerBound size = " <<
                   (Integer) stateLowerBound.GetSize() << std::endl;
      std::cout << "   stateUpperBound size = " <<
                   (Integer) stateUpperBound.GetSize() << std::endl;
   #endif

   for (Integer stateIdx = 0; stateIdx < numStateVars; stateIdx++)
   {
      if (stateLowerBound(stateIdx) >
          stateUpperBound(stateIdx) - boundTolerance)
      {
         std::stringstream errmsg("");
         errmsg << "For ProblemCharacteristics::StateProperties,";
         errmsg << " state lower bound for state id ";
         errmsg << stateIdx << " must be at least ";
         errmsg << boundTolerance << " less than state upper bound";
         throw LowThrustException(errmsg.str());
         //         std::string errmsg="For ProblemCharacteristics::StateProperties,";
         //         errmsg += "state lower bound for state id ";
         //		       errmsg += std::to_string((long double) stateIdx);
         //		       errmsg += " must be 1e-5 less state upper bound";
         //         throw LowThrustException(errmsg);
      }
   }
   #ifdef DEBUG_INIT_VALIDATE
      std::cout << "Leaving PC::ValidateStateProperties ...\n";
   #endif
   return;
}

//------------------------------------------------------------------------------
// void SetControlLowerBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * This method sets the control lower bound
 *
 * @param <bound> control lower bound
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetControlLowerBound(const Rvector &bound)
{
   controlLowerBound = bound;
};

//------------------------------------------------------------------------------
// void SetControlUpperBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * This method sets the control upper bound
 *
 * @param <bound> control upper bound
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetControlUpperBound(const Rvector &bound)
{
   controlUpperBound = bound;
};


//------------------------------------------------------------------------------
// Rvector GetControlLowerBound()
//------------------------------------------------------------------------------
/**
 * This method returns the control lower bound
 *
 * @return control lower bound
 *
 */
//------------------------------------------------------------------------------
Rvector ProblemCharacteristics::GetControlLowerBound()
{
   return controlLowerBound;
};

//------------------------------------------------------------------------------
// Rvector GetControlUpperBound()
//------------------------------------------------------------------------------
/**
 * This method returns the control upper bound
 *
 * @return control upper bound
 *
 */
//------------------------------------------------------------------------------
Rvector ProblemCharacteristics::GetControlUpperBound()
{
   return controlUpperBound;
};

//------------------------------------------------------------------------------
// void ValidateControlProperties()
//------------------------------------------------------------------------------
/**
 * This method validates the control properties
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::ValidateControlProperties()
{
   // Validate that control related properties are valid
   
   // Check dimensions of lower bounds
   if (controlLowerBound.GetSize() != numControlVars)
   {
      std::string errmsg="For ProblemCharacteristics::StateProperties,";
      errmsg += "length of controlLowerBounds must be numStateVars";
      throw LowThrustException(errmsg);
   }
   
   // Check dimensions of upper bounds
   if (controlUpperBound.GetSize() != numControlVars)
   {
      std::string errmsg="For ProblemCharacteristics::StateProperties,";
      errmsg += "length of controlUpperBounds must be numStateVars";
      throw LowThrustException(errmsg);
   }
   
   // Lower bound must be less than upper bound
   //   Real boundDiff = 1e-5;
   for (Integer controlIdx = 0; controlIdx < numControlVars; ++controlIdx)
   {
      if (controlLowerBound(controlIdx) >
          controlUpperBound(controlIdx) - boundTolerance)
      {
         std::stringstream errmsg("");
         errmsg << "For ProblemCharacteristics::StateProperties,";
         errmsg << " control lower bound for state id ";
         errmsg << controlIdx << " must be at least ";
         errmsg << boundTolerance << " less than control upper bound";
         throw LowThrustException(errmsg.str());
         //         std::string errmsg="For ProblemCharacteristics::StateProperties,";
         //         errmsg += "control lower bound for control id ";
         //		       errmsg += std::to_string((long double) controlIdx);
         //		       errmsg += " must be 1e-5 less control upper bound";
         //         throw LowThrustException(errmsg);
      }
   }
   return;
}


//------------------------------------------------------------------------------
// void SetMeshIntervalFractions(const Rvector &fractions)
//------------------------------------------------------------------------------
/**
 * This method sets the mesh interval fractions
 *
 * @param <fractions> the input mesh interval fractions
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetMeshIntervalFractions(const Rvector &fractions)
{
   if (fractions.GetSize() <= 1)
   {	         
      std::string errmsg = "For ProblemCharacteristics::";
      errmsg += "SetMeshIntervalFractions, meshIntervalFractions must contain ";
      errmsg += "at least two elements";
      throw LowThrustException(errmsg);
   }
   else
   {
	     numMeshIntervals = fractions.GetSize() - 1;
   }

   meshIntervalFractions.SetSize(fractions.GetSize());
   meshIntervalFractions = fractions;

   return;
}

//------------------------------------------------------------------------------
// void SetMeshIntervalNumPoints(IntegerArray numPoints)
//------------------------------------------------------------------------------
/**
 * This method sets the mesh interval number of points
 *
 * @param <fractions> the input mesh interval nunber of points
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetMeshIntervalNumPoints(IntegerArray numPoints)
{
   if (numPoints.size() < 1)
   {	   
      std::string errmsg = "For ProblemCharacteristics::";
      errmsg += "SetMeshIntervalFractions, meshIntervalNumPoints must contain ";
      errmsg += "at least one element";
      throw LowThrustException(errmsg);
   }
   meshIntervalNumPoints.resize(numPoints.size());
   meshIntervalNumPoints = numPoints;
   return;
}

//------------------------------------------------------------------------------
// Rvector GetMeshIntervalFractions()
//------------------------------------------------------------------------------
/**
 * This method returns the mesh interval fractions
 *
 * @return  the mesh interval fractions
 *
 */
//------------------------------------------------------------------------------
Rvector ProblemCharacteristics::GetMeshIntervalFractions()
{
   return meshIntervalFractions;
};

//------------------------------------------------------------------------------
// IntegerArray GetMeshIntervalNumPoints()
//------------------------------------------------------------------------------
/**
 * This method returns the mesh interval number of points array
 *
 * @return  the mesh interval number of points array
 *
 */
//------------------------------------------------------------------------------
IntegerArray ProblemCharacteristics::GetMeshIntervalNumPoints()
{
   return meshIntervalNumPoints;
};

//------------------------------------------------------------------------------
// Integer GetNumMeshIntervals()
//------------------------------------------------------------------------------
/**
 * This method returns the number of mesh intervals
 *
 * @return  the number of mesh intervals
 *
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumMeshIntervals()
{
   return numMeshIntervals;
};

        
//------------------------------------------------------------------------------
// Integer GetNumPointsInMeshInterval(Integer mIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the number of points in the specified mesh interval
 *
 * @param <mIdx> mesh interval number
 *
 * @return the number of points in the specified mesh interval
 *
 */
//------------------------------------------------------------------------------
Integer ProblemCharacteristics::GetNumPointsInMeshInterval(Integer mIdx)
{
   if (mIdx > numMeshIntervals)
   {	   
      std::string errmsg = "For ProblemCharacteristics::";
      errmsg += "GetNumPointsInMeshInterval, meshId must be <= to number of ";
      errmsg += "mesh intervals";
      throw LowThrustException(errmsg);
   }
   return meshIntervalNumPoints[mIdx];
}        
        
//------------------------------------------------------------------------------
// Real GetMeshIntervalFraction(Integer mIdx)
//------------------------------------------------------------------------------
/**
 * This method returns the mesh interval fraction for the specified interval
 *
 * @param <mIdx> mesh interval number
 *
 * @return the mesh interval fraction for the specified mesh interval
 *
 */
//------------------------------------------------------------------------------
Real ProblemCharacteristics::GetMeshIntervalFraction(Integer mIdx)
{
   if (mIdx > numMeshIntervals + 1)
   {	   
      std::string errmsg = "For ProblemCharacteristics::";
      errmsg += "GetNumPointsInMeshInterval, meshId must be <= to number of ";
      errmsg += "mesh intervals + 1";
      throw LowThrustException(errmsg);
   }
   return meshIntervalFractions(mIdx);
}        

//------------------------------------------------------------------------------
// void ValidateMeshConfig()
//------------------------------------------------------------------------------
/**
 * This method valdates the mesh configuration
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::ValidateMeshConfig()
{
   if ((meshIntervalNumPoints.size() + 1) != (UnsignedInt)meshIntervalFractions.GetSize())
   {	   
      std::string errmsg="For ProblemCharacteristics::ValidateMeshConfig,";
      errmsg += "Length of meshIntervalNumPoints ";
      errmsg += "must be one less than the length ";
      errmsg += "of meshIntervalFractions\n";
      throw LowThrustException(errmsg);      
   }
   if (meshIntervalNumPoints.size() != (UnsignedInt)numMeshIntervals)
   {	   
      std::string errmsg="For ProblemCharacteristics::ValidateMeshConfig,";
      errmsg += "Length of meshIntervalNumPoints ";
      errmsg += "must be equal to the number of ";
      errmsg += "mesh intervals\n";
      throw LowThrustException(errmsg);      
   }
   return;
}

//------------------------------------------------------------------------------
// void SetTimeLowerBound(Real bound)
//------------------------------------------------------------------------------
/**
 * This method sets the time lower bound
 *
 * @param  <bound> the time lower bound
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetTimeLowerBound(Real bound)
{
   timeLowerBound = bound;
};

//------------------------------------------------------------------------------
// void SetTimeUpperBound(Real bound)
//------------------------------------------------------------------------------
/**
 * This method sets the time upper bound
 *
 * @param  <bound> the time upper bound
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetTimeUpperBound(Real bound)
{
   timeUpperBound = bound;
};

//------------------------------------------------------------------------------
// void SetTimeInitialGuess(Real initGuess)
//------------------------------------------------------------------------------
/**
 * This method sets the time initial guess
 *
 * @param  <initGuess> the time initial guess
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetTimeInitialGuess(Real initGuess)
{
   timeInitialGuess    = initGuess;
   timeInitialGuessSet = true;
};

//------------------------------------------------------------------------------
// void SetTimeFinalGuess(Real finalGuess)
//------------------------------------------------------------------------------
/**
 * This method sets the time final guess
 *
 * @param  <finalGuess> the time final guess
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetTimeFinalGuess(Real finalGuess)
{
   timeFinalGuess    = finalGuess;
   timeFinalGuessSet = true;
};

//------------------------------------------------------------------------------
// Real GetTimeLowerBound()
//------------------------------------------------------------------------------
/**
 * This method returns the time lower bound
 *
 * @return the time lower bound
 *
 */
//------------------------------------------------------------------------------
Real ProblemCharacteristics::GetTimeLowerBound()
{
   return timeLowerBound;
};

//------------------------------------------------------------------------------
// Real GetTimeUpperBound()
//------------------------------------------------------------------------------
/**
 * This method returns the time upper bound
 *
 * @return the time upper bound
 *
 */
//------------------------------------------------------------------------------
Real ProblemCharacteristics::GetTimeUpperBound()
{
   return timeUpperBound;
};

//------------------------------------------------------------------------------
// Real GetTimeInitialGuess()
//------------------------------------------------------------------------------
/**
 * This method returns the time initial guess
 *
 * @return the time initial guess
 *
 */
//------------------------------------------------------------------------------
Real ProblemCharacteristics::GetTimeInitialGuess()
{
   return timeInitialGuess;
};

//------------------------------------------------------------------------------
// Real GetTimeFinalGuess()
//------------------------------------------------------------------------------
/**
 * This method returns the time final guess
 *
 * @return the time final guess
 *
 */
//------------------------------------------------------------------------------
Real ProblemCharacteristics::GetTimeFinalGuess()
{
   return timeFinalGuess;
};


//------------------------------------------------------------------------------
// void ValidateTimeProperties()
//------------------------------------------------------------------------------
/**
 * This method valdates the time properties
 *
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::ValidateTimeProperties()
{
   // Validate that time related properties are valid
   
   if (timeInitialGuessSet)
   {
      // Check that initial guess falls between upper and lower bounds

      if ((timeInitialGuess > timeUpperBound) || 
          (timeInitialGuess < timeLowerBound))
      {
         std::string errmsg="For ProblemCharacteristics::StateProperties,";
         errmsg += "timeInitialGuess  must fall between upper and lower bounds";
         throw LowThrustException(errmsg);     
      }
   }

   if (timeFinalGuessSet)
   {
      // Check that final guess falls between upper and lower bounds
      if ((timeFinalGuess > timeUpperBound) || 
          (timeFinalGuess < timeLowerBound))
      {
         std::string errmsg="For ProblemCharacteristics::StateProperties,";
         errmsg += "timeFinalGuess must fall between upper and lower bounds";
         throw LowThrustException(errmsg);     
      }
   }
 
   // Lower bound must be less than upper bound
   //  Real boundDiff = 1e-5;
   if (timeLowerBound > (timeUpperBound - boundTolerance))
   {
      std::stringstream errmsg("");
	   errmsg << "For ProblemCharacteristics::StateProperties,";
      errmsg << "Time lower bound must be ";
	   errmsg << boundTolerance << " less than time upper bound";
      throw LowThrustException(errmsg.str());    
   }  
   return;
}

//------------------------------------------------------------------------------
// void SetStaticLowerBound(const Rvector &boundVec)
//------------------------------------------------------------------------------
/**
* This method sets the static lower bound vector
*
* @param <boundVec> static lower bound vector
*/
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetStaticLowerBound(const Rvector &boundVec)
{
   staticLowerBound = boundVec;
};

//------------------------------------------------------------------------------
// void SetstaticUpperBound(const Rvector &boundVec)
//------------------------------------------------------------------------------
/**
* This method sets the static upper bound vector
*
* @param <boundVec> static upper bound vector
*/
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetStaticUpperBound(const Rvector &bound)
{
   staticUpperBound = bound;
};

//------------------------------------------------------------------------------
// void SetstaticVector(const Rvector &initGuess)
//------------------------------------------------------------------------------
/**
* This method sets the static initial guess vector
*
* @param <initGuess> static initial guess vector
*/
//------------------------------------------------------------------------------
void ProblemCharacteristics::SetStaticVector(const Rvector &initGuess)
{
   if ((!staticVector.IsSized()) ||
      (staticVector.GetSize() != initGuess.GetSize()))
      staticVector.Resize(initGuess.GetSize());
   staticVector = initGuess;
   staticVectorSet = true;
};

//------------------------------------------------------------------------------
// Rvector GetStaticLowerBound()
//------------------------------------------------------------------------------
/**
* This method returns the static lower bound vector
*
* @return static lower bound vector
*/
//------------------------------------------------------------------------------
Rvector ProblemCharacteristics::GetStaticLowerBound()
{
   return staticLowerBound;
};

//------------------------------------------------------------------------------
// Rvector GetStateUpperBound()
//------------------------------------------------------------------------------
/**
* This method returns the state upper bound vector
*
* @return state upper bound vector
*/
//------------------------------------------------------------------------------
Rvector ProblemCharacteristics::GetStaticUpperBound()
{
   return staticUpperBound;
};

//------------------------------------------------------------------------------
// Rvector GetStateGuess()
//------------------------------------------------------------------------------
/**
* This method returns the state initial guess vector
*
* @return state initial guess vector
*/
//------------------------------------------------------------------------------
Rvector ProblemCharacteristics::GetStaticVector()
{
   if (!staticVector.IsSized())
   {
      std::string errmsg = "For ProblemCharacteristics::GetStaticVector,";
      errmsg += "the attempt to obtain static vector failed.";
      errmsg += " because the static vector is not set, yet";
      throw LowThrustException(errmsg);
   }
   return staticVector;
};

//------------------------------------------------------------------------------
// void ValidateStaticProperties()
//------------------------------------------------------------------------------
/**
* This method validates the static properties
*
*/
//------------------------------------------------------------------------------
void ProblemCharacteristics::ValidateStaticProperties()
{
#ifdef DEBUG_INIT_VALIDATE
   std::cout << "Entering PC::ValidateStaticProperties ...\n";
   std::cout << "   staticVectorSet = " <<
      (Integer)staticVectorSet << std::endl;
#endif
   // Validate that static related properties are valid

   // Check dimensions of lower bounds
   if (staticLowerBound.GetSize() != numStaticVars)
   {
      std::string errmsg = "For ProblemCharacteristics::StaticProperties,";
      errmsg += "length of staticLowerBounds must be numStaticVars";
      throw LowThrustException(errmsg);
   }
   // Check dimensions of upper bounds
   if (staticUpperBound.GetSize() != numStaticVars)
   {
      std::string errmsg = "For ProblemCharacteristics::StaticProperties,";
      errmsg += "length of staticUpperBounds must be numStaticVars";
      throw LowThrustException(errmsg);
   }
   // Only check the initial guess if it has been set
   if (staticVectorSet)
   {
      // Check dimensions of initial guess
      if (staticVector.GetSize() != numStaticVars)
      {
         std::string errmsg = "For ProblemCharacteristics::StaticProperties,";
         errmsg += "length of staticVector must be numStaticVars";
         throw LowThrustException(errmsg);
      }
      // Check that initial guess falls between upper and lower bounds
      for (Integer staticIdx = 0; staticIdx < numStaticVars; ++staticIdx)
      {
#ifdef DEBUG_BOUNDS_VALUES
         std::stringstream debugMsg;
         debugMsg << "In ProbChar::Validate, staticVector(";
         debugMsg << staticIdx << ") = " << staticVector(staticIdx);
         std::cout << debugMsg.str().c_str() << std::endl;
#endif
         if ((staticVector(staticIdx) > staticUpperBound(staticIdx)) ||
            (staticVector(staticIdx) < staticLowerBound(staticIdx)))
         {
            std::string errmsg = "For ProblemCharacteristics::StaticProperties,";
            errmsg += "staticVector elements must fall between upper ";
            errmsg += "and lower bounds";
            throw LowThrustException(errmsg);
         }
      }
   }


   // Lower bound must be less than upper bound
   //   Real boundDiff = 1e-5;
#ifdef DEBUG_INIT_VALIDATE
   std::cout << "In PC::ValidateStaticProperties ...\n";
   std::cout << "numStaticVars = " << numStaticVars << std::endl;
   std::cout << "   staticLowerBound size = " <<
      (Integer)staticLowerBound.GetSize() << std::endl;
   std::cout << "   staticUpperBound size = " <<
      (Integer)staticUpperBound.GetSize() << std::endl;
#endif

   for (Integer staticIdx = 0; staticIdx < numStaticVars; staticIdx++)
   {
      if (staticLowerBound(staticIdx) >
         staticUpperBound(staticIdx) - boundTolerance)
      {
         std::stringstream errmsg("");
         errmsg << "For ProblemCharacteristics::StaticProperties,";
         errmsg << " static lower bound for static id ";
         errmsg << staticIdx << " must be at least ";
         errmsg << boundTolerance << " less than static upper bound";
         throw LowThrustException(errmsg.str());
         //         std::string errmsg="For ProblemCharacteristics::StaticProperties,";
         //         errmsg += "static lower bound for static id ";
         //		       errmsg += std::to_string((long double) staticIdx);
         //		       errmsg += " must be 1e-5 less static upper bound";
         //         throw LowThrustException(errmsg);
      }
   }
#ifdef DEBUG_INIT_VALIDATE
   std::cout << "Leaving PC::ValidateStaticProperties ...\n";
#endif
   return;
}

//------------------------------------------------------------------------------
//  protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void CopyData(const ProblemCharacteristics &prb)
//------------------------------------------------------------------------------
/**
 * This method copies the dat of the input object to the 'this' object
 *
 * @param <prb> object to copy
 */
//------------------------------------------------------------------------------
void ProblemCharacteristics::CopyData(const ProblemCharacteristics &prb)
{
   // ===== Optimal control parameter dimensions
   numStateVars    = prb.numStateVars;
   numControlVars  = prb.numControlVars;
   numIntegralVars = prb.numIntegralVars;
   numStaticVars   = prb.numStaticVars;
   numTimeVars     = prb.numTimeVars;
   
   //===== NLP parameter dimensions
   numStateVarsNLP    = prb.numStateVarsNLP;
   numControlVarsNLP  = prb.numControlVarsNLP;
   numTimeVarsNLP     = prb.numTimeVarsNLP;
   numDecisionVarsNLP = prb.numDecisionVarsNLP;
   numDefectConNLP    = prb.numDefectConNLP;
   numTotalConNLP     = prb.numTotalConNLP;
   
   // ===== Number of variables by type in optimal control problem
   
   hasIntegralVars  = prb.hasIntegralVars;
   hasStaticVars    = prb.hasStaticVars;
   hasTimeVars      = prb.hasTimeVars;
   
   // ===== Flags describing function types in optimal control problem
   
   hasDefectCons    = prb.hasDefectCons;
   hasIntegralCons  = prb.hasIntegralCons;
   hasAlgPathCons   = prb.hasAlgPathCons;
   hasIntegralCost  = prb.hasIntegralCost;
   hasAlgebraicCost = prb.hasAlgebraicCost;
   
   // ==== State related quantities
   hasStateVars = prb.hasStateVars;
   if (hasStateVars == true)
   {
      // Lower bound on state vector
      stateLowerBound.SetSize(prb.numStateVars);
      stateLowerBound = prb.stateLowerBound;
      // Upper bound on state vector
      stateUpperBound.SetSize(prb.numStateVars);
      stateUpperBound = prb.stateUpperBound;
      // State guess at initial time
      stateInitialGuess.SetSize(prb.numStateVars);
      stateInitialGuess = prb.stateInitialGuess;
      // State guess at final time
      stateFinalGuess.SetSize(prb.numStateVars);
      stateFinalGuess = prb.stateFinalGuess;
      stateInitialGuessSet  = prb.stateInitialGuessSet;
      stateFinalGuessSet    = prb.stateFinalGuessSet;
   }
   
   
   // ==== Control related quantities
   hasControlVars = prb.hasControlVars;
   if (hasControlVars == true)
   {
      // Lower bound on control vector
      controlLowerBound.SetSize(prb.numControlVars);
      controlLowerBound = prb.controlLowerBound;
      // Upper bound on control vector
      controlUpperBound.SetSize(prb.numControlVars);
      controlUpperBound = prb.controlUpperBound;
   }
   
   // ==== Discretization properties
   meshIntervalFractions.SetSize(prb.numMeshIntervals + 1);
   meshIntervalFractions = prb.meshIntervalFractions;
   meshIntervalNumPoints.resize(prb.numMeshIntervals); // necessary?
   meshIntervalNumPoints = prb.meshIntervalNumPoints;
   numMeshIntervals      = prb.numMeshIntervals;
   
   // Lower bound on time
   timeLowerBound   = prb.timeLowerBound;
   // Upper bound on time
   timeUpperBound   = prb.timeUpperBound;
   // Time guess at beginning of phase
   timeInitialGuess = prb.timeInitialGuess;
   // Time guess at end of phase
   timeFinalGuess   = prb.timeFinalGuess;
   
   timeInitialGuessSet   = prb.timeInitialGuessSet;
   timeFinalGuessSet     = prb.timeFinalGuessSet;

   // ==== Static parameter related quantities; YK mod static params
   hasStaticVars = prb.hasStaticVars;
   if (hasStaticVars == true)
   {
      // Lower bound on static vector
      staticLowerBound.SetSize(prb.numStateVars);
      staticLowerBound = prb.staticLowerBound;
      // Upper bound on static vector
      staticUpperBound.SetSize(prb.numStateVars);
      staticUpperBound = prb.staticUpperBound;
      // State guess at initial time
      staticVector.SetSize(prb.numStateVars);
      staticVector = prb.staticVector;
      // State guess at final time
      staticVectorSet = prb.staticVectorSet;
   }

}
