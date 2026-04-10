//$Id$
//------------------------------------------------------------------------------
//                                  Target
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
// Author: Darrel J. Conway
// Created: 2004/01/22
//
/**
 * Definition for the Target command class
 */
//------------------------------------------------------------------------------


#ifndef Target_hpp
#define Target_hpp
 

#include "SolverBranchCommand.hpp"
#include "Solver.hpp"
#include "Spacecraft.hpp"


/**
 * Command that manages processing for entry to the targeter loop
 *
 * The Target command manages the targeter loop.  All targeters implement a 
 * state machine that evaluates the current state of the targeting process, and
 * provides data to the command sequence about the next step to be taken in the 
 * targeting process.
 */
class GMAT_API Target : public SolverBranchCommand
{
public:
   Target();
   virtual ~Target();
    
   Target(const Target& t);
   Target&             operator=(const Target& t);
    
   // Inherited methods that need some enhancement from the base class
   virtual bool        Append(GmatCommand *cmd);


   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   const std::string&   GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName);

   virtual bool RenameRefObject(const UnsignedInt type,
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
    
   // Methods used to run the command
   virtual bool        Initialize();
   virtual bool        Execute();
   virtual void        RunComplete();

protected:
   /// Flag indicating is the targeter has converged
   bool                targeterConverged;
   bool                targeterInFunctionInitialized;
   bool                targeterRunOnce;
   
   // Parameter IDs 
   /// ID for the burn object
   Integer             TargeterConvergedID;
   bool                targeterInDebugMode;
};


#endif  // Target_hpp

