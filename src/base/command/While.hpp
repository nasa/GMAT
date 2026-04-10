//$Id$
//------------------------------------------------------------------------------
//                                While
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
// Author:  Wendy Shoan/GSFC/MAB
// Created: 2004/08/10
//
/**
* Definition for the While command class
 */
//------------------------------------------------------------------------------

#ifndef While_hpp
#define While_hpp

#include "gmatdefs.hpp"
#include "ConditionalBranch.hpp"
#include "Parameter.hpp"

/**
* Command that manages processing for entry to the While statement
 *
 * The While command manages the While statement.
 *
 */
class GMAT_API While : public ConditionalBranch
{
public:
   // default constructor
   While();
   // copy constructor
   While(const While &wc);
   // operator =
   While& operator=(const While &wc);
   // destructor
   virtual ~While(void);

   // Inherited methods that need some enhancement from the base class
   virtual bool         Append(GmatCommand *cmd);

   // Methods used to run the command
   virtual bool         Initialize();
   virtual bool         Execute();

   // inherited from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   virtual GmatBase*    Clone() const;
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName);

   DEFAULT_TO_NO_CLONES

protected:

      enum
   {
      NEST_LEVEL = ConditionalBranchParamCount,
      WhileParamCount
   };
   
   static const std::string PARAMETER_TEXT[WhileParamCount - ConditionalBranchParamCount];
   
   static const Gmat::ParameterType PARAMETER_TYPE[WhileParamCount - ConditionalBranchParamCount];
   
   /// Counter to track the depth of the While
   Integer                  nestLevel;
   /// Container for locally cloned Parameters
   std::vector<Parameter*> localParameters;

   /// Flag used to make sure empty loops only warn one time
   bool emptyLoopWarningPosted;
};
#endif  // While_hpp
