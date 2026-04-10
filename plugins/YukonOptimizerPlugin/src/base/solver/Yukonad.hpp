//------------------------------------------------------------------------------
//                              Yukonad
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task 08
//
// Converted: 2017/10/12, Joshua Raymond, Thinking Systems, Inc.
//
/**
 * Implementation for the steepest descent optimizer.
 *
 * This is prototype code.  Interested parties need to roll the Yukonad optimizer
 * code into a separate library and link to it.  Contact Thinking Systems for
 * details.
 */
//------------------------------------------------------------------------------

#ifndef Yukonad_hpp
#define Yukonad_hpp

#include "yukon_defs.hpp"

#include "gmatdefs.hpp"
#include "InternalOptimizer.hpp"
#include "Gradient.hpp"
#include "Jacobian.hpp"
#include "Yukon.hpp"
#include "GmatProblemInterface.hpp"

/**
* The Yukonad optimizer if the prototypical optimization method.  While
* not the most efficient method, it is the simplest to implement, since all it
* needs to do is run nominal trajectories, calculate gradients (via finite
* differences if no analytic form exists), scan in the "downhill" direction,
* and repeat until the magnitude of the gradient is small enough to declare
* victory.
*/
class YUKON_API Yukonad : public InternalOptimizer
{
   friend class YUKON_API GmatProblemInterface;

public:
   Yukonad(const std::string &name);
   virtual ~Yukonad();
   Yukonad(const Yukonad& sd);
   Yukonad& operator=(const Yukonad& sd);

   virtual void Copy(const GmatBase*);

   virtual const StringArray&
      GetRefObjectNameArray(const Gmat::ObjectType type);

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   virtual Gmat::ParameterType
      GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
      const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
      const std::string &value);
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
      const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
      const bool value);

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
      const Real value);
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
      const Integer value);
   //virtual Real         GetRealParameter(const Integer id,
   //                                      const Integer index) const;
   //virtual Real         GetRealParameter(const Integer id, const Integer row,
   //                                      const Integer col) const;
   //virtual Real         SetRealParameter(const Integer id,
   //                                      const Real value,
   //                                      const Integer index);
   //virtual Real         SetRealParameter(const Integer id, const Real value,
   //                                      const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
      const Real value);
   //virtual Real         GetRealParameter(const std::string &label,
   //                                      const Integer index) const;
   //virtual Real         SetRealParameter(const std::string &label,
   //                                      const Real value,
   //                                      const Integer index);
   //virtual Real         GetRealParameter(const std::string &label,
   //                                      const Integer row,
   //                                      const Integer col) const;
   //virtual Real         SetRealParameter(const std::string &label,
   //                                      const Real value, const Integer row,
   //                                      const Integer col);
   virtual const StringArray& GetPropertyEnumStrings(const Integer id) const;

   virtual Integer      SetSolverResults(Real *data,
      const std::string &name,
      const std::string &type = "");
   virtual void         SetResultValue(Integer id, Real value,
      const std::string &resultType = "");
   virtual GmatBase*    Clone() const;
   virtual bool         TakeAction(const std::string &action,
      const std::string &actionData = "");
   virtual bool         Initialize();
   virtual Solver::SolverState
      AdvanceState();
   virtual bool         Optimize();

   virtual const RealArray*
      GetSolverData(const std::string &type);

   enum
   {
      goalNameID = InternalOptimizerParamCount,
      useCentralDifferencesID,
      feasibilityToleranceID,
      hessianUpdateMethodID,
      maximumFunctionEvalsID,
      optimalityToleranceID,
      functionToleranceID,
      maximumElasticWeightID,
      YukonadParamCount
   };

   enum Hessian_Update_Method
   {
      DampedBFGS,
      SelfScaledBFGS,
      MaxUpdateMethod
   };

   static const std::string
      PARAMETER_TEXT[YukonadParamCount - OptimizerParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[YukonadParamCount - OptimizerParamCount];
   static const std::string
      HESSIAN_UPDATE_METHOD[MaxUpdateMethod - DampedBFGS];

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   std::string          objectiveName;
   bool                 objectiveSet;

   Gradient             gradientCalculator;
   Jacobian             jacobianCalculator;
   Integer              retCode;
   bool                 useCentralDifferences;

   // Interface to bug fix by Juan Senent
   /// Parameter added to force constraint matching to specified tolerance
   Real                 feasibilityTolerance;
   /// Attribute used to track perts when central differencing
   Integer              currentPertState;

   StringArray          dummyArray;

   // State machine methods
   virtual void                  RunNominal();
   virtual void                  RunPerturbation();
   virtual void                  CalculateParameters();
   virtual void                  CheckCompletion();
   virtual void                  RunComplete();

   void                          FreeArrays();

   virtual void                  WriteToTextFile(
      SolverState stateToUse = UNDEFINED_STATE);
   std::string                   InterpretRetCode(Integer retCode);
   std::string                   GetProgressString();

   // Variables used in Yukon optimizer
   /// Method used to update the Hessian matrix each step
   std::string hessianUpdateMethod;
   /// The maximum number of allowed function evaluations before the optimizer
   /// fails
   Integer maximumFunctionEvals;
   /// The tolerance on the gradient of the Lagrangian, used when determining
   /// whether convergence was achieved
   Real optimalityTolerance;
   /// The tolerance on the change of the cost function, used when determining
   /// whether convergence was achieved
   Real functionTolerance;
   /// The maximum elastic weight to be used if elastic mode is used
   Integer maximumElasticWeight;
   /// The number of iterations completed by the optimizer
   Integer optIterations;
   /// Boolean determining when to send new constraint values to the optimizer
   bool setNewConValues;
   /// Pointer to the optimizer itself
   Yukon                        *runOptimizer;
   /// Pointer to the interface used to send Gmat data to the optimizer
   GmatProblemInterface          *gmatProblem;
};

#endif /*Yukonad_HPP_*/
