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
// Author: Wendy C. Shoan/GSFC
// Created: 2006.07.14
//
/**
 * Implementation for the optimizer base class. 
 *
 */
//------------------------------------------------------------------------------


#include <sstream>
#include "Optimizer.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"     // for GmatMathUtil::Abs()
#include "MessageInterface.hpp"
#include "StringUtil.hpp"

//#define DEBUG_SET_RESULT
//#define DEBUG_SIM_CONVERGENCE
//#define DEBUG_SHOW_CONSTRAINT_DATA

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
Optimizer::PARAMETER_TEXT[OptimizerParamCount -SolverParamCount] =
{
   "ObjectiveFunction",
   "Tolerance",
   "EqualityConstraintNames",
   "InequalityConstraintNames",
   "PlotCost",
   "SourceType",
   "CheckPhysicalTolerances",
};

const Gmat::ParameterType
Optimizer::PARAMETER_TYPE[OptimizerParamCount - SolverParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::STRING_TYPE,
   Gmat::BOOLEAN_TYPE,
};

const Integer Optimizer::EQ_CONST_START   = 1000;
const Integer Optimizer::INEQ_CONST_START = 2000;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

Optimizer::Optimizer(std::string typeName, std::string name) :
   Solver                  (typeName, name),
   sourceType              ("None"),
   objectiveDefined        (false),
   objectiveFnName         (""),
   cost                    (0.0),   // valid value?
   tolerance               (0.0),   // valid value?
   feasibilityTolerance    (0.0),   // Override default in supporting optimizers
   converged               (false),
   eqConstraintCount       (0),
   ineqConstraintCount     (0),
   supportsPhysicalToleranceCheck (false),  // true to support tolerance monitor
   checkPhysicalTolerances (false),
   physicalTolerancesMet   (false)
{
   objectTypeNames.push_back("Optimizer");
   parameterCount = OptimizerParamCount;
}


Optimizer::~Optimizer()
{
   FreeArrays();
}


Optimizer::Optimizer(const Optimizer &opt) :
   Solver                  (opt),
   sourceType              (opt.sourceType),
   objectiveDefined        (false),
   objectiveFnName         (""),
   cost                    (opt.cost), 
   tolerance               (opt.tolerance), 
   feasibilityTolerance    (opt.feasibilityTolerance),
   converged               (false),
   eqConstraintCount       (opt.eqConstraintCount),
   ineqConstraintCount     (opt.ineqConstraintCount),
   supportsPhysicalToleranceCheck (opt.supportsPhysicalToleranceCheck),
   checkPhysicalTolerances (opt.checkPhysicalTolerances),
   physicalTolerancesMet   (false)
{
   eqConstraintNames    = opt.eqConstraintNames;
   ineqConstraintNames  = opt.ineqConstraintNames;
   eqConstraintValues   = opt.eqConstraintValues;
   eqConstraintDesiredValues   = opt.eqConstraintDesiredValues;
   eqConstraintAchievedValues   = opt.eqConstraintAchievedValues;
   eqConstraintOp       = opt.eqConstraintOp;
   eqConstraintTolerances = opt.eqConstraintTolerances;

   ineqConstraintValues = opt.ineqConstraintValues;
   ineqConstraintDesiredValues   = opt.ineqConstraintDesiredValues;
   ineqConstraintAchievedValues   = opt.ineqConstraintAchievedValues;
   ineqConstraintOp     = opt.ineqConstraintOp;
   ineqConstraintTolerances = opt.ineqConstraintTolerances;

   gradient             = opt.gradient;
   jacobian             = opt.jacobian;
   //eqConstraintJacobian = opt.eqConstraintJacobian;
   //ineqConstraintJacobian = opt.ineqConstraintJacobian;
   parameterCount       = opt.parameterCount;
}


//------------------------------------------------------------------------------
// Optimizer& operator=(const Optimizer& opt)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param opt The optimizer copied to this one
 *
 * @return This optimizer configured like opt
 */
//------------------------------------------------------------------------------
Optimizer& Optimizer::operator=(const Optimizer& opt)
{
   if (&opt != this)
   {
      Solver::operator=(opt);
   
      sourceType       = opt.sourceType;
      objectiveFnName  = opt.objectiveFnName;
      cost             = opt.cost;
      tolerance        = opt.tolerance;
      converged        = opt.converged;
   
      feasibilityTolerance = opt.feasibilityTolerance;

      FreeArrays();
      eqConstraintCount    = opt.eqConstraintCount;
      ineqConstraintCount  = opt.ineqConstraintCount;
      eqConstraintNames    = opt.eqConstraintNames;
      ineqConstraintNames  = opt.ineqConstraintNames;
      eqConstraintValues   = opt.eqConstraintValues;
      eqConstraintDesiredValues   = opt.eqConstraintDesiredValues;
      eqConstraintAchievedValues   = opt.eqConstraintAchievedValues;
      eqConstraintOp       = opt.eqConstraintOp;
      eqConstraintTolerances = opt.eqConstraintTolerances;

      ineqConstraintValues = opt.ineqConstraintValues;
      ineqConstraintDesiredValues   = opt.ineqConstraintDesiredValues;
      ineqConstraintAchievedValues   = opt.ineqConstraintAchievedValues;
      ineqConstraintOp     = opt.ineqConstraintOp;
      ineqConstraintTolerances = opt.ineqConstraintTolerances;

      gradient             = opt.gradient;
      jacobian             = opt.jacobian;
      //eqConstraintJacobian = opt.eqConstraintJacobian;
      //ineqConstraintJacobian = opt.ineqConstraintJacobian;
      parameterCount       = opt.parameterCount;

      supportsPhysicalToleranceCheck = opt.supportsPhysicalToleranceCheck;
      checkPhysicalTolerances = opt.checkPhysicalTolerances;
      physicalTolerancesMet   = false;
   }
 
   return *this;
}

bool Optimizer::IsParameterReadOnly(const Integer id) const
{
   if ((id == OBJECTIVE_FUNCTION) ||
       (id == EQUALITY_CONSTRAINT_NAMES) ||
       (id == INEQUALITY_CONSTRAINT_NAMES) ||
       (id == PLOT_COST_FUNCTION))
      return true;

   if (id == SOURCE_TYPE)
      return true;

   if (id == CHECK_PHYSICAL_TOLERANCES)
      return !supportsPhysicalToleranceCheck;
   
   return Solver::IsParameterReadOnly(id);
}


bool Optimizer::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


bool Optimizer::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Initializing Optimizer %s\n", 
         instanceName.c_str());
   #endif

   if (variableNames.size() == 0)  // constraints are not required
   {
      std::string errorMessage = "Optimizer cannot initialize: ";
      errorMessage += "No variables are set.\n";
      throw SolverException(errorMessage);
   }

   FreeArrays();
   Solver::Initialize();
   oldCost = 1.0e99;
   physicalTolerancesMet = false;
   
   return true;
}

//------------------------------------------------------------------------------
// Integer SetSolverResults(Real *data, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets up the data fields used for the results of an iteration.
 * 
 * @param <data> An array of data appropriate to the results used in the
 *               algorithm (for instance, constraints).
 * @param <name> A label for the data parameter.  Defaults to the empty
 *               string.
 * @param <type> string indicating type of result to set
 * 
 * @return The ID used for this variable.
 */
//------------------------------------------------------------------------------
Integer Optimizer::SetSolverResults(Real *data,
                                    const std::string &name,
                                    const std::string &type)
{
   #ifdef DEBUG_SET_RESULT
      MessageInterface::ShowMessage(
         "Optimizer::SetSolverResults - name = %s, type = %s, data = %.16f\n",
         name.c_str(), type.c_str(), data[0]); 
   #endif
   
   if (type == "Objective")
   {
      if (objectiveDefined)
         throw SolverException("Error configuring the \"" + instanceName +
               "\" Optimizer: Multiple objective functions were set using "
               "Minimize commands, but optimizers only support one objective "
               "function.");

      // need to check here if the name is not the same as the 
      // objectiveFnName? (error)
      objectiveDefined = true;
      objectiveFnName = name;
      cost = data[0];
      return 0;
   }
   else if (type == "EqConstraint")
   {
      //if (eqConstraintNames[eqConstraintCount] != name)
      //  throw SolverException(
      //     "Mismatch between parsed and configured equality constraint");
      ///eqConstraintValues[eqConstraintCount] = data[0];
      eqConstraintNames.push_back(name);
      eqConstraintValues.push_back(data[0]);
      eqConstraintDesiredValues.push_back(-1);
      eqConstraintAchievedValues.push_back(-1);
      eqConstraintOp.push_back(0);
      eqConstraintTolerances.push_back(-1.0);
      ++eqConstraintCount;
      return EQ_CONST_START + eqConstraintCount - 1;
    }
    else if (type == "IneqConstraint")
    {
      //if (ineqConstraintNames[ineqConstraintCount] != name)
      //  throw SolverException(
      //     "Mismatch between parsed and configured inequality constraint");
      //ineqConstraintValues[ineqConstraintCount] = data[0];
      ineqConstraintNames.push_back(name);
      ineqConstraintValues.push_back(data[0]);
      ineqConstraintDesiredValues.push_back(-1);
      ineqConstraintAchievedValues.push_back(-1);
      ineqConstraintOp.push_back(0);
      ineqConstraintTolerances.push_back(-1.0);
      ++ineqConstraintCount;
      return INEQ_CONST_START + ineqConstraintCount - 1;
    }
    // add Gradient and Jacobian later ...
    else
    {
       throw SolverException(
           "Unknown type passed in to SetSolverResults");
    }
}

//------------------------------------------------------------------------------
// void SetResultValue(Integer id, Real value, const std::string &resultType = "")
//------------------------------------------------------------------------------
/**
 * Passes in the results obtained from a run in the Optimizer loop.
 * 
 * @param <id>    The ID used for this result.
 * @param <value> The corresponding result.
 */
//------------------------------------------------------------------------------
void Optimizer::SetResultValue(Integer id, Real value, 
                               const std::string &resultType)
{
   #ifdef DEBUG_SET_RESULT // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("In Optimizer::SetResultValue\n");
      MessageInterface::ShowMessage(
      "   id = %d; value = %.12f, resultType = %s\n",
      id, value, resultType.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   if (resultType == "Objective")
   {
      cost = value;
   }
   else if (resultType == "EqConstraint")
   {
      if (id > (EQ_CONST_START + eqConstraintCount))
        throw SolverException(
           "id range error for equality constraint");
     eqConstraintValues[id - EQ_CONST_START] = value;
    }
    else if (resultType == "IneqConstraint")
    {
       if (id > (INEQ_CONST_START + ineqConstraintCount))
        throw SolverException(
           "id range error for inequality constraint");
     ineqConstraintValues[id - INEQ_CONST_START] = value;
    }
    // add Gradient and Jacobian later ...
    else
    {
       throw SolverException(
           "Unknown result type passed in to SetResultValue");
    }
}


//------------------------------------------------------------------------------
// void SetConstraintValues(Integer id, Real desiredValue, Real achievedValue,
//          Integer condition, Real within)
//------------------------------------------------------------------------------
/**
 * Passes in the constraints for the loop
 * 
 * @param id            The ID used for this result.
 * @param desiredValue  The desired result.
 * @param achievedValue The current actual result.
 * @param condition     Type of constraint being evaluated
 * @param within        Optional tolerance for the constraint
 */
//------------------------------------------------------------------------------
void Optimizer::SetConstraintValues(Integer id, Real desiredValue, Real achievedValue,
                                      Integer condition, Real within)
{
   #ifdef DEBUG_SET_RESULT // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("In Optimizer::SetConstraintValues\n");
      MessageInterface::ShowMessage(
      "   id = %d; desired = %.12f, achieved = %.12f, condition = %d\n",
      id, desiredValue, achievedValue, condition);
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   if (condition == 0)
   {
      if (id > (EQ_CONST_START + eqConstraintCount))
         throw SolverException(
            "id range error for equality constraint");
      eqConstraintDesiredValues[id - EQ_CONST_START] = desiredValue;
      eqConstraintAchievedValues[id - EQ_CONST_START] = achievedValue;
      eqConstraintOp[id - EQ_CONST_START] = condition;
      if (within > 0.0)
         eqConstraintTolerances[id - EQ_CONST_START] = within;
      else
         eqConstraintTolerances[id - EQ_CONST_START] = feasibilityTolerance;
   }
   else 
   {
      if (id > (INEQ_CONST_START + ineqConstraintCount))
      throw SolverException(
         "id range error for inequality constraint");
      ineqConstraintDesiredValues[id - INEQ_CONST_START] = desiredValue;
      ineqConstraintAchievedValues[id - INEQ_CONST_START] = achievedValue;
      ineqConstraintOp[id - INEQ_CONST_START] = condition;
      if (within > 0.0)
         ineqConstraintTolerances[id - INEQ_CONST_START] = within;
      else
         ineqConstraintTolerances[id - INEQ_CONST_START] = feasibilityTolerance;
   }
}

//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string that reporting the current differential corrector state.
 */
//------------------------------------------------------------------------------
std::string Optimizer::GetProgressString()
{
   StringArray::iterator current;
   Integer i;
   std::stringstream progress;
   progress.str("");
   progress.precision(12);

   if (isInitialized)
   {
      switch (currentState)
      {
         case INITIALIZING:
            // This state is basically a "paused state" used for the Optimize
            // command to finalize the initial data for the variables and
            // goals.  All that is written here is the header information.
            {
               Integer localVariableCount = variableNames.size();
               Integer localEqCount       = eqConstraintNames.size();
               Integer localIneqCount     = ineqConstraintNames.size();
               progress << "************************************************"
                        << "********\n"
                        << "*** Performing "
                        << typeName 
                        << " Optimization "
                        << "(using \"" << instanceName << "\")\n";

               // Write out the setup data
               progress << "*** " << localVariableCount << " variables; "
                        << localEqCount << " equality constraints; "
                        << localIneqCount << " inequality constraints\n   Variables:  ";

               // Iterate through the variables and goals, writing them to
               // the file
               for (current = variableNames.begin(), i = 0;
                    current != variableNames.end(); ++current)
               {
                  if (current != variableNames.begin())
                     progress << ", ";
                  progress << *current;
               }

               if (localEqCount > 0)
               {
                  progress << "\n   Equality Constraints:  ";
   
                  for (current = eqConstraintNames.begin(), i = 0;
                       current != eqConstraintNames.end(); ++current)
                  {
                     if (current != eqConstraintNames.begin())
                        progress << ", ";
                     progress << *current;
                  }
               }
               
               if (localIneqCount > 0)
               {
                  progress << "\n   Inequality Constraints:  ";
   
                  for (current = ineqConstraintNames.begin(), i = 0;
                       current != ineqConstraintNames.end(); ++current)
                  {
                     if (current != ineqConstraintNames.begin())
                        progress << ", ";
                     progress << *current;
                  }
               }
               
               progress << "\n****************************"
                        << "****************************";
            }
            break;

         case NOMINAL:
            progress << instanceName << " Iteration " << iterationsTaken+1
                     << "; Nominal Pass\n   Variables:  ";
            // Iterate through the variables, writing them to the string
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               if (current != variableNames.begin())
                  progress << ", ";
               progress << *current << " = " << variable[i++];
            }
            break;

         case PERTURBING:  // does this apply to optimization??
            progress << "   Completed iteration " << iterationsTaken
                     << ", pert " << pertNumber+1 << " ("
                     << variableNames[pertNumber] << " = "
                     << variable[pertNumber] << ")\n";
            break;

         case CALCULATING:
            // Just forces a blank line
            break;

         case CHECKINGRUN:
            // Iterate through the constraints, writing them to the file
            progress << "   Equality Constraints and achieved values:\n      ";

            for (current = eqConstraintNames.begin(), i = 0;
                 current != eqConstraintNames.end(); ++current)
            {
               if (current != eqConstraintNames.begin())
                  progress << ",  ";
                  // does this make sense???
               //progress << *current << "  Desired: " << eqConstaint[i]
               //         << "  Achieved: " << nominal[i];
               ++i;
            }

           progress << "   Inequality Constraints and achieved values:\n      ";

            for (current = ineqConstraintNames.begin(), i = 0;
                 current != ineqConstraintNames.end(); ++current)
            {
               if (current != ineqConstraintNames.begin())
                  progress << ",  ";
                  // does this make sense???
               //progress << *current << "  Desired: " << eqConstaint[i]
               //         << "  Achieved: " << nominal[i];
               ++i;
            }

            break;

         case RUNEXTERNAL:
            progress << instanceName << " Iteration " << iterationsTaken+1
                     << "; External Run\n   Variables:  ";
            // Iterate through the variables, writing them to the string
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               if (current != variableNames.begin())
                  progress << ", ";
               progress << *current << " = " << variable[i++];
            }
            break;

         case FINISHED:
            if (converged)
               progress << "\n*** Optimization Completed in " << iterationsTaken
                        << " iterations\n*** The Optimizer Converged!";
            else
               progress << "\n*** Optimization did not converge in "
                        << iterationsTaken
                        << " iterations";
                     
            if ((iterationsTaken >= maxIterations) && (converged == false))
               progress << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                        << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << "!!! WARNING: Optimizer did NOT converge in "
                        << maxIterations << " iterations!"
                        << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                        << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
            
            progress << "\nFinal Variable values:\n";
            // Iterate through the variables, writing them to the string
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
               progress << "   " << *current << " = " << variable[i++] << "\n";
            break;

         case ITERATING:     // Intentional fall through
         default:
            throw SolverException(
               "Solver state not supported for the targeter");
      }
   }
   else
      return Solver::GetProgressString();
      
   return progress.str();

}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Optimizer::GetParameterText(const Integer id) const
{
   if ((id >= SolverParamCount) && (id < OptimizerParamCount))
      return PARAMETER_TEXT[id - SolverParamCount];
   return Solver::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer Optimizer::GetParameterID(const std::string &str) const
{
   for (Integer i = SolverParamCount; i < OptimizerParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - SolverParamCount])
         return i;
   }

   return Solver::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Optimizer::GetParameterType(
                                              const Integer id) const
{
   if ((id >= SolverParamCount) && (id < OptimizerParamCount))
      return PARAMETER_TYPE[id - SolverParamCount];

   return Solver::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Optimizer::GetParameterTypeString(
                                      const Integer id) const
{
   return Solver::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Optimizer::GetRealParameter(const Integer id) const
{
   if (id == OPTIMIZER_TOLERANCE)               return tolerance;

   return Solver::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return  Real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Optimizer::SetRealParameter(const Integer id, const Real value)
{
   if (id == OPTIMIZER_TOLERANCE)
   {
      if (value <= 0.0)
         throw SolverException(
               "The value entered for the optimizer tolerance on " +
               instanceName + " is not an allowed value. The allowed value "
               "is: [Real > 0.0].");

      tolerance = value;
      if (feasibilityTolerance <= 0,0)
         feasibilityTolerance   = tolerance;
      return tolerance;
   }
   return Solver::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Optimizer::GetStringParameter(const Integer id) const
{
    if (id == OBJECTIVE_FUNCTION)
        return objectiveFnName;
    if (id == SOURCE_TYPE)
       return sourceType;
    
    return Solver::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  Integer SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets a string or string array parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> string value for the parameter.
 *
 * @return  The value of the parameter at the completion of the call.
 */
//------------------------------------------------------------------------------
bool Optimizer::SetStringParameter(const Integer id,
                                   const std::string &value)
{
    if (id == OBJECTIVE_FUNCTION) 
    {
        objectiveFnName = value;
        return true;
    }
        
    
    return Solver::SetStringParameter(id, value);
}

// compiler complained again - so here they are ....
std::string Optimizer::GetStringParameter(const std::string& label) const
{
   return Solver::GetStringParameter(label);
}
bool Optimizer::SetStringParameter(const std::string& label,
                                           const std::string &value)
{
   return Solver::SetStringParameter(label, value);
}
std::string Optimizer::GetStringParameter(const Integer id,
                                                  const Integer index) const
{
   return Solver::GetStringParameter(id, index);
}

bool Optimizer::SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index)
{
   return Solver::SetStringParameter(id, value, index);
}

std::string Optimizer::GetStringParameter(const std::string &label,
                                                  const Integer index) const
{
   return Solver::GetStringParameter(label, index);
}

bool Optimizer::SetStringParameter(const std::string &label, 
                                           const std::string &value,
                                           const Integer index)
{
   return Solver::SetStringParameter(label, value, index);
}

//------------------------------------------------------------------------------
//  std::string  GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 */
//------------------------------------------------------------------------------
const StringArray& Optimizer::GetStringArrayParameter(
                                                        const Integer id) const
{
        
    if (id == EQUALITY_CONSTRAINT_NAMES)
        return eqConstraintNames;
    if (id == INEQUALITY_CONSTRAINT_NAMES)
        return ineqConstraintNames;
        
    return Solver::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Boolean field access method
 *
 * @param id The ID of the field
 *
 * @return The field setting
 */
//------------------------------------------------------------------------------
bool Optimizer::GetBooleanParameter(const Integer id) const
{
   if (id == CHECK_PHYSICAL_TOLERANCES)
      return checkPhysicalTolerances;

   return Solver::GetBooleanParameter(id);
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Boolean field setting method
 *
 * @param id The ID of the field
 * @param value The new field setting
 *
 * @return The (new) field setting
 */
//------------------------------------------------------------------------------
bool Optimizer::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == CHECK_PHYSICAL_TOLERANCES)
   {
      checkPhysicalTolerances = value;
      return checkPhysicalTolerances;
   }

   return Solver::SetBooleanParameter(id, value);
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Boolean array member access method
 *
 * @note: This method is a pass through to the parent class
 *
 * @param id The ID of the field
 * @param index The index into the Boolean array
 *
 * @return The array member setting
 */
//------------------------------------------------------------------------------
bool Optimizer::GetBooleanParameter(const Integer id, const Integer index) const
{
   // Note: Solver is missing the pass-through
   // return Solver::GetBooleanParameter(id, index);
   return GmatBase::GetBooleanParameter(id, index);
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Boolean array member access method
 *
 * @note: This method is a pass through to the parent class
 *
 * @param id The ID of the field
 * @param value The new member setting
 * @param index The index into the Boolean array
 *
 * @return The array member setting
 */
//------------------------------------------------------------------------------
bool Optimizer::SetBooleanParameter(const Integer id, const bool value, const Integer index)
{
   // Note: Solver is missing the pass-through
   // return Solver::SetBooleanParameter(id, value, index);
   return GmatBase::SetBooleanParameter(id, value, index);
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Boolean field access method
 *
 * @param label The text label of the field
 *
 * @return The field setting
 */
//------------------------------------------------------------------------------
bool Optimizer::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Boolean field setting method
 *
 * @param label The text label of the field
 * @param value The new field setting
 *
 * @return The (new) field setting
 */
//------------------------------------------------------------------------------
bool Optimizer::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Boolean array member access method
 *
 * @note: This method is a pass through to the parent class
 *
 * @param label The text label of the field
 * @param index The index into the Boolean array
 *
 * @return The array member setting
 */
//------------------------------------------------------------------------------
bool Optimizer::GetBooleanParameter(const std::string &label, const Integer index) const
{
   return GetBooleanParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Boolean array member access method
 *
 * @note: This method is a pass through to the parent class
 *
 * @param label The text label of the field
 * @param value The new member setting
 * @param index The index into the Boolean array
 *
 * @return The array member setting
 */
//------------------------------------------------------------------------------
bool Optimizer::SetBooleanParameter(const std::string &label, const bool value, const Integer index)
{
   return SetBooleanParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * This method performs an action on the instance.
 *
 * TakeAction is a method overridden from GmatBase.  The only actions defined for
 * an Optimizer are "IncrementInstanceCount", which the Sandbox uses
 * to tell an instance if if it is a reused instance (i.e. a clone) of the
 * configured instance of the Optimizer; and "Reset" which resets the
 * optimizer data.
 *
 * @param <action>      Text label for the action.
 * @param <actionData>  Related action data, if needed.
 *
 * @return  The value of the parameter at the completion of the call.
 */
//------------------------------------------------------------------------------
bool Optimizer::TakeAction(const std::string &action,
                                       const std::string &actionData)
{
   if (action == "ResetInstanceCount")
   {
      instanceNumber = 0;
      return true;
   }

   if (action == "IncrementInstanceCount")
   {
      ++instanceNumber;
      return true;
   }
 
   if (action == "Reset")
   {
      currentState = INITIALIZING;
   }

   return Solver::TakeAction(action, actionData);
}

//------------------------------------------------------------------------------
//  std::string GetJacobianString()
//------------------------------------------------------------------------------
/**
* This method generates the string of the current Jacobian values.
*
* This method is called by internal optimizers when printing to the output
* file when the verbose report style is used.
*
* @return jacString The string containing the Jacobian matrix.
*/
//------------------------------------------------------------------------------
std::string Optimizer::GetJacobianString()
{
   std::stringstream jacString;
   jacString.str("");
   jacString.precision(12);

   jacString << "\n   Jacobian:\n      ";

   for (Integer i = 0; i < variableCount; ++i)
      jacString << gradient.at(i) << "    ";

   jacString << "\n      ";
   for (Integer j = 0; j < eqConstraintCount + ineqConstraintCount; ++j)
   {
      for (Integer i = 0; i < variableCount; ++i)
      {
         if (j < eqConstraintCount)
            jacString << jacobian.at(i + variableCount * j) << "    ";
         else
            jacString << -jacobian.at(i + variableCount * j) << "    ";
      }
      jacString << "\n      ";
   }
   jacString << "\n";

   return jacString.str();
}


//------------------------------------------------------------------------------
// const RealArray* GetSolverData(const std::string type)
//------------------------------------------------------------------------------
/**
 *  Method used to pull data for reporting from other objects
 *
 *  This method retrieves the current values for Solver fields, so that they can be
 *  exposed elsewhere.  The Solver Window uses it to retrieve values for the VF13
 *  optimizer.
 *
 *  @param type The type of data requested
 *
 *  @return The numbers that match the type
 */
//------------------------------------------------------------------------------
const RealArray* Optimizer::GetSolverData(const std::string &type)
{
   RealArray *retval = nullptr;

   if (type == "IneqConstraints")
   {
      ineqConstraintAchievedValues.clear();
      for (UnsignedInt i = 0; i < ineqConstraintValues.size(); ++i)
      {
         if (ineqConstraintOp[i] == 1)
            ineqConstraintAchievedValues.push_back(ineqConstraintDesiredValues.at(i) - ineqConstraintValues[i]);
         else
            ineqConstraintAchievedValues.push_back(ineqConstraintValues.at(i) + ineqConstraintDesiredValues[i]);
      }
      return &ineqConstraintAchievedValues;
   }
   if (type == "EqConstraints")
   {
      eqConstraintAchievedValues.clear();
      for (UnsignedInt i = 0; i < eqConstraintValues.size(); ++i)
      {
         eqConstraintAchievedValues.push_back(eqConstraintValues.at(i) + eqConstraintDesiredValues[i]);
      }
      return &eqConstraintAchievedValues;
   }
   if (type == "IneqConstraintsDeltas")
      return &ineqConstraintValues;
   if (type == "EqConstraints")
      return &eqConstraintValues;

   return retval;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void FreeArrays()
//------------------------------------------------------------------------------
/**
 * Frees (clears) the constraints used by the optimizer.  This method is also 
 * called by the destructor when the script is cleared.
 */
//------------------------------------------------------------------------------
void Optimizer::FreeArrays()
{
   Solver::FreeArrays();
   //eqConstraintNames.clear(); ????? do I need to do this?
   //ineqConstraintNames.clear(); ?????
   //eqConstraintCount - 0; ?????
   //ineqConstraintCount - 0; ?????
   eqConstraintValues.clear();
   eqConstraintDesiredValues.clear();
   eqConstraintAchievedValues.clear();
   eqConstraintOp.clear();
   ineqConstraintValues.clear();
   ineqConstraintDesiredValues.clear();
   ineqConstraintAchievedValues.clear();
   ineqConstraintOp.clear();
    
}


//------------------------------------------------------------------------------
//  void ReportProgress()
//------------------------------------------------------------------------------
/**
 * Shows the progress string to the user.
 *
 * This default version just passes the progress string to the MessageInterface.
 */
//------------------------------------------------------------------------------
void Optimizer::ReportProgress(const SolverState forState)
{
   Solver::ReportProgress(forState);
}


//------------------------------------------------------------------------------
//  void ReportProgress()
//------------------------------------------------------------------------------
/**
 * Send to all listeners a progress report
 *
 */
//------------------------------------------------------------------------------
void Optimizer::ReportProgress(std::list<ISolverListener*> listeners, const SolverState forState)
{
   Solver::ReportProgress(listeners, forState);
}


//------------------------------------------------------------------------------
//  void ReportProgress()
//------------------------------------------------------------------------------
/**
 * Send to the listener a progress report
 */
//------------------------------------------------------------------------------
void Optimizer::ReportProgress(ISolverListener* listener, const SolverState forState)
{
   StringArray::iterator current;
   Integer i;

   const RealArray *deltavalues;
   RealArray values;

   if (isInitialized)
   {
      SolverState useState = forState;
      if (useState == UNDEFINED_STATE)
         useState = currentState;

      switch (useState)
      {
         case NOMINAL:
            // Iterate through the variables, notifying the listener
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               listener->VariabledChanged(*current, unscaledVariable.at(i));
               ++i;
            }
            break;

         case RUNEXTERNAL:
            // Iterate through the variables, notifying the listener
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               listener->VariabledChanged(*current, unscaledVariable.at(i));
               ++i;
            }
            // fall through

         case CHECKINGRUN:
             // Iterate through the equality constraints, notifying the listener
             if (eqConstraintCount > 0)
             {
                values.clear();
                deltavalues = GetSolverData("EqConstraints");
                if (deltavalues == NULL)
                   deltavalues = &eqConstraintAchievedValues;
                for (current = eqConstraintNames.begin(), i = 0;
                     current != eqConstraintNames.end(); ++current)
                {
                   #ifdef DEBUG_SET_RESULT
                      MessageInterface::ShowMessage(
                         "Optimizer::ReportProgress CHECKINGRUN: Eqname = %s, desired = %.16f, achieved = %.16f, delta = %.16f, expected =  %.16f\n",
                         (*current).c_str(), eqConstraintDesiredValues[i], eqConstraintAchievedValues[i],
                         eqConstraintDesiredValues[i]-eqConstraintAchievedValues[i], eqConstraintValues[i]);
                   #endif
                   listener->ConstraintChanged(*current, eqConstraintDesiredValues[i], deltavalues->at(i));
                   ++i;
                }
             }

             if (ineqConstraintCount > 0)
             {
                values.clear();
                deltavalues = GetSolverData("IneqConstraints");
                if (deltavalues == NULL) deltavalues = &ineqConstraintAchievedValues;

                // Iterate through the inequality constraints, notifying the listener
                for (current = ineqConstraintNames.begin(), i = 0;
                     current != ineqConstraintNames.end(); ++current)
                {
                   #ifdef DEBUG_SET_RESULT
                      MessageInterface::ShowMessage(
                         "Optimizer::ReportProgress CHECKINGRUN: inEqname = %s, desired = %.16f, achieved = %.16f, delta = %.16f, expected =  %.16f\n",
                         (*current).c_str(), ineqConstraintDesiredValues[i], ineqConstraintAchievedValues[i],
                         ineqConstraintDesiredValues[i]-ineqConstraintAchievedValues[i], ineqConstraintValues[i]);
                   #endif
                   listener->ConstraintChanged(*current, ineqConstraintDesiredValues[i],
                      deltavalues->at(i), ineqConstraintOp[i]);
                   ++i;
                }
             }

             if (objectiveDefined)
                listener->ObjectiveChanged("Cost", cost);
             break;

         case FINISHED:
            // Iterate through the variables, notifying the listener
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               listener->VariabledChanged(*current, unscaledVariable.at(i));
               ++i;
            }
            if (converged)
               listener->Convergence(converged, "Optimization Completed in " + 
                  GmatStringUtil::Trim( GmatStringUtil::ToString(iterationsTaken) ) +
                  " passes through the Solver Control Sequence");
            else
               listener->Convergence(converged, "Optimization did not converge in " + 
                  GmatStringUtil::Trim( GmatStringUtil::ToString(iterationsTaken) ) +
                  " passes through the Solver Control Sequence");
            break;

         default:
            #ifdef DEBUG_SET_RESULT
               std::cout << "Default branch taken in Optimizer::ReportProgress\n";
            #endif
            break;
      }
   }
}


//------------------------------------------------------------------------------
// bool PerformToleranceCheck()
//------------------------------------------------------------------------------
/**
 * Check real world values of the constraints and minimization history
 *
 * @return true if everything is in tolerance
 */
//------------------------------------------------------------------------------
bool Optimizer::PerformToleranceCheck()
{
   #ifdef DEBUG_SIM_CONVERGENCE
      MessageInterface::ShowMessage("Checking for physical convergence\n");

      for (UnsignedInt i = 0; i < eqConstraintTolerances.size(); ++i)
         MessageInterface::ShowMessage("Eq[%d} = %lf\n", i,
               eqConstraintTolerances[i]);

      for (UnsignedInt i = 0; i < ineqConstraintTolerances.size(); ++i)
         MessageInterface::ShowMessage("Ineq[%d} = %lf\n", i,
               ineqConstraintTolerances[i]);

      for (UnsignedInt i = 0; i < eqConstraintDesiredValues.size(); ++i)
      {
         MessageInterface::ShowMessage("   %lf v %lf\n",
               eqConstraintDesiredValues[i], eqConstraintAchievedValues[i]);
      }
   #endif

   bool retval = false;

   bool isMinimal     = false;

   // Check the objective function if minimizing
   if (objectiveDefined)
   {
      #ifdef DEBUG_SIM_CONVERGENCE
         Real magG = 0.0;
         for (Integer i = 0; i < variableCount; ++i)
            magG += gradient.at(i)*gradient.at(i);
         magG = sqrt(magG);
         MessageInterface::ShowMessage("   Cost: %le   Previous: %le   "
               "Diff: %lf   Gradient: %le   Tolerance: %lf\n", cost, oldCost,
               (cost - oldCost), magG, tolerance);

         MessageInterface::ShowMessage("Optimization cost: %lf, old cost: %lf, "
               "diff:  %lf tolerance: %lf\n", cost, oldCost, fabs(cost - oldCost),
               tolerance);
      #endif

      if (fabs(cost - oldCost) <= tolerance)
         isMinimal = true;
      oldCost = cost;
   }
   else
      // If not minimizing, enable constraint checks
      isMinimal = true;

   #ifdef DEBUG_SIM_CONVERGENCE
      MessageInterface::ShowMessage("Minimal: %s\n", (isMinimal ? "true" : "false"));
   #endif

   RealArray deltaValues;
   bool eqSatisfied = true;
   bool ineqSatisfied = true;

   if (isMinimal)
   {
      #ifdef DEBUG_SIM_CONVERGENCE
         MessageInterface::ShowMessage("%d eq targets; %d ineq targets\n",
               eqConstraintDesiredValues.size(), ineqConstraintDesiredValues.size());
         MessageInterface::ShowMessage("%d eq values; %d ineq values\n",
               eqConstraintAchievedValues.size(), ineqConstraintAchievedValues.size());
         MessageInterface::ShowMessage("%d eq tolerances; %d ineq tolerances\n",
               eqConstraintTolerances.size(), ineqConstraintTolerances.size());
      #endif

      #ifdef DEBUG_SHOW_CONSTRAINT_DATA
          // Walk equality constraints.  Any out of tolerance, clear flag and break
         for (UnsignedInt i = 0; i < eqConstraintDesiredValues.size(); ++i)
            MessageInterface::ShowMessage("EQ Desired: %lf, "
                  "Achieved: %lf\n",
                  eqConstraintDesiredValues[i],
                  eqConstraintAchievedValues[i]);
         for (UnsignedInt i = 0; i < ineqConstraintDesiredValues.size(); ++i)
            MessageInterface::ShowMessage("INEQ Desired: %lf, "
                  "Achieved: %lf\n",
                  ineqConstraintDesiredValues[i],
                  ineqConstraintAchievedValues[i]);
      #endif

      // Walk equality constraints.  Any out of tolerance, clear flag and break
      for (UnsignedInt i = 0; i < eqConstraintDesiredValues.size(); ++i)
      {
         #ifdef DEBUG_SIM_CONVERGENCE
            MessageInterface::ShowMessage("Checking eq %d: Desired: %lf, "
                  "Achieved: %lf, Diff: %lf\n", i,
                  eqConstraintDesiredValues[i],
                  eqConstraintAchievedValues[i],
                  eqConstraintDesiredValues[i] - eqConstraintAchievedValues[i]);
            MessageInterface::ShowMessage("   Test Value: %le > %le?\n",
                  eqConstraintDesiredValues[i] - eqConstraintDesiredValues[i], eqConstraintTolerances[i]);
         #endif

         if (fabs(eqConstraintDesiredValues[i] - eqConstraintAchievedValues[i]) > eqConstraintTolerances[i])
         {
            eqSatisfied = false;
            #ifdef DEBUG_SIM_CONVERGENCE
               MessageInterface::ShowMessage("Eq %d failed: %lf - %lf = %lf > %lf\n", i,
                     eqConstraintDesiredValues[i], eqConstraintAchievedValues[i],
                     eqConstraintDesiredValues[i] - eqConstraintAchievedValues[i],
                     eqConstraintTolerances[i]);
            #endif
            break;
         }
      }

      if (eqSatisfied)
      {
         // Walk inequality constraints.  Any out of tolerance, clear flag and break
         for (UnsignedInt i = 0; i < ineqConstraintDesiredValues.size(); ++i)
         {
            Real testValue = ineqConstraintDesiredValues[i] - ineqConstraintAchievedValues[i];
            if (ineqConstraintOp[i] == -1)   // Handle <= / >= settign
               testValue *= -1.0;

            #ifdef DEBUG_SIM_CONVERGENCE
               MessageInterface::ShowMessage("Checking ineq %d: Desired: %lf, "
                     "Achieved: %lf, Diff: %lf\n", i,
                     ineqConstraintDesiredValues[i],
                     ineqConstraintAchievedValues[i],
                     ineqConstraintDesiredValues[i] - ineqConstraintAchievedValues[i]);
               MessageInterface::ShowMessage("   Test Value: %le > %le?\n",
                     testValue, ineqConstraintTolerances[i]);
            #endif

            if (testValue > ineqConstraintTolerances[i])
            {
               ineqSatisfied = false;
               #ifdef DEBUG_SIM_CONVERGENCE
                  MessageInterface::ShowMessage("Ineq %d failed: %lf > %lf\n", i,
                        testValue, ineqConstraintTolerances[i]);
               #endif
               break;
            }
         }
      }
   }

   #ifdef DEBUG_SIM_CONVERGENCE
      MessageInterface::ShowMessage("Minimal: %s\n", (isMinimal ? "true" : "false"));
      MessageInterface::ShowMessage("Eq Met: %s\n", (eqSatisfied ? "true" : "false"));
      MessageInterface::ShowMessage("Ineq Met: %s\n", (ineqSatisfied ? "true" : "false"));
   #endif

   if (isMinimal && eqSatisfied && ineqSatisfied)
   {
      MessageInterface::ShowMessage("*** Optimization meets scripted tolerances "
            "for actual constraint and objective function values ***\n");

      currentState = FINISHED;
      converged = true;
      status = Gmat::IN_TOLERANCE;
      physicalTolerancesMet = true;

      retval = true;
   }
   else
   {
      #ifdef DEBUG_SIM_CONVERGENCE
         MessageInterface::ShowMessage("Optimization does not yet meet scripted "
               "tolerances for actual constraint and objective function values\n");
      #endif

      // Update the state for internal optimizers
      if (isInternal)
         currentState = NOMINAL;
   }

   return retval;
}
