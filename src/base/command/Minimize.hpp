//$Id$
//------------------------------------------------------------------------------
//                                  Minimize
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
// Author: Daniel Hunter, Wendy Shoan (GSFC/MAB)
// Created: 2006.08.11
//
/**
 * Definition for the Minimize command class
 */
//------------------------------------------------------------------------------


#ifndef Minimize_hpp
#define Minimize_hpp
 

#include "SolverSequenceCommand.hpp"
#include "Solver.hpp"
#include "Variable.hpp"
#include "ElementWrapper.hpp"

/**
 * Command that manages processing for optimizer variables.
 */
class GMAT_API Minimize : public SolverSequenceCommand
{
public:
   Minimize();
   Minimize(const Minimize& m);
   Minimize&           operator=(const Minimize& m);
   virtual ~Minimize();

   // inherited from GmatBase
   virtual GmatBase*   Clone() const;

   virtual bool        RenameRefObject(const UnsignedInt type,
                                       const std::string &oldName,
                                       const std::string &newName);
   virtual const ObjectTypeArray&
                       GetRefObjectTypeArray();
   virtual const StringArray&
                       GetRefObjectNameArray(const UnsignedInt type);
   
   
   // Parameter accessors
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual bool        IsParameterReadOnly(const Integer id) const;
   virtual bool        IsParameterReadOnly(const std::string &label) const;

   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
                                           
   virtual bool        SetRefObject(GmatBase *obj, const UnsignedInt type,
                                    const std::string &name = "");
    
    // Inherited methods overridden from the base class
   virtual bool        InterpretAction();
   virtual const StringArray& 
                       GetWrapperObjectNameArray(bool completeSet = false);
   virtual bool        SetElementWrapper(ElementWrapper* toWrapper,
                                         const std::string &withName);
   virtual void        ClearWrappers();

   virtual bool        Initialize();
   virtual bool        Execute();
   virtual void        RunComplete();
   
   virtual const std::string&
                       GetGeneratingString(Gmat::WriteMode mode,
                                           const std::string &prefix,
                                           const std::string &useName);

   DEFAULT_TO_NO_CLONES
    
protected:
   /// The name of the spacecraft that gets maneuvered
   std::string         optimizerName;
   /// Name of the variable to be minimized
   std::string         objectiveName;
   // pointer to the Variable that is to be minimized
   //Variable            *objective;
   /// pointer to the wrapper containing the objective 
   ElementWrapper      *objective;
   /// value of the variable
   //Real                objectiveValue; // maybe I don't need this
   /// The optimizer instance used to manage the optimizer state machine
   Solver              *optimizer;
   /// Flag used to finalize the targeter data during execution
   bool                optimizerDataFinalized;
   /// is it a parameter?
   //bool                isMinimizeParm;
   /// Optimizer ID for the variable 
   Integer             objId;
   /// flag indicating whether or not the generating string has been interpreted
   bool                interpreted;

   
   // Parameter IDs
   enum 
   {
      OPTIMIZER_NAME = SolverSequenceCommandParamCount,
      OBJECTIVE_NAME,
      OBJECTIVE_VALUE,
      MinimizeParamCount
   };
   static const std::string
                       PARAMETER_TEXT[MinimizeParamCount - SolverSequenceCommandParamCount];
   static const Gmat::ParameterType
                       PARAMETER_TYPE[MinimizeParamCount - SolverSequenceCommandParamCount];

   //bool                InterpretParameter(const std::string text,
   //                                       std::string &paramType,
   //                                       std::string &paramObj,
   //                                       std::string &parmSystem);
   //bool                ConstructGoal(const char* str);

};


#endif  // Minimize_hpp

