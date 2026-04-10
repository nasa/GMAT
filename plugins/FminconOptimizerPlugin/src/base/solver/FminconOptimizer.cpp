//$Id: FminconOptimizer.cpp 9461 2011-04-21 22:10:15Z lindajun $
//------------------------------------------------------------------------------
//                         FminconOptimizer
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan/GSFC
// Created: 2006.07.14
//
/**
 * Implementation for the FminconOptimizer class.
 */
//------------------------------------------------------------------------------


#include <sstream>                 // for stringstream, istringstream
#include <stdlib.h>                // for atoi atof, etc.
#include "FminconOptimizer.hpp"
#include "SolverException.hpp"
#include "StringUtil.hpp"
#include "FileManager.hpp"         // for GetAllMatlabFunctionPaths()
#include "GmatInterface.hpp"
#include "MessageInterface.hpp"
#include "MatlabInterface.hpp"     // for Matlab Engine functions
#include "GmatGlobal.hpp"          // for IsMatlabDebugOn()
#include "FileUtil.hpp"            // for GetPathName()
#include "InterfaceException.hpp"

//#define DEBUG_STATE_MACHINE
//#define DEBUG_ML_CONNECTIONS
//#define DEBUG_FMINCON_OPTIONS
//#define DEBUG_OPTIMIZER_DATA

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
FminconOptimizer::PARAMETER_TEXT[
       FminconOptimizerParamCount -ExternalOptimizerParamCount] =
{
   "Options",
   "OptionValues",
};

const Gmat::ParameterType
FminconOptimizer::PARAMETER_TYPE[
       FminconOptimizerParamCount - ExternalOptimizerParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
};

// list of allowed fmincon options
// NOTE - if changes are made to this list, changes MUST also be
// made to the Optimize command (where it is adding single quotes to
// string options) and to the IsAllowedValue method; the
// NUM_MATLAB_OPTIONS parameter may also need to be changed
const std::string FminconOptimizer::ALLOWED_OPTIONS[6] =
{
   "DiffMaxChange",
   "DiffMinChange",
   "MaxFunEvals",
   "TolX",
   "TolFun",
   "TolCon",
};

const std::string FminconOptimizer::DEFAULT_OPTION_VALUES[6] =
{
   "0.1000",
   "1.0000e-08",
   "1000",
   "1.0000e-04",
   "1.0000e-04",
   "1.0000e-04",
};

const Integer FminconOptimizer::NUM_MATLAB_OPTIONS    = 6;
const Integer FminconOptimizer::MATLAB_OPTIONS_OFFSET = 1000;

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FminconOptimizer(std::string name)
//------------------------------------------------------------------------------
FminconOptimizer::FminconOptimizer(const std::string &name) :
   ExternalOptimizer       ("FminconOptimizer", name),
   fminconExitFlag         (-999)
 {
   objectTypeNames.push_back("FminconOptimizer");
   parameterCount = FminconOptimizerParamCount;
   // set up options list, based on allowed options
   #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "In fmincon constructor, about to set up option and option value lists.\n");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   for (Integer i=0; i<NUM_MATLAB_OPTIONS; i++)
   {
      #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "Fmincon construct: now adding option %s to list\n",
         (ALLOWED_OPTIONS[i]).c_str());
          MessageInterface::ShowMessage(
         "Fmincon construct: now adding option value %s to list\n",
         (DEFAULT_OPTION_VALUES[i]).c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      options.push_back(ALLOWED_OPTIONS[i]);
      optionValues.push_back(DEFAULT_OPTION_VALUES[i]);
   }

   // ********* BEGIN temporary prototype, testing, etc. *****************************//
   //functionPath = "/Users/wcshoan/dev/Ec_GMAT/bin/files/matlab_functions";
   //OpenConnection();
   // ********* END temporary prototype, testing, etc. *****************************//

   AllowStepsizeLimit = false;
   AllowIndependentPerts = false;

   matlabIf = NULL;
}


//------------------------------------------------------------------------------
// ~FminconOptimizer()
//------------------------------------------------------------------------------
FminconOptimizer::~FminconOptimizer()
{
   FreeArrays();

   // loj: Let's leave it open (4/11/07)
   // So that it will be automatically connected to the current running Matlab session.
   // User has to manually close the Matlab session if we want to create a new session.
   // This works on Windows, will this cause any problems on other platforms?
   //CloseConnection();
}


//------------------------------------------------------------------------------
// FminconOptimizer(const FminconOptimizer &opt)
//------------------------------------------------------------------------------
FminconOptimizer::FminconOptimizer(const FminconOptimizer &opt) :
   ExternalOptimizer       (opt),
   fminconExitFlag         (-999)
{
   #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "In fmincon copy constructor, about to copy option and option value lists.\n");
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   options          = opt.options;
   optionValues     = opt.optionValues;
   #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "In fmincon copy constructor, number of options copied = %d\n",
      (Integer) options.size());
      MessageInterface::ShowMessage(
      "In fmincon copy constructor, number of option values copied = %d\n",
      (Integer) optionValues.size());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   parameterCount   = opt.parameterCount;
}


//------------------------------------------------------------------------------
// FminconOptimizer& operator=(const FminconOptimizer& opt)
//------------------------------------------------------------------------------
FminconOptimizer&
FminconOptimizer::operator=(const FminconOptimizer& opt)
{
   if (&opt == this)
      return *this;

   ExternalOptimizer::operator=(opt);
   options.clear();
   optionValues.clear();
   options          = opt.options;
   optionValues     = opt.optionValues;
   fminconExitFlag  = opt.fminconExitFlag; // right?

   return *this;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
bool FminconOptimizer::Initialize()
{
   ExternalOptimizer::Initialize();

   // open connection(s) to the external source
   //if (!OpenConnection())
   //   throw SolverException(
   //         "Fmincon - Unable to connect to external interface");

   // Open Matlab engine and find fmincon related files
   if (OpenConnection())
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
Solver::SolverState FminconOptimizer::AdvanceState()
{
   switch (currentState)
   {
      case INITIALIZING:
         #ifdef DEBUG_STATE_MACHINE
            MessageInterface::ShowMessage(
            "Entered FminconOptimizer state machine; INITIALIZING\n");
         #endif
         WriteToTextFile();
         ReportProgress();
         #ifdef DEBUG_STATE_MACHINE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
            "FminconOptimizer::AdvanceState about to complete initialization\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         CompleteInitialization();
         #ifdef DEBUG_STATE_MACHINE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
            "FminconOptimizer::AdvanceState initialization complete\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         currentState = RUNEXTERNAL;
         break;

      case RUNEXTERNAL:
         #ifdef DEBUG_STATE_MACHINE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
               "Entered FminconOptimizer state machine; RUNEXTERNAL\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         ReportProgress();
         RunExternal();
         ReportProgress();
         break;

      case FINISHED:
         #ifdef DEBUG_STATE_MACHINE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
               "Entered FminconOptimizer state machine; FINISHED\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         RunComplete();
         ReportProgress();
         break;
      default:
         break;
   }

   return currentState;
}


//------------------------------------------------------------------------------
// StringArray AdvanceNestedState(std::vector<Real> vars)
//------------------------------------------------------------------------------
StringArray FminconOptimizer::AdvanceNestedState(std::vector<Real> vars)
{
   #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "Entering FminconOptimizer::AdvanceNestedState\n");
      for (Integer ii=0; ii<(Integer) vars.size();ii++)
         MessageInterface::ShowMessage(" vars(%d) = %.12f\n",
         ii, vars.at(ii));
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   StringArray results;
   if (nestedState == INITIALIZING)
   {
      nestedState = NOMINAL;
   }
   else if (nestedState == NOMINAL)
   {
      if ((Integer) vars.size() != variableCount)
      {
         std::string errorStr = "FminconOptimizer: incorrect number of ";
         errorStr += "variable values returned from MATLAB";
         throw SolverException(errorStr);
      }
      for (Integer i=0;i<variableCount;i++)
         variable.at(i) = vars.at(i);
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage("   Starting fmincon pass...");
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      RunNominal();
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage("pass complete\n");
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      nestedState = CALCULATING;
      status = Gmat::RUN;  // Enables "Apply corrections"
      GmatGlobal::Instance()->SetSolverStatus(instanceName, status);
   }
   else if (nestedState == CALCULATING)
   {
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "about to call FminconOptimizer::CalculateParameters\n");
         MessageInterface::ShowMessage(
         "now building return string array\n");
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      CalculateParameters();
      std::stringstream outS;
      outS.precision(18);
      std::string       oneResult;

      // F
      //outS.str();
      outS.str("");
      outS << cost;
      oneResult = "F = " + outS.str() + ";";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);

      // GradF
      outS.str("");
      for (Integer i=0; i<(Integer) gradient.size(); i++)
         outS << gradient.at(i) << ";";
      oneResult = "GradF = [" + outS.str() + "];";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);

      // equality constraints
      outS.str("");
      for (Integer i=0; i<(Integer) eqConstraintValues.size(); i++)
         outS << eqConstraintValues.at(i) << ";";
      oneResult = "NonLinearEqCon = [" + outS.str() + "];";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);

      // inequality constraints
      outS.str("");
      for (Integer i=0; i<(Integer) ineqConstraintValues.size(); i++)
         outS << ineqConstraintValues.at(i) << ";";
      oneResult = "NonLinearIneqCon = [" + outS.str() + "];";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);

      // Jacobian in the future ---------------------- <future> ----------------
      oneResult = "JacNonLinearEqCon = [];";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);
      oneResult = "JacNonLinearIneqCon = [];";
      #ifdef DEBUG_OPTIMIZER_DATA // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "   adding %s to string array\n", oneResult.c_str());
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
      results.push_back(oneResult);

      // Jacobian in the future ---------------------- <future> ----------------

      WriteToTextFile(nestedState);
      nestedState = NOMINAL;
   }
   return results;
}


//------------------------------------------------------------------------------
// bool Optimize()
//------------------------------------------------------------------------------
bool FminconOptimizer::Optimize()
{
   bool retval = false;

   #ifdef DEBUG_ML_CONNECTIONS
   MessageInterface::ShowMessage("Entering Optimize method ....\n");
   #endif

   // set format long so that we don't lose precision between string transmission
   matlabIf->EvalString("format long");
   // clear last errormsg
   matlabIf->EvalString("clear errormsg");

   // set up options/values list for OPTIMSET call
   bool allEmpty = true;
   std::string optionsStr     = "GMAToptions = optimset(";
   std::string defaultOptions = optionsStr +"\'fmincon\');";
   std::ostringstream optS;
   bool debugMatlabIF = GmatGlobal::Instance()->IsMatlabDebugOn();

   if (debugMatlabIF) // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   {
      MessageInterface::ShowMessage(
      "In Optimize method, the number of options is %d ....\n",
      (Integer)options.size());
      MessageInterface::ShowMessage(
      "In Optimize method, the number of option values is %d ....\n",
      (Integer)optionValues.size());
   } // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~

   // Add in the maxIterations setting
   std::stringstream maxIter;
   maxIter << maxIterations;
   StringArray::iterator maxIterIterator = find(options.begin(), options.end(), "MaxIter");
   if (maxIterIterator == options.end())
   {
      options.push_back("MaxIter");
      optionValues.push_back(maxIter.str());
   }
   else
   {
      for (UnsignedInt i = 0; i < options.size(); ++i)
         if (options.at(i) == "MaxIter")
            optionValues.at(i) = maxIter.str();
   }

   for (Integer i=0; i < (Integer) options.size(); i++)
   {
      if (optionValues.at(i) != "")
      {
         allEmpty = false;
         if (i != 0) optS << ",";
         optS << "\'" << options.at(i)      << "\',";
         /// @todo do this in a better way, not using 7->11, etc.
         if ((7 <= i) && (i <= 11)) // put single quotes around strings
            optS << "\'" << optionValues.at(i) << "\'";
         else
            optS << optionValues.at(i);         }
   }
   optionsStr += optS.str() + ");";
   if (debugMatlabIF) // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   {
      MessageInterface::ShowMessage(
      "In Optimize method, the options are: %s ....\n", optionsStr.c_str());
   } // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   // call OPTIMSET (using EvalStr) to set up options for fmincon
   if (allEmpty) // if none were set, set options to be default for fmincon
      EvalMatlabString(defaultOptions);
   else
      EvalMatlabString(optionsStr);

   std::string inParm;
   std::ostringstream mlS;
   // pass to MATLAB the X0 array (needs to be a column vector)
   mlS.str("");
   mlS.precision(18);

   for (Integer i=0;i<(Integer)variable.size();i++)
      mlS << variable.at(i) << ";";
   inParm = "X0 = [" + mlS.str() + "];";
   if (debugMatlabIF) // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   {
      MessageInterface::ShowMessage(
      "In Optimize method, parameter string is: %s ....\n", inParm.c_str());
   } // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   EvalMatlabString(inParm);

   // pass to MATLAB the Lower column vector
   mlS.str("");
   for (Integer i=0;i<(Integer)variableMinimum.size();i++)
      mlS << variableMinimum.at(i) << ";";
   inParm = "Lower = [" + mlS.str() + "];";
   if (debugMatlabIF) // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   {
      MessageInterface::ShowMessage(
      "In Optimize method, parameter string is: %s ....\n", inParm.c_str());
   } // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   EvalMatlabString(inParm);

   // pass to MATLAB the Upper column vector
   mlS.str("");
   for (Integer i=0;i<(Integer)variableMaximum.size();i++)
      mlS << variableMaximum.at(i) << ";";

   inParm = "Upper = [" + mlS.str() + "];";
   if (debugMatlabIF) // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   {
      MessageInterface::ShowMessage(
      "In Optimize method, parameter string is: %s ....\n", inParm.c_str());
   } // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   EvalMatlabString(inParm);

   // clear last errormsg
   matlabIf->EvalString("clear errormsg");
   //std::string runString =
   //   "[X] = GmatFminconOptimizationDriver(X0,Lower,Upper);";
   // wcs - made it into an m-file instead of a function
   std::string runString = "GmatFminconOptimizationDriver;";
   EvalMatlabString(runString);
   // ask MATLAB for the value of exitFlag here and evaluate
   double      outArr[1];
   int         OKint = 0;
   std::string resStr    = "exitFlag";
   Integer numRowsReceived = 0;
   Integer numColsReceived = 0;
   OKint = matlabIf->GetRealArray(resStr, 1, outArr, numRowsReceived, numColsReceived);
   
   if (!OKint)
      throw SolverException("Error determining exitFlag from fmincon");
   fminconExitFlag = (Integer) outArr[0];

   if (fminconExitFlag > 0)
   {
      retval = true;
      converged = true;
   }

   // need to ask for fVal as well?
   return retval;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the FminconOptimizer.
 *
 * @return clone of the FminconOptimizer.
 */
//------------------------------------------------------------------------------
GmatBase* FminconOptimizer::Clone() const
{
   return (new FminconOptimizer(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void FminconOptimizer::Copy(const GmatBase* orig)
{
   operator=(*((FminconOptimizer *)(orig)));
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
std::string FminconOptimizer::GetParameterText(const Integer id) const
{
   if ((id >= ExternalOptimizerParamCount) && (id < FminconOptimizerParamCount))
      return PARAMETER_TEXT[id - ExternalOptimizerParamCount];
   if ((id >= MATLAB_OPTIONS_OFFSET) &&
       (id <  (MATLAB_OPTIONS_OFFSET + NUM_MATLAB_OPTIONS)))
   {
      return ALLOWED_OPTIONS[id - MATLAB_OPTIONS_OFFSET];
   }
   return ExternalOptimizer::GetParameterText(id);
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
Integer FminconOptimizer::GetParameterID(const std::string &str) const
{
   // part 1: this part will be removed in future build // made a change here
   if ((str == "DerivativeCheck")||(str == "Diagnostics")||
       (str == "Display")||(str == "GradObj")||
       (str == "GradConstr")||(str == "MaxIter"))
   {
      return Gmat::PARAMETER_REMOVED; // return the id of the fields no longer in use
   }

   // part 2:
   for (Integer i = ExternalOptimizerParamCount; i < FminconOptimizerParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - ExternalOptimizerParamCount])
         return i;
   }
   for (Integer j =0; j < NUM_MATLAB_OPTIONS; j++)
      if (str == ALLOWED_OPTIONS[j])
         return (MATLAB_OPTIONS_OFFSET + j);

   return ExternalOptimizer::GetParameterID(str);
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
Gmat::ParameterType FminconOptimizer::GetParameterType(const Integer id) const
{
   if ((id >= ExternalOptimizerParamCount) && (id < FminconOptimizerParamCount))
      return PARAMETER_TYPE[id - ExternalOptimizerParamCount];

   if ((id >= MATLAB_OPTIONS_OFFSET) &&
       (id <  (MATLAB_OPTIONS_OFFSET + NUM_MATLAB_OPTIONS)))
   {
      return Gmat::STRING_TYPE;
   }
   return ExternalOptimizer::GetParameterType(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool FminconOptimizer::IsParameterReadOnly(const Integer id) const
{
   if (id == OPTIMIZER_TOLERANCE || id == SOURCE_TYPE)
      return true;

   return ExternalOptimizer::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool FminconOptimizer::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string FminconOptimizer::GetParameterTypeString(const Integer id) const
{
   return ExternalOptimizer::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string FminconOptimizer::GetStringParameter(const Integer id) const
{
   if ((id >= MATLAB_OPTIONS_OFFSET) &&
       (id <  (MATLAB_OPTIONS_OFFSET + NUM_MATLAB_OPTIONS)))
   {
      return optionValues[id - MATLAB_OPTIONS_OFFSET];
   }
   return ExternalOptimizer::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool FminconOptimizer::SetStringParameter(const Integer id,
                                          const std::string &value)
{
   #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Fmincon::setting id %d value to %s\n",
      id, value.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~

   if ((id >= MATLAB_OPTIONS_OFFSET) &&
       (id <  (MATLAB_OPTIONS_OFFSET + NUM_MATLAB_OPTIONS)))
   {
      if (!IsAllowedValue(options[id - MATLAB_OPTIONS_OFFSET], value))
      {
          std::string errorStr = "FminconOptimizer error: Value " +
               value + " not valid for option "
               + options[id - MATLAB_OPTIONS_OFFSET] + "\n";
          throw SolverException(errorStr);
      }
      optionValues[id - MATLAB_OPTIONS_OFFSET] = value;
      return true;
   }
   return ExternalOptimizer::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string FminconOptimizer::GetStringParameter(const std::string &label) const
{
   for (Integer i=0; i<NUM_MATLAB_OPTIONS; i++)
      if (label == options[i])
      {
         return optionValues[i];
      }
   return ExternalOptimizer::GetStringParameter(label);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool FminconOptimizer::SetStringParameter(const std::string &label,
                                          const std::string &value)
{

   #ifdef DEBUG_FMINCON_OPTIONS // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage("Fmincon::setting param %s value to %s\n",
      label.c_str(), value.c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   // check options first
   for (Integer i=0; i<NUM_MATLAB_OPTIONS; i++)
      if (label == options[i])
      {
         if (!IsAllowedValue(label, value))
         {
            std::string errorStr = "FminconOptimizer error: Value " +
               value + " not valid for option " + label + "\n";
            throw SolverException(errorStr);
         }
         optionValues[i] = value;
         return true;
      }
   return ExternalOptimizer::SetStringParameter(label, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
std::string FminconOptimizer::GetStringParameter(const Integer id,
                                                 const Integer index) const
{
   if (id == OPTIONS)
   {
     if ((index < 0) || (index >= (Integer) options.size()))
         throw SolverException("FminconOptimizer::GetString index out of range.");
     return options[index];
   }
   if (id == OPTION_VALUES)
   {
      if ((index < 0) || (index >= (Integer) optionValues.size()))
         throw SolverException("FminconOptimizer::GetString index out of range.");
      return optionValues[index];
   }
    return ExternalOptimizer::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
bool FminconOptimizer::SetStringParameter(const Integer id,
                                          const std::string &value,
                                          const Integer index)
{
   if (id == OPTIONS)
   {
     if ((index < 0) || (index > (Integer) options.size()))
         throw SolverException("FminconOptimizer::SetString index out of range.");
     if (!IsAllowedOption(value))
         throw SolverException(
            "FminconOptimizer::SetString - invalid option value: " + value);
     if (((Integer) optionValues.size()) > index)
     {
        if (!IsAllowedValue(value, optionValues[index]))
           throw SolverException(
              "FminconOptimizer::SetString - invalid value for option "
              + value);
     }

     if (index == (Integer) options.size())
     {
         options.push_back(value);
     }
     else  // substitute string for already existing one
        options[index] = value;

     return true;
   }
   if (id == OPTION_VALUES)
   {
      if ((index < 0) || (index > (Integer) optionValues.size()))
         throw SolverException("FminconOptimizer::SetString index out of range.");
     if (((Integer) options.size()) > index)
        if (!IsAllowedValue(options[index], value))
           throw SolverException(
           "FminconOptimizer::SetString - invalid value for option "
           + options[index]);
      if (index == (Integer) optionValues.size())
      {
         optionValues.push_back(value);
      }
      else // replace a value currently there
         optionValues[index] = value;

      return true;
   }
    return ExternalOptimizer::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//                                const Integer index) const
//------------------------------------------------------------------------------
std::string FminconOptimizer::GetStringParameter(const std::string &label,
                                                 const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
bool FminconOptimizer::SetStringParameter(const std::string &label,
                                          const std::string &value,
                                          const Integer index)
{
   return SetStringParameter(GetParameterID(label),value,index);
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
const StringArray& FminconOptimizer::GetStringArrayParameter(
                                                        const Integer id) const
{
   if (id == OPTIONS)
      return options;
   if (id == OPTION_VALUES)
      return optionValues;

   return ExternalOptimizer::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void CompleteInitialization()
//------------------------------------------------------------------------------
void FminconOptimizer::CompleteInitialization()
{
   ExternalOptimizer::CompleteInitialization();
   // set callback class pointer for the GmatInterface -> probably don't need to
}


//------------------------------------------------------------------------------
// void RunExternal()
//------------------------------------------------------------------------------
void FminconOptimizer::RunExternal()
{
   Optimize();
   currentState = FINISHED;
   nestedState  = INITIALIZING;
}


//------------------------------------------------------------------------------
// void RunNominal()
//------------------------------------------------------------------------------
void FminconOptimizer::RunNominal()
{
   ++iterationsTaken;
   WriteToTextFile(nestedState);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void FminconOptimizer::CalculateParameters()
{
   //ExternalOptimizer::CalculateParameters();
   // check to make sure we have all of the data we need, from Minimize, etc.?
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void FminconOptimizer::RunComplete()
{
   ExternalOptimizer::RunComplete();
   status = Gmat::RUN;  // Enables "Apply corrections"
   GmatGlobal::Instance()->SetSolverStatus(instanceName, status);
   hasFired = true;

   WriteToTextFile();
}


//------------------------------------------------------------------------------
// void FreeArrays()
//------------------------------------------------------------------------------
void FminconOptimizer::FreeArrays()
{
   ExternalOptimizer::FreeArrays();
}


//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string that reporting the current differential corrector state.
 */
//------------------------------------------------------------------------------
std::string FminconOptimizer::GetProgressString()
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
            progress << instanceName << " Control Sequence Pass " << iterationsTaken+1
                     << "; Fmincon Pass\n   Variables:  ";
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
            progress << "   Completed Control Sequence Pass " << iterationsTaken
                     << ", pert " << pertNumber+1 << " ("
                     << variableNames[pertNumber] << " = "
                     << variable[pertNumber] << ")\n";
            break;

         case CALCULATING:
            // Just forces a blank line
            break;

         case CHECKINGRUN:
            // Iterate through the constraints, writing them to the file
            if (eqConstraintNames.size() > 0)
            {
               progress << "   Equality constraint variances:\n      ";

               for (current = eqConstraintNames.begin(), i = 0;
                    current != eqConstraintNames.end(); ++current)
               {
                  if (current != eqConstraintNames.begin())
                     progress << ",  ";
                  progress << *current // << "  Desired: " << eqConstraint[i]
                           << "  Achieved: " << eqConstraintValues[i];
                  ++i;
               }
            }
            if (ineqConstraintNames.size() > 0)
            {
               progress << "   Inequality constraint variances:\n      ";

               for (current = ineqConstraintNames.begin(), i = 0;
                    current != ineqConstraintNames.end(); ++current)
               {
                  if (current != ineqConstraintNames.begin())
                     progress <<  ",  ";
                  progress << *current // << "  Desired: " << eqConstraint[i]
                           << "  Achieved: " << ineqConstraintValues[i];
                  ++i;
               }
            }
            if (objectiveDefined)
               progress << "\n   Cost Function Value: " << cost;
             break;

         case RUNEXTERNAL:
            progress << instanceName << " Control Sequence Pass " << iterationsTaken+1
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
            {
               progress << "\n*** Optimization Completed in " << iterationsTaken
                        << " passes through the Solver Control Sequence\n"
                        << "*** The Optimizer Converged!";

               status = Gmat::CONVERGED;
            }
            else
            {
               progress << "\n*** Optimization did not converge in "
                        << iterationsTaken
                        << " passes through the Solver Control Sequence";
               status = Gmat::FAILED;
            }         
            if ((iterationsTaken >= maxIterations) && (converged == false))
            {
               progress << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                        << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                        << "!!! WARNING: Optimizer did NOT converge in "
                        << maxIterations << " iterations!"
                        << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                        << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
               status = Gmat::EXCEEDED_ITERATIONS;
            }

            GmatGlobal::Instance()->SetSolverStatus(instanceName, status);

            progress << "\nFinal Variable values:\n";
            // Iterate through the variables, writing them to the string
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
               progress << "   " << *current << " = " << variable[i++] << "\n";
            break;

         case ITERATING:     // Intentional fall through
         default:
            throw SolverException(
               "Solver state not supported for the optimizer");
      }
   }
   else
      return Solver::GetProgressString();
      
   return progress.str();

}


//------------------------------------------------------------------------------
// void WriteToTextFile(SolverState stateToUse)
//------------------------------------------------------------------------------
void FminconOptimizer::WriteToTextFile(SolverState stateToUse)
{
   StringArray::iterator current;
   Integer i;

   SolverState trigger = currentState;
   if (stateToUse != UNDEFINED_STATE)
      trigger = stateToUse;

   if (!textFile.is_open())
      OpenSolverTextFile();

   if (isInitialized)
   {
      std::stringstream message;
      message.precision(16);

      switch (trigger)
      {
         case INITIALIZING:
            // This state is basically a "paused state" used for the Optimize
            // command to finalize the initial data for the variables and
            // goals.  All that is written here is the header information.
            {
               Integer localVariableCount = variableNames.size();
               Integer localEqCount = eqConstraintNames.size();
               Integer localIneqCount = ineqConstraintNames.size();
               message << "************************************************"
                        << "********\n"
                        << "*** Fmincon Text File\n"
                        << "*** \n"
                        << "*** Using Fmincon Optimization\n***\n";

               // Write out the setup data
               message << "*** " << localVariableCount << " variables\n*** "
                        << localEqCount << " equality constraints\n***\n*** "
                        << localIneqCount << " inequality constraints\n***\n*** "
                        << "Variables:\n***    ";

               // Iterate through the variables and constraints, writing them to
               // the file
               for (current = variableNames.begin(), i = 0;
                    current != variableNames.end(); ++current)
               {
                  message << *current << "\n***    ";
               }

               message << "\n*** Equality Constraints:\n***    ";

               for (current = eqConstraintNames.begin(), i = 0;
                    current != eqConstraintNames.end(); ++current, ++i)
               {
                  message << *current
                  // We'd like to do this, but the data isn't currently passed 
                  // into the Optimizer
                  //        << " = " << eqConstraintTargetValues[i] 
                          << "\n***    ";
               }

               message << "\n*** Inequality Constraints:\n***    ";

               for (current = ineqConstraintNames.begin(), i = 0;
                    current != ineqConstraintNames.end(); ++current, ++i)
               {
                  message << *current 
                  // We'd like to do this, but the data isn't currently passed 
                  // into the Optimizer
                  //        << " = " << ineqConstraintTargetValues[i] 
                          << "\n***    ";
               }
               message << "\n****************************"
                        << "****************************\n"
                        << std::endl;
            }
            break;

         case NOMINAL:
//            message << "Pass " << iterationsTaken
//                     << "\n   Variable Values: [";
//            // Iterate through the variables, writing them to the file
//            for (current = variableNames.begin(), i = 0;
//                 current != variableNames.end(); ++current)
//            {
//               message << " " << unscaledVariable[i++] << " ";
//            }
//            message << "]" << std::endl;
                 message << instanceName << " Control Sequence Pass " << iterationsTaken+1
                      << "; Fmincon Pass\n   Variables:  ";
             // Iterate through the variables, writing them to the string
             for (current = variableNames.begin(), i = 0;
                  current != variableNames.end(); ++current)
             {
                if (current != variableNames.begin())
                        message << ", ";
                message << *current << " = " << unscaledVariable.at(i);
                if (textFileMode == "Verbose")
                        message << "; optimizer scaled value: " << variable[i];
                ++i;
             }
             message << "\n";
            break;


         case CALCULATING:
            if (textFileMode == "Verbose")
               message << "In the Calculating state" << std::endl;
            if (ineqConstraintCount > 0)
            {
               message << "   Inequality Constraint Deltas: [";
               for (std::vector<Real>::iterator i = ineqConstraintValues.begin();
                    i != ineqConstraintValues.end(); ++i)
                  message << " " << (*i) << " ";
               message << "]" << std::endl;
            }
            if (eqConstraintCount > 0)
            {
               message << "   Equality Constraint Deltas: [";

               for (std::vector<Real>::iterator i = eqConstraintValues.begin();
                    i != eqConstraintValues.end(); ++i)
                  message << " " << (*i) << " ";
               message << "]" << std::endl;
            }
            if (objectiveDefined)
               message << "   Objective function value:  " << cost << std::endl;
            break;

         case FINISHED:
            {
               std::string completionState;
               bool convergenceMet = false;

               switch(fminconExitFlag)
               {
               case 1:
                  completionState = "First order optimality conditions were satisfied";
                  convergenceMet = true;
                  break;

               case 2:
                  completionState = "Variables are as close as possible to the optimal point";
                  convergenceMet = true;
                  break;

               case 3:
                  completionState = "Changes in the objective function are smaller than the minimum change for convergence";
                  convergenceMet = true;
                  break;

               case 4:
                  completionState = "The search direction is too small for further optimization";
                  convergenceMet = true;
                  break;

               case 5:
                  completionState = "Changes in the objective function are smaller than the convergence criteria";
                  convergenceMet = true;
                  break;

               case 0:
                  completionState = "Too many function evaluations or iterations attempted.";
                  break;

               case -1:
                  completionState = "Optimization halted by the output or plot function";
                  break;

               case -2:
                  completionState = "No feasible optimization state was located";
                  break;

               case -3:
                  completionState = "The optimization state appears to be unbounded";
                  break;

               default:
                  completionState = "An unknown return code was received from fmincon";
                  break;
               };

               if (convergenceMet)
               {
                  message << "\n****************************"
                           << "****************************\n"
                           << "*** Optimization Converged in " << iterationsTaken
                           << " Control Sequence Passes"
                           << "\n****************************"
                           << "****************************\n"
                           << std::endl
                           << "Convergence meets the following criterion:\n   "
                           << completionState << std::endl;
               }
               else
               {
                  message  << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                           << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                           << "!!! Optimization Failed to Converge"
                           << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                           << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                           << std::endl
                           << "Convergence failed because of the following condition:\n   "
                           << completionState << std::endl;
               }
            }
            break;

         case RUNEXTERNAL:
            message << "Control Sequence Pass " << iterationsTaken+1
                     << "\nExternal Run\nVariables:\n   ";
            // Iterate through the variables, writing them to the file
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               message << *current << " = " << unscaledVariable[i++] << "\n   ";
            }
            message << std::endl;
            break;

         default:
            MessageInterface::ShowMessage(
            "Solver state %d not supported for FminconOptimizer\n",
            currentState);
            //throw SolverException(
            //   "Solver state not supported for the Fmincon optimizer");
      }
      textFile << message.str();

      if (showProgress)
         MessageInterface::ShowMessage("%s", message.str().c_str());
      else
      {
         // Always show the convergence status message
         if (trigger == FINISHED)
            MessageInterface::ShowMessage("%s", message.str().c_str());
      }
   }
}


//------------------------------------------------------------------------------
// bool OpenConnection()
//------------------------------------------------------------------------------
bool FminconOptimizer::OpenConnection()
{
   bool debugMatlabIF = GmatGlobal::Instance()->IsMatlabDebugOn();

   if (debugMatlabIF) // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
   {
      MessageInterface::ShowMessage("\nFminconOptimizer::OpenConnection() entered\n");
   } // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~

   if (!GmatGlobal::Instance()->IsMatlabAvailable())
   {
      throw SolverException
         ("Error attempting to access interface to MATLAB.\nMATLAB is not installed "
          "on the system or use of MATLAT was disabled from the gmat_startup_file.\n");
   }

   matlabIf = MatlabInterface::Instance();

   if (debugMatlabIF)
   {
      MessageInterface::ShowMessage
      ("Got MatlabInterface pointer = %p\n", matlabIf);
   }

   // open the MatlabInterface
   if (!matlabIf->Open("GmatMatlab"))
      throw SolverException
         ("Error attempting to access interface to MATLAB.\nMATLAB is not installed "
          "on the system or GMAT was not built with MATLAB engine.\n");

   // clear the last error message
   matlabIf->EvalString("clear errormsg");

   // Get current path and cd (LOJ: 2010.08.11)
   std::string evalStr;
   std::string resStr;
   std::string currPath = FileManager::Instance()->GetCurrentWorkingDirectory();

   // Shoud I cd to current path before adding relative path?
   RunCdCommand(currPath);

   // Add path to the top of the Matlab path in reverse order(loj: 2008.10.16)
   // since FileManager::GetAllMatlabFunctionPaths() returns in top to bottom order
   FileManager *fm = FileManager::Instance();
   StringArray paths = fm->GetAllMatlabFunctionPaths();
   std::string pathName;
   StringArray::reverse_iterator rpos = paths.rbegin();
   while (rpos != paths.rend())
   {
      pathName = *rpos;
      if (pathName != "")
      {
         if (debugMatlabIF)
         {
            MessageInterface::ShowMessage
               ("Adding matlab path '%s' to the top\n", pathName.c_str());
         }
         std::string addPath = "path('" + pathName + "', path)";
         matlabIf->EvalString(addPath);
      }
      rpos++;
   }

   // Now add the function path to the top using path('newpath', path)
   if (functionPath != "")
   {
      std::string setPath = "path('" + functionPath + "', path)";
      EvalMatlabString(setPath);
      if (debugMatlabIF) // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      {
         MessageInterface::ShowMessage("MATLAB path set to %s\n",
         functionPath.c_str());
      } // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
   }

   // check for availability of Optimization Toolbox (well, really
   // just fmincon, but of course, its existence implies the existence of
   // the entire toolbox) here
   double      outArr[1], outArr2[1];
   double      out1[1], out2[1], out3[1], out4[1];
   int         OKint = 0;
   evalStr   = "fminconexist = exist(\'fmincon\');";
   resStr    = "fminconexist";
   EvalMatlabString(evalStr);
   Integer numRowsReceived = 0;
   Integer numColsReceived = 0;
   OKint = matlabIf->GetRealArray(resStr, 1, outArr, numRowsReceived, numColsReceived);

   if (!OKint)
      throw SolverException(
            "Error determining existence of Optimization Toolbox");

   if (outArr[0] > 0.0) // 2 means it is in the MATLAB path
   {
      if (debugMatlabIF)
      {   MessageInterface::ShowMessage(
         "fmincon exists (code = %.4f)\n", outArr[0]);
      }
      evalStr   = "startupexist = exist(\'gmat_startup\');";
      resStr    = "startupexist";
      EvalMatlabString(evalStr);
      OKint     = matlabIf->GetRealArray(resStr, 1, outArr2, numRowsReceived, numColsReceived);
      if (!OKint)
         throw SolverException(
               "Error determining existence of MATLAB gmat_startup");

      if (outArr2[0] > 0.0) // 2 means it is in the MATLAB path
      {
         if (debugMatlabIF) // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         {
            MessageInterface::ShowMessage(
            "gmat_startup exists (code = %.4f), running gmat_startup\n",
            outArr2[0]);
         } // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~

         // Find directory that has gmat_startup.m and cd to that directory
         // before running gmat_startup.m
         evalStr = "whichStr = which('gmat_startup');";
         EvalMatlabString(evalStr);
         if (matlabIf->GetString("whichStr", resStr) == 1)
         {
            if (debugMatlabIF)
               MessageInterface::ShowMessage
                  ("Here is the path of gmat_startup:\n%s\n", resStr.c_str());

            std::string startupPath = GmatFileUtil::ParsePathName(resStr);

            // cd to directory has gmat_startup.m
            RunCdCommand(startupPath);
         }
         else
         {
            MessageInterface::ShowMessage
               ("Unable to get whichStr from MATLAB workspace.\n");
         }

         // run the startup file to add to the MATLAB path correctly
         evalStr = "gmat_startup;";
         EvalMatlabString(evalStr);
         // check for existence on the path of the support MATLAB files
         evalStr = "driverexist = exist(\'GmatFminconOptimizationDriver\');";
         resStr  = "driverexist";
         EvalMatlabString(evalStr);
         OKint   = matlabIf->GetRealArray(resStr, 1, out1, numRowsReceived, numColsReceived);
         evalStr = "objectiveexist = exist(\'EvaluateGMATObjective\');";
         resStr  = "objectiveexist";
         EvalMatlabString(evalStr);
         OKint   = matlabIf->GetRealArray(resStr, 1, out2, numRowsReceived, numColsReceived);
         evalStr = "constraintexist = exist(\'EvaluateGMATConstraints\');";
         resStr  = "constraintexist";
         EvalMatlabString(evalStr);
         OKint   = matlabIf->GetRealArray(resStr, 1, out3, numRowsReceived, numColsReceived);
         evalStr = "callbackexist = exist(\'CallGMATfminconSolver\');";
         resStr  = "callbackexist";
         EvalMatlabString(evalStr);
         OKint   = matlabIf->GetRealArray(resStr, 1, out4, numRowsReceived, numColsReceived);
         if (debugMatlabIF) // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         {
            MessageInterface::ShowMessage(
            "existence codes for support files  = %.4f  %.4f  %.4f  %.4f\n",
            out1[0], out2[0], out3[0], out4[0]);
         } // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~

         if ((out1[0] <= 0.0) || (out2[0] <= 0.0) ||
             (out3[0] <= 0.0) || (out4[0] <= 0.0))
         {
            std::string missingFiles;
            if (out1[0] <= 0.0)
               missingFiles = missingFiles + "GmatFminconOptimizationDriver, ";
            if (out2[0] <= 0.0)
               missingFiles = missingFiles + "EvaluateGMATObjective, ";
            if (out3[0] <= 0.0)
               missingFiles = missingFiles + "EvaluateGMATConstraints, ";
            if (out4[0] <= 0.0)
               missingFiles = missingFiles + "CallGMATfminconSolver, ";

            if (debugMatlabIF)
               WriteSearchPath("In FminconOptimizer::OpenConnection():");

            throw SolverException
               ("MATLAB support files: " + missingFiles + " not in the MATLAB search path\n");
         }
         if (inSource == NULL) inSource = GmatInterface::Instance();
         if (inSource == NULL)
            throw SolverException(
            "Error attempting to access GMAT server (to MATLAB)");

         // Should I cd to initial directory?
         //RunCdCommand(currPath);

         // ********* BEGIN temporary prototype, testing, etc. *****************************//
         //inSource->RegisterCallbackServer(this);
         //evalStr = "bogusFunction;";
         //MessageInterface::ShowMessage("Now trying to execute bogusFunction ...\n");
         //int OK = matlabIf->EvalString(evalStr);
         //if (OK == 0)
         //   MessageInterface::ShowMessage("ERROR calling EvalString\n");
         //MessageInterface::ShowMessage("Done executing bogusFunction\n");
         // ********* END temporary prototype, testing, etc. *****************************//
      }
      else
      {
         if (debugMatlabIF)
            WriteSearchPath("In FminconOptimizer::OpenConnection():");

         throw SolverException("gmat_startup.m not in the MATLAB search path\n");
      }
   }
   else
   {
      if (debugMatlabIF)
         WriteSearchPath("In FminconOptimizer::OpenConnection():");

      throw SolverException("fmincon.m not in the MATLAB search path\n");
   }

   if (debugMatlabIF)
      MessageInterface::ShowMessage("FminconOptimizer::OpenConnection() leaving\n\n");

   return true;
}


//------------------------------------------------------------------------------
// void CloseConnection()
//------------------------------------------------------------------------------
void FminconOptimizer::CloseConnection()
{
   matlabIf->Close();   // but wait!  what if someone else is still using it??
   // no need to close anything when running the server??
}


//------------------------------------------------------------------------------
// void WriteParameters(std::string &prefix, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Code that writes the parameter details for an object.
 *
 * @param prefix Starting portion of the script string used for the parameter.
 * @param obj The object that is written.
 */
//------------------------------------------------------------------------------
void FminconOptimizer::WriteParameters(Gmat::WriteMode mode, std::string &prefix,
                                 std::stringstream &stream)
{
   Integer id;
   Gmat::ParameterType parmType;
   std::stringstream value;
   value.precision(GetDataPrecision());
   for (id = 0; id < parameterCount; ++id)
   {
      if (IsParameterReadOnly(id) == false)
      {
         parmType = GetParameterType(id);
         if (parmType != Gmat::STRINGARRAY_TYPE)
         {
            parmType = GetParameterType(id);
            // Skip unhandled types
            if (
                (parmType != Gmat::UNSIGNED_INTARRAY_TYPE) &&
                (parmType != Gmat::RVECTOR_TYPE) &&
                (parmType != Gmat::RMATRIX_TYPE) &&
                (parmType != Gmat::UNKNOWN_PARAMETER_TYPE)
               )
            {
               // Fill in the l.h.s.
               value.str("");
               WriteParameterValue(id, value);
               if (value.str() != "")
               {
                    std::string attCmtLn = GetAttributeCommentLine(id);

                    if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) ||
                     (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                     stream << attCmtLn;

                  stream << prefix << GetParameterText(id)
                         << " = " << value.str() << ";";

                  // overwrite tmp variable for attribute cmt line
                  attCmtLn = GetInlineAttributeComment(id);
                  if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) ||
                      (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                     stream << attCmtLn << "\n";
                  else
                     stream << "\n";
               }
            }
         }
         else
         {
            // Handle StringArrays
            if ((id != OPTIONS) && (id != OPTION_VALUES))
            {
               StringArray sar = GetStringArrayParameter(id);
               if (sar.size() > 0)
               {
                  std::string attCmtLn = GetAttributeCommentLine(id);

                  if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) ||
                     (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                  {
                     stream << attCmtLn.c_str();
                  }

                  stream << prefix << GetParameterText(id) << " = {";

                  for (StringArray::iterator n = sar.begin(); n != sar.end(); ++n)
                  {
                     if (n != sar.begin())
                        stream << ", ";
                     if (inMatlabMode)
                        stream << "'";
                     stream << (*n);
                     if (inMatlabMode)
                        stream << "'";
                  }

                  attCmtLn  = GetInlineAttributeComment(id);

                  if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) ||
                     (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                  {
                     stream << "};" << attCmtLn << "\n";
                  }
                  else
                     stream << "};\n";
               }
            }
            else if (id == OPTIONS)
            {
               // Handle options list
               for (Integer ii = 0; ii < NUM_MATLAB_OPTIONS; ii++)
               {
                  value.str("");
                  WriteParameterValue((ii + MATLAB_OPTIONS_OFFSET), value);
                  if (value.str() != "")
                  {
                       std::string attCmtLn = GetAttributeCommentLine(id);

                       if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) ||
                        (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                        stream << attCmtLn;

                     stream << prefix << GetParameterText(ii + MATLAB_OPTIONS_OFFSET)
                            << " = " << value.str() << ";";

                     // overwrite tmp variable for attribute cmt line
                     attCmtLn = GetInlineAttributeComment(id);
                     if ((attCmtLn != "") && ((mode == Gmat::SCRIPTING) ||
                         (mode == Gmat::OWNED_OBJECT) || (mode == Gmat::SHOW_SCRIPT)))
                        stream << attCmtLn << "\n";
                     else
                        stream << "\n";
                  }
               }
            }
            else
               ; // do nothing for OPTION_VALUES - handled with OPTIONS
         }
      }
   } // for
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// bool IsAllowedOption(const std::string &str)
//------------------------------------------------------------------------------
bool FminconOptimizer::IsAllowedOption(const std::string &str)
{
   for (Integer i=0; i<NUM_MATLAB_OPTIONS; i++)
      if (str == FminconOptimizer::ALLOWED_OPTIONS[i])  return true;
   return false;
}


//------------------------------------------------------------------------------
// bool IsAllowedValue(const std::string &opt, const std::string &val)
//------------------------------------------------------------------------------
bool FminconOptimizer::IsAllowedValue(const std::string &opt,
                                      const std::string &val)
{
   if ((opt == ALLOWED_OPTIONS[0]) ||
       (opt == ALLOWED_OPTIONS[1]) ||
       (opt == ALLOWED_OPTIONS[3]) ||
       (opt == ALLOWED_OPTIONS[4]) ||
       (opt == ALLOWED_OPTIONS[5]))
   {
      Real tmpVal;
      if (!GmatStringUtil::ToReal(val.c_str(), &tmpVal)) return false;
      if (tmpVal <= 0.0)
         return false;
      return true;
   }
   else if (opt == ALLOWED_OPTIONS[2])
   {
      if (atoi(val.c_str()) > 0) return true;
      return false;
   }

   return false;
}


//------------------------------------------------------------------------------
// void RunCdCommand(const std::string &pathName)
//------------------------------------------------------------------------------
void FminconOptimizer::RunCdCommand(const std::string &pathName)
{
   std::string evalStr, resStr;
   bool debugMatlabIF = GmatGlobal::Instance()->IsMatlabDebugOn();

   evalStr = "cd ",
   evalStr = evalStr + pathName;
   EvalMatlabString(evalStr);

   if (debugMatlabIF)
   {
      MessageInterface::ShowMessage
         ("Changed directory to: %s\n", pathName.c_str());

      evalStr = "pwdStr = pwd";
      EvalMatlabString(evalStr);

      if (matlabIf->GetString("pwdStr", resStr) == 1)
         MessageInterface::ShowMessage
            ("The current path is:\n%s\n", resStr.c_str());
      else
         MessageInterface::ShowMessage
            ("Unable to get pwdStr from MATLAB workspace.\n");
   }
}


//------------------------------------------------------------------------------
// void WriteSearchPath(const std::string &msg)
//------------------------------------------------------------------------------
void FminconOptimizer::WriteSearchPath(const std::string &msg)
{
   std::string evalStr, resStr;

   MessageInterface::ShowMessage("%s\n", msg.c_str());
   evalStr = "pathStr = path; pathStr = regexprep(pathStr, ';', '\\n');";
   EvalMatlabString(evalStr);

   if (matlabIf->GetString("pathStr", resStr) == 1)
      MessageInterface::ShowMessage
         ("The current path is:\n%s\n", resStr.c_str());
   else
      MessageInterface::ShowMessage
         ("Unable to get pathStr from MATLAB workspace.\n");
}


//------------------------------------------------------------------------------
// void EvalMatlabString(const std::string &evalString)
//------------------------------------------------------------------------------
void FminconOptimizer::EvalMatlabString(const std::string &evalString)
{
   #ifdef DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage
      ("CallMatlabFunction::EvalMatlabString() calling MI::RunMatlabString() with\n"
       "======================================================================\n"
       "%s\n\n", evalString.c_str());
   #endif
   
   try
   {
      matlabIf->RunMatlabString(evalString);
   }
   catch (InterfaceException &ie)
   {
      bool rethrowException = true;
      std::string errMsg = ie.GetFullMessage();
      #ifdef DEBUG_MATLAB_EVAL
      MessageInterface::ShowMessage(errMsg + "\n");
      #endif
      
      // If error from evaluating string but not from opening matlab engine or
      // not from undefined function, or error from MATLAB, close and re-initialize
      // and re-execute as user may have manually closed it.
      if (errMsg.find("engEvalString") != errMsg.npos)
      {
         rethrowException = false;
         MessageInterface::ShowMessage("Trying to close and reopen MATLAB engine...\n");
         matlabIf->Close();
         Initialize();
      }
      
      if (rethrowException)
         throw;
   }
}

