//$Id$
//------------------------------------------------------------------------------
//                                Optimize 
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author:  Daniel Hunter/GSFC/MAB (CoOp)
// Created: 2006.07.20
//
/**
 * Declaration for the Optimize command class
 */
//------------------------------------------------------------------------------

#ifndef Optimize_hpp
#define Optimize_hpp

#include "SolverBranchCommand.hpp"
#include "Solver.hpp"
#include "Spacecraft.hpp"

class GMAT_API Optimize : public SolverBranchCommand
{
public:
   Optimize();
   Optimize(const Optimize& o);
   Optimize& operator=(const Optimize& o);
   virtual ~Optimize();
    
   // Inherited methods that need some enhancement from the base class
   virtual bool        Append(GmatCommand *cmd);

   // Methods used to run the command
   virtual bool        Initialize();
   virtual bool        Execute();
   virtual void        RunComplete();

   // Method override to manage penup/pendown
   virtual bool        ExecuteBranch(Integer which = 0);

   // inherited from GmatBase
   // Method to execute a callback from an external function
   virtual bool        ExecuteCallback();
   virtual bool        PutCallbackData(std::string &data);
   virtual bool        PutCallbackRealData(RealArray &data);
   virtual std::string GetCallbackResults();
   virtual GmatBase*   Clone() const;
   const std::string&  GetGeneratingString(Gmat::WriteMode mode,
                                           const std::string &prefix,
                                           const std::string &useName);

   virtual bool        RenameRefObject(const UnsignedInt type,
                                       const std::string &oldName,
                                       const std::string &newName);

   // Parameter access methods
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual bool        GetBooleanParameter(const Integer id) const;
   virtual std::string GetRefObjectName(const UnsignedInt type) const;
   virtual bool        SetRefObjectName(const UnsignedInt type,
                                        const std::string &name);
    

protected:

   bool RunInternalSolver(Solver::SolverState state);
   bool RunExternalSolver(Solver::SolverState state);
   
   enum
   {
      OPTIMIZER_NAME = SolverBranchCommandParamCount,
      OPTIMIZER_CONVERGED,
      OptimizeParamCount
   };

   // save for possible later use
   static const std::string
          PARAMETER_TEXT[OptimizeParamCount - SolverBranchCommandParamCount];
   
   static const Gmat::ParameterType
          PARAMETER_TYPE[OptimizeParamCount - SolverBranchCommandParamCount];
   
   /// Flag indicating is the optimizer has converged
   bool                optimizerConverged;
   /// FLag indicating that this SolverControlSequence ran once already
   bool                optimizerRunOnce;
   /// Flag indicating optimizer in function initialized
   bool                optimizerInFunctionInitialized;
   /// Flag used to avoid multiple pen downs
   bool                penIsDown;
   
   StringArray         callbackResults;
   std::string         callbackData;
   RealArray           callbackRealData;
   
   bool                optimizerInDebugMode;

   Integer             minimizeCount;
};

#endif /*Optimize_hpp*/
