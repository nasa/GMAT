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
// Created: 2017, Steven Hughes
//
// Converted: 2017/10/12, Joshua Raymond, Thinking Systems, Inc.
//
/**
 * Implementation for the Yukonad optimizer.
 *
 * This is prototype code.  Interested parties need to roll the Yukonad optimizer
 * code into a separate library and link to it.  Contact Thinking Systems for
 * details.
 */
//------------------------------------------------------------------------------


#include <sstream>
#include "Yukonad.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_Yukonad
//#define YUKON_DEBUG_STATE_MACHINE
//#define DEBUG_YUKON_INIT
//#define DEBUG_YUKON_CALL
//#define DEBUG_YUKON_PERT

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string
Yukonad::PARAMETER_TEXT[YukonadParamCount - OptimizerParamCount] =
{
   "goalName",
   "UseCentralDifferences",
   "FeasibilityTolerance",
   "HessianUpdateMethod",
   "MaximumFunctionEvals",
   "OptimalityTolerance",
   "FunctionTolerance",
   "MaximumElasticWeight"
};

const Gmat::ParameterType
Yukonad::PARAMETER_TYPE[YukonadParamCount - OptimizerParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::REAL_TYPE,
   Gmat::ENUMERATION_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::INTEGER_TYPE
};

const std::string
Yukonad::HESSIAN_UPDATE_METHOD[MaxUpdateMethod - DampedBFGS] =
{
   "DampedBFGS",
   "SelfScaledBFGS"
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Yukonad(const std::string &name) :
//------------------------------------------------------------------------------
/**
* Constructor
*
* @param name The name of the constructed object
*/
//------------------------------------------------------------------------------
Yukonad::Yukonad(const std::string &name) :
InternalOptimizer("Yukon", name),
objectiveSet(false),
retCode(-101),
useCentralDifferences(false),
feasibilityTolerance(1.0e-4),
hessianUpdateMethod("SelfScaledBFGS"),
maximumFunctionEvals(1000),
optimalityTolerance(1.0e-4),
functionTolerance(1.0e-4),
maximumElasticWeight(10000),
optIterations(0),
currentPertState(0),
gmatProblem(NULL),
runOptimizer(NULL)
{
   objectTypeNames.push_back("Yukon");
   objectiveFnName = "SDObjective";
   tolerance = 1.0e-5;
   maxIterations = 200;
   parameterCount = YukonadParamCount;
   AllowRangeLimits = false;

   isInitialized = false;
   setNewConValues = false;
}


//------------------------------------------------------------------------------
// ~Yukonad()
//------------------------------------------------------------------------------
/**
* Destructor
*/
//------------------------------------------------------------------------------
Yukonad::~Yukonad()
{
   if (runOptimizer)
      delete runOptimizer;
}


//------------------------------------------------------------------------------
// Yukonad(const Yukonad& sd)
//------------------------------------------------------------------------------
/**
* Copy constructor
*
* @param sd The instance copied here
*/
//------------------------------------------------------------------------------
Yukonad::Yukonad(const Yukonad& sd) :
InternalOptimizer(sd),
objectiveSet(sd.objectiveSet),
retCode(sd.retCode),
useCentralDifferences(sd.useCentralDifferences),
feasibilityTolerance(sd.feasibilityTolerance),
currentPertState(sd.currentPertState),
hessianUpdateMethod(sd.hessianUpdateMethod),
maximumFunctionEvals(sd.maximumFunctionEvals),
optimalityTolerance(sd.optimalityTolerance),
functionTolerance(sd.functionTolerance),
maximumElasticWeight(sd.maximumElasticWeight),
optIterations(sd.optIterations),
gmatProblem(sd.gmatProblem),
runOptimizer(sd.runOptimizer)
{
   isInitialized = false;
   setNewConValues = sd.setNewConValues;
}


//------------------------------------------------------------------------------
// Yukonad& operator=(const Yukonad& sd)
//------------------------------------------------------------------------------
/**
* Assignment operator
*
* @param sd The instance copied to this one
*
* @return this instance, set to match sd
*/
//------------------------------------------------------------------------------
Yukonad& Yukonad::operator=(const Yukonad& sd)
{
   if (&sd != this)
   {
      InternalOptimizer::operator=(sd);

      objectiveSet = sd.objectiveSet;
      useCentralDifferences = sd.useCentralDifferences;
      feasibilityTolerance = sd.feasibilityTolerance;
      hessianUpdateMethod = sd.hessianUpdateMethod;
      maximumFunctionEvals = sd.maximumFunctionEvals;
      optimalityTolerance = sd.optimalityTolerance;
      functionTolerance = sd.functionTolerance;
      maximumElasticWeight = sd.maximumElasticWeight;
      optIterations = sd.optIterations;
      gmatProblem = sd.gmatProblem;
      runOptimizer = sd.runOptimizer;
      gradient = sd.gradient;  // Should this be "grad"?
      jacobian = sd.jacobian;

      isInitialized = false;
      setNewConValues = sd.setNewConValues;
   }

   return *this;
}

//------------------------------------------------------------------------------
// void Copy(const GMatBase *obj)
//------------------------------------------------------------------------------
/**
 * Base class override for object copying
 *
 * @param obj The instance copied to this one
 */
//------------------------------------------------------------------------------
void Yukonad::Copy(const GmatBase *obj)
{
   if (obj->IsOfType("Yukon"))
      (*this) = *((Yukonad*)obj);
   else
      throw SolverException("Cannot set the Yukon optimizer " + instanceName +
            " to match the object " + obj->GetName());
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
* Cloning method used to reproduce this instance
*
* @return A new object configured like this one
*/
//------------------------------------------------------------------------------
GmatBase* Yukonad::Clone() const
{
   return new Yukonad(*this);
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
* Retrieves a list of reference objects of a given type
*
* @param type The type of reference objects desired
*
* @return The list of object names
*/
//------------------------------------------------------------------------------
const StringArray& Yukonad::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   dummyArray.clear();
   return dummyArray;
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
* Determines if a parameter should be written to script when the object is
* serialized
*
* @param id The parameter's ID
*
* @return true if the parameter should be hidden, false if not
*/
//------------------------------------------------------------------------------
bool Yukonad::IsParameterReadOnly(const Integer id) const
{
   if (id == goalNameID)
      return true;
   if (id == OPTIMIZER_TOLERANCE)
      return true;

   return InternalOptimizer::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Determines if a parameter should be written to script when the object is
* serialized
*
* @param label The parameter's scripted name
*
* @return true if the parameter should be hidden, false if not
*/
//------------------------------------------------------------------------------
bool Yukonad::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieve the enumerated type of the object.
*
* @param <id> The integer ID for the parameter.
*
* @return The enumeration for the type of the parameter, or
*         UNKNOWN_PARAMETER_TYPE.
*/
//------------------------------------------------------------------------------
Gmat::ParameterType Yukonad::GetParameterType(const Integer id) const
{
   if (id >= InternalOptimizerParamCount && id < YukonadParamCount)
      return PARAMETER_TYPE[id - InternalOptimizerParamCount];

   return InternalOptimizer::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieve the string associated with a parameter.
*
* @param <id> The integer ID for the parameter.
*
* @return Text description for the type of the parameter, or the empty
*         string ("").
*/
//------------------------------------------------------------------------------
std::string Yukonad::GetParameterTypeString(const Integer id) const
{
   return InternalOptimizer::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieve the description for the parameter.
*
* @param <id> The integer ID for the parameter.
*
* @return String description for the requested parameter.
*
* @note The parameter strings should not include any white space
*/
//------------------------------------------------------------------------------
std::string Yukonad::GetParameterText(const Integer id) const
{
   if (id >= InternalOptimizerParamCount && id < YukonadParamCount)
      return PARAMETER_TEXT[id - InternalOptimizerParamCount];
   return InternalOptimizer::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
* Retrieve the ID for the parameter given its description.
*
* @param <str> Description for the parameter.
*
* @return the parameter ID, or -1 if there is no associated ID.
*/
//------------------------------------------------------------------------------
Integer Yukonad::GetParameterID(const std::string &str) const
{
   for (Integer i = InternalOptimizerParamCount; i < YukonadParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - InternalOptimizerParamCount])
         return i;
   }

   return InternalOptimizer::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieve a string parameter.
*
* @param <id> The integer ID for the parameter.
*
* @return The string stored for this parameter, or throw ab=n exception if
*         there is no string association.
*/
//------------------------------------------------------------------------------
std::string Yukonad::GetStringParameter(const Integer id) const
{
   if (id == goalNameID)
      return objectiveName;

   if (id == hessianUpdateMethodID)
      return hessianUpdateMethod;

   return InternalOptimizer::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
* Change the value of a string parameter.
*
* @param <id> The integer ID for the parameter.
* @param <value> The new string for this parameter.
*
* @return true if the string is stored, throw if the parameter is not stored.
*/
//------------------------------------------------------------------------------
bool Yukonad::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == goalNameID)
   {
      objectiveName = value;
      return true;
   }

   if (id == hessianUpdateMethodID)
   {
      for (Integer i = DampedBFGS; i < MaxUpdateMethod; ++i)
      {
         if (value == HESSIAN_UPDATE_METHOD[i - DampedBFGS])
         {
            hessianUpdateMethod = value;
            return true;
         }
      }
      throw SolverException(
         "The value of \"" + value + "\" for field \"Hessian Update Method\""
         " on object \"" + instanceName + "\" is not an allowed value.\n"
         "The allowed values are: [DampedBFGS, SelfScaledBFGS, "
         "MiNLPHessUpdateMethod].");
   }

   return InternalOptimizer::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Retrieve a string parameter.
*
* @param label The (string) label for the parameter.
*
* @return The string stored for this parameter, or the empty string if there
*         is no string association.
*/
//------------------------------------------------------------------------------
std::string Yukonad::GetStringParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
* Change the value of a string parameter.
*
* @param label The (string) label for the parameter.
* @param value The new string for this parameter.
*
* @return true if the string is stored, false if not.
*/
//------------------------------------------------------------------------------
bool Yukonad::SetStringParameter(const std::string &label,
   const std::string &value)
{
   Integer id = GetParameterID(label);
   return SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieve a boolean parameter.
*
* @param id The integer ID for the parameter.
*
* @return the boolean value for this parameter, or throw an exception if the
*         parameter access in invalid.
*/
//------------------------------------------------------------------------------
bool Yukonad::GetBooleanParameter(const Integer id) const
{
   if (id == useCentralDifferencesID)
      return useCentralDifferences;

   return InternalOptimizer::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
* Sets the value for a boolean parameter.
*
* @param id The integer ID for the parameter.
* @param value The new value.
*
* @return the boolean value for this parameter, or throw an exception if the
*         parameter is invalid or not boolean.
*/
//------------------------------------------------------------------------------
bool Yukonad::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == useCentralDifferencesID)
   {
      useCentralDifferences = value;
      if (useCentralDifferences)
      {
         gradientCalculator.SetDifferenceMode(
            DerivativeModel::CENTRAL_DIFFERENCE);
         jacobianCalculator.SetDifferenceMode(
            DerivativeModel::CENTRAL_DIFFERENCE);
      }
      else
      {
         gradientCalculator.SetDifferenceMode(
            DerivativeModel::FORWARD_DIFFERENCE);
         jacobianCalculator.SetDifferenceMode(
            DerivativeModel::FORWARD_DIFFERENCE);
      }
      return useCentralDifferences;
   }

   return InternalOptimizer::SetBooleanParameter(id, value);
}

//------------------------------------------------------------------------------
//  bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Retrieve a boolean parameter.
*
* @param <label> The (string) label for the parameter.
*
* @return the boolean value for this parameter, or false if the parameter is
*         not boolean.
*/
//------------------------------------------------------------------------------
bool Yukonad::GetBooleanParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
* Sets the value for a boolean parameter.
*
* @param <label> The (string) label for the parameter.
*
* @return the boolean value for this parameter, or false if the parameter is
*         not boolean.
*/
//------------------------------------------------------------------------------
bool Yukonad::SetBooleanParameter(const std::string &label, const bool value)
{
   Integer id = GetParameterID(label);
   return SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves Real parameter data
*
* @param id The ID for the parameter that is set
*
* @return The parameter value
*/
//------------------------------------------------------------------------------
Real Yukonad::GetRealParameter(const Integer id) const
{
   if (id == feasibilityToleranceID)
   {
      return feasibilityTolerance;
   }
   if (id == optimalityToleranceID)
   {
      return optimalityTolerance;
   }
   if (id == functionToleranceID)
   {
      return functionTolerance;
   }
   return InternalOptimizer::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
* Sets Real parameter data
*
* @param id The ID for the parameter that is set
* @param value The new parameter value
*
* @return The parameter value
*/
//------------------------------------------------------------------------------
Real Yukonad::SetRealParameter(const Integer id, const Real value)
{
   if (id == feasibilityToleranceID)
   {
      if (value > 0.0)
         feasibilityTolerance = value;
      else
      {
         char msg[512];
         std::stringstream val;
         val.precision(16);
         val << value;
         std::sprintf(msg, errorMessageFormat.c_str(), val.str().c_str(),
            PARAMETER_TEXT[id - InternalOptimizerParamCount].c_str(),
            "Real > 0.0");
         throw SolverException(msg);
      }
      return feasibilityTolerance;
   }

   if (id == optimalityToleranceID)
   {
      if (value > 0.0)
         optimalityTolerance = value;
      else
      {
         char msg[512];
         std::stringstream val;
         val.precision(16);
         val << value;
         std::sprintf(msg, errorMessageFormat.c_str(), val.str().c_str(),
            PARAMETER_TEXT[id - InternalOptimizerParamCount].c_str(),
            "Real > 0.0");
         throw SolverException(msg);
      }
      return optimalityTolerance;
   }

   if (id == functionToleranceID)
   {
      if (value > 0.0)
         functionTolerance = value;
      else
      {
         char msg[512];
         std::stringstream val;
         val.precision(16);
         val << value;
         std::sprintf(msg, errorMessageFormat.c_str(), val.str().c_str(),
            PARAMETER_TEXT[id - InternalOptimizerParamCount].c_str(),
            "Real > 0.0");
         throw SolverException(msg);
      }
      return functionTolerance;
   }

   if (id == OPTIMIZER_TOLERANCE)
   {
      throw SolverException("Tolerance is not an option for the Yukon "
         "optimizer.  Allowed tolerance settings are [FeasibilityTolerance, "
         "OptimialityTolerance, FunctionTolerance]");
   }

   return InternalOptimizer::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves Integer parameter data
*
* @param id The ID for the parameter that is set
*
* @return The parameter value
*/
//------------------------------------------------------------------------------
Integer Yukonad::GetIntegerParameter(const Integer id) const
{
   if (id == maximumFunctionEvalsID)
   {
      return maximumFunctionEvals;
   }
   if (id == maximumElasticWeightID)
   {
      return maximumElasticWeight;
   }

   return Solver::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
* Sets Integer parameter data
*
* @param id The ID for the parameter that is set
* @param value The new parameter value
*
* @return The parameter value
*/
//------------------------------------------------------------------------------
Integer Yukonad::SetIntegerParameter(const Integer id,
   const Integer value)
{
   if (id == maximumFunctionEvalsID)
   {
      if (value > 0.0)
         maximumFunctionEvals = value;
      else
      {
         char msg[512];
         std::stringstream val;
         val.precision(16);
         val << value;
         std::sprintf(msg, errorMessageFormat.c_str(), val.str().c_str(),
            PARAMETER_TEXT[id - InternalOptimizerParamCount].c_str(),
            "Real numbers > 0.0");
         throw SolverException(msg);
      }
      return maximumFunctionEvals;
   }

   if (id == maximumElasticWeightID)
   {
      if (value > 0.0)
         maximumElasticWeight = value;
      else
      {
         char msg[512];
         std::stringstream val;
         val.precision(16);
         val << value;
         std::sprintf(msg, errorMessageFormat.c_str(), val.str().c_str(),
            PARAMETER_TEXT[id - InternalOptimizerParamCount].c_str(),
            "Real numbers > 0.0");
         throw SolverException(msg);
      }
      return maximumElasticWeight;
   }

   return InternalOptimizer::SetIntegerParameter(id, value);
}


//Real Yukonad::GetRealParameter(const Integer id, const Integer index) const
//{
//   return InternalOptimizer::GetRealParameter(id, index);
//}
//
//Real Yukonad::GetRealParameter(const Integer id, const Integer row, const Integer col) const
//{
//   return InternalOptimizer::GetRealParameter(id, row, col);
//}
//
//Real Yukonad::SetRealParameter(const Integer id, const Real value, const Integer index)
//{
//   return InternalOptimizer::SetRealParameter(id, value, index);
//}
//
//Real Yukonad::SetRealParameter(const Integer id, const Real value, const Integer row, const Integer col)
//{
//   return InternalOptimizer::SetRealParameter(id, value, row, col);
//}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Retrieves Real parameter data
*
* @param label The script label for the parameter that is set
*
* @return The parameter value
*/
//------------------------------------------------------------------------------
Real Yukonad::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
* Sets Real parameter data
*
* @param label The script label for the parameter that is set
* @param value The new parameter value
*
* @return The parameter value
*/
//------------------------------------------------------------------------------
Real Yukonad::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//Real Yukonad::GetRealParameter(const std::string &label, const Integer index) const
//{
//   return GetRealParameter(GetParameterID(label), index);
//}
//
//Real Yukonad::SetRealParameter(const std::string &label, const Real value, const Integer index)
//{
//   return SetRealParameter(GetParameterID(label), value, index);
//}
//
//Real Yukonad::GetRealParameter(const std::string &label, const Integer row, const Integer col) const
//{
//   return GetRealParameter(GetParameterID(label), row, col);
//}
//
//Real Yukonad::SetRealParameter(const std::string &label, const Real value, const Integer row, const Integer col)
//{
//   return SetRealParameter(GetParameterID(label), value, row, col);
//}

//------------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//------------------------------------------------------------------------------
/**
* Returns the list of allowable settings for the enumerated parameters
*
* @param id The ID of the parameter
*
* @return A const string array with the allowed settings.
*/
//------------------------------------------------------------------------------
const StringArray& Yukonad::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   enumStrings.clear();

   if (id == hessianUpdateMethodID)
   {
      enumStrings.push_back("DampedBFGS");
      enumStrings.push_back("SelfScaledBFGS");
      return enumStrings;
   }
   return Solver::GetPropertyEnumStrings(id);
}

//------------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
* This method performs an action on the instance.
*
* TakeAction is a method overridden from GmatBase.  The only action defined for
* a DifferentialCorrector is "IncrementInstanceCount", which the Sandbox uses
* to tell an instance if if it is a reused instance (i.e. a clone) of the
* configured instance of the DifferentialCorrector.
*
* @param <action>      Text label for the action.
* @param <actionData>  Related action data, if needed.
*
* @return  The value of the parameter at the completion of the call.
*/
//------------------------------------------------------------------------------
bool Yukonad::TakeAction(const std::string &action,
   const std::string &actionData)
{
   if (action == "Reset")
   {
      currentState = INITIALIZING;
      FreeArrays();
      return true;
   }

   return InternalOptimizer::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// Integer SetSolverResults(Real *data, const std::string &name)
//------------------------------------------------------------------------------
/**
* Implements the Yukonad state machine.
*
* @return true if the state maching step worked, false if an error was
* detected.
*/
//------------------------------------------------------------------------------
Solver::SolverState  Yukonad::AdvanceState()
{
   switch (currentState)
   {
   case INITIALIZING:
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage("Entered state machine; "
         "INITIALIZING\n");
#endif
      optIterations = 0;
      iterationsTaken = 0;
      WriteToTextFile();
      ReportProgress();
      if (gmatProblem == NULL)
         Initialize();
      CompleteInitialization();

#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage(
         "Yukonad State Transitions from %d to %d\n",
         INITIALIZING, currentState);
#endif
      break;

   case NOMINAL:
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage("Entered state machine; "
         "NOMINAL\n");
#endif
      WriteToTextFile();
      ReportProgress();
      RunNominal();
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage(
         "Yukonad State Transitions from %d to %d\n", NOMINAL,
         currentState);
#endif
      // ReportProgress();
      break;

   case PERTURBING:
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage("Entered state machine; "
         "PERTURBING\n");
#endif
      RunPerturbation();
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage(
         "Yukonad State Transitions from %d to %d\n", PERTURBING,
         currentState);
#endif
      break;

   case Solver::CALCULATING:
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage("Entered state machine; "
         "CALCULATING\n");
#endif
      CalculateParameters();
      WriteToTextFile();
      ReportProgress();
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage(
         "Yukonad State Transitions from %d to %d\n", CALCULATING,
         currentState);
#endif
      break;

   case CHECKINGRUN:
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage("Entered state machine; "
         "CHECKINGRUN\n");
#endif
      CheckCompletion();
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage(
         "Yukonad State Transitions from %d to %d\n", CHECKINGRUN,
         currentState);
#endif
      break;

   case FINISHED:
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage("Entered state machine; "
         "FINISHED\n");
#endif
      WriteToTextFile();
      ReportProgress();
      RunComplete();
#ifdef YUKON_DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage(
         "Yukonad State Transitions from %d to %d\n", FINISHED,
         currentState);
#endif
      break;

   default:
      throw SolverException(
         "Yukonad Solver \"" + instanceName +
         "\" encountered an unexpected state.");
   }

   return currentState;
}


//------------------------------------------------------------------------------
// bool Optimize()
//------------------------------------------------------------------------------
bool Yukonad::Optimize()
{
   return true;
}

//------------------------------------------------------------------------------
// Integer SetSolverResults(Real *data, const std::string &name)
//------------------------------------------------------------------------------
/**
* Sets up the data fields used for the results of an iteration.
*
* @param <data> An array of data appropriate to the results used in the
*               algorithm (for instance, tolerances for targeter goals).
* @param <name> A label for the data parameter.  Defaults to the empty
*               string.
*
* @return The ID used for this variable.
*/
//------------------------------------------------------------------------------
Integer Yukonad::SetSolverResults(Real *data,
   const std::string &name,
   const std::string &type)
{
#ifdef DEBUG_Yukonad
   MessageInterface::ShowMessage("*** Setting Results for '%s' of type '%s'\n",
      name.c_str(), type.c_str());
#endif

   if (type == "Objective")
   {
      objectiveName = name;
      objectiveSet = true;
   }

   return InternalOptimizer::SetSolverResults(data, name, type);
}


//------------------------------------------------------------------------------
// void SetResultValue(Integer id, Real value)
//------------------------------------------------------------------------------
/**
* Passes in the results obtained from a run in the DifferentialCorrector loop.
*
* @param <id>    The ID used for this result.
* @param <value> The corresponding result.
*/
//------------------------------------------------------------------------------
void Yukonad::SetResultValue(Integer id, Real value,
   const std::string &resultType)
{
#ifdef DEBUG_Yukonad
   MessageInterface::ShowMessage("Setting SD result for id = %d, type = %s, "
      "value = %.12f\n", id, resultType.c_str(), value);
#endif

   bool plusEffect = true;
   if (useCentralDifferences && (currentPertState == -1))
      plusEffect = false;

   // Gradients use the objective function
   if (resultType == "Objective")
   {
      if (currentState == NOMINAL)
      {
         // id (2nd parameter here) for gradients is always 0
         cost = value;
         gradientCalculator.Achieved(-1, 0, 0.0, value, plusEffect);
      }

      if (currentState == PERTURBING)
      {
         gradientCalculator.Achieved(pertNumber, 0, (pertNumber < 0 ? 0.0 :
            perturbation[pertNumber]), value, plusEffect);
      }
   }
   else
   {
      // build the correct ID number
      Integer idToUse;
      if (resultType == "EqConstraint")
      {
         idToUse = id - 1000;
         if (currentState == NOMINAL)
         {
            eqConstraintValues[idToUse] = value;
            setNewConValues = true;
         }
         else if (currentState == PERTURBING && setNewConValues)
         {
            gmatProblem->SetConFunction(idToUse,
               eqConstraintAchievedValues[idToUse], "EqCon");
            if (idToUse == eqConstraintCount + ineqConstraintCount - 1)
               setNewConValues = false;
         }
      }
      else
      {
         idToUse = id - 2000;
         if (currentState == NOMINAL)
         {
            ineqConstraintValues[idToUse] = value;
            setNewConValues = true;
         }
         else if ((currentState == PERTURBING) && (setNewConValues))
         {
            gmatProblem->SetConFunction(idToUse,
               ineqConstraintAchievedValues[idToUse], "IneqCon");
            if (idToUse == eqConstraintCount + ineqConstraintCount - 1)
               setNewConValues = false;
         }
         idToUse += eqConstraintCount;
      }

      if (currentState == NOMINAL)
      {
         jacobianCalculator.Achieved(-1, idToUse, 0.0, value, plusEffect);
      }

      if (currentState == PERTURBING)
      {
         jacobianCalculator.Achieved(pertNumber, idToUse, (pertNumber < 0 ? 0.0 :
            perturbation[pertNumber]), value, plusEffect);
      }

   }


   /// Add code for the constraint feeds here
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
* Initializes the optimizer prior to optimization.
*/
//------------------------------------------------------------------------------
bool Yukonad::Initialize()
{
#ifdef DEBUG_YUKON_INIT
   MessageInterface::ShowMessage("Yukonad::Initialize() entered for %s; "
      "%d variables and %d constraints\n", instanceName.c_str(),
      registeredVariableCount, registeredComponentCount);
#endif

   // Variable initialization is in the Solver code
   bool retval = InternalOptimizer::Initialize();

   if (retval)
   {
      if (useCentralDifferences)
      {
         gradientCalculator.SetDifferenceMode(
            DerivativeModel::CENTRAL_DIFFERENCE);
         jacobianCalculator.SetDifferenceMode(
            DerivativeModel::CENTRAL_DIFFERENCE);
      }
      else
      {
         gradientCalculator.SetDifferenceMode(
            DerivativeModel::FORWARD_DIFFERENCE);
         jacobianCalculator.SetDifferenceMode(
            DerivativeModel::FORWARD_DIFFERENCE);
      }

      retval = gradientCalculator.Initialize(registeredVariableCount);
   }

   if (retval)
   {
      if (registeredComponentCount > 0)
         retval = jacobianCalculator.Initialize(registeredVariableCount,
         registeredComponentCount);
   }

   for (int i = 0; i < registeredVariableCount; ++i)
   {
      gradient.push_back(0.0);
      for (int j = 0; j < registeredComponentCount; ++j)
         jacobian.push_back(0.0);
   }

   retCode = -101;

#ifdef DEBUG_YUKON_INIT
   MessageInterface::ShowMessage
      ("Yukonad::Initialize() completed; %d variables and %d constraints\n",
      registeredVariableCount, registeredComponentCount);
#endif
   
   // Initialize the Gmat to MiNLP interface and send a pointer so the
   // interface can access optimizer data
   gmatProblem = new GmatProblemInterface();
   gmatProblem->SetPointerToOptimizer(this);

   isInitialized = true;

   return retval;
}

//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// State machine methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void RunNominal()
//------------------------------------------------------------------------------
/**
* Runs out an unperturbed optimization control sequence
*/
//------------------------------------------------------------------------------
void Yukonad::RunNominal()
{
   pertNumber = -1;
   currentPertState = 0;
   currentState = PERTURBING;
   status = Gmat::RUN;
   GmatGlobal::Instance()->SetSolverStatus(instanceName, status);
}


//------------------------------------------------------------------------------
// void RunPerturbation()
//------------------------------------------------------------------------------
/**
* Runs out a perturbed optimization control sequence
*/
//------------------------------------------------------------------------------
void Yukonad::RunPerturbation()
{
#ifdef DEBUG_YUKON_PERT
   MessageInterface::ShowMessage
      ("Yukonad::RunPerturbation() entered, pertNumber=%d, variableCount=%d\n",
      pertNumber, variableCount);
#endif

   if (!useCentralDifferences)
   {
      // Calculate the perts, one at a time
      if (pertNumber != -1)
         // Back out the last pert applied
         variable.at(pertNumber) = lastUnperturbedValue;
      ++pertNumber;

      if (pertNumber == variableCount)  // Current set of perts have been run
      {
         currentState = CALCULATING;
         pertNumber = -1;
         return;
      }

      lastUnperturbedValue = variable.at(pertNumber);
      variable.at(pertNumber) += perturbation.at(pertNumber);
      pertDirection.at(pertNumber) = 1.0;
   }
   else
   {
      // Calculate the perts, one at a time
      if (pertNumber != -1)
         // Back out the last pert applied
         variable.at(pertNumber) = lastUnperturbedValue;

      if (currentPertState == -1)
      {
         currentPertState = 0;
      }

      if (currentPertState == 1)
      {
         currentPertState = -1;
      }

      if (currentPertState == 0)
      {
         ++pertNumber;
         currentPertState = 1;
      }

      if (pertNumber == variableCount)  // Current set of perts have been run
      {
         currentState = CALCULATING;
         pertNumber = -1;
         currentPertState = 0;

#ifdef DEBUG_YUKON_PERT
         MessageInterface::ShowMessage
            ("Yukonad::RunPerturbation() leaving, pertNumber set to -1\n");
#endif
         return;
      }

      lastUnperturbedValue = variable.at(pertNumber);
      if (currentPertState == 1)
      {
         variable.at(pertNumber) += perturbation.at(pertNumber);
         pertDirection.at(pertNumber) = 1.0;
      }

      if (currentPertState == -1)
      {
         variable.at(pertNumber) -= perturbation.at(pertNumber);
         pertDirection.at(pertNumber) = 1.0;
      }
   }

   WriteToTextFile();

#ifdef DEBUG_YUKON_PERT
   MessageInterface::ShowMessage
      ("Yukonad::RunPerturbation() leaving, pertNumber=%d\n", pertNumber);
#endif
}


//------------------------------------------------------------------------------
// void CalculateParameters()
//------------------------------------------------------------------------------
/**
* Calculates the data needed to iterate the optimization process
*/
//------------------------------------------------------------------------------
void Yukonad::CalculateParameters()
{
   if (objectiveDefined)
      gradientCalculator.Calculate(gradient);
   if (eqConstraintCount + ineqConstraintCount > 0)
      jacobianCalculator.Calculate(jacobian);

   // runOptimizer->PrepareToOptimize();

   currentState = CHECKINGRUN;

#ifdef YUKON_DEBUG_STATE_MACHINE
   MessageInterface::ShowMessage(
      "Raw Gradient and Jacobian Information\n   Gradient: [");
   for (std::vector<Real>::iterator i = gradient.begin();
      i != gradient.end(); ++i)
      MessageInterface::ShowMessage(" %.12lf ", *i);
   MessageInterface::ShowMessage("]\n   Jacobian: [");
   for (std::vector<Real>::iterator i = jacobian.begin();
      i != jacobian.end(); ++i)
      MessageInterface::ShowMessage(" %.12lf ", *i);
   MessageInterface::ShowMessage("]\n\n");
#endif
}


//------------------------------------------------------------------------------
// void CheckCompletion()
//------------------------------------------------------------------------------
/**
* Checks the status of the optimization
*/
//------------------------------------------------------------------------------
void Yukonad::CheckCompletion()
{
   // If this is the first iteration of the optmizer, perform initializtion
   // methods
   if (iterationsTaken == 0)
   {
      if (runOptimizer)
         delete runOptimizer;
      runOptimizer = new Yukon(gmatProblem, hessianUpdateMethod,
         maxIterations, maximumFunctionEvals, feasibilityTolerance,
         optimalityTolerance, functionTolerance, maximumElasticWeight);
      runOptimizer->PrepareToOptimize();
      runOptimizer->PrepareLineSearch();
   }

   // Run iterations of the optimizer
   retCode = -101;
   Integer funTypes;
   Rvector decVector;
   bool isNewX;
   YukonUserProblem *userFunPointer = gmatProblem;
   runOptimizer->RespondToData();
   runOptimizer->CheckStatus(retCode, funTypes, optIterations, decVector,
      isNewX, userFunPointer);

   if (retCode == -1)
   {
      currentState = CHECKINGRUN;
      return;
   }

#ifdef DEBUG_YUKON_CALL
   MessageInterface::ShowMessage("After calling Yukonad, retcode = %d\n",
      retCode);
   MessageInterface::ShowMessage("   decVector = [");
   for (Integer i = 0; i < variableCount; ++i)
   {
      MessageInterface::ShowMessage("%.12lf", decVector[i]);
      if (i < variableCount - 1)
         MessageInterface::ShowMessage(", ");
   }
   MessageInterface::ShowMessage("]\n");
#endif   
   // retCode
   //       = -1 WHEN UPDATED DATA IS REQUIRED FOR STEP
   //       = 0 DURING CALCULATION
   //       = 1 WHEN REQUIRED ACCURACY IS ACHIEVED
   //       = 2 WHEN MAX NUMBER OF ITERATIONS IS REACHED
   //       = 3 WHEN MAX NUMBER OF FUNC EVALUATIONS IS REACHED
   //       = 4 WHEN STEP SIZE BECOMES TOO SMALL AND OPTIMIZER FAILS
   //       = 5 WHEN A GOOD STEP DIRECTION COULD NOT BE FOUND
   if (retCode == 0)
   {
      for (Integer i = 0; i < variableCount; ++i)
         variable.at(i) = decVector[i];

      currentState = NOMINAL;
   }
   else
   {
      currentState = FINISHED;
      if (retCode == 1)
      {
         status = Gmat::CONVERGED;
         converged = true;
      }
      else if (retCode == 2 || retCode == 3)
      {
         status = Gmat::EXCEEDED_ITERATIONS;
         converged = false;
      }
      else
      {
         status = Gmat::FAILED;
         converged = false;
      }

      GmatGlobal::Instance()->SetSolverStatus(instanceName, status);
   }

   ++iterationsTaken;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
* Finalizes the run
*
* @note No actions are required in the state machine for this process
*/
//------------------------------------------------------------------------------
void Yukonad::RunComplete()
{
   hasFired = true;
}


//------------------------------------------------------------------------------
// void FreeArrays()
//------------------------------------------------------------------------------
/**
* Cleans up memory from the allocated arrays of data
*/
//------------------------------------------------------------------------------
void Yukonad::FreeArrays()
{
}


//------------------------------------------------------------------------------
//  void WriteToTextFile(SolverState stateToUse)
//------------------------------------------------------------------------------
/**
* Utility function used by the solvers to generate a progress file.
*
* @param stateToUse SolverState used for the report; if this parameter is
*                   different from the default value (UNDEFINED_STATE),
*                   it is used.  If the value is UNDEFINED_STATE, then the
*                   value of currentState is used.
*/
//------------------------------------------------------------------------------
void Yukonad::WriteToTextFile(SolverState stateToUse)
{
#ifdef DEBUG_SOLVER_WRITE
   MessageInterface::ShowMessage
      ("Yukonad::WriteToTextFile() entered, stateToUse=%d, solverTextFile='%s', "
      "textFileOpen=%d, initialized=%d\n", stateToUse, solverTextFile.c_str(),
      textFile.is_open(), initialized);
#endif

   if (!showProgress)
      return;

   if (!textFile.is_open())
      OpenSolverTextFile();

   if (isInitialized)
   {
      SolverState buffer = UNDEFINED_STATE;
      if (stateToUse != UNDEFINED_STATE)
      {
         buffer = currentState;
         currentState = stateToUse;
      }

      std::string data = GetProgressString();
      textFile << data << std::endl;

      if (buffer != UNDEFINED_STATE)
         currentState = buffer;
   }
}


//------------------------------------------------------------------------------
// std::string InterpretRetCode(Integer retCode)
//------------------------------------------------------------------------------
/**
* Produces strings describing what happened based on Yukonad return codes
*
* @param retCode The code that needs to be described
*
* @return A string containing the meaning of the return code
*/
//------------------------------------------------------------------------------
std::string Yukonad::InterpretRetCode(Integer retCode)
{
   std::string retString = "";

   switch (retCode)
   {
   case -1:
   case -101:
   case -110:
   case -111:
      retString += ": Optimization ready to start.\n";
      break;

   case 0:
      retString += ": Optimization is proceeding as expected.\n";
      break;

   case 1:
      retString += " converged to within target accuracy.\n";
      break;

   case 2:
      retString += " failed to converge: "
         "Maximum number of iterations exceeded.\n";
      break;

   case 3:
      retString += " failed to converge: "
         "Maximum number of function evaluations exceeded.\n";
      break;

   case 4:
      retString += " failed to converge: "
         "Step size became too small.\n";
      break;

   case 5:
      retString += " failed to converge: "
         "The last two step directions failed to converge.\n";
      break;

   case 6:
      retString += " failed to converge: "
         "The problem appears to be infeasible.\n";
      break;

   default:
      retString += " terminated with an unknown error code.\n";
      break;
   }

   return retString;
}


//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
* Generates a string that reports the current Yukonad state.
*
* @return The string
*/
//------------------------------------------------------------------------------
std::string Yukonad::GetProgressString()
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
      case NOMINAL:
         progress << instanceName << " Iteration " << optIterations 
            << "; Function Evaluation " << iterationsTaken + 1
            << "; Nominal Pass\n   Variables:  ";
         // Iterate through the variables, writing them to the string
         for (current = variableNames.begin(), i = 0;
            current != variableNames.end(); ++current)
         {
            if (current != variableNames.begin())
               progress << ", ";
            progress << *current << " = " << unscaledVariable.at(i);
            if (textFileMode == "Verbose")
               progress << "; optimizer scaled value: " << variable[i];
            ++i;
         }
         if (objectiveSet)
            progress << "\n   Cost Function Value: " << cost;
         progress << "\n   " << instanceName << " State"
            << InterpretRetCode(retCode);
         break;

      case PERTURBING:
         if (textFileMode == "Verbose")
            progress << "   Completed function evaluation "
            << iterationsTaken + 1 << ", pert " << pertNumber + 1 << " for "
            << variableNames[pertNumber]
            << "; optimizer scaled value: " << variable[pertNumber];
         break;

      case CALCULATING:

         break;

      case CHECKINGRUN:
         if (textFileMode == "Verbose")
         {
            progress << "\n   Cost Gradient:\n      ";

            for (Integer i = 0; i < variableCount; ++i)
               progress << gradient.at(i) << "    ";

            progress << "\n\n   Constraint Jacobian:\n      ";
            for (Integer j = 0; j < eqConstraintCount + ineqConstraintCount; ++j)
            {
               for (Integer i = 0; i < variableCount; ++i)
               {
                  if (j < eqConstraintCount)
                     progress << jacobian.at(i + variableCount * j) << "    ";
                  else
                     progress << -jacobian.at(i + variableCount * j) << "    ";
               }
               progress << "\n      ";
            }
            progress << "\n";
         }

         if (eqConstraintCount > 0)
         {
            // Iterate through the constraints, writing them to the file
            progress << "   Equality Constraint Variances:\n";

            for (current = eqConstraintNames.begin(), i = 0;
               current != eqConstraintNames.end(); ++current)
            {
               progress << "      Delta " << (*current) << " = "
                  << eqConstraintValues[i] << "\n";
               ++i;
            }
         }

         if (ineqConstraintCount > 0)
         {
            progress << "   Inequality Constraint Variances:\n";

            for (current = ineqConstraintNames.begin(), i = 0;
               current != ineqConstraintNames.end(); ++current)
            {
               progress << "      Delta " << (*current) << " = "
                  << ineqConstraintValues[i] << "\n";
               ++i;
            }
         }

         break;

      case FINISHED:
         if (status == Gmat::CONVERGED)
            progress << "\n*** Optimization Completed in " << optIterations 
            << " iterations and " << iterationsTaken
            << " function evaluations\n*** The Optimizer Converged!";
         else if (retCode == 2)
            progress << "\n*** Optimization did not converge in "
            << maxIterations
            << " iterations";
         else if (retCode == 3)
            progress << "\n*** Optimization did not converge in "
            << iterationsTaken
            << " function evaluations";
         else if (retCode == 6)
            progress << "\n*** Optimization terminated in "
            << optIterations << " iterations and " << iterationsTaken
            << " function evaluations";
         else
            progress << "\n*** An error occurred during optimization";

         progress << "\nFinal Variable values:\n";
         // Iterate through the variables, writing them to the string
         for (current = variableNames.begin(), i = 0;
            current != variableNames.end(); ++current)
            progress << "   " << *current << " = " << unscaledVariable[i++]
            << "\n";
         progress << instanceName << InterpretRetCode(retCode) << "\n";

         break;

      default:
         progress << Optimizer::GetProgressString();
         break;
      }
   }
   else
      return Solver::GetProgressString();

   return progress.str();

}


//------------------------------------------------------------------------------
// const RealArray* GetSolverData(const std::string type)
//------------------------------------------------------------------------------
/**
*  Method used to pull data for reporting from other objects
*
*  This method retrieves the current values for Solver fields, so that they can
*  be exposed elsewhere.  The Solver Window uses it to retrieve values for the
*  Yukon optimizer.
*
*  @param type The type of data requested
*
*  @return The numbers that match the type
*/
//------------------------------------------------------------------------------
const RealArray* Yukonad::GetSolverData(const std::string &type)
{
   RealArray *retval = NULL;

   if (type == "IneqConstraints")
   {
      ineqConstraintAchievedValues.clear();
      for (UnsignedInt i = 0; i < ineqConstraintValues.size(); ++i)
      {
         if (ineqConstraintOp[i] == 1)
         {
            ineqConstraintAchievedValues.push_back(ineqConstraintDesiredValues.at(i) -
               ineqConstraintValues[i]);
         }
         else
         {
            ineqConstraintAchievedValues.push_back(ineqConstraintValues.at(i) +
               ineqConstraintDesiredValues[i]);
         }
      }
      return &ineqConstraintAchievedValues;
   }
   if (type == "EqConstraints")
   {
      eqConstraintAchievedValues.clear();
      for (UnsignedInt i = 0; i < eqConstraintValues.size(); ++i)
      {
         eqConstraintAchievedValues.push_back(eqConstraintValues.at(i) +
            eqConstraintDesiredValues[i]);
      }
      return &eqConstraintAchievedValues;
   }
   if (type == "IneqConstraintsDeltas")
      return &ineqConstraintValues;
   if (type == "EqConstraints")
      return &eqConstraintValues;

   return retval;
}
