//$Id: FminconOptimizer.hpp 9461 2011-04-21 22:10:15Z lindajun $
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
// Author: Wendy C. Shoan, GSFC
// Created: 2006.07.21
//
/**
 * Definition for the FminconOptimizer class. 
 */
//------------------------------------------------------------------------------
#ifndef FminconOptimizer_hpp
#define FminconOptimizer_hpp

#include "fmincon_defs.hpp"
#include "gmatdefs.hpp"
#include "ExternalOptimizer.hpp"

class MatlabInterface;

/**
 * Code that wraps MATLAB's fmincon optimizer for use in GMAT
 *
 * This class wraps MATLAB's fmincon optimizer into a GMAT Optimizer that works
 * with GMAT's Solver subsystem.  The resulting class can be used to instantiate
 * a Solver that works with the Optimize/EndOptimize, Vary, Minimize, and 
 * NonlinearConstraint commands.
 */
class FMINCON_API FminconOptimizer : public ExternalOptimizer
{
public:
   FminconOptimizer(const std::string &name);
   virtual ~FminconOptimizer();
   FminconOptimizer(const FminconOptimizer &opt);
   FminconOptimizer&    operator=(const FminconOptimizer& opt);
   
   virtual bool         Initialize();
   virtual SolverState  AdvanceState();
   virtual StringArray  AdvanceNestedState(std::vector<Real> vars);
   virtual bool         Optimize();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   // Access methods overriden from the base class
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   
   virtual void         WriteParameters(Gmat::WriteMode mode, std::string &prefix, 
                                        std::stringstream &stream);
   
   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:
   /// The list of fmincon paramters supported
   StringArray  options;
   /// The current values for the fmincon parameters
   StringArray  optionValues;
   /// The return code from fmincon when it completes an optimization
   Integer      fminconExitFlag;
   /// The interface ised to exchange data with fmincon running in MATLAB
   MatlabInterface *matlabIf;
   
   /// Table of options supported, excluding options inherited from base classes
   static const std::string    ALLOWED_OPTIONS[6];
   /// Default values for the supported options
   static const std::string    DEFAULT_OPTION_VALUES[6];
   /// Option count
   static const Integer        NUM_MATLAB_OPTIONS;
   /// Option strting index, for use in the parameter access methods
   static const Integer        MATLAB_OPTIONS_OFFSET;
   
   // Methods from Solver
   virtual void         CompleteInitialization();
   virtual void         RunExternal();
   virtual void         RunNominal();
   virtual void         CalculateParameters();
   virtual void         RunComplete();
   
   virtual void         FreeArrays();
   virtual void         WriteToTextFile(SolverState stateToUse = UNDEFINED_STATE);
   virtual std::string  GetProgressString();
   
   virtual bool         OpenConnection();
   virtual void         CloseConnection();
   
   virtual bool         IsAllowedOption(const std::string &str);
   virtual bool         IsAllowedValue(const std::string &opt,
                                       const std::string &val);
   
   void                 RunCdCommand(const std::string &pathName);
   void                 WriteSearchPath(const std::string &msg);
   void                 EvalMatlabString(const std::string &evalString);
   
public:
   // Parameter IDs
   enum
   {
      OPTIONS = ExternalOptimizerParamCount,
      OPTION_VALUES,
      FminconOptimizerParamCount
   };
   
   static const std::string
   PARAMETER_TEXT[FminconOptimizerParamCount - ExternalOptimizerParamCount];
   static const Gmat::ParameterType
   PARAMETER_TYPE[FminconOptimizerParamCount - ExternalOptimizerParamCount];
};

#endif // FminconOptimizer_hpp
