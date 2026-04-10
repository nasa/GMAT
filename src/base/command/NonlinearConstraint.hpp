//$Id$
//------------------------------------------------------------------------------
//                                  NonlinearConstraint
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
// Author: Wendy Shoan (GSFC/MAB)
// Created: 2006.08.14
//
/**
 * Definition for the NonlinearConstraint command class
 */
//------------------------------------------------------------------------------


#ifndef NonlinearConstraint_hpp
#define NonlinearConstraint_hpp
 

#include "SolverSequenceCommand.hpp"
#include "Solver.hpp"
#include "Parameter.hpp"
#include "ElementWrapper.hpp"

/**
 * Command that manages processing for targeter goals.
 */
class GMAT_API NonlinearConstraint : public SolverSequenceCommand
{
public:
   NonlinearConstraint();
   NonlinearConstraint(const NonlinearConstraint& nlc);
   NonlinearConstraint&           operator=(const NonlinearConstraint& nlc);
   virtual ~NonlinearConstraint();

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
                                          const char *value);
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);

   // Override allowing real settings from the API for the Vary fields
   virtual void         SetField(const Integer id, const Real value);
   virtual void        SetNumber(const std::string &label, const Real value);

   virtual Real        GetNumber(const Integer id);
   virtual Real        GetNumber(const std::string &label);
                                           
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
   // Parameter IDs
   enum 
   {
      OPTIMIZER_NAME = SolverSequenceCommandParamCount,
      CONSTRAINT_ARG1,
      LHS_CONSTRAINT_VALUE,
      OPERATOR,
      CONSTRAINT_ARG2,
      RHS_CONSTRAINT_VALUE,
      TOLERANCE,
      NonlinearConstraintParamCount
   };
   static const std::string
                       PARAMETER_TEXT[NonlinearConstraintParamCount - SolverSequenceCommandParamCount];
   static const Gmat::ParameterType
                       PARAMETER_TYPE[NonlinearConstraintParamCount - SolverSequenceCommandParamCount];

   enum Operator
   {
      LESS_THAN_OR_EQUAL = 0,
      GREATER_THAN_OR_EQUAL,
      EQUAL
   };
   
   static const std::string OP_STRINGS[3];

   /// The name of the spacecraft that gets maneuvered
   std::string         optimizerName;
   /// The optimizer instance used to manage the optimizer state machine
   Solver              *optimizer;
   /// Name of the variable to be constrained
   std::string         arg1Name;
   // pointer to the Variable that is to be minimized
   //Parameter           *constraint;
   ElementWrapper      *arg1;
   /// most recent value of the variable
   Real                constraintValue; // do I still need this?
   /// name of the parameter part of the right-hand-side
   std::string         arg2Name;
   //Parameter           *nlcParm;
   ElementWrapper      *arg2;

   /// String of value array name  // I don't think I need any of this stuff
   //std::string         nlcArrName;
   /// constraint array row index variable name
   //std::string         nlcArrRowStr;
   /// constraint array column index variable name
   //std::string         nlcArrColStr;
   /// constraint array row index
   //Integer             nlcArrRow;
   /// constraint array column index
   //Integer             nlcArrCol;
   //Parameter           *nlcArrRowParm;
   //Parameter           *nlcArrColParm;

   /// flag indicating whether or not the constraint is an inequality
   /// constraint
   bool                isInequality;
   /// string to send into the optimizer, based on isInequality
   std::string         isIneqString;
   /// the desired value (right hand side of the constraint equation)
   Real                desiredValue;
   /// indicates what type of operator was passed in for the generating
   /// string
   Operator            op;
   /// tolerance (in simulation space) for the constraint
   std::string         toleranceName;
   /// Wrapper for the tolerance setting
   ElementWrapper      *tolerance;
   /// Evaluated tolerance
   Real                toleranceValue;
   /// Flag used to warn about invalid tolerance exactly once per run
   bool                toleranceWarningIssued;
   /// Diff btwn tolerance and achieved value; positive numbers exceed tolerance
   Real                deltaTolerance;
   /// Flag used to finalize the targeter data during execution
   bool                optimizerDataFinalized;
   /// ID for this constraint (returned from the optimizer)
   Integer             constraintId;
   /// is the right hand side a parameter?
   //bool                isNlcParm;
   /// is the right hand side an array?
   //bool                isNlcArray;
   /// Pointer to the object that owns the goal
   //GmatBase            *constraintObject;
   /// Object ID for the parameter
   //Integer             parmId;
   /// flag indicating whether or not the generating string has been interpreted
   bool                interpreted;
   std::string         panelDescriptor;        // Shows "(<=) Sat.SMA" or whatever 
   Real                panelDesired;
   Real                panelAchieved;
   
   //bool                InterpretParameter(const std::string text,
   //                                       std::string &paramType,
   //                                       std::string &paramObj,
   //                                       std::string &parmSystem);
                                          

   //bool                ConstructConstraint(const char* str);

};


#endif  // NonlinearConstraint_hpp

