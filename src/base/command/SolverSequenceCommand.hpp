//$Id$
//------------------------------------------------------------------------------
//                           SolverSequenceCommand
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28.
//
// Author: Darrel J. Conway
// Created: 2012/08/22
//
/**
 * Definition for the SolverSequenceCommand base class
 */
//------------------------------------------------------------------------------

#ifndef SOLVERSEQUENCECOMMAND_HPP_
#define SOLVERSEQUENCECOMMAND_HPP_

#include "GmatCommand.hpp"


/**
 * Base class for commands specific to Solver Control Sequences
 *
 * This code is a partial refactorization of the Vary, Achieve, Minimize,
 * and NonlinearConstraint commands to provide them with a common interface
 * for accessing the name of the Solver associated with each command.  Full
 * refactorization should be performed when schedule permits.
 */
class GMAT_API SolverSequenceCommand : public GmatCommand
{
public:
   SolverSequenceCommand(const std::string &cmdType);
   virtual ~SolverSequenceCommand();
   SolverSequenceCommand(const SolverSequenceCommand& ssc);
   SolverSequenceCommand& operator=(const SolverSequenceCommand& ssc);

   // for Ref Objects
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
//
//   virtual const ObjectTypeArray&
//                        GetRefObjectTypeArray();
//   virtual const StringArray&
//                        GetRefObjectNameArray(const UnsignedInt type);

   // Parameter accessors
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
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);

   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

protected:
   /// Name of the Solver used by the command
   std::string solverName;

   // Parameter IDs
   enum
   {
      SOLVER_NAME = GmatCommandParamCount,
      SolverSequenceCommandParamCount
   };

   static const std::string    PARAMETER_TEXT[SolverSequenceCommandParamCount -
                                              GmatCommandParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[SolverSequenceCommandParamCount -
                                              GmatCommandParamCount];
};

#endif /* SOLVERSEQUENCECOMMAND_HPP_ */
