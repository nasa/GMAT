//$Id$
//------------------------------------------------------------------------------
//                         Optimizer
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
// number NNG04CC06P
//
// Author: Wendy C. Shoan, GSFC
// Created: 2006.07.12
//
/**
 * Definition for the Optimizer base class. 
 */
//------------------------------------------------------------------------------


#ifndef Optimizer_hpp
#define Optimizer_hpp

#include "Solver.hpp"

class GMAT_API Optimizer : public Solver
{
public:
   Optimizer(std::string typeName, std::string name);
   virtual ~Optimizer();
   Optimizer(const Optimizer &opt);
   Optimizer&      operator=(const Optimizer& opt);
   
   virtual bool        Initialize();
   //virtual SolverState         AdvanceState();  // do I need this?

   // Solver interfaces used to talk to the Vary and Minimize commands
   //virtual Integer     SetSolverVariables(Real *data, const std::string &name);
   //virtual Real        GetSolverVariable(Integer id);
   virtual Integer     SetSolverResults(Real *data, const std::string &name,
                                        const std::string &type = "");
   virtual void        SetResultValue(Integer id, Real value,
                                      const std::string &resultType = "");

   virtual void        SetConstraintValues(Integer id, Real desiredValue, Real achievedValue,
                                      Integer condition = 0, Real within = -1.0);
   virtual void        ReportProgress(const SolverState forState = UNDEFINED_STATE);
   virtual void        ReportProgress(std::list<ISolverListener*> listeners, const SolverState forState);
   virtual void        ReportProgress(ISolverListener* listener, const SolverState forState = UNDEFINED_STATE);


   virtual std::string GetProgressString();

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;


   // Access methods overridden from the base class

   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);

   //virtual Integer     GetIntegerParameter(const Integer id) const;
   //virtual Integer     SetIntegerParameter(const Integer id,
   //                                        const Integer value);
   //virtual bool        GetBooleanParameter(const Integer id) const;
   //virtual bool        SetBooleanParameter(const Integer id,
   //                                        const bool value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   // compiler complained again - so here they are ....
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value);
   virtual std::string GetStringParameter(const Integer id,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value,
                                          const Integer index);
   virtual std::string GetStringParameter(const std::string &label,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const std::string &label, 
                                          const std::string &value,
                                          const Integer index);
   virtual const StringArray&
                       GetStringArrayParameter(const Integer id) const;

   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const Integer id,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value,
                                            const Integer index);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label,
                                            const Integer index) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value,
                                            const Integer index);

   virtual bool        TakeAction(const std::string &action,
                                  const std::string &actionData = "");
   std::string GetJacobianString();

   virtual const RealArray*
                       GetSolverData(const std::string &type);


//------------------------------------------------------------------------------
//  bool  Optimize()    <pure virtual>
//------------------------------------------------------------------------------
/**
 * This method performs the optimization.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
   virtual bool        Optimize() = 0;

protected:
   // Parameter IDs
   enum
   {
      OBJECTIVE_FUNCTION = SolverParamCount,
      OPTIMIZER_TOLERANCE,
      EQUALITY_CONSTRAINT_NAMES,
      INEQUALITY_CONSTRAINT_NAMES,
      PLOT_COST_FUNCTION,
      SOURCE_TYPE,
      CHECK_PHYSICAL_TOLERANCES,
      OptimizerParamCount
   };
   
   /// type of external interface used
   std::string       sourceType;
   /// Flag that is toggled when a Minimize command registers itself
   bool              objectiveDefined;
   /// name of the objective function
   std::string       objectiveFnName;
   /// latest value obtained for the objective function
   Real              cost;
   /// Previous value obtained for the objective function
   Real              oldCost;
   /// tolerance for determining convergence - the optimizer has converged 
   /// on a solution when the magnitude of the gradient of the cost function 
   /// is smaller than this tolerance
   Real              tolerance;
   /// Parameter added to force constraint matching to specified tolerance
   Real              feasibilityTolerance;      // Not supported everywhere
   /// flag indicating when the optimizer has reached an acceptable 
   /// value for the objective function
   bool              converged;

   /// the number of equality constraints defined for this optimization
   Integer           eqConstraintCount;
   /// the number of inequality constraints defined for this optimization
   Integer           ineqConstraintCount;
   /// names of the equality constraint variables
   StringArray       eqConstraintNames;
   /// names of the inequality constraint variables
   StringArray       ineqConstraintNames;

   /// array of equality constraint values
   std::vector<Real> eqConstraintValues;
   /// array of desired equality constraint values
   std::vector<Real> eqConstraintDesiredValues;
   /// array of achieved equality constraint values
   std::vector<Real> eqConstraintAchievedValues;
   /// Unscaled equality constraint tolerances
   std::vector<Real> eqConstraintTolerances;
   /// Operator used for the comparisons
   std::vector<Integer> eqConstraintOp;

   /// array of inequality constraint values
   std::vector<Real> ineqConstraintValues;
   /// array of inequality constraint limits
   std::vector<Real> ineqConstraintDesiredValues;
   /// array of current inequality constraint values
   std::vector<Real> ineqConstraintAchievedValues;
   /// Unscaled inequality constraint tolerances
   std::vector<Real> ineqConstraintTolerances;
   /// Operator used for the inequality comparisons
   std::vector<Integer> ineqConstraintOp;

   /// most recently calculated gradient of the objective function
   std::vector<Real> gradient;   // ********* future ***********
   std::vector<Real> jacobian;
   /// most recently calculated Jacobian of the equality constraints
   Rmatrix           eqConstraintJacobian;  // size of this?
   /// most recently calculated Jacobian of the inequality constraints
   Rmatrix           ineqConstraintJacobian;  // size of this?
   /// Flag used to identify optimizers that monitor the optimization process
   bool              supportsPhysicalToleranceCheck;
   /// Flag used for optimizer "real world" check of constraints and cost
   bool              checkPhysicalTolerances;
   /// Flag set when physical tolerances are satisfied
   bool              physicalTolerancesMet;

   static const std::string    PARAMETER_TEXT[OptimizerParamCount -
                                              SolverParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[OptimizerParamCount -
                                              SolverParamCount];

   virtual bool PerformToleranceCheck();

   static const Integer        EQ_CONST_START;
   static const Integer        INEQ_CONST_START;
 
   virtual void      FreeArrays();
};

#endif // Optimizer_hpp
